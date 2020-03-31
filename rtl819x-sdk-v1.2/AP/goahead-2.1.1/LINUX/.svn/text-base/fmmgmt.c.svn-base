/*
 *      Web server handler routines for management (password, save config, f/w update)
 *
 *      Authors: David Hsu	<davidhsu@realtek.com.tw>
 *
 *      $Id: fmmgmt.c,v 1.61.2.5 2011/03/14 10:48:13 bruce Exp $
 *
 */

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/reboot.h>
#include <unistd.h>

#include "../webs.h"
#include "../um.h"
#include "apmib.h"
#include "apform.h"
#include "utility.h"

// added by rock /////////////////////////////////////////
#ifdef VOIP_SUPPORT
#include "web_voip.h"
#include "voip_flash_mib.h"
#include "voip_flash_tool.h"
#endif

#if defined(POWER_CONSUMPTION_SUPPORT)
#include "powerCon.h"
#endif

#define DEFAULT_GROUP		T("administrators")
#define ACCESS_URL		T("/")

#ifdef CONFIG_RTL_WAPI_SUPPORT
#define MTD1_SIZE 0x2d0000	//Address space: 0x2d0000
#define WAPI_SIZE 0x10000	//Address space: 64K
#define WAPI_AREA_BASE (MTD1_SIZE-WAPI_SIZE)
#endif

#if defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT)
extern void Stop_Domain_Query_Process(void);
extern unsigned char WaitCountTime;
#endif
static char superName[MAX_SUPER_NAME_LEN]={0}, superPass[MAX_NAME_LEN]={0};
static char userName[MAX_NAME_LEN]={0}, userPass[MAX_NAME_LEN]={0};
int isUpgrade_OK=0;
int isFWUpgrade=0;
int FW_Data_Size=0;
unsigned char *FW_Data=NULL;
int Reboot_Wait=0;
int isCFG_ONLY=0;
#ifdef LOGIN_URL
static void delete_user(webs_t wp);
#endif

int  opModeHandler(webs_t wp, char *tmpBuf);

////////////////////////////////////////////////////////////////////////////////
#ifdef _LITTLE_ENDIAN_
static void swap_mib_word_value(APMIB_Tp pMib)
{
	pMib->wlan[wlan_idx][vwlan_idx].fragThreshold = WORD_SWAP(pMib->wlan[wlan_idx][vwlan_idx].fragThreshold);
	pMib->wlan[wlan_idx][vwlan_idx].rtsThreshold = WORD_SWAP(pMib->wlan[wlan_idx][vwlan_idx].rtsThreshold);
	pMib->wlan[wlan_idx][vwlan_idx].supportedRates = WORD_SWAP(pMib->wlan[wlan_idx][vwlan_idx].supportedRates);
	pMib->wlan[wlan_idx][vwlan_idx].basicRates = WORD_SWAP(pMib->wlan[wlan_idx][vwlan_idx].basicRates);
	pMib->wlan[wlan_idx][vwlan_idx].beaconInterval = WORD_SWAP(pMib->wlan[wlan_idx][vwlan_idx].beaconInterval);
	pMib->wlan[wlan_idx][vwlan_idx].inactivityTime = DWORD_SWAP(pMib->wlan[wlan_idx][vwlan_idx].inactivityTime);
	pMib->wlan[wlan_idx][vwlan_idx].wpaGroupRekeyTime = DWORD_SWAP(pMib->wlan[wlan_idx][vwlan_idx].wpaGroupRekeyTime);
	pMib->wlan[wlan_idx][vwlan_idx].rsPort = WORD_SWAP(pMib->wlan[wlan_idx][vwlan_idx].rsPort);

#ifdef HOME_GATEWAY
{
	int i;
	pMib->pppIdleTime = WORD_SWAP(pMib->pppIdleTime);
	for (i=0; i<pMib->portFwNum; i++) {
		pMib->portFwArray[i].fromPort = WORD_SWAP(pMib->portFwArray[i].fromPort);
		pMib->portFwArray[i].toPort = WORD_SWAP(pMib->portFwArray[i].toPort);
	}

	for (i=0; i<pMib->portFilterNum; i++) {
		pMib->portFilterArray[i].fromPort = WORD_SWAP(pMib->portFilterArray[i].fromPort);
		pMib->portFilterArray[i].toPort = WORD_SWAP(pMib->portFilterArray[i].toPort);
	}
	for (i=0; i<pMib->triggerPortNum; i++) {
		pMib->triggerPortArray[i].tri_fromPort = WORD_SWAP(pMib->triggerPortArray[i].tri_fromPort);
		pMib->triggerPortArray[i].tri_toPort = WORD_SWAP(pMib->triggerPortArray[i].tri_toPort);
		pMib->triggerPortArray[i].inc_fromPort = WORD_SWAP(pMib->triggerPortArray[i].inc_fromPort);
		pMib->triggerPortArray[i].inc_toPort = WORD_SWAP(pMib->triggerPortArray[i].inc_toPort);
	}
#ifdef GW_QOS_ENGINE
	pMib->qosManualUplinkSpeed = DWORD_SWAP(pMib->qosManualUplinkSpeed);

	for (i=0; i<pMib->qosRuleNum; i++) {
		pMib->qosRuleArray[i].protocol = WORD_SWAP(pMib->qosRuleArray[i].protocol);
		pMib->qosRuleArray[i].local_port_start = WORD_SWAP(pMib->qosRuleArray[i].local_port_start);
		pMib->qosRuleArray[i].local_port_end = WORD_SWAP(pMib->qosRuleArray[i].local_port_end);
		pMib->qosRuleArray[i].remote_port_start = WORD_SWAP(pMib->qosRuleArray[i].remote_port_start);
		pMib->qosRuleArray[i].remote_port_end = WORD_SWAP(pMib->qosRuleArray[i].remote_port_end);
	}
#endif

#ifdef QOS_BY_BANDWIDTH
	pMib->qosManualUplinkSpeed = DWORD_SWAP(pMib->qosManualUplinkSpeed);

	for (i=0; i<pMib->qosRuleNum; i++) {
		pMib->qosRuleArray[i].bandwidth = DWORD_SWAP(pMib->qosRuleArray[i].bandwidth);
	}
#endif
}
#endif

// added by rock /////////////////////////////////////////
#ifdef VOIP_SUPPORT
	voip_mibtbl_swap_value(&pMib->voipCfgParam);
#endif
}
#endif

///////////////////////////////////////////////////////////////////
#if 0
static void reset_user_profile()
{
	struct stat status;
	umClose();

	if ( stat(UM_TXT_FILENAME, &status) == 0) // file existed
               	unlink(UM_TXT_FILENAME);

	umOpen();
	umRestore(UM_TXT_FILENAME);

	set_user_profile();
}
#endif

/////////////////////////////////////////////////////////////////////////////
#if 0 //Keith. move to utility.c
static int fwChecksumOk(char_t *data, int len)
{
	unsigned short sum=0;
	int i;

	for (i=0; i<len; i+=2) {
#ifdef _LITTLE_ENDIAN_
		sum += WORD_SWAP( *((unsigned short *)&data[i]) );
#else
		sum += *((unsigned short *)&data[i]);
#endif

	}
	return( (sum==0) ? 1 : 0);
}
#endif //#if 0 //Keith. move to utility.c
/////////////////////////////////////////////////////////////////////////////

static int updateConfigIntoFlash(unsigned char *data, int total_len, int *pType, int *pStatus)
{
	int len=0, status=1, type=0, ver, force;
	PARAM_HEADER_Tp pHeader;
	char *ptr;

	do {
		pHeader = (PARAM_HEADER_Tp)&data[len];
#ifdef _LITTLE_ENDIAN_
		pHeader->len = WORD_SWAP(pHeader->len);
#endif
		len += sizeof(PARAM_HEADER_T);

		if ( sscanf(&pHeader->signature[TAG_LEN], "%02d", &ver) != 1)
			ver = -1;

		force = -1;
		if ( !memcmp(pHeader->signature, CURRENT_SETTING_HEADER_TAG, TAG_LEN) )
			force = 1; // update
		else if ( !memcmp(pHeader->signature, CURRENT_SETTING_HEADER_FORCE_TAG, TAG_LEN))
			force = 2; // force
		else if ( !memcmp(pHeader->signature, CURRENT_SETTING_HEADER_UPGRADE_TAG, TAG_LEN))
			force = 0; // upgrade

		if ( force >= 0 ) {
#if 0
			if ( !force && (ver < CURRENT_SETTING_VER || // version is less than current
				(pHeader->len < (sizeof(APMIB_T)+1)) ) { // length is less than current
				status = 0;
				break;
			}
#endif
			ptr = &data[len];
			DECODE_DATA(ptr, pHeader->len);
			if ( !CHECKSUM_OK(ptr, pHeader->len)) {
				status = 0;
				break;
			}
#ifdef _LITTLE_ENDIAN_
			swap_mib_word_value((APMIB_Tp)ptr);
#endif

// added by rock /////////////////////////////////////////
#ifdef VOIP_SUPPORT
			flash_voip_import_fix(&((APMIB_Tp)ptr)->voipCfgParam, &pMib->voipCfgParam);
#endif

			apmib_updateFlash(CURRENT_SETTING, ptr, pHeader->len-1, force, ver);
			len += pHeader->len;
			type |= CURRENT_SETTING;
			continue;
		}


		if ( !memcmp(pHeader->signature, DEFAULT_SETTING_HEADER_TAG, TAG_LEN) )
			force = 1;	// update
		else if ( !memcmp(pHeader->signature, DEFAULT_SETTING_HEADER_FORCE_TAG, TAG_LEN) )
			force = 2;	// force
		else if ( !memcmp(pHeader->signature, DEFAULT_SETTING_HEADER_UPGRADE_TAG, TAG_LEN) )
			force = 0;	// upgrade

		if ( force >= 0 ) {
#if 0
			if ( (ver < DEFAULT_SETTING_VER) || // version is less than current
				(pHeader->len < (sizeof(APMIB_T)+1)) ) { // length is less than current
				status = 0;
				break;
			}
#endif
			ptr = &data[len];
			DECODE_DATA(ptr, pHeader->len);
			if ( !CHECKSUM_OK(ptr, pHeader->len)) {
				status = 0;
				break;
			}

#ifdef _LITTLE_ENDIAN_
			swap_mib_word_value((APMIB_Tp)ptr);
#endif

// added by rock /////////////////////////////////////////
#ifdef VOIP_SUPPORT
			flash_voip_import_fix(&((APMIB_Tp)ptr)->voipCfgParam, &pMibDef->voipCfgParam);
#endif

			apmib_updateFlash(DEFAULT_SETTING, ptr, pHeader->len-1, force, ver);
			len += pHeader->len;
			type |= DEFAULT_SETTING;
			continue;
		}

		if ( !memcmp(pHeader->signature, HW_SETTING_HEADER_TAG, TAG_LEN) )
			force = 1;	// update
		else if ( !memcmp(pHeader->signature, HW_SETTING_HEADER_FORCE_TAG, TAG_LEN) )
			force = 2;	// force
		else if ( !memcmp(pHeader->signature, HW_SETTING_HEADER_UPGRADE_TAG, TAG_LEN) )
			force = 0;	// upgrade

		if ( force >= 0 ) {
#if 0
			if ( (ver < HW_SETTING_VER) || // version is less than current
				(pHeader->len < (sizeof(HW_SETTING_T)+1)) ) { // length is less than current
				status = 0;
				break;
			}
#endif
			ptr = &data[len];
			DECODE_DATA(ptr, pHeader->len);
			if ( !CHECKSUM_OK(ptr, pHeader->len)) {
				status = 0;
				break;
			}
			apmib_updateFlash(HW_SETTING, ptr, pHeader->len-1, force, ver);
			len += pHeader->len;
			type |= HW_SETTING;
			continue;
		}
	} while (len < total_len);

	*pType = type;
	*pStatus = status;
	return len;
}

///////////////////////////////////////////////////////////////////////////////
void sig_alm(int signo)
{
	if(isUpgrade_OK ==1){
		reboot( RB_AUTOBOOT);
		return;
	}

}
///////////////////////////////////////////////////////////////////////////////
void formSaveConfig(webs_t wp, char_t *path, char_t *query)
{
	char_t *strRequest;
	char *buf, *ptr=NULL;
	PARAM_HEADER_Tp pHeader;
	unsigned char checksum;
	int len, status=0, len1;
	char tmpBuf[200];
	CONFIG_DATA_T type=0;
	char_t *submitUrl;
	char lan_ip_buf[30], lan_ip[30];

//displayPostDate(wp->postData);

strRequest = websGetVar(wp, T("reset"), T(""));
	if (strRequest[0] && strcmp(strRequest,"Reset") == 0) {
		if ( !apmib_updateDef() ) {
			free(ptr);
			strcpy(tmpBuf, "Write default to current setting failed!\n");
			goto back;
		}

#ifdef CONFIG_RTL_WAPI_SUPPORT
		//To clear CA files
		system("storeWapiFiles -reset");
#endif
#if 0	/* Reboot DUT. Keith */
		if (apmib_reinit() ==0) {
			free(ptr);
			strcpy(tmpBuf, "Re-initialize AP MIB failed!\n");
			goto back;
		}

		reset_user_profile();  // re-initialize user password

#ifndef NO_ACTION
		/* restart system init script */
		run_init_script("all");
#endif

#endif

		Reboot_Wait = 40;
		#ifdef HOME_GATEWAY
		sprintf(tmpBuf, "%s","Reload setting successfully!<br><br>The Router is booting.<br>Do not turn off or reboot the Device during this time.<br>");
		#else
		sprintf(tmpBuf, "%s", "Reload setting successfully!<br><br>The AP is booting.<br>");
		#endif
		//ERR_MSG(tmpBuf);

		apmib_reinit();

		apmib_update_web(CURRENT_SETTING);	// update configuration to flash
		apmib_get( MIB_IP_ADDR,  (void *)lan_ip_buf) ;
  	sprintf(lan_ip,"%s",inet_ntoa(*((struct in_addr *)lan_ip_buf)) );

		OK_MSG_FW(tmpBuf, submitUrl,Reboot_Wait,lan_ip);

		if(ptr != NULL)
		{
			free(ptr);
		}

		/* Reboot DUT. Keith */
		isUpgrade_OK=1;
		//alarm(2);
		system("reboot");
	}

	len1 = sizeof(PARAM_HEADER_T) + sizeof(APMIB_T) + sizeof(checksum) + 100;  // 100 for expansion
	len = csHeader.len;
#ifdef _LITTLE_ENDIAN_
#ifdef VOIP_SUPPORT
	// rock: don't need swap here
	// 1. write to private space (ex: flash)
	// 2. read from private space (ex: flash)
#else
	len  = WORD_SWAP(len);
#endif
#endif
	len += sizeof(PARAM_HEADER_T) + 100;
	if (len1 > len)
		len = len1;

	buf = malloc(len);
	if ( buf == NULL ) {
		strcpy(tmpBuf, "Allocate buffer failed!");
		goto back;
	}

	strRequest = websGetVar(wp, T("save-cs"), T(""));
	if (strRequest[0])
		type |= CURRENT_SETTING;

	strRequest = websGetVar(wp, T("save"), T(""));
	if (strRequest[0])
		type |= CURRENT_SETTING;

	strRequest = websGetVar(wp, T("save-hs"), T(""));
	if (strRequest[0])
		type |= HW_SETTING;

	strRequest = websGetVar(wp, T("save-ds"), T(""));
	if (strRequest[0])
		type |= DEFAULT_SETTING;

	strRequest = websGetVar(wp, T("save-all"), T(""));
	if (strRequest[0])
		type |= HW_SETTING | DEFAULT_SETTING | CURRENT_SETTING;

	if (type) {
		websWrite(wp, "HTTP/1.0 200 OK\n");
		websWrite(wp, "Content-Type: application/octet-stream;\n");
		websWrite(wp, "Content-Disposition: attachment;filename=\"config.dat\" \n");
		websWrite(wp, "Pragma: no-cache\n");
		websWrite(wp, "Cache-Control: no-cache\n");
		websWrite(wp, "\n");

		if (type & HW_SETTING) {
			pHeader = (PARAM_HEADER_Tp)buf;
			len = pHeader->len = hsHeader.len;
			memcpy(&buf[sizeof(PARAM_HEADER_T)], pHwSetting, pHeader->len-1);

#ifdef _LITTLE_ENDIAN_
			pHeader->len  = WORD_SWAP(pHeader->len);
#endif
			memcpy(pHeader->signature, hsHeader.signature, SIGNATURE_LEN);
			ptr = (char *)&buf[sizeof(PARAM_HEADER_T)];
			checksum = CHECKSUM(ptr, len-1);
			buf[sizeof(PARAM_HEADER_T)+len-1] = checksum;

			ptr = &buf[sizeof(PARAM_HEADER_T)];
			ENCODE_DATA(ptr,  len);
			websWriteDataNonBlock(wp, buf, len+sizeof(PARAM_HEADER_T));
		}

		if (type & DEFAULT_SETTING) {
			pHeader = (PARAM_HEADER_Tp)buf;
			len = pHeader->len = dsHeader.len;
			memcpy(&buf[sizeof(PARAM_HEADER_T)], pMibDef, len-1);

#ifdef _LITTLE_ENDIAN_
			pHeader->len  = WORD_SWAP(pHeader->len);
			swap_mib_word_value((APMIB_Tp)&buf[sizeof(PARAM_HEADER_T)]);
#endif
			memcpy(pHeader->signature, dsHeader.signature, SIGNATURE_LEN);
			ptr = (char *)&buf[sizeof(PARAM_HEADER_T)];
			checksum = CHECKSUM(ptr, len-1);
			buf[sizeof(PARAM_HEADER_T)+len-1] = checksum;

			ptr = &buf[sizeof(PARAM_HEADER_T)];
			ENCODE_DATA(ptr,  len);
			websWriteDataNonBlock(wp, buf, len+sizeof(PARAM_HEADER_T));
		}

		if (type & CURRENT_SETTING) {
			pHeader = (PARAM_HEADER_Tp)buf;
			len = pHeader->len = csHeader.len;
			memcpy(&buf[sizeof(PARAM_HEADER_T)], pMib, len-1);

#ifdef _LITTLE_ENDIAN_
			pHeader->len  = WORD_SWAP(pHeader->len);
			swap_mib_word_value((APMIB_Tp)&buf[sizeof(PARAM_HEADER_T)]);
#endif
			memcpy(pHeader->signature, csHeader.signature, SIGNATURE_LEN);
			ptr = (char *)&buf[sizeof(PARAM_HEADER_T)];
			checksum = CHECKSUM(ptr, len-1);
			buf[sizeof(PARAM_HEADER_T)+len-1] = checksum;

			ptr = &buf[sizeof(PARAM_HEADER_T)];
			ENCODE_DATA(ptr,  len);
			websWriteDataNonBlock(wp, buf, len+sizeof(PARAM_HEADER_T));
		}
		websDone(wp, 200);
		free(buf);
		return;
	}

	signal(SIGALRM, sig_alm);
	apmib_get( MIB_IP_ADDR,  (void *)lan_ip_buf) ;
  sprintf(lan_ip,"%s",inet_ntoa(*((struct in_addr *)lan_ip_buf)) );

	strRequest = websGetVar(wp, T("load"), T(""));
	if (strRequest[0] && strcmp(strRequest,"Upload") == 0 ) {
		updateConfigIntoFlash(wp->postData, wp->lenPostData, (int *)&type, &status);
		if (status == 0 || type == 0) // checksum error
			strcpy(tmpBuf, "Invalid configuration file!");
		else {
			if (type) // upload success
			{
#if 0
				if (apmib_reinit() ==0) {
					strcpy(tmpBuf,T("Re-initialize AP MIB failed!\n"));
					goto back;
				}
				reset_user_profile();  // re-initialize user password

#ifndef NO_ACTION
				/* restart system init script */
				run_init_script("all");
#endif
#endif
			}
#ifdef HOME_GATEWAY
	sprintf(tmpBuf, T("%s"), "Update successfully!<br><br>Update in progressing.<br>Do not turn off or reboot the Device during this time.<br>");
#else
	sprintf(tmpBuf, T("%s"), "Update successfully!<br><br>Update in progress.<br> Do not turn off or reboot the AP during this time.");
#endif

			Reboot_Wait = 60;
			submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page

			apmib_reinit();
			apmib_update_web(CURRENT_SETTING);	// update configuration to flash
			apmib_get( MIB_IP_ADDR,  (void *)lan_ip_buf) ;
  		sprintf(lan_ip,"%s",inet_ntoa(*((struct in_addr *)lan_ip_buf)) );
#if 0//def REBOOT_CHECK
			sprintf(lastUrl,"%s",submitUrl);
			sprintf(okMsg,"%s",tmpBuf);
			countDownTime = Reboot_Wait;
			websRedirect(wp, COUNTDOWN_PAGE);
#else
			OK_MSG_FW(tmpBuf, submitUrl,Reboot_Wait,lan_ip);
#endif

			free(buf);

			/* Reboot DUT. Keith */
			isUpgrade_OK=1;
			alarm(2);

			return;
		}

back:
		ERR_MSG(tmpBuf);

		free(buf);
		return;
	}


}
///////////////////////////////////////////////////////////////////////////////

#if 0 //Keith. move to utility.c
void kill_processes(void)
{


	printf("upgrade: killing tasks...\n");

	kill(1, SIGTSTP);		/* Stop init from reforking tasks */
	kill(1, SIGSTOP);
	kill(2, SIGSTOP);
	kill(3, SIGSTOP);
	kill(4, SIGSTOP);
	kill(5, SIGSTOP);
	kill(6, SIGSTOP);
	kill(7, SIGSTOP);
	//atexit(restartinit);		/* If exit prematurely, restart init */
	sync();

	signal(SIGTERM,SIG_IGN);	/* Don't kill ourselves... */
	setpgrp(); 			/* Don't let our parent kill us */
	sleep(1);
	signal(SIGHUP, SIG_IGN);	/* Don't die if our parent dies due to
					 * a closed controlling terminal */

}
#endif //#if 0 //Keith. move to utility.c

//////////////////////////////////////////////////////////////////////////////
int FirmwareUpgrade(char *upload_data, int upload_len, int is_root, char *buffer)
{
int head_offset=0 ;
int isIncludeRoot=0;
 int		 len;
    int          locWrite;
    int          numLeft;
    int          numWrite;
    IMG_HEADER_Tp pHeader;
	int flag=0, startAddr=-1, startAddrWeb=-1;
	int update_fw=0, update_cfg=0;
#ifdef __mips__
    int fh;
#else
    FILE *       fp;
    char_t *     bn = NULL;
#endif
unsigned char cmdBuf[30];

#ifdef CONFIG_RTL_WAPI_SUPPORT	//Support WAPI/openssl, the flash MUST up to 4m
	int fwSizeLimit = 0x400000;
#else
	int fwSizeLimit = 0x200000;
#endif

if(isCFG_ONLY == 0){
#ifdef CONFIG_RTL8196B
	sprintf(cmdBuf, "echo \"4 %d\" > /proc/gpio", (Reboot_Wait+12));
#else
	sprintf(cmdBuf, "echo \"4 %d\" > /proc/gpio", (Reboot_Wait+20));
#endif
	system(cmdBuf);
	system("ifconfig br0 down 2> /dev/null");
}


while(head_offset <   upload_len) {

    locWrite = 0;
    pHeader = (IMG_HEADER_Tp) &upload_data[head_offset];
    len = pHeader->len;
#ifdef _LITTLE_ENDIAN_
    len  = DWORD_SWAP(len);
#endif
    numLeft = len + sizeof(IMG_HEADER_T) ;

    // check header and checksum
    if (!memcmp(&upload_data[head_offset], FW_HEADER, SIGNATURE_LEN) ||
			!memcmp(&upload_data[head_offset], FW_HEADER_WITH_ROOT, SIGNATURE_LEN))
    	flag = 1;
    else if (!memcmp(&upload_data[head_offset], WEB_HEADER, SIGNATURE_LEN))
    	flag = 2;
    else if (!memcmp(&upload_data[head_offset], ROOT_HEADER, SIGNATURE_LEN)){
    	flag = 3;
    	isIncludeRoot = 1;
	}else if ( !memcmp(&upload_data[head_offset], CURRENT_SETTING_HEADER_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], CURRENT_SETTING_HEADER_FORCE_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], CURRENT_SETTING_HEADER_UPGRADE_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], DEFAULT_SETTING_HEADER_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], DEFAULT_SETTING_HEADER_FORCE_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], DEFAULT_SETTING_HEADER_UPGRADE_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], HW_SETTING_HEADER_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], HW_SETTING_HEADER_FORCE_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], HW_SETTING_HEADER_UPGRADE_TAG, TAG_LEN) )	 {
		int type, status, cfg_len;
		cfg_len = updateConfigIntoFlash(&upload_data[head_offset], 0, &type, &status);

		if (status == 0 || type == 0) { // checksum error
			strcpy(buffer, "Invalid configuration file!");
			goto ret_upload;
		}
		else { // upload success
			strcpy(buffer, "Update successfully!");
			head_offset += cfg_len;
			update_cfg = 1;
		}
		continue;
	}
    else {
       	strcpy(buffer, T("Invalid file format!"));
		goto ret_upload;
    }

       if(len > fwSizeLimit){ //len check by sc_yang
      		sprintf(buffer, T("Image len exceed max size 0x%x ! len=0x%x</b><br>"),fwSizeLimit, len);
		goto ret_upload;
    }
    if ( (flag == 1) || (flag == 3)) {
    	if ( !fwChecksumOk(&upload_data[sizeof(IMG_HEADER_T)+head_offset], len)) {
      		sprintf(buffer, T("Image checksum mismatched! len=0x%x, checksum=0x%x</b><br>"), len,
			*((unsigned short *)&upload_data[len-2]) );
		goto ret_upload;
	}
    }
    else {
    	char *ptr = &upload_data[sizeof(IMG_HEADER_T)+head_offset];
    	if ( !CHECKSUM_OK(ptr, len) ) {
     		sprintf(buffer, T("Image checksum mismatched! len=0x%x</b><br>"), len);
		goto ret_upload;
	}
    }

#ifdef __mips__
    if(flag == 3)
    	fh = open(FLASH_DEVICE_NAME1, O_RDWR);
    else
    fh = open(FLASH_DEVICE_NAME, O_RDWR);

    if ( fh == -1 ) {
#else
    if (flag == 1)
    	bn = "apcode.bin";
    else if (flag == 3)
    	bn = "root.bin" ;
    else
    	bn = "web.gz.up";

    if ((fp = fopen((bn == NULL ? "upldForm.bin" : bn), "w+b")) == NULL) {
#endif
       	strcpy(buffer, T("File open failed!"));
    } else {

#ifdef __mips__
	if (flag == 1) {
		if ( startAddr == -1){
			//startAddr = CODE_IMAGE_OFFSET;
			startAddr = pHeader->burnAddr ;
			#ifdef _LITTLE_ENDIAN_
    				startAddr = DWORD_SWAP(startAddr);
    			#endif
		}

	}
	else if (flag == 3) {
		if ( startAddr == -1){
			startAddr = 0; // always start from offset 0 for 2nd FLASH partition
		}
	}
	else {
		if ( startAddrWeb == -1){
			//startAddr = WEB_PAGE_OFFSET;
			startAddr = pHeader->burnAddr ;
			#ifdef _LITTLE_ENDIAN_
    				startAddr = DWORD_SWAP(startAddr);
    			#endif
		}
		else
			startAddr = startAddrWeb;
	}
	lseek(fh, startAddr, SEEK_SET);
	if(flag == 3){
fprintf(stderr,"\r\n close all interface");
		locWrite += sizeof(IMG_HEADER_T); // remove header
		numLeft -=  sizeof(IMG_HEADER_T);
		system("ifconfig br0 down 2> /dev/null");
		system("ifconfig eth0 down 2> /dev/null");
		system("ifconfig eth1 down 2> /dev/null");
		system("ifconfig ppp0 down 2> /dev/null");
		system("ifconfig wlan0 down 2> /dev/null");
		system("ifconfig wlan0-vxd down 2> /dev/null");
		system("ifconfig wlan0-va0 down 2> /dev/null");
		system("ifconfig wlan0-va1 down 2> /dev/null");
		system("ifconfig wlan0-va2 down 2> /dev/null");
		system("ifconfig wlan0-va3 down 2> /dev/null");
		system("ifconfig wlan0-wds0 down 2> /dev/null");
		system("ifconfig wlan0-wds1 down 2> /dev/null");
		system("ifconfig wlan0-wds2 down 2> /dev/null");
		system("ifconfig wlan0-wds3 down 2> /dev/null");
		system("ifconfig wlan0-wds4 down 2> /dev/null");
		system("ifconfig wlan0-wds5 down 2> /dev/null");
		system("ifconfig wlan0-wds6 down 2> /dev/null");
		system("ifconfig wlan0-wds7 down 2> /dev/null");

		kill_processes();
		sleep(2);
	}


	numWrite = write(fh, &(upload_data[locWrite+head_offset]), numLeft);

#else
	numWrite = fwrite(&(upload_data[locWrite+head_offset]), sizeof(*(upload_data)), numLeft, fp);
#endif

	if (numWrite < numLeft) {
#ifdef __mips__
		sprintf(buffer, T("File write failed. locWrite=%d numLeft=%d numWrite=%d Size=%d bytes."), locWrite, numLeft, numWrite, upload_len);

#else
                sprintf(buffer, T("File write failed. ferror=%d locWrite=%d numLeft=%d numWrite=%d Size=%d bytes."), ferror(fp), locWrite, numLeft, numWrite, upload_len);
#endif
	goto ret_upload;
	}

	locWrite += numWrite;
 	numLeft -= numWrite;
#ifdef __mips__
	close(fh);
#else
	fclose(fp);
#endif

	head_offset += len + sizeof(IMG_HEADER_T) ;
	startAddr = -1 ; //by sc_yang to reset the startAddr for next image
	update_fw = 1;
    }
} //while //sc_yang
#ifndef NO_ACTION

		isUpgrade_OK=1;

//		alarm(2);
		system("reboot");
		for(;;);

#else
#ifdef VOIP_SUPPORT
	// rock: for x86 simulation
	if (update_cfg && !update_fw) {
		if (apmib_reinit()) {
			reset_user_profile();  // re-initialize user password
		}
		if(FW_Data)
			free(FW_Data);
	}
#endif
#endif

  return 1;
  ret_upload:
  	fprintf(stderr, "%s\n", buffer);
	return 0;
}
//////////////////////////////////////////////////////////////////////////////
void formUpload(webs_t wp, char_t * path, char_t * query)
{
#ifdef __mips__
//    int fh;
#else
    FILE *       fp;
    char_t *     bn = NULL;
#endif
    int		 len;
    int          locWrite;
    int          numLeft;
//    int          numWrite;
    IMG_HEADER_Tp pHeader;
    char tmpBuf[200];
    char lan_ip_buf[30];
	char lan_ip[30];
    char_t *strRequest, *submitUrl, *strVal;
    int flag=0, startAddr=-1, startAddrWeb=-1;
    int isIncludeRoot=0;
#ifndef NO_ACTION
//    int pid;
#endif
    int head_offset=0 ;
	int update_fw=0, update_cfg=0;
#ifdef CONFIG_RTL_WAPI_SUPPORT	//Support WAPI/openssl, the flash MUST up to 4m
	int fwSizeLimit = 0x400000;
#else
	int fwSizeLimit = 0x200000;
#endif

	signal(SIGALRM, sig_alm);
	apmib_get( MIB_IP_ADDR,  (void *)lan_ip_buf) ;
  	sprintf(lan_ip,"%s",inet_ntoa(*((struct in_addr *)lan_ip_buf)) );
    strRequest = websGetVar(wp, T("save"), T(""));
    if (strRequest[0]) {
	int fh=0;
	char *buf=NULL;

#ifdef __mips__
	char *filename=FLASH_DEVICE_NAME;
	int imageLen=-1;
	IMG_HEADER_T header;

	strVal = websGetVar(wp, T("readAddr"), T(""));
	if ( strVal[0] )
		startAddr = strtol( strVal, (char **)NULL, 16);

	strVal = websGetVar(wp, T("size"), T(""));
	if ( strVal[0] )
		imageLen = strtol( strVal, (char **)NULL, 16);

	fh = open(filename, O_RDONLY);
	if ( fh == -1 ) {
      		strcpy(tmpBuf, T("Open file failed!"));
		goto ret_err;
	}

	if (startAddr==-1 || imageLen==-1) {
		// read system image
		lseek(fh, CODE_IMAGE_OFFSET, SEEK_SET);
		if ( read(fh, &header, sizeof(header)) != sizeof(header)) {
     			strcpy(tmpBuf, T("Read image header error!"));
			goto ret_err;
		}
		if ( memcmp(header.signature, FW_HEADER, SIGNATURE_LEN)  &&
			 memcmp(header.signature, FW_HEADER_WITH_ROOT, SIGNATURE_LEN)) {
       			strcpy(tmpBuf, T("Invalid file format!"));
			goto ret_err;
	    	}
		startAddr = CODE_IMAGE_OFFSET;
		imageLen =  sizeof(header) + header.len;
	}

	buf = malloc(0x10000);
	if ( buf == NULL) {
       		strcpy(tmpBuf, T("Allocate buffer failed!"));
		goto ret_err;
	}

	lseek(fh, startAddr, SEEK_SET);

	websWrite(wp, "HTTP/1.0 200 OK\n");
	websWrite(wp, "Content-Type: application/octet-stream;\n");
	websWrite(wp, "Content-Disposition: attachment;filename=\"apcode.bin\" \n");
	websWrite(wp, "Pragma: no-cache\n");
	websWrite(wp, "Cache-Control: no-cache\n");
	websWrite(wp, "\n");

	while (imageLen > 0) {
		int blocksize=0x10000;
		if (imageLen < blocksize)
			blocksize=imageLen;

		if ( read(fh, buf, blocksize) != blocksize) {
	     		strcpy(tmpBuf, T("Read image error!"));
			goto ret_err;
		}
		websWriteBlock(wp, (char *)buf, blocksize);
		imageLen -= blocksize;
	}
	websDone(wp, 200);
#else
	struct stat status;
	char *filename="apcode.bin";
	if ( stat(filename, &status) < 0 ) {
       		strcpy(tmpBuf, T("Stat file failed!"));
		goto ret_err;
	}
	buf = malloc(status.st_size);
	if ( buf == NULL) {
       		strcpy(tmpBuf, T("Allocate buffer failed!"));
		goto ret_err;
	}

	fh = open(filename, O_RDONLY);
	if ( fh == -1 ) {
      		strcpy(tmpBuf, T("Open file failed!"));
		goto ret_err;
	}
	lseek(fh, CODE_IMAGE_OFFSET, SEEK_SET);

	if ( read(fh, buf, status.st_size) != status.st_size) {
      		strcpy(tmpBuf, T("Read file failed!"));
		goto ret_err;
	}
	websWriteBlock(wp, (char *)buf, status.st_size);
	websDone(wp, 200);
#endif
	goto ret_ok;

ret_err:
	ERR_MSG(tmpBuf);
ret_ok:
	if (fh>0)
		close(fh);
	if (buf)
		free(buf);
	return;
   }

    // assume as firmware upload

    strVal = websGetVar(wp, T("writeAddrCode"), T(""));
    if ( strVal[0] ) {
	if ( !memcmp(strVal, "0x", 2))
		startAddr = strtol( &strVal[2], (char **)NULL, 16);
    }
    strVal = websGetVar(wp, T("writeAddrWebPages"), T(""));
    if ( strVal[0] ) {
	if ( !memcmp(strVal, "0x", 2))
		startAddrWeb = strtol( &strVal[2], (char **)NULL, 16);
    }

    submitUrl = websGetVar(wp, T("submit-url"), T(""));
//support multiple image

while(head_offset <   wp->lenPostData) {
    locWrite = 0;
    pHeader = (IMG_HEADER_Tp) &wp->postData[head_offset];
    len = pHeader->len;
#ifdef _LITTLE_ENDIAN_
    len  = DWORD_SWAP(len);
#endif
    numLeft = len + sizeof(IMG_HEADER_T) ;

    // check header and checksum
    if (!memcmp(&wp->postData[head_offset], FW_HEADER, SIGNATURE_LEN) ||
			!memcmp(&wp->postData[head_offset], FW_HEADER_WITH_ROOT, SIGNATURE_LEN)){
    	flag = 1;
		//Reboot_Wait = Reboot_Wait+ 50;
   } else if (!memcmp(&wp->postData[head_offset], WEB_HEADER, SIGNATURE_LEN)){
    	flag = 2;
		//Reboot_Wait = Reboot_Wait+ 40;
   } else if (!memcmp(&wp->postData[head_offset], ROOT_HEADER, SIGNATURE_LEN)){
    	flag = 3;
    		//Reboot_Wait = Reboot_Wait+ 60;
    	isIncludeRoot = 1;
	}else if ( !memcmp(&wp->postData[head_offset], CURRENT_SETTING_HEADER_TAG, TAG_LEN) ||
				!memcmp(&wp->postData[head_offset], CURRENT_SETTING_HEADER_FORCE_TAG, TAG_LEN) ||
				!memcmp(&wp->postData[head_offset], CURRENT_SETTING_HEADER_UPGRADE_TAG, TAG_LEN) ||
				!memcmp(&wp->postData[head_offset], DEFAULT_SETTING_HEADER_TAG, TAG_LEN) ||
				!memcmp(&wp->postData[head_offset], DEFAULT_SETTING_HEADER_FORCE_TAG, TAG_LEN) ||
				!memcmp(&wp->postData[head_offset], DEFAULT_SETTING_HEADER_UPGRADE_TAG, TAG_LEN) ||
				!memcmp(&wp->postData[head_offset], HW_SETTING_HEADER_TAG, TAG_LEN) ||
				!memcmp(&wp->postData[head_offset], HW_SETTING_HEADER_FORCE_TAG, TAG_LEN) ||
				!memcmp(&wp->postData[head_offset], HW_SETTING_HEADER_UPGRADE_TAG, TAG_LEN) )	 {
		PARAM_HEADER_Tp pHeader_cfg;
		pHeader_cfg = (PARAM_HEADER_Tp)&wp->postData[head_offset];
		head_offset +=  pHeader_cfg->len+sizeof(PARAM_HEADER_T);
		update_cfg = 1;
		continue;
	}
    else {
       	strcpy(tmpBuf, T("<b>Invalid file format!"));
		goto ret_upload;
    }

    if(len > fwSizeLimit){ //len check by sc_yang
      		sprintf(tmpBuf, T("<b>Image len exceed max size 0x%x ! len=0x%x</b><br>"),fwSizeLimit, len);
		goto ret_upload;
    }
#ifdef CONFIG_RTL_WAPI_SUPPORT
    if((flag == 3) && (len>WAPI_AREA_BASE))
    {
    		sprintf(tmpBuf, T("<b>Root image len 0x%x exceed 0x%x which will overwrite wapi area at flash ! </b><br>"), len, WAPI_AREA_BASE);
		goto ret_upload;
    }
#endif
    if ( (flag == 1) || (flag == 3)) {
    	if ( !fwChecksumOk(&wp->postData[sizeof(IMG_HEADER_T)+head_offset], len)) {
      		sprintf(tmpBuf, T("<b>Image checksum mismatched! len=0x%x, checksum=0x%x</b><br>"), len,
			*((unsigned short *)&wp->postData[len-2]) );
		goto ret_upload;
	}
    }
    else {
    	char *ptr = &wp->postData[sizeof(IMG_HEADER_T)+head_offset];
    	if ( !CHECKSUM_OK(ptr, len) ) {
     		sprintf(tmpBuf, T("<b>Image checksum mismatched! len=0x%x</b><br>"), len);
		goto ret_upload;
	}
    }
#ifdef HOME_GATEWAY
#ifdef REBOOT_CHECK
	sprintf(tmpBuf, T("Upload successfully (size = %d bytes)!<br><br>Firmware update in progress."), wp->lenPostData);
#else
	sprintf(tmpBuf, T("Upload successfully (size = %d bytes)!<br><br>Firmware update in progress.<br> Do not turn off or reboot the AP during this time."), wp->lenPostData);
#endif
#else
	sprintf(tmpBuf, T("Upload successfully (size = %d bytes)!<br><br>Firmware update in progress.<br>"), wp->lenPostData);
#endif
	//sc_yang
	head_offset += len + sizeof(IMG_HEADER_T) ;
	startAddr = -1 ; //by sc_yang to reset the startAddr for next image
	update_fw = 1;

} //while //sc_yang


FW_Data_Size = wp->lenPostData;

/*FW_Data = calloc(1,FW_Data_Size);
if(FW_Data != NULL){*/
	if(wp->postData !=NULL){
		//memcpy(FW_Data, ((char *)(wp->postData)), FW_Data_Size);
		/* 		patch from ZHaoBo
                         * Since memcpy need double SDRAM usage, and there are not
                         * sufficient free memory, we had to do such a
                         * patch for saving memory...
                         * Obviously the patch will lead to memory leak, we should not
                         * worry about because of the following reboot...
                */

                        FW_Data = wp->postData;
                        wp->postData = calloc(1, 4);
                        wp->lenPostData = 4;

	}
//}else{
//	printf("Memory allocate fail for firmware upgrade\n");
//}
isFWUpgrade = 1;
#if defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT)
Stop_Domain_Query_Process();
WaitCountTime=2;
#endif
#ifdef CONFIG_RTL8196B
Reboot_Wait = (FW_Data_Size/69633)+67+5;
if(update_cfg==1 && update_fw==0){
	strcpy(tmpBuf, "<b>Update successfully!");
	Reboot_Wait = (FW_Data_Size/69633)+55+5;
	isCFG_ONLY= 1;
}
#else
Reboot_Wait = (FW_Data_Size/43840)+45;
if(update_cfg==1 && update_fw==0){
	strcpy(tmpBuf, "<b>Update successfully!");
	Reboot_Wait = (FW_Data_Size/43840)+40;
	isCFG_ONLY= 1;
}
#endif

#ifdef REBOOT_CHECK
	sprintf(lastUrl,"%s","status.asp");
	sprintf(okMsg,"%s",tmpBuf);
	countDownTime = Reboot_Wait;
	websRedirect(wp, COUNTDOWN_PAGE);
#else
OK_MSG_FW(tmpBuf, submitUrl,Reboot_Wait,lan_ip);
#endif

    return;

ret_upload:
	Reboot_Wait=0;
    ERR_MSG(tmpBuf);
}

/////////////////////////////////////////////////////////////////////////////
void formPasswordSetup(webs_t wp, char_t *path, char_t *query)
{
	char_t *submitUrl, *strUser, *strPassword, *userid, *nextUserid;
	char tmpBuf[100];

	strUser = websGetVar(wp, T("username"), T(""));
	strPassword = websGetVar(wp, T("newpass"), T(""));
	if ( strUser[0] && !strPassword[0] ) {
		strcpy(tmpBuf, T("ERROR: Password cannot be empty."));
		goto setErr_pass;
	}

	if ( strUser[0] ) {
		/* Check if user name is the same as supervisor name */
		if ( !apmib_get(MIB_SUPER_NAME, (void *)tmpBuf)) {
			strcpy(tmpBuf, T("ERROR: Get supervisor name MIB error!"));
			goto setErr_pass;
		}
		if ( !strcmp(strUser, tmpBuf)) {
			strcpy(tmpBuf, T("ERROR: Cannot use the same user name as supervisor."));
			goto setErr_pass;
		}

		/* Check if supervisor account exist. if not, create it */
		if ( !umGroupExists(DEFAULT_GROUP) )
			if ( umAddGroup(DEFAULT_GROUP, (short)PRIV_ADMIN, AM_BASIC, FALSE, FALSE) ) {
				strcpy(tmpBuf, T("ERROR: Unable to add group."));
				goto setErr_pass;
			}
		if ( !umAccessLimitExists(ACCESS_URL) )
			if ( umAddAccessLimit(ACCESS_URL, AM_FULL, (short)0, DEFAULT_GROUP) ) {
				strcpy(tmpBuf, T("ERROR: Unable to add access limit."));
				goto setErr_pass;
			}
		if(superName[0]){
			if ( !umUserExists(superName))
				if ( umAddUser(superName, superPass, DEFAULT_GROUP, FALSE, FALSE) ) {
					strcpy(tmpBuf, T("ERROR: Unable to add supervisor account."));
					goto setErr_pass;
				}
		}

		/* Add new one */
		if ( umUserExists(strUser))
			umDeleteUser(strUser);

		if ( umAddUser(strUser, strPassword, DEFAULT_GROUP, FALSE, FALSE) ) {
			strcpy(tmpBuf, T("ERROR: Unable to add user account."));
			goto setErr_pass;
		}
	}
	else {
		/* Set NULL account, delete supervisor from DB */
			umDeleteAccessLimit("/");
			umDeleteUser(superName);
			umDeleteGroup(DEFAULT_GROUP);
	}

	/* Delete current user account */
	userid = umGetFirstUser();
	while (userid) {
		if ( gstrcmp(userid, superName) && gstrcmp(userid, strUser)) {
			nextUserid = umGetNextUser(userid);
			if ( umDeleteUser(userid) ) {
				strcpy(tmpBuf, T("ERROR: Unable to delete user account."));
				goto setErr_pass;
			}
			userid = nextUserid;
			continue;
		}
		userid = umGetNextUser(userid);
	}

	if (umCommit(NULL) != 0) {
		strcpy(tmpBuf, T("ERROR: Unable to save user configuration."));
		goto setErr_pass;
	}

	/* Set user account to MIB */
	if ( !apmib_set(MIB_USER_NAME, (void *)strUser) ) {
		strcpy(tmpBuf, T("ERROR: Set user name to MIB database failed."));
		goto setErr_pass;
	}

	if ( !apmib_set(MIB_USER_PASSWORD, (void *)strPassword) ) {
		strcpy(tmpBuf, T("ERROR: Set user password to MIB database failed."));
		goto setErr_pass;
	}

	/* Retrieve next page URL */
	apmib_update_web(CURRENT_SETTING);

	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page

#ifdef LOGIN_URL
	if (strUser[0])
		submitUrl = "/login.asp";
#endif

#ifdef REBOOT_CHECK
{
	char tmpMsg[300];
	char lan_ip_buf[30], lan_ip[30];

	sprintf(tmpMsg, "%s","Change setting successfully!<br><br>Do not turn off or reboot the Device during this time.");
	apmib_get( MIB_IP_ADDR,  (void *)lan_ip_buf) ;
  sprintf(lan_ip,"%s",inet_ntoa(*((struct in_addr *)lan_ip_buf)) );
	OK_MSG_FW(tmpMsg, submitUrl,APPLY_COUNTDOWN_TIME,lan_ip);
#ifdef REBOOT_CHECK
	run_init_script_flag = 1;
#endif
#ifndef NO_ACTION
	run_init_script("all");
#endif
}
#else
	OK_MSG(submitUrl);
#endif
	return;

setErr_pass:
	ERR_MSG(tmpBuf);
}

////////////////////////////////////////////////////////////////////
void set_user_profile()
{
	/* first time load, get mib */
	if ( !apmib_get( MIB_SUPER_NAME, (void *)superName ) ||
		!apmib_get( MIB_SUPER_PASSWORD, (void *)superPass ) ||
			!apmib_get( MIB_USER_NAME, (void *)userName ) ||
				!apmib_get( MIB_USER_PASSWORD, (void *)userPass ) ) {
		error(E_L, E_LOG, T("Get user account MIB failed"));
		return;
	}

	/* Create umconfig.txt if necessary */
	if ( userName[0] ) {
		/* Create supervisor */
		if ( !umGroupExists(DEFAULT_GROUP) )
			if ( umAddGroup(DEFAULT_GROUP, (short)PRIV_ADMIN, AM_BASIC, FALSE, FALSE) ) {
				error(E_L, E_LOG, T("ERROR: Unable to add group."));
				return;
			}
		if ( !umAccessLimitExists(ACCESS_URL) )
			if ( umAddAccessLimit(ACCESS_URL, AM_FULL, (short)0, DEFAULT_GROUP) ) {
				error(E_L, E_LOG, T("ERROR: Unable to add access limit."));
				return;
			}
		if(superName[0]){
			if ( !umUserExists(superName))
				if ( umAddUser(superName, superPass, DEFAULT_GROUP, FALSE, FALSE) ) {
					error(E_L, E_LOG, T("ERROR: Unable to add supervisor account."));
					return;
				}
		}

		/* Create user */
		if ( umUserExists(userName))
			umDeleteUser(userName);

		if ( umAddUser(userName, userPass, DEFAULT_GROUP, FALSE, FALSE) ) {
			error(E_L, E_LOG, T("ERROR: Unable to add user account."));
			return;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void formStats(webs_t wp, char_t *path, char_t *query)
{
	char_t *submitUrl;

	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page

	if (submitUrl[0])
		websRedirect(wp, submitUrl);
}

#ifdef CONFIG_RTK_MESH
void formMeshStatus(webs_t wp, char_t *path, char_t *query)
{
	char_t *submitUrl;

	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page

	if (submitUrl[0])
		websRedirect(wp, submitUrl);
}
#endif // CONFIG_RTK_MESH

#ifdef HOME_GATEWAY
/////////////////////////////////////////////////////////////////////////////
int  ntpHandler(webs_t wp, char *tmpBuf, int fromWizard)
{
	int enabled=0, ntpServerIdx ;
	struct in_addr ipAddr ;
	char *tmpStr ;
//Brad add for daylight save
	int dlenabled=0;
//Brad add end
	if (fromWizard) {
		tmpStr = websGetVar(wp, T("enabled"), T(""));
		if(!strcmp(tmpStr, "ON"))
			enabled = 1 ;
		else
			enabled = 0 ;

		if ( apmib_set( MIB_NTP_ENABLED, (void *)&enabled) == 0) {
			strcpy(tmpBuf, T("Set enabled flag error!"));
			goto setErr_ntp;
		}
//Brad add for daylight save
		tmpStr = websGetVar(wp, T("dlenabled"), T(""));
		if(!strcmp(tmpStr, "ON"))
			dlenabled = 1 ;
		else
			dlenabled = 0 ;

		if ( apmib_set( MIB_DAYLIGHT_SAVE, (void *)&dlenabled) == 0) {
			strcpy(tmpBuf, T("Set enabled flag error!"));
			goto setErr_ntp;
		}
//Brad add end
	}
	else
		enabled = 1;
	if(enabled){
		tmpStr = websGetVar(wp, T("ntpServerId"), T(""));
		if(tmpStr[0]){
			ntpServerIdx = tmpStr[0] - '0' ;
			if ( apmib_set(MIB_NTP_SERVER_ID, (void *)&ntpServerIdx) == 0) {
				strcpy(tmpBuf, T("Set Time Zone error!"));
				goto setErr_ntp;
			}
		}
		tmpStr = websGetVar(wp, T("timeZone"), T(""));
		if(tmpStr[0]){
			if ( apmib_set(MIB_NTP_TIMEZONE, (void *)tmpStr) == 0) {
					strcpy(tmpBuf, T("Set Time Zone error!"));
				goto setErr_ntp;
		}
		}

		tmpStr = websGetVar(wp, T("ntpServerIp1"), T(""));
		if(tmpStr[0]){
			inet_aton(tmpStr, &ipAddr);
			if ( apmib_set(MIB_NTP_SERVER_IP1, (void *)&ipAddr) == 0) {
				strcpy(tmpBuf, T("Set NTP server error!"));
				goto setErr_ntp;
			}
			}
		tmpStr = websGetVar(wp, T("ntpServerIp2"), T(""));
		if(tmpStr[0]){
			inet_aton(tmpStr, &ipAddr);
			if ( apmib_set(MIB_NTP_SERVER_IP2,(void *) &ipAddr ) == 0) {
				strcpy(tmpBuf, T("Set NTP server IP error!"));
				goto setErr_ntp;
			}
		}
	}
	return 0 ;
setErr_ntp:
	return -1 ;

}
void formNtp(webs_t wp, char_t *path, char_t *query)
{
	char_t *submitUrl,*strVal, *tmpStr;
	char tmpBuf[100];
	int enabled=0;
//Brad add for daylight save
	int dlenabled=0;
//Brad add end
#ifndef NO_ACTION
//	int pid;
#endif
	int time_value=0;
	int cur_year=0;

	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	strVal = websGetVar(wp, T("save"), T(""));

	if(strVal[0]){
		struct tm tm_time;
		time_t tm;
		memcpy(&tm_time, localtime(&tm), sizeof(tm_time));
		tm_time.tm_sec = 0;
		tm_time.tm_min = 0;
		tm_time.tm_hour = 0;
		tm_time.tm_isdst = -1;  /* Be sure to recheck dst. */
		strVal = websGetVar(wp, T("year"), T(""));
		cur_year= atoi(strVal);
		tm_time.tm_year = atoi(strVal) - 1900;
		strVal = websGetVar(wp, T("month"), T(""));
		tm_time.tm_mon = atoi(strVal)-1;
		strVal = websGetVar(wp, T("day"), T(""));
		tm_time.tm_mday = atoi(strVal);
		strVal = websGetVar(wp, T("hour"), T(""));
		tm_time.tm_hour = atoi(strVal);
		strVal = websGetVar(wp, T("minute"), T(""));
		tm_time.tm_min = atoi(strVal);
		strVal = websGetVar(wp, T("second"), T(""));
		tm_time.tm_sec = atoi(strVal);
		tm = mktime(&tm_time);
		if(tm < 0){
			sprintf(tmpBuf, "set Time Error\n");
			goto setErr_end;
		}
		if(stime(&tm) < 0){
			sprintf(tmpBuf, "set Time Error\n");
			goto setErr_end;
		}

		apmib_set( MIB_TIME_YEAR, (void *)&cur_year);
		time_value = tm_time.tm_mon;
		apmib_set( MIB_TIME_MONTH, (void *)&time_value);
		time_value = tm_time.tm_mday;
		apmib_set( MIB_TIME_DAY, (void *)&time_value);
		time_value = tm_time.tm_hour;
		apmib_set( MIB_TIME_HOUR, (void *)&time_value);
		time_value = tm_time.tm_min;
		apmib_set( MIB_TIME_MIN, (void *)&time_value);
		time_value = tm_time.tm_sec;
		apmib_set( MIB_TIME_SEC, (void *)&time_value);

		tmpStr = websGetVar(wp, T("timeZone"), T(""));
		if(tmpStr[0]){
			if ( apmib_set(MIB_NTP_TIMEZONE, (void *)tmpStr) == 0) {
					strcpy(tmpBuf, T("Set Time Zone error!"));
				goto setErr_end;
			}
		}

		tmpStr = websGetVar(wp, T("enabled"), T(""));
		if(!strcmp(tmpStr, "ON"))
			enabled = 1 ;
		else
			enabled = 0 ;
		if ( apmib_set( MIB_NTP_ENABLED, (void *)&enabled) == 0) {
			strcpy(tmpBuf, T("Set enabled flag error!"));
			goto setErr_end;
		}
//Brad add for daylight save
		tmpStr = websGetVar(wp, T("dlenabled"), T(""));
		if(!strcmp(tmpStr, "ON"))
			dlenabled = 1 ;
		else
			dlenabled = 0 ;
		if ( apmib_set( MIB_DAYLIGHT_SAVE, (void *)&dlenabled) == 0) {
			strcpy(tmpBuf, T("Set dl enabled flag error!"));
			goto setErr_end;
		}
//Brad add end
	}
	if (enabled == 0)
		goto  set_ntp_end;

	if(ntpHandler(wp, tmpBuf, 0) < 0)
		goto setErr_end ;

set_ntp_end:
	apmib_update_web(CURRENT_SETTING);
//Brad modify for system re-init method
#if 0
	pid = find_pid_by_name("ntp.sh");
	if(pid)
		kill(pid, SIGTERM);

	pid = fork();
        if (pid)
		waitpid(pid, NULL, 0);
        else if (pid == 0) {
		snprintf(tmpBuf, 100, "%s/%s", _CONFIG_SCRIPT_PATH, _NTP_SCRIPT_PROG);
		execl( tmpBuf, _NTP_SCRIPT_PROG, NULL);
               	exit(1);
       	}
#endif
#ifndef NO_ACTION
	run_init_script("all");
#endif
	OK_MSG(submitUrl);
	return;

setErr_end:
	ERR_MSG(tmpBuf);
}

#endif
extern int update_linkchain(int fmt, void *Entry_old, void *Entry_new, int type_size);

void formPocketWizard(webs_t wp, char_t *path, char_t *query)
{
	char_t *tmpStr, *strVal;
	char tmpBuf[100];
	char varName[20];
	int i=0;
	int mode=-1;
	int val;

//displayPostDate(wp->postData);

	wlan_idx = 0 ;

	strVal = websGetVar(wp, "mode0", T(""));
	val = atoi(strVal);
	apmib_set( MIB_WLAN_MODE, (void *)&val);

	strVal = websGetVar(wp, "pocket_ssid", T(""));
	apmib_set(MIB_WLAN_SSID, (void *)strVal);

	strVal = websGetVar(wp, "band0", T(""));
	val = strtol( strVal, (char **)NULL, 10);
	val = (val + 1);
	apmib_set( MIB_WLAN_BAND, (void *)&val);

	sprintf(varName, "method%d", i);
	tmpStr = websGetVar(wp, varName, T(""));
	if(tmpStr[0])
	{
		val = atoi(tmpStr);
		if(val == ENCRYPT_DISABLED)
		{
			ENCRYPT_T encrypt = ENCRYPT_DISABLED;
			apmib_set( MIB_WLAN_ENCRYPT, (void *)&encrypt);
		}
		else if(val == ENCRYPT_WEP)
		{
			if(wepHandler(wp, tmpBuf, i) < 0)
			{
				goto setErr_end;
			}
		}
		else if(val > ENCRYPT_WEP && val <= WSC_AUTH_WPA2PSKMIXED)
		{
			if(wpaHandler(wp, tmpBuf, i) < 0)
			{
				goto setErr_end;
			}
		}
	}


	apmib_update_web(CURRENT_SETTING);

#ifdef REBOOT_CHECK
	run_init_script_flag = 1;
#endif
#ifndef NO_ACTION
	run_init_script("all");
#endif
	tmpStr = websGetVar(wp, T("method0"), T(""));
	REBOOT_WAIT("/wizard.asp");

	return ;
setErr_end:

	OK_MSG1(tmpBuf,"/wizard.asp");
	return ;
}

void formPocketWizardGW(webs_t wp, char_t *path, char_t *query)
{
	char_t *tmpStr, *strVal;
	char tmpBuf[100];
	char varName[20];
	int i=0;
	int mode=-1;
	int val;
	int dns_changed=0;

//displayPostDate(wp->postData);
#ifdef HOME_GATEWAY
	if(tcpipWanHandler(wp, tmpBuf, &dns_changed) < 0){
		goto setErr_end;
	}
#endif
#if 0
	wlan_idx = 0 ;

	strVal = websGetVar(wp, "mode0", T(""));
	val = atoi(strVal);
	apmib_set( MIB_WLAN_MODE, (void *)&val);

	strVal = websGetVar(wp, "pocket_ssid", T(""));
	apmib_set(MIB_WLAN_SSID, (void *)strVal);

	strVal = websGetVar(wp, "band0", T(""));
	val = strtol( strVal, (char **)NULL, 10);
	val = (val + 1);
	apmib_set( MIB_WLAN_BAND, (void *)&val);

	sprintf(varName, "method%d", i);
	tmpStr = websGetVar(wp, varName, T(""));
	if(tmpStr[0])
	{
		val = atoi(tmpStr);
		if(val == ENCRYPT_DISABLED)
		{
			ENCRYPT_T encrypt = ENCRYPT_DISABLED;
			apmib_set( MIB_WLAN_ENCRYPT, (void *)&encrypt);
		}
		else if(val == ENCRYPT_WEP)
		{
			if(wepHandler(wp, tmpBuf, i) < 0)
			{
				goto setErr_end;
			}
		}
		else if(val > ENCRYPT_WEP && val <= WSC_AUTH_WPA2PSKMIXED)
		{
			if(wpaHandler(wp, tmpBuf, i) < 0)
			{
				goto setErr_end;
			}
		}
	}

#endif
	apmib_update_web(CURRENT_SETTING);

#ifdef REBOOT_CHECK
	run_init_script_flag = 1;
#endif
#ifndef NO_ACTION
	run_init_script("all");
#endif
	tmpStr = websGetVar(wp, T("hiwanType"), T(""));
	REBOOT_WAIT("/wizard.asp");

	return ;
setErr_end:

	OK_MSG1(tmpBuf,"/wizard.asp");
	return ;
}

void formWizard(webs_t wp, char_t *path, char_t *query)
{
	char_t *tmpStr;
	char tmpBuf[100];
	char varName[20];
	int i;
#ifdef HOME_GATEWAY
	int dns_changed=0;
#endif
	int mode=-1;
	char_t *submitUrl;
	char buffer[200];
	struct in_addr inLanaddr_orig, inLanaddr_new;
	struct in_addr inLanmask_orig, inLanmask_new;
	int	entryNum_resvdip;
	DHCPRSVDIP_T entry_resvdip, checkentry_resvdip;
	int link_type;
	struct in_addr private_host, tmp_private_host, update;
	struct in_addr dhcpRangeStart, dhcpRangeEnd;
	int isRptOn;
	int mibId;

//displayPostDate(wp->postData);


	apmib_get( MIB_IP_ADDR,  (void *)buffer); //save the orig lan subnet
	memcpy((void *)&inLanaddr_orig, buffer, 4);

	apmib_get( MIB_SUBNET_MASK,  (void *)buffer); //save the orig lan mask
	memcpy((void *)&inLanmask_orig, buffer, 4);
#ifdef HOME_GATEWAY
	if(opModeHandler(wp, tmpBuf) < 0)
		goto setErr_end;

	if(ntpHandler(wp, tmpBuf, 1) < 0)
		goto setErr_end;
#endif
	if(tcpipLanHandler(wp, tmpBuf) < 0){
		submitUrl = websGetVar(wp, T("submit-url-lan"), T(""));   // hidden page
		goto setErr_end;
	}

#ifdef HOME_GATEWAY
	if(tcpipWanHandler(wp, tmpBuf, &dns_changed) < 0){
		submitUrl = websGetVar(wp, T("submit-url-wan"), T(""));   // hidden page
		goto setErr_end;
	}
#endif
	for(i=0 ; i < wlan_num ;i++){
		wlan_idx = i ;
		sprintf(WLAN_IF, "wlan%d", wlan_idx);
		if(wlanHandler(wp, tmpBuf,&mode, i) < 0){
			submitUrl = websGetVar(wp, T("submit-url-wlan1"), T(""));   // hidden page
			goto setErr_end;
		}
		sprintf(varName, "method%d", i);
		tmpStr = websGetVar(wp, varName, T(""));
		if(tmpStr[0] && tmpStr[0] == '1'){
			if(wepHandler(wp, tmpBuf, i) < 0){
				submitUrl = websGetVar(wp, T("submit-url-wlan2"), T(""));   // hidden page
				goto setErr_end;
			}
		}
		if(wpaHandler(wp, tmpBuf, i) < 0){
			submitUrl = websGetVar(wp, T("submit-url-wlan2"), T(""));   // hidden page
			goto setErr_end;
		}

		sprintf(varName, "is_rpt_enabled%d", i);
		tmpStr = websGetVar(wp, varName, T(""));
		if(tmpStr[0])
		{
			isRptOn = atoi(tmpStr);

			if (wlan_idx == 0)
				mibId = MIB_REPEATER_ENABLED1;
			else
				mibId = MIB_REPEATER_ENABLED2;
			apmib_set(mibId, (void *)&isRptOn);
		}

		sprintf(varName, "isSurveyHandle%d", i);
		tmpStr = websGetVar(wp, varName, T(""));
		if(tmpStr[0] && tmpStr[0] == '1')
		{
			int wlanMode;

			sprintf(varName, "mode%d", i);
			tmpStr = websGetVar(wp, varName, T(""));
			wlanMode = atoi(tmpStr);

			if(wlanMode == AP_MODE && isRptOn == 1)
			{
				if(surveyHandle(wp, tmpBuf, i) != 1)
				{
					submitUrl = websGetVar(wp, T("submit-url-wlan2"), T(""));   // hidden page
					goto setErr_end;
				}
			}
		}
	}

	apmib_update_web(CURRENT_SETTING);
	apmib_get( MIB_IP_ADDR,  (void *)buffer); //check the new lan subnet
	memcpy((void *)&inLanaddr_new, buffer, 4);

	apmib_get( MIB_SUBNET_MASK,  (void *)buffer); //check the new lan mask
	memcpy((void *)&inLanmask_new, buffer, 4);

	if((inLanaddr_orig.s_addr & inLanmask_orig.s_addr) != (inLanaddr_new.s_addr & inLanmask_new.s_addr)){

		//check static dhcp ip
		apmib_get(MIB_DHCPRSVDIP_NUM, (void *)&entryNum_resvdip);
		link_type = 8; //DHCPRSVDIP_ARRY_T
		for (i=1; i<=entryNum_resvdip; i++) {
			memset(&checkentry_resvdip, '\0', sizeof(checkentry_resvdip));
			*((char *)&entry_resvdip) = (char)i;
			apmib_get(MIB_DHCPRSVDIP, (void *)&entry_resvdip);
			memcpy(&checkentry_resvdip, &entry_resvdip, sizeof(checkentry_resvdip));
			memcpy((void *)&private_host, &(entry_resvdip.ipAddr), 4);
			if((inLanaddr_new.s_addr & inLanmask_new.s_addr) != (private_host.s_addr & inLanmask_new.s_addr)){
				update.s_addr = inLanaddr_new.s_addr & inLanmask_new.s_addr;
				tmp_private_host.s_addr  = ~(inLanmask_new.s_addr) & private_host.s_addr;
				update.s_addr = update.s_addr | tmp_private_host.s_addr;
				memcpy((void *)&(checkentry_resvdip.ipAddr), &(update), 4);
				update_linkchain(link_type, &entry_resvdip, &checkentry_resvdip , sizeof(checkentry_resvdip));
			}
		}
		apmib_get( MIB_DHCP_CLIENT_START,  (void *)buffer); //save the orig dhcp start
		memcpy((void *)&dhcpRangeStart, buffer, 4);
		apmib_get( MIB_DHCP_CLIENT_END,  (void *)buffer); //save the orig dhcp end
		memcpy((void *)&dhcpRangeEnd, buffer, 4);

		if((dhcpRangeStart.s_addr & inLanmask_new.s_addr) != (inLanaddr_new.s_addr & inLanmask_new.s_addr)){
			update.s_addr = inLanaddr_new.s_addr & inLanmask_new.s_addr;
			tmp_private_host.s_addr  = ~(inLanmask_new.s_addr) & dhcpRangeStart.s_addr;
			update.s_addr = update.s_addr | tmp_private_host.s_addr;
			memcpy((void *)&(dhcpRangeStart), &(update), 4);
			apmib_set(MIB_DHCP_CLIENT_START, (void *)&dhcpRangeStart);
		}
		if((dhcpRangeEnd.s_addr & inLanmask_new.s_addr) != (inLanaddr_new.s_addr & inLanmask_new.s_addr)){
			update.s_addr = inLanaddr_new.s_addr & inLanmask_new.s_addr;
			tmp_private_host.s_addr  = ~(inLanmask_new.s_addr) & dhcpRangeEnd.s_addr;
			update.s_addr = update.s_addr | tmp_private_host.s_addr;
			memcpy((void *)&(dhcpRangeEnd), &(update), 4);
			apmib_set(MIB_DHCP_CLIENT_END, (void *)&dhcpRangeEnd);
		}

		apmib_update_web(CURRENT_SETTING);
	}

#ifdef REBOOT_CHECK
	run_init_script_flag = 1;
#endif
#ifndef NO_ACTION
	run_init_script("all");
#endif
	submitUrl = websGetVar(wp, T("next_url"), T(""));
	REBOOT_WAIT("/wizard.asp");

	return ;
setErr_end:

	OK_MSG1(tmpBuf,"/wizard.asp");
	return ;

}

///////////////////////////////////////////////////////////////////////////////////////////////
int logout=0 ;
void formLogout(webs_t wp, char_t *path, char_t *query)
{
	char_t *logout_str, *return_url;
	logout_str = websGetVar(wp, T("logout"), T(""));
	if (logout_str[0]) {
		logout = 1 ;
#ifdef LOGIN_URL
		delete_user(wp);
	    OK_MSG("/login.asp");
	    return;
#endif
	}

	return_url = websGetVar(wp, T("return-url"), T(""));

#ifdef REBOOT_CHECK
	websRedirect(wp, return_url);
#else
        OK_MSG(return_url);
#endif

	return;
}
#define _PATH_SYSCMD_LOG "/tmp/syscmd.log"

void formSysCmd(webs_t wp, char_t *path, char_t *query)
{
	char_t  *submitUrl, *sysCmd;
#ifndef NO_ACTION
	char_t tmpBuf[100];
#endif

	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	sysCmd = websGetVar(wp, T("sysCmd"), T(""));   // hidden page

#ifndef NO_ACTION
	if(sysCmd[0]){
		snprintf(tmpBuf, 100, "%s 2>&1 > %s",sysCmd,  _PATH_SYSCMD_LOG);
		system(tmpBuf);
	}
#endif
		websRedirect(wp, submitUrl);
	return;
}

int sysCmdLog(int eid, webs_t wp, int argc, char_t **argv)
{
        FILE *fp;
	char  buf[150];
	int nBytesSent=0;

        fp = fopen(_PATH_SYSCMD_LOG, "r");
        if ( fp == NULL )
                goto err1;
        while(fgets(buf,150,fp)){
		nBytesSent += websWrite(wp, T("%s"), buf);
        }
	fclose(fp);
	unlink(_PATH_SYSCMD_LOG);
err1:
	return nBytesSent;
}


#ifdef HOME_GATEWAY

int  opModeHandler(webs_t wp, char *tmpBuf)
{
	char_t *tmpStr;
	int opmode, wanId;

	tmpStr = websGetVar(wp, T("opMode"), T(""));
	if(tmpStr[0]){
		opmode = tmpStr[0] - '0' ;
		if ( apmib_set(MIB_OP_MODE, (void *)&opmode) == 0) {
			strcpy(tmpBuf, T("Set Opmode error!"));
			goto setErr_opmode;
		}
	}
	tmpStr = websGetVar(wp, T("wispWanId"), T(""));
	if(tmpStr[0]){
		wanId = tmpStr[0] - '0' ;
		if ( apmib_set(MIB_WISP_WAN_ID, (void *)&wanId) == 0) {
			strcpy(tmpBuf, T("Set WISP WAN Id error!"));
			goto setErr_opmode;
		}
	}
	return 0;

setErr_opmode:
	return -1;

}
void formOpMode(webs_t wp, char_t *path, char_t *query)
{
	char_t *submitUrl;
	char tmpBuf[100];

	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page

	if(opModeHandler(wp, tmpBuf) < 0)
			goto setErr;

	apmib_update_web(CURRENT_SETTING);

#ifdef REBOOT_CHECK
	run_init_script_flag = 1;
#endif
#ifndef NO_ACTION
	run_init_script("all");
#endif

#ifdef REBOOT_CHECK
	REBOOT_WAIT(submitUrl);
#else //#ifdef REBOOT_CHECK	.
	OK_MSG(submitUrl);
#endif //#ifdef REBOOT_CHECK

	return;

setErr:
	ERR_MSG(tmpBuf);
}
#endif

#ifdef REBOOT_CHECK
void formRebootCheck(webs_t wp, char_t *path, char_t *query)
{
	char_t *submitUrl;
	char tmpBuf[100];

	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page

	apmib_update_web(CURRENT_SETTING);
#ifdef REBOOT_CHECK
	run_init_script_flag = 1;
#endif
#ifndef NO_ACTION
	run_init_script("all");
#endif

	REBOOT_WAIT(submitUrl);
	needReboot = 0;

	return;

setErr:
	ERR_MSG(tmpBuf);
}
#endif //#ifdef REBOOT_CHECK

void formSysLog(webs_t wp, char_t *path, char_t *query)
{
	char_t *submitUrl, *tmpStr;
	char tmpBuf[100];
	int enabled, rt_enabled;
	struct in_addr ipAddr ;

	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page

	tmpStr = websGetVar(wp, T("clear"), T(""));
	if(tmpStr[0]){
		snprintf(tmpBuf, 100, "echo \" \" > %s", "/var/log/messages");
		system(tmpBuf);
#ifdef RINGLOG
		system("rm /var/log/message.* >/dev/null 2>&1");
#endif
		websRedirect(wp, submitUrl);
		return;
	}

/*
 *	NOTE: If variable enabled (MIB_LOG_ENABLED) bitmask modify(bitmap),
 *	 	Please modify driver rtl8190 reference variable (dot1180211sInfo.log_enabled in linux-2.4.18/drivers/net/rtl8190/8190n_cfg.h)
 */
	apmib_get(MIB_LOG_ENABLED, (void *)&enabled);

	tmpStr = websGetVar(wp, T("logEnabled"), T(""));
	if(!strcmp(tmpStr, "ON")) {
		enabled |= 1;

		tmpStr = websGetVar(wp, T("syslogEnabled"), T(""));
		if(!strcmp(tmpStr, "ON"))
			enabled |= 2;
		else
			enabled &= ~2;

		tmpStr = websGetVar(wp, T("wlanlogEnabled"), T(""));
		if(!strcmp(tmpStr, "ON"))
			enabled |= 4;
		else
			enabled &= ~4;

#ifdef HOME_GATEWAY
#ifdef DOS_SUPPORT
		tmpStr = websGetVar(wp, T("doslogEnabled"), T(""));
		if(!strcmp(tmpStr, "ON"))
			enabled |= 8;
		else
			enabled &= ~8;
#endif
#endif

#ifdef CONFIG_RTK_MESH
		tmpStr = websGetVar(wp, T("meshlogEnabled"), T(""));
		if(!strcmp(tmpStr, "ON"))
			enabled |= 16;
		else
			enabled &= ~16;
#endif

	}
	else
		enabled &= ~1;

	if ( apmib_set(MIB_LOG_ENABLED, (void *)&enabled) == 0) {
		strcpy(tmpBuf, T("Set log enable error!"));
		goto setErr;
	}

	if(enabled & 1){
		tmpStr = websGetVar(wp, T("rtLogEnabled"), T(""));

		if(!strcmp(tmpStr, "ON"))
			rt_enabled= 1;
		else
			rt_enabled= 0;
		if ( apmib_set(MIB_REMOTELOG_ENABLED, (void *)&rt_enabled) == 0) {
			strcpy(tmpBuf, T("Set remote log enable error!"));
			goto setErr;
		}

		tmpStr = websGetVar(wp, T("logServer"), T(""));
		if(tmpStr[0]){
			inet_aton(tmpStr, &ipAddr);
			if ( apmib_set(MIB_REMOTELOG_SERVER, (void *)&ipAddr) == 0) {
				strcpy(tmpBuf, T("Set remote log server error!"));
				goto setErr;
			}
		}
	}
	apmib_update_web(CURRENT_SETTING);
#ifndef NO_ACTION
	run_init_script("all");
#endif
	OK_MSG(submitUrl);
	return;

setErr:
	ERR_MSG(tmpBuf);
}

static int process_msg(char *msg, int is_wlan_only)
{
	char *p1, *p2;
	p1 = strstr(msg, "(none)"); // host name
	if (p1 == NULL)
		return 0;

#ifdef CONFIG_RTK_MESH
	if (is_wlan_only == 4) {
		p2 = strstr(p1, "msh");
		if (p2 && p2[4]==':')
			memcpy(p1, p2, strlen(p2)+1);
		else
			return 0;

	}else
#endif

	if (is_wlan_only == 3){
		p2 = strstr(p1, "DoS");
		if (p2 && p2[3]==':'){
			memcpy(p1, p2, strlen(p2)+1);
		}else{
			p2 = strstr(p1, "wlan");
			if ((p2 && p2[5]==':') || (p2 && p2[9]==':'))	{// vxd interface
				memcpy(p1, p2, strlen(p2)+1);
			}else
				return 0;
			}
	}else if (is_wlan_only == 2){
		p2 = strstr(p1, "DoS");
		if (p2 && p2[3]==':')
			memcpy(p1, p2, strlen(p2)+1);
		else
			return 0;

	}else{
		p2 = strstr(p1, "wlan");
		if ((p2 && p2[5]==':') ||
			 (p2 && p2[9]==':'))	// vxd interface
			memcpy(p1, p2, strlen(p2)+1);
		else {
			if (is_wlan_only)
				return 0;

			p2 = strstr(p1, "klogd: ");
			if (p2 == NULL)
				return 0;
			memcpy(p1, p2+7, strlen(p2)-7+1);
		}
	}
	return 1;
}

int sysLogList(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fp;
	char  buf[200];
	int nBytesSent=0;
	int enabled;
#ifdef RINGLOG
	char logname[32];
	int lognum = LOG_SPLIT;
#endif

	apmib_get(MIB_LOG_ENABLED, (void *)&enabled);
	if ( !(enabled & 1))
		goto err1;

#ifdef RINGLOG
	fp = fopen("/var/log/log_split", "r");
	if (fp == NULL)
		goto err1;
	fgets(buf,200,fp);
	lognum = atoi(buf);
	fclose(fp);

while (lognum >= 0)
{
	if (lognum > 0)
		snprintf(logname, 32, "/var/log/messages.%d", lognum-1);
	else if (lognum == 0)
		snprintf(logname, 32, "/var/log/messages");
	else
		goto err1;

	fp = fopen(logname, "r");
	if (fp == NULL)
		goto next_log;
#else
	fp = fopen("/var/log/messages", "r");
	if (fp == NULL)
		goto err1;
#endif

	while(fgets(buf,200,fp)){
		int ret=0;
		if (enabled&2) // system all
			ret = process_msg(buf, 0);
		else {
			if((enabled&0xC) == 0xC){ //both wlan and DoS
				ret = process_msg(buf, 3);
			}else if (enabled&4)	// wlan only
				ret = process_msg(buf, 1);
			else if (enabled&8)	//DoS only
				ret = process_msg(buf, 2);

#ifdef CONFIG_RTK_MESH
			 if(enabled&16 && ret==0)	// mesh only
				ret = process_msg(buf, 4);
#endif

		}
		if (ret==0)
			continue;

		nBytesSent += websWrite(wp, T("%s"), buf);
	}
	fclose(fp);

#ifdef RINGLOG
next_log:
	lognum--;
}
#endif
err1:
	return nBytesSent;
}

#ifdef HOME_GATEWAY
#ifdef DOS_SUPPORT
void formDosCfg(webs_t wp, char_t *path, char_t *query)
{
	char_t	*submitUrl, *tmpStr;
	char	tmpBuf[100];
	int	floodCount=0,blockTimer=0;
	long	enabled = 0;

	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page

	apmib_get(MIB_DOS_ENABLED, (void *)&enabled);

	tmpStr = websGetVar(wp, T("dosEnabled"), T(""));
	if(!strcmp(tmpStr, "ON")) {
		enabled |= 1;

		tmpStr = websGetVar(wp, T("sysfloodSYN"), T(""));
		if(!strcmp(tmpStr, "ON")) {
			enabled |= 2;
			tmpStr = websGetVar(wp, T("sysfloodSYNcount"), T(""));
			string_to_dec(tmpStr,&floodCount);
			if ( apmib_set(MIB_DOS_SYSSYN_FLOOD, (void *)&floodCount) == 0) {
				strcpy(tmpBuf, T("Set DoS SYSSYN_FLOOD error!"));
				goto setErr;
			}
		}
		else{
			enabled &= ~2;
		}
		tmpStr = websGetVar(wp, T("sysfloodFIN"), T(""));
		if(!strcmp(tmpStr, "ON")) {
			enabled |= 4;
			tmpStr = websGetVar(wp, T("sysfloodFINcount"), T(""));
			string_to_dec(tmpStr,&floodCount);
			if ( apmib_set(MIB_DOS_SYSFIN_FLOOD, (void *)&floodCount) == 0) {
				strcpy(tmpBuf, T("Set DoS SYSFIN_FLOOD error!"));
				goto setErr;
			}
		}
		else{
			enabled &= ~4;
		}
		tmpStr = websGetVar(wp, T("sysfloodUDP"), T(""));
		if(!strcmp(tmpStr, "ON")) {
			enabled |= 8;
			tmpStr = websGetVar(wp, T("sysfloodUDPcount"), T(""));
			string_to_dec(tmpStr,&floodCount);
			if ( apmib_set(MIB_DOS_SYSUDP_FLOOD, (void *)&floodCount) == 0) {
				strcpy(tmpBuf, T("Set DoS SYSUDP_FLOOD error!"));
				goto setErr;
			}
		}
		else{
			enabled &= ~8;
		}
		tmpStr = websGetVar(wp, T("sysfloodICMP"), T(""));
		if(!strcmp(tmpStr, "ON")) {
			enabled |= 0x10;
			tmpStr = websGetVar(wp, T("sysfloodICMPcount"), T(""));
			string_to_dec(tmpStr,&floodCount);
			if ( apmib_set(MIB_DOS_SYSICMP_FLOOD, (void *)&floodCount) == 0) {
				strcpy(tmpBuf, T("Set DoS SYSICMP_FLOOD error!"));
				goto setErr;
			}
		}
		else{
			enabled &= ~0x10;
		}
		tmpStr = websGetVar(wp, T("ipfloodSYN"), T(""));
		if(!strcmp(tmpStr, "ON")) {
			enabled |= 0x20;
			tmpStr = websGetVar(wp, T("ipfloodSYNcount"), T(""));
			string_to_dec(tmpStr,&floodCount);
			if ( apmib_set(MIB_DOS_PIPSYN_FLOOD, (void *)&floodCount) == 0) {
				strcpy(tmpBuf, T("Set DoS PIPSYN_FLOOD error!"));
				goto setErr;
			}
		}
		else{
			enabled &= ~0x20;
		}
		tmpStr = websGetVar(wp, T("ipfloodFIN"), T(""));
		if(!strcmp(tmpStr, "ON")) {
			enabled |= 0x40;
			tmpStr = websGetVar(wp, T("ipfloodFINcount"), T(""));
			string_to_dec(tmpStr,&floodCount);
			if ( apmib_set(MIB_DOS_PIPFIN_FLOOD, (void *)&floodCount) == 0) {
				strcpy(tmpBuf, T("Set DoS PIPFIN_FLOOD error!"));
				goto setErr;
			}
		}
		else{
			enabled &= ~0x40;
		}
		tmpStr = websGetVar(wp, T("ipfloodUDP"), T(""));
		if(!strcmp(tmpStr, "ON")) {
			enabled |= 0x80;
			tmpStr = websGetVar(wp, T("ipfloodUDPcount"), T(""));
			string_to_dec(tmpStr,&floodCount);
			if ( apmib_set(MIB_DOS_PIPUDP_FLOOD, (void *)&floodCount) == 0) {
				strcpy(tmpBuf, T("Set DoS PIPUDP_FLOOD error!"));
				goto setErr;
			}
		}
		else{
			enabled &= ~0x80;
		}
		tmpStr = websGetVar(wp, T("ipfloodICMP"), T(""));
		if(!strcmp(tmpStr, "ON")) {
			enabled |= 0x100;
			tmpStr = websGetVar(wp, T("ipfloodICMPcount"), T(""));
			string_to_dec(tmpStr,&floodCount);
			if ( apmib_set(MIB_DOS_PIPICMP_FLOOD, (void *)&floodCount) == 0) {
				strcpy(tmpBuf, T("Set DoS PIPICMP_FLOOD error!"));
				goto setErr;
			}
		}
		else{
			enabled &= ~0x100;
		}
		tmpStr = websGetVar(wp, T("TCPUDPPortScan"), T(""));
		if(!strcmp(tmpStr, "ON")) {
			enabled |= 0x200;

			tmpStr = websGetVar(wp, T("portscanSensi"), T(""));
			if( tmpStr[0]=='1' ) {
				enabled |= 0x800000;
			}
			else{
				enabled &= ~0x800000;
			}
		}
		else{
			enabled &= ~0x200;
		}
		tmpStr = websGetVar(wp, T("ICMPSmurfEnabled"), T(""));
		if(!strcmp(tmpStr, "ON")) {
			enabled |= 0x400;
		}
		else{
			enabled &= ~0x400;
		}
		tmpStr = websGetVar(wp, T("IPLandEnabled"), T(""));
		if(!strcmp(tmpStr, "ON")) {
			enabled |= 0x800;
		}
		else{
			enabled &= ~0x800;
		}
		tmpStr = websGetVar(wp, T("IPSpoofEnabled"), T(""));
		if(!strcmp(tmpStr, "ON")) {
			enabled |= 0x1000;
		}
		else{
			enabled &= ~0x1000;
		}
		tmpStr = websGetVar(wp, T("IPTearDropEnabled"), T(""));
		if(!strcmp(tmpStr, "ON")) {
			enabled |= 0x2000;
		}
		else{
			enabled &= ~0x2000;
		}
		tmpStr = websGetVar(wp, T("PingOfDeathEnabled"), T(""));
		if(!strcmp(tmpStr, "ON")) {
			enabled |= 0x4000;
		}
		else{
			enabled &= ~0x4000;
		}
		tmpStr = websGetVar(wp, T("TCPScanEnabled"), T(""));
		if(!strcmp(tmpStr, "ON")) {
			enabled |= 0x8000;
		}
		else{
			enabled &= ~0x8000;
		}
		tmpStr = websGetVar(wp, T("TCPSynWithDataEnabled"), T(""));
		if(!strcmp(tmpStr, "ON")) {
			enabled |= 0x10000;
		}
		else{
			enabled &= ~0x10000;
		}
		tmpStr = websGetVar(wp, T("UDPBombEnabled"), T(""));
		if(!strcmp(tmpStr, "ON")) {
			enabled |= 0x20000;
		}
		else{
			enabled &= ~0x20000;
		}
		tmpStr = websGetVar(wp, T("UDPEchoChargenEnabled"), T(""));
		if(!strcmp(tmpStr, "ON")) {
			enabled |= 0x40000;
		}
		else{
			enabled &= ~0x40000;
		}
		tmpStr = websGetVar(wp, T("sourceIPblock"), T(""));
		if(!strcmp(tmpStr, "ON")) {
			enabled |= 0x400000;
			tmpStr = websGetVar(wp, T("IPblockTime"), T(""));
			string_to_dec(tmpStr,&blockTimer);
			if ( apmib_set(MIB_DOS_BLOCK_TIME, (void *)&blockTimer) == 0) {
				strcpy(tmpBuf, T("Set DoS IP Block Timer error!"));
				goto setErr;
			}
		}
		else{
			enabled &= ~0x400000;
		}
	}
	else
		enabled = 0;

	if ( apmib_set(MIB_DOS_ENABLED, (void *)&enabled) == 0) {
		strcpy(tmpBuf, T("Set DoS enable error!"));
		goto setErr;
	}

	apmib_update_web(CURRENT_SETTING);

#ifndef NO_ACTION
	run_init_script("all");
#endif

	OK_MSG(submitUrl);
	return;

setErr:
	ERR_MSG(tmpBuf);
}
#endif
#endif


#ifdef LOGIN_URL

#define MAX_USER	5
#define ACCESS_TIMEOUT	 300	// 5m

#define MAGIC_NUMER	7168186

struct user_profile {
	int flag;
	time_t last_time;
	char_t ipaddr[32];
};

static struct user_profile users[MAX_USER];

///////////////////////////////////////////////////////////////////
static void delete_user(webs_t wp)
{
	int i;
	for (i=0; i<MAX_USER; i++) {
		if (users[i].flag == MAGIC_NUMER && !strcmp(wp->ipaddr, users[i].ipaddr)) {
			users[i].flag = 0;
			return;
		}
	}
}

///////////////////////////////////////////////////////////////////
static int add_user(webs_t wp)
{
	int i;
	for (i=0; i<MAX_USER; i++) {
		if (users[i].flag == MAGIC_NUMER && strcmp(wp->ipaddr, users[i].ipaddr) &&
			((unsigned long)wp->timestamp)-((unsigned long)users[i].last_time) < ACCESS_TIMEOUT )
			continue;

		users[i].flag = MAGIC_NUMER;
		users[i].last_time = wp->timestamp;
		strcpy(users[i].ipaddr, wp->ipaddr);
		return 0;
	}

	printf("webs: add_user error (exceed max connection)!\n");

	return -1;
}

///////////////////////////////////////////////////////////////////
int is_valid_user(webs_t wp)
{
	int i;
	for (i=0; i<MAX_USER; i++) {
		if (users[i].flag == MAGIC_NUMER && !strcmp(wp->ipaddr, users[i].ipaddr)) {
			if (((unsigned long)wp->timestamp)-((unsigned long)users[i].last_time) > ACCESS_TIMEOUT)
				return -1; // timeout
			return 1;
		}
	}

	return 0; // not a valid user
}

///////////////////////////////////////////////////////////////////
void formLogin(webs_t wp, char_t *path, char_t *query)
{
	char_t *strUser, *strPassword, *userpass;
	char tmpbuf[200];

	strUser = websGetVar(wp, T("username"), T(""));
	strPassword = websGetVar(wp, T("password"), T(""));
	if ( strUser[0] && !strPassword[0] ) {
		strcpy(tmpbuf, T("ERROR: Password cannot be empty."));
		goto login_err;
	}

	if (!umUserExists(strUser)) {
		strcpy(tmpbuf, T("ERROR: Access denied, unknown user!"));
		goto login_err;
	}
	userpass = umGetUserPassword(strUser);
	if (userpass) {
		if (strcmp(strPassword, userpass) != 0) {
			strcpy(tmpbuf, T("ERROR: Access denied, unknown user!"));
			goto login_err;
		}
	}

	if (add_user(wp) < 0) {
		strcpy(tmpbuf, T("ERROR: Exceed max user number!"));
		goto login_err;
	}

	websRedirect(wp, T("home.asp"));
	return;

login_err:
	ERR_MSG(tmpbuf);
}
#endif // LOGIN_URL

#if defined(POWER_CONSUMPTION_SUPPORT)
unsigned int pre_cpu_d4, pre_time_secs, max_cpu_delta=0;
unsigned int ethBytesCount_previous[5] = {0};

/* http://www.360doc.com/content/070213/11/17255_365683.html */
int getPowerConsumption(int eid, webs_t wp, int argc, char_t **argv)
{
	char dev[80];
	char *devPtr;
	FILE *stream;
	int i=1,j;
	//char logbuf[500];
	unsigned int rxbytes=0,rxpackets=0,rxerrs=0,rxdrops=0,txbytes=0,txpackets=0,txerrs=0,txdrops=0,txcolles=0;
	unsigned int txeth0packets=0;
	unsigned int tmp1,tmp2,tmp3,tmp4;
	char askfor[20];

//	unsigned int totalPwrCon = 0;
	unsigned int totalPwrCon = (rand()%2 ? 10 :0);

	typedef enum { NO_LINK=0, NORMAL_LINK=1, EEE_LINK=2} ETHERNET_LINK_T;
	unsigned short isLink_eth0[5]={0};
	unsigned short ethLinkNum= 0, ethEeeLinkNum = 0;
	unsigned short perEthPwrCon = PWRCON_PER_ETHERNET;
	unsigned int perEthEeeMinus = PWRCON_PER_EEE_ETHERNET_LINK_MINUS; // mw*100
	unsigned int perEthEeePwrCon = PWRCON_PER_EEE_ETHERNET; // mw*100/Mbps
	unsigned int ethThroughPut[5] = {0};
	unsigned int ethEeeThroughPut_Total = 0;
	int ethPwrCon_Total = 0;

	typedef enum { CHIP_UNKNOWN=0, CHIP_RTL8188C=1, CHIP_RTL8192C=2} CHIP_VERSION_T;
	CHIP_VERSION_T chipVersion = CHIP_UNKNOWN;

	typedef enum { CPU_NORMAL=0, CPU_SUSPEND=1} CPU_MODE_T;
	CPU_MODE_T cpuMode = CPU_NORMAL;
	unsigned short cpuPwrCon[3][2] = { {0,0},{PWRCON_CPU_NORMAL_88C,PWRCON_CPU_SUSPEND_88C},{PWRCON_CPU_NORMAL_92C,PWRCON_CPU_SUSPEND_92C} }; // 3:chipVersion; 2:cpu mode

	typedef enum { WLAN_OFF=0, WLAN_NO_LINK=1, WLAN_LINK=2} WLAN_STATE_T;
	WLAN_STATE_T wlanState = WLAN_OFF;
	unsigned short wlanStatePwrCon[3][3] = { {0,0,0},{PWRCON_WLAN_OFF_88C,PWRCON_WLAN_NOLINK_88C,PWRCON_WLAN_LINK_88C},{PWRCON_WLAN_OFF_92C,PWRCON_WLAN_NOLINK_92C,PWRCON_WLAN_LINK_92C}}; //3:chipVersion; 3:wlanState
	int wlanOff = 0;

	typedef enum { WLAN_MCS8_15=0, WLAN_MCS0_7=1, WLAN_OFDM=2, WLAN_CCK=3} WLAN_TRAFFIC_STATE_T;
	WLAN_TRAFFIC_STATE_T wlanTrafficState = WLAN_MCS8_15;
	unsigned int wlanTrafficStatePwrCon[3][4] = { {0,0,0,0},{PWRCON_WLAN_TRAFFIC_MCS8_15_88C,PWRCON_WLAN_TRAFFIC_MCS0_7_88C,PWRCON_WLAN_TRAFFIC_OFDM_88C,PWRCON_WLAN_TRAFFIC_CCK_88C},{PWRCON_WLAN_TRAFFIC_MCS8_15_92C,PWRCON_WLAN_TRAFFIC_MCS0_7_92C,PWRCON_WLAN_TRAFFIC_OFDM_92C,PWRCON_WLAN_TRAFFIC_CCK_92C}}; //3:chipVersion; 4:wlanTrafficState
	unsigned int wlanTrafficStatePwrConZ[3][28] = {
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{1000,1099,1188,1454,2014,3254,4271,8039,1082,1176,1289,1681,2768,4482,6275,10458,719,919,1225,1697,2377,3735,5038,7557,938,1681,2894,5865},
		{1000,1099,1188,1454,2014,3254,4271,8039,1082,1176,1289,1681,2768,4482,6275,10458,1000,1278,1705,2360,3306,5195,7008,10511,938,1681,2894,5865}
	};//3:chipVersion; 28:DataRate MCS15~1
	unsigned int wlanTrafficZ = 0;

	unsigned int tx_average = 0;
	unsigned short tx_average_multiply2 = 0;
	unsigned int rx_average = 0;
	unsigned int wlanTrafficStatePwrCon_Total;

	unsigned int cpuUtilizationPwrCon[3] = { 0,PWRCON_CPU_UTILIZATION_88C,PWRCON_CPU_UTILIZATION_92C}; //3:chipVersion;
	unsigned short cpu_utilization=0;

	unsigned short debug_check = 0;

	time_t current_secs;
	unsigned int time_delta = 1;
#if 0
	for(i=0 ;i<3;i++)
		for(j=0; j<1; j++)
			fprintf(stderr,"\r\n cpuUtilizationPwrCon[%d][%d]=[%f]",i,j,cpuUtilizationPwrCon[i][j]);
#endif

	//get current system time in second.
	time(&current_secs);
	if(pre_time_secs == 0) //first time
	{
		pre_time_secs = (int)(current_secs);
		time_delta = 1;
	}
	else
	{
		time_delta = (int)(current_secs) - (int)(pre_time_secs);
		pre_time_secs = (int)(current_secs);
	}


	//get chipVersion
	stream = fopen ( "/var/pwrConDebug", "r" );
	if ( stream != NULL )
	{
		char *strtmp;
		char line[100];
		char strTmp[10];

		while (fgets(line, sizeof(line), stream))
		{
			strtmp = line;

			while(*strtmp == ' ')
			{
				strtmp++;
			}

			sscanf(strtmp,"%[01]",strTmp);

			debug_check=atoi(strTmp);

		}

		fclose ( stream );
	}


	if(debug_check)
		fprintf(stderr,"\r\n  === Pwr Con Debug ===");
	//get chipVersion
	chipVersion = getWLAN_ChipVersion();
#if 0
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
				if(debug_check)
					fprintf(stderr,"\r\n [%s]",strtmp);
				chipVersion = CHIP_RTL8188C;
			}
			else if(strstr(strtmp,"RTL8192C") != 0)
			{
				if(debug_check)
					fprintf(stderr,"\r\n [%s]",strtmp);
				chipVersion = CHIP_RTL8192C;
			}
		}
		fclose ( stream );
	}
#endif

	if(debug_check)
	{
		fprintf(stderr,"\r\n chipVersion=[%u]",chipVersion);
		fprintf(stderr,"\r\n");
	}

	//get cpu mode
	stream = fopen ( "/proc/suspend_check", "r" );
	if ( stream != NULL )
	{
		char *strtmp;
		char line[100];

		while (fgets(line, sizeof(line), stream))
		{
			//enable=1, winsize=5(10), high=3200, low=2200, suspend=1
			strtmp = strstr(line,"suspend");
			if(strtmp != NULL)
			{

				//suspend=1
				if(debug_check)
					fprintf(stderr,"\r\n [%s]",strtmp);
				sscanf(strtmp,"%*[^=]=%u",&cpuMode);
			}

		}
		fclose ( stream );
	}
	if(debug_check)
	{
		fprintf(stderr,"\r\n cpuMode=[%u]",cpuMode);
		fprintf(stderr,"\r\n cpuPwrCon=[%u]",cpuPwrCon[chipVersion][cpuMode]);
		fprintf(stderr,"\r\n");
	}
	totalPwrCon+=cpuPwrCon[chipVersion][cpuMode];

	//get Eth0 port link and bytesCount
	for(i=0; i<5; i++)
	{
		unsigned int ethBytesCount[5] = {0};

		isLink_eth0[i]=getEth0PortLink(i);
		if(isLink_eth0[i])
		{
			isLink_eth0[i] = NORMAL_LINK;
			if(getEthernetEeeState(i))
				isLink_eth0[i] = EEE_LINK;
	}
		else
		{
			isLink_eth0[i] = NO_LINK;
		}

		ethBytesCount[i] = getEthernetBytesCount(i);

		if(time_delta <= 0)
			time_delta = 1;
		ethThroughPut[i] = (ethBytesCount[i] - ethBytesCount_previous[i])/time_delta;
		ethBytesCount_previous[i] = ethBytesCount[i];
	}

	for(i=0; i<5; i++)
	{
		if(isLink_eth0[i] == NORMAL_LINK)
		{
			ethLinkNum++;
		}
		else if(isLink_eth0[i] == EEE_LINK)
		{
			ethEeeLinkNum++;
			ethEeeThroughPut_Total += ethThroughPut[i];
		}
	}
	ethEeeThroughPut_Total *= 8; // transfer to bits.

	ethPwrCon_Total += ethLinkNum*perEthPwrCon;
	ethPwrCon_Total -= (ethEeeLinkNum*perEthEeeMinus)/100;
	ethPwrCon_Total += (((float)ethEeeThroughPut_Total*perEthEeePwrCon)/100)/1000000;


	if(debug_check)
	{
		fprintf(stderr,"\r\n Eth Link State:%u-%u-%u-%u-%u", isLink_eth0[0],isLink_eth0[1],isLink_eth0[2],isLink_eth0[3],isLink_eth0[4]);
		fprintf(stderr,"\r\n Eth ThroughPut:%u-%u-%u-%u-%u (bits/sec)", ethThroughPut[0]*8,ethThroughPut[1]*8,ethThroughPut[2]*8,ethThroughPut[3]*8,ethThroughPut[4]*8);
		fprintf(stderr,"\r\n ethEeeThroughPut_Total: %u (bits/sec)",ethEeeThroughPut_Total);
		fprintf(stderr,"\r\n perEthPwrCon Total: (%u*%u)-(%u*%u)/100+(%u*%u)/100/10^6 = %u",ethLinkNum,perEthPwrCon,ethEeeLinkNum,perEthEeeMinus,ethEeeThroughPut_Total,perEthEeePwrCon,ethPwrCon_Total);
		fprintf(stderr,"\r\n");
	}
	totalPwrCon+=ethPwrCon_Total;

	//get wlan state
	apmib_get( MIB_WLAN_DISABLED, (void *)&wlanOff);
	if(wlanOff)
		wlanState = WLAN_OFF;
	else
	{
		stream = fopen ( "proc/wlan0/sta_info", "r" );
		if ( stream != NULL )
		{
			char *strtmp;
			char line[100];
			while (fgets(line, sizeof(line), stream))
			{
				unsigned char *p;
				strtmp = line;

				while(*strtmp == ' ')
					strtmp++;

				if(strstr(strtmp,"active") != 0)
				{
					unsigned char str1[10], str2[10];

					//-- STA info table -- (active: 1)
					if(debug_check)
						fprintf(stderr,"\r\n [%s]",strtmp);

					sscanf(strtmp, "%*[^:]:%[^)]",str1);

					p = str1;
					while(*p == ' ')
						p++;

					if(strcmp(p,"0") == 0)
					{
						wlanState = WLAN_NO_LINK;
					}
					else
					{
						wlanState = WLAN_LINK;
					}

					break;
				}

			}
			fclose(stream );

		}
	}

	if(debug_check)
					{
		fprintf(stderr,"\r\n wlanState=[%u]",wlanState);
		fprintf(stderr,"\r\n wlanStatePwrCon = [%u]",wlanStatePwrCon[chipVersion][wlanState]);
		fprintf(stderr,"\r\n");
	}

	totalPwrCon+=wlanStatePwrCon[chipVersion][wlanState];

	// get wlan traffic power consumption
	if(wlanState == WLAN_LINK)
	{
			//get chipVersion
		stream = fopen ( "proc/wlan0/stats", "r" );
		if ( stream != NULL )
		{
			char *strtmp;
			char line[100];
			while (fgets(line, sizeof(line), stream))
			{
				unsigned char *p;
				strtmp = line;


				while(*strtmp == ' ')
					strtmp++;


				if(strstr(strtmp,"tx_avarage") != 0)
				{
					unsigned char str1[10];

					if(debug_check)
						fprintf(stderr,"\r\n [%s]",strtmp);

					//tx_avarage:    1449
					sscanf(strtmp, "%*[^:]:%s",str1);

					p = str1;
					while(*p == ' ')
						p++;

					tx_average = atoi(p);
					tx_average*=8; // bytes->bits

					if(debug_check)
						fprintf(stderr,"\r\n tx_average=[%u]",tx_average);
				}
				else if(strstr(strtmp,"rx_avarage") != 0)
				{
					unsigned char str1[10];

					if(debug_check)
						fprintf(stderr,"\r\n [%s]",strtmp);

					//rx_avarage:    1449
					sscanf(strtmp, "%*[^:]:%s",str1);

					p = str1;
					while(*p == ' ')
						p++;

					rx_average = atoi(p);
					rx_average*=8; // bytes->bits

					if(debug_check)
						fprintf(stderr,"\r\n rx_average=[%u]",rx_average);
					}
				else if(strstr(strtmp,"cur_tx_rate") != 0)
				{
					unsigned char str1[10];
					unsigned short OFDM_CCK = 0;

					if(debug_check)
						fprintf(stderr,"\r\n [%s]",strtmp);

					//cur_tx_rate:   MCS[8-15]
					//cur_tx_rate:   MCS[0-7]
					//cur_tx_rate:   [1,2,5,11]
					//cur_tx_rate:   [6,9,12,18,24,36,48,54]
					sscanf(strtmp, "%*[^:]:%s",str1);
					p = str1;
					while(*p == ' ')
						p++;

					if(debug_check)
						fprintf(stderr,"\r\n p=[%s]",p);

					if(strstr(p, "MCS8") != 0 || strstr(p, "MCS9") != 0 ||
						 strstr(p, "MCS10") != 0 || strstr(p, "MCS11") != 0 ||
						 strstr(p, "MCS12") != 0 || strstr(p, "MCS13") != 0 ||
						 strstr(p, "MCS14") != 0 || strstr(p, "MCS15") != 0 )
					{
						wlanTrafficState = WLAN_MCS8_15;
					}
					else if(strstr(p, "MCS0") != 0 || strstr(p, "MCS1") != 0 ||
									 strstr(p, "MCS2") != 0 || strstr(p, "MCS3") != 0 ||
									 strstr(p, "MCS4") != 0 || strstr(p, "MCS5") != 0 ||
									 strstr(p, "MCS6") != 0 || strstr(p, "MCS7") != 0 )
					{
						wlanTrafficState = WLAN_MCS0_7;
					}
					else
					{
						OFDM_CCK = atoi(p);

						if(OFDM_CCK == 1 || OFDM_CCK == 2 || OFDM_CCK == 5 || OFDM_CCK ==11)
						{
							wlanTrafficState = WLAN_CCK;
						}
						else if(OFDM_CCK == 6 || OFDM_CCK == 9 || OFDM_CCK == 12 || OFDM_CCK == 18 ||
							      OFDM_CCK == 24 || OFDM_CCK == 36 || OFDM_CCK == 48 || OFDM_CCK == 54 )
						{
							wlanTrafficState = WLAN_OFDM;
						}
					}

					if(strstr(p, "MCS15") != 0)
						wlanTrafficZ = wlanTrafficStatePwrConZ[chipVersion][0];
					else if(strstr(p, "MCS14") != 0)
						wlanTrafficZ = wlanTrafficStatePwrConZ[chipVersion][1];
					else if(strstr(p, "MCS13") != 0)
						wlanTrafficZ = wlanTrafficStatePwrConZ[chipVersion][2];
					else if(strstr(p, "MCS12") != 0)
						wlanTrafficZ = wlanTrafficStatePwrConZ[chipVersion][3];
					else if(strstr(p, "MCS11") != 0)
						wlanTrafficZ = wlanTrafficStatePwrConZ[chipVersion][4];
					else if(strstr(p, "MCS10") != 0)
						wlanTrafficZ = wlanTrafficStatePwrConZ[chipVersion][5];
					else if(strstr(p, "MCS9") != 0)
						wlanTrafficZ = wlanTrafficStatePwrConZ[chipVersion][6];
					else if(strstr(p, "MCS8") != 0)
						wlanTrafficZ = wlanTrafficStatePwrConZ[chipVersion][7];
					else if(strstr(p, "MCS7") != 0)
						wlanTrafficZ = wlanTrafficStatePwrConZ[chipVersion][8];
					else if(strstr(p, "MCS6") != 0)
						wlanTrafficZ = wlanTrafficStatePwrConZ[chipVersion][9];
					else if(strstr(p, "MCS5") != 0)
						wlanTrafficZ = wlanTrafficStatePwrConZ[chipVersion][10];
					else if(strstr(p, "MCS4") != 0)
						wlanTrafficZ = wlanTrafficStatePwrConZ[chipVersion][11];
					else if(strstr(p, "MCS3") != 0)
						wlanTrafficZ = wlanTrafficStatePwrConZ[chipVersion][12];
					else if(strstr(p, "MCS2") != 0)
						wlanTrafficZ = wlanTrafficStatePwrConZ[chipVersion][13];
					else if(strstr(p, "MCS1") != 0)
						wlanTrafficZ = wlanTrafficStatePwrConZ[chipVersion][14];
					else if(strstr(p, "MCS0") != 0)
						wlanTrafficZ = wlanTrafficStatePwrConZ[chipVersion][15];
					else if(OFDM_CCK == 54)
						wlanTrafficZ = wlanTrafficStatePwrConZ[chipVersion][16];
					else if(OFDM_CCK == 48)
						wlanTrafficZ = wlanTrafficStatePwrConZ[chipVersion][17];
					else if(OFDM_CCK == 36)
						wlanTrafficZ = wlanTrafficStatePwrConZ[chipVersion][18];
					else if(OFDM_CCK == 24)
						wlanTrafficZ = wlanTrafficStatePwrConZ[chipVersion][19];
					else if(OFDM_CCK == 18)
						wlanTrafficZ = wlanTrafficStatePwrConZ[chipVersion][20];
					else if(OFDM_CCK == 12)
						wlanTrafficZ = wlanTrafficStatePwrConZ[chipVersion][21];
					else if(OFDM_CCK == 9)
						wlanTrafficZ = wlanTrafficStatePwrConZ[chipVersion][22];
					else if(OFDM_CCK == 6)
						wlanTrafficZ = wlanTrafficStatePwrConZ[chipVersion][23];
					else if(OFDM_CCK == 11)
						wlanTrafficZ = wlanTrafficStatePwrConZ[chipVersion][24];
					else if(OFDM_CCK == 5)
						wlanTrafficZ = wlanTrafficStatePwrConZ[chipVersion][25];
					else if(OFDM_CCK == 2)
						wlanTrafficZ = wlanTrafficStatePwrConZ[chipVersion][26];
					else if(OFDM_CCK == 1)
						wlanTrafficZ = wlanTrafficStatePwrConZ[chipVersion][27];

				}

			}
			fclose(stream );

		}
	}

	if(debug_check)
		fprintf(stderr,"\r\n wlanTrafficState=[%u], wlanTrafficZ=[%u]",wlanTrafficState, wlanTrafficZ);

	switch(wlanTrafficState)
	{
		case WLAN_MCS8_15:
			//tx_average /= 1000000;
			if(tx_average > 95000000)
				tx_average = 95000000;

			wlanTrafficStatePwrCon_Total = ((((float)tx_average*wlanTrafficStatePwrCon[chipVersion][wlanTrafficState]*wlanTrafficZ)/1000)/100)/1000000;
			if(debug_check)
				fprintf(stderr,"\r\n wlanTrafficStatePwrCon_Total:(((%u*%u*%u)/1000)/100)/10^6 = [%u]",tx_average,wlanTrafficStatePwrCon[chipVersion][wlanTrafficState],wlanTrafficZ,wlanTrafficStatePwrCon_Total);

			totalPwrCon+=wlanTrafficStatePwrCon_Total;
			break;
		case WLAN_MCS0_7:
			//tx_average /= 1000000;
			if(tx_average > 90000000)
				tx_average = 90000000;

			wlanTrafficStatePwrCon_Total = ((((float)tx_average*wlanTrafficStatePwrCon[chipVersion][wlanTrafficState]*wlanTrafficZ)/1000)/100)/1000000;
			if(debug_check)
				fprintf(stderr,"\r\n wlanTrafficStatePwrCon_Total:(((%u*%u*%u)/1000)/100)/10^6 = [%u]",tx_average,wlanTrafficStatePwrCon[chipVersion][wlanTrafficState],wlanTrafficZ,wlanTrafficStatePwrCon_Total);

			totalPwrCon+=wlanTrafficStatePwrCon_Total;
			break;
		case WLAN_OFDM:
			//tx_average /= 1000000;
			if(tx_average > 25000000)
				tx_average = 25000000;

			wlanTrafficStatePwrCon_Total = ((((float)tx_average*wlanTrafficStatePwrCon[chipVersion][wlanTrafficState]*wlanTrafficZ)/1000)/100)/1000000;
			if(debug_check)
				fprintf(stderr,"\r\n wlanTrafficStatePwrCon_Total:(((%u*%u*%u)/1000)/100)/10^6 = [%u]",tx_average,wlanTrafficStatePwrCon[chipVersion][wlanTrafficState],wlanTrafficZ,wlanTrafficStatePwrCon_Total);

			totalPwrCon+=wlanTrafficStatePwrCon_Total;
			break;
		case WLAN_CCK:

		wlanTrafficStatePwrCon_Total = ((((float)tx_average*wlanTrafficStatePwrCon[chipVersion][wlanTrafficState]*wlanTrafficZ)/1000)/100)/1000000;
			if(debug_check)
				fprintf(stderr,"\r\n wlanTrafficStatePwrCon_Total:(((%u*%u*%u)/1000)/100)/10^6 = [%u]",tx_average, wlanTrafficStatePwrCon[chipVersion][wlanTrafficState],wlanTrafficZ,wlanTrafficStatePwrCon_Total);
			totalPwrCon+=wlanTrafficStatePwrCon_Total;
			break;
	}

	//get CPU utilization
	stream = fopen ( "/proc/stat", "r" );
	if ( stream != NULL )
	{
		char buf[512];
		unsigned int d1, d2, d3, d4;

		fgets(buf, sizeof(buf), stream);	/* eat line */


		sscanf(buf, "cpu %d %d %d %d", &d1, &d2, &d3, &d4);
		fclose(stream);

		if(pre_cpu_d4 == 0)
		{
			pre_cpu_d4 = d4;
		}
		else
		{

			unsigned int delta = 0;

			delta = (d4 - pre_cpu_d4)/time_delta;

			pre_cpu_d4 = d4;
			if(delta > max_cpu_delta)
				max_cpu_delta = delta;

			cpu_utilization = 100 - (int)(delta*100/max_cpu_delta);

			if(debug_check)
				fprintf(stderr,"\r\n cpu_busy: (%u*%u)/100=[%u] ",cpu_utilization,cpuUtilizationPwrCon[chipVersion],((cpu_utilization*cpuUtilizationPwrCon[chipVersion])/100));

	}

	}

	if(cpuMode == CPU_NORMAL)
		totalPwrCon+=((cpu_utilization*cpuUtilizationPwrCon[chipVersion])/100);


	if(1 || strcmp(askfor,"all")==0){


		if(debug_check)
		fprintf(stderr,"\r\n totalPwrCon=%u",totalPwrCon);

		if(tx_average_multiply2)
			tx_average/=2;

		websWrite(wp, "<interface><name>LAN</name><type>LAN</type><totalPwrCon>%d</totalPwrCon><wlanTx>%d</wlanTx><wlanRx>%d</wlanRx></interface>",totalPwrCon,tx_average,rx_average);

	}

	return 0;

}
#endif // #if defined(POWER_CONSUMPTION_SUPPORT)
