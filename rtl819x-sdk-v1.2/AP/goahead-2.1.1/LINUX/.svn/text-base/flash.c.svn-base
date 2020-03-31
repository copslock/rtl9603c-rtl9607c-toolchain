/*
 *      Operation routines for FLASH MIB access
 *
 *      Authors: David Hsu	<davidhsu@realtek.com.tw>
 *
 *      $Id: flash.c,v 1.119.2.5 2011/04/29 06:38:24 pluswang Exp $
 *
 */


/* System include files */
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <ctype.h>
#include <regex.h>
#define noPARSE_TXT_FILE

#define WLAN_FAST_INIT
#define BR_SHORTCUT	

/* Local include files */
#include "apmib.h"
#include "mibtbl.h"
#include "../webs.h"

#ifdef CONFIG_RTL_COMAPI_CFGFILE
extern int comapi_initWlan(char *ifname);
extern int dumpCfgFile(char *ifname, struct wifi_mib *pmib);
#endif

/* Constand definitions */
#define DEC_FORMAT	("%d")
#define BYTE5_FORMAT	("%02x%02x%02x%02x%02x")
#define BYTE6_FORMAT	("%02x%02x%02x%02x%02x%02x")
#define BYTE13_FORMAT	("%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x")
#define STR_FORMAT	("%s")
#define HEX_FORMAT	("%02x")
#define SCHEDULE_FORMAT	("%d,%d,%d,%d")
#ifdef HOME_GATEWAY
#define PORTFW_FORMAT	("%s, %d, %d, %d")
#define PORTFILTER_FORMAT ("%d, %d, %d")
#define IPFILTER_FORMAT	("%s, %d")
#define TRIGGERPORT_FORMAT ("%d, %d, %d, %d, %d, %d")
#endif
#define MACFILTER_FORMAT ("%02x%02x%02x%02x%02x%02x")
#define MACFILTER_COLON_FORMAT ("%02x:%02x:%02x:%02x:%02x:%02x")
#define WDS_FORMAT	("%02x%02x%02x%02x%02x%02x,%d")
#define DHCPRSVDIP_FORMAT ("%02x%02x%02x%02x%02x%02x,%s,%s")
#if defined(CONFIG_RTK_VLAN_ROUTETYPE)
#define VLANCONFIG_FORMAT ("%s,%d,%d,%d,%d,%d,%d,%d")
#else
#define VLANCONFIG_FORMAT ("%s,%d,%d,%d,%d,%d,%d")
#endif
#ifdef HOME_GATEWAY
#ifdef VPN_SUPPORT
//#define IPSECTUNNEL_FORMAT ("%d, %d, %s, %d, %s, %d, %d, %s , %d, %s, %d, %d, %d, %d, %d, %d, %s, %d, %d, %d, %lu, %lu, %d, %s, %s, %s")
#define IPSECTUNNEL_FORMAT ("%d, %d, %s, %d, %s, %d, %d, %s , %d, %s, %d, %d,  %d, %d,  %s, %d, %d, %d, %lu, %lu, %d, %s, %s, %s, %d, %s, %s, %d, %d, %s")
#endif
#endif

#ifdef CONFIG_RTL_WAPI_SUPPORT
#define CA_CERT "/var/myca/CA.cert"
#endif

#define SPACE	(' ')
#define EOL	('\n')

static int config_area;

/* Macro definition */
static int _is_hex(char c)
{
    return (((c >= '0') && (c <= '9')) ||
            ((c >= 'A') && (c <= 'F')) ||
            ((c >= 'a') && (c <= 'f')));
}

static int string_to_hex(char *string, unsigned char *key, int len)
{
	char tmpBuf[4];
	int idx, ii=0;
	for (idx=0; idx<len; idx+=2) {
		tmpBuf[0] = string[idx];
		tmpBuf[1] = string[idx+1];
		tmpBuf[2] = 0;
		if ( !_is_hex(tmpBuf[0]) || !_is_hex(tmpBuf[1]))
			return 0;

		key[ii++] = (unsigned char) strtol(tmpBuf, (char**)NULL, 16);
	}
	return 1;
}

static void convert_lower(char *str)
{	int i;
	int len = strlen(str);
	for (i=0; i<len; i++)
		str[i] = tolower(str[i]);
}

static int APMIB_GET(int id, void *val)
{
	if (config_area == 3 || config_area == 4)
		return apmib_getDef(id, val);
	else
		return apmib_get(id, val);
}

static int APMIB_SET(int id, void *val)
{
	if (config_area == 3 || config_area == 4)
		return apmib_setDef(id, val);
	else
		return apmib_set(id, val);
}

/* Local declarations routines */
static int flash_read(char *buf, int offset, int len);
static int writeDefault(int isAll);
static int searchMIB(char *token);
static void getMIB(char *name, int id, TYPE_T type, int num, int array_separate, char **val);
static void setMIB(char *name, int id, TYPE_T type, int len, int valnum, char **val);
static void dumpAll(void);
static void dumpAllHW(void);
static void showHelp(void);
static void showAllMibName(void);
static void showAllHWMibName(void);
static void showSetACHelp(void);

#if defined(CONFIG_RTK_MESH) && defined(_MESH_ACL_ENABLE_)
static void showSetMeshACLHelp(void);
#endif
static void showSetVlanConfigHelp(void);
static void showSetWdsHelp(void);
static int read_flash_webpage(char *prefix, char *file);
#ifdef TLS_CLIENT
static int read_flash_cert(char *prefix, char *certfile);
#endif
#ifdef VPN_SUPPORT
static int read_flash_rsa(char *prefix);
#endif
#ifdef HOME_GATEWAY
static void showSetPortFwHelp(void);
static void showSetPortFilterHelp(void);
static void showSetIpFilterHelp(void);
static void showSetMacFilterHelp(void);
static void showSetUrlFilterHelp(void);
static void showSetTriggerPortHelp(void);
#ifdef GW_QOS_ENGINE
//static void showSetQosHelp(void);
#endif
#ifdef ROUTE_SUPPORT
static void showSetStaticRouteHelp(void);
#endif

#ifdef VPN_SUPPORT
static void showSetIpsecTunnelHelp(void);
#endif
static int generatePPPConf(int is_pppoe, char *option_file, char *pap_file, char *chap_file);
#endif

#ifdef TLS_CLIENT
static void showSetCertRootHelp(void);
static void showSetCertUserHelp(void);
#endif

static void generateWpaConf(char *outputFile, int isWds);

#ifdef WLAN_FAST_INIT
static int initWlan(char *ifname);
#endif

#ifdef WIFI_SIMPLE_CONFIG
static int updateWscConf(char *in, char *out, int genpin);
#endif

#ifdef PARSE_TXT_FILE
static int parseTxtConfig(char *filename, APMIB_Tp pConfig);
static int getToken(char *line, char *value);
static int set_mib(APMIB_Tp pMib, int id, void *value);
static void getVal2(char *value, char **p1, char **p2);
#ifdef HOME_GATEWAY
static void getVal3(char *value, char **p1, char **p2, char **p3);
static void getVal4(char *value, char **p1, char **p2, char **p3, char **p4);
static void getVal5(char *value, char **p1, char **p2, char **p3, char **p4, char **p5);
#endif

static int acNum;

#if defined(CONFIG_RTK_MESH) && defined(_MESH_ACL_ENABLE_)
static int meshAclNum;
#endif

static int wdsNum;

#ifdef HOME_GATEWAY
static int macFilterNum, portFilterNum, ipFilterNum, portFwNum, triggerPortNum, staticRouteNum;
static int urlFilterNum;

#if defined(GW_QOS_ENGINE) || defined(QOS_BY_BANDWIDTH)
static int qosRuleNum;
#endif
#endif
static int dhcpRsvdIpNum;
#ifdef TLS_CLIENT
static int certRootNum, certUserNum ;
#endif
#if defined(VLAN_CONFIG_SUPPORTED)
static int vlanConfigNum=MAX_IFACE_VLAN_CONFIG;	
#endif
static is_wlan_mib=0;

/////////////////////////////////////////////////////////////////////////////////////////
static char __inline__ *getVal(char *value, char **p)
{
	int len=0;

	while (*value == ' ' ) value++;

	*p = value;

	while (*value && *value!=',') {
		value++;
		len++;
	}

	if ( !len ) {
		*p = NULL;
		return NULL;
	}

	if ( *value == 0)
		return NULL;

	*value = 0;
	value++;

	return value;
}
#endif  // PARSE_TXT_FILE

//////////////////////////////////////////////////////////////////////
static char *get_token(char *data, char *token)
{
	char *ptr=data;
	int len=0, idx=0;

	while (*ptr && *ptr != '\n' ) {
		if (*ptr == '=') {
			if (len <= 1)
				return NULL;
			memcpy(token, data, len);

			/* delete ending space */
			for (idx=len-1; idx>=0; idx--) {
				if (token[idx] !=  ' ')
					break;
			}
			token[idx+1] = '\0';
			
			return ptr+1;
		}
		len++;
		ptr++;
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////
static int get_value(char *data, char *value)
{
	char *ptr=data;	
	int len=0, idx, i;

	while (*ptr && *ptr != '\n' && *ptr != '\r') {
		len++;
		ptr++;
	}

	/* delete leading space */
	idx = 0;
	while (len-idx > 0) {
		if (data[idx] != ' ') 
			break;	
		idx++;
	}
	len -= idx;

	/* delete bracing '"' */
	if (data[idx] == '"') {
		for (i=idx+len-1; i>idx; i--) {
			if (data[i] == '"') {
				idx++;
				len = i - idx;
			}
			break;
		}
	}

	if (len > 0) {
		memcpy(value, &data[idx], len);
		value[len] = '\0';
	}
	return len;
}

static int setSystemTime_flash(void)
{
	#include <time.h>		
	int cur_year=0;
	//int time_mode=0;
	struct tm tm_time;
	time_t tm;
	FILE *fp;
	char *month_index[]={"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	char week_date[20], month[20], date[20], year[20], tmp1[20];
	int i;
	char buffer[200];
	regex_t re;
	regmatch_t match[2];
	int status;

	fp = fopen("/proc/version", "r");
	if (!fp) {
		printf("Read /proc/version failed!\n");
	}else
		{
			fgets(buffer, sizeof(buffer), fp);
			fclose(fp);
		}

		if (regcomp(&re, "#[0-9][0-9]* \\(.*\\)$", 0) == 0)
		{
			status = regexec(&re, buffer, 2, match, 0);
			regfree(&re);
			if (status == 0 &&
				match[1].rm_so >= 0)
			{
				buffer[match[1].rm_eo] = 0;
   				
			}
		}
	//Mon Nov 10 16:42:19 CST 2008
	
	memset(&tm_time, 0 , sizeof(tm_time));
	sscanf(&buffer[match[1].rm_so],"%s %s %s %d:%d:%d %s %s", week_date, month, date, &(tm_time.tm_hour), &(tm_time.tm_min), &(tm_time.tm_sec), tmp1, year);
	tm_time.tm_isdst = -1;  /* Be sure to recheck dst. */
		for(i=0;i< 12;i++){
			if(strcmp(month_index[i], month)==0){
				tm_time.tm_mon = i;
				break;
			}
		}
		tm_time.tm_year = atoi(year) - 1900;
		tm_time.tm_mday =atoi(date);
		tm = mktime(&tm_time);
		if(tm < 0){
			fprintf(stderr, "set Time Error for tm!");
		}
		if(stime(&tm) < 0){
			fprintf(stderr, "set system Time Error");
		}
	
	if ( !apmib_init()) {
		printf("Initialize AP MIB failed in setSystemTime_flash!\n");
		return -1;
	}
	//apmib_get( MIB_NTP_ENABLED, (void *)&time_mode);

	//if(time_mode == 0)
	{		
		apmib_get( MIB_TIME_YEAR, (void *)&cur_year);

		if(cur_year != 0){
			tm_time.tm_year = cur_year - 1900;
			apmib_get( MIB_TIME_MONTH, (void *)&(tm_time.tm_mon));
			apmib_get( MIB_TIME_DAY, (void *)&(tm_time.tm_mday));
			apmib_get( MIB_TIME_HOUR, (void *)&(tm_time.tm_hour));
			apmib_get( MIB_TIME_MIN, (void *)&(tm_time.tm_min));
			apmib_get( MIB_TIME_SEC, (void *)&(tm_time.tm_sec));
			tm = mktime(&tm_time);
			if(tm < 0){
				fprintf(stderr, "make Time Error!\n");
			}
			if(stime(&tm) < 0){
				fprintf(stderr, "set system Time Error\n");
			}
		}
	}

	system("echo done > /var/system/set_time");
	return 0;	
}
//////////////////////////////////////////////////////////////////////
static void readFileSetParam(char *file)
{
	FILE *fp;
	char line[200], token[40], value[100], *ptr;
	int idx, hw_setting_found=0, ds_setting_found=0, cs_setting_found=0;
	char *arrayval[2];
	mib_table_entry_T *pTbl;

	
	if ( !apmib_init()) {
		printf("Initialize AP MIB failed!\n");
		return;
	}

	fp = fopen(file, "r");
	if (fp == NULL) {
		printf("read file [%s] failed!\n", file);
		return;
	}

	arrayval[0] = value;

	while ( fgets(line, 200, fp) ) {
		ptr = get_token(line, token);
		if (ptr == NULL)
			continue;
		if (get_value(ptr, value)==0)
			continue;

		idx = searchMIB(token);
		if ( idx == -1 ) {
			printf("invalid param [%s]!\n", token);
			return;
		}
		if (config_area == 1 || config_area == 2) {
			hw_setting_found = 1;
			if (config_area == 1)
				pTbl = hwmib_table;
			else
				pTbl = hwmib_wlan_table;
		}
		else if (config_area == 3 || config_area == 4) {
			ds_setting_found = 1;
			if (config_area == 3)
				pTbl = mib_table;
			else
				pTbl = mib_wlan_table;
		}
		else {
			cs_setting_found = 1;
			if (config_area == 5)
				pTbl = mib_table;
			else
				pTbl = mib_wlan_table;
		}
		config_area = 0;
		setMIB(token, pTbl[idx].id, pTbl[idx].type, pTbl[idx].size, 1, arrayval);
	}
	fclose(fp);

	if (hw_setting_found)
		apmib_update(HW_SETTING);
	if (ds_setting_found)
		apmib_update(DEFAULT_SETTING);
	if (cs_setting_found)
		apmib_update(CURRENT_SETTING);
}

//////////////////////////////////////////////////////////////////////
static int resetDefault()
{
	char *defMib;
	int fh;

	fh = open(FLASH_DEVICE_NAME, O_RDWR);

#if CONFIG_APMIB_SHARED_MEMORY == 1	
    apmib_sem_lock();
#endif

	if ((defMib=apmib_dsconf()) == NULL) {
		printf("Default configuration invalid!\n");
#if CONFIG_APMIB_SHARED_MEMORY == 1	
        apmib_sem_unlock();
#endif
		return -1;
	}

	memcpy( dsHeader.signature, CURRENT_SETTING_HEADER_TAG, TAG_LEN);

	lseek(fh, CURRENT_SETTING_OFFSET, SEEK_SET);

	if ( write(fh, (const void *)&dsHeader, sizeof(dsHeader))!=sizeof(dsHeader) ) {
		printf("write cs header failed!\n");
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_shm_free(defMib, DSCONF_SHM_KEY);
	    apmib_sem_unlock();
#else
		free(defMib);
#endif
		return -1;
	}
	if ( write(fh, (const void *)defMib, dsHeader.len) != dsHeader.len ) {
		printf("write cs MIB failed!\n");
#if CONFIG_APMIB_SHARED_MEMORY == 1
	    apmib_shm_free(defMib, DSCONF_SHM_KEY);
	    apmib_sem_unlock();
#else
		free(defMib);
#endif
		return -1;
	}
	close(fh);
	sync();

#if CONFIG_APMIB_SHARED_MEMORY == 1
    apmib_load_csconf(); //read current settings diectly from flash 
	apmib_shm_free(defMib, DSCONF_SHM_KEY);
	apmib_sem_unlock();
#else
	free(defMib);
#endif

	return 0;
}



//////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
	int argNum=1, action=0, idx, num, valNum=0;
	char mib[100]={0}, valueArray[170][100], *value[170], *ptr;
#ifdef PARSE_TXT_FILE
	char filename[100]={0};
	APMIB_T apmib;
#endif
	mib_table_entry_T *pTbl=NULL;

	if ( argc > 1 ) {
#ifdef PARSE_TXT_FILE
		if ( !strcmp(argv[argNum], "-f") ) {
			if (++argNum < argc)
				sscanf(argv[argNum++], "%s", filename);
		}
#endif
		if ( !strcmp(argv[argNum], "get") ) {
			action = 1;
			if (++argNum < argc) {
				if (argc > 3 && !memcmp(argv[argNum], "wlan", 4)) {
					int idx;
#ifdef MBSSID
					if (strlen(argv[argNum]) >= 9 && argv[argNum][5] == '-' &&
						argv[argNum][6] == 'v' && argv[argNum][7] == 'a') {
						idx = atoi(&argv[argNum][8]);
						if (idx >= NUM_VWLAN_INTERFACE) {
							printf("invalid virtual wlan interface index number!\n");
							return 0;
						}
						vwlan_idx = idx+1;
					}
#ifdef UNIVERSAL_REPEATER
				if (strlen(argv[argNum]) >= 9 && argv[argNum][5] == '-' &&
						!memcmp(&argv[argNum][6], "vxd", 3)) {
					vwlan_idx = NUM_VWLAN_INTERFACE;
				}
#endif									
#endif
					idx = atoi(&argv[argNum++][4]);
					if (idx >= NUM_WLAN_INTERFACE) {
						printf("invalid wlan interface index number!\n");
						goto normal_return;
					}
					wlan_idx = idx;
				}
				sscanf(argv[argNum], "%s", mib);
				while (++argNum < argc) {
					sscanf(argv[argNum], "%s", valueArray[valNum]);
					value[valNum] = valueArray[valNum];
					valNum++;
				}
				value[valNum]= NULL;
			}
		}
		else if ( !strcmp(argv[argNum], "set") ) {
			action = 2;
			if (++argNum < argc) {
				if (argc > 4 && !memcmp(argv[argNum], "wlan", 4)) {
					int idx;
#ifdef MBSSID
					if (strlen(argv[argNum]) >= 9 && argv[argNum][5] == '-' &&
						argv[argNum][6] == 'v' && argv[argNum][7] == 'a') {
						idx = atoi(&argv[argNum][8]);
						if (idx >= NUM_VWLAN_INTERFACE) {
							printf("invalid virtual wlan interface index number!\n");
							return 0;
						}
						vwlan_idx = idx+1;
					}
#ifdef UNIVERSAL_REPEATER
				if (strlen(argv[argNum]) >= 9 && argv[argNum][5] == '-' &&
						!memcmp(&argv[argNum][6], "vxd", 3)) {
					vwlan_idx = NUM_VWLAN_INTERFACE;
				}
#endif									
#endif
					idx = atoi(&argv[argNum++][4]);
					if (idx >= NUM_WLAN_INTERFACE) {
						printf("invalid wlan interface index number!\n");
						goto normal_return;
					}
					wlan_idx = idx;
				}
				sscanf(argv[argNum], "%s", mib);
				int SettingSSID = 0;
				if( !strcmp(mib , "WLAN0_SSID") ||
					!strcmp(mib , "WLAN0_WSC_SSID") ||
					!strcmp(mib , "WLAN0_VAP0_SSID") ||
					!strcmp(mib , "WLAN0_VAP1_SSID") ||
					!strcmp(mib , "WLAN0_VAP2_SSID") ||
					!strcmp(mib , "WLAN0_VAP3_SSID") ||	
					!strcmp(mib , "WLAN0_VAP4_SSID") ||	
					!strcmp(mib , "REPEATER_SSID1") ||	
					!strcmp(mib , "REPEATER_SSID2") ||
					!strcmp(mib , "NTP_TIMEZONE") 
					){
					
					SettingSSID = 1;
				}
				
				while (++argNum < argc) {
					if(SettingSSID == 1){						

						//memcpy(valueArray[valNum] , argv[argNum] , strlen(argv[argNum]));
						strcpy(valueArray[valNum] , argv[argNum] );
						value[valNum] = valueArray[valNum];
						valNum++;
						break;
					}					
					sscanf(argv[argNum], "%s", valueArray[valNum]);
					value[valNum] = valueArray[valNum];
					valNum++;
				}
				value[valNum]= NULL;
			}
		}
		else if ( !strcmp(argv[argNum], "all") ) {
			action = 3;
		}
		else if ( !strcmp(argv[argNum], "default") ) {
			return writeDefault(1);
		}
		else if ( !strcmp(argv[argNum], "default-sw") ) {
			return writeDefault(0);
		}
		else if ( !strcmp(argv[argNum], "reset") ) {
			return resetDefault();
		}
		else if ( !strcmp(argv[argNum], "extr") ) {
			if (++argNum < argc) {
				char prefix[20], file[20]={0};
				int ret;
				sscanf(argv[argNum], "%s", prefix);
				if (++argNum < argc)
					sscanf(argv[argNum], "%s", file);
				ret  = read_flash_webpage(prefix, file);
				if (ret == 0) // success
					unlink(file);
				return ret;
			}
			printf("Usage: %s web prefix\n", argv[0]);
			return -1;
		}
#ifdef TLS_CLIENT
		else if ( !strcmp(argv[argNum], "cert") ) {
			if (++argNum < argc) {
				char prefix[20], file[20]={0};
				int ret;
				sscanf(argv[argNum], "%s", prefix);
				if (++argNum < argc)
					sscanf(argv[argNum], "%s", file);
				ret  = read_flash_cert(prefix,file);
				//if (ret == 0) // success
				//	unlink(file);
				return ret;
			}
			printf("Usage: %s cert prefix\n", argv[0]);
			return -1;
		}
#endif
#ifdef VPN_SUPPORT
		else if ( !strcmp(argv[argNum], "rsa") ) {
			if (++argNum < argc) {
				char prefix[20], file[20]={0};
				int ret;
				sscanf(argv[argNum], "%s", prefix);
				if (++argNum < argc)
					sscanf(argv[argNum], "%s", file);
				ret  = read_flash_rsa(prefix);
				//if (ret == 0) // success
				//	unlink(file);
				return ret;
			}
			printf("Usage: %s rsa prefix\n", argv[0]);
			return -1;
		}
#endif
		else if ( !strcmp(argv[argNum], "test-hwconf") ) {
#if CONFIG_APMIB_SHARED_MEMORY == 1	
            apmib_sem_lock();
#endif
			if ((ptr=apmib_hwconf()) == NULL) {
#if CONFIG_APMIB_SHARED_MEMORY == 1	
                apmib_sem_unlock();
#endif
				return -1;
			}
#if CONFIG_APMIB_SHARED_MEMORY == 1
		    apmib_shm_free(ptr, HWCONF_SHM_KEY);
		    apmib_sem_unlock();
#else
			free(ptr);
#endif
			return 0;
		}
		else if ( !strcmp(argv[argNum], "test-dsconf") ) {
#if CONFIG_APMIB_SHARED_MEMORY == 1	
            apmib_sem_lock();
#endif
			if ((ptr=apmib_dsconf()) == NULL) {
#if CONFIG_APMIB_SHARED_MEMORY == 1	
                apmib_sem_unlock();
#endif
				return -1;
			}
#if CONFIG_APMIB_SHARED_MEMORY == 1
		    apmib_shm_free(ptr, DSCONF_SHM_KEY);
		    apmib_sem_unlock();
#else
			free(ptr);
#endif
			return 0;
		}
		else if ( !strcmp(argv[argNum], "test-csconf") ) {
#if CONFIG_APMIB_SHARED_MEMORY == 1	
            apmib_sem_lock();
#endif
			if ((ptr=apmib_csconf()) == NULL) {
#if CONFIG_APMIB_SHARED_MEMORY == 1	
                apmib_sem_unlock();
#endif
				return -1;
			}
#if CONFIG_APMIB_SHARED_MEMORY == 1
		    apmib_shm_free(ptr, CSCONF_SHM_KEY);
		    apmib_sem_unlock();
#else
			free(ptr);
#endif
			return 0;
		}
		else if ( !strcmp(argv[argNum], "wpa") ) {
			int isWds = 0;
			if ((argNum+2) < argc) {
				if (memcmp(argv[++argNum], "wlan", 4)) {
					printf("Miss wlan_interface argument!\n");
					return 0;
				}
#ifdef MBSSID
				if (strlen(argv[argNum]) >= 9 && argv[argNum][5] == '-' &&
						argv[argNum][6] == 'v' && argv[argNum][7] == 'a') {
					idx = atoi(&argv[argNum][8]);
					if (idx >= NUM_VWLAN_INTERFACE) {
						printf("invalid virtual wlan interface index number!\n");
						return 0;
					}
					vwlan_idx = idx+1;
				}
#ifdef UNIVERSAL_REPEATER
				if (strlen(argv[argNum]) >= 9 && argv[argNum][5] == '-' &&
						!memcmp(&argv[argNum][6], "vxd", 3)) {
					vwlan_idx = NUM_VWLAN_INTERFACE;
				}
#endif				
#endif
				wlan_idx = atoi(&argv[argNum][4]);
				if (wlan_idx >= NUM_WLAN_INTERFACE) {
					printf("invalid wlan interface index number!\n");
					goto normal_return;
				}

#ifdef CONFIG_RTK_MESH
				if ((argNum+2) < argc)
				{
					if( !strcmp(argv[argNum+2], "wds")) 
						isWds = 1;
					else if(!strcmp(argv[argNum+2], "msh"))
						isWds = 7;
				}
#else
				if (((argc-1) > (argNum+1)) && !strcmp(argv[argNum+2], "wds")) 
					isWds = 1;
#endif // CONFIG_RTK_MESH

				generateWpaConf(argv[argNum+1], isWds);
				goto normal_return;
			}
			else {
				printf("Miss arguments [wlan_interface config_filename]!\n");
				return 0;
			}
		}
		// set flash parameters by reading from file
		else if ( !strcmp(argv[argNum], "-param_file") ) {
			if ((argNum+2) < argc) {
				if (memcmp(argv[++argNum], "wlan", 4)) {
					printf("Miss wlan_interface argument!\n");
					return 0;
				}
#ifdef MBSSID
				if (strlen(argv[argNum]) >= 9 && argv[argNum][5] == '-' &&
						argv[argNum][6] == 'v' && argv[argNum][7] == 'a') {
					idx = atoi(&argv[argNum][8]);
					if (idx >= NUM_VWLAN_INTERFACE) {
						printf("invalid virtual wlan interface index number!\n");
						return 0;
					}
					vwlan_idx = idx+1;
				}
#ifdef UNIVERSAL_REPEATER
				if (strlen(argv[argNum]) >= 9 && argv[argNum][5] == '-' &&
						!memcmp(&argv[argNum][6], "vxd", 3)) {
					vwlan_idx = NUM_VWLAN_INTERFACE;
				}
#endif				
#endif
				wlan_idx = atoi(&argv[argNum][4]);
				if (wlan_idx >= NUM_WLAN_INTERFACE) {
					printf("invalid wlan interface index number!\n");
					goto normal_return;
				}
				readFileSetParam(argv[argNum+1]);
			}
			else
				printf("Miss arguments [wlan_interface param_file]!\n");
			return 0;
		}
// added by rock /////////////////////////////////////////
#ifdef VOIP_SUPPORT
		else if ( !strcmp(argv[argNum], "size") ) {
			printf("Flash Setting Size = %d, MIB Size = %d\n",
				CURRENT_SETTING_OFFSET - DEFAULT_SETTING_OFFSET, sizeof(*pMib));
			return 0;
		}
		else if (strcmp(argv[argNum], "voip") == 0) {

			if (argc >= 2) // have voip param
				return flash_voip_cmd(argc - 2, &argv[2]);
			else
				return flash_voip_cmd(0, NULL);
		}
#endif		
#ifdef CONFIG_RTL_WAPI_SUPPORT
		else if(0==strcmp(argv[argNum],"wapi-check"))
		{
			/*if WAPI ,check need to init CA*/
			 struct stat status;
			int init;
			if ( !apmib_init()) {
				printf("Initialize AP MIB failed!\n");
				return -1;
			}
			apmib_get(MIB_WLAN_WAPI_CA_INIT,(void *)&init);
			if(init)
				return 0;
			/*check if CA.cert exists. since the defauts maybe load*/
			if (stat(CA_CERT, &status) < 0)
			{
				system("initCAFiles.sh");
				init=1;
				if(!apmib_set(MIB_WLAN_WAPI_CA_INIT,(void *)&init))
					printf("set MIB_WLAN_WAPI_CA_INIT error\n");
				apmib_update(CURRENT_SETTING);
			}
			return 0;
		}
#else	
	else if(0==strcmp(argv[argNum],"wapi-check"))
	{
		//printf("WAPI Support Not Enabled\n");
		return 0;
	}
#endif
#ifdef WLAN_FAST_INIT
		else if ( !strcmp(argv[argNum], "set_mib") ) {
			if ((argNum+1) < argc) {
				if (memcmp(argv[++argNum], "wlan", 4)) {
					printf("Miss wlan_interface argument!\n");
					return -1;
				}
#ifdef MBSSID
				if (strlen(argv[argNum]) >= 9 && argv[argNum][5] == '-' &&
						argv[argNum][6] == 'v' && argv[argNum][7] == 'a') {
					idx = atoi(&argv[argNum][8]);
					if (idx >= NUM_VWLAN_INTERFACE) {
						printf("invalid virtual wlan interface index number!\n");
						return 0;
					}
					vwlan_idx = idx+1;
				}
#ifdef UNIVERSAL_REPEATER
				if (strlen(argv[argNum]) >= 9 && argv[argNum][5] == '-' &&
						!memcmp(&argv[argNum][6], "vxd", 3)) {
					vwlan_idx = NUM_VWLAN_INTERFACE;
				}
#endif
#endif
				wlan_idx = atoi(&argv[argNum][4]);
				if (wlan_idx >= NUM_WLAN_INTERFACE) {
					printf("invalid wlan interface index number!\n");
					goto error_return;
				}
				
#ifdef MBSSID
				int ret;
  #ifdef CONFIG_RTL_COMAPI_CFGFILE
				ret = comapi_initWlan(argv[argNum]);
  #else
				ret = initWlan(argv[argNum]);
  #endif 
				vwlan_idx=0;
				return ret;
#else
  				return initWlan(argv[argNum]);
#endif

			}
			else
				printf("Miss arguments [wlan_interface]!\n");
			return -1;
		}
#endif
#ifdef HOME_GATEWAY
		else if ( !strcmp(argv[argNum], "gen-pppoe") ) { // generate pppoe config file
			if ((argNum+3) < argc) {
				return generatePPPConf(1, argv[argNum+1], argv[argNum+2], argv[argNum+3]);
			}
			else {
				printf("Miss arguments [option-file pap-file chap-file]!\n");
				return -1;
			}
		}
		else if ( !strcmp(argv[argNum], "gen-pptp") ) { // generate pptp config file
			if ((argNum+3) < argc) {
				return generatePPPConf(0, argv[argNum+1], argv[argNum+2], argv[argNum+3]);
			}
			else {
				printf("Miss arguments [option-file pap-file chap-file]!\n");
				return -1;
			}
		}
#endif		

	else if ( !strcmp(argv[argNum], "settime") ) {
			return setSystemTime_flash();			
		}


#ifdef WIFI_SIMPLE_CONFIG
		else if ( !strcmp(argv[argNum], "upd-wsc-conf") ) {
			return updateWscConf(argv[argNum+1], argv[argNum+2], 0);			
		}
#ifdef CONFIG_RTL_COMAPI_CFGFILE
        else if ( !strcmp(argv[argNum], "def-wsc-conf") ) {
			return defaultWscConf(argv[argNum+1], argv[argNum+2], 0);			
		}
#endif
		else if ( !strcmp(argv[argNum], "gen-pin") ) {
			return updateWscConf(0, 0, 1);			
		}		
#endif // WIFI_SIMPLE_CONFIG
	else if ( !strcmp(argv[argNum], "gethw") ) {
			action = 4;
			if (++argNum < argc) {
				if (argc > 3 && !memcmp(argv[argNum], "wlan", 4)) {
					int idx;
					idx = atoi(&argv[argNum++][4]);
					if (idx >= NUM_WLAN_INTERFACE) {
						printf("invalid wlan interface index number!\n");
						return 0;
					}
					wlan_idx = idx;
				}
				sscanf(argv[argNum], "%s", mib);
				while (++argNum < argc) {
					sscanf(argv[argNum], "%s", valueArray[valNum]);
					value[valNum] = valueArray[valNum];
					valNum++;
				}
				value[valNum]= NULL;
			}
		}else if ( !strcmp(argv[argNum], "sethw") ) {
			action = 5;
			if (++argNum < argc) {
				if (argc > 4 && !memcmp(argv[argNum], "wlan", 4)) {
					int idx;
					idx = atoi(&argv[argNum++][4]);
					if (idx >= NUM_WLAN_INTERFACE) {
						printf("invalid wlan interface index number!\n");
						return 0;
					}
					wlan_idx = idx;
				}
				sscanf(argv[argNum], "%s", mib);
				while (++argNum < argc) {
					sscanf(argv[argNum], "%s", valueArray[valNum]);
					value[valNum] = valueArray[valNum];
					valNum++;
				}
				value[valNum]= NULL;
			}
		}else if ( !strcmp(argv[argNum], "allhw") ) {
			action = 6;
		}
	}

	if ( action == 0) {
		showHelp();
		goto error_return;
	}
	if ( (action==1 && !mib[0]) ||
	     (action==2 && !mib[0]) ) {
		showAllMibName();
		goto error_return;
	}

	if ( action==2 && (!strcmp(mib, "MACAC_ADDR") || !strcmp(mib, "DEF_MACAC_ADDR"))) {
		if (!valNum || (strcmp(value[0], "add") && strcmp(value[0], "del") && strcmp(value[0], "delall"))) {
			showSetACHelp();
			goto error_return;
		}
		if ( (!strcmp(value[0], "del") && !value[1]) || (!strcmp(value[0], "add") && !value[1]) ) {
			showSetACHelp();
			goto error_return;
		}
	}

#if defined(CONFIG_RTK_MESH) && defined(_MESH_ACL_ENABLE_) // below code copy above ACL code
	if ( action==2 && (!strcmp(mib, "MESH_ACL_ADDR") || !strcmp(mib, "DEF_MESH_ACL_ADDR"))) {
		if (!valNum || (strcmp(value[0], "add") && strcmp(value[0], "del") && strcmp(value[0], "delall"))) {
			showSetMeshACLHelp();
			goto error_return;
		}
		if ( (!strcmp(value[0], "del") && !value[1]) || (!strcmp(value[0], "add") && !value[1]) ) {
			showSetMeshACLHelp();
			goto error_return;
		}
	}
#endif
#if defined(VLAN_CONFIG_SUPPORTED)

	if ( action==2 && (!strcmp(mib, "VLANCONFIG_TBL") || !strcmp(mib, "DEF_VLANCONFIG_TBL")) ) {
		if (!valNum || (strcmp(value[0], "add") && strcmp(value[0], "del") && strcmp(value[0], "delall"))) {
			showSetVlanConfigHelp();
			goto error_return;
		}
	}
#endif	
	

	if ( action==2 && (!strcmp(mib, "WDS") || !strcmp(mib, "DEF_WDS"))) {
		if (!valNum || (strcmp(value[0], "add") && strcmp(value[0], "del") && strcmp(value[0], "delall"))) {
			showSetWdsHelp();
			goto error_return;
		}
		if ( (!strcmp(value[0], "del") && !value[1]) || (!strcmp(value[0], "add") && !value[1]) ) {
			showSetWdsHelp();
			goto error_return;
		}
	}

#ifdef HOME_GATEWAY
	if ( action==2 && (!strcmp(mib, "PORTFW_TBL") || !strcmp(mib, "DEF_PORTFW_TBL"))) {
		if (!valNum || (strcmp(value[0], "add") && strcmp(value[0], "del") && strcmp(value[0], "delall"))) {
			showSetPortFwHelp();
			goto error_return;
		}
	}
	if ( action==2 && (!strcmp(mib, "PORTFILTER_TBL") || !strcmp(mib, "DEF_PORTFILTER_TBL")) ) {
		if (!valNum || (strcmp(value[0], "add") && strcmp(value[0], "del") && strcmp(value[0], "delall"))) {
			showSetPortFilterHelp();
			goto error_return;
		}
	}
	if ( action==2 && (!strcmp(mib, "IPFILTER_TBL") || !strcmp(mib, "DEF_IPFILTER_TBL"))) {
		if (!valNum || (strcmp(value[0], "add") && strcmp(value[0], "del") && strcmp(value[0], "delall"))) {
			showSetIpFilterHelp();
			goto error_return;
		}
	}
	if ( action==2 && (!strcmp(mib, "MACFILTER_TBL") || !strcmp(mib, "DEF_MACFILTER_TBL")) ) {
		if (!valNum || (strcmp(value[0], "add") && strcmp(value[0], "del") && strcmp(value[0], "delall"))) {
			showSetMacFilterHelp();
			goto error_return;
		}
	}
	if ( action==2 && (!strcmp(mib, "URLFILTER_TBL") || !strcmp(mib, "DEF_URLFILTER_TBL")) ) {
		if (!valNum || (strcmp(value[0], "add") && strcmp(value[0], "del") && strcmp(value[0], "delall"))) {
			showSetUrlFilterHelp();
			goto error_return;
		}
	}
	if ( action==2 && (!strcmp(mib, "TRIGGERPORT_TBL") || !strcmp(mib, "DEF_TRIGGERPORT_TBL")) ) {
		if (!valNum || (strcmp(value[0], "add") && strcmp(value[0], "del") && strcmp(value[0], "delall"))) {
			showSetTriggerPortHelp();
			goto error_return;
		}
	}
#ifdef ROUTE_SUPPORT
	if ( action==2 && (!strcmp(mib, "STATICROUTE_TBL") || !strcmp(mib, "DEF_STATICROUTE_TBL")) ) {
		if (!valNum || (strcmp(value[0], "add") && strcmp(value[0], "del") && strcmp(value[0], "delall"))) {
			showSetStaticRouteHelp();
			goto error_return;
		}
	}
#endif //ROUTE
#endif
#ifdef HOME_GATEWAY
#ifdef VPN_SUPPORT
	if ( action==2 && (!strcmp(mib, "IPSECTUNNEL_TBL") || !strcmp(mib, "DEF_IPSECTUNNEL_TBL")) ) {
		if (!valNum || (strcmp(value[0], "add") && strcmp(value[0], "del") && strcmp(value[0], "delall"))) {
			showSetIpsecTunnelHelp();
			goto error_return;
		}
	}
#endif
#endif
#ifdef TLS_CLIENT
	if ( action==2 && (!strcmp(mib, "CERTROOT_TBL") || !strcmp(mib, "DEF_CERTROOT_TBL")) ) {
		if (!valNum || (strcmp(value[0], "add") && strcmp(value[0], "del") && strcmp(value[0], "delall"))) {
			showSetCertRootHelp();
			goto error_return;
		}
	}
	if ( action==2 && (!strcmp(mib, "CERTUSER_TBL") || !strcmp(mib, "DEF_CERTUSER_TBL")) ) {
		if (!valNum || (strcmp(value[0], "add") && strcmp(value[0], "del") && strcmp(value[0], "delall"))) {
			showSetCertUserHelp();
			goto error_return;
		}
	}	
#endif

	switch (action) {
	case 1: // get

#ifdef PARSE_TXT_FILE
		if ( filename[0] ) {
			if ( parseTxtConfig(filename, &apmib) < 0) {
				printf("Parse text file error!\n");
				goto error_return;
			}

			if ( !apmib_init(&apmib)) {
				printf("Initialize AP MIB failed!\n");
				goto error_return;
			}
		}
		else

#endif
		if ( !apmib_init()) {
			printf("Initialize AP MIB failed!\n");
			goto error_return;
		}

		idx = searchMIB(mib);

		if ( idx == -1 ) {
			showHelp();
			showAllMibName();
			goto error_return;
		}
		num = 1;

		if (config_area == 4 || config_area == 6) { // wlan default or current
			if (mib_wlan_table[idx].id == MIB_WLAN_AC_ADDR)
				APMIB_GET(MIB_WLAN_AC_NUM, (void *)&num);
			else if (mib_wlan_table[idx].id == MIB_WLAN_WDS)
				APMIB_GET(MIB_WLAN_WDS_NUM, (void *)&num);
		}
#if defined(CONFIG_RTK_MESH) && defined(_MESH_ACL_ENABLE_) // below code copy above ACL code
#if 0
		else if (config_area == 5) {	// mib_table
			if (mib_table[idx].id == MIB_MESH_ACL_ADDR)
			{				
				APMIB_GET(MIB_MESH_ACL_NUM, (void *)&num);
			}
			
		}
#else
		else if (mib_table[idx].id == MIB_MESH_ACL_ADDR)
			APMIB_GET(MIB_MESH_ACL_NUM, (void *)&num);		
#endif

#endif
#ifdef HOME_GATEWAY
		else if (!strcmp(mib, "PORTFW_TBL"))
			APMIB_GET(MIB_PORTFW_NUM, (void *)&num);
		else if (!strcmp(mib, "PORTFILTER_TBL"))
			APMIB_GET(MIB_PORTFILTER_NUM, (void *)&num);
		else if (!strcmp(mib, "IPFILTER_TBL"))
			APMIB_GET(MIB_IPFILTER_NUM, (void *)&num);
		else if (!strcmp(mib, "MACFILTER_TBL"))
			APMIB_GET(MIB_MACFILTER_NUM, (void *)&num);
		else if (!strcmp(mib, "URLFILTER_TBL"))
			APMIB_GET(MIB_URLFILTER_NUM, (void *)&num);
		else if (!strcmp(mib, "TRIGGERPORT_TBL"))
			APMIB_GET(MIB_TRIGGERPORT_NUM, (void *)&num);

#if defined(GW_QOS_ENGINE) || defined(QOS_BY_BANDWIDTH)
		else if (!strcmp(mib, "QOS_RULE_TBL"))
			APMIB_GET(MIB_QOS_RULE_NUM, (void *)&num);
#endif

#ifdef ROUTE_SUPPORT
		else if (!strcmp(mib, "STATICROUTE_TBL"))
			APMIB_GET(MIB_STATICROUTE_NUM, (void *)&num);
#endif //ROUTE
#endif

#ifdef HOME_GATEWAY
#ifdef VPN_SUPPORT
		else if (!strcmp(mib, "IPSECTUNNEL_TBL"))
			APMIB_GET(MIB_IPSECTUNNEL_NUM, (void *)&num);
#endif
#endif

#ifdef TLS_CLIENT
		else if (!strcmp(mib, "CERTROOT_TBL"))
			APMIB_GET(MIB_CERTROOT_NUM, (void *)&num);
		else if (!strcmp(mib, "CERTUSER_TBL"))
			APMIB_GET(MIB_CERTUSER_NUM, (void *)&num);			
#endif
		else if (!strcmp(mib, "DHCPRSVDIP_TBL"))
			APMIB_GET(MIB_DHCPRSVDIP_NUM, (void *)&num);
#if defined(VLAN_CONFIG_SUPPORTED)
		else if (!strcmp(mib, "VLANCONFIG_TBL"))
		{
			APMIB_GET(MIB_VLANCONFIG_NUM, (void *)&num);
		}
#endif

		if (config_area == 1)
			pTbl = hwmib_table;
		else if (config_area == 2)
			pTbl = hwmib_wlan_table;
		else if (config_area == 3  || config_area == 5)
			pTbl = mib_table;
		else
			pTbl = mib_wlan_table;

		getMIB(mib, pTbl[idx].id, pTbl[idx].type, num, 1 ,value);
		break;

	case 2: // set
		if ( !apmib_init()) {
			printf("Initialize AP MIB failed!\n");
			goto error_return;
		}
		idx = searchMIB(mib);
		if ( idx == -1 ) {
			showHelp();
			showAllMibName();
			goto error_return;
		}
		if ( valNum < 1) {
			showHelp();
			goto error_return;
		}
		if (config_area == 1)
			pTbl = hwmib_table;
		else if (config_area == 2)
			pTbl = hwmib_wlan_table;
		else if (config_area == 3  || config_area == 5)
			pTbl = mib_table;
		else
			pTbl = mib_wlan_table;
		setMIB(mib, pTbl[idx].id, pTbl[idx].type, pTbl[idx].size, valNum, value);
		break;

	case 3: // all
		dumpAll();
		break;
	case 4: // gethw
		if ( !apmib_init_HW()) {
			printf("Initialize AP HW MIB failed!\n");
			return -1;
		}
		idx = searchMIB(mib);
		if ( idx == -1 ) {
			showHelp();
			showAllHWMibName();
			return -1;
		}
		num = 1;
		if (config_area == 1)
			pTbl = hwmib_table;
		else if (config_area == 2)
			pTbl = hwmib_wlan_table;
		getMIB(mib, pTbl[idx].id, pTbl[idx].type, num, 1 ,value);
		break;
		
		case 5: // sethw
		if ( !apmib_init_HW()) {
			printf("Initialize AP MIB failed!\n");
			return -1;
		}
		idx = searchMIB(mib);
		if ( idx == -1 ) {
			showHelp();
			showAllHWMibName();
			return -1;
		}
		if ( valNum < 1) {
			showHelp();
			return -1;
		}
		if (config_area == 1)
			pTbl = hwmib_table;
		else if (config_area == 2)
			pTbl = hwmib_wlan_table;
		setMIB(mib, pTbl[idx].id, pTbl[idx].type, pTbl[idx].size, valNum, value);
		break;
		case 6: // allhw
		dumpAllHW();
		break;	
	}

normal_return:
	vwlan_idx = 0;

	return 0;

error_return:
	vwlan_idx = 0;
	
	return -1;
}

//////////////////////////////////////////////////////////////////////////////////

static unsigned char getWLAN_ChipVersion()
{
	FILE *stream;
	typedef enum { CHIP_UNKNOWN=0, CHIP_RTL8188C=1, CHIP_RTL8192C=2} CHIP_VERSION_T;
	CHIP_VERSION_T chipVersion = CHIP_UNKNOWN;	
	
	stream = fopen ( "/proc/wlan0/mib_rf", "r" );
	if ( stream != NULL )
	{		
		char *strtmp;
		char line[100];
								 
		while (fgets(line, sizeof(line), stream))
		{
			
			strtmp = line;
			while(*strtmp == ' ')
			{
				strtmp++;
			}
			

			if(strstr(strtmp,"RTL8192SE") != 0)
			{
				chipVersion = CHIP_UNKNOWN;
			}
			else if(strstr(strtmp,"RTL8188C") != 0)
			{
				chipVersion = CHIP_RTL8188C;
			}
			else if(strstr(strtmp,"RTL8192C") != 0)
			{
				chipVersion = CHIP_RTL8192C;
			}
		}			
		fclose ( stream );
	}

	return chipVersion;


}

static int writeDefault(int isAll)
{	PARAM_HEADER_T header;
	APMIB_T mib;
	APMIB_Tp pMib=&mib;
	HW_SETTING_T hwmib;
	char *data;
	int status, fh, offset, i, idx;
	unsigned char checksum;
//	unsigned char buff[sizeof(APMIB_T)+sizeof(checksum)+1];
	unsigned char *buff;
#ifdef VLAN_CONFIG_SUPPORTED
	int vlan_entry=0;
#endif
        unsigned char	chipVersion = 0;

	buff=calloc(1, 0x6000);
	if ( buff == NULL ) {
		printf("Allocate buffer failed!\n");
		return -1;
	}

#ifdef __mips__
	fh = open(FLASH_DEVICE_NAME, O_RDWR);
#endif

#ifdef __i386__
	fh = open(FLASH_DEVICE_NAME, O_RDWR|O_CREAT|O_TRUNC);
	write(fh, buff, 0x6000);
#endif
	if ( fh == -1 ) {
		printf("create file failed!\n");
		return -1;
	}

	if (isAll) {
		// write hw setting
		sprintf(header.signature, "%s%02d", HW_SETTING_HEADER_TAG, HW_SETTING_VER);
		header.len = sizeof(hwmib) + sizeof(checksum);

		memset((char *)&hwmib, '\0', sizeof(hwmib));
		hwmib.boardVer = 1;
#if defined(CONFIG_RTL8196B)
	#if defined(CONFIG_RTL8198)
		memcpy(hwmib.nic0Addr, "\x0\xe0\x4c\x81\x96\xc1", 6);
		memcpy(hwmib.nic1Addr, "\x0\xe0\x4c\x81\x96", 6);
		hwmib.nic1Addr[5] = 0xc1 + NUM_WLAN_MULTIPLE_SSID;
	#elif defined(CONFIG_RTL8196C)
		memcpy(hwmib.nic0Addr, "\x0\xe0\x4c\x81\x96\xc1", 6);
		memcpy(hwmib.nic1Addr, "\x0\xe0\x4c\x81\x96", 6);
		hwmib.nic1Addr[5] = 0xc1 + NUM_WLAN_MULTIPLE_SSID;
	#else
		memcpy(hwmib.nic0Addr, "\x0\xe0\x4c\x81\x96\xb1", 6);
		memcpy(hwmib.nic1Addr, "\x0\xe0\x4c\x81\x96", 6);
		hwmib.nic1Addr[5] = 0xb1 + NUM_WLAN_MULTIPLE_SSID;
	#endif
#else
//!CONFIG_RTL8196B =>rtl8651c+rtl8190
		memcpy(hwmib.nic0Addr, "\x0\xe0\x4c\x86\x51\xd1", 6);
		memcpy(hwmib.nic1Addr, "\x0\xe0\x4c\x86\x51", 6);
		hwmib.nic1Addr[5] = 0xd1 + NUM_WLAN_MULTIPLE_SSID;		
#endif	
		// set RF parameters
		for (idx=0; idx<NUM_WLAN_INTERFACE; idx++) {
#if defined(CONFIG_RTL8196B)
	#if defined(CONFIG_RTL8198)
		memcpy(hwmib.wlan[idx].macAddr, "\x0\xe0\x4c\x81\x96", 5);		
		hwmib.wlan[idx].macAddr[5] = 0xc1 + idx;			
		
		memcpy(hwmib.wlan[idx].macAddr1, "\x0\xe0\x4c\x81\x96", 5);		
		hwmib.wlan[idx].macAddr1[5] = 0xc1 + 1;	
		
		memcpy(hwmib.wlan[idx].macAddr2, "\x0\xe0\x4c\x81\x96", 5);		
		hwmib.wlan[idx].macAddr2[5] = 0xc1 + 2;	
		
		memcpy(hwmib.wlan[idx].macAddr3, "\x0\xe0\x4c\x81\x96", 5);		
		hwmib.wlan[idx].macAddr3[5] = 0xc1 + 3;	
		
		memcpy(hwmib.wlan[idx].macAddr4, "\x0\xe0\x4c\x81\x96", 5);		
		hwmib.wlan[idx].macAddr4[5] = 0xc1 + 4;			

		memcpy(hwmib.wlan[idx].macAddr5, "\x0\xe0\x4c\x81\x96", 5);		
		hwmib.wlan[idx].macAddr5[5] = 0xc1 + 5;	
		
		memcpy(hwmib.wlan[idx].macAddr6, "\x0\xe0\x4c\x81\x96", 5);		
		hwmib.wlan[idx].macAddr6[5] = 0xc1 + 6;	
		
		memcpy(hwmib.wlan[idx].macAddr7, "\x0\xe0\x4c\x81\x96", 5);		
		hwmib.wlan[idx].macAddr7[5] = 0xc1 + 7;		
		hwmib.wlan[idx].regDomain = FCC;
		hwmib.wlan[idx].rfType = 10;
		hwmib.wlan[idx].xCap = 0;
		hwmib.wlan[idx].Ther = 0;
		hwmib.wlan[idx].trswitch = 1;
		
		for (i=0; i<MAX_2G_CHANNEL_NUM_MIB; i++)
			hwmib.wlan[idx].pwrlevelCCK_A[i] = 0;
			
		for (i=0; i<MAX_2G_CHANNEL_NUM_MIB; i++)
			hwmib.wlan[idx].pwrlevelCCK_B[i] = 0;
			
		for (i=0; i<MAX_2G_CHANNEL_NUM_MIB; i++)
			hwmib.wlan[idx].pwrlevelHT40_1S_A[i] = 0;	
			
		for (i=0; i<MAX_2G_CHANNEL_NUM_MIB; i++)
			hwmib.wlan[idx].pwrlevelHT40_1S_B[i] = 0;	
			
		for (i=0; i<MAX_2G_CHANNEL_NUM_MIB; i++)
			hwmib.wlan[idx].pwrdiffHT40_2S[i] = 0;	
			
		for (i=0; i<MAX_2G_CHANNEL_NUM_MIB; i++)
			hwmib.wlan[idx].pwrdiffHT20[i] = 0;	
			
		for (i=0; i<MAX_2G_CHANNEL_NUM_MIB; i++)
			hwmib.wlan[idx].pwrdiffOFDM[i] = 0;	
			
		hwmib.wlan[idx].TSSI1 = 0;
		hwmib.wlan[idx].TSSI2 = 0;
		for (i=0; i<MAX_5G_CHANNEL_NUM_MIB; i++)
			hwmib.wlan[idx].pwrlevel5GHT40_1S_A[i] = 0;
		for (i=0; i<MAX_5G_CHANNEL_NUM_MIB; i++)
			hwmib.wlan[idx].pwrlevel5GHT40_1S_B[i] = 0;
		for (i=0; i<MAX_5G_CHANNEL_NUM_MIB; i++)
			hwmib.wlan[idx].pwrdiff5GHT40_2S[i] = 0;		
		for (i=0; i<MAX_5G_CHANNEL_NUM_MIB; i++)
			hwmib.wlan[idx].pwrdiff5GHT20[i] = 0;
		for (i=0; i<MAX_5G_CHANNEL_NUM_MIB; i++)
			hwmib.wlan[idx].pwrdiff5GOFDM[i] = 0;	
	#elif defined(CONFIG_RTL8196C)
		memcpy(hwmib.wlan[idx].macAddr, "\x0\xe0\x4c\x81\x96", 5);		
		hwmib.wlan[idx].macAddr[5] = 0xc1 + idx;			
		
		memcpy(hwmib.wlan[idx].macAddr1, "\x0\xe0\x4c\x81\x96", 5);		
		hwmib.wlan[idx].macAddr1[5] = 0xc1 + 1;	
		
		memcpy(hwmib.wlan[idx].macAddr2, "\x0\xe0\x4c\x81\x96", 5);		
		hwmib.wlan[idx].macAddr2[5] = 0xc1 + 2;	
		
		memcpy(hwmib.wlan[idx].macAddr3, "\x0\xe0\x4c\x81\x96", 5);		
		hwmib.wlan[idx].macAddr3[5] = 0xc1 + 3;	
		
		memcpy(hwmib.wlan[idx].macAddr4, "\x0\xe0\x4c\x81\x96", 5);		
		hwmib.wlan[idx].macAddr4[5] = 0xc1 + 4;			

		memcpy(hwmib.wlan[idx].macAddr5, "\x0\xe0\x4c\x81\x96", 5);		
		hwmib.wlan[idx].macAddr5[5] = 0xc1 + 5;	
		
		memcpy(hwmib.wlan[idx].macAddr6, "\x0\xe0\x4c\x81\x96", 5);		
		hwmib.wlan[idx].macAddr6[5] = 0xc1 + 6;	
		
		memcpy(hwmib.wlan[idx].macAddr7, "\x0\xe0\x4c\x81\x96", 5);		
		hwmib.wlan[idx].macAddr7[5] = 0xc1 + 7;		
		hwmib.wlan[idx].regDomain = FCC;
		hwmib.wlan[idx].rfType = 10;
		hwmib.wlan[idx].xCap = 0;
		hwmib.wlan[idx].Ther = 0;
		hwmib.wlan[idx].trswitch = 0;
		
		for (i=0; i<MAX_2G_CHANNEL_NUM_MIB; i++)
			hwmib.wlan[idx].pwrlevelCCK_A[i] = 0;
			
		for (i=0; i<MAX_2G_CHANNEL_NUM_MIB; i++)
			hwmib.wlan[idx].pwrlevelCCK_B[i] = 0;
			
		for (i=0; i<MAX_2G_CHANNEL_NUM_MIB; i++)
			hwmib.wlan[idx].pwrlevelHT40_1S_A[i] = 0;	
			
		for (i=0; i<MAX_2G_CHANNEL_NUM_MIB; i++)
			hwmib.wlan[idx].pwrlevelHT40_1S_B[i] = 0;	
			
		for (i=0; i<MAX_2G_CHANNEL_NUM_MIB; i++)
			hwmib.wlan[idx].pwrdiffHT40_2S[i] = 0;	
			
		for (i=0; i<MAX_2G_CHANNEL_NUM_MIB; i++)
			hwmib.wlan[idx].pwrdiffHT20[i] = 0;	
			
		for (i=0; i<MAX_2G_CHANNEL_NUM_MIB; i++)
			hwmib.wlan[idx].pwrdiffOFDM[i] = 0;	
			
		hwmib.wlan[idx].TSSI1 = 0;
		hwmib.wlan[idx].TSSI2 = 0;
		for (i=0; i<MAX_5G_CHANNEL_NUM_MIB; i++)
			hwmib.wlan[idx].pwrlevel5GHT40_1S_A[i] = 0;
		for (i=0; i<MAX_5G_CHANNEL_NUM_MIB; i++)
			hwmib.wlan[idx].pwrlevel5GHT40_1S_B[i] = 0;
		for (i=0; i<MAX_5G_CHANNEL_NUM_MIB; i++)
			hwmib.wlan[idx].pwrdiff5GHT40_2S[i] = 0;		
		for (i=0; i<MAX_5G_CHANNEL_NUM_MIB; i++)
			hwmib.wlan[idx].pwrdiff5GHT20[i] = 0;
		for (i=0; i<MAX_5G_CHANNEL_NUM_MIB; i++)
			hwmib.wlan[idx].pwrdiff5GOFDM[i] = 0;	
	#else
	
		memcpy(hwmib.wlan[idx].macAddr, "\x0\xe0\x4c\x81\x96", 5);		
		hwmib.wlan[idx].macAddr[5] = 0xb1 + idx;			
		
		memcpy(hwmib.wlan[idx].macAddr1, "\x0\xe0\x4c\x81\x96", 5);		
		hwmib.wlan[idx].macAddr1[5] = 0xb1 + 1;	
		
		memcpy(hwmib.wlan[idx].macAddr2, "\x0\xe0\x4c\x81\x96", 5);		
		hwmib.wlan[idx].macAddr2[5] = 0xb1 + 2;	
		
		memcpy(hwmib.wlan[idx].macAddr3, "\x0\xe0\x4c\x81\x96", 5);		
		hwmib.wlan[idx].macAddr3[5] = 0xb1 + 3;	
		
		memcpy(hwmib.wlan[idx].macAddr4, "\x0\xe0\x4c\x81\x96", 5);		
		hwmib.wlan[idx].macAddr4[5] = 0xb1 + 4;			

		memcpy(hwmib.wlan[idx].macAddr5, "\x0\xe0\x4c\x81\x96", 5);		
		hwmib.wlan[idx].macAddr5[5] = 0xb1 + 5;	
		
		memcpy(hwmib.wlan[idx].macAddr6, "\x0\xe0\x4c\x81\x96", 5);		
		hwmib.wlan[idx].macAddr6[5] = 0xb1 + 6;	
		
		memcpy(hwmib.wlan[idx].macAddr7, "\x0\xe0\x4c\x81\x96", 5);		
		hwmib.wlan[idx].macAddr7[5] = 0xb1 + 7;		
		hwmib.wlan[idx].regDomain = FCC;
		hwmib.wlan[idx].rfType = 10;
		hwmib.wlan[idx].xCap = 0;
		hwmib.wlan[idx].Ther = 0;
		for (i=0; i<MAX_CCK_CHAN_NUM; i++)
			hwmib.wlan[idx].txPowerCCK[i] = 0;
		for (i=0; i<MAX_OFDM_CHAN_NUM; i++)
			hwmib.wlan[idx].txPowerOFDM_HT_OFDM_1S[i] = 0;
		for (i=0; i<MAX_OFDM_CHAN_NUM; i++)
			hwmib.wlan[idx].txPowerOFDM_HT_OFDM_2S[i] = 0;
		hwmib.wlan[idx].LOFDMPwDiffA = 0;
		hwmib.wlan[idx].LOFDMPwDiffB = 0;
		hwmib.wlan[idx].TSSI1 = 0;
		hwmib.wlan[idx].TSSI2 = 0;

	#endif

	chipVersion = (unsigned char)getWLAN_ChipVersion();
	if(chipVersion== 1 || chipVersion== 2)
		hwmib.wlan[idx].ledType = 11;
	
#else
//!CONFIG_RTL8196B => rtl8651c+rtl8190
			memcpy(hwmib.wlan[idx].macAddr, "\x0\xe0\x4c\x86\x51", 5);		
			hwmib.wlan[idx].macAddr[5] = 0xd1 + idx;		
			memcpy(hwmib.wlan[idx].macAddr1, "\x0\xe0\x4c\x86\x51", 5);		
			hwmib.wlan[idx].macAddr1[5] = 0xd1 + 1;	
			memcpy(hwmib.wlan[idx].macAddr2, "\x0\xe0\x4c\x86\x51", 5);		
			hwmib.wlan[idx].macAddr2[5] = 0xd1 + 2;	
			memcpy(hwmib.wlan[idx].macAddr3, "\x0\xe0\x4c\x86\x51", 5);		
			hwmib.wlan[idx].macAddr3[5] = 0xd1 + 3;	
			memcpy(hwmib.wlan[idx].macAddr4, "\x0\xe0\x4c\x86\x51", 5);		
			hwmib.wlan[idx].macAddr4[5] = 0xd1 + 4;	
			hwmib.wlan[idx].regDomain = FCC;
			//hwmib.wlan[idx].rfType = (unsigned char)RF_ZEBRA;
			hwmib.wlan[idx].rfType = 10;
			hwmib.wlan[idx].antDiversity = (unsigned char)0; // disabled
			hwmib.wlan[idx].txAnt = 0;
			hwmib.wlan[idx].initGain = 4;
			hwmib.wlan[idx].xCap = 0;
			hwmib.wlan[idx].LOFDMPwDiff = 0;
//			hwmib.wlan[idx].AntPwDiff_B = 0;
			hwmib.wlan[idx].AntPwDiff_C = 0;
//			hwmib.wlan[idx].AntPwDiff_D = 0;
			hwmib.wlan[idx].TherRFIC = 0;
//			strcpy(hwmib.wlan[idx].wscPin, "12345670");
			for (i=0; i<MAX_CCK_CHAN_NUM; i++)
				hwmib.wlan[idx].txPowerCCK[i] = 0;
			for (i=0; i<MAX_OFDM_CHAN_NUM; i++)
				hwmib.wlan[idx].txPowerOFDM[i] = 0;
#endif
			}
		data = (char *)&hwmib;
		checksum = CHECKSUM(data, header.len-1);

		lseek(fh, HW_SETTING_OFFSET, SEEK_SET);
		if ( write(fh, (const void *)&header, sizeof(header))!=sizeof(header) ) {
			printf("write hs header failed!\n");
			return -1;
		}
		if ( write(fh, (const void *)&hwmib, sizeof(hwmib))!=sizeof(hwmib) ) {
			printf("write hs MIB failed!\n");
			return -1;
		}
		if ( write(fh, (const void *)&checksum, sizeof(checksum))!=sizeof(checksum) ) {
			printf("write hs checksum failed!\n");
			return -1;
		}

		close(fh);
		sync();
		fh = open(FLASH_DEVICE_NAME, O_RDWR);
	}
	// write default & current setting
	memset(pMib, '\0', sizeof(APMIB_T));
	apmib_default_setting(pMib);

	data = (char *)pMib;
	// write default setting
	sprintf(header.signature, "%s%02d", DEFAULT_SETTING_HEADER_TAG, DEFAULT_SETTING_VER);
	header.len = sizeof(APMIB_T) + sizeof(checksum);
	checksum = CHECKSUM(data, header.len-1);
	
#if !defined(MOVE_OUT_DEFAULT_SETTING_FROM_FLASH)
	
	
	lseek(fh, DEFAULT_SETTING_OFFSET, SEEK_SET);
	if ( write(fh, (const void *)&header, sizeof(header))!=sizeof(header) ) {
		printf("write ds header failed!\n");
		return -1;
	}
	if ( write(fh, (const void *)pMib, sizeof(mib))!=sizeof(mib) ) {
		printf("write ds MIB failed!\n");
		return -1;
	}
	if ( write(fh, (const void *)&checksum, sizeof(checksum))!=sizeof(checksum) ) {
		printf("write ds checksum failed!\n");
		return -1;
	}
	close(fh);
	sync();
	fh = open(FLASH_DEVICE_NAME, O_RDWR);
#endif
	// write current setting
	sprintf(header.signature, "%s%02d", CURRENT_SETTING_HEADER_TAG, CURRENT_SETTING_VER);
	header.len = sizeof(APMIB_T) + sizeof(checksum);
	lseek(fh, CURRENT_SETTING_OFFSET, SEEK_SET);
	if ( write(fh, (const void *)&header, sizeof(header))!=sizeof(header) ) {
		printf("write cs header failed!\n");
		return -1;
	}
	if ( write(fh, (const void *)pMib, sizeof(mib))!=sizeof(mib) ) {
		printf("write cs MIB failed!\n");
		return -1;
	}
	if ( write(fh, (const void *)&checksum, sizeof(checksum))!=sizeof(checksum) ) {
		printf("write cs checksum failed!\n");
		return -1;
	}

	close(fh);
	sync();

	// check if hw, ds, cs checksum is ok
	offset = HW_SETTING_OFFSET;
	if ( flash_read((char *)&header, offset, sizeof(header)) == 0) {
		printf("read hs header failed!\n");
		return -1;
	}
	offset += sizeof(header);
	if ( flash_read(buff, offset, header.len) == 0) {
		printf("read hs MIB failed!\n");
		return -1;
	}
	status = CHECKSUM_OK(buff, header.len);
	if ( !status) {
		printf("hs Checksum error!\n");
		return -1;
	}
	
#if !defined(MOVE_OUT_DEFAULT_SETTING_FROM_FLASH)
	offset = DEFAULT_SETTING_OFFSET;
	if ( flash_read((char *)&header, offset, sizeof(header)) == 0) {
		printf("read ds header failed!\n");
		return -1;
	}
	offset += sizeof(header);
	if ( flash_read(buff, offset, header.len) == 0) {
		printf("read ds MIB failed!\n");
		return -1;
	}
	status = CHECKSUM_OK(buff, header.len);
	if ( !status) {
		printf("ds Checksum error!\n");
		return -1;
	}
#endif

	offset = CURRENT_SETTING_OFFSET;
	if ( flash_read((char *)&header, offset, sizeof(header)) == 0) {
		printf("read cs header failed!\n");
		return -1;
	}
	offset += sizeof(header);
	if ( flash_read(buff, offset, header.len) == 0) {
		printf("read cs MIB failed!\n");
		return -1;
	}
	status = CHECKSUM_OK(buff, header.len);

	if ( !status) {
		printf("cs Checksum error!\n");
		return -1;
	}

	free(buff);

#if CONFIG_APMIB_SHARED_MEMORY == 1	
    apmib_sem_lock();
    apmib_load_hwconf();
    apmib_load_dsconf();
    apmib_load_csconf();
    apmib_sem_unlock();
#endif

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
static int flash_read(char *buf, int offset, int len)
{
	int fh;
	int ok=1;

#ifdef __mips__
	fh = open(FLASH_DEVICE_NAME, O_RDWR);
#endif

#ifdef __i386__
	fh = open(FLASH_DEVICE_NAME, O_RDONLY);
#endif
	if ( fh == -1 )
		return 0;

	lseek(fh, offset, SEEK_SET);

	if ( read(fh, buf, len) != len)
		ok = 0;

	close(fh);

	return ok;
}

///////////////////////////////////////////////////////////////////////////////
static int searchMIB(char *token)
{
	int idx = 0;
	char tmpBuf[100];
	int desired_config=0;

	if (!memcmp(token, "HW_", 3)) {
		config_area = 1;
		if (!memcmp(&token[3], "WLAN", 4) && token[8] == '_') {
			wlan_idx = token[7] - '0';
			if (wlan_idx >= NUM_WLAN_INTERFACE)
				return -1;
			strcpy(tmpBuf, &token[9]);
			desired_config = config_area+1;
		}
		else
			strcpy(tmpBuf, &token[3]);
	}
	else if (!memcmp(token, "DEF_", 4)) {
		config_area = 3;

		if (!memcmp(&token[4], "WLAN", 4) && token[9] == '_') {
			wlan_idx = token[8] - '0';
			if (wlan_idx >= NUM_WLAN_INTERFACE)
				return -1;
#ifdef MBSSID
			if (!memcmp(&token[10], "VAP", 3) && token[14] == '_') {
				vwlan_idx = token[13] - '0';
				if (vwlan_idx >= NUM_VWLAN_INTERFACE) {
					vwlan_idx = 0;
					return -1;
				}
				vwlan_idx += 1;
				strcpy(tmpBuf, &token[15]);
			}
			else
#endif
			strcpy(tmpBuf, &token[10]);
			desired_config = config_area+1;
		}
		else
			strcpy(tmpBuf, &token[4]);
	}
	else {
		config_area = 5;

		if (!memcmp(&token[0], "WLAN", 4) && token[5] == '_') {
			wlan_idx = token[4] - '0';
			if (wlan_idx >= NUM_WLAN_INTERFACE)
				return -1;
#ifdef MBSSID
			if (!memcmp(&token[6], "VAP", 3) && token[10] == '_') {
				vwlan_idx = token[9] - '0';
				if (vwlan_idx >= NUM_VWLAN_INTERFACE) {
					vwlan_idx = 0;
					return -1;
				}
				vwlan_idx += 1;
				strcpy(tmpBuf, &token[11]);
			}
#ifdef UNIVERSAL_REPEATER
			else if (!memcmp(&token[6], "VXD", 3) && token[9] == '_') {
				vwlan_idx = NUM_VWLAN_INTERFACE;
				strcpy(tmpBuf, &token[10]);
			}
#endif
			else
#endif
			strcpy(tmpBuf, &token[6]);
			desired_config = config_area+1;
		}
		else
			strcpy(tmpBuf, &token[0]);
	}

	if ( config_area == 1 ) {
		while (hwmib_table[idx].id) {
			if ( !strcmp(hwmib_table[idx].name, tmpBuf)) {
				if (desired_config && config_area != desired_config)
					return -1;
				return idx;
			}
			idx++;
		}
		idx=0;
		while (hwmib_wlan_table[idx].id) {
			if ( !strcmp(hwmib_wlan_table[idx].name, tmpBuf)) {
				config_area++;
				if (desired_config && config_area != desired_config)
					return -1;
				return idx;
			}
			idx++;
		}
		return -1;
	}
	else {
		while (mib_table[idx].id) {
			if ( !strcmp(mib_table[idx].name, tmpBuf)) {
				if (desired_config && config_area != desired_config)
					return -1;
				return idx;
			}
			idx++;
		}
		idx=0;
		while (mib_wlan_table[idx].id) {
			if ( !strcmp(mib_wlan_table[idx].name, tmpBuf)) {
				config_area++;
				if (desired_config && config_area != desired_config)
					return -1;
				return idx;
			}
			idx++;
		}
		return -1;
	}
}

///////////////////////////////////////////////////////////////////////////////
static void getMIB(char *name, int id, TYPE_T type, int num, int array_separate, char **val)
{
	unsigned char array_val[2048];
	struct in_addr ia_val;
	void *value;
	unsigned char tmpBuf[1024]={0}, *format=NULL, *buf, tmp1[400];
	int int_val, i;
	int index=1, tbl=0;
	char mibName[100]={0};

	if (num ==0)
		goto ret;

	strcat(mibName, name);


getval:
	buf = &tmpBuf[strlen(tmpBuf)];
	switch (type) {
	case BYTE_T:
		value = (void *)&int_val;
		format = DEC_FORMAT;
		break;
	case WORD_T:
		value = (void *)&int_val;
		format = DEC_FORMAT;
		break;
	case IA_T:
		value = (void *)&ia_val;
		format = STR_FORMAT;
		break;
	case BYTE5_T:
		value = (void *)array_val;
		format = BYTE5_FORMAT;
		break;
	case BYTE6_T:
		value = (void *)array_val;
		format = BYTE6_FORMAT;
		break;
	case BYTE13_T:
		value = (void *)array_val;
		format = BYTE13_FORMAT;
		break;

	case STRING_T:
		value = (void *)array_val;
		format = STR_FORMAT;
		break;

	case BYTE_ARRAY_T:
		value = (void *)array_val;
		break;

	case DWORD_T:
		value = (void *)&int_val;
		format = DEC_FORMAT;
		break;

	case WLAC_ARRAY_T:
#if defined(CONFIG_RTK_MESH) && defined(_MESH_ACL_ENABLE_)
	case MESH_ACL_ARRAY_T:
#endif
	case WDS_ARRAY_T:
	case DHCPRSVDIP_ARRY_T:	
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
#ifdef VPN_SUPPORT
	case IPSECTUNNEL_ARRAY_T:
#endif
#endif
#ifdef TLS_CLIENT
	case CERTROOT_ARRAY_T:
	case CERTUSER_ARRAY_T:
#endif

#if defined(VLAN_CONFIG_SUPPORTED)
	case VLANCONFIG_ARRAY_T:
#endif		
		tbl = 1;
		value = (void *)array_val;
		array_val[0] = index++;
		break;
	default: printf("invalid mib!\n"); return;
	}

	if ( !APMIB_GET(id, value)) {
		printf("Get MIB failed!\n");
		return;
	}

	if ( type == IA_T )
		value = inet_ntoa(ia_val);

	if (type == BYTE_T || type == WORD_T)
		sprintf(buf, format, int_val);
	else if ( type == IA_T || type == STRING_T ) {
		sprintf(buf, format, value);
		
		if (type == STRING_T ) {
			unsigned char tmpBuf1[1024];
			int srcIdx, dstIdx;
			for (srcIdx=0, dstIdx=0; buf[srcIdx]; srcIdx++, dstIdx++) {
				if ( buf[srcIdx] == '"' || buf[srcIdx] == '\\' || buf[srcIdx] == '$' || buf[srcIdx] == '`' || buf[srcIdx] == ' ' )
					tmpBuf1[dstIdx++] = '\\';

				tmpBuf1[dstIdx] = buf[srcIdx];
			}
			if (dstIdx != srcIdx) {
				memcpy(buf, tmpBuf1, dstIdx);
				buf[dstIdx] ='\0';
			}
		}
	}
	else if (type == BYTE5_T) {
		sprintf(buf, format, array_val[0],array_val[1],array_val[2],
			array_val[3],array_val[4],array_val[5]);
		convert_lower(buf);
	}
	else if (type == BYTE6_T ) {
		sprintf(buf, format, array_val[0],array_val[1],array_val[2],
			array_val[3],array_val[4],array_val[5],array_val[6]);
		convert_lower(buf);
	}
	else if (type == BYTE13_T) {
		sprintf(buf, format, array_val[0],array_val[1],array_val[2],
			array_val[3],array_val[4],array_val[5],array_val[6],
			array_val[7],array_val[8],array_val[9],array_val[10],
			array_val[11],array_val[12]);
		convert_lower(buf);
	}
	else if(type == BYTE_ARRAY_T ) {
		int max_chan_num=0, chan;
#if defined(CONFIG_RTL8196B)
	#if defined(CONFIG_RTL8198)
		if((id >= MIB_HW_TX_POWER_CCK_A &&  id <=MIB_HW_TX_POWER_DIFF_OFDM))
				max_chan_num = MAX_2G_CHANNEL_NUM_MIB;
			else if((id >= MIB_HW_TX_POWER_5G_HT40_1S_A &&  id <=MIB_HW_TX_POWER_5G_DIFF_OFDM))
				max_chan_num = MAX_5G_CHANNEL_NUM_MIB;
			if(val == NULL || val[0] == NULL){
				for(i=0;i< max_chan_num;i++){
					sprintf(tmp1, "%02x", array_val[i]);
					strcat(buf, tmp1);
				}
				convert_lower(buf);
			}
			else{
				chan = atoi(val[0]);
				if(chan < 1 || chan > max_chan_num){
					printf("invalid channel number\n");
					return;
				}
				sprintf(buf, "%d", *(((unsigned char *)value)+chan-1) );
			}
	#elif defined(CONFIG_RTL8196C)
		if((id >= MIB_HW_TX_POWER_CCK_A &&  id <=MIB_HW_TX_POWER_DIFF_OFDM))
				max_chan_num = MAX_2G_CHANNEL_NUM_MIB;
			else if((id >= MIB_HW_TX_POWER_5G_HT40_1S_A &&  id <=MIB_HW_TX_POWER_5G_DIFF_OFDM))
				max_chan_num = MAX_5G_CHANNEL_NUM_MIB;
			if(val == NULL || val[0] == NULL){
				for(i=0;i< max_chan_num;i++){
					sprintf(tmp1, "%02x", array_val[i]);
					strcat(buf, tmp1);
				}
				convert_lower(buf);
			}
			else{
				chan = atoi(val[0]);
				if(chan < 1 || chan > max_chan_num){
					printf("invalid channel number\n");
					return;
				}
				sprintf(buf, "%d", *(((unsigned char *)value)+chan-1) );
			}
	#else
	
		max_chan_num = (id == MIB_HW_TX_POWER_CCK)? MAX_CCK_CHAN_NUM : MAX_OFDM_CHAN_NUM;
		if(val == NULL || val[0] == NULL){
			for(i=0 ;i< max_chan_num ;i++){
				sprintf(tmp1, "%02x", array_val[i]);
				strcat(buf, tmp1);
			}
			convert_lower(buf);
		}
		else{
			chan = atoi(val[0]);
			if(chan < 1 || chan > max_chan_num){
				if((chan<1) || (id==MIB_HW_TX_POWER_CCK) || ((id==MIB_HW_TX_POWER_OFDM) && (chan>216))){
					printf("invalid channel number\n");
					return;
				}
				else{
					if((chan >= 163) && (chan <= 181))
						chan -= 148;
					else // 182 ~ 216
						chan -= 117;
				}
			}
			sprintf(buf, "%d", *(((unsigned char *)value)+chan-1) );
		}
		
		#endif
		#else
		max_chan_num = (id == MIB_HW_TX_POWER_CCK)? MAX_CCK_CHAN_NUM : MAX_OFDM_CHAN_NUM;
		if(val == NULL || val[0] == NULL){
			for(i=0 ;i< max_chan_num ;i++){
				sprintf(tmp1, "%02x", array_val[i]);
				strcat(buf, tmp1);
			}
			convert_lower(buf);
		}
		else{
			chan = atoi(val[0]);
			if(chan < 1 || chan > max_chan_num){
				if((chan<1) || (id==MIB_HW_TX_POWER_CCK) || ((id==MIB_HW_TX_POWER_OFDM) && (chan>216))){
					printf("invalid channel number\n");
					return;
				}
				else{
					if((chan >= 163) && (chan <= 181))
						chan -= 148;
					else // 182 ~ 216
						chan -= 117;
				}
			}
			sprintf(buf, "%d", *(((unsigned char *)value)+chan-1) );
		}
		#endif
	}
	else if (type == DWORD_T)
		sprintf(buf, format, int_val);

	else if (type == DHCPRSVDIP_ARRY_T) {		
		DHCPRSVDIP_Tp pEntry=(DHCPRSVDIP_Tp)array_val;
		sprintf(buf, DHCPRSVDIP_FORMAT, 			
			pEntry->macAddr[0],pEntry->macAddr[1],pEntry->macAddr[2],
			pEntry->macAddr[3],pEntry->macAddr[4],pEntry->macAddr[5],
			inet_ntoa(*((struct in_addr*)pEntry->ipAddr)), pEntry->hostName);
	}	
#if defined(VLAN_CONFIG_SUPPORTED)	
	else if (type == VLANCONFIG_ARRAY_T) 
	{
		OPMODE_T opmode=-1;
		int isLan=1;
		apmib_get( MIB_OP_MODE, (void *)&opmode);
		
		VLAN_CONFIG_Tp pEntry=(VLAN_CONFIG_Tp)array_val;

		if(strncmp(pEntry->netIface,"eth1",strlen("eth1")) == 0)
		{			
			if(opmode == WISP_MODE || opmode == BRIDGE_MODE)
				isLan=1;
			else
				isLan=0;
		}
		else if(strncmp("wlan0",pEntry->netIface, strlen(pEntry->netIface)) == 0)
		{						
			if(opmode == WISP_MODE)
				isLan=0;
			else
				isLan=1;
		}
		else
		{						
			isLan=1;
		}

		sprintf(buf, VLANCONFIG_FORMAT, 			
			pEntry->netIface,pEntry->enabled,pEntry->tagged,pEntry->priority,pEntry->cfi, pEntry->vlanId,isLan
#if defined(CONFIG_RTK_VLAN_ROUTETYPE)
			,pEntry->forwarding
#endif
			);
	}		
#endif	
	else if (type == SCHEDULE_ARRAY_T) 
	{
		SCHEDULE_Tp pEntry=(SCHEDULE_Tp)array_val;
		sprintf(buf, SCHEDULE_FORMAT, pEntry->eco, pEntry->fTime, pEntry->tTime, pEntry->day);
		if ( strlen(pEntry->text) ) {
			sprintf(tmp1, ",%s", pEntry->text);
			strcat(buf, tmp1);
		}
	}
#ifdef HOME_GATEWAY
	else if (type == PORTFW_ARRAY_T) {
		PORTFW_Tp pEntry=(PORTFW_Tp)array_val;
		sprintf(buf, PORTFW_FORMAT, inet_ntoa(*((struct in_addr*)pEntry->ipAddr)),
			 pEntry->fromPort, pEntry->toPort, pEntry->protoType);
		if ( strlen(pEntry->comment) ) {
			sprintf(tmp1, ", %s", pEntry->comment);
			strcat(buf, tmp1);
		}
	}
	else if (type == PORTFILTER_ARRAY_T) {
		PORTFILTER_Tp pEntry=(PORTFILTER_Tp)array_val;
		sprintf(buf, PORTFILTER_FORMAT,
			 pEntry->fromPort, pEntry->toPort, pEntry->protoType);
		if ( strlen(pEntry->comment) ) {
			sprintf(tmp1, ", %s", pEntry->comment);
			strcat(buf, tmp1);
		}
	}
	else if (type == IPFILTER_ARRAY_T) {
		IPFILTER_Tp pEntry=(IPFILTER_Tp)array_val;
		sprintf(buf, IPFILTER_FORMAT, inet_ntoa(*((struct in_addr*)pEntry->ipAddr)),
			 pEntry->protoType);
		if ( strlen(pEntry->comment) ) {
			sprintf(tmp1, ", %s", pEntry->comment);
			strcat(buf, tmp1);
		}
	}
	else if (type == MACFILTER_ARRAY_T) {
		MACFILTER_Tp pEntry=(MACFILTER_Tp)array_val;
		sprintf(buf, MACFILTER_COLON_FORMAT, pEntry->macAddr[0],pEntry->macAddr[1],pEntry->macAddr[2],
			 pEntry->macAddr[3],pEntry->macAddr[4],pEntry->macAddr[5]);
		if ( strlen(pEntry->comment) ) {
			sprintf(tmp1, ", %s", pEntry->comment);
			strcat(buf, tmp1);
		}
	}
	else if (type == URLFILTER_ARRAY_T) {
		URLFILTER_Tp pEntry=(URLFILTER_Tp)array_val;
		sprintf(buf, STR_FORMAT, pEntry->urlAddr);
		//if ( strlen(pEntry->comment) ) {
		//	sprintf(tmp1, ", %s", pEntry->comment);
		//	strcat(buf, tmp1);
		//}
	}
	else if (type == TRIGGERPORT_ARRAY_T) {
		TRIGGERPORT_Tp pEntry=(TRIGGERPORT_Tp)array_val;
		sprintf(buf, TRIGGERPORT_FORMAT,
			 pEntry->tri_fromPort, pEntry->tri_toPort, pEntry->tri_protoType,
			 pEntry->inc_fromPort, pEntry->inc_toPort, pEntry->inc_protoType);
		if ( strlen(pEntry->comment) ) {
			sprintf(tmp1, ", %s", pEntry->comment);
			strcat(buf, tmp1);
		}
	}
#ifdef GW_QOS_ENGINE
	else if (type == QOS_ARRAY_T) {
		QOS_Tp pEntry=(QOS_Tp)array_val;
            strcpy(tmp1, inet_ntoa(*((struct in_addr*)pEntry->local_ip_start)));
            strcpy(&tmp1[20], inet_ntoa(*((struct in_addr*)pEntry->local_ip_end)));
            strcpy(&tmp1[40], inet_ntoa(*((struct in_addr*)pEntry->remote_ip_start)));
            strcpy(&tmp1[60], inet_ntoa(*((struct in_addr*)pEntry->remote_ip_end)));
		sprintf(buf, QOS_FORMAT, pEntry->enabled,
                      pEntry->priority, pEntry->protocol, 
                      tmp1, &tmp1[20], 
                      pEntry->local_port_start, pEntry->local_port_end, 
                      &tmp1[40], &tmp1[60],
			   pEntry->remote_port_start, pEntry->remote_port_end, pEntry->entry_name);
	}
#endif

#ifdef QOS_BY_BANDWIDTH
	else if (type == QOS_ARRAY_T) {
		IPQOS_Tp pEntry=(IPQOS_Tp)array_val;
            strcpy(tmp1, inet_ntoa(*((struct in_addr*)pEntry->local_ip_start)));
            strcpy(&tmp1[20], inet_ntoa(*((struct in_addr*)pEntry->local_ip_end)));
		sprintf(buf, QOS_FORMAT, pEntry->enabled,
                      pEntry->mac[0],pEntry->mac[1],pEntry->mac[2],pEntry->mac[3],pEntry->mac[4],pEntry->mac[5],
                      pEntry->mode, tmp1, &tmp1[20], 
                      (int)pEntry->bandwidth, (int)pEntry->bandwidth_downlink,
			   pEntry->entry_name);
	}
#endif

#ifdef ROUTE_SUPPORT
	else if (type == STATICROUTE_ARRAY_T) {
		char strIp[20], strMask[20], strGw[20];
		STATICROUTE_Tp pEntry=(STATICROUTE_Tp)array_val;
		strcpy(strIp, inet_ntoa(*((struct in_addr*)pEntry->dstAddr)));
                strcpy(strMask, inet_ntoa(*((struct in_addr*)pEntry->netmask)));
                strcpy(strGw, inet_ntoa(*((struct in_addr*)pEntry->gateway)));
		sprintf(buf, "%s, %s, %s, %d, %d",strIp, strMask, strGw, pEntry->_interface_, pEntry->metric);
	}
#endif //ROUTE
#endif
#ifdef HOME_GATEWAY
#ifdef VPN_SUPPORT
	else if (type == IPSECTUNNEL_ARRAY_T) {
		IPSECTUNNEL_Tp pEntry=(IPSECTUNNEL_Tp)array_val;
		char strLcIp[20], strRtIp[20], strRtGw[20];
		strcpy(strLcIp, inet_ntoa(*((struct in_addr*)pEntry->lc_ipAddr)));
		strcpy(strRtIp, inet_ntoa(*((struct in_addr*)pEntry->rt_ipAddr)));
		strcpy(strRtGw, inet_ntoa(*((struct in_addr*)pEntry->rt_gwAddr)));

		sprintf(buf, IPSECTUNNEL_FORMAT, pEntry->tunnelId, pEntry->enable,  pEntry->connName, 
		pEntry->lcType, strLcIp, pEntry->lc_maskLen, 
		pEntry->rtType, strRtIp, pEntry->rt_maskLen, 
		strRtGw, pEntry->keyMode, 
		pEntry->conType, pEntry->espEncr, pEntry->espAuth, 
		pEntry->psKey, pEntry->ikeEncr, pEntry->ikeAuth, pEntry->ikeKeyGroup, pEntry->ikeLifeTime, 
		pEntry->ipsecLifeTime, pEntry->ipsecPfs, pEntry->spi, pEntry->encrKey, pEntry->authKey, pEntry->authType, pEntry->lcId, pEntry->rtId, pEntry->lcIdType, pEntry->rtIdType, pEntry->rsaKey
		);
	}
#endif
#endif
#ifdef TLS_CLIENT
	else if (type == CERTROOT_ARRAY_T) {
		CERTROOT_Tp pEntry=(CERTROOT_Tp)array_val;
		sprintf(buf, "%s", pEntry->comment);
	}
	else if (type == CERTUSER_ARRAY_T) {
		CERTUSER_Tp pEntry=(CERTUSER_Tp)array_val;
		sprintf(buf, "%s,%s", pEntry->comment, pEntry->pass);
	}	
#endif
	else if (type == WLAC_ARRAY_T) {
		MACFILTER_Tp pEntry=(MACFILTER_Tp)array_val;
		sprintf(buf, MACFILTER_FORMAT, pEntry->macAddr[0],pEntry->macAddr[1],pEntry->macAddr[2],
			 pEntry->macAddr[3],pEntry->macAddr[4],pEntry->macAddr[5]);
		if ( strlen(pEntry->comment) ) {
			sprintf(tmp1, ", %s", pEntry->comment);
			strcat(buf, tmp1);
		}
	}
	
#if defined(CONFIG_RTK_MESH) && defined(_MESH_ACL_ENABLE_) // below code copy above ACL code
	else if (type == MESH_ACL_ARRAY_T) {
		MACFILTER_Tp pEntry=(MACFILTER_Tp)array_val;
		sprintf(buf, MACFILTER_FORMAT, pEntry->macAddr[0],pEntry->macAddr[1],pEntry->macAddr[2],
			 pEntry->macAddr[3],pEntry->macAddr[4],pEntry->macAddr[5]);
		if ( strlen(pEntry->comment) ) {
			sprintf(tmp1, ", %s", pEntry->comment);
			strcat(buf, tmp1);
		}
	}
#endif

	else if (type == WDS_ARRAY_T) {
		WDS_Tp pEntry=(WDS_Tp)array_val;
		sprintf(buf, WDS_FORMAT, pEntry->macAddr[0],pEntry->macAddr[1],pEntry->macAddr[2],
			 pEntry->macAddr[3],pEntry->macAddr[4],pEntry->macAddr[5], pEntry->fixedTxRate);
		if ( strlen(pEntry->comment) ) {
			sprintf(tmp1, ", %s", pEntry->comment);
			strcat(buf, tmp1);
		}
	}
	if (--num > 0) {
		if (!array_separate)
			strcat(tmpBuf, " ");
		else {
			if (tbl){
				if(type == STRING_T)
					printf("%s%d=\"%s\"\n", mibName, index-1, tmpBuf);
				else
					printf("%s%d=%s\n", mibName, index-1, tmpBuf);
			}
			else{
				if(type == STRING_T)
					printf("%s=\"%s\"\n", mibName, tmpBuf);
				else
					printf("%s=%s\n", mibName, tmpBuf);
			}
			tmpBuf[0] = '\0';
		}
		goto getval;
	}
ret:
	if (tbl) {
		if(type == STRING_T)
			printf("%s%d=\"%s\"\n", mibName, index-1, tmpBuf);
		else
			printf("%s%d=%s\n", mibName, index-1, tmpBuf);
	}
	else{
		if(type == STRING_T)
                                printf("%s=\"%s\"\n", mibName, tmpBuf);
		else
			printf("%s=%s\n", mibName, tmpBuf);
	}
}

///////////////////////////////////////////////////////////////////////////////
static void setMIB(char *name, int id, TYPE_T type, int len, int valNum, char **val)
{
	unsigned char key[200];
	struct in_addr ia_val;
	void *value=NULL;
	int int_val, i;
	MACFILTER_T wlAc;	// Use with MESH_ACL
	WDS_T wds;

	SCHEDULE_T SchEntry;
#ifdef HOME_GATEWAY
	PORTFW_T portFw;
	PORTFILTER_T portFilter;
	IPFILTER_T ipFilter;
	MACFILTER_T macFilter;
	URLFILTER_T urlFilter;
	TRIGGERPORT_T triggerPort;

#ifdef GW_QOS_ENGINE
	QOS_T qos;
#endif

#ifdef QOS_BY_BANDWIDTH
	IPQOS_T qos;
#endif

#ifdef ROUTE_SUPPORT
	STATICROUTE_T staticRoute;
#endif
#endif

#ifdef HOME_GATEWAY
#ifdef VPN_SUPPORT
	IPSECTUNNEL_T ipsecTunnel;
#endif
#endif
#ifdef TLS_CLIENT
	CERTROOT_T certRoot;
	CERTUSER_T certUser;
#endif
	DHCPRSVDIP_T dhcpRsvd;
	
#if defined(VLAN_CONFIG_SUPPORTED)	
	VLAN_CONFIG_T vlanConfig_entry;
#endif		
	int entryNum;
	int max_chan_num=0, tx_power_cnt=0;

	switch (type) {
	case BYTE_T:
	case WORD_T:
		int_val = atoi(val[0]);
		value = (void *)&int_val;
		break;

	case IA_T:
		if ( !inet_aton(val[0], &ia_val) ) {
			printf("invalid internet address!\n");
			return;
		}
		value = (void *)&ia_val;
		break;

	case BYTE5_T:
		if ( strlen(val[0])!=10 || !string_to_hex(val[0], key, 10)) {
			printf("invalid value!\n");
			return;
		}
		value = (void *)key;
		break;

	case BYTE6_T:
		if ( strlen(val[0])!=12 || !string_to_hex(val[0], key, 12)) {
			printf("invalid value!\n");
			return;
		}
		value = (void *)key;
		break;

	case BYTE_ARRAY_T:
		#if defined(CONFIG_RTL8196B)
		
		#if defined(CONFIG_RTL8198)
			if(!(id >= MIB_HW_TX_POWER_CCK_A &&  id <=MIB_HW_TX_POWER_DIFF_OFDM) &&
			!(id >= MIB_HW_TX_POWER_5G_HT40_1S_A &&  id <=MIB_HW_TX_POWER_5G_DIFF_OFDM)){
				printf("invalid mib!\n");
				return;
			}
		if((id >= MIB_HW_TX_POWER_CCK_A &&  id <=MIB_HW_TX_POWER_DIFF_OFDM))
			max_chan_num = MAX_2G_CHANNEL_NUM_MIB;
		else if((id >= MIB_HW_TX_POWER_5G_HT40_1S_A &&  id <=MIB_HW_TX_POWER_5G_DIFF_OFDM))
			max_chan_num = MAX_5G_CHANNEL_NUM_MIB;
			
			for (i=0; i<max_chan_num ; i++) {
				if(val[i] == NULL) break;
				if ( !sscanf(val[i], "%d", &int_val) ) {
					printf("invalid value!\n");
					return;
				}
				key[i+1] = (unsigned char)int_val;
				tx_power_cnt ++;
			}	
			if(tx_power_cnt != 1 && tx_power_cnt !=2 && tx_power_cnt != max_chan_num){
				unsigned char key_tmp[200];
				memcpy(key_tmp, key+1, tx_power_cnt);		
				APMIB_GET(id, key+1);
				memcpy(key+1, key_tmp, tx_power_cnt);
			}
			if(tx_power_cnt == 1){
				for(i=1 ; i <= max_chan_num; i++) {
					key[i] = key[1];
				}
			}
		else if(tx_power_cnt ==2){
			//key[1] is channel number to set
			//key[2] is tx power value
			//key[3] is tx power key for check set mode
			if(key[1] < 1 || key[1] > max_chan_num){
				if((key[1]<1) || ((id >= MIB_HW_TX_POWER_CCK_A &&  id <=MIB_HW_TX_POWER_DIFF_OFDM)) ||
					 ((id >= MIB_HW_TX_POWER_5G_HT40_1S_A &&  id <=MIB_HW_TX_POWER_5G_DIFF_OFDM) && (key[1]>216))){
					printf("invalid channel number\n");
					return;
				}
			}
			key[3] = 0xff ;
		}
		key[0] = tx_power_cnt;
	#elif defined(CONFIG_RTL8196C)
		if(!(id >= MIB_HW_TX_POWER_CCK_A &&  id <=MIB_HW_TX_POWER_DIFF_OFDM) &&
			!(id >= MIB_HW_TX_POWER_5G_HT40_1S_A &&  id <=MIB_HW_TX_POWER_5G_DIFF_OFDM)){
				printf("invalid mib!\n");
				return;
			}
		if((id >= MIB_HW_TX_POWER_CCK_A &&  id <=MIB_HW_TX_POWER_DIFF_OFDM))
			max_chan_num = MAX_2G_CHANNEL_NUM_MIB;
		else if((id >= MIB_HW_TX_POWER_5G_HT40_1S_A &&  id <=MIB_HW_TX_POWER_5G_DIFF_OFDM))
			max_chan_num = MAX_5G_CHANNEL_NUM_MIB;
			
			for (i=0; i<max_chan_num ; i++) {
				if(val[i] == NULL) break;
				if ( !sscanf(val[i], "%d", &int_val) ) {
					printf("invalid value!\n");
					return;
				}
				key[i+1] = (unsigned char)int_val;
				tx_power_cnt ++;
			}	
			if(tx_power_cnt != 1 && tx_power_cnt !=2 && tx_power_cnt != max_chan_num){
				unsigned char key_tmp[200];
				memcpy(key_tmp, key+1, tx_power_cnt);		
				APMIB_GET(id, key+1);
				memcpy(key+1, key_tmp, tx_power_cnt);
			}
			if(tx_power_cnt == 1){
				for(i=1 ; i <= max_chan_num; i++) {
					key[i] = key[1];
				}
			}
		else if(tx_power_cnt ==2){
			//key[1] is channel number to set
			//key[2] is tx power value
			//key[3] is tx power key for check set mode
			if(key[1] < 1 || key[1] > max_chan_num){
				if((key[1]<1) || ((id >= MIB_HW_TX_POWER_CCK_A &&  id <=MIB_HW_TX_POWER_DIFF_OFDM)) ||
					 ((id >= MIB_HW_TX_POWER_5G_HT40_1S_A &&  id <=MIB_HW_TX_POWER_5G_DIFF_OFDM) && (key[1]>216))){
					printf("invalid channel number\n");
					return;
				}
			}
			key[3] = 0xff ;
		}
		key[0] = tx_power_cnt;
	#else
		if ( (id!=MIB_HW_TX_POWER_CCK) && (id!=MIB_HW_TX_POWER_OFDM_1S) && (id!=MIB_HW_TX_POWER_OFDM_2S)) {		
			printf("invalid mib!\n");
			return;
		}
		max_chan_num = (id==MIB_HW_TX_POWER_CCK)? MAX_CCK_CHAN_NUM : MAX_OFDM_CHAN_NUM ;
	
		for (i=0; i<max_chan_num ; i++) {
			if(val[i] == NULL) break;
			if ( !sscanf(val[i], "%d", &int_val) ) {
				printf("invalid value!\n");
				return;
			}
			key[i] = (unsigned char)int_val;
			tx_power_cnt ++;
		}		
		if(tx_power_cnt != 1 && tx_power_cnt !=2 && tx_power_cnt != max_chan_num){
			unsigned char key_tmp[170];
			memcpy(key_tmp, key, tx_power_cnt);		
			APMIB_GET(id, key);
			memcpy(key, key_tmp, tx_power_cnt);
		}
		if(tx_power_cnt == 1){
			for(i=1 ; i < max_chan_num; i++) {
				key[i] = key[0];
			}
		}
		else if(tx_power_cnt ==2){
			//key[0] is channel number to set
			//key[1] is tx power value
			if(key[0] < 1 || key[0] > max_chan_num){
				if((key[0]<1) || (id==MIB_HW_TX_POWER_CCK) || ((id==MIB_HW_TX_POWER_OFDM) && (key[0]>216))){
					printf("invalid channel number\n");
					return;
				}
				else{
					if ((key[0] >= 163) && (key[0] <= 181))
						key[0] -= 148;
					else // 182 ~ 216
						key[0] -= 117;
				}
			}
			key[2] = 0xff ;
		}	
		
		#endif	
		#else
				//!CONFIG_RTL8196B => rtl8651c+rtl8190
				if ( (id!=MIB_HW_TX_POWER_CCK) && (id!=MIB_HW_TX_POWER_OFDM)) {		
					printf("invalid mib!\n");
					return;
				}
				max_chan_num = (id==MIB_HW_TX_POWER_CCK)? MAX_CCK_CHAN_NUM : MAX_OFDM_CHAN_NUM ;
				
				
				for (i=0; i<max_chan_num ; i++) {
					if(val[i] == NULL) break;
					if ( !sscanf(val[i], "%d", &int_val) ) {
						printf("invalid value!\n");
						return;
					}
					key[i] = (unsigned char)int_val;
					tx_power_cnt ++;
				}		
				if(tx_power_cnt != 1 && tx_power_cnt !=2 && tx_power_cnt != max_chan_num){
					unsigned char key_tmp[170];
					memcpy(key_tmp, key, tx_power_cnt);		
					APMIB_GET(id, key);
					memcpy(key, key_tmp, tx_power_cnt);
				}
				if(tx_power_cnt == 1){
					for(i=1 ; i < max_chan_num; i++) {
						key[i] = key[0];
					}
				}
				else if(tx_power_cnt ==2){
					//key[0] is channel number to set
					//key[1] is tx power value
					if(key[0] < 1 || key[0] > max_chan_num){
						if((key[0]<1) || (id==MIB_HW_TX_POWER_CCK) || ((id==MIB_HW_TX_POWER_OFDM) && (key[0]>216))){
							printf("invalid channel number\n");
							return;
						}
						else{
							if ((key[0] >= 163) && (key[0] <= 181))
								key[0] -= 148;
							else // 182 ~ 216
								key[0] -= 117;
						}
					}
					key[2] = 0xff ;
				}
		#endif
		value = (void *)key;
		break;
	case DWORD_T:
		int_val = atoi(val[0]);
		value = (void *)&int_val;
		break;

#ifdef HOME_GATEWAY
	case PORTFW_ARRAY_T:
		if ( !strcmp(val[0], "add")) {
			id = MIB_PORTFW_ADD;
			if ( valNum < 5 ) {
				printf("input argument is not enough!\n");
				return;
			}
			if ( !inet_aton(val[1], (struct in_addr *)&portFw.ipAddr)) {
				printf("invalid internet address!\n");
				return;
			}
			portFw.fromPort = atoi(val[2]);
			portFw.toPort = atoi(val[3]);
			portFw.protoType = atoi(val[4]);
			if ( valNum > 5)
				strcpy(portFw.comment, val[5]);
			else
				portFw.comment[0] = '\0';

		}
		else if ( !strcmp(val[0], "del")) {
			id = MIB_PORTFW_DEL;
			if ( valNum < 2 ) {
				printf("input argument is not enough!\n");
				return;
			}
			int_val = atoi(val[1]);
			if ( !APMIB_GET(MIB_PORTFW_NUM, (void *)&entryNum)) {
				printf("Get port forwarding entry number error!");
				return;
			}
			if ( int_val > entryNum ) {
				printf("Element number is too large!\n");
				return;
			}
			*((char *)&portFw) = (char)int_val;
			if ( !APMIB_GET(MIB_PORTFW, (void *)&portFw)) {
				printf("Get table entry error!");
				return;
			}
		}
		else if ( !strcmp(val[0], "delall"))
			id = MIB_PORTFW_DELALL;

		value = (void *)&portFw;
		break;

	case PORTFILTER_ARRAY_T:
		if ( !strcmp(val[0], "add")) {
			id = MIB_PORTFILTER_ADD;
			if ( valNum < 4 ) {
				printf("input argument is not enough!\n");
				return;
			}
			portFilter.fromPort = atoi(val[1]);
			portFilter.toPort = atoi(val[2]);
			portFilter.protoType = atoi(val[3]);
			if ( valNum > 4)
				strcpy(portFilter.comment, val[4]);
			else
				portFilter.comment[0] = '\0';

		}
		else if ( !strcmp(val[0], "del")) {
			id = MIB_PORTFILTER_DEL;
			if ( valNum < 2 ) {
				printf("input argument is not enough!\n");
				return;
			}
			int_val = atoi(val[1]);
			if ( !APMIB_GET(MIB_PORTFILTER_NUM, (void *)&entryNum)) {
				printf("Get port filter entry number error!");
				return;
			}
			if ( int_val > entryNum ) {
				printf("Element number is too large!\n");
				return;
			}
			*((char *)&portFilter) = (char)int_val;
			if ( !APMIB_GET(MIB_PORTFILTER, (void *)&portFilter)) {
				printf("Get table entry error!");
				return;
			}
		}
		else if ( !strcmp(val[0], "delall"))
			id = MIB_PORTFILTER_DELALL;

		value = (void *)&portFilter;
		break;

	case IPFILTER_ARRAY_T:
		if ( !strcmp(val[0], "add")) {
			id = MIB_IPFILTER_ADD;
			if ( valNum < 3 ) {
				printf("input argument is not enough!\n");
				return;
			}
			if ( !inet_aton(val[1], (struct in_addr *)&ipFilter.ipAddr)) {
				printf("invalid internet address!\n");
				return;
			}
			ipFilter.protoType = atoi(val[2]);
			if ( valNum > 3)
				strcpy(ipFilter.comment, val[3]);
			else
				ipFilter.comment[0] = '\0';

		}
		else if ( !strcmp(val[0], "del")) {
			id = MIB_IPFILTER_DEL;
			if ( valNum < 2 ) {
				printf("input argument is not enough!\n");
				return;
			}
			int_val = atoi(val[1]);
			if ( !APMIB_GET(MIB_IPFILTER_NUM, (void *)&entryNum)) {
				printf("Get port forwarding entry number error!");
				return;
			}
			if ( int_val > entryNum ) {
				printf("Element number is too large!\n");
				return;
			}
			*((char *)&ipFilter) = (char)int_val;
			if ( !APMIB_GET(MIB_IPFILTER, (void *)&ipFilter)) {
				printf("Get table entry error!");
				return;
			}
		}
		else if ( !strcmp(val[0], "delall"))
			id = MIB_IPFILTER_DELALL;
		
		value = (void *)&ipFilter;
		break;

	case MACFILTER_ARRAY_T:
		if ( !strcmp(val[0], "add")) {
			id = MIB_MACFILTER_ADD;
			if ( valNum < 2 ) {
				printf("input argument is not enough!\n");
				return;
			}
			if ( strlen(val[1])!=12 || !string_to_hex(val[1], wlAc.macAddr, 12)) {
				printf("invalid value!\n");
				return;
			}

			if ( valNum > 2)
				strcpy(macFilter.comment, val[2]);
			else
				macFilter.comment[0] = '\0';

		}
		else if ( !strcmp(val[0], "del")) {
			id = MIB_MACFILTER_DEL;
			if ( valNum < 2 ) {
				printf("input argument is not enough!\n");
				return;
			}
			int_val = atoi(val[1]);
			if ( !APMIB_GET(MIB_MACFILTER_NUM, (void *)&entryNum)) {
				printf("Get port forwarding entry number error!");
				return;
			}
			if ( int_val > entryNum ) {
				printf("Element number is too large!\n");
				return;
			}
			*((char *)&macFilter) = (char)int_val;
			if ( !APMIB_GET(MIB_MACFILTER, (void *)&macFilter)) {
				printf("Get table entry error!");
				return;
			}
		}
		else if ( !strcmp(val[0], "delall"))
			id = MIB_MACFILTER_DELALL;
		value = (void *)&macFilter;
		break;

	case URLFILTER_ARRAY_T:
		if ( !strcmp(val[0], "add")) {
			id = MIB_URLFILTER_ADD;
			if ( valNum < 2 ) {
				printf("input argument is not enough!\n");
				return;
			}
			//if ( strlen(val[1])!=12 || !string_to_hex(val[1], wlAc.macAddr, 12)) {
			//	printf("invalid value!\n");
			//	return;
			//}

			//if ( valNum > 2)
			//	strcpy(urlFilter.comment, val[2]);
			//else
			//	uslFilter.comment[0] = '\0';

		}
		else if ( !strcmp(val[0], "del")) {
			id = MIB_URLFILTER_DEL;
			if ( valNum < 2 ) {
				printf("input argument is not enough!\n");
				return;
			}
			int_val = atoi(val[1]);
			if ( !APMIB_GET(MIB_URLFILTER_NUM, (void *)&entryNum)) {
				printf("Get port forwarding entry number error!");
				return;
			}
			if ( int_val > entryNum ) {
				printf("Element number is too large!\n");
				return;
			}
			*((char *)&urlFilter) = (char)int_val;
			if ( !APMIB_GET(MIB_URLFILTER, (void *)&urlFilter)) {
				printf("Get table entry error!");
				return;
			}
		}
		else if ( !strcmp(val[0], "delall"))
			id = MIB_URLFILTER_DELALL;
		value = (void *)&urlFilter;
		break;
		
	case TRIGGERPORT_ARRAY_T:
		if ( !strcmp(val[0], "add")) {
			id = MIB_TRIGGERPORT_ADD;
			if ( valNum < 7 ) {
				printf("input argument is not enough!\n");
				return;
			}
			triggerPort.tri_fromPort = atoi(val[1]);
			triggerPort.tri_toPort = atoi(val[2]);
			triggerPort.tri_protoType = atoi(val[3]);
			triggerPort.inc_fromPort = atoi(val[4]);
			triggerPort.inc_toPort = atoi(val[5]);
			triggerPort.inc_protoType = atoi(val[6]);

			if ( valNum > 7)
				strcpy(triggerPort.comment, val[7]);
			else
				triggerPort.comment[0] = '\0';

		}
		else if ( !strcmp(val[0], "del")) {
			id = MIB_TRIGGERPORT_DEL;
			if ( valNum < 2 ) {
				printf("input argument is not enough!\n");
				return;
			}
			int_val = atoi(val[1]);
			if ( !APMIB_GET(MIB_TRIGGERPORT_NUM, (void *)&entryNum)) {
				printf("Get trigger-port entry number error!");
				return;
			}
			if ( int_val > entryNum ) {
				printf("Element number is too large!\n");
				return;
			}
			*((char *)&triggerPort) = (char)int_val;
			if ( !APMIB_GET(MIB_TRIGGERPORT, (void *)&triggerPort)) {
				printf("Get trigger-port table entry error!");
				return;
			}
		}
		else if ( !strcmp(val[0], "delall"))
			id = MIB_TRIGGERPORT_DELALL;

		value = (void *)&triggerPort;
		break;
#ifdef GW_QOS_ENGINE
	case QOS_ARRAY_T:
		if ( !strcmp(val[0], "add")) {
			id = MIB_QOS_ADD;
			if ( valNum < 13 ) {
				printf("input argument is not enough!\n");
				return;
			}
			qos.enabled = atoi(val[1]);
			if ( !inet_aton(val[4], (struct in_addr *)&qos.local_ip_start) ||
                        !inet_aton(val[5], (struct in_addr *)&qos.local_ip_end) ||
                        !inet_aton(val[8], (struct in_addr *)&qos.remote_ip_start) ||
                        !inet_aton(val[9], (struct in_addr *)&qos.remote_ip_end) 
                      ) {
				printf("invalid internet address!\n");
				return;
			}
			qos.priority = atoi(val[2]);
			qos.protocol = atoi(val[3]);
			qos.local_port_start = atoi(val[6]);
			qos.local_port_end = atoi(val[7]);
			qos.remote_port_start = atoi(val[10]);
			qos.remote_port_end = atoi(val[11]);
			strcpy(qos.entry_name, val[12]);

		}
		else if ( !strcmp(val[0], "del")) {
			id = MIB_QOS_DEL;
			if ( valNum < 2 ) {
				printf("input argument is not enough!\n");
				return;
			}
			int_val = atoi(val[1]);
			if ( !APMIB_GET(MIB_QOS_RULE_NUM, (void *)&entryNum)) {
				printf("Get QoS entry number error!");
				return;
			}
			if ( int_val > entryNum ) {
				printf("Element number is too large!\n");
				return;
			}
			*((char *)&qos) = (char)int_val;
			if ( !APMIB_GET(MIB_QOS_RULE, (void *)&qos)) {
				printf("Get table entry error!");
				return;
			}
		}
		else if ( !strcmp(val[0], "delall"))
			id = MIB_QOS_DELALL;

		value = (void *)&qos;
		break;
#endif

#ifdef QOS_BY_BANDWIDTH
	case QOS_ARRAY_T:
		if ( !strcmp(val[0], "add")) {
			id = MIB_QOS_ADD;
			if ( valNum < 9 ) {
				printf("input argument is not enough!\n");
				return;
			}
			qos.enabled = atoi(val[1]);
			if ( !inet_aton(val[4], (struct in_addr *)&qos.local_ip_start) ||
                        !inet_aton(val[5], (struct in_addr *)&qos.local_ip_end) 
                      ) {
				printf("invalid internet address!\n");
				return;
			}
			//strcpy(qos.mac, val[2]);
			if (strlen(val[2])!=12 || !string_to_hex(val[2], qos.mac, 12))  {
				printf("invalid MAC address!\n");
				return;			
			}
			qos.mode = atoi(val[3]);
			qos.bandwidth = atoi(val[6]);
			strcpy(qos.entry_name, val[7]);
			qos.bandwidth_downlink = atoi(val[8]);

		}
		else if ( !strcmp(val[0], "del")) {
			id = MIB_QOS_DEL;
			if ( valNum < 2 ) {
				printf("input argument is not enough!\n");
				return;
			}
			int_val = atoi(val[1]);
			if ( !APMIB_GET(MIB_QOS_RULE_NUM, (void *)&entryNum)) {
				printf("Get QoS entry number error!");
				return;
			}
			if ( int_val > entryNum ) {
				printf("Element number is too large!\n");
				return;
			}
			*((char *)&qos) = (char)int_val;
			if ( !APMIB_GET(MIB_QOS_RULE, (void *)&qos)) {
				printf("Get table entry error!");
				return;
			}
		}
		else if ( !strcmp(val[0], "delall"))
			id = MIB_QOS_DELALL;

		value = (void *)&qos;
		break;
#endif

#ifdef ROUTE_SUPPORT
		case STATICROUTE_ARRAY_T:
		if ( !strcmp(val[0], "add")) {
			id = MIB_STATICROUTE_ADD;
			if ( valNum < 3 ) {
				printf("input argument is not enough!\n");
				return;
			}
			if ( !inet_aton(val[1], (struct in_addr *)&staticRoute.dstAddr)) {
                                printf("invalid destination IP address!\n");
                                return;
                        }
			if ( !inet_aton(val[2], (struct in_addr *)&staticRoute.netmask)) {
                                printf("invalid netmask !\n");
                                return;
                        }
			if ( !inet_aton(val[3], (struct in_addr *)&staticRoute.gateway)) {
                                printf("invalid gateway address!\n");
                                return;
                        }
		}
		else if ( !strcmp(val[0], "del")) {
			id = MIB_STATICROUTE_DEL;
			if ( valNum < 2 ) {
				printf("input argument is not enough!\n");
				return;
			}
			int_val = atoi(val[1]);
			if ( !APMIB_GET(MIB_STATICROUTE_NUM, (void *)&entryNum)) {
				printf("Get trigger-port entry number error!");
				return;
			}
			if ( int_val > entryNum ) {
				printf("Element number is too large!\n");
				return;
			}
			*((char *)&staticRoute) = (char)int_val;
			if ( !APMIB_GET(MIB_STATICROUTE, (void *)&staticRoute)) {
				printf("Get trigger-port table entry error!");
				return;
			}
		}
		else if ( !strcmp(val[0], "delall"))
			id = MIB_STATICROUTE_DELALL;

		value = (void *)&staticRoute;
		break;
#endif //ROUTE

#endif

	case SCHEDULE_ARRAY_T:
		if ( !strcmp(val[0], "add")) {

			if ( valNum < 6 ) {
				printf("input argument is not enough!\n");
				return;
			}

			id = MIB_SCHEDULE_ADD ;			
			memset(&SchEntry, '\0', sizeof(SchEntry));			
			
			SchEntry.eco =atoi(val[1]);
			SchEntry.fTime =atoi(val[2]);			
			SchEntry.tTime =atoi(val[3]);			
			SchEntry.day =atoi(val[4]);						
			strcpy(SchEntry.text , val[5]);
			value = (void *)&SchEntry;

		}
		else if ( !strcmp(val[0], "del")) {
			//may not used
		}
		else if ( !strcmp(val[0], "delall")){
			id = MIB_SCHEDULE_DELALL ;						
		}

		break;
	case DHCPRSVDIP_ARRY_T:
		if ( !strcmp(val[0], "add")) {
			id = MIB_DHCPRSVDIP_ADD;
			if ( valNum < 3 ) {
				printf("input argument is not enough!\n");
				return;
			}
			if ( strlen(val[1])!=12 || !string_to_hex(val[1], dhcpRsvd.macAddr, 12)) {
				printf("invalid value!\n");
				return;
			}
			if (!inet_aton(val[2], (struct in_addr *)&dhcpRsvd.ipAddr)) {
				printf("invalid internet address!\n");
				return;
			}		
			if ( valNum > 3 )
				strcpy(dhcpRsvd.hostName, val[3]);			
		}
		else if ( !strcmp(val[0], "del")) {
			id = MIB_DHCPRSVDIP_DEL;
			if ( valNum < 2 ) {
				printf("input argument is not enough!\n");
				return;
			}
			int_val = atoi(val[1]);
			if ( !APMIB_GET(MIB_DHCPRSVDIP_NUM, (void *)&entryNum)) {
				printf("Get DHCP resvd IP entry number error!");
				return;
			}
			if ( int_val > entryNum ) {
				printf("Element number is too large!\n");
				return;
			}
			*((char *)&dhcpRsvd) = (char)int_val;
			if ( !APMIB_GET(MIB_DHCPRSVDIP, (void *)&dhcpRsvd)) {
				printf("Get table entry error!");
				return;
			}
		}
		else if ( !strcmp(val[0], "delall"))
			id = MIB_DHCPRSVDIP_DELALL;
		value = (void *)&dhcpRsvd;
		break;
#if defined(VLAN_CONFIG_SUPPORTED)	
		case VLANCONFIG_ARRAY_T:
		if ( !strcmp(val[0], "add")) {
			id = MIB_VLANCONFIG_ADD;
#if defined(CONFIG_RTK_VLAN_ROUTETYPE)
			if ( valNum < 7 ) {
#else
			if ( valNum < 6 ) {
#endif
				printf("input argument is not enough!\n");
				return;
			}
			vlanConfig_entry.enabled=(unsigned char)atoi(val[1]);
			sprintf(vlanConfig_entry.netIface, "%s", val[2]);
			vlanConfig_entry.tagged = (unsigned char)atoi(val[3]);
			vlanConfig_entry.priority = (unsigned char)atoi(val[4]);
			vlanConfig_entry.cfi = (unsigned char)atoi(val[5]);
			vlanConfig_entry.vlanId = (unsigned short)atoi(val[6]);
#if defined(CONFIG_RTK_VLAN_ROUTETYPE)
			vlanConfig_entry.forwarding = (unsigned short)atoi(val[7]);
#endif
		}
		else if ( !strcmp(val[0], "del")) {
			id = MIB_VLANCONFIG_DEL;
			if ( valNum < 2 ) {
				printf("input argument is not enough!\n");
				return;
			}
			int_val = atoi(val[1]); //index of entry
			if ( !APMIB_GET(MIB_VLANCONFIG_NUM, (void *)&entryNum)) {
				printf("Get VLAN config entry number error!");
				return;
			}
			if ( int_val > entryNum ) {
				printf("Element number is too large!\n");
				return;
			}
			*((char *)&vlanConfig_entry) = (char)int_val;
			if ( !APMIB_GET(MIB_VLANCONFIG, (void *)&vlanConfig_entry)) {
				printf("Get table entry error!");
				return;
			}
			vlanConfig_entry.enabled=0;

		}
		else if ( !strcmp(val[0], "delall"))
			id = MIB_VLANCONFIG_DELALL;
		value = (void *)&vlanConfig_entry;
		break;
#endif
	case WLAC_ARRAY_T:
		if ( !strcmp(val[0], "add")) {
			id = MIB_WLAN_AC_ADDR_ADD;
			if ( valNum < 2 ) {
				printf("input argument is not enough!\n");
				return;
			}
			if ( strlen(val[1])!=12 || !string_to_hex(val[1], wlAc.macAddr, 12)) {
				printf("invalid value!\n");
				return;
			}

			if ( valNum > 2)
				strcpy(wlAc.comment, val[2]);
			else
				wlAc.comment[0] = '\0';
		}
		else if ( !strcmp(val[0], "del")) {
			id = MIB_WLAN_AC_ADDR_DEL;
			if ( valNum < 2 ) {
				printf("input argument is not enough!\n");
				return;
			}
			int_val = atoi(val[1]);
			if ( !APMIB_GET(MIB_WLAN_AC_NUM, (void *)&entryNum)) {
				printf("Get port forwarding entry number error!");
				return;
			}
			if ( int_val > entryNum ) {
				printf("Element number is too large!\n");
				return;
			}
			*((char *)&wlAc) = (char)int_val;
			if ( !APMIB_GET(MIB_WLAN_AC_ADDR, (void *)&wlAc)) {
				printf("Get table entry error!");
				return;
			}
		}
		else if ( !strcmp(val[0], "delall"))
			id = MIB_WLAN_AC_ADDR_DELALL;
		value = (void *)&wlAc;
		break;

#if defined(CONFIG_RTK_MESH) && defined(_MESH_ACL_ENABLE_) // below code copy above ACL code
	case MESH_ACL_ARRAY_T:
		if ( !strcmp(val[0], "add")) {
			id = MIB_MESH_ACL_ADDR_ADD;
			if ( valNum < 2 ) {
				printf("Mesh Acl Addr input argument is not enough!\n");
				return;
			}
			if ( strlen(val[1])!=12 || !string_to_hex(val[1], wlAc.macAddr, 12)) {
				printf("Mesh Acl Addr invalid value!\n");
				return;
			}

			if ( valNum > 2)
				strcpy(wlAc.comment, val[2]);
			else
				wlAc.comment[0] = '\0';
		}
		else if ( !strcmp(val[0], "del")) {
			id = MIB_MESH_ACL_ADDR_DEL;
			if ( valNum < 2 ) {
				printf("Mesh Acl Addr input argument is not enough!\n");
				return;
			}
			int_val = atoi(val[1]);
			if ( !APMIB_GET(MIB_MESH_ACL_NUM, (void *)&entryNum)) {
				printf("Mesh Acl Addr get port forwarding entry number error!");
				return;
			}
			if ( int_val > entryNum ) {
				printf("Mesh Acl Addr element number is too large!\n");
				return;
			}
			*((char *)&wlAc) = (char)int_val;
			if ( !APMIB_GET(MIB_MESH_ACL_ADDR, (void *)&wlAc)) {
				printf("Mesh Acl Addr get table entry error!");
				return;
			}
		}
		else if ( !strcmp(val[0], "delall"))
			id = MIB_MESH_ACL_ADDR_DELALL;
		value = (void *)&wlAc;
		break;
#endif	// CONFIG_RTK_MESH && _MESH_ACL_ENABLE_

	case WDS_ARRAY_T:
		if ( !strcmp(val[0], "add")) {
			id = MIB_WLAN_WDS_ADD;
			if ( valNum < 3 ) {
				printf("input argument is not enough!\n");
				return;
			}
			if ( strlen(val[1])!=12 || !string_to_hex(val[1], wds.macAddr, 12)) {
				printf("invalid value!\n");
				return;
			}

			if ( valNum > 2)
				strcpy(wds.comment, val[2]);
			else
				wds.comment[0] = '\0';
				
			wds.fixedTxRate = atoi(val[3]);	
		}
		else if ( !strcmp(val[0], "del")) {
			id = MIB_WLAN_WDS_DEL;
			if ( valNum < 2 ) {
				printf("input argument is not enough!\n");
				return;
			}
			int_val = atoi(val[1]);
			if ( !APMIB_GET(MIB_WLAN_WDS_NUM, (void *)&entryNum)) {
				printf("Get wds number error!");
				return;
			}
			if ( int_val > entryNum ) {
				printf("Element number is too large!\n");
				return;
			}
			*((char *)&wds) = (char)int_val;
			if ( !APMIB_GET(MIB_WLAN_WDS, (void *)&wds)) {
				printf("Get table entry error!");
				return;
			}
		}
		else if ( !strcmp(val[0], "delall"))
			id = MIB_WLAN_WDS_DELALL;
		value = (void *)&wds;
		break;

#ifdef HOME_GATEWAY
#ifdef VPN_SUPPORT
	case IPSECTUNNEL_ARRAY_T:
		if ( !strcmp(val[0], "add")) {
			id = MIB_IPSECTUNNEL_ADD;
			if ( valNum < 27 ) {
				printf("input argument is not enough!\n");
				return;
			}
			if ( !inet_aton(val[5], (struct in_addr *)&ipsecTunnel.lc_ipAddr)) {
				printf("invalid local IP address!\n");
				return;
			}
			
			if ( !inet_aton(val[8], (struct in_addr *)&ipsecTunnel.rt_ipAddr)) {
				printf("invalid remote IP address!\n");
				return;
			}
			if ( !inet_aton(val[10], (struct in_addr *)&ipsecTunnel.rt_gwAddr)) {
				printf("invalid remote gateway address!\n");
				return;
			}
			ipsecTunnel.tunnelId =  atoi(val[1]); 
			ipsecTunnel.enable = atoi(val[2]);
			ipsecTunnel.lcType = atoi(val[4]);

			if(strlen(val[3]) > (MAX_NAME_LEN-1)){
				printf("Connection Name too long !\n");
				return;
			}else
				strcpy(ipsecTunnel.connName, val[3]); 
			ipsecTunnel.lc_maskLen = atoi(val[6]);
			ipsecTunnel.rt_maskLen  = atoi(val[9]);
			ipsecTunnel.keyMode= atoi(val[11]);
			ipsecTunnel.conType = atoi(val[12]);
			ipsecTunnel.espEncr = atoi(val[13]);
			ipsecTunnel.espAuth = atoi(val[14]);
			if(strlen(val[15]) >  (MAX_NAME_LEN-1)){
				printf("Preshared Key too long !\n");
				return;
			}else
				strcpy(ipsecTunnel.psKey, val[15]); 

			ipsecTunnel.ikeEncr = atoi(val[16]);
			ipsecTunnel.ikeAuth = atoi(val[17]);
			ipsecTunnel.ikeKeyGroup = atoi(val[18]);
			ipsecTunnel.ikeLifeTime= strtol(val[19], (char **)NULL, 10);

			ipsecTunnel.ipsecLifeTime= strtol(val[20], (char **)NULL, 10);
			ipsecTunnel.ipsecPfs= atoi(val[21]);
			if(strlen(val[22]) >  (MAX_SPI_LEN-1)){
				printf("SPI too long !\n");
				return;
			}else
				strcpy(ipsecTunnel.spi, val[22]); 

			if(strlen(val[23]) >  (MAX_ENCRKEY_LEN-1)){
				printf("Encryption key too long !\n");
				return;
			}else
				strcpy(ipsecTunnel.encrKey, val[23]); 

			if(strlen(val[24]) >  (MAX_AUTHKEY_LEN-1)){
				printf("Authentication key too long !\n");
				return;
			}else
				strcpy(ipsecTunnel.authKey, val[24]); 


		}
		else if ( !strcmp(val[0], "del")) {
			id = MIB_IPSECTUNNEL_DEL;
			if ( valNum < 2 ) {
				printf("input argument is not enough!\n");
				return;
			}
			int_val = atoi(val[1]);
			if ( !APMIB_GET(MIB_IPSECTUNNEL_NUM, (void *)&entryNum)) {
				printf("Get ipsec tunnel number error!");
				return;
			}
			if ( int_val > entryNum ) {
				printf("Element number is too large!\n");
				return;
			}
			*((char *)&ipsecTunnel) = (char)int_val;
			if ( !APMIB_GET(MIB_IPSECTUNNEL, (void *)&ipsecTunnel)) {
				printf("Get table entry error!");
				return;
			}
		}
		else if ( !strcmp(val[0], "delall"))
			id = MIB_IPSECTUNNEL_DELALL;
		value = (void *)&ipsecTunnel;
		break;

#endif
#endif
#ifdef TLS_CLIENT
	case CERTROOT_ARRAY_T:
	if ( !strcmp(val[0], "add")) {
		id = MIB_CERTROOT_ADD;
		strcpy(certRoot.comment, val[1]);
	}
	else if ( !strcmp(val[0], "del")) {
			id = MIB_CERTROOT_DEL;
			if ( valNum < 2 ) {
				printf("input argument is not enough!\n");
				return;
			}
			int_val = atoi(val[1]);
			if ( !APMIB_GET(MIB_CERTROOT_NUM, (void *)&entryNum)) {
				printf("Get cert ca number error!");
				return;
			}
			if ( int_val > entryNum ) {
				printf("Element number is too large!\n");
				return;
			}
			*((char *)&certRoot) = (char)int_val;
			if ( !APMIB_GET(MIB_CERTROOT, (void *)&certRoot)) {
				printf("Get table entry error!");
				return;
			}			
	}
	else if ( !strcmp(val[0], "delall"))
			id = MIB_CERTROOT_DELALL;
	value = (void *)&certRoot;
	break;
	case CERTUSER_ARRAY_T:
	if ( !strcmp(val[0], "add")) {
		id = MIB_CERTUSER_ADD;
		strcpy(certUser.comment, val[1]);
		strcpy(certUser.pass , val[2]);
	}
	else if ( !strcmp(val[0], "del")) {
			id = MIB_CERTUSER_DEL;
			if ( valNum < 2 ) {
				printf("input argument is not enough!\n");
				return;
			}
			int_val = atoi(val[1]);
			if ( !APMIB_GET(MIB_CERTUSER_NUM, (void *)&entryNum)) {
				printf("Get cert ca number error!");
				return;
			}
			if ( int_val > entryNum ) {
				printf("Element number is too large!\n");
				return;
			}
			*((char *)&certUser) = (char)int_val;
			if ( !APMIB_GET(MIB_CERTUSER, (void *)&certUser)) {
				printf("Get table entry error!");
				return;
			}			
	}
	else if ( !strcmp(val[0], "delall"))
			id = MIB_CERTUSER_DELALL;
	value = (void *)&certUser;
	break;	
#endif
	case BYTE13_T:
		if ( strlen(val[0])!=26 || !string_to_hex(val[0], key, 26)) {
			printf("invalid value!\n");
			return;
		}
		value = (void *)key;
		break;

	case STRING_T:
		if ( strlen(val[0]) > len) {
			printf("string value too long!\n");
			return;
		}
		value = (void *)val[0];
		break;
	default: printf("invalid mib!\n"); return;
	}

	if ( !APMIB_SET(id, value))
		printf("set MIB failed!\n");

	if (config_area) {
		if (config_area == 1 || config_area == 2)
			apmib_update(HW_SETTING);
		else if (config_area == 3 || config_area == 4)
			apmib_update(DEFAULT_SETTING);
		else
			apmib_update(CURRENT_SETTING);
	}
}
static void dumpAllHW(void)
{
	int idx=0, num;
	mib_table_entry_T *pTbl=NULL;

	if ( !apmib_init_HW()) {
		printf("Initialize AP MIB failed!\n");
		return;
	}
#ifdef MBSSID
	vwlan_idx=0;
#endif
	 config_area=0;

next_tbl:
	if (++config_area > 2)
	 	return;
	if (config_area == 1)
		pTbl = hwmib_table;
	else if (config_area == 2)
		pTbl = hwmib_wlan_table;
	

next_wlan:
	while (pTbl[idx].id) {
			num = 1;
		if (num >0) {
			if (config_area == 1 || config_area == 2)
				printf("HW_");
			if (config_area == 2) {
				printf("WLAN%d_", wlan_idx);
			}	
			getMIB(pTbl[idx].name, pTbl[idx].id, pTbl[idx].type, num, 1 , NULL);
		}
		idx++;
	}
	idx = 0;

	if (config_area == 2 ) {
		if (++wlan_idx < NUM_WLAN_INTERFACE) 
			goto next_wlan;
		else
			wlan_idx = 0;		
	}
	
	goto next_tbl;
}



////////////////////////////////////////////////////////////////////////////////
static void dumpAll(void)
{
	int idx=0, num;
	mib_table_entry_T *pTbl=NULL;

	if ( !apmib_init()) {
		printf("Initialize AP MIB failed!\n");
		return;
	}

#ifdef MBSSID
	vwlan_idx=0;
#endif
	 config_area=0;

next_tbl:
	if (++config_area > 6)
	 	return;
	if (config_area == 1)
		pTbl = hwmib_table;
	else if (config_area == 2)
		pTbl = hwmib_wlan_table;
	else if (config_area == 3 || config_area == 5)
		pTbl = mib_table;
	else if (config_area == 4 || config_area == 6)
		pTbl = mib_wlan_table;

next_wlan:
	while (pTbl[idx].id) {
		if ( pTbl[idx].id == MIB_WLAN_AC_ADDR)
			APMIB_GET(MIB_WLAN_AC_NUM, (void *)&num);
		
#if defined(CONFIG_RTK_MESH) && defined(_MESH_ACL_ENABLE_) // below code copy above ACL code
		else if ( pTbl[idx].id == MIB_MESH_ACL_ADDR)
			APMIB_GET(MIB_MESH_ACL_NUM, (void *)&num);
#endif

		else if ( pTbl[idx].id == MIB_WLAN_WDS)
			APMIB_GET(MIB_WLAN_WDS_NUM, (void *)&num);
		else if ( pTbl[idx].id == MIB_SCHEDULE)
			APMIB_GET(MIB_SCHEDULE_NUM, (void *)&num);		
			
#if defined(VLAN_CONFIG_SUPPORTED)				
		else if ( pTbl[idx].id == MIB_VLANCONFIG){
			APMIB_GET(MIB_VLANCONFIG_NUM, (void *)&num);
		}
#endif						
#ifdef HOME_GATEWAY
		else if ( pTbl[idx].id == MIB_PORTFW)
			APMIB_GET(MIB_PORTFW_NUM, (void *)&num);
		else if ( pTbl[idx].id == MIB_PORTFILTER)
			APMIB_GET(MIB_PORTFILTER_NUM, (void *)&num);
		else if ( pTbl[idx].id == MIB_IPFILTER)
			APMIB_GET(MIB_IPFILTER_NUM, (void *)&num);
		else if ( pTbl[idx].id == MIB_MACFILTER)
			APMIB_GET(MIB_MACFILTER_NUM, (void *)&num);
		else if ( pTbl[idx].id == MIB_URLFILTER)
			APMIB_GET(MIB_URLFILTER_NUM, (void *)&num);
		else if ( pTbl[idx].id == MIB_TRIGGERPORT)
			APMIB_GET(MIB_TRIGGERPORT_NUM, (void *)&num);

#if defined(GW_QOS_ENGINE) || defined(QOS_BY_BANDWIDTH)
		else if ( pTbl[idx].id == MIB_QOS_RULE)
			APMIB_GET(MIB_QOS_RULE_NUM, (void *)&num);
#endif
#ifdef ROUTE_SUPPORT
		else if ( pTbl[idx].id == MIB_STATICROUTE)
			APMIB_GET(MIB_STATICROUTE_NUM, (void *)&num);
#endif //ROUTE
#ifdef VPN_SUPPORT
		else if ( pTbl[idx].id == MIB_IPSECTUNNEL)
			APMIB_GET(MIB_IPSECTUNNEL_NUM, (void *)&num);
#endif
#endif
#ifdef TLS_CLIENT
		else if ( pTbl[idx].id == MIB_CERTROOT)
			APMIB_GET(MIB_CERTROOT_NUM, (void *)&num);
		else if ( pTbl[idx].id == MIB_CERTUSER)
			APMIB_GET(MIB_CERTUSER_NUM, (void *)&num);			
#endif
		else if ( pTbl[idx].id == MIB_DHCPRSVDIP)
			APMIB_GET(MIB_DHCPRSVDIP_NUM, (void *)&num);	
		else
			num = 1;
		if (num >0) {
			if (config_area == 1 || config_area == 2)
				printf("HW_");
			else if (config_area == 3 || config_area == 4)
				printf("DEF_");
			if (config_area == 2 || config_area == 4 || config_area == 6) {
#ifdef MBSSID
				if ((config_area == 4 || config_area == 6) && vwlan_idx > 0)
					printf("WLAN%d_VAP%d_", wlan_idx, vwlan_idx-1);
				else
#endif
				printf("WLAN%d_", wlan_idx);
			}
			
			getMIB(pTbl[idx].name, pTbl[idx].id,
						pTbl[idx].type, num, 1 , NULL);
		}
		idx++;
	}
	idx = 0;

	if (config_area == 2 || config_area == 4 || config_area == 6) {
#ifdef MBSSID
		if (config_area == 4 || config_area == 6) {
				
			if (++vwlan_idx <= NUM_VWLAN_INTERFACE) 
				goto next_wlan;
			else
				vwlan_idx = 0;
		}
#endif
		if (++wlan_idx < NUM_WLAN_INTERFACE) 
			goto next_wlan;
		else
			wlan_idx = 0;		
	}
	
	goto next_tbl;
}

//////////////////////////////////////////////////////////////////////////////////
static void showHelp(void)
{
	printf("Usage: flash cmd\n");
	printf("option:\n");
	printf("cmd:\n");
	printf("      default -- write flash parameters to default.\n");
	printf("      get [wlan interface-index] mib-name -- get a specific mib from flash\n");
	printf("          memory.\n");
	printf("      set [wlan interface-index] mib-name mib-value -- set a specific mib into\n");
	printf("          flash memory.\n");
	printf("      all -- dump all flash parameters.\n");
	printf("      gethw hw-mib-name -- get a specific mib from flash\n");
	printf("          memory.\n");
	printf("      sethw hw-mib-name mib-value -- set a specific mib into\n");
	printf("          flash memory.\n");
	printf("      allhw -- dump all hw flash parameters.\n");
	printf("      reset -- reset current setting to default.\n");
#ifdef WLAN_FAST_INIT
	printf("      set_mib -- get mib from flash and set to wlan interface.\n");
#endif
	printf("\n");
}

//////////////////////////////////////////////////////////////////////////////////
static void showAllHWMibName(void)
{
	int idx;
	mib_table_entry_T *pTbl;

	config_area = 0;
	while (config_area++ < 7) {
		idx = 0;
		if (config_area == 1 || config_area == 2) {
			if (config_area == 1)
				pTbl = hwmib_table;
			else
				pTbl = hwmib_wlan_table;
			while (pTbl[idx].id) {
				printf("HW_%s\n", pTbl[idx].name);
				idx++;
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////
static void showAllMibName(void)
{
	int idx;
	mib_table_entry_T *pTbl;

	config_area = 0;
	while (config_area++ < 7) {
		idx = 0;
		if (config_area == 1 || config_area == 2) {
			if (config_area == 1)
				pTbl = hwmib_table;
			else
				pTbl = hwmib_wlan_table;
			while (pTbl[idx].id) {
				printf("HW_%s\n", pTbl[idx].name);
				idx++;
			}
		}
		else {
			if (config_area == 3 || config_area == 5)
				pTbl = mib_table;
			else
				pTbl = mib_wlan_table;

			if (config_area == 3 || config_area == 4)
				printf("DEF_");

			while (pTbl[idx].id) {
				printf("%s\n", pTbl[idx].name);
				idx++;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////
static void showSetACHelp(void)
{
#if 0
	printf("flash set MACAC_ADDR cmd\n");
	printf("cmd:\n");
	printf("      add mac-addr comment -- append a filter mac address.\n");
	printf("      del entry-number -- delete a filter entry.\n");
	printf("      delall -- delete all filter mac address.\n");
#endif	
}

#if defined(CONFIG_RTK_MESH) && defined(_MESH_ACL_ENABLE_) // below code copy above ACL code
static void showSetMeshACLHelp(void)
{
#if 0
	printf("flash set MESH_ACL_ADDR cmd\n");
	printf("cmd:\n");
	printf("      add mac-addr comment -- append a filter mac address.\n");
	printf("      del entry-number -- delete a filter entry.\n");
	printf("      delall -- delete all filter mac address.\n");
#endif	
}
#endif

#ifdef VLAN_CONFIG_SUPPORTED
////////////////////////////////////////////////////////////////////////////////////
static void showSetVlanConfigHelp(void)
{
#if 0
	printf("flash set VLAN CONFIG  cmd\n");
	printf("cmd:\n");
	printf("      add enable iface -- update vlan config for specific iface.\n");
	printf("      del entry-number -- delete a vlan config entry.\n");
	printf("      delall -- delete all vlan config entry\n");
#endif		
}
#endif

///////////////////////////////////////////////////////////////////////////////////
static void showSetWdsHelp(void)
{
#if 0
	printf("flash set WDS cmd\n");
	printf("cmd:\n");
	printf("      add mac-addr comment -- append a WDS mac address.\n");
	printf("      del entry-number -- delete a WDS entry.\n");
	printf("      delall -- delete all WDS mac address.\n");
#endif	
}

#ifdef HOME_GATEWAY
///////////////////////////////////////////////////////////////////////////////////
static void showSetPortFwHelp(void)
{
#if 0
	printf("flash set PORTFW_TBL cmd\n");
	printf("cmd:\n");
	printf("      add ip from-port to-port protocol comment -- add a filter.\n");
	printf("      del entry-number -- delete a filter.\n");
	printf("      delall -- delete all filter.\n");
#endif	
}


///////////////////////////////////////////////////////////////////////////////////
static void showSetPortFilterHelp(void)
{
#if 0
	printf("flash set PORTFILTER_TBL cmd\n");
	printf("cmd:\n");
	printf("      add from-port to-port protocol comment -- add a filter.\n");
	printf("      del entry-number -- delete a filter.\n");
	printf("      delall -- delete all filter.\n");
#endif	
}

///////////////////////////////////////////////////////////////////////////////////
static void showSetIpFilterHelp(void)
{
#if 0
	printf("flash set IPFILTER_TBL cmd\n");
	printf("cmd:\n");
	printf("      add ip protocol comment -- add a filter.\n");
	printf("      del entry-number -- delete a filter.\n");
	printf("      delall -- delete all filter.\n");
#endif	
}

///////////////////////////////////////////////////////////////////////////////////
static void showSetMacFilterHelp(void)
{
#if 0
	printf("flash set MACFILTER_TBL cmd\n");
	printf("cmd:\n");
	printf("      add mac-addr comment -- add a filter.\n");
	printf("      del entry-number -- delete a filter.\n");
	printf("      delall -- delete all filter.\n");
#endif	
}
///////////////////////////////////////////////////////////////////////////////////
static void showSetUrlFilterHelp(void)
{
#if 0
	printf("flash set URLFILTER_TBL cmd\n");
	printf("cmd:\n");
	printf("      add url-addr -- add a filter.\n");
	printf("      del entry-number -- delete a filter.\n");
	printf("      delall -- delete all filter.\n");
#endif	
}
///////////////////////////////////////////////////////////////////////////////////
static void showSetTriggerPortHelp(void)
{
#if 0
	printf("flash set TRIGGER_PORT cmd\n");
	printf("cmd:\n");
	printf("   add trigger-from trigger-to trigger-proto incoming-from incoming-to incoming-proto comment -- add a trigger-port.\n");
	printf("   del entry-number -- delete a trigger-port.\n");
	printf("   delall -- delete all trigger-port.\n");
#endif	
}




#ifdef GW_QOS_ENGINE
///////////////////////////////////////////////////////////////////////////////////
//static void showSetQosHelp(void) {}
#endif
///////////////////////////////////////////////////////////////////////////////////
#ifdef ROUTE_SUPPORT
static void showSetStaticRouteHelp(void)
{
	printf("flash set STATICROUTE_TBL cmd\n");
	printf("cmd:\n");
	printf("   add dest_ip netmask gateway  -- add a static route.\n");
	printf("   del entry-number -- delete a static route.\n");
	printf("   delall -- delete all static route.\n");


}
#endif //ROUTE
#endif

#ifdef HOME_GATEWAY
#ifdef VPN_SUPPORT
static void  showSetIpsecTunnelHelp(void)
{
        printf("flash set IPSECTUNNEL_TBL cmd\n");
        printf("cmd:\n");
        printf("   add tunnel_id enable name local_type local_ip local_mask_len remote_type remote_ip remote_mask_len remote_gw keymode connectType espEncr espAuth psKey ike_encr ike_auth ike_keygroup ike_lifetime ipsec_lifetime ipsec_pfs spi encrKey authKey -- add a ipsec manual tunnel.\n");
        printf("   del entry-number -- delete a vpn tunnel.\n");
        printf("   delall -- delete all tunnel.\n");
}
#endif
#endif

#ifdef TLS_CLIENT
static void  showSetCertRootHelp(void)
{
        printf("flash set CERTROOT_TBL cmd\n");
        printf("cmd:\n");
        printf("   add comment.\n");
        printf("   del entry-number -- delete a certca .\n");
        printf("   delall -- delete all certca.\n");
}
static void  showSetCertUserHelp(void)
{
        printf("flash set CERTUSER_TBL cmd\n");
        printf("cmd:\n");
        printf("   add comment password.\n");
        printf("   del entry-number -- delete a certca .\n");
        printf("   delall -- delete all certca.\n");
}
#endif

#ifdef PARSE_TXT_FILE
////////////////////////////////////////////////////////////////////////////////
static int parseTxtConfig(char *filename, APMIB_Tp pConfig)
{
	char line[300], value[300];
	FILE *fp;
	int id;

	fp = fopen(filename, "r");
	if ( fp == NULL )
		return -1;

	acNum = 0;
	
#if defined(CONFIG_RTK_MESH) && defined(_MESH_ACL_ENABLE_)
	meshAclNum = 0;
#endif

	wdsNum = 0;

#ifdef HOME_GATEWAY
	portFilterNum = ipFilterNum = macFilterNum = portFwNum = staticRouteNum=0;
	urlFilterNum = 0;

#if defined(GW_QOS_ENGINE) || defined(QOS_BY_BANDWIDTH)
	qosRuleNum = 0;
#endif
#endif
#ifdef TLS_CLIENT
	certRootNum =  certUserNum = 0;
#endif

#if defined(VLAN_CONFIG_SUPPORTED)
 vlanConfigNum=0;	
#endif
	while ( fgets(line, 100, fp) ) {
		id = getToken(line, value);
		if ( id == 0 )
			continue;
		if ( set_mib(pConfig, id, value) < 0) {
			printf("Parse MIB [%d] error!\n", id );
			fclose(fp);
			return -1;
		}
	}

	fclose(fp);
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
static int getToken(char *line, char *value)
{
	char *ptr=line, *p1;
	char token[300]={0};
	int len=0, idx;

	if ( *ptr == ';' )	// comments
		return 0;

	// get token
	while (*ptr && *ptr!=EOL) {
		if ( *ptr == '=' ) {
			memcpy(token, line, len);

			// delete ending space
			for (idx=len-1; idx>=0; idx--) {
				if (token[idx]!= SPACE )
					break;
			}
			token[idx+1] = '\0';
			ptr++;
			break;
		}
		ptr++;
		len++;
	}
	if ( !token[0] )
		return 0;

	// get value
	len=0;
	while (*ptr == SPACE ) ptr++; // delete space

	p1 = ptr;
	while ( *ptr && *ptr!=EOL) {
		ptr++;
		len++;
	}
	memcpy(value, p1, len );
	value[len] = '\0';

	idx = 0;
	while (mib_table[idx].id) {
		if (!strcmp(mib_table[idx].name, token))
			return mib_table[idx].id;
		idx++;
	}
	return 0;
}


////////////////////////////////////////////////////////////////////////////////
static int set_mib(APMIB_Tp pMib, int id, void *value)
{
	unsigned char key[100];
	char *p1, *p2;
#ifdef HOME_GATEWAY
	char *p3, *p4, *p5;
#if defined(GW_QOS_ENGINE) || defined(VLAN_CONFIG_SUPPORTED) && !defined(QOS_BY_BANDWIDTH)
	char *p6, *p7, *p8, *p9, *p10, *p11, *p12;
#endif

#if defined(QOS_BY_BANDWIDTH) && !defined(GW_QOS_ENGINE)

#if defined(VLAN_CONFIG_SUPPORTED)
	char *p6, *p7, *p8;
#else
	char *p6, *p7;
#endif

#endif

#else

#if defined(VLAN_CONFIG_SUPPORTED)	
	char *p3, *p4, *p5, *p6, *p7, *p8;
#endif

#endif
	struct in_addr inAddr;
	int i;
	MACFILTER_Tp pWlAc;
	WDS_Tp pWds;

#ifdef HOME_GATEWAY
	PORTFW_Tp pPortFw;
	PORTFILTER_Tp pPortFilter;
	IPFILTER_Tp pIpFilter;
	MACFILTER_Tp pMacFilter;
	URLFILTER_Tp pUrlFilter;

#ifdef GW_QOS_ENGINE
	QOS_Tp pQos;    
#endif

#ifdef QOS_BY_BANDWIDTH
	IPQOS_Tp pQos;    
#endif
#endif

#ifdef TLS_CLIENT
	CERTROOT_Tp pCertRoot;
	CERTUSER_Tp pCertUser;
#endif
	DHCPRSVDIP_Tp pDhcpRsvd;
#if defined(VLAN_CONFIG_SUPPORTED)	
	VLAN_CONFIG_Tp pVlanConfig;
	int j;
#endif	
	for (i=0; mib_table[i].id; i++) {
		if ( mib_table[i].id == id )
			break;
	}
	if ( mib_table[i].id == 0 )
		return -1;

	switch (mib_table[i].type) {
	case BYTE_T:
		*((unsigned char *)(((long)pMib) + mib_table[i].offset)) = (unsigned char)atoi(value);
		break;

	case WORD_T:
		*((unsigned short *)(((long)pMib) + mib_table[i].offset)) = (unsigned short)atoi(value);
		break;

	case STRING_T:
		if ( strlen(value)+1 > mib_table[i].size )
			return 0;
		strcpy((char *)(((long)pMib) + mib_table[i].offset), (char *)value);
		break;

	case BYTE5_T:
		if (strlen(value)!=10 || !string_to_hex(value, key, 10))
			return -1;
		memcpy((unsigned char *)(((long)pMib) + mib_table[i].offset), key, 5);
		break;

	case BYTE6_T:
		if (strlen(value)!=12 || !string_to_hex(value, key, 12))
			return -1;
		memcpy((unsigned char *)(((long)pMib) + mib_table[i].offset), key, 6);
		break;

	case BYTE13_T:
		if (strlen(value)!=26 || !string_to_hex(value, key, 26))
			return -1;
		memcpy((unsigned char *)(((long)pMib) + mib_table[i].offset), key, 13);
		break;
	
	case DWORD_T:
		*((unsigned long *)(((long)pMib) + mib_table[i].offset)) = (unsigned long)atoi(value);
		break;

	case IA_T:
		if ( !inet_aton(value, &inAddr) )
			return -1;
		memcpy((unsigned char *)(((long)pMib) + mib_table[i].offset), (unsigned char *)&inAddr,  4);
		break;

	// CONFIG_RTK_MESH Note: The statement haven't use maybe, Because mib_table haven't WLAC_ARRAY_T
	case WLAC_ARRAY_T:
		getVal2((char *)value, &p1, &p2);
		if (p1 == NULL) {
			printf("Invalid WLAC in argument!\n");
			break;
		}
		if (strlen(p1)!=12 || !string_to_hex(p1, key, 12))
			return -1;

		pWlAc = (MACFILTER_Tp)(((long)pMib)+mib_table[i].offset+acNum*sizeof(MACFILTER_T));
		memcpy(pWlAc->macAddr, key, 6);
		if (p2 != NULL )
			strcpy(pWlAc->comment, p2);
		acNum++;
		break;

#if defined(CONFIG_RTK_MESH) && defined(_MESH_ACL_ENABLE_) // below code copy above ACL code
	case MESH_ACL_ARRAY_T:
		getVal2((char *)value, &p1, &p2);
		if (p1 == NULL) {
			printf("Invalid Mesh Acl in argument!\n");
			break;
		}
		if (strlen(p1)!=12 || !string_to_hex(p1, key, 12))
			return -1;

		pWlAc = (MACFILTER_Tp)(((long)pMib)+mib_table[i].offset+meshAclNum*sizeof(MACFILTER_T));
		memcpy(pWlAc->macAddr, key, 6);
		if (p2 != NULL )
			strcpy(pWlAc->comment, p2);
		meshAclNum++;
		break;
#endif

	case WDS_ARRAY_T:
		getVal3((char *)value, &p1, &p2, &p3);
		if (p1 == NULL) {
			printf("Invalid WDS in argument!\n");
			break;
		}
		if (strlen(p1)!=12 || !string_to_hex(p1, key, 12))
			return -1;

		pWds = (WDS_Tp)(((long)pMib)+mib_table[i].offset+wdsNum*sizeof(WDS_T));
		memcpy(pWds->macAddr, key, 6);
		if (p2 != NULL )
			strcpy(pWds->comment, p2);
		pWds->fixedTxRate = (unsigned int)atoi(p3);	
		wdsNum++;
		break;

#ifdef HOME_GATEWAY
	case MACFILTER_ARRAY_T:
		getVal2((char *)value, &p1, &p2);
		if (p1 == NULL) {
			printf("Invalid MACFILTER in argument!\n");
			break;
		}
		if (strlen(p1)!=12 || !string_to_hex(p1, key, 12))
			return -1;

		pMacFilter = (MACFILTER_Tp)(((long)pMib)+mib_table[i].offset+macFilterNum*sizeof(MACFILTER_T));
		memcpy(pMacFilter->macAddr, key, 6);
		if (p2 != NULL )
			strcpy(pMacFilter->comment, p2);
		macFilterNum++;
		break;

	case URLFILTER_ARRAY_T:
		getVal2((char *)value, &p1, &p2);
		if (p1 == NULL) {
			printf("Invalid URLFILTER in argument!\n");
			break;
		}
		//if (strlen(p1)!=12 || !string_to_hex(p1, key, 12))
		//	return -1;

		pUrlFilter = (URLFILTER_Tp)(((long)pMib)+mib_table[i].offset+urlFilterNum*sizeof(URLFILTER_T));
		memcpy(pUrlFilter->urlAddr, key, 20);
		//if (p2 != NULL )
		//	strcpy(pMacFilter->comment, p2);
		urlFilterNum++;
		break;

	case PORTFW_ARRAY_T:
		getVal5((char *)value, &p1, &p2, &p3, &p4, &p5);
		if (p1 == NULL || p2 == NULL || p3 == NULL || p4 == NULL ) {
			printf("Invalid PORTFW arguments!\n");
			break;
		}
		if ( !inet_aton(p1, &inAddr) )
			return -1;

		pPortFw = (PORTFW_Tp)(((long)pMib)+mib_table[i].offset+portFwNum*sizeof(PORTFW_T));
		memcpy(pPortFw->ipAddr, (unsigned char *)&inAddr, 4);
		pPortFw->fromPort = (unsigned short)atoi(p2);
		pPortFw->toPort = (unsigned short)atoi(p3);
		pPortFw->protoType = (unsigned char)atoi(p4);
		if ( p5 )
			strcpy( pPortFw->comment, p5 );
		portFwNum++;
		break;

	case IPFILTER_ARRAY_T:
		getVal3((char *)value, &p1, &p2, &p3);
		if (p1 == NULL || p2 == NULL) {
			printf("Invalid IPFILTER arguments!\n");
			break;
		}
		if ( !inet_aton(p1, &inAddr) )
			return -1;
		pIpFilter = (IPFILTER_Tp)(((long)pMib)+mib_table[i].offset+ipFilterNum*sizeof(IPFILTER_T));
		memcpy(pIpFilter->ipAddr, (unsigned char *)&inAddr, 4);
		pIpFilter->protoType = (unsigned char)atoi(p2);
		if ( p3 )
			strcpy( pIpFilter->comment, p3 );
		ipFilterNum++;
		break;

	case PORTFILTER_ARRAY_T:
		getVal4((char *)value, &p1, &p2, &p3, &p4);
		if (p1 == NULL || p2 == NULL || p3 == NULL) {
			printf("Invalid PORTFILTER arguments!\n");
			break;
		}
		if ( !inet_aton(p1, &inAddr) )
			return -1;
		pPortFilter = (PORTFILTER_Tp)(((long)pMib)+mib_table[i].offset+portFilterNum*sizeof(PORTFILTER_T));
		pPortFilter->fromPort = (unsigned short)atoi(p1);
		pPortFilter->toPort = (unsigned short)atoi(p2);
		pPortFilter->protoType = (unsigned char)atoi(p3);
		if ( p4 )
			strcpy( pPortFilter->comment, p4 );
		portFilterNum++;
		break;
#ifdef GW_QOS_ENGINE
	case QOS_ARRAY_T:
		getVal12((char *)value, &p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10, &p11, &p12);
		if (p1 == NULL || p2 == NULL || p3 == NULL || p4 == NULL || p5 == NULL || p6 == NULL || p7 == NULL ||
		    p8 == NULL || p9 == NULL || p10 == NULL || p11 == NULL || p12 == NULL ) {
			printf("Invalid QoS arguments!\n");
			break;
		}
		pQos = (QOS_Tp)(((long)pMib)+mib_table[i].offset+qosRuleNum*sizeof(QOS_T));
		pQos->enabled = (unsigned char)atoi(p1);
		pQos->priority = (unsigned char)atoi(p2);
		pQos->protocol = (unsigned short)atoi(p3);
		if ( !inet_aton(p4, &inAddr) )
			return -1;
		memcpy(pQos->local_ip_start, (unsigned char *)&inAddr, 4);
		if ( !inet_aton(p5, &inAddr) )
			return -1;
		memcpy(pQos->local_ip_end, (unsigned char *)&inAddr, 4);
        
		pQos->local_port_start = (unsigned short)atoi(p6);
		pQos->local_port_end = (unsigned short)atoi(p7);
		if ( !inet_aton(p8, &inAddr) )
			return -1;
		memcpy(pQos->remote_ip_start, (unsigned char *)&inAddr, 4);
		if ( !inet_aton(p9, &inAddr) )
			return -1;
		memcpy(pQos->remote_ip_end, (unsigned char *)&inAddr, 4);

		pQos->remote_port_start = (unsigned short)atoi(p10);
		pQos->remote_port_end = (unsigned short)atoi(p11);
        	strcpy( pQos->entry_name, p12 );
		qosRuleNum++;        
		break;
#endif

#ifdef QOS_BY_BANDWIDTH
	case QOS_ARRAY_T:
		getVal7((char *)value, &p1, &p2, &p3, &p4, &p5, &p6, &p7);
		if (p1 == NULL || p2 == NULL || p3 == NULL || p4 == NULL || p5 == NULL || p6 == NULL || p7 == NULL) {
			printf("Invalid QoS arguments!\n");
			break;
		}
		pQos = (IPQOS_Tp)(((long)pMib)+mib_table[i].offset+qosRuleNum*sizeof(IPQOS_T));
		pQos->enabled = (unsigned char)atoi(p1);
        	//strcpy( pQos->mac, p2 );
		if (strlen(p2)!=12 || !string_to_hex(p2, pQos->mac, 12)) 
			return -1;		
		pQos->mode = (unsigned char)atoi(p3);
		if ( !inet_aton(p4, &inAddr) )
			return -1;
		memcpy(pQos->local_ip_start, (unsigned char *)&inAddr, 4);
		if ( !inet_aton(p5, &inAddr) )
			return -1;
		memcpy(pQos->local_ip_end, (unsigned char *)&inAddr, 4);
        
		pQos->bandwidth = (unsigned long)atoi(p6);

        	strcpy( pQos->entry_name, p7 );
		qosRuleNum++;        
		break;
#endif

#ifdef ROUTE_SUPPORT
	case STATICROUTE_ARRAY_T:
		getVal5((char *)value, &p1, &p2, &p3, &p4, &p5);
		if (p1 == NULL || p2 == NULL || p3 == NULL) {
			printf("Invalid PORTFILTER arguments!\n");
			break;
		}
		if ( !inet_aton(p1, &inAddr) )
			return -1;
		pStaticRoute = (STATICROUTE_Tp)(((long)pMib)+mib_table[i].offset+staticRouteNum*sizeof(STATICROUTE_T));
		if( !inet_aton(p1, &pStaticRoute->destAddr))
			return -1 ;
		if( !inet_aton(p2, &pStaticRoute->netmask))
			return -1 ;
		if( !inet_aton(p3, &pStaticRoute->gateway))
			return -1 ;
		pStaticRoute->_interface_=(unsigned char)atoi(p4);
		pStaticRoute->metric=(unsigned char)atoi(p5);
			
		staticRouteNum++;
		break;
#endif // ROUTE_SUPPORT
#endif
#ifdef TLS_CLIENT
	case CERTROOT_ARRAY_T:
		getVal1((char *)value, &p1);
		if (p1 == NULL ) {
			printf("Invalid CERTCA arguments!\n");
			break;
		}
		pCertRoot = (CERTROOT_Tp)(((long)pMib)+mib_table[i].offset+certRootNum*sizeof(CERTROOT_T));
		strcpy( pCertRoot->comment, p1 );
		certRootNum++;
		break;
	case CERTUSER_ARRAY_T:
		getVal2((char *)value,&p1, &p2);
		if (p1 == NULL || p2 = NULL) {
			printf("Invalid CERTPR arguments!\n");
			break;
		}
		pCertUser = (CERTUSER_Tp)(((long)pMib)+mib_table[i].offset+certUserNum*sizeof(CERTUSER_T));
		strcpy( pCertUser->pass, p1 );
		strcpy( pCertUser->comment, p2 );
		certUserNum++;
		break;		
#endif

	case DHCPRSVDIP_ARRY_T:
		getVal3((char *)value, &p1, &p2, &p3);
		if (p1 == NULL || p2 == NULL || p3 == NULL) {
			printf("Invalid DHCPRSVDIP in argument!\n");
			break;
		}	
		if (strlen(p2)!=12 || !string_to_hex(p2, key, 12))
			return -1;
		pDhcpRsvd= (DHCPRSVDIP_Tp)(((long)pMib)+mib_table[i].offset+dhcpRsvdIpNum*sizeof(DHCPRSVDIP_T));
		strcpy(pDhcpRsvd->hostName, p1);		
		memcpy(pDhcpRsvd->macAddr, key, 6);
		if( !inet_aton(p3, &pDhcpRsvd->ipAddr))
			return -1;
		dhcpRsvdIpNum++;
		break;
	case SCHEDULE_ARRAY_T:
	//may not used
		break;
#if defined(VLAN_CONFIG_SUPPORTED)		
	case VLANCONFIG_ARRAY_T:
#if defined(CONFIG_RTK_VLAN_ROUTETYPE)
	getVal7((char *)value, &p1, &p2, &p3, &p4, &p5, &p6, &p7);
#else
	getVal6((char *)value, &p1, &p2, &p3, &p4, &p5, &p6);
#endif
		if (p1 == NULL || p2 == NULL || p3 == NULL || p4 == NULL || p5 == NULL || p6 == NULL ) {
			printf("Invalid VLAN Config arguments!\n");
			break;
		}	
		if (p2){
			pVlanConfig = (VLAN_CONFIG_Tp)(((long)pMib)+mib_table[i].offset);
			for(j=0;j<vlanConfigNum;j++){
			if(!strcmp((pVlanConfig+(j*sizeof(VLAN_CONFIG_T)))->netIface, p2){
				pVlanConfig =  (VLAN_CONFIG_Tp)(((long)pMib)+mib_table[i].offset+(j*sizeof(VLAN_CONFIG_T)));
				pVlanConfig->enabled = (unsigned char)atoi(p1);
				pVlanConfig->tagged = (unsigned char)atoi(p3);
				pVlanConfig->priority = (unsigned char)atoi(p4);
				pVlanConfig->cfi = (unsigned char)atoi(p5);
				pVlanConfig->vlanId = (unsigned short)atoi(p6);
#if defined(CONFIG_RTK_VLAN_ROUTETYPE)
				pVlanConfig->forwarding = (unsigned short)atoi(p7);
#endif
	        	}
	        }
        	}
	break;
#endif		
	default:
		return -1;
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
static void getVal2(char *value, char **p1, char **p2)
{
	value = getVal(value, p1);
	if ( value )
		getVal(value, p2);
	else
		*p2 = NULL;
}

#ifdef HOME_GATEWAY
////////////////////////////////////////////////////////////////////////////////
static void getVal3(char *value, char **p1, char **p2, char **p3)
{
	*p1 = *p2 = *p3 = NULL;

	value = getVal(value, p1);
	if ( !value )
		return;
	value = getVal(value, p2);
	if ( !value )
		return;
	getVal(value, p3);
}

////////////////////////////////////////////////////////////////////////////////
static void getVal4(char *value, char **p1, char **p2, char **p3, char **p4)
{
	*p1 = *p2 = *p3 = *p4 = NULL;

	value = getVal(value, p1);
	if ( !value )
		return;
	value = getVal(value, p2);
	if ( !value )
		return;
	value = getVal(value, p3);
	if ( !value )
		return;
	getVal(value, p4);
}

////////////////////////////////////////////////////////////////////////////////
static void getVal5(char *value, char **p1, char **p2, char **p3, char **p4, char **p5)
{
	*p1 = *p2 = *p3 = *p4 = *p5 = NULL;

	value = getVal(value, p1);
	if ( !value )
		return;
	value = getVal(value, p2);
	if ( !value )
		return;
	value = getVal(value, p3);
	if ( !value )
		return;
	value = getVal(value, p4);
	if ( !value )
		return;
	getVal(value, p5);
}
#endif // HOME_GATEWAY

#endif // PARSE_TXT_FILE

////////////////////////////////////////////////////////////////////////////////
static int getdir(char *fullname, char *path, int loop)
{
	char tmpBuf[100], *p, *p1;

	strcpy(tmpBuf, fullname);
	path[0] = '\0';

	p1 = tmpBuf;
	while (1) {
		if ((p=strchr(p1, '/'))) {
			if (--loop == 0) {
				*p = '\0';
				strcpy(path, tmpBuf);
				return 0;
			}
			p1 = ++p;
		}
		else
			break;
	}
	return -1;
}

////////////////////////////////////////////////////////////////////////////////
static int read_flash_webpage(char *prefix, char *webfile)
{
	WEB_HEADER_T header;
	char *buf, tmpFile[100], tmpFile1[100], tmpBuf[100];
	int fh=0, i, loop, size;
	FILE_ENTRY_Tp pEntry;
	struct stat sbuf;
	char *file;
	if (webfile[0])
		file = webfile;
	else
		file = NULL;

	if (!file) {
		if ( flash_read((char *)&header, WEB_PAGE_OFFSET, sizeof(header)) == 0) {
			printf("Read web header failed!\n");
			return -1;
		}
	}
	else {
		if ((fh = open(file, O_RDONLY)) < 0) {
			printf("Can't open file %s\n", file);
			return -1;
		}
		lseek(fh, 0L, SEEK_SET);
		if (read(fh, &header, sizeof(header)) != sizeof(header)) {
			printf("Read web header failed %s!\n", file);
			close(fh);
			return -1;
		}
	}
#ifndef __mips__
	header.len = DWORD_SWAP(header.len);
#endif

	if (memcmp(header.signature, WEB_HEADER, SIGNATURE_LEN)) {
		printf("Invalid web image! Expect %s\n",WEB_HEADER);
		return -1;
	}

// for debug
//printf("web size=%ld\n", header.len);
	buf = malloc(header.len);
	if (buf == NULL) {
		sprintf(tmpBuf, "Allocate buffer failed %ld!\n", header.len);
		printf(tmpBuf);
		return -1;
	}

	if (!file) {
		if ( flash_read(buf, WEB_PAGE_OFFSET+sizeof(header), header.len) == 0) {
			printf("Read web image failed!\n");
			return -1;
		}
	}
	else {
		if (read(fh, buf, header.len) != header.len) {
			printf("Read web image failed!\n");
			return -1;
		}
		close(fh);
	}

	if ( !CHECKSUM_OK(buf, header.len) ) {
		printf("Web image invalid!\n");
		free(buf);
		return -1;
	}
// for debug
//printf("checksum ok!\n");

	// save to a file
	strcpy(tmpFile, "flashweb.bz2");
	fh = open(tmpFile, O_RDWR|O_CREAT|O_TRUNC);
	if (fh == -1) {
		printf("Create output file error %s!\n", tmpFile );
		return -1;
	}
	if ( write(fh, buf, header.len-1) != header.len -1) {
		printf("write file error %s!\n", tmpFile);
		return -1;
	}
	close(fh);
	free(buf);
	sync();

	// decompress file
	sprintf(tmpFile1, "%sXXXXXX", tmpFile);
	mkstemp(tmpFile1);

	sprintf(tmpBuf, "bunzip2 -c %s > %s", tmpFile, tmpFile1);
	system(tmpBuf);

	unlink(tmpFile);
	sync();

	if (stat(tmpFile1, &sbuf) != 0) {
		printf("Stat file error %s!\n", tmpFile1);
		return -1;
	}
	if (sbuf.st_size < sizeof(FILE_ENTRY_T) ) {
		sprintf(tmpBuf, "Invalid decompress file size %ld!\n", sbuf.st_size);
		printf(tmpBuf);
		unlink(tmpFile1);
		return -1;
	}
// for debug
//printf("decompress size=%ld\n", sbuf.st_size);

	buf = malloc(sbuf.st_size);
	if (buf == NULL) {
		sprintf(tmpBuf,"Allocate buffer failed %ld!\n", sbuf.st_size);
		printf(tmpBuf);
		return -1;
	}
	if ((fh = open(tmpFile1, O_RDONLY)) < 0) {
		printf("Can't open file %s\n", tmpFile1);
		return -1;
	}
	lseek(fh, 0L, SEEK_SET);
	if ( read(fh, buf, sbuf.st_size) != sbuf.st_size) {
		printf("Read file error %ld!\n", sbuf.st_size);
		return -1;
	}
	close(fh);
	unlink(tmpFile1);
	sync();
	size = sbuf.st_size;
	for (i=0; i<size; ) {
		pEntry = (FILE_ENTRY_Tp)&buf[i];

#ifndef __mips__
		pEntry->size = DWORD_SWAP(pEntry->size);
#endif

		strcpy(tmpFile, prefix);
		strcat(tmpFile, "/");
		strcat(tmpFile, pEntry->name);

		loop = 0;
		while (1) {
			if (getdir(tmpFile, tmpBuf, ++loop) < 0)
				break;
			if (tmpBuf[0] && stat(tmpBuf, &sbuf) < 0) { // not exist
 				if ( mkdir(tmpBuf, S_IREAD|S_IWRITE) < 0) {
					printf("Create directory %s failed!\n", tmpBuf);
					return -1;
				}
			}
		}
// for debug
//printf("write file %s, size=%ld\n", tmpFile, pEntry->size);

		fh = open(tmpFile, O_RDWR|O_CREAT|O_TRUNC);
		if (fh == -1) {
			printf("Create output file error %s!\n", tmpFile );
			return -1;
		}
// for debug
//if ( (i+sizeof(FILE_ENTRY_T)+pEntry->size) > size ) {
//printf("error in size, %ld !\n", pEntry->size);
//}
		if ( write(fh, &buf[i+sizeof(FILE_ENTRY_T)], pEntry->size) != pEntry->size ) {
			printf("Write file error %s, len=%ld!\n", tmpFile, pEntry->size);
			return -1;
		}
		close(fh);
		// always set execuatble for script file
//		chmod(tmpFile,  S_IXUSR);

		i += (pEntry->size + sizeof(FILE_ENTRY_T));
	}

	return 0;
}

#ifdef VPN_SUPPORT
static int read_flash_rsa(char *outputFile)
{
	int fh;
	char *rsaBuf;

	if ( !apmib_init()) {
		printf("Initialize AP MIB failed!\n");
		return -1;
	}

	fh = open(outputFile,  O_RDWR|O_CREAT);
	if (fh == -1) {
		printf("Create WPA config file error!\n");
		return -1;
	}
	rsaBuf = malloc(sizeof(unsigned char) * MAX_RSA_FILE_LEN);
	apmib_get( MIB_IPSEC_RSA_FILE, (void *)rsaBuf);
	write(fh, rsaBuf, MAX_RSA_FILE_LEN);
	close(fh);
	free(rsaBuf);
	chmod(outputFile,  DEFFILEMODE);
	return 0;
}
#endif
#ifdef TLS_CLIENT
////////////////////////////////////////////////////////////////////////////////
static int read_flash_cert(char *prefix, char *certfile)
{
	CERT_HEADER_T header;
	char *buf, tmpFile[100], tmpFile1[100], tmpBuf[100];
	int fh=0, i, loop, size;
	FILE_ENTRY_Tp pEntry;
	struct stat sbuf;
	char *file;

	if (certfile[0])
		file = certfile;
	else
		file = NULL;

	if (!file) {
		if ( flash_read((char *)&header, CERT_PAGE_OFFSET, sizeof(header)) == 0) {
			printf("Read web header failed!\n");
			return -1;
		}
	}
	else {
		if ((fh = open(file, O_RDONLY)) < 0) {
			printf("Can't open file %s\n", file);
			return -1;
		}
		lseek(fh, 0L, SEEK_SET);
		if (read(fh, &header, sizeof(header)) != sizeof(header)) {
			printf("Read web header failed %s!\n", file);
			close(fh);
			return -1;
		}
	}
#ifndef __mips__
	header.len = DWORD_SWAP(header.len);
#endif
	
	if (memcmp(header.signature, CERT_HEADER, SIGNATURE_LEN)) {
		printf("Invalid cert image!\n");
		return -1;
	}

// for debug
//printf("web size=%ld\n", header.len);
	buf = malloc(header.len);
	if (buf == NULL) {
		sprintf(tmpBuf, "Allocate buffer failed %ld!\n", header.len);
		printf(tmpBuf);
		return -1;
	}

	if (!file) {
		if ( flash_read(buf, CERT_PAGE_OFFSET+sizeof(header), header.len) == 0) {
			printf("Read web image failed!\n");
			return -1;
		}
	}
	else {
		if (read(fh, buf, header.len) != header.len) {
			printf("Read web image failed!\n");
			return -1;
		}
		close(fh);
	}

	if ( !CHECKSUM_OK(buf, header.len) ) {
		printf("Web image invalid!\n");
		free(buf);
		return -1;
	}
// for debug
//printf("checksum ok!\n");

	// save to a file
	strcpy(tmpFile, "/tmp/cert.tmp");
	fh = open(tmpFile, O_RDWR|O_CREAT|O_TRUNC);
	if (fh == -1) {
		printf("Create output file error %s!\n", tmpFile );
		return -1;
	}
	if ( write(fh, buf, header.len-1) != header.len -1) {
		printf("write file error %s!\n", tmpFile);
		return -1;
	}
	close(fh);
	free(buf);
	sync();

	// decompress file
	sprintf(tmpFile1, "%sXXXXXX", tmpFile);
	mkstemp(tmpFile1);

	//sprintf(tmpBuf, "bunzip2 -c %s > %s", tmpFile, tmpFile1);
	sprintf(tmpBuf, "cat %s  > %s", tmpFile, tmpFile1);
	system(tmpBuf);

	unlink(tmpFile);
	sync();

	if (stat(tmpFile1, &sbuf) != 0) {
		printf("Stat file error %s!\n", tmpFile1);
		return -1;
	}
	if (sbuf.st_size < sizeof(FILE_ENTRY_T) ) {
		sprintf(tmpBuf, "Invalid decompress file size %ld!\n", sbuf.st_size);
		printf(tmpBuf);
		unlink(tmpFile1);
		return -1;
	}
// for debug
//printf("decompress size=%ld\n", sbuf.st_size);

	buf = malloc(sbuf.st_size);
	if (buf == NULL) {
		sprintf(tmpBuf,"Allocate buffer failed %ld!\n", sbuf.st_size);
		printf(tmpBuf);
		return -1;
	}
	if ((fh = open(tmpFile1, O_RDONLY)) < 0) {
		printf("Can't open file %s\n", tmpFile1);
		return -1;
	}
	lseek(fh, 0L, SEEK_SET);
	if ( read(fh, buf, sbuf.st_size) != sbuf.st_size) {
		printf("Read file error %ld!\n", sbuf.st_size);
		return -1;
	}
	close(fh);
	unlink(tmpFile1);
	sync();
	size = sbuf.st_size;
	for (i=0; i<size; ) {
		pEntry = (FILE_ENTRY_Tp)&buf[i];

#ifndef __mips__
		pEntry->size = DWORD_SWAP(pEntry->size);
#endif

		strcpy(tmpFile, prefix);
		strcat(tmpFile, "/");
		strcat(tmpFile, pEntry->name);
		if(!strcmp(pEntry->name , ""))
			break;
		//printf("name = %s\n", pEntry->name);	
		loop = 0;
		while (1) {
			if (getdir(tmpFile, tmpBuf, ++loop) < 0)
				break;
			if (tmpBuf[0] && stat(tmpBuf, &sbuf) < 0) { // not exist
 				if ( mkdir(tmpBuf, S_IREAD|S_IWRITE) < 0) {
					printf("Create directory %s failed!\n", tmpBuf);
					return -1;
				}
			}
		}
// for debug
//printf("write file %s, size=%ld\n", tmpFile, pEntry->size);

		fh = open(tmpFile, O_RDWR|O_CREAT|O_TRUNC);
		if (fh == -1) {
			printf("Create output file error %s!\n", tmpFile );
			return -1;
		}
// for debug
//if ( (i+sizeof(FILE_ENTRY_T)+pEntry->size) > size ) {
//printf("error in size, %ld !\n", pEntry->size);
//}

		if ( write(fh, &buf[i+sizeof(FILE_ENTRY_T)], pEntry->size) != pEntry->size ) {
			printf("Write file error %s, len=%ld!\n", tmpFile, pEntry->size);
			return -1;
		}
		close(fh);
		// always set execuatble for script file
//		chmod(tmpFile,  S_IXUSR);

		i += (pEntry->size + sizeof(FILE_ENTRY_T));
	}

	return 0;
}
#endif
////////////////////////////////////////////////////////////////////////////////
static void __inline__ WRITE_WPA_FILE(int fh, unsigned char *buf)
{
	if ( write(fh, buf, strlen(buf)) != strlen(buf) ) {
		printf("Write WPA config file error!\n");
		close(fh);
		exit(1);
	}
}

////////////////////////////////////////////////////////////////////////////////
static void generateWpaConf(char *outputFile, int isWds)
{
	int fh, intVal, encrypt, enable1x, wep;
	unsigned char buf1[1024], buf2[1024];

#if 0
//#ifdef UNIVERSAL_REPEATER	
	int isVxd = 0;
	
	if (strstr(outputFile, "-vxd")) 
		isVxd = 1;	
#endif		
	
	if ( !apmib_init()) {
		printf("Initialize AP MIB failed!\n");
		return;
	}

	fh = open(outputFile, O_RDWR|O_CREAT|O_TRUNC);
	if (fh == -1) {
		printf("Create WPA config file error!\n");
		return;
	}
	if (!isWds) {
	apmib_get( MIB_WLAN_ENCRYPT, (void *)&encrypt);

#if 0
//#ifdef UNIVERSAL_REPEATER
	if (isVxd && (encrypt == ENCRYPT_WPA2_MIXED)) {
		apmib_get( MIB_WLAN_MODE, (void *)&intVal);
		if (intVal == AP_MODE || intVal == AP_WDS_MODE) 
			encrypt = ENCRYPT_WPA;		
	}
#endif			
	
	sprintf(buf2, "encryption = %d\n", encrypt);
	WRITE_WPA_FILE(fh, buf2);

#if 0
//#ifdef UNIVERSAL_REPEATER
	if (isVxd) {
		if (strstr(outputFile, "wlan0-vxd"))
			apmib_get( MIB_REPEATER_SSID1, (void *)buf1);		
		else			
			apmib_get( MIB_REPEATER_SSID2, (void *)buf1);	
	}
	else
#endif
	apmib_get( MIB_WLAN_SSID,  (void *)buf1);
	sprintf(buf2, "ssid = \"%s\"\n", buf1);
	WRITE_WPA_FILE(fh, buf2);

	apmib_get( MIB_WLAN_ENABLE_1X, (void *)&enable1x);
	sprintf(buf2, "enable1x = %d\n", enable1x);
	WRITE_WPA_FILE(fh, buf2);

	apmib_get( MIB_WLAN_ENABLE_MAC_AUTH, (void *)&intVal);
	sprintf(buf2, "enableMacAuth = %d\n", intVal);
	WRITE_WPA_FILE(fh, buf2);

	apmib_get( MIB_WLAN_ENABLE_SUPP_NONWPA, (void *)&intVal);
	if (intVal)
		apmib_get( MIB_WLAN_SUPP_NONWPA, (void *)&intVal);

	sprintf(buf2, "supportNonWpaClient = %d\n", intVal);
	WRITE_WPA_FILE(fh, buf2);

	apmib_get( MIB_WLAN_WEP, (void *)&wep);
	sprintf(buf2, "wepKey = %d\n", wep);
	WRITE_WPA_FILE(fh, buf2);

	if ( encrypt==1 && enable1x ) {
		if (wep == 1) {
			apmib_get( MIB_WLAN_WEP64_KEY1, (void *)buf1);
			sprintf(buf2, "wepGroupKey = \"%02x%02x%02x%02x%02x\"\n", buf1[0],buf1[1],buf1[2],buf1[3],buf1[4]);
		}
		else {
			apmib_get( MIB_WLAN_WEP128_KEY1, (void *)buf1);
			sprintf(buf2, "wepGroupKey = \"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\"\n",
				buf1[0],buf1[1],buf1[2],buf1[3],buf1[4],
				buf1[5],buf1[6],buf1[7],buf1[8],buf1[9],
				buf1[10],buf1[11],buf1[12]);
		}
	}
	else
		strcpy(buf2, "wepGroupKey = \"\"\n");
	WRITE_WPA_FILE(fh, buf2);

	apmib_get( MIB_WLAN_WPA_AUTH, (void *)&intVal);
	sprintf(buf2, "authentication = %d\n", intVal);
	WRITE_WPA_FILE(fh, buf2);

	apmib_get( MIB_WLAN_WPA_CIPHER_SUITE, (void *)&intVal);
	sprintf(buf2, "unicastCipher = %d\n", intVal);
	WRITE_WPA_FILE(fh, buf2);

	apmib_get( MIB_WLAN_WPA2_CIPHER_SUITE, (void *)&intVal);
	sprintf(buf2, "wpa2UnicastCipher = %d\n", intVal);
	WRITE_WPA_FILE(fh, buf2);

	apmib_get( MIB_WLAN_WPA2_PRE_AUTH, (void *)&intVal);
	sprintf(buf2, "enablePreAuth = %d\n", intVal);
	WRITE_WPA_FILE(fh, buf2);

	apmib_get( MIB_WLAN_WPA_PSK_FORMAT, (void *)&intVal);
	if (intVal==0)
		sprintf(buf2, "usePassphrase = 1\n");
	else
		sprintf(buf2, "usePassphrase = 0\n");
	WRITE_WPA_FILE(fh, buf2);

	apmib_get( MIB_WLAN_WPA_PSK, (void *)buf1);
	sprintf(buf2, "psk = \"%s\"\n", buf1);
	WRITE_WPA_FILE(fh, buf2);

	apmib_get( MIB_WLAN_WPA_GROUP_REKEY_TIME, (void *)&intVal);
	sprintf(buf2, "groupRekeyTime = %d\n", intVal);
	WRITE_WPA_FILE(fh, buf2);

	apmib_get( MIB_WLAN_RS_PORT, (void *)&intVal);
	sprintf(buf2, "rsPort = %d\n", intVal);
	WRITE_WPA_FILE(fh, buf2);

	apmib_get( MIB_WLAN_RS_IP, (void *)buf1);
	sprintf(buf2, "rsIP = %s\n", inet_ntoa(*((struct in_addr *)buf1)));
	WRITE_WPA_FILE(fh, buf2);

	apmib_get( MIB_WLAN_RS_PASSWORD, (void *)buf1);
	sprintf(buf2, "rsPassword = \"%s\"\n", buf1);
	WRITE_WPA_FILE(fh, buf2);

	apmib_get( MIB_WLAN_RS_RETRY, (void *)&intVal);
	sprintf(buf2, "rsMaxReq = %d\n", intVal);
	WRITE_WPA_FILE(fh, buf2);

	apmib_get( MIB_WLAN_RS_INTERVAL_TIME, (void *)&intVal);
	sprintf(buf2, "rsAWhile = %d\n", intVal);
	WRITE_WPA_FILE(fh, buf2);

	apmib_get( MIB_WLAN_ACCOUNT_RS_ENABLED, (void *)&intVal);
	sprintf(buf2, "accountRsEnabled = %d\n", intVal);
	WRITE_WPA_FILE(fh, buf2);

	apmib_get( MIB_WLAN_ACCOUNT_RS_PORT, (void *)&intVal);
	sprintf(buf2, "accountRsPort = %d\n", intVal);
	WRITE_WPA_FILE(fh, buf2);

	apmib_get( MIB_WLAN_ACCOUNT_RS_IP, (void *)buf1);
	sprintf(buf2, "accountRsIP = %s\n", inet_ntoa(*((struct in_addr *)buf1)));
	WRITE_WPA_FILE(fh, buf2);

	apmib_get( MIB_WLAN_ACCOUNT_RS_PASSWORD, (void *)buf1);
	sprintf(buf2, "accountRsPassword = \"%s\"\n", buf1);
	WRITE_WPA_FILE(fh, buf2);

	apmib_get( MIB_WLAN_ACCOUNT_UPDATE_ENABLED, (void *)&intVal);
	sprintf(buf2, "accountRsUpdateEnabled = %d\n", intVal);
	WRITE_WPA_FILE(fh, buf2);

	apmib_get( MIB_WLAN_ACCOUNT_UPDATE_DELAY, (void *)&intVal);
	sprintf(buf2, "accountRsUpdateTime = %d\n", intVal);
	WRITE_WPA_FILE(fh, buf2);

	apmib_get( MIB_WLAN_ACCOUNT_RS_RETRY, (void *)&intVal);
	sprintf(buf2, "accountRsMaxReq = %d\n", intVal);
	WRITE_WPA_FILE(fh, buf2);

	apmib_get( MIB_WLAN_ACCOUNT_RS_INTERVAL_TIME, (void *)&intVal);
	sprintf(buf2, "accountRsAWhile = %d\n", intVal);
	WRITE_WPA_FILE(fh, buf2);
	}

#ifdef CONFIG_RTK_MESH
	else if (isWds==7) {
		
		apmib_get( MIB_MESH_ENCRYPT, (void *)&encrypt);	
		sprintf(buf2, "encryption = %d\n", encrypt);
		WRITE_WPA_FILE(fh, buf2);

		apmib_get( MIB_MESH_ID,  (void *)buf1);
		sprintf(buf2, "ssid = \"%s\"\n", buf1);
		WRITE_WPA_FILE(fh, buf2);
		
		WRITE_WPA_FILE(fh, "enable1x = 0\n");
		WRITE_WPA_FILE(fh, "enableMacAuth = 0\n");
		WRITE_WPA_FILE(fh, "supportNonWpaClient = 0\n");
		WRITE_WPA_FILE(fh, "wepKey = 0\n");
		WRITE_WPA_FILE(fh,  "wepGroupKey = \"\"\n");

		apmib_get( MIB_MESH_WPA_AUTH, (void *)&intVal);
		sprintf(buf2, "authentication = %d\n", intVal);
		WRITE_WPA_FILE(fh, buf2);

		//sprintf(buf2, "unicastCipher = %d\n", intVal);
		sprintf(buf2, "unicastCipher = 1\n");
		WRITE_WPA_FILE(fh, buf2);
		
		apmib_get( MIB_MESH_WPA2_CIPHER_SUITE, (void *)&intVal);	

		sprintf(buf2, "wpa2UnicastCipher = %d\n", intVal);
		WRITE_WPA_FILE(fh, buf2);

		WRITE_WPA_FILE(fh, "enablePreAuth = 0\n");

		apmib_get( MIB_MESH_WPA_PSK_FORMAT, (void *)&intVal);
		if (intVal==0)
			sprintf(buf2, "usePassphrase = 1\n");
		else
			sprintf(buf2, "usePassphrase = 0\n");
		WRITE_WPA_FILE(fh, buf2);

		apmib_get( MIB_MESH_WPA_PSK, (void *)buf1);
		sprintf(buf2, "psk = \"%s\"\n", buf1);
		WRITE_WPA_FILE(fh, buf2);

		WRITE_WPA_FILE(fh, "groupRekeyTime = 86400\n");
		WRITE_WPA_FILE(fh, "rsPort = 1812\n");
		WRITE_WPA_FILE(fh, "rsIP = 0.0.0.0\n");
		WRITE_WPA_FILE(fh, "rsPassword = \"\"\n");
		WRITE_WPA_FILE(fh, "rsMaxReq = 3\n");
		WRITE_WPA_FILE(fh, "rsAWhile = 5\n");
		WRITE_WPA_FILE(fh, "accountRsEnabled = 0\n");
		WRITE_WPA_FILE(fh, "accountRsPort = 1813\n");
		WRITE_WPA_FILE(fh, "accountRsIP = 0.0.0.0\n");
		WRITE_WPA_FILE(fh, "accountRsPassword = \"\"\n");
		WRITE_WPA_FILE(fh, "accountRsUpdateEnabled = 0\n");
		WRITE_WPA_FILE(fh, "accountRsUpdateTime = 60\n");
		WRITE_WPA_FILE(fh, "accountRsMaxReq = 3\n");
		WRITE_WPA_FILE(fh, "accountRsAWhile = 5\n");
	}
#endif // CONFIG_RTK_MESH

	else {
		apmib_get( MIB_WLAN_WDS_ENCRYPT, (void *)&encrypt);
		if (encrypt == WDS_ENCRYPT_TKIP)		
			encrypt = ENCRYPT_WPA;
		else if (encrypt == WDS_ENCRYPT_AES)		
			encrypt = ENCRYPT_WPA2;		
		else
			encrypt = 0;
	
		sprintf(buf2, "encryption = %d\n", encrypt);
		WRITE_WPA_FILE(fh, buf2);
		WRITE_WPA_FILE(fh, "ssid = \"REALTEK\"\n");
		WRITE_WPA_FILE(fh, "enable1x = 1\n");
		WRITE_WPA_FILE(fh, "enableMacAuth = 0\n");
		WRITE_WPA_FILE(fh, "supportNonWpaClient = 0\n");
		WRITE_WPA_FILE(fh, "wepKey = 0\n");
		WRITE_WPA_FILE(fh,  "wepGroupKey = \"\"\n");
		WRITE_WPA_FILE(fh,  "authentication = 2\n");

		if (encrypt == ENCRYPT_WPA)
			intVal = WPA_CIPHER_TKIP;
		else
			intVal = WPA_CIPHER_AES;
			
		sprintf(buf2, "unicastCipher = %d\n", intVal);
		WRITE_WPA_FILE(fh, buf2);

		sprintf(buf2, "wpa2UnicastCipher = %d\n", intVal);
		WRITE_WPA_FILE(fh, buf2);

		WRITE_WPA_FILE(fh, "enablePreAuth = 0\n");

		apmib_get( MIB_WLAN_WDS_PSK_FORMAT, (void *)&intVal);
		if (intVal==0)
			sprintf(buf2, "usePassphrase = 1\n");
		else
			sprintf(buf2, "usePassphrase = 0\n");
		WRITE_WPA_FILE(fh, buf2);

		apmib_get( MIB_WLAN_WDS_PSK, (void *)buf1);
		sprintf(buf2, "psk = \"%s\"\n", buf1);
		WRITE_WPA_FILE(fh, buf2);

		WRITE_WPA_FILE(fh, "groupRekeyTime = 0\n");
		WRITE_WPA_FILE(fh, "rsPort = 1812\n");
		WRITE_WPA_FILE(fh, "rsIP = 192.168.1.1\n");
		WRITE_WPA_FILE(fh, "rsPassword = \"\"\n");
		WRITE_WPA_FILE(fh, "rsMaxReq = 3\n");
		WRITE_WPA_FILE(fh, "rsAWhile = 10\n");
		WRITE_WPA_FILE(fh, "accountRsEnabled = 0\n");
		WRITE_WPA_FILE(fh, "accountRsPort = 1813\n");
		WRITE_WPA_FILE(fh, "accountRsIP = 192.168.1.1\n");
		WRITE_WPA_FILE(fh, "accountRsPassword = \"\"\n");
		WRITE_WPA_FILE(fh, "accountRsUpdateEnabled = 0\n");
		WRITE_WPA_FILE(fh, "accountRsUpdateTime = 1000\n");
		WRITE_WPA_FILE(fh, "accountRsMaxReq = 3\n");
		WRITE_WPA_FILE(fh, "accountRsAWhile = 1\n");
	}

	close(fh);
}

////////////////////////////////////////////////////////////////////////////////
#ifdef WLAN_FAST_INIT

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/wireless.h>

#if defined(CONFIG_NET_RADIO)
#if defined(CONFIG_RTL8192CD)
	#include "../../linux-2.4.18/drivers/net/rtl8192cd/ieee802_mib.h"
#else
	#include "../../linux-2.4.18/drivers/net/rtl8190/ieee802_mib.h"
#endif
#endif //#if defined(CONFIG_NET_RADIO)

#if defined(CONFIG_NET_RADIO)
static int initWlan(char *ifname)
{
	struct wifi_mib *pmib;
	int i, intVal, intVal2, encrypt, enable1x, wep, mode/*, enable1xVxd*/;
	unsigned char buf1[1024], buf2[1024], mac[6];
	int skfd;
	struct iwreq wrq, wrq_root;
	int wlan_band=0, channel_bound=0, aggregation=0;
	MACFILTER_T *pAcl=NULL;
	struct wdsEntry *wds_Entry=NULL;
	WDS_Tp pwds_EntryUI;
#ifdef MBSSID
	int v_previous=0;
#ifdef CONFIG_RTL8196B
	int vap_enable=0, intVal4=0;
#endif
#endif
	
	skfd = socket(AF_INET, SOCK_DGRAM, 0);
	strncpy(wrq.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(skfd, SIOCGIWNAME, &wrq) < 0) {
		printf("Interface open failed!\n");
		return -1;
	}

	if ((pmib = (struct wifi_mib *)malloc(sizeof(struct wifi_mib))) == NULL) {
		printf("MIB buffer allocation failed!\n");
		return -1;
	}

	if (!apmib_init()) {
		printf("Initialize AP MIB failed!\n");
		return -1;
	}

	// Disable WLAN MAC driver and shutdown interface first
	sprintf(buf1, "ifconfig %s down", ifname);
	system(buf1);

	if (vwlan_idx == 0) {
		// shutdown all WDS interface
		for (i=0; i<8; i++) {
			sprintf(buf1, "ifconfig %s-wds%d down", ifname, i);
			system(buf1);
		}
	
		// kill wlan application daemon
		sprintf(buf1, "wlanapp.sh kill %s", ifname);
		system(buf1);
	}
	else { // virtual interface
		sprintf(buf1, "wlan%d", wlan_idx);		
		strncpy(wrq_root.ifr_name, buf1, IFNAMSIZ);
		if (ioctl(skfd, SIOCGIWNAME, &wrq_root) < 0) {
			printf("Root Interface open failed!\n");
			return -1;
		}		
	}

#if 0	// unnecessary
	if (vwlan_idx == 0) {
		apmib_get(MIB_HW_RF_TYPE, (void *)&intVal);
		if (intVal == 0) {
			printf("RF type is NULL!\n");
			return 0;
		}
	}
#endif
	apmib_get(MIB_WLAN_DISABLED, (void *)&intVal);

	if (intVal == 1) {
		free(pmib);
		close(skfd);		
		return 0;
	}

	// get mib from driver
	wrq.u.data.pointer = (caddr_t)pmib;
	wrq.u.data.length = sizeof(struct wifi_mib);

	if (vwlan_idx == 0) {
		if (ioctl(skfd, 0x8B42, &wrq) < 0) {
			printf("Get WLAN MIB failed!\n");
			return -1;
		}
	}
	else {
		wrq_root.u.data.pointer = (caddr_t)pmib;
		wrq_root.u.data.length = sizeof(struct wifi_mib);				
		if (ioctl(skfd, 0x8B42, &wrq_root) < 0) {
			printf("Get WLAN MIB failed!\n");
			return -1;
		}		
	}

	// check mib version
	if (pmib->mib_version != MIB_VERSION) {
		printf("WLAN MIB version mismatch!\n");
		return -1;
	}

	if (vwlan_idx > 0) {	//if not root interface, clone root mib to virtual interface
		wrq.u.data.pointer = (caddr_t)pmib;
		wrq.u.data.length = sizeof(struct wifi_mib);
		if (ioctl(skfd, 0x8B43, &wrq) < 0) {
			printf("Set WLAN MIB failed!\n");
			return -1;
		}	
		pmib->miscEntry.func_off = 0;		
	}

	// Set parameters to driver
	if (vwlan_idx == 0) {	
		apmib_get(MIB_HW_REG_DOMAIN, (void *)&intVal);
		pmib->dot11StationConfigEntry.dot11RegDomain = intVal;
	}

	apmib_get(MIB_WLAN_MAC_ADDR, (void *)mac);
	if (!memcmp(mac, "\x00\x00\x00\x00\x00\x00", 6)) {
#ifdef MBSSID
		if (vwlan_idx > 0 && vwlan_idx != NUM_VWLAN_INTERFACE) {
			switch (vwlan_idx)
			{
				case 1:
					apmib_get(MIB_HW_WLAN_ADDR1, (void *)mac);
					break;
				case 2:
					apmib_get(MIB_HW_WLAN_ADDR2, (void *)mac);
					break;
				case 3:
					apmib_get(MIB_HW_WLAN_ADDR3, (void *)mac);
					break;
				case 4:
					apmib_get(MIB_HW_WLAN_ADDR4, (void *)mac);
					break;
				default:
					printf("Fail to get MAC address of VAP%d!\n", vwlan_idx-1);
					return 0;
			}
		}
		else
#endif
		apmib_get(MIB_HW_WLAN_ADDR, (void *)mac);
	}

	// ifconfig all wlan interface when not in WISP
	// ifconfig wlan1 later interface when in WISP mode, the wlan0  will be setup in WAN interface
	apmib_get(MIB_OP_MODE, (void *)&intVal);
	apmib_get(MIB_WISP_WAN_ID, (void *)&intVal2);
	sprintf(buf1, "wlan%d", intVal2);
	if ((intVal != 2) ||
#ifdef MBSSID
		vwlan_idx > 0 ||
#endif
		strcmp(ifname, buf1)) {
		sprintf(buf2, "ifconfig %s hw ether %02x%02x%02x%02x%02x%02x", ifname, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		system(buf2);
		memcpy(&(pmib->dot11OperationEntry.hwaddr[0]), mac, 6);
	}

#ifdef BR_SHORTCUT	
	if (intVal == 2
#ifdef MBSSID
		&& vwlan_idx == 0
#endif
	) 
		pmib->dot11OperationEntry.disable_brsc = 1;
#endif
	
	apmib_get(MIB_HW_LED_TYPE, (void *)&intVal);
	pmib->dot11OperationEntry.ledtype = intVal;

	// set AP/client/WDS mode
	apmib_get(MIB_WLAN_SSID, (void *)buf1);
	intVal2 = strlen(buf1);
	pmib->dot11StationConfigEntry.dot11DesiredSSIDLen = intVal2;
	memset(pmib->dot11StationConfigEntry.dot11DesiredSSID, 0, 32);
	memcpy(pmib->dot11StationConfigEntry.dot11DesiredSSID, buf1, intVal2);
	if ((pmib->dot11StationConfigEntry.dot11DesiredSSIDLen == 3) &&
		((pmib->dot11StationConfigEntry.dot11DesiredSSID[0] == 'A') || (pmib->dot11StationConfigEntry.dot11DesiredSSID[0] == 'a')) &&
		((pmib->dot11StationConfigEntry.dot11DesiredSSID[1] == 'N') || (pmib->dot11StationConfigEntry.dot11DesiredSSID[1] == 'n')) &&
		((pmib->dot11StationConfigEntry.dot11DesiredSSID[2] == 'Y') || (pmib->dot11StationConfigEntry.dot11DesiredSSID[2] == 'y'))) {
		pmib->dot11StationConfigEntry.dot11SSIDtoScanLen = 0;
		memset(pmib->dot11StationConfigEntry.dot11SSIDtoScan, 0, 32);
	}
	else {
		pmib->dot11StationConfigEntry.dot11SSIDtoScanLen = intVal2;
		memset(pmib->dot11StationConfigEntry.dot11SSIDtoScan, 0, 32);
		memcpy(pmib->dot11StationConfigEntry.dot11SSIDtoScan, buf1, intVal2);
	}

	apmib_get(MIB_WLAN_MODE, (void *)&mode);
	if (mode == 1) {
		// client mode
		apmib_get(MIB_WLAN_NETWORK_TYPE, (void *)&intVal2);
		if (intVal2 == 0)
			pmib->dot11OperationEntry.opmode = 8;
		else {
			pmib->dot11OperationEntry.opmode = 32;
			apmib_get(MIB_WLAN_DEFAULT_SSID, (void *)buf1);
			intVal2 = strlen(buf1);
			pmib->dot11StationConfigEntry.dot11DefaultSSIDLen = intVal2;
			memset(pmib->dot11StationConfigEntry.dot11DefaultSSID, 0, 32);
			memcpy(pmib->dot11StationConfigEntry.dot11DefaultSSID, buf1, intVal2);
		}
	}
	else
		pmib->dot11OperationEntry.opmode = 16;

	if (mode == 2)	// WDS only
		pmib->dot11WdsInfo.wdsPure = 1;
	else
		pmib->dot11WdsInfo.wdsPure = 0;

	if (vwlan_idx == 0) { // root interface	
		// set RF parameters
		apmib_get(MIB_HW_RF_TYPE, (void *)&intVal);
		pmib->dot11RFEntry.dot11RFType = intVal;
		
#if defined(CONFIG_RTL8196B)
	#if defined(CONFIG_RTL8198)
		apmib_get(MIB_HW_BOARD_VER, (void *)&intVal);
	if (intVal == 1)
		pmib->dot11RFEntry.MIMO_TR_mode = 3;	// 2T2R
	else if(intVal == 2)
		pmib->dot11RFEntry.MIMO_TR_mode = 4; // 1T1R
	else
		pmib->dot11RFEntry.MIMO_TR_mode = 1;	// 1T2R
	apmib_get(MIB_HW_TX_POWER_CCK_A, (void *)buf1);
	memcpy(pmib->dot11RFEntry.pwrlevelCCK_A, buf1, MAX_2G_CHANNEL_NUM_MIB);	
	
	apmib_get(MIB_HW_TX_POWER_CCK_B, (void *)buf1);
	memcpy(pmib->dot11RFEntry.pwrlevelCCK_B, buf1, MAX_2G_CHANNEL_NUM_MIB);
	
	apmib_get(MIB_HW_TX_POWER_HT40_1S_A, (void *)buf1);
	memcpy(pmib->dot11RFEntry.pwrlevelHT40_1S_A, buf1, MAX_2G_CHANNEL_NUM_MIB);
	
	apmib_get(MIB_HW_TX_POWER_HT40_1S_B, (void *)buf1);
	memcpy(pmib->dot11RFEntry.pwrlevelHT40_1S_B, buf1, MAX_2G_CHANNEL_NUM_MIB);
	
	apmib_get(MIB_HW_TX_POWER_HT40_2S, (void *)buf1);
	memcpy(pmib->dot11RFEntry.pwrdiffHT40_2S, buf1, MAX_2G_CHANNEL_NUM_MIB);
	
	apmib_get(MIB_HW_TX_POWER_HT20, (void *)buf1);
	memcpy(pmib->dot11RFEntry.pwrdiffHT20, buf1, MAX_2G_CHANNEL_NUM_MIB);
	
	apmib_get(MIB_HW_TX_POWER_DIFF_OFDM, (void *)buf1);
	memcpy(pmib->dot11RFEntry.pwrdiffOFDM, buf1, MAX_2G_CHANNEL_NUM_MIB);
	
	apmib_get(MIB_HW_11N_TSSI1, (void *)&intVal);
	pmib->dot11RFEntry.tssi1 = intVal;

	apmib_get(MIB_HW_11N_TSSI2, (void *)&intVal);
	pmib->dot11RFEntry.tssi2 = intVal;

	apmib_get(MIB_HW_11N_THER, (void *)&intVal);
	pmib->dot11RFEntry.ther = intVal;
	
	apmib_get(MIB_HW_11N_TRSWITCH, (void *)&intVal);
	pmib->dot11RFEntry.trswitch = intVal;
/**************** ToDo**************/
/*		
	if (pmib->dot11RFEntry.dot11RFType == 10) { // Zebra
		apmib_get(MIB_WLAN_RF_POWER, (void *)&intVal);
		if(intVal == 1)
			intVal = 3;
		else if(intVal == 2)
				intVal = 6;
			else if(intVal == 3)
					intVal = 9;
				else if(intVal == 4)
						intVal = 17;
		if (intVal) {
			for (i=0; i<14; i++) {
				if(pmib->dot11RFEntry.pwrlevelCCK_A[i] != 0){ 
					if ((pmib->dot11RFEntry.pwrlevelCCK_A[i] - intVal) >= 1)
						pmib->dot11RFEntry.pwrlevelCCK_A[i] -= intVal;
					else
						pmib->dot11RFEntry.pwrlevelCCK_A[i] = 1;
				}
				if(pmib->dot11RFEntry.pwrlevelCCK_B[i] != 0){ 
					if ((pmib->dot11RFEntry.pwrlevelCCK_B[i] - intVal) >= 1)
						pmib->dot11RFEntry.pwrlevelCCK_B[i] -= intVal;
					else
						pmib->dot11RFEntry.pwrlevelCCK_B[i] = 1;
				}
			}
		}
	}	
*/	
	#elif defined(CONFIG_RTL8196C)
		apmib_get(MIB_HW_BOARD_VER, (void *)&intVal);
	if (intVal == 1)
		pmib->dot11RFEntry.MIMO_TR_mode = 3;	// 2T2R
	else if(intVal == 2)
		pmib->dot11RFEntry.MIMO_TR_mode = 4; // 1T1R
	else
		pmib->dot11RFEntry.MIMO_TR_mode = 1;	// 1T2R
	apmib_get(MIB_HW_TX_POWER_CCK_A, (void *)buf1);
	memcpy(pmib->dot11RFEntry.pwrlevelCCK_A, buf1, MAX_2G_CHANNEL_NUM_MIB);	
	
	apmib_get(MIB_HW_TX_POWER_CCK_B, (void *)buf1);
	memcpy(pmib->dot11RFEntry.pwrlevelCCK_B, buf1, MAX_2G_CHANNEL_NUM_MIB);
	
	apmib_get(MIB_HW_TX_POWER_HT40_1S_A, (void *)buf1);
	memcpy(pmib->dot11RFEntry.pwrlevelHT40_1S_A, buf1, MAX_2G_CHANNEL_NUM_MIB);
	
	apmib_get(MIB_HW_TX_POWER_HT40_1S_B, (void *)buf1);
	memcpy(pmib->dot11RFEntry.pwrlevelHT40_1S_B, buf1, MAX_2G_CHANNEL_NUM_MIB);
	
	apmib_get(MIB_HW_TX_POWER_HT40_2S, (void *)buf1);
	memcpy(pmib->dot11RFEntry.pwrdiffHT40_2S, buf1, MAX_2G_CHANNEL_NUM_MIB);
	
	apmib_get(MIB_HW_TX_POWER_HT20, (void *)buf1);
	memcpy(pmib->dot11RFEntry.pwrdiffHT20, buf1, MAX_2G_CHANNEL_NUM_MIB);
	
	apmib_get(MIB_HW_TX_POWER_DIFF_OFDM, (void *)buf1);
	memcpy(pmib->dot11RFEntry.pwrdiffOFDM, buf1, MAX_2G_CHANNEL_NUM_MIB);
	
	apmib_get(MIB_HW_11N_TSSI1, (void *)&intVal);
	pmib->dot11RFEntry.tssi1 = intVal;

	apmib_get(MIB_HW_11N_TSSI2, (void *)&intVal);
	pmib->dot11RFEntry.tssi2 = intVal;

	apmib_get(MIB_HW_11N_THER, (void *)&intVal);
	pmib->dot11RFEntry.ther = intVal;
	
	apmib_get(MIB_HW_11N_TRSWITCH, (void *)&intVal);
	pmib->dot11RFEntry.trswitch = intVal;
	
	if (pmib->dot11RFEntry.dot11RFType == 10) { // Zebra
		apmib_get(MIB_WLAN_RF_POWER, (void *)&intVal);
		if(intVal == 1)
			intVal = 3;
		else if(intVal == 2)
				intVal = 6;
			else if(intVal == 3)
					intVal = 9;
				else if(intVal == 4)
						intVal = 17;
		if (intVal) {
			for (i=0; i<MAX_2G_CHANNEL_NUM_MIB; i++) {
				if(pmib->dot11RFEntry.pwrlevelCCK_A[i] != 0){ 
					if ((pmib->dot11RFEntry.pwrlevelCCK_A[i] - intVal) >= 1)
						pmib->dot11RFEntry.pwrlevelCCK_A[i] -= intVal;
					else
						pmib->dot11RFEntry.pwrlevelCCK_A[i] = 1;
				}
				if(pmib->dot11RFEntry.pwrlevelCCK_B[i] != 0){ 
					if ((pmib->dot11RFEntry.pwrlevelCCK_B[i] - intVal) >= 1)
						pmib->dot11RFEntry.pwrlevelCCK_B[i] -= intVal;
					else
						pmib->dot11RFEntry.pwrlevelCCK_B[i] = 1;
				}
				if(pmib->dot11RFEntry.pwrlevelHT40_1S_A[i] != 0){ 
					if ((pmib->dot11RFEntry.pwrlevelHT40_1S_A[i] - intVal) >= 1)
						pmib->dot11RFEntry.pwrlevelHT40_1S_A[i] -= intVal;
					else
						pmib->dot11RFEntry.pwrlevelHT40_1S_A[i] = 1;
			}
				if(pmib->dot11RFEntry.pwrlevelHT40_1S_B[i] != 0){ 
					if ((pmib->dot11RFEntry.pwrlevelHT40_1S_B[i] - intVal) >= 1)
						pmib->dot11RFEntry.pwrlevelHT40_1S_B[i] -= intVal;
					else
						pmib->dot11RFEntry.pwrlevelHT40_1S_B[i] = 1;
		}
			}
		}
	}	

	#else
		apmib_get(MIB_HW_BOARD_VER, (void *)&intVal);
		if (intVal == 1)
			pmib->dot11RFEntry.MIMO_TR_mode = 3;	// 2T2R
		else if(intVal == 2)
                        pmib->dot11RFEntry.MIMO_TR_mode = 4; // 1T1R
		else
			pmib->dot11RFEntry.MIMO_TR_mode = 1;	// 1T2R

		apmib_get(MIB_HW_TX_POWER_CCK, (void *)buf1);
		memcpy(pmib->dot11RFEntry.pwrlevelCCK, buf1, 14);
		
		apmib_get(MIB_HW_TX_POWER_OFDM_1S, (void *)buf1);
		memcpy(pmib->dot11RFEntry.pwrlevelOFDM_1SS, buf1, 162);
		
		apmib_get(MIB_HW_TX_POWER_OFDM_2S, (void *)buf1);
		memcpy(pmib->dot11RFEntry.pwrlevelOFDM_2SS, buf1, 162);

		// not used for RTL8192SE
		//apmib_get(MIB_HW_11N_XCAP, (void *)&intVal);
		//pmib->dot11RFEntry.crystalCap = intVal;
		
		apmib_get(MIB_HW_11N_LOFDMPWDA, (void *)&intVal);
		pmib->dot11RFEntry.LOFDM_pwd_A = intVal;

		apmib_get(MIB_HW_11N_LOFDMPWDB, (void *)&intVal);
		pmib->dot11RFEntry.LOFDM_pwd_B = intVal;

		apmib_get(MIB_HW_11N_TSSI1, (void *)&intVal);
		pmib->dot11RFEntry.tssi1 = intVal;

		apmib_get(MIB_HW_11N_TSSI2, (void *)&intVal);
		pmib->dot11RFEntry.tssi2 = intVal;

		apmib_get(MIB_HW_11N_THER, (void *)&intVal);
		pmib->dot11RFEntry.ther = intVal;
				
		if (pmib->dot11RFEntry.dot11RFType == 10) { // Zebra
			apmib_get(MIB_WLAN_RF_POWER, (void *)&intVal);
			if(intVal == 1)
				intVal = 3;
			else if(intVal == 2)
					intVal = 6;
				else if(intVal == 3)
						intVal = 9;
					else if(intVal == 4)
							intVal = 17;
			if (intVal) {
				for (i=0; i<14; i++) {
					if(pmib->dot11RFEntry.pwrlevelCCK[i] != 0){ 
						if ((pmib->dot11RFEntry.pwrlevelCCK[i] - intVal) >= 1)
							pmib->dot11RFEntry.pwrlevelCCK[i] -= intVal;
						else
							pmib->dot11RFEntry.pwrlevelCCK[i] = 1;
					}
				}
				for (i=0; i<162; i++) {
					if (pmib->dot11RFEntry.pwrlevelOFDM_1SS[i] != 0){
						if((pmib->dot11RFEntry.pwrlevelOFDM_1SS[i] - intVal) >= 1)
							pmib->dot11RFEntry.pwrlevelOFDM_1SS[i] -= intVal;
						else
							pmib->dot11RFEntry.pwrlevelOFDM_1SS[i] = 1;
					}
					if (pmib->dot11RFEntry.pwrlevelOFDM_2SS[i] != 0){
						if((pmib->dot11RFEntry.pwrlevelOFDM_2SS[i] - intVal) >= 1)
							pmib->dot11RFEntry.pwrlevelOFDM_2SS[i] -= intVal;
						else
							pmib->dot11RFEntry.pwrlevelOFDM_2SS[i] = 1;
					}
				}		
			}
		}
	
#endif		
#else
//!CONFIG_RTL8196B => rtl8651c+rtl8190
		apmib_get(MIB_HW_ANT_DIVERSITY, (void *)&intVal);
		pmib->dot11RFEntry.dot11DiversitySupport = intVal;

		apmib_get(MIB_HW_TX_ANT, (void *)&intVal);
		pmib->dot11RFEntry.defaultAntennaB = intVal;

#if 0
		apmib_get(MIB_HW_INIT_GAIN, (void *)&intVal);
		pmib->dot11RFEntry.initialGain = intVal;
#endif

		apmib_get(MIB_HW_TX_POWER_CCK, (void *)buf1);
		memcpy(pmib->dot11RFEntry.pwrlevelCCK, buf1, 14);
		
		apmib_get(MIB_HW_TX_POWER_OFDM, (void *)buf1);
		memcpy(pmib->dot11RFEntry.pwrlevelOFDM, buf1, 162);

		apmib_get(MIB_HW_11N_LOFDMPWD, (void *)&intVal);
		pmib->dot11RFEntry.legacyOFDM_pwrdiff = intVal;
		
		apmib_get(MIB_HW_11N_ANTPWD_C, (void *)&intVal);
		pmib->dot11RFEntry.antC_pwrdiff = intVal;
		
		apmib_get(MIB_HW_11N_THER_RFIC, (void *)&intVal);
		pmib->dot11RFEntry.ther_rfic = intVal;
		
		apmib_get(MIB_HW_11N_XCAP, (void *)&intVal);
		pmib->dot11RFEntry.crystalCap = intVal;

		// set output power scale
		//if (pmib->dot11RFEntry.dot11RFType == 7) { // Zebra
			if (pmib->dot11RFEntry.dot11RFType == 10) { // Zebra
			apmib_get(MIB_WLAN_RF_POWER, (void *)&intVal);
			if(intVal == 1)
				intVal = 3;
			else if(intVal == 2)
					intVal = 6;
				else if(intVal == 3)
						intVal = 9;
					else if(intVal == 4)
							intVal = 17;
			if (intVal) {
				for (i=0; i<14; i++) {
					if(pmib->dot11RFEntry.pwrlevelCCK[i] != 0){ 
						if ((pmib->dot11RFEntry.pwrlevelCCK[i] - intVal) >= 1)
							pmib->dot11RFEntry.pwrlevelCCK[i] -= intVal;
						else
							pmib->dot11RFEntry.pwrlevelCCK[i] = 1;
					}
				}
				for (i=0; i<162; i++) {
					if (pmib->dot11RFEntry.pwrlevelOFDM[i] != 0){
						if((pmib->dot11RFEntry.pwrlevelOFDM[i] - intVal) >= 1)
							pmib->dot11RFEntry.pwrlevelOFDM[i] -= intVal;
						else
							pmib->dot11RFEntry.pwrlevelOFDM[i] = 1;
					}
				}		
			}
		}
#endif  //For Check CONFIG_RTL8196B
		
		apmib_get(MIB_WLAN_BEACON_INTERVAL, (void *)&intVal);
		pmib->dot11StationConfigEntry.dot11BeaconPeriod = intVal;

		apmib_get(MIB_WLAN_CHAN_NUM, (void *)&intVal);
		pmib->dot11RFEntry.dot11channel = intVal;

		apmib_get(MIB_WLAN_RTS_THRESHOLD, (void *)&intVal);
		pmib->dot11OperationEntry.dot11RTSThreshold = intVal;

		apmib_get(MIB_WLAN_FRAG_THRESHOLD, (void *)&intVal);
		pmib->dot11OperationEntry.dot11FragmentationThreshold = intVal;

		apmib_get(MIB_WLAN_INACTIVITY_TIME, (void *)&intVal);
		pmib->dot11OperationEntry.expiretime = intVal;

		apmib_get(MIB_WLAN_PREAMBLE_TYPE, (void *)&intVal);
		pmib->dot11RFEntry.shortpreamble = intVal;

		apmib_get(MIB_WLAN_DTIM_PERIOD, (void *)&intVal);
		pmib->dot11StationConfigEntry.dot11DTIMPeriod = intVal;

		// enable/disable the notification for IAPP
		apmib_get(MIB_WLAN_IAPP_DISABLED, (void *)&intVal);
		if (intVal == 0)
			pmib->dot11OperationEntry.iapp_enable = 1;
		else
			pmib->dot11OperationEntry.iapp_enable = 0;

		// set 11g protection mode
		apmib_get(MIB_WLAN_PROTECTION_DISABLED, (void *)&intVal);
		pmib->dot11StationConfigEntry.protectionDisabled = intVal;

		// set block relay
		apmib_get(MIB_WLAN_BLOCK_RELAY, (void *)&intVal);
		pmib->dot11OperationEntry.block_relay = intVal;

		// set WiFi specific mode
		apmib_get(MIB_WIFI_SPECIFIC, (void *)&intVal);
		pmib->dot11OperationEntry.wifi_specific = intVal;

		// Set WDS
		apmib_get(MIB_WLAN_WDS_ENABLED, (void *)&intVal);
		apmib_get(MIB_WLAN_WDS_NUM, (void *)&intVal2);
		pmib->dot11WdsInfo.wdsNum = 0;
#ifdef MBSSID 
		if (v_previous > 0) 
			intVal = 0;
#endif
		if (((mode == 2) || (mode == 3)) &&
			(intVal != 0) &&
			(intVal2 != 0)) {
			for (i=0; i<intVal2; i++) {
				buf1[0] = i+1;
				apmib_get(MIB_WLAN_WDS, (void *)buf1);
				pwds_EntryUI = (WDS_Tp)buf1;
				wds_Entry = &(pmib->dot11WdsInfo.entry[i]);
				memcpy(wds_Entry->macAddr, &(pwds_EntryUI->macAddr[0]), 6);
				wds_Entry->txRate = pwds_EntryUI->fixedTxRate;
				pmib->dot11WdsInfo.wdsNum++;
				sprintf(buf2, "ifconfig %s-wds%d hw ether %02x%02x%02x%02x%02x%02x", ifname, i, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
				system(buf2);
			}
			pmib->dot11WdsInfo.wdsEnabled = intVal;
		}
		else
			pmib->dot11WdsInfo.wdsEnabled = 0;

		if (((mode == 2) || (mode == 3)) &&
			(intVal != 0)) {
			apmib_get(MIB_WLAN_WDS_ENCRYPT, (void *)&intVal);
			if (intVal == 0)
				pmib->dot11WdsInfo.wdsPrivacy = 0;
			else if (intVal == 1) {
				apmib_get(MIB_WLAN_WDS_WEP_KEY, (void *)buf1);
				pmib->dot11WdsInfo.wdsPrivacy = 1;
				string_to_hex(buf1, &(pmib->dot11WdsInfo.wdsWepKey[0]), 10);
			}
			else if (intVal == 2) {
				apmib_get(MIB_WLAN_WDS_WEP_KEY, (void *)buf1);
				pmib->dot11WdsInfo.wdsPrivacy = 5;
				string_to_hex(buf1, &(pmib->dot11WdsInfo.wdsWepKey[0]), 26);
			}
			else if (intVal == 3)
				pmib->dot11WdsInfo.wdsPrivacy = 2;
			else
				pmib->dot11WdsInfo.wdsPrivacy = 4;
		}

		// enable/disable the notification for IAPP
		apmib_get(MIB_WLAN_IAPP_DISABLED, (void *)&intVal);
		if (intVal == 0)
			pmib->dot11OperationEntry.iapp_enable = 1;
		else
			pmib->dot11OperationEntry.iapp_enable = 0;

		pmib->dot11StationConfigEntry.dot11AclNum = 0;
		apmib_get(MIB_WLAN_AC_ENABLED, (void *)&intVal);
		pmib->dot11StationConfigEntry.dot11AclMode = intVal;
		if (intVal != 0) {
			apmib_get(MIB_WLAN_AC_NUM, (void *)&intVal);
			if (intVal != 0) {
				for (i=0; i<intVal; i++) {
					buf1[0] = i+1;
					apmib_get(MIB_WLAN_AC_ADDR, (void *)buf1);
					pAcl = (MACFILTER_T *)buf1;
					memcpy(&(pmib->dot11StationConfigEntry.dot11AclAddr[i][0]), &(pAcl->macAddr[0]), 6);
					pmib->dot11StationConfigEntry.dot11AclNum++;
				}
			}
		}

#if defined(CONFIG_RTK_MESH) && defined(_MESH_ACL_ENABLE_) // below code copy above ACL code
		// Copy Webpage setting to userspace MIB struct table
		pmib->dot1180211sInfo.mesh_acl_num = 0;
		apmib_get(MIB_MESH_ACL_ENABLED, (void *)&intVal);
		pmib->dot1180211sInfo.mesh_acl_mode = intVal;

		if (intVal != 0) {
			apmib_get(MIB_MESH_ACL_NUM, (void *)&intVal);
			if (intVal != 0) {
				for (i=0; i<intVal; i++) {
					buf1[0] = i+1;
					apmib_get(MIB_MESH_ACL_ADDR, (void *)buf1);
					pAcl = (MACFILTER_T *)buf1;
					memcpy(&(pmib->dot1180211sInfo.mesh_acl_addr[i][0]), &(pAcl->macAddr[0]), 6);
					pmib->dot1180211sInfo.mesh_acl_num++;
				}
			}
		}
#endif

		// set nat2.5 disable when client and mac clone is set
		apmib_get(MIB_WLAN_NAT25_MAC_CLONE, (void *)&intVal);
		if ((intVal == 1) && (mode == 1)) {
			pmib->ethBrExtInfo.nat25_disable = 1;
			pmib->ethBrExtInfo.macclone_enable = 1;
		}
		else {
			pmib->ethBrExtInfo.nat25_disable = 0;
			pmib->ethBrExtInfo.macclone_enable = 0;
		}		

		// set nat2.5 disable and macclone disable when wireless isp mode
		apmib_get(MIB_OP_MODE, (void *)&intVal);
		if (intVal == 2) {
			pmib->ethBrExtInfo.nat25_disable = 1;
			pmib->ethBrExtInfo.macclone_enable = 0;
		}

#ifdef WIFI_SIMPLE_CONFIG
		pmib->wscEntry.wsc_enable = 0;
#endif

	// for 11n
		apmib_get(MIB_WLAN_CHANNEL_BONDING, &channel_bound);
		pmib->dot11nConfigEntry.dot11nUse40M = channel_bound;
		apmib_get(MIB_WLAN_CONTROL_SIDEBAND, &intVal);
		if(channel_bound ==0){
			pmib->dot11nConfigEntry.dot11n2ndChOffset = 0;
		}else {
			if(intVal == 0 )
				pmib->dot11nConfigEntry.dot11n2ndChOffset = 1;
			if(intVal == 1 )
				pmib->dot11nConfigEntry.dot11n2ndChOffset = 2;	
		}
		apmib_get(MIB_WLAN_SHORT_GI, &intVal);
		pmib->dot11nConfigEntry.dot11nShortGIfor20M = intVal;
		pmib->dot11nConfigEntry.dot11nShortGIfor40M = intVal;
		
		apmib_get(MIB_WLAN_11N_STBC, &intVal);
		pmib->dot11nConfigEntry.dot11nSTBC = intVal;
		apmib_get(MIB_WLAN_11N_COEXIST, &intVal);
		pmib->dot11nConfigEntry.dot11nCoexist = intVal;
		
		apmib_get(MIB_WLAN_AGGREGATION, &aggregation);
		if(aggregation ==0){
			pmib->dot11nConfigEntry.dot11nAMPDU = 0;
			pmib->dot11nConfigEntry.dot11nAMSDU = 0;
		}else if(aggregation ==1){
			pmib->dot11nConfigEntry.dot11nAMPDU = 1;
			pmib->dot11nConfigEntry.dot11nAMSDU = 0;
		}else if(aggregation ==2){
			pmib->dot11nConfigEntry.dot11nAMPDU = 0;
			pmib->dot11nConfigEntry.dot11nAMSDU = 1;
		}
		else if(aggregation ==3){
			pmib->dot11nConfigEntry.dot11nAMPDU = 1;
			pmib->dot11nConfigEntry.dot11nAMSDU = 1;
		}

#if defined(CONFIG_RTL8196B) && defined(MBSSID)
		if(pmib->dot11OperationEntry.opmode & 0x00000010){// AP mode
			for (vwlan_idx = 1; vwlan_idx < 5; vwlan_idx++) {
				apmib_get(MIB_WLAN_DISABLED, (void *)&intVal4);
				if (intVal4 == 0)
					vap_enable++;
				intVal4=0;
			}
			vwlan_idx = 0;
		}
		if (vap_enable && (mode ==  AP_MODE || mode ==  AP_WDS_MODE))	
			pmib->miscEntry.vap_enable=1;
		else
			pmib->miscEntry.vap_enable=0;
#endif
	}

	if (vwlan_idx != NUM_VWLAN_INTERFACE) { // not repeater interface
		apmib_get(MIB_WLAN_BASIC_RATE, (void *)&intVal);
		pmib->dot11StationConfigEntry.dot11BasicRates = intVal;

		apmib_get(MIB_WLAN_SUPPORTED_RATE, (void *)&intVal);
		pmib->dot11StationConfigEntry.dot11SupportedRates = intVal;

		apmib_get(MIB_WLAN_RATE_ADAPTIVE_ENABLED, (void *)&intVal);
		if (intVal == 0) {
			pmib->dot11StationConfigEntry.autoRate = 0;
			apmib_get(MIB_WLAN_FIX_RATE, (void *)&intVal);
			pmib->dot11StationConfigEntry.fixedTxRate = intVal;
		}
		else
			pmib->dot11StationConfigEntry.autoRate = 1;

		apmib_get(MIB_WLAN_HIDDEN_SSID, (void *)&intVal);
		pmib->dot11OperationEntry.hiddenAP = intVal;

	// set band
		apmib_get(MIB_WLAN_BAND, (void *)&intVal);
		wlan_band = intVal;
		if ((mode != 1) && (pmib->dot11OperationEntry.wifi_specific == 1) && (wlan_band == 2))
			wlan_band = 3;

		if (wlan_band == 8) { // pure-11n
			wlan_band += 3; // b+g+n
			pmib->dot11StationConfigEntry.legacySTADeny = 3;
		}
		else if (wlan_band == 2) { // pure-11g
			wlan_band += 1; // b+g
			pmib->dot11StationConfigEntry.legacySTADeny = 1;
		}
		else if (wlan_band == 10) { // g+n
			wlan_band += 1; // b+g+n
			pmib->dot11StationConfigEntry.legacySTADeny = 1;
		}
		else
			pmib->dot11StationConfigEntry.legacySTADeny = 0;	

		pmib->dot11BssType.net_work_type = wlan_band;

		// set guest access
		apmib_get(MIB_WLAN_ACCESS, (void *)&intVal);
		pmib->dot11OperationEntry.guest_access = intVal;

		// set WMM
		apmib_get(MIB_WLAN_WMM_ENABLED, (void *)&intVal);
		pmib->dot11QosEntry.dot11QosEnable = intVal;		
	}

	apmib_get(MIB_WLAN_AUTH_TYPE, (void *)&intVal);
	apmib_get(MIB_WLAN_ENCRYPT, (void *)&encrypt);
#ifdef CONFIG_RTL_WAPI_SUPPORT
	/*wapi is independed. disable WAPI first if not WAPI*/
	if(7 !=encrypt)
	{
		pmib->wapiInfo.wapiType=0;	
	}
#endif
	if ((intVal == 1) && (encrypt != 1)) {
		// shared-key and not WEP enabled, force to open-system
		intVal = 0;
	}
	pmib->dot1180211AuthEntry.dot11AuthAlgrthm = intVal;

	if (encrypt == 0)
		pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = 0;
	else if (encrypt == 1) {
		// WEP mode
		apmib_get(MIB_WLAN_WEP, (void *)&wep);
		if (wep == 1) {
			pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = 1;
			apmib_get(MIB_WLAN_WEP64_KEY1, (void *)buf1);
			memcpy(&(pmib->dot11DefaultKeysTable.keytype[0]), buf1, 5);
			apmib_get(MIB_WLAN_WEP64_KEY2, (void *)buf1);
			memcpy(&(pmib->dot11DefaultKeysTable.keytype[1]), buf1, 5);
			apmib_get(MIB_WLAN_WEP64_KEY3, (void *)buf1);
			memcpy(&(pmib->dot11DefaultKeysTable.keytype[2]), buf1, 5);
			apmib_get(MIB_WLAN_WEP64_KEY4, (void *)buf1);
			memcpy(&(pmib->dot11DefaultKeysTable.keytype[3]), buf1, 5);
			apmib_get(MIB_WLAN_WEP_DEFAULT_KEY, (void *)&intVal);
			pmib->dot1180211AuthEntry.dot11PrivacyKeyIndex = intVal;
		}
		else {
			pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = 5;
			apmib_get(MIB_WLAN_WEP128_KEY1, (void *)buf1);
			memcpy(&(pmib->dot11DefaultKeysTable.keytype[0]), buf1, 13);
			apmib_get(MIB_WLAN_WEP128_KEY2, (void *)buf1);
			memcpy(&(pmib->dot11DefaultKeysTable.keytype[1]), buf1, 13);
			apmib_get(MIB_WLAN_WEP128_KEY3, (void *)buf1);
			memcpy(&(pmib->dot11DefaultKeysTable.keytype[2]), buf1, 13);
			apmib_get(MIB_WLAN_WEP128_KEY4, (void *)buf1);
			memcpy(&(pmib->dot11DefaultKeysTable.keytype[3]), buf1, 13);
			apmib_get(MIB_WLAN_WEP_DEFAULT_KEY, (void *)&intVal);
			pmib->dot1180211AuthEntry.dot11PrivacyKeyIndex = intVal;
		}
	}
#ifdef CONFIG_RTL_WAPI_SUPPORT	
	else if(7 == encrypt)
	{
		pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = 0;
		pmib->dot1180211AuthEntry.dot11AuthAlgrthm = 0;
	}
#endif	
	else {
		// WPA mode
		pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = 2;
	}

#ifndef CONFIG_RTL8196B_TLD
#ifdef MBSSID
	if (vwlan_idx > 0 && pmib->dot11OperationEntry.guest_access)
		pmib->dot11OperationEntry.block_relay = 1;	
#endif
#endif

	// Set 802.1x flag
	enable1x = 0;
	if (encrypt < 2) {
		apmib_get(MIB_WLAN_ENABLE_1X, (void *)&intVal);
		apmib_get(MIB_WLAN_ENABLE_MAC_AUTH, (void *)&intVal2);
		if ((intVal != 0) || (intVal2 != 0))
			enable1x = 1;
	}
#ifdef CONFIG_RTL_WAPI_SUPPORT
	else if(encrypt == 7)
	{
		/*wapi*/
		enable1x = 0;
	}
#endif	
	else
		enable1x = 1;
	pmib->dot118021xAuthEntry.dot118021xAlgrthm = enable1x;

#ifdef CONFIG_RTL_WAPI_SUPPORT
	if(7 == encrypt)
	{
		//apmib_get(MIB_WLAN_WAPI_ASIPADDR,);
		apmib_get(MIB_WLAN_WAPI_AUTH,(void *)&intVal);
		pmib->wapiInfo.wapiType=intVal;

		apmib_get(MIB_WLAN_WAPI_MCAST_PACKETS,(void *)&intVal);
		pmib->wapiInfo.wapiUpdateMCastKeyPktNum=intVal;
		
		apmib_get(MIB_WLAN_WAPI_MCAST_REKEYTYPE,(void *)&intVal);
		pmib->wapiInfo.wapiUpdateMCastKeyType=intVal;

		apmib_get(MIB_WLAN_WAPI_MCAST_TIME,(void *)&intVal);
		pmib->wapiInfo.wapiUpdateMCastKeyTimeout=intVal;

		apmib_get(MIB_WLAN_WAPI_UCAST_PACKETS,(void *)&intVal);
		pmib->wapiInfo.wapiUpdateUCastKeyPktNum=intVal;
		
		apmib_get(MIB_WLAN_WAPI_UCAST_REKETTYPE,(void *)&intVal);
		pmib->wapiInfo.wapiUpdateUCastKeyType=intVal;

		apmib_get(MIB_WLAN_WAPI_UCAST_TIME,(void *)&intVal);
		pmib->wapiInfo.wapiUpdateUCastKeyTimeout=intVal;

		/*1: hex  -else passthru*/
		apmib_get(MIB_WLAN_WAPI_PSK_FORMAT,(void *)&intVal2);
		apmib_get(MIB_WLAN_WAPI_PSKLEN,(void *)&intVal);
		apmib_get(MIB_WLAN_WAPI_PSK,(void *)buf1);
		pmib->wapiInfo.wapiPsk.len=intVal;
		if(1 == intVal2 )
		{
			/*hex*/	
			string_to_hex(buf1, buf2, pmib->wapiInfo.wapiPsk.len*2);
		}else
		{
			/*passthru*/
			strcpy(buf2,buf1);
		}
		memcpy(pmib->wapiInfo.wapiPsk.octet,buf2,pmib->wapiInfo.wapiPsk.len);
	}
#endif

#ifdef CONFIG_RTK_MESH

#ifdef CONFIG_NEW_MESH_UI
	//new feature:Mesh enable/disable
	//brian add new key:MIB_WLAN_MESH_ENABLE
	pmib->dot1180211sInfo.meshSilence = 0;

	apmib_get(MIB_WLAN_MESH_ENABLE,(void *)&intVal);
	if (mode == AP_MESH_MODE || mode == MESH_MODE)
	{
		if( intVal )
			pmib->dot1180211sInfo.mesh_enable = 1;
		else
			pmib->dot1180211sInfo.mesh_enable = 0;
	}
	else
		pmib->dot1180211sInfo.mesh_enable = 0;

	// set mesh argument
	// brian change to shutdown portal/root as default
	if (mode == AP_MESH_MODE)
	{
		pmib->dot1180211sInfo.mesh_ap_enable = 1;
		pmib->dot1180211sInfo.mesh_portal_enable = 0;
	}
	else if (mode == MESH_MODE)
	{
		if( !intVal )
			//pmib->dot11OperationEntry.opmode += 64; // WIFI_MESH_STATE = 0x00000040
			pmib->dot1180211sInfo.meshSilence = 1;

		pmib->dot1180211sInfo.mesh_ap_enable = 0;
		pmib->dot1180211sInfo.mesh_portal_enable = 0;		
	}
	else
	{
		pmib->dot1180211sInfo.mesh_ap_enable = 0;
		pmib->dot1180211sInfo.mesh_portal_enable = 0;	
	}
	#if 0	//by brian, dont enable root by default
	apmib_get(MIB_MESH_ROOT_ENABLE, (void *)&intVal);
	pmib->dot1180211sInfo.mesh_root_enable = intVal;
	#else
	pmib->dot1180211sInfo.mesh_root_enable = 0;
	#endif
#else
	if (mode == AP_MPP_MODE)
	{
		pmib->dot1180211sInfo.mesh_enable = 1;
		pmib->dot1180211sInfo.mesh_ap_enable = 1;
		pmib->dot1180211sInfo.mesh_portal_enable = 1;	
	}
	else if (mode == MPP_MODE)
	{
		pmib->dot1180211sInfo.mesh_enable = 1;
		pmib->dot1180211sInfo.mesh_ap_enable = 0;
		pmib->dot1180211sInfo.mesh_portal_enable = 1;
	}
	else if (mode == MAP_MODE)
	{
		pmib->dot1180211sInfo.mesh_enable = 1;
		pmib->dot1180211sInfo.mesh_ap_enable = 1;
		pmib->dot1180211sInfo.mesh_portal_enable = 0;
	}		
	else if (mode == MP_MODE)
	{
		pmib->dot1180211sInfo.mesh_enable = 1;
		pmib->dot1180211sInfo.mesh_ap_enable = 0;
		pmib->dot1180211sInfo.mesh_portal_enable = 0;		
	}
	else
	{
		pmib->dot1180211sInfo.mesh_enable = 0;
		pmib->dot1180211sInfo.mesh_ap_enable = 0;
		pmib->dot1180211sInfo.mesh_portal_enable = 0;	
	}

	apmib_get(MIB_MESH_ROOT_ENABLE, (void *)&intVal);
	pmib->dot1180211sInfo.mesh_root_enable = intVal;
#endif
	apmib_get(MIB_MESH_MAX_NEIGHTBOR, (void *)&intVal);
	pmib->dot1180211sInfo.mesh_max_neightbor = intVal;

	apmib_get(MIB_LOG_ENABLED, (void *)&intVal);
	pmib->dot1180211sInfo.log_enabled = intVal;

	apmib_get(MIB_MESH_ID, (void *)buf1);
	intVal2 = strlen(buf1);
	memset(pmib->dot1180211sInfo.mesh_id, 0, 32);
	memcpy(pmib->dot1180211sInfo.mesh_id, buf1, intVal2);

	apmib_get(MIB_MESH_ENCRYPT, (void *)&intVal);
	apmib_get(MIB_MESH_WPA_AUTH, (void *)&intVal2);

	if( intVal2 == 2 && intVal)
		pmib->dot11sKeysTable.dot11Privacy  = 2;
	else
		pmib->dot11sKeysTable.dot11Privacy  = 0;
	
#ifdef 	_11s_TEST_MODE_	

	apmib_get(MIB_MESH_TEST_PARAM1, (void *)&intVal);
	pmib->dot1180211sInfo.mesh_reserved1 = intVal;

	apmib_get(MIB_MESH_TEST_PARAM2, (void *)&intVal);
	pmib->dot1180211sInfo.mesh_reserved2 = intVal;

	apmib_get(MIB_MESH_TEST_PARAM3, (void *)&intVal);
	pmib->dot1180211sInfo.mesh_reserved3 = intVal;

	apmib_get(MIB_MESH_TEST_PARAM4, (void *)&intVal);
	pmib->dot1180211sInfo.mesh_reserved4 = intVal;

	apmib_get(MIB_MESH_TEST_PARAM5, (void *)&intVal);
	pmib->dot1180211sInfo.mesh_reserved5 = intVal;

	apmib_get(MIB_MESH_TEST_PARAM6, (void *)&intVal);
	pmib->dot1180211sInfo.mesh_reserved6 = intVal;

	apmib_get(MIB_MESH_TEST_PARAM7, (void *)&intVal);
	pmib->dot1180211sInfo.mesh_reserved7 = intVal;

	apmib_get(MIB_MESH_TEST_PARAM8, (void *)&intVal);
	pmib->dot1180211sInfo.mesh_reserved8 = intVal;
	
	apmib_get(MIB_MESH_TEST_PARAM9, (void *)&intVal);
	pmib->dot1180211sInfo.mesh_reserved9 = intVal;

	apmib_get(MIB_MESH_TEST_PARAMA, (void *)&intVal);
	pmib->dot1180211sInfo.mesh_reserveda = intVal;

	apmib_get(MIB_MESH_TEST_PARAMB, (void *)&intVal);
	pmib->dot1180211sInfo.mesh_reservedb = intVal;
	
	apmib_get(MIB_MESH_TEST_PARAMC, (void *)&intVal);
	pmib->dot1180211sInfo.mesh_reservedc = intVal;

	apmib_get(MIB_MESH_TEST_PARAMD, (void *)&intVal);
	pmib->dot1180211sInfo.mesh_reservedd = intVal;

	apmib_get(MIB_MESH_TEST_PARAME, (void *)&intVal);
	pmib->dot1180211sInfo.mesh_reservede = intVal;

	apmib_get(MIB_MESH_TEST_PARAMF, (void *)&intVal);
	pmib->dot1180211sInfo.mesh_reservedf = intVal;
	
	apmib_get(MIB_MESH_TEST_PARAMSTR1, (void *)buf1);
	intVal2 = strlen(buf1)<15 ? strlen(buf1) : 15;
	memset(pmib->dot1180211sInfo.mesh_reservedstr1, 0, 16);
	memcpy(pmib->dot1180211sInfo.mesh_reservedstr1, buf1, intVal2);
	
#endif
	
#endif // CONFIG_RTK_MESH

	// When using driver base WPA, set wpa setting to driver
#if 1
	int intVal3;
	apmib_get(MIB_WLAN_WPA_AUTH, (void *)&intVal3);
//#ifdef CONFIG_RTL8196B
// button 2009.05.21
#if 1
	if ((intVal3 & WPA_AUTH_PSK) && encrypt >= 2 
#ifdef CONFIG_RTL_WAPI_SUPPORT
&& encrypt < 7
#endif
)
#else
	if (mode != 1 && (intVal3 & WPA_AUTH_PSK) && encrypt >= 2 
#ifdef CONFIG_RTL_WAPI_SUPPORT
&& encrypt < 7
#endif
)
#endif
	{
		if (encrypt == 2)
			intVal = 1;
		else if (encrypt == 4)
			intVal = 2;
		else if (encrypt == 6)
			intVal = 3;
		else {
			printf("invalid ENCRYPT value!\n");
			return -1;
		}
		pmib->dot1180211AuthEntry.dot11EnablePSK = intVal;

		apmib_get(MIB_WLAN_WPA_PSK, (void *)buf1);
		strcpy(pmib->dot1180211AuthEntry.dot11PassPhrase, buf1);

		apmib_get(MIB_WLAN_WPA_GROUP_REKEY_TIME, (void *)&intVal);
		pmib->dot1180211AuthEntry.dot11GKRekeyTime = intVal;			
	}
	else		
		pmib->dot1180211AuthEntry.dot11EnablePSK = 0;

#if 1
if (intVal3 != 0 && encrypt >= 2 
#ifdef CONFIG_RTL_WAPI_SUPPORT
&& encrypt < 7
#endif
)
#else
	if (mode != 1 && intVal3 != 0 && encrypt >= 2 
#ifdef CONFIG_RTL_WAPI_SUPPORT
&& encrypt < 7
#endif
)
#endif
	{
		if (encrypt == 2 || encrypt == 6) {
			apmib_get(MIB_WLAN_WPA_CIPHER_SUITE, (void *)&intVal2);
			if (intVal2 == 1)
				intVal = 2;
			else if (intVal2 == 2)
				intVal = 8;
			else if (intVal2 == 3)
				intVal = 10;
			else {
				printf("invalid WPA_CIPHER_SUITE value!\n");
				return -1;
			}
			pmib->dot1180211AuthEntry.dot11WPACipher = intVal;			
		}
		
		if (encrypt == 4 || encrypt == 6) {
			apmib_get(MIB_WLAN_WPA2_CIPHER_SUITE, (void *)&intVal2);
			if (intVal2 == 1)
				intVal = 2;
			else if (intVal2 == 2)
				intVal = 8;
			else if (intVal2 == 3)
				intVal = 10;
			else {
				printf("invalid WPA2_CIPHER_SUITE value!\n");
				return -1;
			}
			pmib->dot1180211AuthEntry.dot11WPA2Cipher = intVal;			
		}
	}
#endif

	wrq.u.data.pointer = (caddr_t)pmib;
	wrq.u.data.length = sizeof(struct wifi_mib);
	if (ioctl(skfd, 0x8B43, &wrq) < 0) {
		printf("Set WLAN MIB failed!\n");
		return -1;
	}
	close(skfd);

#if 0
//#ifdef UNIVERSAL_REPEATER
	// set repeater interface
	if (!strcmp(ifname, "wlan0")) {
		apmib_get(MIB_REPEATER_ENABLED1, (void *)&intVal);
		apmib_get(MIB_WLAN_NETWORK_TYPE, (void *)&intVal2);		
		system("ifconfig wlan0-vxd down");
		if (intVal != 0 && mode != WDS_MODE && 
				!(mode==CLIENT_MODE && intVal2==ADHOC)) {
			skfd = socket(AF_INET, SOCK_DGRAM, 0);
			strncpy(wrq.ifr_name, "wlan0-vxd", IFNAMSIZ);
			if (ioctl(skfd, SIOCGIWNAME, &wrq) < 0) {
				printf("Interface open failed!\n");
				return -1;
			}

			wrq.u.data.pointer = (caddr_t)pmib;
			wrq.u.data.length = sizeof(struct wifi_mib);
			if (ioctl(skfd, 0x8B42, &wrq) < 0) {
				printf("Get WLAN MIB failed!\n");
				return -1;
			}

			apmib_get(MIB_REPEATER_SSID1, (void *)buf1);
			intVal2 = strlen(buf1);
			pmib->dot11StationConfigEntry.dot11DesiredSSIDLen = intVal2;
			memset(pmib->dot11StationConfigEntry.dot11DesiredSSID, 0, 32);
			memcpy(pmib->dot11StationConfigEntry.dot11DesiredSSID, buf1, intVal2);

			sprintf(buf1, "ifconfig %s-vxd hw ether %02x%02x%02x%02x%02x%02x", ifname, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
			system(buf1);

			enable1xVxd = 0;
			if (encrypt == 0)
				pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = 0;
			else if (encrypt == 1) {
				if (enable1x == 0) {
					if (wep == 1)
						pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = 1;
					else
						pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = 5;
				}
				else
					pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = 0;
			}
			else {
				apmib_get(MIB_WLAN_WPA_AUTH, (void *)&intVal2);
				if (intVal2 == 2) {
					pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = 2;
					enable1xVxd = 1;
				}
				else
					pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = 0;
			}
			pmib->dot118021xAuthEntry.dot118021xAlgrthm = enable1xVxd;
			
			wrq.u.data.pointer = (caddr_t)pmib;
			wrq.u.data.length = sizeof(struct wifi_mib);
			if (ioctl(skfd, 0x8B43, &wrq) < 0) {
				printf("Set WLAN MIB failed!\n");
				return -1;
			}
			close(skfd);
		}
	}

	if (!strcmp(ifname, "wlan1")) {
		apmib_get(MIB_REPEATER_ENABLED1, (void *)&intVal);
		system("ifconfig wlan1-vxd down");
		if (intVal != 0) {
			skfd = socket(AF_INET, SOCK_DGRAM, 0);
			strncpy(wrq.ifr_name, "wlan1-vxd", IFNAMSIZ);
			if (ioctl(skfd, SIOCGIWNAME, &wrq) < 0) {
				printf("Interface open failed!\n");
				return -1;
			}

			wrq.u.data.pointer = (caddr_t)pmib;
			wrq.u.data.length = sizeof(struct wifi_mib);
			if (ioctl(skfd, 0x8B42, &wrq) < 0) {
				printf("Get WLAN MIB failed!\n");
				return -1;
			}

			apmib_get(MIB_REPEATER_SSID1, (void *)buf1);
			intVal2 = strlen(buf1);
			pmib->dot11StationConfigEntry.dot11DesiredSSIDLen = intVal2;
			memset(pmib->dot11StationConfigEntry.dot11DesiredSSID, 0, 32);
			memcpy(pmib->dot11StationConfigEntry.dot11DesiredSSID, buf1, intVal2);

			sprintf(buf1, "ifconfig %s-vxd hw ether %02x%02x%02x%02x%02x%02x", ifname, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
			system(buf1);

			enable1xVxd = 0;
			if (encrypt == 0)
				pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = 0;
			else if (encrypt == 1) {
				if (enable1x == 0) {
					if (wep == 1)
						pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = 1;
					else
						pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = 5;
				}
				else
					pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = 0;
			}
			else {
				apmib_get(MIB_WLAN_WPA_AUTH, (void *)&intVal2);
				if (intVal2 == 2) {
					pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = 2;
					enable1xVxd = 1;
				}
				else
					pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = 0;
			}
			pmib->dot118021xAuthEntry.dot118021xAlgrthm = enable1xVxd;

			wrq.u.data.pointer = (caddr_t)pmib;
			wrq.u.data.length = sizeof(struct wifi_mib);
			if (ioctl(skfd, 0x8B43, &wrq) < 0) {
				printf("Set WLAN MIB failed!\n");
				return -1;
			}
			close(skfd);
		}
	}
#endif

	free(pmib);
	return 0;
}
#else
static int initWlan(char *ifname)
{
	return 0;
	
}
#endif //#if defined(CONFIG_NET_RADIO)
#endif // WLAN_FAST_INIT

#ifdef HOME_GATEWAY
static int generatePPPConf(int is_pppoe, char *option_file, char *pap_file, char *chap_file)
{
	FILE *fd;
	char tmpbuf[200], buf1[100], buf2[100];
	int srcIdx, dstIdx;

	if ( !apmib_init()) {
		printf("Initialize AP MIB failed!\n");
		return -1;
	}

	if (is_pppoe) {
		apmib_get(MIB_PPP_USER, (void *)buf1);
		apmib_get(MIB_PPP_PASSWORD, (void *)buf2);

	}
	else {
		apmib_get(MIB_PPTP_USER, (void *)buf1);
		apmib_get(MIB_PPTP_PASSWORD, (void *)buf2);		
	}
	// delete '"' in the value
	for (srcIdx=0, dstIdx=0; buf1[srcIdx]; srcIdx++, dstIdx++) {
		if (buf1[srcIdx] == '"')
			tmpbuf[dstIdx++] = '\\';
		tmpbuf[dstIdx] = buf1[srcIdx];
	}
	if (dstIdx != srcIdx) {
		memcpy(buf1, tmpbuf, dstIdx);
		buf1[dstIdx] ='\0';
	}
	
	for (srcIdx=0, dstIdx=0; buf2[srcIdx]; srcIdx++, dstIdx++) {
		if (buf2[srcIdx] == '"')
			tmpbuf[dstIdx++] = '\\';
		tmpbuf[dstIdx] = buf2[srcIdx];
	}
	if (dstIdx != srcIdx) {
		memcpy(buf2, tmpbuf, dstIdx);
		buf2[dstIdx] ='\0';
	}
	
	fd = fopen(option_file, "w");
	if (fd == NULL) {
		printf("open file %s error!\n", option_file);
		return -1;
	}
	sprintf(tmpbuf, "name \"%s\"\n", buf1);
	fputs(tmpbuf, fd);
	if(strlen(buf2)>31)
	{
	  sprintf(tmpbuf, "-mschap\r\n");
		fputs(tmpbuf, fd);
		sprintf(tmpbuf, "-mschap-v2\r\n");
		fputs(tmpbuf, fd);		
	}
	fclose(fd);
	
	fd = fopen(pap_file, "w");
	if (fd == NULL) {
		printf("open file %s error!\n", pap_file);
		return -1;
	}
	fputs("#################################################\n", fd);
	sprintf(tmpbuf, "\"%s\"	*	\"%s\"\n", buf1, buf2);	
	fputs(tmpbuf, fd);

	fd = fopen(chap_file, "w");
	if (fd == NULL) {
		printf("open file %s error!\n", chap_file);
		return -1;
	}
	fputs("#################################################\n", fd);
	sprintf(tmpbuf, "\"%s\"	*	\"%s\"\n", buf1, buf2);	
	fputs(tmpbuf, fd);
	return 0;
}
#endif // HOME_GATEWAY

#ifdef WIFI_SIMPLE_CONFIG
enum { 
	MODE_AP_UNCONFIG=1, 			// AP unconfigured (enrollee)
	MODE_CLIENT_UNCONFIG=2, 		// client unconfigured (enrollee) 
	MODE_CLIENT_CONFIG=3,			// client configured (registrar) 
	MODE_AP_PROXY=4, 			// AP configured (proxy)
	MODE_AP_PROXY_REGISTRAR=5,		// AP configured (proxy and registrar)
	MODE_CLIENT_UNCONFIG_REGISTRAR=6		// client unconfigured (registrar) 
};

#define WRITE_WSC_PARAM(dst, tmp, str, val) {	\
	sprintf(tmp, str, val); \
	memcpy(dst, tmp, strlen(tmp)); \
	dst += strlen(tmp); \
}

static void convert_bin_to_str(unsigned char *bin, int len, char *out)
{
	int i;
	char tmpbuf[10];

	out[0] = '\0';

	for (i=0; i<len; i++) {
		sprintf(tmpbuf, "%02x", bin[i]);
		strcat(out, tmpbuf);
	}
}

static void convert_hex_to_ascii(unsigned long code, char *out)
{
	*out++ = '0' + ((code / 10000000) % 10);  
	*out++ = '0' + ((code / 1000000) % 10);
	*out++ = '0' + ((code / 100000) % 10);
	*out++ = '0' + ((code / 10000) % 10);
	*out++ = '0' + ((code / 1000) % 10);
	*out++ = '0' + ((code / 100) % 10);
	*out++ = '0' + ((code / 10) % 10);
	*out++ = '0' + ((code / 1) % 10);
	*out = '\0';
}

static int compute_pin_checksum(unsigned long int PIN)
{
	unsigned long int accum = 0;
	int digit;
	
	PIN *= 10;
	accum += 3 * ((PIN / 10000000) % 10); 	
	accum += 1 * ((PIN / 1000000) % 10);
	accum += 3 * ((PIN / 100000) % 10);
	accum += 1 * ((PIN / 10000) % 10); 
	accum += 3 * ((PIN / 1000) % 10); 
	accum += 1 * ((PIN / 100) % 10); 
	accum += 3 * ((PIN / 10) % 10);

	digit = (accum % 10);
	return (10 - digit) % 10;
}

#ifdef CONFIG_RTL_COMAPI_CFGFILE
static int defaultWscConf(char *in, char *out)
{
	int fh;
	struct stat status;
	char *buf, *ptr;
	int intVal, intVal2, is_client, is_registrar, len, is_wep=0, wep_key_type=0, wep_transmit_key=0;
	char tmpbuf[100], tmp1[100];
		
	if ( !apmib_init()) {
		printf("Initialize AP MIB failed!\n");
		return -1;
	}

        apmib_get(MIB_WLAN_MODE, (void *)&is_client);
	if (is_client == CLIENT_MODE)
                return ;

	if (stat(in, &status) < 0) {
		printf("stat() error [%s]!\n", in);
		return -1;
	}

	buf = malloc(status.st_size+2048);
	if (buf == NULL) {
		printf("malloc() error [%d]!\n", (int)status.st_size+2048);
		return -1;		
	}

	ptr = buf;

	WRITE_WSC_PARAM(ptr, tmpbuf, "mode = %d\n", MODE_AP_UNCONFIG);
	WRITE_WSC_PARAM(ptr, tmpbuf, "upnp = 1\n", NULL);
	WRITE_WSC_PARAM(ptr, tmpbuf, "config_method = %d\n", CONFIG_METHOD_ETH | CONFIG_METHOD_PIN | CONFIG_METHOD_PBC);
	WRITE_WSC_PARAM(ptr, tmpbuf, "auth_type = 1\n", NULL);
	WRITE_WSC_PARAM(ptr, tmpbuf, "encrypt_type = 1\n", NULL);
	WRITE_WSC_PARAM(ptr, tmpbuf, "connection_type = 1\n", NULL);
	WRITE_WSC_PARAM(ptr, tmpbuf, "manual_config = 0\n", NULL);
	WRITE_WSC_PARAM(ptr, tmpbuf, "network_key = \n", NULL);
	WRITE_WSC_PARAM(ptr, tmpbuf, "ssid = \n", NULL);
    apmib_get(MIB_WSC_PIN, (void *)&tmp1);	
	WRITE_WSC_PARAM(ptr, tmpbuf, "pin_code = %s\n", tmp1);
	WRITE_WSC_PARAM(ptr, tmpbuf, "rf_band = 1\n", NULL);
	WRITE_WSC_PARAM(ptr, tmpbuf, "device_name = \n", NULL);

	len = (int)(((long)ptr)-((long)buf));
	
	fh = open(in, O_RDONLY);
	if (fh == -1) {
		printf("open() error [%s]!\n", in);
		return -1;
	}

	lseek(fh, 0L, SEEK_SET);
	if (read(fh, ptr, status.st_size) != status.st_size) {		
		printf("read() error [%s]!\n", in);
		return -1;	
	}
	close(fh);

	// search UUID field, replace last 12 char with hw mac address
	ptr = strstr(ptr, "uuid =");
	if (ptr) {
		char tmp2[100];
		apmib_get(MIB_HW_NIC0_ADDR, (void *)&tmp1);
		convert_bin_to_str(tmp1, 6, tmp2);
		memcpy(ptr+27, tmp2, 12);
	}

	fh = open(out, O_RDWR|O_CREAT|O_TRUNC);
	if (fh == -1) {
		printf("open() error [%s]!\n", out);
		return -1;
	}

	if (write(fh, buf, len+status.st_size) != len+status.st_size ) {
		printf("Write() file error [%s]!\n", out);
		return -1;
	}
	close(fh);
	free(buf);

	return 0;
}
#endif

static int updateWscConf(char *in, char *out, int genpin)
{
	int fh;
	struct stat status;
	char *buf, *ptr;
	int intVal, intVal2, is_client, is_config, is_registrar, len, is_wep=0, wep_key_type=0, wep_transmit_key=0;
	char tmpbuf[100], tmp1[100];
		
	if ( !apmib_init()) {
		printf("Initialize AP MIB failed!\n");
		return -1;
	}

	apmib_get(MIB_WSC_PIN, (void *)tmpbuf);
	if (genpin || !memcmp(tmpbuf, "\x0\x0\x0\x0\x0\x0\x0\x0", PIN_LEN)) {
		#include <sys/time.h>			
		struct timeval tod;
		unsigned long num;
		
		gettimeofday(&tod , NULL);

		apmib_get(MIB_HW_NIC0_ADDR, (void *)&tmp1);			
		tod.tv_sec += tmp1[4]+tmp1[5];		
		srand(tod.tv_sec);
		num = rand() % 10000000;
		num = num*10 + compute_pin_checksum(num);
		convert_hex_to_ascii((unsigned long)num, tmpbuf);

		apmib_set(MIB_WSC_PIN, (void *)tmpbuf);
//		apmib_update(CURRENT_SETTING);		
		apmib_update(HW_SETTING);		

		printf("Generated PIN = %s\n", tmpbuf);

		if (genpin)
			return 0;
	}

	if (stat(in, &status) < 0) {
		printf("stat() error [%s]!\n", in);
		return -1;
	}

	buf = malloc(status.st_size+2048);
	if (buf == NULL) {
		printf("malloc() error [%d]!\n", (int)status.st_size+2048);
		return -1;		
	}

	ptr = buf;
	apmib_get(MIB_WLAN_MODE, (void *)&is_client);
	apmib_get(MIB_WSC_CONFIGURED, (void *)&is_config);
	apmib_get(MIB_WSC_REGISTRAR_ENABLED, (void *)&is_registrar);	
#ifdef CONFIG_RTL8186_KLD_REPEATER
	int is_repeater_enabled;
	int wps_vxdAP_enabled=0;
	apmib_get(MIB_REPEATER_ENABLED1, (void *)&is_repeater_enabled);
#endif
	if (is_client == CLIENT_MODE) {
#ifdef CONFIG_RTL8186_KLD_REPEATER
		if (is_repeater_enabled && is_config) {
			intVal = MODE_AP_PROXY_REGISTRAR;
			wps_vxdAP_enabled = 1;
			WRITE_WSC_PARAM(ptr, tmpbuf, "disable_configured_by_exReg = %d\n", 1);
		}
		else
#endif
		{
			if (is_registrar) {
				if (!is_config)
					intVal = MODE_CLIENT_UNCONFIG_REGISTRAR;
				else
					intVal = MODE_CLIENT_CONFIG;			
			}
			else
				intVal = MODE_CLIENT_UNCONFIG;
		}
	}
	else {
		if (!is_config)
			intVal = MODE_AP_UNCONFIG;
		else
			intVal = MODE_AP_PROXY_REGISTRAR;
	}
	WRITE_WSC_PARAM(ptr, tmpbuf, "mode = %d\n", intVal);

	if (is_client) {
#ifdef CONFIG_RTL8186_KLD_REPEATER
		if (wps_vxdAP_enabled)
			apmib_get(MIB_WSC_UPNP_ENABLED, (void *)&intVal);
		else
#endif
			intVal = 0;
	}
	else
		apmib_get(MIB_WSC_UPNP_ENABLED, (void *)&intVal);

	WRITE_WSC_PARAM(ptr, tmpbuf, "upnp = %d\n", intVal);

	intVal = 0;
	apmib_get(MIB_WSC_METHOD, (void *)&intVal);
	//Ethernet(0x2)+Label(0x4)+PushButton(0x80) Bitwise OR
	if (intVal == 1) //Pin+Ethernet
		intVal = (CONFIG_METHOD_ETH | CONFIG_METHOD_PIN);
	else if (intVal == 2) //PBC+Ethernet
		intVal = (CONFIG_METHOD_ETH | CONFIG_METHOD_PBC);
	if (intVal == 3) //Pin+PBC+Ethernet
		intVal = (CONFIG_METHOD_ETH | CONFIG_METHOD_PIN | CONFIG_METHOD_PBC);
	WRITE_WSC_PARAM(ptr, tmpbuf, "config_method = %d\n", intVal);

	apmib_get(MIB_WSC_AUTH, (void *)&intVal2);
	WRITE_WSC_PARAM(ptr, tmpbuf, "auth_type = %d\n", intVal2);

	apmib_get(MIB_WSC_ENC, (void *)&intVal);
	WRITE_WSC_PARAM(ptr, tmpbuf, "encrypt_type = %d\n", intVal);

	if (intVal == WSC_ENCRYPT_WEP)
		is_wep = 1;

	if (is_client) {
#ifdef CONFIG_RTL8186_KLD_REPEATER
		if (wps_vxdAP_enabled)
			intVal = 1;
		else
#endif
		{
			apmib_get(MIB_WLAN_NETWORK_TYPE, (void *)&intVal);
			if (intVal == 0)
				intVal = 1;
			else
				intVal = 2;
		}
	}
	else
		intVal = 1;
	
	WRITE_WSC_PARAM(ptr, tmpbuf, "connection_type = %d\n", intVal);

	apmib_get(MIB_WSC_MANUAL_ENABLED, (void *)&intVal);
	WRITE_WSC_PARAM(ptr, tmpbuf, "manual_config = %d\n", intVal);


	if (is_wep) { // only allow WEP in none-MANUAL mode (configured by external registrar)
		apmib_get(MIB_WLAN_ENCRYPT, (void *)&intVal);
		if (intVal != ENCRYPT_WEP) {
			printf("WEP mismatched between WPS and host system\n");
			free(buf);
			return -1;
		}
		apmib_get(MIB_WLAN_WEP, (void *)&intVal);
		if (intVal <= WEP_DISABLED || intVal > WEP128) {
			printf("WEP encrypt length error\n");
			free(buf);
			return -1;
		}
		apmib_get(MIB_WLAN_WEP_KEY_TYPE, (void *)&wep_key_type);
		apmib_get(MIB_WLAN_WEP_DEFAULT_KEY, (void *)&wep_transmit_key);
		wep_transmit_key++;
		WRITE_WSC_PARAM(ptr, tmpbuf, "wep_transmit_key = %d\n", wep_transmit_key);
		if (intVal == WEP64) {
			apmib_get(MIB_WLAN_WEP64_KEY1, (void *)&tmpbuf);

				convert_bin_to_str(tmpbuf, 5, tmp1);
				tmp1[10] = '\0';

			WRITE_WSC_PARAM(ptr, tmpbuf, "network_key = %s\n", tmp1);

			apmib_get(MIB_WLAN_WEP64_KEY2, (void *)&tmpbuf);
				convert_bin_to_str(tmpbuf, 5, tmp1);
				tmp1[10] = '\0';

			WRITE_WSC_PARAM(ptr, tmpbuf, "wep_key2 = %s\n", tmp1);

			apmib_get(MIB_WLAN_WEP64_KEY3, (void *)&tmpbuf);
				convert_bin_to_str(tmpbuf, 5, tmp1);
				tmp1[10] = '\0';

			WRITE_WSC_PARAM(ptr, tmpbuf, "wep_key3 = %s\n", tmp1);


			apmib_get(MIB_WLAN_WEP64_KEY4, (void *)&tmpbuf);
				convert_bin_to_str(tmpbuf, 5, tmp1);
				tmp1[10] = '\0';

			WRITE_WSC_PARAM(ptr, tmpbuf, "wep_key4 = %s\n", tmp1);
		}
		else {
			apmib_get(MIB_WLAN_WEP128_KEY1, (void *)&tmpbuf);
				convert_bin_to_str(tmpbuf, 13, tmp1);
				tmp1[26] = '\0';

			WRITE_WSC_PARAM(ptr, tmpbuf, "network_key = %s\n", tmp1);

			apmib_get(MIB_WLAN_WEP128_KEY2, (void *)&tmpbuf);
				convert_bin_to_str(tmpbuf, 13, tmp1);
				tmp1[26] = '\0';

			WRITE_WSC_PARAM(ptr, tmpbuf, "wep_key2 = %s\n", tmp1);

			apmib_get(MIB_WLAN_WEP128_KEY3, (void *)&tmpbuf);
				convert_bin_to_str(tmpbuf, 13, tmp1);
				tmp1[26] = '\0';

			WRITE_WSC_PARAM(ptr, tmpbuf, "wep_key3 = %s\n", tmp1);

			apmib_get(MIB_WLAN_WEP128_KEY4, (void *)&tmpbuf);
				convert_bin_to_str(tmpbuf, 13, tmp1);
				tmp1[26] = '\0';

			WRITE_WSC_PARAM(ptr, tmpbuf, "wep_key4 = %s\n", tmp1);
		}
	}
	else {
		apmib_get(MIB_WLAN_WPA_PSK, (void *)&tmp1);
		WRITE_WSC_PARAM(ptr, tmpbuf, "network_key = %s\n", tmp1);
		
	}

#ifdef CONFIG_RTL8186_KLD_REPEATER
	if (wps_vxdAP_enabled)
		apmib_get(MIB_REPEATER_SSID1, (void *)&tmp1);	
	else
#endif
		apmib_get(MIB_WLAN_SSID, (void *)&tmp1);	
		WRITE_WSC_PARAM(ptr, tmpbuf, "ssid = %s\n", tmp1);

#if 0	
//	}
//	else {			
		apmib_get(MIB_WSC_PSK, (void *)&tmp1);
		WRITE_WSC_PARAM(ptr, tmpbuf, "network_key = %s\n", tmp1);		
		
		apmib_get(MIB_WSC_SSID, (void *)&tmp1);
		WRITE_WSC_PARAM(ptr, tmpbuf, "ssid = %s\n", tmp1);
//	}
#endif

	apmib_get(MIB_WSC_PIN, (void *)&tmp1);
	WRITE_WSC_PARAM(ptr, tmpbuf, "pin_code = %s\n", tmp1);
	

	apmib_get(MIB_WLAN_CHAN_NUM, (void *)&intVal);
	if (intVal > 14)
		intVal = 2;
	else
		intVal = 1;
	WRITE_WSC_PARAM(ptr, tmpbuf, "rf_band = %d\n", intVal);

/*
	apmib_get(MIB_HW_MODEL_NUM, (void *)&tmp1);	
	WRITE_WSC_PARAM(ptr, tmpbuf, "model_num = \"%s\"\n", tmp1);	

	apmib_get(MIB_HW_SERIAL_NUM, (void *)&tmp1);	
	WRITE_WSC_PARAM(ptr, tmpbuf, "serial_num = \"%s\"\n", tmp1);	
*/
	apmib_get(MIB_DEVICE_NAME, (void *)&tmp1);	
	WRITE_WSC_PARAM(ptr, tmpbuf, "device_name = \"%s\"\n", tmp1);

	apmib_get(MIB_WSC_CONFIG_BY_EXT_REG, (void *)&intVal);
	WRITE_WSC_PARAM(ptr, tmpbuf, "config_by_ext_reg = %d\n", intVal);

	len = (int)(((long)ptr)-((long)buf));
	
	fh = open(in, O_RDONLY);
	if (fh == -1) {
		printf("open() error [%s]!\n", in);
		return -1;
	}

	lseek(fh, 0L, SEEK_SET);
	if (read(fh, ptr, status.st_size) != status.st_size) {		
		printf("read() error [%s]!\n", in);
		return -1;	
	}
	close(fh);

	// search UUID field, replace last 12 char with hw mac address
	ptr = strstr(ptr, "uuid =");
	if (ptr) {
		char tmp2[100];
		apmib_get(MIB_HW_NIC0_ADDR, (void *)&tmp1);	
		convert_bin_to_str(tmp1, 6, tmp2);
		memcpy(ptr+27, tmp2, 12);		
	}

	fh = open(out, O_RDWR|O_CREAT|O_TRUNC);
	if (fh == -1) {
		printf("open() error [%s]!\n", out);
		return -1;
	}

	if (write(fh, buf, len+status.st_size) != len+status.st_size ) {
		printf("Write() file error [%s]!\n", out);
		return -1;
	}
	close(fh);
	free(buf);

	return 0;
}
#endif
