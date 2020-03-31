/*
 *      Routines to handle MIB operation
 *
 *      Authors: David Hsu	<davidhsu@realtek.com.tw>
 *
 *      $Id: apmib.c,v 1.33.2.5 2011/04/27 03:20:47 keith_huang Exp $
 *
 */

// include file
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "apmib.h"
#include "mibtbl.h"

/* Shared Memory */
#if CONFIG_APMIB_SHARED_MEMORY == 1
#include <linux/types.h>
#include <linux/ipc.h>
#include <linux/sem.h>
#include <linux/shm.h>
#endif

// MAC address filtering
typedef struct _filter {
	struct _filter *prev, *next;
	char val[1];
} FILTER_T, *FILTER_Tp;

typedef struct _linkChain {
	FILTER_Tp pUsedList, pFreeList;
	int size, num, usedNum, compareLen, realsize;
	char *buf;
} LINKCHAIN_T, *LINKCHAIN_Tp;


// macro to remove a link list entry
#define REMOVE_LINK_LIST(entry) { \
	if ( entry ) { \
		if ( entry->prev ) \
			entry->prev->next = entry->next; \
		if ( entry->next ) \
			entry->next->prev = entry->prev; \
	} \
}

// macro to add a link list entry
#define ADD_LINK_LIST(list, entry) { \
	if ( list == NULL ) { \
		list = entry; \
		list->prev = list->next = entry; \
	} \
	else { \
		entry->prev = list; \
		entry->next = list->next; \
		list->next = entry; \
		entry->next->prev = entry; \
	} \
}

// local routine declaration
static int flash_read(char *buf, int offset, int len);
static int flash_write(char *buf, int offset, int len);
static int init_linkchain(LINKCHAIN_Tp pLinkChain, int size, int num);
static int add_linkchain(LINKCHAIN_Tp pLinkChain, char *val);
static int delete_linkchain(LINKCHAIN_Tp pLinkChain, char *val);
static void delete_all_linkchain(LINKCHAIN_Tp pLinkChain);
static int get_linkchain(LINKCHAIN_Tp pLinkChain, char *val, int index);

// local & global variable declaration
APMIB_Tp pMib=NULL;
APMIB_Tp pMibDef;
PARAM_HEADER_T hsHeader, dsHeader, csHeader;
HW_SETTING_Tp pHwSetting;
int wlan_idx=0;	// interface index 
int vwlan_idx=0;	// initially set interface index to root

static LINKCHAIN_T wlanMacChain[NUM_WLAN_INTERFACE][NUM_VWLAN_INTERFACE+1];
static LINKCHAIN_T wdsChain[NUM_WLAN_INTERFACE][NUM_VWLAN_INTERFACE+1];
static LINKCHAIN_T scheduleRuleChain;

#if defined(CONFIG_RTK_MESH) && defined(_MESH_ACL_ENABLE_)
static LINKCHAIN_T meshAclChain;
#endif

#ifdef HOME_GATEWAY
static LINKCHAIN_T portFwChain, ipFilterChain, portFilterChain, macFilterChain, triggerPortChain;
static LINKCHAIN_T urlFilterChain;
#ifdef ROUTE_SUPPORT
static LINKCHAIN_T staticRouteChain;
#endif

#if defined(GW_QOS_ENGINE) || defined(QOS_BY_BANDWIDTH)
static LINKCHAIN_T qosChain;
#endif
#endif
static LINKCHAIN_T dhcpRsvdIpChain;

#if defined(VLAN_CONFIG_SUPPORTED)
static LINKCHAIN_T vlanConfigChain;
#endif


#if 0 //michael
#ifdef WEBS
extern int save_cs_to_file();
#endif
#endif

#ifdef HOME_GATEWAY
#ifdef VPN_SUPPORT
static LINKCHAIN_T  ipsecTunnelChain;
#endif
#endif

#ifdef TLS_CLIENT
static LINKCHAIN_T  certRootChain;
static LINKCHAIN_T  certUserChain;
#endif

#if CONFIG_APMIB_SHARED_MEMORY == 1
char *shm_name[] = {"/var/HWCONF",	/* HWCONF_SHM_KEY */
				 					 "/var/DSCONF",	/* DSCONF_SHM_KEY */
									 "/var/CSCONF",	/* CSCONF_SHM_KEY */
									};
static int apmib_sem_id = -1;

static int apmib_sem_create(void)
{
	int sem_id;
	
	/* Generate a System V IPC key */ 
	key_t key;
	key = ftok("/var", 0xD4);
	if (key == -1) {
		printf("APMIB Semaphore ftok() failed !! [%s]\n", strerror(errno));
		return -1;
	}
	
	/* Get a semaphore set with 1 semaphore */
	sem_id = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666);
	if (sem_id == -1) {
		if (errno == EEXIST) {
			sem_id = semget(key, 1, 0666);
			if (sem_id == -1) {
				printf("APMIB Semaphore semget() failed !! [%s]\n", strerror(errno));
				return -1;
			}
		} else {
			printf("APMIB Semaphore semget() failed !! [%s]\n", strerror(errno));
			return -1;
		}
	} else {
        /* Initialize semaphore #0 to 1 */
		union semun arg;
        arg.val = 1;
        if (semctl(sem_id, 0, SETVAL, arg) == -1) {
            printf("APMIB Semaphore semctl() failed !! [%s]\n", strerror(errno));
            return -1;
        }
    }

	apmib_sem_id = sem_id;
	return 0;
}

int apmib_sem_lock(void)
{
	struct sembuf sop[1];
	
	if (apmib_sem_id == -1) {
		if (apmib_sem_create() == -1) {
		    printf("apmib_sem_create fail\n");
			return -1;
		}
	}
	
	sop[0].sem_num = 0;
	sop[0].sem_op = -1;
	sop[0].sem_flg = SEM_UNDO;
try_again:
	if (semop(apmib_sem_id, sop, 1) == -1) {
		if (errno == EINTR) {
			printf("APMIB Semaphore Lock semop() failed !! [%s]\n", strerror(errno));
			goto try_again;
		}
		printf("APMIB Semaphore Lock semop() failed !! [%s]\n", strerror(errno));
		return -1;
	}
	return 0;		
}

int apmib_sem_unlock(void)
{
	struct sembuf sop[1];
		
	sop[0].sem_num = 0;
	sop[0].sem_op = 1;
	sop[0].sem_flg = SEM_UNDO;
	if (semop(apmib_sem_id, sop, 1) == -1) {
		printf("APMIB Semaphore Unlock semop() failed !! [%s]\n", strerror(errno));
		return -1;
	}	
	return 0;	
}
	
int apmib_shm_free(void *shm_memory, int shm_key)
{
	return (shmdt(shm_memory));
	return 0;
}

static char *apmib_shm_calloc(size_t nmemb, size_t size, int shm_key, int *created)
{
	int shm_id, shm_size;
	char *shm_memory; //, *shm_name;	
	
	*created = 0;
	shm_size = (nmemb * size);

	/* Generate a System V IPC key */ 
	key_t key;
	key = ftok("/var", (0x3C + shm_key));
	if (key == -1) {
		printf("%s ftok() failed !! [%s]\n", shm_name[shm_key], strerror(errno));
		return NULL;
	}
		 
	/* Allocate a shared memory segment */
	shm_id = shmget(key, shm_size, IPC_CREAT | IPC_EXCL | 0666);
	if (shm_id == -1) {
		if (errno == EEXIST) {
			*created = 1;
			shm_id = shmget(key, shm_size, 0666);
			if (shm_id == -1) {
				printf("%s shmget() failed !! [%s]\n", shm_name[shm_key], strerror(errno));
				return NULL;
			}
		} else {
			printf("%s shmget() failed !! [%s]\n", shm_name[shm_key], strerror(errno));
			return NULL;
		}
	}
		 
	/* Attach the shared memory segment */
	shm_memory = (char *)shmat(shm_id, NULL, 0);
	if ((int)shm_memory == -1) {
		printf("%s shmat() failed [%s]\n", shm_name[shm_key], strerror(errno));
		return NULL;
	}
		
	if (*created) {
		return shm_memory;
	}

	memset(shm_memory, 0, shm_size);
	return shm_memory;
}
#endif

////////////////////////////////////////////////////////////////////////////////
char *apmib_hwconf(void)
{
	int ver;
	char *buff;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	int created;
#endif

	// Read hw setting
	if ( flash_read((char *)&hsHeader, HW_SETTING_OFFSET, sizeof(hsHeader))==0 ) {
//		printf("Read hw setting header failed!\n");
		return NULL;
	}

	if ( sscanf(&hsHeader.signature[TAG_LEN], "%02d", &ver) != 1)
		ver = -1;

	if ( memcmp(hsHeader.signature, HW_SETTING_HEADER_TAG, TAG_LEN) || // invalid signatur
		(ver != HW_SETTING_VER) || // version not equal to current
		(hsHeader.len < (sizeof(HW_SETTING_T)+1)) ) { // length is less than current
//		printf("Invalid hw setting signature or version number [sig=%c%c, ver=%d, len=%d]!\n", hsHeader.signature[0],
//			hsHeader.signature[1], ver, hsHeader.len);
		return NULL;
	}
//	if (ver > HW_SETTING_VER)
//		printf("HW setting version is greater than current [f:%d, c:%d]!\n", ver, HW_SETTING_VER);

#if CONFIG_APMIB_SHARED_MEMORY == 1
	buff = apmib_shm_calloc(1, hsHeader.len, HWCONF_SHM_KEY, &created);
#else
	buff = calloc(1, hsHeader.len);
#endif
	if ( buff == 0 ) {
//		printf("Allocate buffer failed!\n");
		return NULL;
	}

#if CONFIG_APMIB_SHARED_MEMORY == 1
    if (created) {
        //printf("No need to read hw setting!\n");
        return buff;
    }
#endif

	if ( flash_read(buff, HW_SETTING_OFFSET+sizeof(hsHeader), hsHeader.len)==0 ) {
//		printf("Read hw setting failed!\n");
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_shm_free(buff, HWCONF_SHM_KEY);
#else
		free(buff);
#endif		
		return NULL;
	}
	if ( !CHECKSUM_OK(buff, hsHeader.len) ) {
//		printf("Invalid checksum of hw setting!\n");
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_shm_free(buff, HWCONF_SHM_KEY);
#else
		free(buff);
#endif		
		return NULL;
	}
	return buff;
}
     
void apmib_default_setting(APMIB_Tp pMib)
{	
	int i;
	int idx;
//#ifdef VLAN_CONFIG_SUPPORTED
	int vlan_entry=0;
//#endif
#if 0 //for expert mode
	char ssidDef[]="RTK 11n AP";
#else
	char ssidDef[]="802.11bgn-SSID";
#endif	
	
		// give a initial value for testing purpose
	strcpy(pMib->deviceName, "Realtek Wireless AP");
	for (idx=0; idx<NUM_WLAN_INTERFACE; idx++) {
		for (i=0; i<(NUM_VWLAN_INTERFACE+1); i++) {		
#ifdef MBSSID
			if (i > 0)
				sprintf(pMib->wlan[idx][i].ssid, "%s-VAP%d", ssidDef,i-1);
			else
#endif
				sprintf(pMib->wlan[idx][i].ssid, "%s", ssidDef);
			pMib->wlan[idx][i].channel = 6;
			pMib->wlan[idx][i].wep = WEP_DISABLED;
			pMib->wlan[idx][i].beaconInterval = 100;
			
			pMib->wlan[idx][i].rateAdaptiveEnabled = 1;
			
			pMib->wlan[idx][i].basicRates = TX_RATE_1M|TX_RATE_2M|TX_RATE_5M|TX_RATE_11M;
			pMib->wlan[idx][i].supportedRates = TX_RATE_1M|TX_RATE_2M|TX_RATE_5M |TX_RATE_11M|
				TX_RATE_6M|TX_RATE_9M|TX_RATE_12M|TX_RATE_18M|TX_RATE_24M|
					TX_RATE_36M|TX_RATE_48M|TX_RATE_54M;

			pMib->wlan[idx][i].preambleType = LONG_PREAMBLE;
			pMib->wlan[idx][i].rtsThreshold = 2347;
			pMib->wlan[idx][i].fragThreshold = 2346;
			pMib->wlan[idx][i].authType = AUTH_BOTH;
			pMib->wlan[idx][i].inactivityTime = 30000; // 300 sec
			pMib->wlan[idx][i].dtimPeriod = 3;

			pMib->wlan[idx][i].wpaGroupRekeyTime = 86400;
			pMib->wlan[idx][i].wpaAuth = (unsigned char)WPA_AUTH_PSK;
			pMib->wlan[idx][i].wpaCipher = (unsigned char)WPA_CIPHER_AES; //Keith
			pMib->wlan[idx][i].wpa2Cipher = (unsigned char)WPA_CIPHER_AES; //Keith
			pMib->wlan[idx][i].rsPort = 1812;
			pMib->wlan[idx][i].accountRsPort = 1813;
			pMib->wlan[idx][i].accountRsUpdateDelay = 60;
			pMib->wlan[idx][i].rsMaxRetry = 3;
			pMib->wlan[idx][i].rsIntervalTime = 5;
			pMib->wlan[idx][i].accountRsMaxRetry = 3;
			pMib->wlan[idx][i].accountRsIntervalTime = 5;
			pMib->wlan[idx][i].wlanBand = BAND_11BG | BAND_11N;
			pMib->wlan[idx][i].wmmEnabled = 1;
			if (i > 0)
				pMib->wlan[idx][i].wlanDisabled = 1;		
#if defined(CONFIG_RTL8196C_CLIENT_ONLY)
			pMib->wlan[idx][i].iappDisabled = 1;
			pMib->wlan[idx][i].wlanMode=1;
#endif
#ifdef WLAN_EASY_CONFIG
			pMib->wlan[idx][i].acfMode = MODE_BUTTON;
			pMib->wlan[idx][i].acfAlgReq = ACF_ALGORITHM_WPA2_AES;
			pMib->wlan[idx][i].acfAlgSupp = ACF_ALGORITHM_WPA_TKIP  | ACF_ALGORITHM_WPA2_AES;
			strcpy(pMib->wlan[idx][i].acfScanSSID, "REALTEK_EASY_CONFIG");
#endif

#ifdef WIFI_SIMPLE_CONFIG
			pMib->wlan[idx][i].wscMethod = 3;
			//strcpy(pMib->wlan[idx].wscPin, "12345670"); //move to hw setting
			pMib->wlan[idx][i].wscAuth = WSC_AUTH_OPEN; //open
			pMib->wlan[idx][i].wscEnc = WSC_ENCRYPT_NONE; //open
			pMib->wlan[idx][i].wscUpnpEnabled = 1;
			pMib->wlan[idx][i].wscRegistrarEnabled = 1;
			strcpy(pMib->wlan[idx][i].wscSsid, pMib->wlan[idx][i].ssid ); //must be the same as pMib->wlan[idx].ssid
#if defined(CONFIG_RTL8196C_CLIENT_ONLY)			
			pMib->wlan[idx][i].wscDisable = 0;
#else
			pMib->wlan[idx][i].wscDisable = 1;
#endif
#endif
			// for 11n
			pMib->wlan[idx][i].aggregation = 3;
			pMib->wlan[idx][i].shortgiEnabled = 1;
			pMib->wlan[idx][i].STBCEnabled = 0;
			pMib->wlan[idx][i].CoexistEnabled= 0;
		}
	}
#if defined(CONFIG_RTL8196B)
	pMib->wifiSpecific = 2; 
#endif	
	pMib->ipAddr[0] = 192;
	pMib->ipAddr[1] = 168;
	pMib->ipAddr[2] = 1;
	pMib->ipAddr[3] = 254;

	pMib->subnetMask[0] = 255;
	pMib->subnetMask[1] = 255;
	pMib->subnetMask[2] = 255;
	pMib->subnetMask[3] = 0;

	pMib->dhcpClientStart[0] = 192;
	pMib->dhcpClientStart[1] = 168;
	pMib->dhcpClientStart[2] = 1;
	pMib->dhcpClientStart[3] = 100;

	pMib->dhcpClientEnd[0] = 192;
	pMib->dhcpClientEnd[1] = 168;
	pMib->dhcpClientEnd[2] = 1;
	pMib->dhcpClientEnd[3] = 200;
	
#if defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT)	
	pMib->dhcp = DHCP_AUTO;
#else	
	pMib->dhcp = DHCP_SERVER;
#endif	

	strcpy(pMib->superName, "super");
	strcpy(pMib->superPassword, "super");

#ifdef HOME_GATEWAY
	pMib->wanIpAddr[0] = 192;
	pMib->wanIpAddr[1] = 168;
	pMib->wanIpAddr[2] = 1;
	pMib->wanIpAddr[3] = 1;

	pMib->wanSubnetMask[0] = 255;
	pMib->wanSubnetMask[1] = 255;
	pMib->wanSubnetMask[2] = 0;
	pMib->wanSubnetMask[3] = 0;

	pMib->wanDefaultGateway[0] = 192;
	pMib->wanDefaultGateway[1] = 168;
	pMib->wanDefaultGateway[2] = 1;
	pMib->wanDefaultGateway[3] = 254;

	pMib->wanDhcp = DHCP_CLIENT;
	pMib->dnsMode = DNS_AUTO;
	pMib->pppIdleTime = 300;
	pMib->pptpIdleTime = 300;
	pMib->l2tpIdleTime = 300;
	pMib->pppMtuSize = 1492;
	pMib->pptpMtuSize = 1400;
	pMib->l2tpMtuSize = 1400; /* keith: add l2tp support. 20080515 */
	pMib->L2tpwanIPMode = 1; /* keith: add l2tp support. 20080515 */
	pMib->fixedIpMtuSize = 1500;
	pMib->dhcpMtuSize = 1492;
#endif

// added by rock /////////////////////////////////////////
#ifdef VOIP_SUPPORT
	flash_voip_default(&pMib->voipCfgParam);
#endif

	// SNMP, Forrest added, 2007.10.25.
#ifdef CONFIG_SNMP
	pMib->snmpEnabled = 1;
	sprintf(pMib->snmpName, "%s", "Realtek");
	sprintf(pMib->snmpLocation, "%s", "AP");
	sprintf(pMib->snmpContact, "%s", "Router");
	sprintf(pMib->snmpRWCommunity, "%s", "private");
	sprintf(pMib->snmpROCommunity, "%s", "public");
	pMib->snmpTrapReceiver1[0] = 0;
	pMib->snmpTrapReceiver1[1] = 0;
	pMib->snmpTrapReceiver1[2] = 0;
	pMib->snmpTrapReceiver1[3] = 0;
	pMib->snmpTrapReceiver2[0] = 0;
	pMib->snmpTrapReceiver2[1] = 0;
	pMib->snmpTrapReceiver2[2] = 0;
	pMib->snmpTrapReceiver2[3] = 0;
	pMib->snmpTrapReceiver3[0] = 0;
	pMib->snmpTrapReceiver3[1] = 0;
	pMib->snmpTrapReceiver3[2] = 0;
	pMib->snmpTrapReceiver3[3] = 0;
#endif

#ifdef CONFIG_RTK_MESH
	pMib->meshEnabled = 0;
#ifdef CONFIG_NEW_MESH_UI
	pMib->meshRootEnabled = 0;	
#else
	pMib->meshRootEnabled = 0;		// if meshEnabled default value "1", Here "1" also
#endif
	pMib->meshMaxNumOfNeighbors = 32;
	strcpy(pMib->meshID, "RTK-mesh");
	
#ifdef 	_11s_TEST_MODE_	
	pMib->meshTestParam6 = 43627 ;
	pMib->meshTestParam7 = 48636 ;
	pMib->meshTestParam8 = 42090 ;
	pMib->meshTestParam9 = 43627 ;
	pMib->meshTestParama = 42606 ;
	pMib->meshTestParamb = 47016 ;
	pMib->meshTestParamc = 57710 ;
	pMib->meshTestParamd = 46323 ;
	pMib->meshTestParame = 47016 ;
	pMib->meshTestParamf = 47811 ;
#endif

#endif // CONFIG_RTK_MESH
#if defined(HOME_GATEWAY) && defined(ROUTE_SUPPORT)
	pMib->natEnabled = 1;
#endif

#ifdef SNMP_SUPPORT
	strcpy(pMib->snmpROcommunity,"public");
	strcpy(pMib->snmpRWcommunity,"private");
#endif
#ifdef HOME_GATEWAY //#if defined(VLAN_CONFIG_SUPPORTED)
for(vlan_entry=0;vlan_entry<MAX_IFACE_VLAN_CONFIG;vlan_entry++){
	pMib->VlanConfigArray[vlan_entry].enabled=0;
	pMib->VlanConfigArray[vlan_entry].vlanId = 1;
	pMib->VlanConfigArray[vlan_entry].cfi = 0;
#ifdef CONFIG_RTK_VLAN_ROUTETYPE
	pMib->VlanConfigArray[vlan_entry].forwarding = 2;
#endif
}	
pMib->VlanConfigEnabled=0;
pMib->VlanConfigNum=MAX_IFACE_VLAN_CONFIG;
sprintf(pMib->VlanConfigArray[0].netIface, "%s", "eth0");
sprintf(pMib->VlanConfigArray[1].netIface, "%s", "eth2");
sprintf(pMib->VlanConfigArray[2].netIface, "%s", "eth3");
sprintf(pMib->VlanConfigArray[3].netIface, "%s", "eth4");
sprintf(pMib->VlanConfigArray[4].netIface, "%s", "wlan0");
sprintf(pMib->VlanConfigArray[5].netIface, "%s", "wlan0-va0");
sprintf(pMib->VlanConfigArray[6].netIface, "%s", "wlan0-va1");
sprintf(pMib->VlanConfigArray[7].netIface, "%s", "wlan0-va2");
sprintf(pMib->VlanConfigArray[8].netIface, "%s", "wlan0-va3");
#ifdef CONFIG_RTK_VLAN_ROUTETYPE
sprintf(pMib->VlanConfigArray[9].netIface, "%s", "eth1");
sprintf(pMib->VlanConfigArray[10].netIface, "%s", "eth5");
#else
sprintf(pMib->VlanConfigArray[9].netIface, "%s", "eth1");
#endif

pMib->VlanConfigArray[0].vlanId = 3022;
pMib->VlanConfigArray[0].priority = 7;
pMib->VlanConfigArray[1].vlanId = 3030;
pMib->VlanConfigArray[1].priority = 0;
pMib->VlanConfigArray[2].vlanId = 500;
pMib->VlanConfigArray[2].priority = 3;
#ifdef CONFIG_RTK_VLAN_ROUTETYPE
pMib->VlanConfigArray[9].forwarding = 1;
#endif	//CONFIG_RTK_VLAN_ROUTETYPE
#endif

#if defined(GW_QOS_ENGINE) || defined(QOS_BY_BANDWIDTH)
pMib->qosAutoUplinkSpeed = 1;
pMib->qosManualUplinkSpeed = 1000;
pMib->qosAutoDownLinkSpeed = 1;
pMib->qosManualDownLinkSpeed = 1000;

#endif

sprintf(pMib->domainName, "%s", "Realtek");

#ifdef HOME_GATEWAY
sprintf(pMib->hostName, "%s", "Realtek");
#endif

#if 0 //for expert mode
pMib->scrlogEnabled = 3; //system all
pMib->ntpEnabled = 1; //enable ntp
sprintf(pMib->ntpTimeZone, "%s", "-8\ 4"); //taipei
pMib->ntpServerIp1[0] = 192;
pMib->ntpServerIp1[1] = 5;
pMib->ntpServerIp1[2] = 41;
pMib->ntpServerIp1[3] = 41;
#endif



}

////////////////////////////////////////////////////////////////////////////////
char *apmib_dsconf(void)
{
	int ver;
	char *buff;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	int created;
#endif
#if defined(MOVE_OUT_DEFAULT_SETTING_FROM_FLASH)
	sprintf(dsHeader.signature, "%s%02d", DEFAULT_SETTING_HEADER_TAG, DEFAULT_SETTING_VER);
	dsHeader.len = sizeof(APMIB_T)+1;
	
#endif

#if !defined(MOVE_OUT_DEFAULT_SETTING_FROM_FLASH)
	// Read default s/w mib
	if ( flash_read((char *)&dsHeader, DEFAULT_SETTING_OFFSET, sizeof(dsHeader))==0 ) {
//		printf("Read default setting header failed!\n");
		return NULL;
	}

	if ( sscanf(&dsHeader.signature[TAG_LEN], "%02d", &ver) != 1)
		ver = -1;

//fprintf(stderr,"\r\n (sizeof(APMIB_T)=%u ",(sizeof(APMIB_T)));
	if ( memcmp(dsHeader.signature, DEFAULT_SETTING_HEADER_TAG, TAG_LEN) || // invalid signatur
		(ver != DEFAULT_SETTING_VER) || // version not equal to current
		(dsHeader.len < (sizeof(APMIB_T)+1)) ) { // length is less than current
//		printf("Invalid default setting signature or version number [sig=%c%c, ver=%d, len=%d]!\n",
//			dsHeader.signature[0], dsHeader.signature[1], ver, dsHeader.len);
		return NULL;
	}
//	if (ver > DEFAULT_SETTING_VER)
//		printf("Default setting version is greater than current [f:%d, c:%d]!\n", ver, DEFAULT_SETTING_VER);
#endif //!defined(MOVE_OUT_DEFAULT_SETTING_FROM_FLASH)

#if CONFIG_APMIB_SHARED_MEMORY == 1
	buff = apmib_shm_calloc(1, dsHeader.len, DSCONF_SHM_KEY, &created);
#else
	buff = calloc(1, dsHeader.len);
#endif	
	if ( buff == 0 ) {
		printf("Allocate buffer failed!\n");
		return NULL;
	}

#if CONFIG_APMIB_SHARED_MEMORY == 1
    if (created) {
        //printf("No need to read default setting!\n");
        return buff;
    }
#endif

#if defined(MOVE_OUT_DEFAULT_SETTING_FROM_FLASH)
	/* read default setting to buff*/
	apmib_default_setting((APMIB_Tp)buff);
	buff[dsHeader.len-1] = CHECKSUM(buff, dsHeader.len-1);

	if ( !CHECKSUM_OK(buff, dsHeader.len) ) {
		printf("Invalid checksum of current setting!\n");
	}	

#endif

#if !defined(MOVE_OUT_DEFAULT_SETTING_FROM_FLASH)

	if ( flash_read(buff, DEFAULT_SETTING_OFFSET+sizeof(dsHeader), dsHeader.len)==0 ) {
//		printf("Read default setting failed!\n");
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_shm_free(buff, DSCONF_SHM_KEY);
#else
		free(buff);
#endif		
		return NULL;
	}

	if ( !CHECKSUM_OK(buff, dsHeader.len) ) {
//		printf("Invalid checksum of current setting!\n");
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_shm_free(buff, DSCONF_SHM_KEY);
#else
		free(buff);
#endif		
		return NULL;
	}
#endif	//!defined(MOVE_OUT_DEFAULT_SETTING_FROM_FLASH)

	return buff;
}

////////////////////////////////////////////////////////////////////////////////
char *apmib_csconf(void)
{
	int ver;
	char *buff;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	int created;
#endif

	// Read current s/w mib
	if ( flash_read((char *)&csHeader, CURRENT_SETTING_OFFSET, sizeof(csHeader))==0 ) {
//		printf("Read current setting header failed!\n");
		return NULL;
	}

	if ( sscanf(&csHeader.signature[TAG_LEN], "%02d", &ver) != 1)
		ver = -1;

	if ( memcmp(csHeader.signature, CURRENT_SETTING_HEADER_TAG, TAG_LEN) || // invalid signatur
		(ver != CURRENT_SETTING_VER) || // version not equal to current
			(csHeader.len < (sizeof(APMIB_T)+1)) ) { // length is less than current
//		printf("Invalid current setting signature or version number [sig=%c%c, ver=%d, len=%d]!\n",
//			csHeader.signature[0], csHeader.signature[1], ver, csHeader.len);
		return NULL;
	}

//	if (ver > CURRENT_SETTING_VER)
//		printf("Current setting version is greater than current [f:%d, c:%d]!\n", ver, CURRENT_SETTING_VER);

#if CONFIG_APMIB_SHARED_MEMORY == 1
	buff = apmib_shm_calloc(1, csHeader.len, CSCONF_SHM_KEY, &created);
#else
	buff = calloc(1, csHeader.len);
#endif	
	if ( buff == 0 ) {
//		printf("Allocate buffer failed!\n");
		return NULL;
	}

#if CONFIG_APMIB_SHARED_MEMORY == 1
    if (created) {
        //printf("No need to read current setting!\n");
        return buff;
    }
#endif

	if ( flash_read(buff, CURRENT_SETTING_OFFSET+sizeof(csHeader), csHeader.len)==0 ) {
//		printf("Read current setting failed!\n");
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_shm_free(buff, CSCONF_SHM_KEY);
#else
		free(buff);
#endif		
		return NULL;
	}

	if ( !CHECKSUM_OK(buff, csHeader.len) ) {
//		printf("Invalid checksum of current setting!\n");
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_shm_free(buff, CSCONF_SHM_KEY);
#else	
		free(buff);
#endif
		return NULL;
	}

	return buff;
}
////////////////////////////////////////////////////////////////////////////
int apmib_init_HW(void)
{
	char *buff;

	if ((buff=apmib_hwconf()) == NULL)
		return 0;
	pHwSetting = (HW_SETTING_Tp)buff;
	return 1;
}
////////////////////////////////////////////////////////////////////////////////
int apmib_init(void)
{
	int i, j, k;
	char *buff;

#if CONFIG_APMIB_SHARED_MEMORY == 1	
    apmib_sem_lock();
#endif

	if ( pMib != NULL )	// has been initialized
#if CONFIG_APMIB_SHARED_MEMORY == 1
		goto linkchain;
#else
		return 1;
#endif

	if ((buff=apmib_hwconf()) == NULL) {
#if CONFIG_APMIB_SHARED_MEMORY == 1	
        apmib_sem_unlock();
#endif
		return 0;
	}
	pHwSetting = (HW_SETTING_Tp)buff;

	if ((buff=apmib_dsconf()) == NULL) {
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_shm_free(pHwSetting, HWCONF_SHM_KEY);
		apmib_sem_unlock();
#else
		free(pHwSetting);
#endif
		return 0;
	}
	pMibDef = (APMIB_Tp)buff;
	
	if ((buff=apmib_csconf()) == NULL) {
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_shm_free(pHwSetting, HWCONF_SHM_KEY);
		apmib_shm_free(pMibDef, DSCONF_SHM_KEY);
		apmib_sem_unlock();
#else
		free(pHwSetting);
		free(pMibDef);
#endif
		return 0;
	}
	pMib = (APMIB_Tp)buff;

#if CONFIG_APMIB_SHARED_MEMORY == 1
linkchain:
#endif
#if !defined(CONFIG_RTL8196C_CLIENT_ONLY)
	for (j=0; j<NUM_WLAN_INTERFACE; j++)
		for (k=0; k<(NUM_VWLAN_INTERFACE+1); k++) // wlan[j][0] is for root
	{

		// initialize MAC access control list
		if ( !init_linkchain(&wlanMacChain[j][k], sizeof(MACFILTER_T), MAX_WLAN_AC_NUM)) {
#if CONFIG_APMIB_SHARED_MEMORY == 1
			apmib_shm_free(pMib, CSCONF_SHM_KEY);
			apmib_shm_free(pMibDef, DSCONF_SHM_KEY);
			apmib_shm_free(pHwSetting, HWCONF_SHM_KEY);
			apmib_sem_unlock();
#else
			free(pMib);
			free(pMibDef);
			free(pHwSetting);
#endif			
			return 0;
		}
		for (i=0; i<pMib->wlan[j][k].acNum; i++) {
			if ( !add_linkchain(&wlanMacChain[j][k], (char *)&pMib->wlan[j][k].acAddrArray[i]) ) {
#if CONFIG_APMIB_SHARED_MEMORY == 1
				apmib_shm_free(pMib, CSCONF_SHM_KEY);
				apmib_shm_free(pMibDef, DSCONF_SHM_KEY);
				apmib_shm_free(pHwSetting, HWCONF_SHM_KEY);
				apmib_sem_unlock();
#else
				free(pMib);
				free(pMibDef);
				free(pHwSetting);
#endif
				return 0;
			}
		}
		wlanMacChain[j][k].compareLen = sizeof(MACFILTER_T) - COMMENT_LEN;

		// initialize WDS list
		if ( !init_linkchain(&wdsChain[j][k], sizeof(WDS_T), MAX_WDS_NUM)) {
#if CONFIG_APMIB_SHARED_MEMORY == 1
			apmib_shm_free(pMib, CSCONF_SHM_KEY);
			apmib_shm_free(pMibDef, DSCONF_SHM_KEY);
			apmib_shm_free(pHwSetting, HWCONF_SHM_KEY);
			apmib_sem_unlock();
#else
			free(pMib);
			free(pMibDef);
			free(pHwSetting);
#endif
			return 0;
		}
		for (i=0; i<pMib->wlan[j][k].wdsNum; i++) {
			if ( !add_linkchain(&wdsChain[j][k], (char *)&pMib->wlan[j][k].wdsArray[i]) ) {
#if CONFIG_APMIB_SHARED_MEMORY == 1
				apmib_shm_free(pMib, CSCONF_SHM_KEY);
				apmib_shm_free(pMibDef, DSCONF_SHM_KEY);
				apmib_shm_free(pHwSetting, HWCONF_SHM_KEY);
				apmib_sem_unlock();
#else
				free(pMib);
				free(pMibDef);
				free(pHwSetting);
#endif
				return 0;
			}
		}
		wdsChain[j][k].compareLen = sizeof(WDS_T) - COMMENT_LEN;
	}
#endif //!defined(CONFIG_RTL8196C_CLIENT_ONLY)

#if defined(CONFIG_RTK_MESH) && defined(_MESH_ACL_ENABLE_)	// below code copy above ACL code
	// initialize MAC access control list
	if ( !init_linkchain(&meshAclChain, sizeof(MACFILTER_T), MAX_MESH_ACL_NUM)) {
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_shm_free(pMib, CSCONF_SHM_KEY);
		apmib_shm_free(pMibDef, DSCONF_SHM_KEY);
		apmib_shm_free(pHwSetting, HWCONF_SHM_KEY);
		apmib_sem_unlock();
#else
		free(pMib);
		free(pMibDef);
		free(pHwSetting);
#endif			
		return 0;
	}

	for (i=0; i<pMib->meshAclNum; i++) {
		if ( !add_linkchain(&meshAclChain, (char *)&pMib->meshAclAddrArray[i]) ) {
#if CONFIG_APMIB_SHARED_MEMORY == 1
			apmib_shm_free(pMib, CSCONF_SHM_KEY);
			apmib_shm_free(pMibDef, DSCONF_SHM_KEY);
			apmib_shm_free(pHwSetting, HWCONF_SHM_KEY);
			apmib_sem_unlock();
#else
			free(pMib);
			free(pMibDef);
			free(pHwSetting);
#endif
			return 0;
		}
	}
	meshAclChain.compareLen = sizeof(MACFILTER_T) - COMMENT_LEN;
#endif	// CONFIG_RTK_MESH && _MESH_ACL_ENABLE_

	// initialize schedule table
	if ( !init_linkchain(&scheduleRuleChain, sizeof(SCHEDULE_T), MAX_SCHEDULE_NUM)) {
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_shm_free(pMib, CSCONF_SHM_KEY);
		apmib_shm_free(pMibDef, DSCONF_SHM_KEY);
		apmib_shm_free(pHwSetting, HWCONF_SHM_KEY);
#else
		free(pMib);
		free(pMibDef);
		free(pHwSetting);
#endif
		return 0;
	}
	for (i=0; i<pMib->scheduleRuleNum; i++) {
		if ( !add_linkchain(&scheduleRuleChain, (char *)&pMib->scheduleRuleArray[i]) ) {
#if CONFIG_APMIB_SHARED_MEMORY == 1
			apmib_shm_free(pMib, CSCONF_SHM_KEY);
			apmib_shm_free(pMibDef, DSCONF_SHM_KEY);
			apmib_shm_free(pHwSetting, HWCONF_SHM_KEY);
#else
			free(pMib);
			free(pMibDef);
			free(pHwSetting);
#endif
			return 0;
		}
	}
	scheduleRuleChain.compareLen = sizeof(SCHEDULE_T);
	




#ifdef HOME_GATEWAY
	// initialize port forwarding table
	if ( !init_linkchain(&portFwChain, sizeof(PORTFW_T), MAX_FILTER_NUM)) {
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_shm_free(pMib, CSCONF_SHM_KEY);
		apmib_shm_free(pMibDef, DSCONF_SHM_KEY);
		apmib_shm_free(pHwSetting, HWCONF_SHM_KEY);
		apmib_sem_unlock();
#else
		free(pMib);
		free(pMibDef);
		free(pHwSetting);
#endif
		return 0;
	}
	for (i=0; i<pMib->portFwNum; i++) {
		if ( !add_linkchain(&portFwChain, (char *)&pMib->portFwArray[i]) ) {
#if CONFIG_APMIB_SHARED_MEMORY == 1
			apmib_shm_free(pMib, CSCONF_SHM_KEY);
			apmib_shm_free(pMibDef, DSCONF_SHM_KEY);
			apmib_shm_free(pHwSetting, HWCONF_SHM_KEY);
			apmib_sem_unlock();
#else
			free(pMib);
			free(pMibDef);
			free(pHwSetting);
#endif
			return 0;
		}
	}
	portFwChain.compareLen = sizeof(PORTFW_T) - COMMENT_LEN;

	// initialize ip-filter table
	if ( !init_linkchain(&ipFilterChain, sizeof(IPFILTER_T), MAX_FILTER_NUM)) {
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_shm_free(pMib, CSCONF_SHM_KEY);
		apmib_shm_free(pMibDef, DSCONF_SHM_KEY);
		apmib_shm_free(pHwSetting, HWCONF_SHM_KEY);
		apmib_sem_unlock();
#else
		free(pMib);
		free(pMibDef);
		free(pHwSetting);
#endif
		return 0;
	}
	for (i=0; i<pMib->ipFilterNum; i++) {
		if ( !add_linkchain(&ipFilterChain, (char *)&pMib->ipFilterArray[i]) ) {
#if CONFIG_APMIB_SHARED_MEMORY == 1
			apmib_shm_free(pMib, CSCONF_SHM_KEY);
			apmib_shm_free(pMibDef, DSCONF_SHM_KEY);
			apmib_shm_free(pHwSetting, HWCONF_SHM_KEY);
			apmib_sem_unlock();
#else
			free(pMib);
			free(pMibDef);
			free(pHwSetting);
#endif
			return 0;
		}
	}
	ipFilterChain.compareLen = sizeof(IPFILTER_T) - COMMENT_LEN;

	// initialize port-filter table
	if ( !init_linkchain(&portFilterChain, sizeof(PORTFILTER_T), MAX_FILTER_NUM)) {
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_shm_free(pMib, CSCONF_SHM_KEY);
		apmib_shm_free(pMibDef, DSCONF_SHM_KEY);
		apmib_shm_free(pHwSetting, HWCONF_SHM_KEY);
		apmib_sem_unlock();
#else
		free(pMib);
		free(pMibDef);
		free(pHwSetting);
#endif
		return 0;
	}
	for (i=0; i<pMib->portFilterNum; i++) {
		if ( !add_linkchain(&portFilterChain, (char *)&pMib->portFilterArray[i]) ) {
#if CONFIG_APMIB_SHARED_MEMORY == 1
			apmib_shm_free(pMib, CSCONF_SHM_KEY);
			apmib_shm_free(pMibDef, DSCONF_SHM_KEY);
			apmib_shm_free(pHwSetting, HWCONF_SHM_KEY);
			apmib_sem_unlock();
#else
			free(pMib);
			free(pMibDef);
			free(pHwSetting);
#endif
			return 0;
		}
	}
	portFilterChain.compareLen = sizeof(PORTFILTER_T) - COMMENT_LEN;

	// initialize mac-filter table
	if ( !init_linkchain(&macFilterChain, sizeof(MACFILTER_T), MAX_FILTER_NUM)) {
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_shm_free(pMib, CSCONF_SHM_KEY);
		apmib_shm_free(pMibDef, DSCONF_SHM_KEY);
		apmib_shm_free(pHwSetting, HWCONF_SHM_KEY);
		apmib_sem_unlock();
#else
		free(pMib);
		free(pMibDef);
		free(pHwSetting);
#endif
		return 0;
	}
	for (i=0; i<pMib->macFilterNum; i++) {
		if ( !add_linkchain(&macFilterChain, (char *)&pMib->macFilterArray[i]) ) {
#if CONFIG_APMIB_SHARED_MEMORY == 1
			apmib_shm_free(pMib, CSCONF_SHM_KEY);
			apmib_shm_free(pMibDef, DSCONF_SHM_KEY);
			apmib_shm_free(pHwSetting, HWCONF_SHM_KEY);
			apmib_sem_unlock();
#else
			free(pMib);
			free(pMibDef);
			free(pHwSetting);
#endif
			return 0;
		}
	}
	macFilterChain.compareLen = sizeof(MACFILTER_T) - COMMENT_LEN;

	// initialize url-filter table
	if ( !init_linkchain(&urlFilterChain, sizeof(URLFILTER_T), MAX_URLFILTER_NUM)) {
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_shm_free(pMib, CSCONF_SHM_KEY);
		apmib_shm_free(pMibDef, DSCONF_SHM_KEY);
		apmib_shm_free(pHwSetting, HWCONF_SHM_KEY);
		apmib_sem_unlock();
#else
		free(pMib);
		free(pMibDef);
		free(pHwSetting);
#endif
		return 0;
	}
	for (i=0; i<pMib->urlFilterNum; i++) {
		if ( !add_linkchain(&urlFilterChain, (char *)&pMib->urlFilterArray[i]) ) {
#if CONFIG_APMIB_SHARED_MEMORY == 1
			apmib_shm_free(pMib, CSCONF_SHM_KEY);
			apmib_shm_free(pMibDef, DSCONF_SHM_KEY);
			apmib_shm_free(pHwSetting, HWCONF_SHM_KEY);
			apmib_sem_unlock();
#else
			free(pMib);
			free(pMibDef);
			free(pHwSetting);
#endif
			return 0;
		}
	}
	urlFilterChain.compareLen = sizeof(URLFILTER_T);// - COMMENT_LEN;

	// initialize trigger-port table
	if ( !init_linkchain(&triggerPortChain, sizeof(TRIGGERPORT_T), MAX_FILTER_NUM)) {
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_shm_free(pMib, CSCONF_SHM_KEY);
		apmib_shm_free(pMibDef, DSCONF_SHM_KEY);
		apmib_shm_free(pHwSetting, HWCONF_SHM_KEY);
		apmib_sem_unlock();
#else
		free(pMib);
		free(pMibDef);
		free(pHwSetting);
#endif
		return 0;
	}
	for (i=0; i<pMib->triggerPortNum; i++) {
		if ( !add_linkchain(&triggerPortChain, (char *)&pMib->triggerPortArray[i]) ) {
#if CONFIG_APMIB_SHARED_MEMORY == 1
			apmib_shm_free(pMib, CSCONF_SHM_KEY);
			apmib_shm_free(pMibDef, DSCONF_SHM_KEY);
			apmib_shm_free(pHwSetting, HWCONF_SHM_KEY);
			apmib_sem_unlock();
#else
			free(pMib);
			free(pMibDef);
			free(pHwSetting);
#endif
			return 0;
		}
	}
	triggerPortChain.compareLen = 5;	// length of trigger port range + proto type
#ifdef GW_QOS_ENGINE
	// initialize QoS rules table
	if ( !init_linkchain(&qosChain, sizeof(QOS_T), MAX_QOS_RULE_NUM)) {
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_shm_free(pMib, CSCONF_SHM_KEY);
		apmib_shm_free(pMibDef, DSCONF_SHM_KEY);
		apmib_shm_free(pHwSetting, HWCONF_SHM_KEY);
		apmib_sem_unlock();
#else
		free(pMib);
		free(pMibDef);
		free(pHwSetting);
#endif
		return 0;
	}
	for (i=0; i<pMib->qosRuleNum; i++) {
		if ( !add_linkchain(&qosChain, (char *)&pMib->qosRuleArray[i]) ) {
#if CONFIG_APMIB_SHARED_MEMORY == 1
			apmib_shm_free(pMib, CSCONF_SHM_KEY);
			apmib_shm_free(pMibDef, DSCONF_SHM_KEY);
			apmib_shm_free(pHwSetting, HWCONF_SHM_KEY);
			apmib_sem_unlock();
#else
			free(pMib);
			free(pMibDef);
			free(pHwSetting);
#endif
			return 0;
		}
	}
	qosChain.compareLen =  sizeof(QOS_T);
#endif

#ifdef QOS_BY_BANDWIDTH
	// initialize QoS rules table
	if ( !init_linkchain(&qosChain, sizeof(IPQOS_T), MAX_QOS_RULE_NUM)) {
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_shm_free(pMib, CSCONF_SHM_KEY);
		apmib_shm_free(pMibDef, DSCONF_SHM_KEY);
		apmib_shm_free(pHwSetting, HWCONF_SHM_KEY);
		apmib_sem_unlock();
#else
		free(pMib);
		free(pMibDef);
		free(pHwSetting);
#endif
		return 0;
	}
	for (i=0; i<pMib->qosRuleNum; i++) {
		if ( !add_linkchain(&qosChain, (char *)&pMib->qosRuleArray[i]) ) {
#if CONFIG_APMIB_SHARED_MEMORY == 1
			apmib_shm_free(pMib, CSCONF_SHM_KEY);
			apmib_shm_free(pMibDef, DSCONF_SHM_KEY);
			apmib_shm_free(pHwSetting, HWCONF_SHM_KEY);
			apmib_sem_unlock();
#else
			free(pMib);
			free(pMibDef);
			free(pHwSetting);
#endif
			return 0;
		}
	}
	qosChain.compareLen =  sizeof(IPQOS_T);
#endif

#ifdef ROUTE_SUPPORT
	// initialize static route table
	if ( !init_linkchain(&staticRouteChain, sizeof(STATICROUTE_T), MAX_ROUTE_NUM)) {
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_shm_free(pMib, CSCONF_SHM_KEY);
		apmib_shm_free(pMibDef, DSCONF_SHM_KEY);
		apmib_shm_free(pHwSetting, HWCONF_SHM_KEY);
		apmib_sem_unlock();
#else
		free(pMib);
		free(pMibDef);
		free(pHwSetting);
#endif
		return 0;
	}
	for (i=0; i<pMib->staticRouteNum; i++) {
		if ( !add_linkchain(&staticRouteChain, (char *)&pMib->staticRouteArray[i]) ) {
#if CONFIG_APMIB_SHARED_MEMORY == 1
			apmib_shm_free(pMib, CSCONF_SHM_KEY);
			apmib_shm_free(pMibDef, DSCONF_SHM_KEY);
			apmib_shm_free(pHwSetting, HWCONF_SHM_KEY);
			apmib_sem_unlock();
#else
			free(pMib);
			free(pMibDef);
			free(pHwSetting);
#endif
			return 0;
		}
	}
	staticRouteChain.compareLen = sizeof(STATICROUTE_T) -4 ; // not contain gateway
#endif //ROUTE
#ifdef VPN_SUPPORT
	// initialize port forwarding table
	if ( !init_linkchain(&ipsecTunnelChain, sizeof(IPSECTUNNEL_T), MAX_TUNNEL_NUM)) {
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_shm_free(pMib, CSCONF_SHM_KEY);
		apmib_shm_free(pMibDef, DSCONF_SHM_KEY);
		apmib_shm_free(pHwSetting, HWCONF_SHM_KEY);
		apmib_sem_unlock();
#else
		free(pMib);
		free(pMibDef);
		free(pHwSetting);
#endif
		return 0;
	}
	for (i=0; i<pMib->ipsecTunnelNum; i++) {
		if ( !add_linkchain(&ipsecTunnelChain, (char *)&pMib->ipsecTunnelArray[i]) ) {
#if CONFIG_APMIB_SHARED_MEMORY == 1
			apmib_shm_free(pMib, CSCONF_SHM_KEY);
			apmib_shm_free(pMibDef, DSCONF_SHM_KEY);
			apmib_shm_free(pHwSetting, HWCONF_SHM_KEY);
			apmib_sem_unlock();
#else
			free(pMib);
			free(pMibDef);
			free(pHwSetting);
#endif
			return 0;
		}
	}
	ipsecTunnelChain.compareLen = 1 ;  // only tunnel id
#endif
#endif // HOME_GATEWAY
#ifdef TLS_CLIENT
	if ( !init_linkchain(&certRootChain, sizeof(CERTROOT_T), MAX_CERTROOT_NUM)) {
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_shm_free(pMib, CSCONF_SHM_KEY);
		apmib_shm_free(pMibDef, DSCONF_SHM_KEY);
		apmib_shm_free(pHwSetting, HWCONF_SHM_KEY);
		apmib_sem_unlock();
#else
		free(pMib);
		free(pMibDef);
		free(pHwSetting);
#endif
		return 0;
	}
	for (i=0; i<pMib->certRootNum; i++) {
		if ( !add_linkchain(&certRootChain, (char *)&pMib->certRootArray[i]) ) {
#if CONFIG_APMIB_SHARED_MEMORY == 1
			apmib_shm_free(pMib, CSCONF_SHM_KEY);
			apmib_shm_free(pMibDef, DSCONF_SHM_KEY);
			apmib_shm_free(pHwSetting, HWCONF_SHM_KEY);
			apmib_sem_unlock();
#else
			free(pMib);
			free(pMibDef);
			free(pHwSetting);
#endif
			return 0;
		}
	}
	certRootChain.compareLen = 21 ;  // only comment
	if ( !init_linkchain(&certUserChain, sizeof(CERTUSER_T), MAX_CERTUSER_NUM)) {
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_shm_free(pMib, CSCONF_SHM_KEY);
		apmib_shm_free(pMibDef, DSCONF_SHM_KEY);
		apmib_shm_free(pHwSetting, HWCONF_SHM_KEY);
		apmib_sem_unlock();
#else
		free(pMib);
		free(pMibDef);
		free(pHwSetting);
#endif
		return 0;
	}
	for (i=0; i<pMib->certUserNum; i++) {
		if ( !add_linkchain(&certUserChain, (char *)&pMib->certUserArray[i]) ) {
#if CONFIG_APMIB_SHARED_MEMORY == 1
			apmib_shm_free(pMib, CSCONF_SHM_KEY);
			apmib_shm_free(pMibDef, DSCONF_SHM_KEY);
			apmib_shm_free(pHwSetting, HWCONF_SHM_KEY);
			apmib_sem_unlock();
#else
			free(pMib);
			free(pMibDef);
			free(pHwSetting);
#endif
			return 0;
		}
	}
	certUserChain.compareLen = 21 ;  // only comment	
#endif
	init_linkchain(&dhcpRsvdIpChain, sizeof(DHCPRSVDIP_T), MAX_DHCP_RSVD_IP_NUM);
	for (i=0; i<pMib->dhcpRsvdIpNum; i++)
		add_linkchain(&dhcpRsvdIpChain, (char *)&pMib->dhcpRsvdIpArray[i]);	
	dhcpRsvdIpChain.compareLen = 4;

#if defined(VLAN_CONFIG_SUPPORTED)
	init_linkchain(&vlanConfigChain, sizeof(VLAN_CONFIG_T), MAX_IFACE_VLAN_CONFIG);
	for (i=0; i<pMib->VlanConfigNum; i++)
		add_linkchain(&vlanConfigChain, (char *)&pMib->VlanConfigArray[i]);	
	vlanConfigChain.compareLen = sizeof(VLAN_CONFIG_T);

#endif

#if 0 //michael
#ifdef WEBS
	save_cs_to_file();
#endif
#endif

#if CONFIG_APMIB_SHARED_MEMORY == 1
    apmib_sem_unlock();
#endif

	return 1;
}


///////////////////////////////////////////////////////////////////////////////
#if CONFIG_APMIB_SHARED_MEMORY == 1
char *apmib_load_hwconf(void)
{
	int ver;
	char *buff;
	int created;

	// Read hw setting
	if ( flash_read((char *)&hsHeader, HW_SETTING_OFFSET, sizeof(hsHeader))==0 ) {
//		printf("Read hw setting header failed!\n");
		return NULL;
	}

	if ( sscanf(&hsHeader.signature[TAG_LEN], "%02d", &ver) != 1)
		ver = -1;

	if ( memcmp(hsHeader.signature, HW_SETTING_HEADER_TAG, TAG_LEN) || // invalid signatur
		(ver != HW_SETTING_VER) || // version not equal to current
		(hsHeader.len < (sizeof(HW_SETTING_T)+1)) ) { // length is less than current
//		printf("Invalid hw setting signature or version number [sig=%c%c, ver=%d, len=%d]!\n", hsHeader.signature[0],
//			hsHeader.signature[1], ver, hsHeader.len);
		return NULL;
	}
//	if (ver > HW_SETTING_VER)
//		printf("HW setting version is greater than current [f:%d, c:%d]!\n", ver, HW_SETTING_VER);

	buff = apmib_shm_calloc(1, hsHeader.len, HWCONF_SHM_KEY, &created);
	if ( buff == 0 ) {
//		printf("Allocate buffer failed!\n");
		return NULL;
	}

	if ( flash_read(buff, HW_SETTING_OFFSET+sizeof(hsHeader), hsHeader.len)==0 ) {
//		printf("Read hw setting failed!\n");
		apmib_shm_free(buff, HWCONF_SHM_KEY);
		return NULL;
	}
	if ( !CHECKSUM_OK(buff, hsHeader.len) ) {
//		printf("Invalid checksum of hw setting!\n");
		apmib_shm_free(buff, HWCONF_SHM_KEY);
		return NULL;
	}
	return buff;
}

////////////////////////////////////////////////////////////////////////////////
char *apmib_load_dsconf(void)
{
	int ver;
	char *buff;
	int created;
	
#if !defined(MOVE_OUT_DEFAULT_SETTING_FROM_FLASH)
	// Read default s/w mib
	if ( flash_read((char *)&dsHeader, DEFAULT_SETTING_OFFSET, sizeof(dsHeader))==0 ) {
//		printf("Read default setting header failed!\n");
		return NULL;
	}

	if ( sscanf(&dsHeader.signature[TAG_LEN], "%02d", &ver) != 1)
		ver = -1;

	if ( memcmp(dsHeader.signature, DEFAULT_SETTING_HEADER_TAG, TAG_LEN) || // invalid signatur
		(ver != DEFAULT_SETTING_VER) || // version not equal to current
		(dsHeader.len < (sizeof(APMIB_T)+1)) ) { // length is less than current
//		printf("Invalid default setting signature or version number [sig=%c%c, ver=%d, len=%d]!\n",
//			dsHeader.signature[0], dsHeader.signature[1], ver, dsHeader.len);
		return NULL;
	}
//	if (ver > DEFAULT_SETTING_VER)
//		printf("Default setting version is greater than current [f:%d, c:%d]!\n", ver, DEFAULT_SETTING_VER);

	buff = apmib_shm_calloc(1, dsHeader.len, DSCONF_SHM_KEY, &created);
	if ( buff == 0 ) {
//		printf("Allocate buffer failed!\n");
		return NULL;
	}

	if ( flash_read(buff, DEFAULT_SETTING_OFFSET+sizeof(dsHeader), dsHeader.len)==0 ) {
//		printf("Read default setting failed!\n");
		apmib_shm_free(buff, DSCONF_SHM_KEY);
		return NULL;
	}

	if ( !CHECKSUM_OK(buff, dsHeader.len) ) {
//		printf("Invalid checksum of current setting!\n");
		apmib_shm_free(buff, DSCONF_SHM_KEY);
		return NULL;
	}
	
#else

	buff = apmib_shm_calloc(1, dsHeader.len, DSCONF_SHM_KEY, &created);
	if ( buff == 0 ) {
//		printf("Allocate buffer failed!\n");
		return NULL;
	}
	/* read default setting to buff*/
	apmib_default_setting((APMIB_Tp)buff);
	buff[dsHeader.len-1] = CHECKSUM(buff, dsHeader.len-1);

	if ( !CHECKSUM_OK(buff, dsHeader.len) ) {
		printf("Invalid checksum of current setting!\n");
		apmib_shm_free(buff, DSCONF_SHM_KEY);
		return NULL;
	}	
	
#endif
	return buff;
}

////////////////////////////////////////////////////////////////////////////////
char *apmib_load_csconf(void)
{
	int ver;
	char *buff;
	int created;

	// Read current s/w mib
	if ( flash_read((char *)&csHeader, CURRENT_SETTING_OFFSET, sizeof(csHeader))==0 ) {
//		printf("Read current setting header failed!\n");
		return NULL;
	}

	if ( sscanf(&csHeader.signature[TAG_LEN], "%02d", &ver) != 1)
		ver = -1;

	if ( memcmp(csHeader.signature, CURRENT_SETTING_HEADER_TAG, TAG_LEN) || // invalid signatur
		(ver != CURRENT_SETTING_VER) || // version not equal to current
			(csHeader.len < (sizeof(APMIB_T)+1)) ) { // length is less than current
//		printf("Invalid current setting signature or version number [sig=%c%c, ver=%d, len=%d]!\n",
//			csHeader.signature[0], csHeader.signature[1], ver, csHeader.len);
		return NULL;
	}

//	if (ver > CURRENT_SETTING_VER)
//		printf("Current setting version is greater than current [f:%d, c:%d]!\n", ver, CURRENT_SETTING_VER);

	buff = apmib_shm_calloc(1, csHeader.len, CSCONF_SHM_KEY, &created);
	if ( buff == 0 ) {
//		printf("Allocate buffer failed!\n");
		return NULL;
	}

	if ( flash_read(buff, CURRENT_SETTING_OFFSET+sizeof(csHeader), csHeader.len)==0 ) {
//		printf("Read current setting failed!\n");
		apmib_shm_free(buff, CSCONF_SHM_KEY);
		return NULL;
	}

	if ( !CHECKSUM_OK(buff, csHeader.len) ) {
//		printf("Invalid checksum of current setting!\n");
		apmib_shm_free(buff, CSCONF_SHM_KEY);
		return NULL;
	}

	return buff;
}
#endif

int apmib_reinit(void)
{
	int i, j;
	if (pMib == NULL)	// has not been initialized
		return 0;

#if CONFIG_APMIB_SHARED_MEMORY != 1
	free(pMib);
	free(pMibDef);
	free(pHwSetting);
#endif
	for (i=0; i<NUM_WLAN_INTERFACE; i++) 
		for (j=0; j<(NUM_VWLAN_INTERFACE+1); j++) 
	{
		free(wlanMacChain[i][j].buf);
		free(wdsChain[i][j].buf);
	}

#ifdef HOME_GATEWAY
	free(portFwChain.buf);
	free(ipFilterChain.buf);
	free(portFilterChain.buf);
	free(macFilterChain.buf);
	free(urlFilterChain.buf);
	free(triggerPortChain.buf);
#if defined(GW_QOS_ENGINE) || defined(QOS_BY_BANDWIDTH)
    	free(qosChain.buf);
#endif
#ifdef ROUTE_SUPPORT
	free(staticRouteChain.buf);
#endif //ROUTE
#ifdef VPN_SUPPORT
	free(ipsecTunnelChain.buf);
#endif
#endif
#ifdef TLS_CLIENT
	free(certRootChain.buf);
	free(certUserChain.buf);
#endif

	free(dhcpRsvdIpChain.buf);
#if defined(VLAN_CONFIG_SUPPORTED)
	free(vlanConfigChain.buf);
#endif	
#if CONFIG_APMIB_SHARED_MEMORY != 1
	pMib=NULL;
	pMibDef=NULL;
	pHwSetting=NULL;
#endif


#if CONFIG_APMIB_SHARED_MEMORY == 1	
    apmib_sem_lock();
    apmib_load_hwconf();
    apmib_load_dsconf();
    apmib_load_csconf();
    apmib_sem_unlock();
#endif

	return apmib_init();
}

////////////////////////////////////////////////////////////////////////////////
static int search_tbl(int id, mib_table_entry_T *pTbl, int *idx)
{
	int i;
	for (i=0; pTbl[i].id; i++) {
		if ( pTbl[i].id == id ) {
			*idx = i;
			return id;
		}
	}
	return 0;
}


////////////////////////////////////////////////////////////////////////////////
int apmib_get(int id, void *value)
{
	int i, index;
	void *pMibTbl;
	mib_table_entry_T *pTbl;
	unsigned char ch;
	unsigned short wd;
	unsigned long dwd;

#if CONFIG_APMIB_SHARED_MEMORY == 1
	apmib_sem_lock();
#endif

	if ( search_tbl(id, mib_table, &i) ) {
		pMibTbl = (void *)pMib;
		pTbl = mib_table;
	}
	else if ( search_tbl(id, mib_wlan_table, &i) ) {
		pMibTbl = (void *)&pMib->wlan[wlan_idx][vwlan_idx];
		pTbl = mib_wlan_table;
	}
	else if ( search_tbl(id, hwmib_table, &i) ) {
		pMibTbl = (void *)pHwSetting;
		pTbl = hwmib_table;
	}
	else if ( search_tbl(id, hwmib_wlan_table, &i) ) {
		pMibTbl = (void *)&pHwSetting->wlan[wlan_idx];
		pTbl = hwmib_wlan_table;
	}
	else {
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_sem_unlock();
#endif
		return 0;
	}
	switch (pTbl[i].type) {
	case BYTE_T:
//		*((int *)value) =(int)(*((unsigned char *)(((long)pMibTbl) + pTbl[i].offset)));
		memcpy((char *)&ch, ((char *)pMibTbl) + pTbl[i].offset, 1);
		*((int *)value) = (int)ch;
		break;

	case WORD_T:
//		*((int *)value) =(int)(*((unsigned short *)(((long)pMibTbl) + pTbl[i].offset)));
		memcpy((char *)&wd, ((char *)pMibTbl) + pTbl[i].offset, 2);
		*((int *)value) = (int)wd;
		break;

	case STRING_T:
		strcpy( (char *)value, (const char *)(((long)pMibTbl) + pTbl[i].offset) );
		break;

	case BYTE5_T:
		memcpy( (unsigned char *)value, (unsigned char *)(((long)pMibTbl) + pTbl[i].offset), 5);
		break;

	case BYTE6_T:
		memcpy( (unsigned char *)value, (unsigned char *)(((long)pMibTbl) + pTbl[i].offset), 6);
		break;

	case BYTE13_T:
		memcpy( (unsigned char *)value, (unsigned char *)(((long)pMibTbl) + pTbl[i].offset), 13);
		break;

	case DWORD_T:
		memcpy((char *)&dwd, ((char *)pMibTbl) + pTbl[i].offset, 4);
		*((int *)value) = (int)dwd;
		break;

	case BYTE_ARRAY_T:
#ifdef VOIP_SUPPORT
		if(id == MIB_VOIP_CFG){
			// rock: do nothing here, use flash voip get xxx to replace
		}
		else
#endif /*VOIP_SUPPORT*/
		memcpy( (unsigned char *)value, (unsigned char *)(((long)pMibTbl) + pTbl[i].offset), pTbl[i].size);
		break;

	case IA_T:
		memcpy( (unsigned char *)value, (unsigned char *)(((long)pMibTbl) + pTbl[i].offset), 4);
		break;

	case WLAC_ARRAY_T:
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_sem_unlock();
#endif	
		index = (int)( *((unsigned char *)value));
		return get_linkchain(&wlanMacChain[wlan_idx][vwlan_idx], (char *)value, index );

#if defined(CONFIG_RTK_MESH) && defined(_MESH_ACL_ENABLE_) // below code copy above ACL code
	case MESH_ACL_ARRAY_T:
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_sem_unlock();
#endif	
		index = (int)( *((unsigned char *)value));
		return get_linkchain(&meshAclChain, (char *)value, index );
#endif	// CONFIG_RTK_MESH && _MESH_ACL_ENABLE_

	case WDS_ARRAY_T:
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_sem_unlock();
#endif	
		index = (int)( *((unsigned char *)value));
		return get_linkchain(&wdsChain[wlan_idx][vwlan_idx], (char *)value, index );

	case SCHEDULE_ARRAY_T:
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_sem_unlock();
#endif	
    index = (int)( *((unsigned char *)value));
 		return get_linkchain(&scheduleRuleChain, (char *)value, index ); 		


#ifdef HOME_GATEWAY
	case PORTFW_ARRAY_T:
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_sem_unlock();
#endif	
                index = (int)( *((unsigned char *)value));
 		return get_linkchain(&portFwChain, (char *)value, index );

	case IPFILTER_ARRAY_T:
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_sem_unlock();
#endif	
		index = (int)( *((unsigned char *)value));
 		return get_linkchain(&ipFilterChain, (char *)value, index );

	case PORTFILTER_ARRAY_T:
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_sem_unlock();
#endif	
                index = (int)( *((unsigned char *)value));
 		return get_linkchain(&portFilterChain, (char *)value, index );

	case MACFILTER_ARRAY_T:
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_sem_unlock();
#endif	
                index = (int)( *((unsigned char *)value));
 		return get_linkchain(&macFilterChain, (char *)value, index );

	case URLFILTER_ARRAY_T:
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_sem_unlock();
#endif	
                index = (int)( *((unsigned char *)value));
 		return get_linkchain(&urlFilterChain, (char *)value, index );

	case TRIGGERPORT_ARRAY_T:
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_sem_unlock();
#endif	
                index = (int)( *((unsigned char *)value));
 		return get_linkchain(&triggerPortChain, (char *)value, index );

#if defined(GW_QOS_ENGINE) || defined(QOS_BY_BANDWIDTH)
	case QOS_ARRAY_T:
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_sem_unlock();
#endif
                index = (int)( *((unsigned char *)value));
 		return get_linkchain(&qosChain, (char *)value, index );
#endif
#ifdef ROUTE_SUPPORT
	case STATICROUTE_ARRAY_T:
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_sem_unlock();
#endif	
                index = (int)( *((unsigned char *)value));
 		return get_linkchain(&staticRouteChain, (char *)value, index );
#endif

#ifdef VPN_SUPPORT
	case IPSECTUNNEL_ARRAY_T:
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_sem_unlock();
#endif
                index = (int)( *((unsigned char *)value));
 		return get_linkchain(&ipsecTunnelChain, (char *)value, index );
#endif
#endif
#ifdef TLS_CLIENT
	case CERTROOT_ARRAY_T:
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_sem_unlock();
#endif
                index = (int)( *((unsigned char *)value));
 		return get_linkchain(&certRootChain, (char *)value, index );
	case CERTUSER_ARRAY_T:
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_sem_unlock();
#endif
                index = (int)( *((unsigned char *)value));
 		return get_linkchain(&certUserChain, (char *)value, index ); 		
#endif
	case DHCPRSVDIP_ARRY_T:	
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_sem_unlock();
#endif
		index = (int)( *((unsigned char *)value));
 		return get_linkchain(&dhcpRsvdIpChain, (char *)value, index );

#if defined(VLAN_CONFIG_SUPPORTED)
	case VLANCONFIG_ARRAY_T:	
#if CONFIG_APMIB_SHARED_MEMORY == 1
	apmib_sem_unlock();
#endif
		index = (int)( *((unsigned char *)value));
 		return get_linkchain(&vlanConfigChain, (char *)value, index ); 		
#endif 		
	}

#if CONFIG_APMIB_SHARED_MEMORY == 1
	apmib_sem_unlock();
#endif
	return 1;
}


////////////////////////////////////////////////////////////////////////////////
int apmib_getDef(int id, void *value)
{
	int ret;
	APMIB_Tp saveMib=pMib;

	pMib = pMibDef;
	ret = apmib_get(id, value);
	pMib = saveMib;
	return ret;
}


////////////////////////////////////////////////////////////////////////////////
int apmib_set(int id, void *value)
{
	int i, ret;
	void *pMibTbl;
	mib_table_entry_T *pTbl;
	unsigned char ch;
	unsigned short wd;
	unsigned long dwd;
	unsigned char* tmp;
	int max_chan_num;

#if CONFIG_APMIB_SHARED_MEMORY == 1
	apmib_sem_lock();
#endif
#if !defined(CONFIG_RTL8196C_CLIENT_ONLY)	
	if (id == MIB_WLAN_AC_ADDR_ADD) {
		ret = add_linkchain(&wlanMacChain[wlan_idx][vwlan_idx], (char *)value);
		if ( ret )
			pMib->wlan[wlan_idx][vwlan_idx].acNum++;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return ret;
	}
	if (id == MIB_WLAN_AC_ADDR_DEL) {
		ret = delete_linkchain(&wlanMacChain[wlan_idx][vwlan_idx], (char *)value);
		if ( ret )
			pMib->wlan[wlan_idx][vwlan_idx].acNum--;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return ret;
	}
	if (id == MIB_WLAN_AC_ADDR_DELALL) {
		delete_all_linkchain(&wlanMacChain[wlan_idx][vwlan_idx]);
		pMib->wlan[wlan_idx][vwlan_idx].acNum = 0;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return 1;
	}
#endif //!defined(CONFIG_RTL8196C_CLIENT_ONLY)	
#if defined(CONFIG_RTK_MESH) && defined(_MESH_ACL_ENABLE_) // below code copy above ACL code
	if (id == MIB_MESH_ACL_ADDR_ADD) {
		ret = add_linkchain(&meshAclChain, (char *)value);
		if ( ret )
			pMib->meshAclNum++;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return ret;
	}
	if (id == MIB_MESH_ACL_ADDR_DEL) {
		ret = delete_linkchain(&meshAclChain, (char *)value);
		if ( ret )
			pMib->meshAclNum--;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return ret;
	}
	if (id == MIB_MESH_ACL_ADDR_DELALL) {
		delete_all_linkchain(&meshAclChain);
		pMib->meshAclNum = 0;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return 1;
	}
#endif	// CONFIG_RTK_MESH && _MESH_ACL_ENABLE_

#if !defined(CONFIG_RTL8196C_CLIENT_ONLY)	
	if (id == MIB_WLAN_WDS_ADD) {
		ret = add_linkchain(&wdsChain[wlan_idx][vwlan_idx], (char *)value);
		if ( ret )
			pMib->wlan[wlan_idx][vwlan_idx].wdsNum++;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return ret;
	}
	if (id == MIB_WLAN_WDS_DEL) {
		ret = delete_linkchain(&wdsChain[wlan_idx][vwlan_idx], (char *)value);
		if ( ret )
			pMib->wlan[wlan_idx][vwlan_idx].wdsNum--;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return ret;
	}
	if (id == MIB_WLAN_WDS_DELALL) {
		delete_all_linkchain(&wdsChain[wlan_idx][vwlan_idx]);
		pMib->wlan[wlan_idx][vwlan_idx].wdsNum = 0;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return 1;
	}
#endif //!defined(CONFIG_RTL8196C_CLIENT_ONLY)		
//Schedule Mib
	if (id == MIB_SCHEDULE_ADD) {
		ret = add_linkchain(&scheduleRuleChain, (char *)value);
		if ( ret )
			pMib->scheduleRuleNum++;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return ret;
	}
	if (id == MIB_SCHEDULE_DEL) {
		ret = delete_linkchain(&scheduleRuleChain, (char *)value);
		if ( ret )
			pMib->scheduleRuleNum--;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return ret;
	}
	if (id == MIB_SCHEDULE_DELALL) {
		delete_all_linkchain(&scheduleRuleChain);
		pMib->scheduleRuleNum = 0;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return 1;
	}

#if defined(VLAN_CONFIG_SUPPORTED)
	if (id == MIB_VLANCONFIG_ADD || id == MIB_VLANCONFIG_DEL) {
		int entryNum=0,i;
		VLAN_CONFIG_T entry;
		VLAN_CONFIG_Tp entry_new;
		entry_new = (VLAN_CONFIG_Tp)value;
		apmib_get(MIB_VLANCONFIG_NUM, (void *)&entryNum);
		for (i=1; i<=entryNum; i++) {
		*((char *)&entry) = (char)i;
			apmib_get(MIB_VLANCONFIG, (void *)&entry);
			if(!strcmp(entry.netIface, entry_new->netIface)){
				update_linkchain(VLANCONFIG_ARRAY_T, &entry, entry_new, sizeof(VLAN_CONFIG_T));
				break;
			}
		}
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return 1;
	}


	if (id == MIB_VLANCONFIG_DELALL) {
		int entryNum=0,i;
		VLAN_CONFIG_T entry;
		VLAN_CONFIG_T entry_new;
		apmib_get(MIB_VLANCONFIG_NUM, (void *)&entryNum);

		for (i=1; i<=entryNum; i++) {
		*((char *)&entry) = (char)i;
			apmib_get(MIB_VLANCONFIG, (void *)&entry);
			memcpy(&entry_new, &entry, sizeof(VLAN_CONFIG_T));
			entry_new.enabled=0;
			update_linkchain(VLANCONFIG_ARRAY_T, &entry, &entry_new, sizeof(VLAN_CONFIG_T));
		}
		
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return 1;
	}

#endif

#ifdef HOME_GATEWAY
	if (id == MIB_PORTFW_ADD) {
		ret = add_linkchain(&portFwChain, (char *)value);
		if ( ret )
			pMib->portFwNum++;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return ret;
	}
	if (id == MIB_PORTFW_DEL) {
		ret = delete_linkchain(&portFwChain, (char *)value);
		if ( ret )
			pMib->portFwNum--;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return ret;
	}
	if (id == MIB_PORTFW_DELALL) {
		delete_all_linkchain(&portFwChain);
		pMib->portFwNum = 0;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return 1;
	}

	if (id == MIB_IPFILTER_ADD) {
		ret = add_linkchain(&ipFilterChain, (char *)value);
		if ( ret )
			pMib->ipFilterNum++;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return ret;
	}
	if (id == MIB_IPFILTER_DEL) {
		ret = delete_linkchain(&ipFilterChain, (char *)value);
		if ( ret )
			pMib->ipFilterNum--;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return ret;
	}
	if (id == MIB_IPFILTER_DELALL) {
		delete_all_linkchain(&ipFilterChain);
		pMib->ipFilterNum = 0;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return 1;
	}

	if (id == MIB_PORTFILTER_ADD) {
		ret = add_linkchain(&portFilterChain, (char *)value);
		if ( ret )
			pMib->portFilterNum++;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return ret;
	}
	if (id == MIB_PORTFILTER_DEL) {
		ret = delete_linkchain(&portFilterChain, (char *)value);
		if ( ret )
			pMib->portFilterNum--;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return ret;
	}
	if (id == MIB_PORTFILTER_DELALL) {
		delete_all_linkchain(&portFilterChain);
		pMib->portFilterNum = 0;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return 1;
	}

	if (id == MIB_MACFILTER_ADD) {
		ret = add_linkchain(&macFilterChain, (char *)value);
		if ( ret )
			pMib->macFilterNum++;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return ret;
	}
	if (id == MIB_MACFILTER_DEL) {
		ret = delete_linkchain(&macFilterChain, (char *)value);
		if ( ret )
			pMib->macFilterNum--;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return ret;
	}
	if (id == MIB_MACFILTER_DELALL) {
		delete_all_linkchain(&macFilterChain);
		pMib->macFilterNum = 0;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return 1;
	}

	if (id == MIB_URLFILTER_ADD) {
		ret = add_linkchain(&urlFilterChain, (char *)value);
		if ( ret )
			pMib->urlFilterNum++;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return ret;
	}
	if (id == MIB_URLFILTER_DEL) {
		ret = delete_linkchain(&urlFilterChain, (char *)value);
		if ( ret )
			pMib->urlFilterNum--;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return ret;
	}
	if (id == MIB_URLFILTER_DELALL) {
		delete_all_linkchain(&urlFilterChain);
		pMib->urlFilterNum = 0;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return 1;
	}

	if (id == MIB_TRIGGERPORT_ADD) {
		ret = add_linkchain(&triggerPortChain, (char *)value);
		if ( ret )
			pMib->triggerPortNum++;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return ret;
	}
	if (id == MIB_TRIGGERPORT_DEL) {
		ret = delete_linkchain(&triggerPortChain, (char *)value);
		if ( ret )
			pMib->triggerPortNum--;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return ret;
	}
	if (id == MIB_TRIGGERPORT_DELALL) {
		delete_all_linkchain(&triggerPortChain);
		pMib->triggerPortNum = 0;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return 1;
	}

#if defined(GW_QOS_ENGINE) || defined(QOS_BY_BANDWIDTH)
	if (id == MIB_QOS_ADD) {
		ret = add_linkchain(&qosChain, (char *)value);
		if ( ret )
			pMib->qosRuleNum++;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return ret;
	}
	if (id == MIB_QOS_DEL) {
		ret = delete_linkchain(&qosChain, (char *)value);
		if ( ret )
			pMib->qosRuleNum--;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return ret;
	}
	if (id == MIB_QOS_DELALL) {
		delete_all_linkchain(&qosChain);
		pMib->qosRuleNum = 0;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return 1;
	}
#endif
#ifdef ROUTE_SUPPORT
	if (id == MIB_STATICROUTE_ADD) {
		ret = add_linkchain(&staticRouteChain, (char *)value);
		if ( ret )
			pMib->staticRouteNum++;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return ret;
	}
	if (id == MIB_STATICROUTE_DEL) {
		ret = delete_linkchain(&staticRouteChain, (char *)value);
		if ( ret )
			pMib->staticRouteNum--;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return ret;
	}
	if (id == MIB_STATICROUTE_DELALL) {
		delete_all_linkchain(&staticRouteChain);
		pMib->staticRouteNum = 0;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return 1;
	}
#endif //ROUTE
#endif

	if (id == MIB_DHCPRSVDIP_DEL) {
		ret = delete_linkchain(&dhcpRsvdIpChain, (char *)value);
		if ( ret )
			pMib->dhcpRsvdIpNum--;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return ret;
	}
	if (id == MIB_DHCPRSVDIP_DELALL) {
		delete_all_linkchain(&dhcpRsvdIpChain);
		pMib->dhcpRsvdIpNum = 0;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return 1;
	}
	if (id == MIB_DHCPRSVDIP_ADD) {
		ret = add_linkchain(&dhcpRsvdIpChain, (char *)value);
		if ( ret )
			pMib->dhcpRsvdIpNum++;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return ret;
	}

#ifdef HOME_GATEWAY
#ifdef VPN_SUPPORT
	if (id == MIB_IPSECTUNNEL_ADD) {
		ret = add_linkchain(&ipsecTunnelChain, (char *)value);
		if ( ret )
			pMib->ipsecTunnelNum++;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return ret;
	}
	if (id == MIB_IPSECTUNNEL_DEL) {
		ret = delete_linkchain(&ipsecTunnelChain, (char *)value);
		if ( ret )
			pMib->ipsecTunnelNum--;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return ret;
	}
	if (id == MIB_IPSECTUNNEL_DELALL) {
		delete_all_linkchain(&ipsecTunnelChain);
		pMib->ipsecTunnelNum= 0;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return 1;
	}
#endif
#endif
#ifdef TLS_CLIENT
	if (id == MIB_CERTROOT_ADD) {
		ret = add_linkchain(&certRootChain, (char *)value);
		if ( ret )
			pMib->certRootNum++;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return ret;
	}
	if (id == MIB_CERTROOT_DEL) {
		ret = delete_linkchain(&certRootChain, (char *)value);
		if ( ret )
			pMib->certRootNum--;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return ret;
	}
	if (id == MIB_CERTROOT_DELALL) {
		delete_all_linkchain(&certRootChain);
		pMib->certRootNum= 0;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return 1;
	}
	if (id == MIB_CERTUSER_ADD) {
		ret = add_linkchain(&certUserChain, (char *)value);
		if ( ret )
			pMib->certUserNum++;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return ret;
	}
	if (id == MIB_CERTUSER_DEL) {
		ret = delete_linkchain(&certUserChain, (char *)value);
		if ( ret )
			pMib->certUserNum--;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return ret;
	}
	if (id == MIB_CERTUSER_DELALL) {
		delete_all_linkchain(&certUserChain);
		pMib->certUserNum= 0;
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif
		return 1;
	}	
#endif
	if ( search_tbl(id, mib_table, &i) ) {
		pMibTbl = (void *)pMib;
		pTbl = mib_table;
	}
	else if ( search_tbl(id, mib_wlan_table, &i) ) {
		pMibTbl = (void *)&pMib->wlan[wlan_idx][vwlan_idx];
		pTbl = mib_wlan_table;
	}
	else if ( search_tbl(id, hwmib_table, &i) ) {
		pMibTbl = (void *)pHwSetting;
		pTbl = hwmib_table;
	}
	else if ( search_tbl(id, hwmib_wlan_table, &i) ) {
		pMibTbl = (void *)&pHwSetting->wlan[wlan_idx];
		pTbl = hwmib_wlan_table;
	}
	else {
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_sem_unlock();
#endif	    
		return 0;	
	}

	switch (pTbl[i].type) {
	case BYTE_T:
//		*((unsigned char *)(((long)pMibTbl) + pTbl[i].offset)) = (unsigned char)(*((int *)value));
		ch = (unsigned char)(*((int *)value));
		memcpy( ((char *)pMibTbl) + pTbl[i].offset, &ch, 1);
		break;

	case WORD_T:
//		*((unsigned short *)(((long)pMibTbl) + pTbl[i].offset)) = (unsigned short)(*((int *)value));
		wd = (unsigned short)(*((int *)value));
		memcpy( ((char *)pMibTbl) + pTbl[i].offset, &wd, 2);
		break;

	case STRING_T:
		if ( strlen(value)+1 > pTbl[i].size )
		{
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    		apmib_sem_unlock();
#endif		
			return 0;
		}
		if (value==NULL || strlen(value)==0)
			*((char *)(((long)pMibTbl) + pTbl[i].offset)) = '\0';
		else
			strcpy((char *)(((long)pMibTbl) + pTbl[i].offset), (char *)value);
		break;

	case BYTE5_T:
		memcpy((unsigned char *)(((long)pMibTbl) + pTbl[i].offset), (unsigned char *)value, 5);
		break;

	case BYTE6_T:
		memcpy((unsigned char *)(((long)pMibTbl) + pTbl[i].offset), (unsigned char *)value, 6);
		break;

	case BYTE13_T:
		memcpy((unsigned char *)(((long)pMibTbl) + pTbl[i].offset), (unsigned char *)value, 13);
		break;

	case DWORD_T:
		dwd = (unsigned long)(*((int *)value));
		memcpy( ((char *)pMibTbl) + pTbl[i].offset, &dwd, 4);
		break;
	case BYTE_ARRAY_T:
		tmp = (unsigned char*) value;
#ifdef VPN_SUPPORT
		if(id == MIB_IPSEC_RSA_FILE){
                        memcpy((unsigned char *)(((long)pMibTbl) + pTbl[i].offset), (unsigned char *)value, MAX_RSA_FILE_LEN);
		}
		else
#endif

#ifdef VOIP_SUPPORT
		if(id == MIB_VOIP_CFG){
			printf("apimb: mib_set MIB_VOIP_CFG\n");

			memcpy((unsigned char *)(((long)pMibTbl) + pTbl[i].offset), (unsigned char *)value, pTbl[i].size);

		}
		else
#endif /*VOIP_SUPPORT*/

		{
#if defined(CONFIG_RTL8196B)

#if defined(CONFIG_RTL8196C) || defined(CONFIG_RTL8198)
				//rtl8196c or rtl8198 platform
			if((id >= MIB_HW_TX_POWER_CCK_A &&  id <=MIB_HW_TX_POWER_DIFF_OFDM))
				max_chan_num = MAX_2G_CHANNEL_NUM_MIB;
			else if((id >= MIB_HW_TX_POWER_5G_HT40_1S_A &&  id <=MIB_HW_TX_POWER_5G_DIFF_OFDM))
				max_chan_num = MAX_5G_CHANNEL_NUM_MIB;
#else
			max_chan_num = (id == MIB_HW_TX_POWER_CCK)? MAX_CCK_CHAN_NUM: MAX_OFDM_CHAN_NUM ;
#endif
			
#else //rtl865XC+rtl8190 platform
				max_chan_num = (id == MIB_HW_TX_POWER_CCK)? MAX_CCK_CHAN_NUM: MAX_OFDM_CHAN_NUM ;
				
#endif

#if defined(CONFIG_RTL8196C) || defined(CONFIG_RTL8198)
			
			if(tmp[0]==2){
				if(tmp[3] == 0xff){ // set one channel value
					memcpy((unsigned char *)(((long)pMibTbl) + pTbl[i].offset + (long)tmp[1] -1), (unsigned char *)(tmp+2), 1);
				}
			}else{
					memcpy((unsigned char *)(((long)pMibTbl) + pTbl[i].offset), (unsigned char *)(value+1), max_chan_num);
				}
				
#else				
			if(tmp[2] == 0xff){ // set one channel value
				memcpy((unsigned char *)(((long)pMibTbl) + pTbl[i].offset + (long)tmp[0] -1), (unsigned char *)(tmp+1), 1);
			}
			else
				memcpy((unsigned char *)(((long)pMibTbl) + pTbl[i].offset), (unsigned char *)value, max_chan_num);	
#endif				
		}
		break;
	case IA_T:
		memcpy((unsigned char *)(((long)pMibTbl) + pTbl[i].offset), (unsigned char *)value,  4);
		break;

	case WLAC_ARRAY_T:
	
#if defined(CONFIG_RTK_MESH) && defined(_MESH_ACL_ENABLE_)
	case MESH_ACL_ARRAY_T:
#endif

	case WDS_ARRAY_T:
	case SCHEDULE_ARRAY_T:
#ifdef HOME_GATEWAY
	case PORTFW_ARRAY_T:
	case IPFILTER_ARRAY_T:
	case PORTFILTER_ARRAY_T:
	case MACFILTER_ARRAY_T:
	case URLFILTER_ARRAY_T:
	case TRIGGERPORT_ARRAY_T:

#if defined(GW_QOS_ENGINE) || defined(QOS_BY_BANDWIDTH)
	case QOS_ARRAY_T:
#endif
#ifdef ROUTE_SUPPORT
	case STATICROUTE_ARRAY_T:
#endif
#endif

#ifdef HOME_GATEWAY
#ifdef VPN_SUPPORT
	case IPSECTUNNEL_ARRAY_T:
#endif
#endif
#ifdef TLS_CLIENT
	case CERTROOT_ARRAY_T:
	case CERTUSER_ARRAY_T:
#endif
	case DHCPRSVDIP_ARRY_T:		
#if defined(VLAN_CONFIG_SUPPORTED)	
	case VLANCONFIG_ARRAY_T:		
#endif
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_sem_unlock();
#endif
		return 0;
	}
	
#if CONFIG_APMIB_SHARED_MEMORY == 1
    apmib_sem_unlock();
#endif

	return 1;
}


////////////////////////////////////////////////////////////////////////////////
int apmib_setDef(int id, void *value)
{
	int ret;
	APMIB_Tp saveMib=pMib;

	pMib = pMibDef;
	ret = apmib_set(id, value);
	pMib = saveMib;
	return ret;
}



////////////////////////////////////////////////////////////////////////////////
/* Update current used MIB into flash in current setting area
 */
int apmib_update(CONFIG_DATA_T type)
{
	int i, len, j, k;
	unsigned char checksum;
	unsigned char *data;

#if CONFIG_APMIB_SHARED_MEMORY == 1
	apmib_sem_lock();
#endif

#if defined(MOVE_OUT_DEFAULT_SETTING_FROM_FLASH)
	if (type & DEFAULT_SETTING) 
		return 1;
#endif		

	if (type & HW_SETTING) {
		data = (char *)pHwSetting;
		checksum = CHECKSUM(data, hsHeader.len-1);
		data[hsHeader.len-1] = checksum;

		if ( flash_write((char *)data, HW_SETTING_OFFSET+sizeof(hsHeader), hsHeader.len)==0 ) {
			printf("write hs MIB failed!\n");
#if CONFIG_APMIB_SHARED_MEMORY == 1
			apmib_sem_unlock();
#endif	
			return 0;
		}
	}

	if ((type & CURRENT_SETTING) || (type & DEFAULT_SETTING)) {
#if !defined(CONFIG_RTL8196C_CLIENT_ONLY)			
		for (j=0; j<NUM_WLAN_INTERFACE; j++) 
			for (k=0; k<(NUM_VWLAN_INTERFACE+1); k++)
		{
			memset( pMib->wlan[j][k].acAddrArray, '\0', MAX_WLAN_AC_NUM*sizeof(MACFILTER_T) );
			for (i=0; i<pMib->wlan[j][k].acNum; i++) {
				get_linkchain(&wlanMacChain[j][k], (void *)&pMib->wlan[j][k].acAddrArray[i], i+1);
			}
			memset( pMib->wlan[j][k].wdsArray, '\0', MAX_WDS_NUM*sizeof(WDS_T) );
			for (i=0; i<pMib->wlan[j][k].wdsNum; i++) {
				get_linkchain(&wdsChain[j][k], (void *)&pMib->wlan[j][k].wdsArray[i], i+1);
			}
		}
#endif	//!defined(CONFIG_RTL8196C_CLIENT_ONLY)

#if defined(CONFIG_RTK_MESH) && defined(_MESH_ACL_ENABLE_)	// below code copy above ACL code
		memset( pMib->meshAclAddrArray, '\0', MAX_MESH_ACL_NUM*sizeof(MACFILTER_T) );
		for (i=0; i<pMib->meshAclNum; i++) {
			get_linkchain(&meshAclChain, (void *)&pMib->meshAclAddrArray[i], i+1);
		}
#endif

		memset( pMib->scheduleRuleArray, '\0', MAX_SCHEDULE_NUM*sizeof(SCHEDULE_T) );
		for (i=0; i<pMib->scheduleRuleNum; i++) {
			get_linkchain(&scheduleRuleChain, (void *)&pMib->scheduleRuleArray[i], i+1);
		}
#ifdef HOME_GATEWAY
		memset( pMib->portFwArray, '\0', MAX_FILTER_NUM*sizeof(PORTFW_T) );
		for (i=0; i<pMib->portFwNum; i++) {
			get_linkchain(&portFwChain, (void *)&pMib->portFwArray[i], i+1);
		}

		memset( pMib->ipFilterArray, '\0', MAX_FILTER_NUM*sizeof(IPFILTER_T) );
		for (i=0; i<pMib->ipFilterNum; i++) {
			get_linkchain(&ipFilterChain, (void *)&pMib->ipFilterArray[i], i+1);
		}
		memset( pMib->portFilterArray, '\0', MAX_FILTER_NUM*sizeof(PORTFILTER_T) );
		for (i=0; i<pMib->portFilterNum; i++) {
			get_linkchain(&portFilterChain, (void *)&pMib->portFilterArray[i], i+1);
		}
		memset( pMib->macFilterArray, '\0', MAX_FILTER_NUM*sizeof(MACFILTER_T) );
		for (i=0; i<pMib->macFilterNum; i++) {
			get_linkchain(&macFilterChain, (void *)&pMib->macFilterArray[i], i+1);
		}
		memset( pMib->urlFilterArray, '\0', MAX_URLFILTER_NUM*sizeof(URLFILTER_T) );
		for (i=0; i<pMib->urlFilterNum; i++) {
			get_linkchain(&urlFilterChain, (void *)&pMib->urlFilterArray[i], i+1);
		}
		memset( pMib->triggerPortArray, '\0', MAX_FILTER_NUM*sizeof(TRIGGERPORT_T) );
		for (i=0; i<pMib->triggerPortNum; i++) {
			get_linkchain(&triggerPortChain, (void *)&pMib->triggerPortArray[i], i+1);
		}
#ifdef GW_QOS_ENGINE
		memset( pMib->qosRuleArray, '\0', MAX_QOS_RULE_NUM*sizeof(QOS_T) );
		for (i=0; i<pMib->qosRuleNum; i++) {
			get_linkchain(&qosChain, (void *)&pMib->qosRuleArray[i], i+1);
		}
#endif

#ifdef QOS_BY_BANDWIDTH
		memset( pMib->qosRuleArray, '\0', MAX_QOS_RULE_NUM*sizeof(IPQOS_T) );
		for (i=0; i<pMib->qosRuleNum; i++) {
			get_linkchain(&qosChain, (void *)&pMib->qosRuleArray[i], i+1);
		}
#endif

#ifdef ROUTE_SUPPORT
		memset( pMib->staticRouteArray, '\0', MAX_ROUTE_NUM*sizeof(STATICROUTE_T) );
		for (i=0; i<pMib->staticRouteNum; i++) {
			get_linkchain(&staticRouteChain, (void *)&pMib->staticRouteArray[i], i+1);
		}
#endif //ROUTE

#endif

#ifdef HOME_GATEWAY
#ifdef VPN_SUPPORT
		memset( pMib->ipsecTunnelArray, '\0', MAX_TUNNEL_NUM*sizeof(IPSECTUNNEL_T) );
		for (i=0; i<pMib->ipsecTunnelNum; i++) {
			get_linkchain(&ipsecTunnelChain, (void *)&pMib->ipsecTunnelArray[i], i+1);
		}
#endif
#endif
#ifdef TLS_CLIENT
		memset( pMib->certRootArray, '\0', MAX_CERTROOT_NUM*sizeof(CERTROOT_T) );
		for (i=0; i<pMib->certRootNum; i++) {
			get_linkchain(&certRootChain, (void *)&pMib->certRootArray[i], i+1);
		}
		memset( pMib->certUserArray, '\0', MAX_CERTUSER_NUM*sizeof(CERTUSER_T) );
		for (i=0; i<pMib->certUserNum; i++) {
			get_linkchain(&certUserChain, (void *)&pMib->certUserArray[i], i+1);
		}		
#endif
		memset(pMib->dhcpRsvdIpArray, '\0', MAX_DHCP_RSVD_IP_NUM*sizeof(DHCPRSVDIP_T));
		for (i=0; i<pMib->dhcpRsvdIpNum; i++) {
			get_linkchain(&dhcpRsvdIpChain, (void *)&pMib->dhcpRsvdIpArray[i], i+1);
		}
#if defined(VLAN_CONFIG_SUPPORTED)		
		memset(pMib->VlanConfigArray, '\0', MAX_IFACE_VLAN_CONFIG*sizeof(VLAN_CONFIG_T));
		for (i=0; i<pMib->VlanConfigNum; i++) {
			get_linkchain(&vlanConfigChain, (void *)&pMib->VlanConfigArray[i], i+1);
		}
#endif		
		if (type & CURRENT_SETTING) {
			data = (unsigned char *)pMib;
			checksum = CHECKSUM(data, csHeader.len-1);
			*(data + csHeader.len - 1) = checksum;
			i = CURRENT_SETTING_OFFSET + sizeof(csHeader);
			len = csHeader.len;
		}
#if !defined(MOVE_OUT_DEFAULT_SETTING_FROM_FLASH)
		else {
			data = (unsigned char *)pMibDef;
			checksum = CHECKSUM(data, dsHeader.len-1);
			*(data + dsHeader.len - 1) = checksum;
			i = DEFAULT_SETTING_OFFSET + sizeof(dsHeader);
			len = dsHeader.len;
		}
#endif
		if ( flash_write((char *)data, i, len)==0 ) {
			printf("Write flash current-setting failed!\n");
#if CONFIG_APMIB_SHARED_MEMORY == 1
			apmib_sem_unlock();
#endif
			return 0;
		}
	}
#if CONFIG_APMIB_SHARED_MEMORY == 1
	apmib_sem_unlock();
#endif

#if 0 //michael
#ifdef WEBS
	if (type & CURRENT_SETTING)
		save_cs_to_file();
#endif		
#endif

	return 1;
}


////////////////////////////////////////////////////////////////////////////////
/* Update default setting MIB into current setting area
 */
int apmib_updateDef(void)
{
	unsigned char *data, checksum;
	PARAM_HEADER_T header;
	int i;

#if CONFIG_APMIB_SHARED_MEMORY == 1
	apmib_sem_lock();
#endif	
	memcpy(header.signature, CURRENT_SETTING_HEADER_TAG, TAG_LEN);
	memcpy(&header.signature[TAG_LEN], &dsHeader.signature[TAG_LEN], SIGNATURE_LEN-TAG_LEN);

	header.len = dsHeader.len;
	data = (unsigned char *)pMibDef;
	checksum = CHECKSUM(data, header.len-1);
	*(data + header.len - 1) = checksum;

	i = CURRENT_SETTING_OFFSET;
	if ( flash_write((char *)&header, i, sizeof(header))==0 ) {
		printf("Write flash current-setting header failed!\n");
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_sem_unlock();
#endif
		return 0;
	}
	i += sizeof(header);

	if ( flash_write((char *)data, i, header.len)==0 ) {
		printf("Write flash current-setting failed!\n");
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_sem_unlock();
#endif
		return 0;
	}

#if CONFIG_APMIB_SHARED_MEMORY == 1
	apmib_sem_unlock();
#endif	
	return 1;
}


////////////////////////////////////////////////////////////////////////////////
/* Update MIB into flash current setting area
 */
int apmib_updateFlash(CONFIG_DATA_T type, char *data, int len, int force, int ver)
{
	unsigned char checksum, checksum1, *ptr=NULL;
	int i, offset=0, curLen, curVer;
	unsigned char *pMibData, *pHdr, tmpBuf[20];

#if CONFIG_APMIB_SHARED_MEMORY == 1
	apmib_sem_lock();
#endif

	if ( type == HW_SETTING ) {
		curLen = hsHeader.len - 1;
		pMibData = (unsigned char *)pHwSetting;
		pHdr = (unsigned char *)&hsHeader;
		i = HW_SETTING_OFFSET;
	}
#if !defined(MOVE_OUT_DEFAULT_SETTING_FROM_FLASH)
	else if ( type == DEFAULT_SETTING ) {
		curLen = dsHeader.len - 1;
		pMibData = (unsigned char *)pMibDef;
		pHdr = (unsigned char *)&dsHeader;
		i = DEFAULT_SETTING_OFFSET;
	}
#else
	else if ( type == DEFAULT_SETTING ) {
		return 1;
	}
#endif
	else  {
		curLen = csHeader.len - 1;
		pMibData = (unsigned char *)pMib;
		pHdr = (unsigned char *)&csHeader;
		i = CURRENT_SETTING_OFFSET;
	}

	if (force==2) { // replace by input mib
		((PARAM_HEADER_Tp)pHdr)->len = len + 1;
		sprintf(tmpBuf, "%02d", ver);
		memcpy(&pHdr[TAG_LEN], tmpBuf, SIGNATURE_LEN-TAG_LEN);
		checksum = CHECKSUM(data, len);
		pMibData = data;
		curLen = len;
	}
	else if (force==1) { // update mib but keep not used mib
		sscanf(&((PARAM_HEADER_Tp)pHdr)->signature[TAG_LEN], "%02d", &curVer);
		if ( curVer < ver ) {
			sprintf(tmpBuf, "%02d", ver);
			memcpy(&((PARAM_HEADER_Tp)pHdr)->signature[TAG_LEN],
					tmpBuf, SIGNATURE_LEN-TAG_LEN);
		}
		checksum = CHECKSUM(data, len);
		if (curLen > len) {
			((PARAM_HEADER_Tp)pHdr)->len = curLen + 1;
			ptr = pMibData + len;
			offset = curLen - len;
			checksum1 = CHECKSUM(ptr, offset);
			checksum +=  checksum1;
		}
		else
			((PARAM_HEADER_Tp)pHdr)->len = len + 1;

		curLen = len;
		pMibData = data;
	}
	else { // keep old mib, only update new added portion
		sscanf(&((PARAM_HEADER_Tp)pHdr)->signature[TAG_LEN], "%02d", &curVer);
		if ( curVer < ver ) {
			sprintf(tmpBuf, "%02d", ver);
			memcpy(&((PARAM_HEADER_Tp)pHdr)->signature[TAG_LEN],
					tmpBuf, SIGNATURE_LEN-TAG_LEN);
		}
		if ( len > curLen ) {
			((PARAM_HEADER_Tp)pHdr)->len = len + 1;
			offset = len - curLen;
			checksum = CHECKSUM(pMibData, curLen);
			ptr = data + curLen;
			checksum1 = CHECKSUM(ptr, offset);
			checksum +=  checksum1;
		}
		else
			checksum = CHECKSUM(pMibData, curLen);
	}

	if ( flash_write((char *)pHdr, i, sizeof(PARAM_HEADER_T))==0 ) {
		printf("Write flash current-setting header failed!\n");
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_sem_unlock();
#endif
		return 0;
	}
	i += sizeof(PARAM_HEADER_T);

	if ( flash_write(pMibData, i, curLen)==0 ) {
		printf("Write flash current-setting failed!\n");
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_sem_unlock();
#endif
		return 0;
	}
	i += curLen;

	if (offset > 0) {
		if ( flash_write((char *)ptr, i, offset)==0 ) {
			printf("Write flash current-setting failed!\n");
#if CONFIG_APMIB_SHARED_MEMORY == 1
			apmib_sem_unlock();
#endif
			return 0;
		}
		i += offset;
	}

	if ( flash_write((char *)&checksum, i, sizeof(checksum))==0 ) {
		printf("Write flash current-setting checksum failed!\n");
#if CONFIG_APMIB_SHARED_MEMORY == 1
		apmib_sem_unlock();
#endif
		return 0;
	}

#if CONFIG_APMIB_SHARED_MEMORY == 1
	apmib_sem_unlock();
#endif	
	return 1;
}

/////////////////////////////////////////////////////////////////////////////////
static int flash_read(char *buf, int offset, int len)
{
	int fh;
	int ok=1;

	fh = open(FLASH_DEVICE_NAME, O_RDWR);
	if ( fh == -1 )
		return 0;

	lseek(fh, offset, SEEK_SET);

	if ( read(fh, buf, len) != len)
		ok = 0;

	close(fh);

	return ok;
}


////////////////////////////////////////////////////////////////////////////////
static int flash_write(char *buf, int offset, int len)
{
	int fh;
	int ok=1;

	fh = open(FLASH_DEVICE_NAME, O_RDWR);

	if ( fh == -1 )
		return 0;

	lseek(fh, offset, SEEK_SET);

	if ( write(fh, buf, len) != len)
		ok = 0;

	close(fh);
	sync();

	return ok;
}


///////////////////////////////////////////////////////////////////////////////
static int init_linkchain(LINKCHAIN_Tp pLinkChain, int size, int num)
{
	FILTER_Tp entry;
	int offset=sizeof(FILTER_Tp)*2;
	char *pBuf;
	int i;

	pLinkChain->realsize = size;

	if (size%4)
		size = (size/4+1)*4;

	pBuf = calloc(num, size+offset);
	if ( pBuf == NULL )
		return 0;

	pLinkChain->buf = pBuf;
	pLinkChain->pUsedList = NULL;
	pLinkChain->pFreeList = NULL;
	entry = (FILTER_Tp)pBuf;

	ADD_LINK_LIST(pLinkChain->pFreeList, entry);
	for (i=1; i<num; i++) {
		entry = (FILTER_Tp)&pBuf[i*(size+offset)];
		ADD_LINK_LIST(pLinkChain->pFreeList, entry);
	}

	pLinkChain->size = size;
	pLinkChain->num = num;
	pLinkChain->usedNum = 0;
	return 1;
}

///////////////////////////////////////////////////////////////////////////////
static int add_linkchain(LINKCHAIN_Tp pLinkChain, char *val)
{
	FILTER_Tp entry;

	// get a free entry
	entry = pLinkChain->pFreeList;
	if (entry == NULL)
		return 0;

	if (entry->next==pLinkChain->pFreeList)
		pLinkChain->pFreeList = NULL;
	else
		pLinkChain->pFreeList = entry->next;

	REMOVE_LINK_LIST(entry);

	// copy content
	memcpy(entry->val, val, pLinkChain->realsize);

	// add to used list
	if (pLinkChain->pUsedList == NULL) {
		ADD_LINK_LIST(pLinkChain->pUsedList, entry);
	}
	else {
		ADD_LINK_LIST(pLinkChain->pUsedList->prev, entry);
	}
	pLinkChain->usedNum++;
	return 1;
}

///////////////////////////////////////////////////////////////////////////////
static int delete_linkchain(LINKCHAIN_Tp pLinkChain, char *val)
{
	FILTER_Tp curEntry=pLinkChain->pUsedList;

	while (curEntry != NULL) {
		if ( !memcmp(curEntry->val,(unsigned char *)val,pLinkChain->compareLen) ) {
				if (curEntry == pLinkChain->pUsedList) {
					if ( pLinkChain->pUsedList->next != pLinkChain->pUsedList )
						pLinkChain->pUsedList = pLinkChain->pUsedList->next;
					else
						pLinkChain->pUsedList = NULL;
				}
				REMOVE_LINK_LIST(curEntry);
				ADD_LINK_LIST(pLinkChain->pFreeList, curEntry);
				pLinkChain->usedNum--;
				return 1;
		}
		if ( curEntry->next == pLinkChain->pUsedList )
		{
			return 0;
		}
		curEntry = curEntry->next;
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
static void delete_all_linkchain(LINKCHAIN_Tp pLinkChain)
{
	FILTER_Tp curEntry;

	if (pLinkChain->pUsedList==NULL)
		return;

	// search for matched mac address
	while (pLinkChain->pUsedList) {
		curEntry = pLinkChain->pUsedList;
		if (pLinkChain->pUsedList->next != pLinkChain->pUsedList)
			pLinkChain->pUsedList = pLinkChain->pUsedList->next;
		else
			pLinkChain->pUsedList = NULL;

		REMOVE_LINK_LIST(curEntry);
		ADD_LINK_LIST(pLinkChain->pFreeList, curEntry);
		pLinkChain->usedNum--;
	}
}

///////////////////////////////////////////////////////////////////////////////
static int get_linkchain(LINKCHAIN_Tp pLinkChain, char *val, int index)
{
	FILTER_Tp curEntry=pLinkChain->pUsedList;

	if ( curEntry == NULL || index > pLinkChain->usedNum)
 		return 0;

	while (--index > 0)
        	curEntry = curEntry->next;
	
	memcpy( (unsigned char *)val, curEntry->val, pLinkChain->realsize);

	return 1;
}

int update_linkchain(int fmt, void *Entry_old, void *Entry_new, int type_size)
{
	LINKCHAIN_Tp pLinkChain=NULL;
	FILTER_Tp curEntry;
	void *entry;
	int i; 
	int entry_cmp;
#ifdef HOME_GATEWAY
	 	if(fmt==PORTFW_ARRAY_T){
			pLinkChain = &portFwChain;	 
		}else if(fmt == IPFILTER_ARRAY_T){
			pLinkChain = &ipFilterChain;
		}else if(fmt == PORTFILTER_ARRAY_T){
			pLinkChain = &portFilterChain;
		}else if(fmt == MACFILTER_ARRAY_T){
			pLinkChain = &macFilterChain;
		}else if(fmt == URLFILTER_ARRAY_T){
			pLinkChain = &urlFilterChain;
		}else	if(fmt==TRIGGERPORT_ARRAY_T){
				pLinkChain = &triggerPortChain;
		}else if(fmt==DHCPRSVDIP_ARRY_T){
			pLinkChain = &dhcpRsvdIpChain;			
		}
#ifdef ROUTE_SUPPORT				
		if(fmt==STATICROUTE_ARRAY_T){
		 	pLinkChain = &staticRouteChain;
		}
#endif			
#else
		 if(fmt==DHCPRSVDIP_ARRY_T){
			pLinkChain = &dhcpRsvdIpChain;	
		}		
#endif			

#if defined(VLAN_CONFIG_SUPPORTED)
		 if(fmt==VLANCONFIG_ARRAY_T){
		 	pLinkChain = &vlanConfigChain;	
		}
#endif	
	curEntry = pLinkChain->pUsedList;
	for(i=0;i<pLinkChain->usedNum;i++){
		entry = curEntry->val;
		entry_cmp=memcmp(entry, Entry_old, type_size );
		if(entry_cmp ==0){
		//	fprintf(stderr,"find the entry to update!\n");
			memcpy(entry, Entry_new, type_size);
			break;
		}
		curEntry = curEntry->next;
	}
	return 1;
}

////////////////////////////////////////////////////////////////////////////////
int save_cs_to_file()
{
	char *buf, *ptr=NULL;
	PARAM_HEADER_Tp pHeader;
	unsigned char checksum;
	int len, fh;
	char tmpBuf[100];

	len = csHeader.len;
#ifdef _LITTLE_ENDIAN_
	//len  = WORD_SWAP(len);
#endif
	len += sizeof(PARAM_HEADER_T);
	buf = malloc(len);
	if ( buf == NULL ) {
		strcpy(tmpBuf, "Allocate buffer failed!");
		return 0;
	}
#ifdef __mips__
	fh = open("/web/config.dat", O_RDWR|O_CREAT|O_TRUNC);
#else
	fh = open("../web/config.dat", O_RDWR|O_CREAT|O_TRUNC);
#endif
	if (fh == -1) {
		printf("Create config file error!\n");
		free(buf);
		return 0;
	}

	pHeader = (PARAM_HEADER_Tp)buf;
	len = pHeader->len = csHeader.len;
	memcpy(&buf[sizeof(PARAM_HEADER_T)], pMib, len-1);

#ifdef _LITTLE_ENDIAN_
#ifdef VOIP_SUPPORT
	// rock: need swap here 
	// 1. write to share space (ex: save setting to config file)
	// 2. read from share space (ex: import config file) 
	pHeader->len  = WORD_SWAP(pHeader->len);
#else
	//pHeader->len  = WORD_SWAP(pHeader->len);
#endif
	swap_mib_word_value((APMIB_Tp)&buf[sizeof(PARAM_HEADER_T)]);
#endif
	memcpy(pHeader->signature, csHeader.signature, SIGNATURE_LEN);
	ptr = (char *)&buf[sizeof(PARAM_HEADER_T)];
	checksum = CHECKSUM(ptr, len-1);
	buf[sizeof(PARAM_HEADER_T)+len-1] = checksum;

	ptr = &buf[sizeof(PARAM_HEADER_T)];
	ENCODE_DATA(ptr, len);

	if ( write(fh, buf, len+sizeof(PARAM_HEADER_T)) != len+sizeof(PARAM_HEADER_T)) {
		printf("Write config file error!\n");
		close(fh);
		free(buf);
		return 0;
	}

	close(fh);
	free(buf);

// added by rock /////////////////////////////////////////
#ifdef VOIP_SUPPORT
	web_voip_saveConfig();
#endif

	return 1;
}
