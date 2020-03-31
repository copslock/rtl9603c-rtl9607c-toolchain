/*
 *      Web server handler routines for get info and index (getinfo(), getindex())
 *
 *      Authors: David Hsu	<davidhsu@realtek.com.tw>
 *
 *      $Id: fmget.c,v 1.83.2.5 2011/05/09 10:42:49 keith_huang Exp $
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/sysinfo.h>

#include "../webs.h"
#include "apmib.h"
#include "apform.h"
#include "utility.h"

#define FW_VERSION	fwVersion

#ifdef CONFIG_RTL_WAPI_SUPPORT
#define CA_CERT "/var/myca/CA.cert"
#define AS_CER "/web/as.cer"
#define WAPI_CERT_CHANGED		T("/tmp/certSatusChanged")
#endif

extern char *fwVersion;	// defined in version.c
#ifdef HOME_GATEWAY
#ifdef VPN_SUPPORT
extern int getIpsecInfo(IPSECTUNNEL_T *entry);
#endif
#endif

#if defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT)
unsigned char WaitCountTime=1;
#endif

#ifdef REBOOT_CHECK
char okMsg[300]={0};
char lastUrl[100]={0};
int countDownTime = 40;
int needReboot = 0;
int run_init_script_flag = 0;
#endif

// added by rock /////////////////////////////////////////
#include <regex.h>
#ifdef VOIP_SUPPORT 
#include "web_voip.h"
#endif

/////////////////////////////////////////////////////////////////////////////
void translate_control_code(char *buffer)
{
	char tmpBuf[200], *p1 = buffer, *p2 = tmpBuf;


	while (*p1) {
		if (*p1 == '"') {
			memcpy(p2, "&quot;", 6);
			p2 += 6;
		}
		else if (*p1 == '\x27') {
			memcpy(p2, "&#39;", 5);
			p2 += 5;
		}
		else if (*p1 == '\x5c') {
			memcpy(p2, "&#92;", 5);
			p2 += 5;
		}
		else
			*p2++ = *p1;
		p1++;
	}
	*p2 = '\0';

	strcpy(buffer, tmpBuf);
}

#ifdef WIFI_SIMPLE_CONFIG
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
#endif

/////////////////////////////////////////////////////////////////////////////
int getInfo(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t	*name;
	struct in_addr	intaddr;
	struct sockaddr hwaddr;
	unsigned char *pMacAddr;
	unsigned long sec, mn, hr, day;
	unsigned char buffer[500];
	int i,intVal;
 	struct user_net_device_stats stats;
	DHCP_T dhcp;
	bss_info bss;
	struct tm * tm_time;
	time_t current_secs;
#ifdef HOME_GATEWAY
	char_t *iface=NULL;
	OPMODE_T opmode=-1;
#endif
#ifdef HOME_GATEWAY
#ifdef VPN_SUPPORT
	IPSECTUNNEL_T entry ;
#endif
#endif
   	if (ejArgs(argc, argv, T("%s"), &name) < 1) {
   		websError(wp, 400, T("Insufficient args\n"));
   		return -1;
   	}

   	if ( !strcmp(name, T("name")) ) {
		buffer[0]='\0';
		if ( !apmib_get(MIB_DEVICE_NAME,  (void *)buffer) )
			return -1;
		return websWrite(wp, T("%s"), buffer);
	}
	else if ( !strcmp(name, T("uptime"))) {
		struct sysinfo info ;

		sysinfo(&info);
		sec = (unsigned long) info.uptime ;
		day = sec / 86400;
		//day -= 10957; // day counted from 1970-2000

		sec %= 86400;
		hr = sec / 3600;
		sec %= 3600;
		mn = sec / 60;
		sec %= 60;

		return websWrite(wp, T("%dday:%dh:%dm:%ds"),
							day, hr, mn, sec);
	}
	else if ( !strcmp(name, T("year"))) {

		time(&current_secs);
		tm_time = localtime(&current_secs);
		#if 0
		sprintf(buffer , "%2d/%2d/%d %2d:%2d:%2d %s",
				(tm_time->tm_mon),
				(tm_time->tm_mday), (tm_time->tm_year+ 1900),
				(tm_time->tm_hour),
				(tm_time->tm_min),(tm_time->tm_sec)
				, _tzname[tm_time->tm_isdst]);
		#endif
		sprintf(buffer,"%d", (tm_time->tm_year+ 1900));

		return websWrite(wp, T("%s"), buffer);

	}
	else if ( !strcmp(name, T("month"))) {
		time(&current_secs);
		tm_time = localtime(&current_secs);
		sprintf(buffer,"%d", (tm_time->tm_mon+1));
		return websWrite(wp, T("%s"), buffer);
	}
	else if ( !strcmp(name, T("day"))) {
		time(&current_secs);
		tm_time = localtime(&current_secs);
		sprintf(buffer,"%d", (tm_time->tm_mday));
		return websWrite(wp, T("%s"), buffer);
	}
	else if ( !strcmp(name, T("hour"))) {
		time(&current_secs);
		tm_time = localtime(&current_secs);
		sprintf(buffer,"%d", (tm_time->tm_hour));
		return websWrite(wp, T("%s"), buffer);
	}
	else if ( !strcmp(name, T("minute"))) {
		time(&current_secs);
		tm_time = localtime(&current_secs);
		sprintf(buffer,"%d", (tm_time->tm_min));
		return websWrite(wp, T("%s"), buffer);
	}
	else if ( !strcmp(name, T("second"))) {
		time(&current_secs);
		tm_time = localtime(&current_secs);
		sprintf(buffer,"%d", (tm_time->tm_sec));
		return websWrite(wp, T("%s"), buffer);
	}
   	else if ( !strcmp(name, T("clientnum"))) {
		apmib_get( MIB_WLAN_DISABLED, (void *)&intVal);

		if (intVal == 1)	// disable
			intVal = 0;
		else {
			if ( getWlStaNum(WLAN_IF, &intVal) < 0)
			intVal = 0;
		}
		sprintf(buffer, "%d", intVal );
		return websWrite(wp, buffer);
	}
   	else if ( !strcmp(name, T("ssid"))) {
		if ( !apmib_get( MIB_WLAN_SSID,  (void *)buffer) )
			return -1;

		translate_control_code(buffer);

		return websWrite(wp, T("%s"), buffer);
	}
   	else if ( !strcmp(name, T("channel"))) {
		if ( !apmib_get( MIB_WLAN_CHAN_NUM,  (void *)&intVal) )
			return -1;
		sprintf(buffer, "%d", intVal );
   		return websWrite(wp, buffer);
	}
        else if ( !strcmp(name, T("wep"))) {
                ENCRYPT_T encrypt;
                if ( !apmib_get( MIB_WLAN_ENCRYPT,  (void *)&encrypt) )
                        return -1;
                if (encrypt == ENCRYPT_DISABLED)
                        strcpy( buffer, T("Disabled") );
                else if (encrypt == ENCRYPT_WPA)
                        strcpy( buffer, T("WPA") );
		else if (encrypt == ENCRYPT_WPA2)
                        strcpy( buffer, T("WPA2") );
		else if (encrypt == (ENCRYPT_WPA | ENCRYPT_WPA2))
                        strcpy( buffer, T("WPA2 Mixed") );
		else if (encrypt == ENCRYPT_WAPI)
				strcpy(buffer,T("WAPI"));
                else {
                        WEP_T wep;
                        if ( !apmib_get( MIB_WLAN_WEP,  (void *)&wep) )
                                return -1;
                        if ( wep == WEP_DISABLED )
                                strcpy( buffer, T("Disabled") );
                        else if ( wep == WEP64 )
                                strcpy( buffer, T("WEP 64bits") );
                        else if ( wep == WEP128)
                                strcpy( buffer, T("WEP 128bits") );
                }
                return websWrite(wp, buffer);
        }
   	else if ( !strcmp(name, T("wdsEncrypt"))) {
   		WDS_ENCRYPT_T encrypt;
		if ( !apmib_get( MIB_WLAN_WDS_ENCRYPT,  (void *)&encrypt) )
			return -1;
		if ( encrypt == WDS_ENCRYPT_DISABLED)
			strcpy( buffer, T("Disabled") );
		else if ( encrypt == WDS_ENCRYPT_WEP64)
			strcpy( buffer, T("WEP 64bits") );
		else if ( encrypt == WDS_ENCRYPT_WEP128)
			strcpy( buffer, T("WEP 128bits") );
		else if ( encrypt == WDS_ENCRYPT_TKIP)
			strcpy( buffer, T("TKIP") );
		else if ( encrypt == WDS_ENCRYPT_AES)
			strcpy( buffer, T("AES") );
		else
			buffer[0] = '\0';
   		return websWrite(wp, buffer);
   	}
#ifdef CONFIG_RTK_MESH
   	else if ( !strcmp(name, T("meshEncrypt"))) {
   		ENCRYPT_T encrypt;
		if ( !apmib_get( MIB_MESH_ENCRYPT,  (void *)&encrypt) )
			return -1;
		if ( encrypt == ENCRYPT_DISABLED)
			strcpy( buffer, T("Disabled") );
		else if ( encrypt == ENCRYPT_WPA2)
			strcpy( buffer, T("WPA2") );
		else
			buffer[0] = '\0';
   		return websWrite(wp, buffer);
   	}	   	
#endif
  	else if ( !strcmp(name, T("ip"))) {
		if ( getInAddr(BRIDGE_IF, IP_ADDR, (void *)&intaddr ) )
			return websWrite(wp, T("%s"), inet_ntoa(intaddr) );
		else
			return websWrite(wp, T("0.0.0.0"));
	}
   	else if ( !strcmp(name, T("mask"))) {
		if ( getInAddr(BRIDGE_IF, SUBNET_MASK, (void *)&intaddr ))
			return websWrite(wp, T("%s"), inet_ntoa(intaddr) );
		else
			return websWrite(wp, T("0.0.0.0"));
	}
   	else if ( !strcmp(name, T("gateway"))) {
		DHCP_T dhcp;
  		apmib_get( MIB_DHCP, (void *)&dhcp);
		if ( dhcp == DHCP_SERVER ) {
		// if DHCP server, default gateway is set to LAN IP
			if ( getInAddr(BRIDGE_IF, IP_ADDR, (void *)&intaddr ) )
				return websWrite(wp, T("%s"), inet_ntoa(intaddr) );
			else
				return websWrite(wp, T("0.0.0.0"));
		}
		else
		if ( getDefaultRoute(BRIDGE_IF, &intaddr) )
			return websWrite(wp, T("%s"), inet_ntoa(intaddr) );
		else
			return websWrite(wp, T("0.0.0.0"));
	}
	else if ( !strcmp(name, T("ip-rom"))) {
		if ( !apmib_get( MIB_IP_ADDR,  (void *)buffer) )
			return -1;
   		return websWrite(wp, T("%s"), inet_ntoa(*((struct in_addr *)buffer)) );
	}
   	else if ( !strcmp(name, T("mask-rom"))) {
		if ( !apmib_get( MIB_SUBNET_MASK,  (void *)buffer) )
			return -1;
   		return websWrite(wp, T("%s"), inet_ntoa(*((struct in_addr *)buffer)) );
	}
   	else if ( !strcmp(name, T("gateway-rom"))) {
		if ( !apmib_get( MIB_DEFAULT_GATEWAY,  (void *)buffer) )
			return -1;
		if (!memcmp(buffer, "\x0\x0\x0\x0", 4))
			return websWrite(wp, T("0.0.0.0"));
   		return websWrite(wp, T("%s"), inet_ntoa(*((struct in_addr *)buffer)) );
	}
	else if ( !strcmp(name, T("static_dhcp_onoff"))) {
		if ( !apmib_get(MIB_DHCPRSVDIP_ENABLED, (void *)&intVal) )
			return -1;
		sprintf(buffer, "%d", intVal);
		return websWrite(wp, T("%s"), buffer);
	}
 	else if ( !strcmp(name, T("dhcp-current")) ) {
   		if ( !apmib_get( MIB_DHCP, (void *)&dhcp) )
			return -1;

		if (dhcp==DHCP_CLIENT) {
			if (!isDhcpClientExist(BRIDGE_IF) &&
					!getInAddr(BRIDGE_IF, IP_ADDR, (void *)&intaddr))
				return websWrite(wp, T("Getting IP from DHCP server..."));
			if (isDhcpClientExist(BRIDGE_IF))
				return websWrite(wp, T("DHCP"));
		}
		return websWrite(wp, T("Fixed IP"));
	}
   	else if ( !strcmp(name, T("dhcpRangeStart"))) {
		if ( !apmib_get( MIB_DHCP_CLIENT_START,  (void *)buffer) )
			return -1;
   		return websWrite(wp, T("%s"), inet_ntoa(*((struct in_addr *)buffer)) );
	}
  	else if ( !strcmp(name, T("dhcpRangeEnd"))) {
		if ( !apmib_get( MIB_DHCP_CLIENT_END,  (void *)buffer) )
			return -1;
   		return websWrite(wp, T("%s"), inet_ntoa(*((struct in_addr *)buffer)) );
	}
 	else if ( !strcmp(name, T("wan-dns1"))) {
		if ( !apmib_get( MIB_WAN_DNS1,  (void *)buffer) )
			return -1;
		if (!memcmp(buffer, "\x0\x0\x0\x0", 4))
			return websWrite(wp, T(""));
   		return websWrite(wp, T("%s"), inet_ntoa(*((struct in_addr *)buffer)) );
	}
  	else if ( !strcmp(name, T("wan-dns2"))) {
		if ( !apmib_get( MIB_WAN_DNS2,  (void *)buffer) )
			return -1;
		if (!memcmp(buffer, "\x0\x0\x0\x0", 4))
			return websWrite(wp, T(""));
   		return websWrite(wp, T("%s"), inet_ntoa(*((struct in_addr *)buffer)) );
	}
  	else if ( !strcmp(name, T("wan-dns3"))) {
		if ( !apmib_get( MIB_WAN_DNS3,  (void *)buffer) )
			return -1;
		if (!memcmp(buffer, "\x0\x0\x0\x0", 4))
			return websWrite(wp, T(""));
   		return websWrite(wp, T("%s"), inet_ntoa(*((struct in_addr *)buffer)) );
	}
#ifdef  HOME_GATEWAY
	else if ( !strcmp(name, T("ntpServerIp1"))) { // sc_yang
		if ( !apmib_get( MIB_NTP_SERVER_IP1,  (void *)buffer) )
			return -1;
		if (!memcmp(buffer, "\x0\x0\x0\x0", 4))
			return websWrite(wp, T(""));

		return websWrite(wp, T("%s"), inet_ntoa(*((struct in_addr *)buffer)) );
	}
	else if ( !strcmp(name, T("ntpServerIp2"))) { // sc_yang
		if ( !apmib_get( MIB_NTP_SERVER_IP2,  (void *)buffer) )
			return -1;
		if (!memcmp(buffer, "\x0\x0\x0\x0", 4))
			return websWrite(wp, T(""));
		return websWrite(wp, T("%s"), inet_ntoa(*((struct in_addr *)buffer)) );
	}
	else if ( !strcmp(name, T("ntpTimeZone"))) { // sc_yang
		if ( !apmib_get( MIB_NTP_TIMEZONE,  (void *)buffer) )
			return -1;
   		return websWrite(wp, T("%s"), buffer);
	}

  	else if ( !strcmp(name, T("wan-ip"))) 
  	{
#if 1
			char strWanIP[16];
			char strWanMask[16];
			char strWanDefIP[16];
			char strWanHWAddr[18];
			getWanInfo(strWanIP,strWanMask,strWanDefIP,strWanHWAddr);
			
			return websWrite(wp, T("%s"), strWanIP);
#else  		
  		if ( !apmib_get( MIB_WAN_DHCP, (void *)&dhcp) )
			return -1;
  		if ( !apmib_get( MIB_OP_MODE, (void *)&opmode) )
			return -1;
		if ( dhcp == PPPOE || dhcp == PPTP || dhcp ==L2TP) { /* # keith: add l2tp support. 20080515 */
			iface = PPPOE_IF;
			if ( !isConnectPPP() )
				iface = NULL;
		}
		else if (opmode == WISP_MODE)
			iface = WLAN_IF;
		else
			iface = WAN_IF;
		if(opmode != WISP_MODE){
			if(iface){
				if(getWanLink("eth1") < 0){
					return websWrite(wp, T("0.0.0.0"));
				}
			}	
		}
		if ( iface && getInAddr(iface, IP_ADDR, (void *)&intaddr ) )
			return websWrite(wp, T("%s"), inet_ntoa(intaddr) );
		else
			return websWrite(wp, T("0.0.0.0"));
#endif			
	}
   	else if ( !strcmp(name, T("wan-mask"))) {
#if 1
			char strWanIP[16];
			char strWanMask[16];
			char strWanDefIP[16];
			char strWanHWAddr[18];
			getWanInfo(strWanIP,strWanMask,strWanDefIP,strWanHWAddr);
			
			return websWrite(wp, T("%s"), strWanMask);
#else   		
		if ( !apmib_get( MIB_WAN_DHCP, (void *)&dhcp) )
			return -1;
  		if ( !apmib_get( MIB_OP_MODE, (void *)&opmode) )
			return -1;
		if ( dhcp == PPPOE || dhcp == PPTP || dhcp ==L2TP) { /* # keith: add l2tp support. 20080515 */
			iface = PPPOE_IF;
			if ( !isConnectPPP() )
				iface = NULL;
		}
		else if (opmode == WISP_MODE)
			iface = WLAN_IF;
		else
			iface = WAN_IF;
		if(opmode != WISP_MODE){
			if(iface){
				if(getWanLink("eth1") < 0){
					return websWrite(wp, T("0.0.0.0"));
				}
			}	
		}
		if ( iface && getInAddr(iface, SUBNET_MASK, (void *)&intaddr ))
			return websWrite(wp, T("%s"), inet_ntoa(intaddr) );
		else
			return websWrite(wp, T("0.0.0.0"));
#endif			
	}
   	else if ( !strcmp(name, T("wan-gateway"))) {
#if 1
			char strWanIP[16];
			char strWanMask[16];
			char strWanDefIP[16];
			char strWanHWAddr[18];
			getWanInfo(strWanIP,strWanMask,strWanDefIP,strWanHWAddr);
			
			return websWrite(wp, T("%s"), strWanDefIP);
#else   		
		if ( !apmib_get( MIB_WAN_DHCP, (void *)&dhcp) )
			return -1;
  		if ( !apmib_get( MIB_OP_MODE, (void *)&opmode) )
			return -1;
		if ( dhcp == PPPOE || dhcp == PPTP || dhcp ==L2TP) { /* # keith: add l2tp support. 20080515 */
			iface = PPPOE_IF;
			if ( !isConnectPPP() )
				iface = NULL;
		}
		else if (opmode == WISP_MODE)
			iface = WLAN_IF;
		else
			iface = WAN_IF;
		if(opmode != WISP_MODE){
			if(iface){
				if(getWanLink("eth1") < 0){
					return websWrite(wp, T("0.0.0.0"));
				}
			}	
		}
		if ( iface && getDefaultRoute(iface, &intaddr) )
			return websWrite(wp, T("%s"), inet_ntoa(intaddr) );
		else
			return websWrite(wp, T("0.0.0.0"));
#endif			
	}
	else if ( !strcmp(name, T("wan-hwaddr"))) {
#if 1
		char strWanIP[16];
		char strWanMask[16];
		char strWanDefIP[16];
		char strWanHWAddr[18];
		getWanInfo(strWanIP,strWanMask,strWanDefIP,strWanHWAddr);

		return websWrite(wp, T("%s"), strWanHWAddr);
#else		
  		if ( !apmib_get( MIB_OP_MODE, (void *)&opmode) )
			return -1;
		if(opmode == WISP_MODE)
			iface = WLAN_IF;
		else
			iface = WAN_IF;
		if ( getInAddr(iface, HW_ADDR, (void *)&hwaddr ) ) {
			pMacAddr = hwaddr.sa_data;
			return websWrite(wp, T("%02x:%02x:%02x:%02x:%02x:%02x"), pMacAddr[0], pMacAddr[1],
				pMacAddr[2], pMacAddr[3], pMacAddr[4], pMacAddr[5]);
		}
		else
			return websWrite(wp, T("00:00:00:00:00:00"));
#endif			
	}
	else if ( !strcmp(name, T("wan-ip-rom"))) {
		if ( !apmib_get( MIB_WAN_IP_ADDR,  (void *)buffer) )
			return -1;
   		return websWrite(wp, T("%s"), inet_ntoa(*((struct in_addr *)buffer)) );
	}
   	else if ( !strcmp(name, T("wan-mask-rom"))) {
		if ( !apmib_get( MIB_WAN_SUBNET_MASK,  (void *)buffer) )
			return -1;
   		return websWrite(wp, T("%s"), inet_ntoa(*((struct in_addr *)buffer)) );
	}
   	else if ( !strcmp(name, T("wan-gateway-rom"))) {
		if ( !apmib_get( MIB_WAN_DEFAULT_GATEWAY,  (void *)buffer) )
			return -1;
		if (!memcmp(buffer, "\x0\x0\x0\x0", 4))
			return websWrite(wp, T("0.0.0.0"));
   		return websWrite(wp, T("%s"), inet_ntoa(*((struct in_addr *)buffer)) );
	}
    	else if ( !strcmp(name, T("wan-ppp-idle"))) {
		if ( !apmib_get( MIB_PPP_IDLE_TIME,  (void *)&intVal) )
			return -1;

		sprintf(buffer, "%d", intVal/60 );
   		return websWrite(wp, buffer);
	}
	else if ( !strcmp(name, T("wan-pptp-idle"))) {
		if ( !apmib_get( MIB_PPTP_IDLE_TIME,  (void *)&intVal) )
			return -1;

		sprintf(buffer, "%d", intVal/60 );
   		return websWrite(wp, buffer);
	}
		else if ( !strcmp(name, T("wan-l2tp-idle"))) {
		if ( !apmib_get( MIB_L2TP_IDLE_TIME,  (void *)&intVal) )
			return -1;

		sprintf(buffer, "%d", intVal/60 );
   		return websWrite(wp, buffer);
	}
 	else if ( !strcmp(name, T("wanDhcp-current")) ) {
 		int isWanPhy_Link=0;
 		if ( !apmib_get( MIB_OP_MODE, (void *)&opmode) )
			return -1;
		if(opmode != WISP_MODE){	
 			isWanPhy_Link=getWanLink("eth1"); 
 		}
 		if ( !apmib_get( MIB_WAN_DHCP, (void *)&dhcp) )
			return -1;
		if ( dhcp == DHCP_CLIENT) {
			if(opmode == WISP_MODE)
				iface = WLAN_IF;
			else
				iface = WAN_IF;
		 	if (!isDhcpClientExist(iface))
				return websWrite(wp, T("Getting IP from DHCP server..."));
			else{
				if(isWanPhy_Link < 0)
					return websWrite(wp, T("Getting IP from DHCP server..."));
				else
					return websWrite(wp, T("DHCP"));
			}
		}
		else if ( dhcp == DHCP_DISABLED ){
			if(isWanPhy_Link < 0)
				return websWrite(wp, T("Fixed IP Disconnected"));
			else
				return websWrite(wp, T("Fixed IP Connected"));
		}
		else if ( dhcp ==  PPPOE ) {
			if ( isConnectPPP()){
				if(isWanPhy_Link < 0)
					return websWrite(wp, T("PPPoE Disconnected"));
				else
					return websWrite(wp, T("PPPoE Connected"));
			}else
				return websWrite(wp, T("PPPoE Disconnected"));
		}
		else if ( dhcp ==  PPTP ) {
			if ( isConnectPPP()){
				if(isWanPhy_Link < 0)
					return websWrite(wp, T("PPTP Disconnected"));
				else
					return websWrite(wp, T("PPTP Connected"));
			}else
				return websWrite(wp, T("PPTP Disconnected"));
		}
		else if ( dhcp ==  L2TP ) { /* # keith: add l2tp support. 20080515 */
			if ( isConnectPPP()){
				if(isWanPhy_Link < 0)
					return websWrite(wp, T("L2TP Disconnected"));
				else
					return websWrite(wp, T("L2TP Connected"));
			}else
				return websWrite(wp, T("L2TP Disconnected"));
		}
	}
   	else if ( !strcmp(name, T("pppUserName"))) {
		buffer[0]='\0';
		if ( !apmib_get( MIB_PPP_USER,  (void *)buffer) )
			return -1;
		translate_control_code(buffer);		
		return websWrite(wp, T("%s"), buffer);
	}
  	else if ( !strcmp(name, T("pppPassword"))) {
		buffer[0]='\0';
		if ( !apmib_get( MIB_PPP_PASSWORD,  (void *)buffer) )
			return -1;
		translate_control_code(buffer);		
		return websWrite(wp, T("%s"), buffer);
	}
	else if ( !strcmp(name, T("pppServiceName"))) {
		buffer[0]='\0';
		//if ( !apmib_get( MIB_PPP_SERVICE,  (void *)buffer) )
		//	return -1;
		apmib_get( MIB_PPP_SERVICE,  (void *)buffer);
		return websWrite(wp, T("%s"), buffer);
	}
 	else if ( !strcmp(name, T("dmzHost"))) {
		if ( !apmib_get( MIB_DMZ_HOST,  (void *)buffer) )
			return -1;
		if (!memcmp(buffer, "\x0\x0\x0\x0", 4))
			return websWrite(wp, T(""));
   		return websWrite(wp, T("%s"), inet_ntoa(*((struct in_addr *)buffer)) );
	}
	else if ( !strcmp(name, T("wanMac"))) {
		if ( !apmib_get(MIB_WAN_MAC_ADDR,  (void *)buffer) )
			return -1;
		return websWrite(wp, T("%02x%02x%02x%02x%02x%02x"), buffer[0], buffer[1],
						buffer[2], buffer[3], buffer[4], buffer[5]);
	}
	else if ( !strcmp(name, T("pppMtuSize"))) {
		if ( !apmib_get( MIB_PPP_MTU_SIZE, (void *)&intVal) )
			return -1;
		sprintf(buffer, "%d", intVal );
   		return websWrite(wp, buffer);
	}
	else if ( !strcmp(name, T("pptpIp"))) {
		if ( !apmib_get( MIB_PPTP_IP,  (void *)buffer) )
			return -1;
   		return websWrite(wp, T("%s"), inet_ntoa(*((struct in_addr *)buffer)) );
	}
	else if ( !strcmp(name, T("pptpSubnet"))) {
		if ( !apmib_get( MIB_PPTP_SUBNET_MASK,  (void *)buffer) )
			return -1;
   		return websWrite(wp, T("%s"), inet_ntoa(*((struct in_addr *)buffer)) );
	}
	else if ( !strcmp(name, T("pptpServerIp"))) {
		if ( !apmib_get( MIB_PPTP_SERVER_IP,  (void *)buffer) )
			return -1;
   		return websWrite(wp, T("%s"), inet_ntoa(*((struct in_addr *)buffer)) );
	}
	else if ( !strcmp(name, T("pptpMtuSize"))) {
		if ( !apmib_get( MIB_PPTP_MTU_SIZE, (void *)&intVal) )
			return -1;
		sprintf(buffer, "%d", intVal );
   		return websWrite(wp, buffer);
	}
  	else if ( !strcmp(name, T("pptpUserName"))) {
		buffer[0]='\0';
		if ( !apmib_get( MIB_PPTP_USER,  (void *)buffer) )
			return -1;
		translate_control_code(buffer);		
		return websWrite(wp, T("%s"), buffer);
	}
  	else if ( !strcmp(name, T("pptpPassword"))) {
		buffer[0]='\0';
		if ( !apmib_get( MIB_PPTP_PASSWORD,  (void *)buffer) )
			return -1;
		translate_control_code(buffer);		
		return websWrite(wp, T("%s"), buffer);
	}
	/* # keith: add l2tp support. 20080515 */
	else if ( !strcmp(name, T("l2tpIp"))) {
		if ( !apmib_get( MIB_L2TP_IP,  (void *)buffer) )
			return -1;
   		return websWrite(wp, T("%s"), inet_ntoa(*((struct in_addr *)buffer)) );
	}
	else if ( !strcmp(name, T("l2tpSubnet"))) {
		if ( !apmib_get( MIB_L2TP_SUBNET_MASK,  (void *)buffer) )
			return -1;
   		return websWrite(wp, T("%s"), inet_ntoa(*((struct in_addr *)buffer)) );
	}
	else if ( !strcmp(name, T("l2tpServerIp"))) {
		if ( !apmib_get( MIB_L2TP_SERVER_IP,  (void *)buffer) )
			return -1;
   		return websWrite(wp, T("%s"), inet_ntoa(*((struct in_addr *)buffer)) );
	}
	else if ( !strcmp(name, T("l2tpMtuSize"))) {
		if ( !apmib_get( MIB_L2TP_MTU_SIZE, (void *)&intVal) )
			return -1;
		sprintf(buffer, "%d", intVal );
   		return websWrite(wp, buffer);
	}
  	else if ( !strcmp(name, T("l2tpUserName"))) {
		buffer[0]='\0';
		if ( !apmib_get( MIB_L2TP_USER,  (void *)buffer) )
			return -1;
		translate_control_code(buffer);		
		return websWrite(wp, T("%s"), buffer);
	}
  	else if ( !strcmp(name, T("l2tpPassword"))) {
		buffer[0]='\0';
		if ( !apmib_get( MIB_L2TP_PASSWORD,  (void *)buffer) )
			return -1;
		translate_control_code(buffer);		
		return websWrite(wp, T("%s"), buffer);
	}
	else if ( !strcmp(name, T("fixedIpMtuSize"))) {
		if ( !apmib_get( MIB_FIXED_IP_MTU_SIZE, (void *)&intVal) )
			return -1;
		sprintf(buffer, "%d", intVal );
   		return websWrite(wp, buffer);
	}
	else if ( !strcmp(name, T("dhcpMtuSize"))) {
		if ( !apmib_get( MIB_DHCP_MTU_SIZE, (void *)&intVal) )
			return -1;
		sprintf(buffer, "%d", intVal );
   		return websWrite(wp, buffer);
	}
#endif
	else if ( !strcmp(name, T("hwaddr"))) {
		if ( getInAddr(BRIDGE_IF, HW_ADDR, (void *)&hwaddr ) ) {
			pMacAddr = hwaddr.sa_data;
			return websWrite(wp, T("%02x:%02x:%02x:%02x:%02x:%02x"), pMacAddr[0], pMacAddr[1],
				pMacAddr[2], pMacAddr[3], pMacAddr[4], pMacAddr[5]);
		}
		else
			return websWrite(wp, T("00:00:00:00:00:00"));
	}
	else if ( !strcmp(name, T("bridgeMac"))) {
		if ( !apmib_get(MIB_ELAN_MAC_ADDR,  (void *)buffer) )
			return -1;
		return websWrite(wp, T("%02x%02x%02x%02x%02x%02x"), buffer[0], buffer[1],
						buffer[2], buffer[3], buffer[4], buffer[5]);
	}

	/* Advance setting stuffs */
	else if ( !strcmp(name, T("fragThreshold"))) {
		if ( !apmib_get( MIB_WLAN_FRAG_THRESHOLD, (void *)&intVal) )
			return -1;
		sprintf(buffer, "%d", intVal );
   		return websWrite(wp, buffer);
	}
	else if ( !strcmp(name, T("rtsThreshold"))) {
		if ( !apmib_get( MIB_WLAN_RTS_THRESHOLD, (void *)&intVal) )
			return -1;
		sprintf(buffer, "%d", intVal );
   		return websWrite(wp, buffer);
	}
	else if ( !strcmp(name, T("beaconInterval"))) {
		if ( !apmib_get( MIB_WLAN_BEACON_INTERVAL, (void *)&intVal) )
			return -1;
		sprintf(buffer, "%d", intVal );
   		return websWrite(wp, buffer);
	}
	else if ( !strcmp(name, T("dtimPeriod"))) {
		if ( !apmib_get( MIB_WLAN_DTIM_PERIOD, (void *)&intVal) )
			return -1;
		sprintf(buffer, "%d", intVal );
   		return websWrite(wp, buffer);
	}
	else if ( !strcmp(name, T("fwVersion"))) {
		sprintf(buffer, "%s", FW_VERSION );
   		return websWrite(wp, buffer);
	}
	// added by rock /////////////////////////////////////////
	else if ( !strcmp(name, T("buildTime"))) {
		FILE *fp;
		regex_t re;
		regmatch_t match[2];
		int status;

		fp = fopen("/proc/version", "r");
		if (!fp) {
			error(E_L, E_LOG, T("Read /proc/version failed!\n"));
			return websWrite(wp, "Unknown");
	   	}
		else
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
   				return websWrite(wp, &buffer[match[1].rm_so]);
			}
		}
   		
		return websWrite(wp, "Unknown");
	}
	else if ( !strcmp(name, T("lanTxPacketNum"))) {
		if ( getStats(ELAN_IF, &stats) < 0)
			stats.tx_packets = 0;
		sprintf(buffer, "%d", (int)stats.tx_packets);
   		return websWrite(wp, buffer);
	}
	else if ( !strcmp(name, T("lanRxPacketNum"))) {
		if ( getStats(ELAN_IF, &stats) < 0)
			stats.rx_packets = 0;
		sprintf(buffer, "%d", (int)stats.rx_packets);
   		return websWrite(wp, buffer);
	}
	else if ( !strcmp(name, T("lan2TxPacketNum"))) {
#if defined(VLAN_CONFIG_SUPPORTED)
		if ( getStats(ELAN2_IF, &stats) < 0)
			stats.tx_packets = 0;
		sprintf(buffer, "%d", (int)stats.tx_packets);
#else
		sprintf(buffer, "%d", 0);
#endif
   		return websWrite(wp, buffer);
	}
	else if ( !strcmp(name, T("lan2RxPacketNum"))) {
#if defined(VLAN_CONFIG_SUPPORTED)
		if ( getStats(ELAN2_IF, &stats) < 0)
			stats.rx_packets = 0;
		sprintf(buffer, "%d", (int)stats.rx_packets);
#else
		sprintf(buffer, "%d", 0);
#endif
   		return websWrite(wp, buffer);
	}
	else if ( !strcmp(name, T("lan3TxPacketNum"))) {
#if defined(VLAN_CONFIG_SUPPORTED)
		if ( getStats(ELAN3_IF, &stats) < 0)
			stats.tx_packets = 0;
		sprintf(buffer, "%d", (int)stats.tx_packets);
#else
		sprintf(buffer, "%d", 0);
#endif
   		return websWrite(wp, buffer);
	}
	else if ( !strcmp(name, T("lan3RxPacketNum"))) {
#if defined(VLAN_CONFIG_SUPPORTED)
		if ( getStats(ELAN3_IF, &stats) < 0)
			stats.rx_packets = 0;
		sprintf(buffer, "%d", (int)stats.rx_packets);
#else
		sprintf(buffer, "%d", 0);
#endif
   		return websWrite(wp, buffer);
	}
	else if ( !strcmp(name, T("lan4TxPacketNum"))) {
#if defined(VLAN_CONFIG_SUPPORTED)
		if ( getStats(ELAN4_IF, &stats) < 0)
			stats.tx_packets = 0;
		sprintf(buffer, "%d", (int)stats.tx_packets);
#else
		sprintf(buffer, "%d", 0);
#endif
   		return websWrite(wp, buffer);
	}
	else if ( !strcmp(name, T("lan4RxPacketNum"))) {
#if defined(VLAN_CONFIG_SUPPORTED)
		if ( getStats(ELAN4_IF, &stats) < 0)
			stats.rx_packets = 0;
		sprintf(buffer, "%d", (int)stats.rx_packets);
#else
		sprintf(buffer, "%d", 0);
#endif
   		return websWrite(wp, buffer);
	}
	else if ( !strcmp(name, T("wlanTxPacketNum"))) {
		if ( getStats(WLAN_IF, &stats) < 0)
			stats.tx_packets = 0;
		sprintf(buffer, "%d", (int)stats.tx_packets);
   		return websWrite(wp, buffer);

	}
	else if ( !strcmp(name, T("wlanRxPacketNum"))) {
		if ( getStats(WLAN_IF, &stats) < 0)
			stats.rx_packets = 0;
		sprintf(buffer, "%d", (int)stats.rx_packets);
   		return websWrite(wp, buffer);
	}
	else if ( !strcmp(name, T("bssid"))) {
		apmib_get( MIB_WLAN_DISABLED, (void *)&intVal);
		if ( intVal == 0 &&  getInAddr(WLAN_IF, HW_ADDR, (void *)&hwaddr ) ) {
			pMacAddr = hwaddr.sa_data;
			return websWrite(wp, T("%02x:%02x:%02x:%02x:%02x:%02x"), pMacAddr[0], pMacAddr[1],
				pMacAddr[2], pMacAddr[3], pMacAddr[4], pMacAddr[5]);
		}
		else
			return websWrite(wp, T("00:00:00:00:00:00"));
	}
	else if ( !strcmp(name, T("bssid_drv"))) {
		if ( getWlBssInfo(WLAN_IF, &bss) < 0)
			return -1;
		return websWrite(wp, T("%02x:%02x:%02x:%02x:%02x:%02x"), bss.bssid[0], bss.bssid[1],
				bss.bssid[2], bss.bssid[3], bss.bssid[4], bss.bssid[5]);
	}
	else if ( !strcmp(name, T("ssid_drv"))) {
		if ( getWlBssInfo(WLAN_IF, &bss) < 0)
			return -1;
		memcpy(buffer, bss.ssid, SSID_LEN+1);
		translate_control_code(buffer);
		return websWrite(wp, T("%s"), buffer);
	}
	else if ( !strcmp(name, T("state_drv"))) {
		char *pMsg;
		if ( getWlBssInfo(WLAN_IF, &bss) < 0)
			return -1;
		switch (bss.state) {
		case STATE_DISABLED:
			pMsg = T("Disabled");
			break;
		case STATE_IDLE:
			pMsg = T("Idle");
			break;
		case STATE_STARTED:
			pMsg = T("Started");
			break;
		case STATE_CONNECTED:
			pMsg = T("Connected");
			break;
		case STATE_WAITFORKEY:
			pMsg = T("Waiting for keys");
			break;
		case STATE_SCANNING:
			pMsg = T("Scanning");
			break;
		default:
			pMsg=NULL;
		}
		return websWrite(wp, T("%s"), pMsg);
	}
	else if ( !strcmp(name, T("channel_drv"))) {
		if ( getWlBssInfo(WLAN_IF, &bss) < 0)
			return -1;

		if (bss.channel)
			sprintf(buffer, "%d", bss.channel);
		else
			buffer[0] = '\0';

		return websWrite(wp, T("%s"), buffer);
	}

#ifdef HOME_GATEWAY
	else if ( !strcmp(name, T("wanTxPacketNum"))) {
  		if ( !apmib_get( MIB_OP_MODE, (void *)&opmode) )
			return -1;
		if(opmode == WISP_MODE)
			iface = WLAN_IF;
		else
			iface = WAN_IF;
		if ( getStats(iface, &stats) < 0)
			stats.tx_packets = 0;
		sprintf(buffer, "%d", (int)stats.tx_packets);
   		return websWrite(wp, buffer);
	}
	else if ( !strcmp(name, T("wanRxPacketNum"))) {
		if ( !apmib_get( MIB_OP_MODE, (void *)&opmode) )
			return -1;
		if(opmode == WISP_MODE)
			iface = WLAN_IF;
		else
			iface = WAN_IF;
		if ( getStats(iface, &stats) < 0)
			stats.rx_packets = 0;
		sprintf(buffer, "%d", (int)stats.rx_packets);
   		return websWrite(wp, buffer);
	}
#endif

	else if ( !strcmp(name, T("rsIp"))) {
		if ( !apmib_get( MIB_WLAN_RS_IP,  (void *)buffer) )
			return -1;
		if (!memcmp(buffer, "\x0\x0\x0\x0", 4))
			return websWrite(wp, T(""));
   		return websWrite(wp, T("%s"), inet_ntoa(*((struct in_addr *)buffer)) );
	}
	else if ( !strcmp(name, T("rsPort"))) {
		if ( !apmib_get( MIB_WLAN_RS_PORT, (void *)&intVal) )
			return -1;
		sprintf(buffer, "%d", intVal );
   		return websWrite(wp, buffer);
	}
 	else if ( !strcmp(name, T("rsPassword"))) {
		buffer[0]='\0';
		if ( !apmib_get( MIB_WLAN_RS_PASSWORD,  (void *)buffer) )
			return -1;
  		return websWrite(wp, T("%s"), buffer);
	}
	else if ( !strcmp(name, T("accountRsIp"))) {
		if ( !apmib_get( MIB_WLAN_ACCOUNT_RS_IP,  (void *)buffer) )
			return -1;
		if (!memcmp(buffer, "\x0\x0\x0\x0", 4))
			return websWrite(wp, T(""));
   		return websWrite(wp, T("%s"), inet_ntoa(*((struct in_addr *)buffer)) );
	}
	else if ( !strcmp(name, T("accountRsPort"))) {
		if ( !apmib_get( MIB_WLAN_ACCOUNT_RS_PORT, (void *)&intVal) )
			return -1;
		sprintf(buffer, "%d", intVal );
   		return websWrite(wp, buffer);
	}
	else if ( !strcmp(name, T("accountRsPassword"))) {
		buffer[0]='\0';
		if ( !apmib_get( MIB_WLAN_ACCOUNT_RS_PASSWORD,  (void *)buffer) )
			return -1;
		return websWrite(wp, T("%s"), buffer);
	}
	else if ( !strcmp(name, T("groupRekeyTime"))) {
		if ( !apmib_get( MIB_WLAN_WPA_GROUP_REKEY_TIME, (void *)&intVal) )
			return -1;
		sprintf(buffer, "%d", intVal );
   		return websWrite(wp, buffer);
	}
	else if ( !strcmp(name, T("groupRekeyTimeDay"))) {
		if ( !apmib_get( MIB_WLAN_WPA_GROUP_REKEY_TIME, (void *)&intVal) )
			return -1;
		sprintf(buffer, "%d", intVal/86400 );
   		return websWrite(wp, buffer);
	}
	else if ( !strcmp(name, T("groupRekeyTimeHr"))) {
		if ( !apmib_get( MIB_WLAN_WPA_GROUP_REKEY_TIME, (void *)&intVal) )
			return -1;
		sprintf(buffer, "%d", (intVal%86400)/3600 );
   		return websWrite(wp, buffer);
	}
	else if ( !strcmp(name, T("groupRekeyTimeMin"))) {
		if ( !apmib_get( MIB_WLAN_WPA_GROUP_REKEY_TIME, (void *)&intVal) )
			return -1;
		sprintf(buffer, "%d", ((intVal%86400)%3600)/60 );
   		return websWrite(wp, buffer);
	}
	else if ( !strcmp(name, T("groupRekeyTimeSec"))) {
		if ( !apmib_get( MIB_WLAN_WPA_GROUP_REKEY_TIME, (void *)&intVal) )
			return -1;
		sprintf(buffer, "%d", ((intVal%86400)%3600)%60 );
   		return websWrite(wp, buffer);
	}
 	else if ( !strcmp(name, T("pskValue"))) {
//		int i;
		buffer[0]='\0';
		if ( !apmib_get(MIB_WLAN_WPA_PSK,  (void *)buffer) )
			return -1;
		#if 0	//Brad modify 20080703
		for (i=0; i<strlen(buffer); i++)
			buffer[i]='*';
		buffer[i]='\0';
		#endif
   		return websWrite(wp, buffer);
	}

#ifdef CONFIG_RTK_MESH	
 	else if ( !strcmp(name, T("meshPskValue"))) {
		int i;
		buffer[0]='\0';
		if ( !apmib_get(MIB_MESH_WPA_PSK,  (void *)buffer) )
			return -1;
		/*for (i=0; i<strlen(buffer); i++)
			buffer[i]='*';
		buffer[i]='\0';*/	//by brian
   		return websWrite(wp, buffer);
	}	
#endif

#ifdef WIFI_SIMPLE_CONFIG 	
 	else if ( !strcmp(name, T("pskValueUnmask"))) {
		buffer[0]='\0';
		if ( !apmib_get(MIB_WLAN_WPA_PSK,  (void *)buffer) )
			return -1;
   		return websWrite(wp, buffer);
	}
 	else if ( !strcmp(name, T("wps_key"))) {
 		int id;
		apmib_get(MIB_WSC_ENC, (void *)&intVal);
		buffer[0]='\0';
		if (intVal == WSC_ENCRYPT_WEP) {
			unsigned char tmp[100];
			apmib_get(MIB_WLAN_WEP, (void *)&intVal);
			apmib_get(MIB_WLAN_WEP_DEFAULT_KEY, (void *)&id);
			if (intVal == 1) {
				if (id == 0)				
					id = MIB_WLAN_WEP64_KEY1;
				else if (id == 1)				
					id = MIB_WLAN_WEP64_KEY2;
				else if (id == 2)				
					id = MIB_WLAN_WEP64_KEY3;
				else	
					id = MIB_WLAN_WEP64_KEY4;				
				apmib_get(id, (void *)tmp);				
				convert_bin_to_str(tmp, 5, buffer);
			}
			else {
				if (id == 0)				
					id = MIB_WLAN_WEP128_KEY1;
				else if (id == 1)				
					id = MIB_WLAN_WEP128_KEY2;
				else if (id == 2)				
					id = MIB_WLAN_WEP128_KEY3;
				else	
					id = MIB_WLAN_WEP128_KEY4;				
				apmib_get(id, (void *)tmp);				
				convert_bin_to_str(tmp, 13, buffer);				
			}			
		}
		else {
			if (intVal==0 || intVal == WSC_ENCRYPT_NONE)
				strcpy(buffer, "N/A");
			else
				apmib_get(MIB_WSC_PSK, (void *)buffer);
		}
   		return websWrite(wp, buffer);
	} 	
#endif 	// WIFI_SIMPLE_CONFIG
 	else if ( !strcmp(name, T("wdsPskValue"))) {
		int i;
		buffer[0]='\0';
		if ( !apmib_get(MIB_WLAN_WDS_PSK,  (void *)buffer) )
			return -1;
		for (i=0; i<strlen(buffer); i++)
			buffer[i]='*';
		buffer[i]='\0';
   		return websWrite(wp, buffer);
	}
	else if ( !strcmp(name, T("accountRsUpdateDelay"))) {
		if ( !apmib_get( MIB_WLAN_ACCOUNT_UPDATE_DELAY, (void *)&intVal) )
			return -1;
		sprintf(buffer, "%d", intVal );
   		return websWrite(wp, buffer);
	}
	else if ( !strcmp(name, T("rsInterval"))) {
		if ( !apmib_get( MIB_WLAN_RS_INTERVAL_TIME, (void *)&intVal) )
			return -1;
		sprintf(buffer, "%d", intVal );
   		return websWrite(wp, buffer);
	}
	else if ( !strcmp(name, T("accountRsInterval"))) {
		if ( !apmib_get( MIB_WLAN_ACCOUNT_RS_INTERVAL_TIME, (void *)&intVal) )
			return -1;
		sprintf(buffer, "%d", intVal );
   		return websWrite(wp, buffer);
	}

#ifdef HOME_GATEWAY
#ifdef VPN_SUPPORT
	else if( !strcmp(name, T("vpnTblIdx"))){
              	sprintf(buffer, "%d", getVpnTblIdx());
                return websWrite(wp, T("%s"), buffer);
	}
	else if( !strcmp(name, T("ipsecConnName"))){
                if ( getIpsecInfo(&entry) < 0)
			sprintf(buffer, "%s", ""); // default
		else
			sprintf(buffer, "%s", entry.connName);

                return websWrite(wp, T("%s"), buffer);
	}
	else if( !strcmp(name, T("ipsecLocalIp"))){
                if ( getIpsecInfo(&entry) < 0){
			if(getInAddr(BRIDGE_IF, IP_ADDR, (void *)&intaddr ))
			 	return websWrite(wp, T("%s"), inet_ntoa(intaddr) );
			else{
 				if ( !apmib_get( MIB_IP_ADDR,  (void *)buffer) )
					 return websWrite(wp, T("0.0.0.0"));
				return websWrite(wp, T("%s"), inet_ntoa(*((struct in_addr *)buffer)) );
			}
		}
		else
                	return websWrite(wp, T("%s"), inet_ntoa(*((struct in_addr *) entry.lc_ipAddr)));
	}
	else if( !strcmp(name, T("ipsecLocalIpMask"))){
                if ( getIpsecInfo(&entry) < 0){
			if ( getInAddr(BRIDGE_IF, SUBNET_MASK, (void *)&intaddr ))
				return websWrite(wp, T("%s"), inet_ntoa(intaddr) );
			else{
 				if ( !apmib_get( MIB_SUBNET_MASK,  (void *)buffer) )
					 return websWrite(wp, T("0.0.0.0"));
				return websWrite(wp, T("%s"), inet_ntoa(*((struct in_addr *)buffer)) );
			}
		}
		else{
			len2Mask(entry.lc_maskLen, buffer);
                	return websWrite(wp, T("%s"), buffer);
		}
	}
	else if( !strcmp(name, T("ipsecRemoteIp"))){
                if ( getIpsecInfo(&entry) < 0)
			 return websWrite(wp, T("0.0.0.0"));
		else
                	return websWrite(wp, T("%s"), inet_ntoa(*((struct in_addr *) entry.rt_ipAddr)));
	}
	else if( !strcmp(name, T("ipsecRemoteIpMask"))){
                if ( getIpsecInfo(&entry) < 0)
			 return websWrite(wp, T("0.0.0.0"));
		else{
			len2Mask(entry.rt_maskLen, buffer);
                	return websWrite(wp, T("%s"), buffer);
		}
	}
	else if( !strcmp(name, T("ipsecRemoteGateway"))){
                if ( getIpsecInfo(&entry) < 0)
			 return websWrite(wp, T("0.0.0.0"));
		else
                	return websWrite(wp, T("%s"), inet_ntoa(*((struct in_addr *) entry.rt_gwAddr)));

	}
	else if( !strcmp(name, T("ipsecSpi"))){
                if ( getIpsecInfo(&entry) < 0)
			sprintf(buffer, "%s", ""); // default
		else
			sprintf(buffer, "%s",entry.spi);

                return websWrite(wp, T("%s"), buffer);
	}
	else if( !strcmp(name, T("ipsecEncrKey"))){
                if ( getIpsecInfo(&entry) < 0)
			sprintf(buffer, "%s", ""); // default
		else
			sprintf(buffer, "%s",entry.encrKey);

                return websWrite(wp, T("%s"), buffer);
	}
	else if( !strcmp(name, T("ipsecAuthKey"))){
                if ( getIpsecInfo(&entry) < 0)
			sprintf(buffer, "%s", ""); // default
		else
			sprintf(buffer, "%s",entry.authKey);

                return websWrite(wp, T("%s"), buffer);
	}
	else if( !strcmp(name, T("ikePsKey"))){
                if ( getIpsecInfo(&entry) < 0)
			sprintf(buffer, "%s", ""); // default
		else
			sprintf(buffer, "%s",entry.psKey);

                return websWrite(wp, T("%s"), buffer);
	}
	else if( !strcmp(name, T("ikeLifeTime"))){
                if ( getIpsecInfo(&entry) < 0)
			sprintf(buffer, "%d", 3600); // default
		else
			sprintf(buffer, "%lu",entry.ikeLifeTime);

                return websWrite(wp, T("%s"), buffer);
	}
	else if( !strcmp(name, T("ikeEncr"))){
                if ( getIpsecInfo(&entry) < 0)
			sprintf(buffer, "%d", TRI_DES_ALGO); // default
		else
			sprintf(buffer, "%d",entry.ikeEncr);

                return websWrite(wp, T("%s"), buffer);
	}
	else if( !strcmp(name, T("ikeAuth"))){
                if ( getIpsecInfo(&entry) < 0)
			sprintf(buffer, "%d", MD5_ALGO); // default
		else
			sprintf(buffer, "%d",entry.ikeAuth);

                return websWrite(wp, T("%s"), buffer);
	}
	else if( !strcmp(name, T("ikeKeyGroup"))){
                if ( getIpsecInfo(&entry) < 0)
			sprintf(buffer, "%d", DH2_GRP); // default 768 bits
		else
			sprintf(buffer, "%d",entry.ikeKeyGroup);

                return websWrite(wp, T("%s"), buffer);
	}
	else if( !strcmp(name, T("ipsecLifeTime"))){
                if ( getIpsecInfo(&entry) < 0)
			sprintf(buffer, "%d", 28800); // default
		else
			sprintf(buffer, "%lu",entry.ipsecLifeTime);

                return websWrite(wp, T("%s"), buffer);
	}
	else if( !strcmp(name, T("ipsecPfs"))){
                if ( getIpsecInfo(&entry) < 0)
			sprintf(buffer, "%d", 1); // default  on
		else
			sprintf(buffer, "%d",entry.ipsecPfs);

                return websWrite(wp, T("%s"), buffer);
	}
	else if( !strcmp(name, T("ipsecLocalId"))){
                if ( getIpsecInfo(&entry) < 0)
			sprintf(buffer, "%s", "");
		else
			sprintf(buffer, "%s",entry.lcId);

                return websWrite(wp, T("%s"), buffer);
	}
	else if( !strcmp(name, T("ipsecRemoteId"))){
                if ( getIpsecInfo(&entry) < 0)
			sprintf(buffer, "%s", "");
		else
			sprintf(buffer, "%s",entry.rtId);

                return websWrite(wp, T("%s"), buffer);
	}
	else if( !strcmp(name, T("rtRsaKey"))){
                if ( getIpsecInfo(&entry) < 0)
			sprintf(buffer, "%s", "");
		else
			sprintf(buffer, "%s",entry.rsaKey);
                return websWrite(wp, T("%s"), buffer);
	}
#endif
#endif
	else if ( !strcmp(name, T("userName"))){
		buffer[0]='\0';
                if ( !apmib_get(MIB_USER_NAME,  (void *)buffer) )
                        return -1;
                return websWrite(wp, T("%s"), buffer);
	}

#ifdef WLAN_EASY_CONFIG
	else if ( !strcmp(name, T("autoCfgAlgReq"))) {
		apmib_get( MIB_WLAN_MODE, (void *)&intVal);
		if (intVal==CLIENT_MODE) { // client
			if ( !apmib_get( MIB_WLAN_EASYCFG_ALG_REQ, (void *)&intVal) )
				return -1;
		}
		else {
			if ( !apmib_get( MIB_WLAN_EASYCFG_ALG_SUPP, (void *)&intVal) )
				return -1;
		}
		buffer[0]='\0';
		if (intVal & ACF_ALGORITHM_WEP64)
			strcat(buffer, "WEP64");
		if (intVal & ACF_ALGORITHM_WEP128) {
			if (strlen(buffer) > 0)
				strcat(buffer, "+");
			strcat(buffer, "WEP128");
		}
		if (intVal & ACF_ALGORITHM_WPA_TKIP) {
			if (strlen(buffer) > 0)
				strcat(buffer, "+");
			strcat(buffer, "WPA_TKIP");
		}
		if (intVal & ACF_ALGORITHM_WPA_AES) {
			if (strlen(buffer) > 0)
				strcat(buffer, "+");
			strcat(buffer, "WPA_AES");
		}
		if (intVal & ACF_ALGORITHM_WPA2_TKIP) {
			if (strlen(buffer) > 0)
				strcat(buffer, "+");
			strcat(buffer, "WPA2_TKIP");
		}
		if (intVal & ACF_ALGORITHM_WPA2_AES) {
			if (strlen(buffer) > 0)
				strcat(buffer, "+");
			strcat(buffer, "WPA2_AES");
		}
   		return websWrite(wp, buffer);
	}

	else if ( !strcmp(name, T("autoCfgKey"))) {
		if ( !apmib_get( MIB_WLAN_EASYCFG_KEY, (void *)buffer) )
			return -1;
		return websWrite(wp, buffer);
	}
#endif // WLAN_EASY_CONFIG

#ifdef WIFI_SIMPLE_CONFIG
	else if ( !strcmp(name, T("wscLoocalPin"))){
		buffer[0] = '\0';
		apmib_get(MIB_WSC_PIN,  (void *)buffer);
		return websWrite(wp, T("%s"), buffer);
	}
#endif // WIFI_SIMPLE_CONFIG
	else if(!strcmp(name, T("powerConsumption_menu")))
	{
#if defined(POWER_CONSUMPTION_SUPPORT)
		//return websWrite(wp, "manage.addItem('Power Consumption', 'powerConsumption.asp', '', 'Display power consumption');" );		
		return websWrite(wp,""); // keith. hidden page even enable power saving.
#else
		return websWrite(wp,"");
#endif
	}
#ifdef HOME_GATEWAY
	else if ( !strcmp(name, T("ddnsDomainName"))) {
		if ( !apmib_get( MIB_DDNS_DOMAIN_NAME, (void *)&buffer) )
			return -1;
   		return websWrite(wp, buffer);

	}
	else if ( !strcmp(name, T("dhcpLeaseTime"))) {
		apmib_get( MIB_DHCP_LEASE_TIME, (void *)&intVal);
		if(intVal == 0)
			intVal = 480;//(28800/60);
		else
			intVal /= 60;
		sprintf(buffer, "%d", intVal );
		return websWrite(wp, buffer);

	}
	else if ( !strcmp(name, T("ddnsUser"))) {
		if ( !apmib_get( MIB_DDNS_USER, (void *)&buffer) )
			return -1;
   		return websWrite(wp, buffer);

	}
	else if ( !strcmp(name, T("ddnsPassword"))) {
		if ( !apmib_get( MIB_DDNS_PASSWORD, (void *)&buffer) )
			return -1;
   		return websWrite(wp, buffer);

	}
#if defined(VLAN_CONFIG_SUPPORTED)
	else if(!strcmp(name, T("maxWebVlanNum")))
	{
		sprintf(buffer, "%d", MAX_IFACE_VLAN_CONFIG - 1 ); // last one is virtual interface, no need to be set
		return websWrite(wp, buffer);
	}
	else if(!strcmp(name, T("isVLANIncludeWLAN")))
	{
#if defined(CONFIG_NET_RADIO)		
		sprintf(buffer, "%s", "1" );
#else
		sprintf(buffer, "%s", "0");
#endif		
		return websWrite(wp, buffer);
	}
#endif //	#if defined(VLAN_CONFIG_SUPPORTED)
	else if(!strcmp(name, T("isPocketRouter")))
	{
#if defined(CONFIG_POCKET_ROUTER_SUPPORT)		
		sprintf(buffer, "%s", "1" );
#else
		sprintf(buffer, "%s", "0");
#endif		
		return websWrite(wp, buffer);
	}
	else if(!strcmp(name, T("pocketRouter_Mode"))) // 0:non-pocketRouter; 3: Router; 2:Bridge AP; 1:Bridge Client
	{
#if defined(CONFIG_POCKET_ROUTER_SUPPORT)
		apmib_get( MIB_OP_MODE, (void *)&intVal);
		if(intVal == 1) //opmode is bridge
		{
			apmib_get( MIB_WLAN_MODE, (void *)&intVal);
			if(intVal == 0) //wlan is AP mode
				sprintf(buffer, "%s", "2" );
			else if(intVal == 1) //wlan is client mode
				sprintf(buffer, "%s", "1" );
			else
				sprintf(buffer, "%s", "0" );
		}
		else if(intVal == 0) //opmode is router
		{
			sprintf(buffer, "%s", "3" );
		}
		
#else
		sprintf(buffer, "%s", "0");
#endif		
		return websWrite(wp, buffer);
	}
else if(!strcmp(name, T("pocketRouter_Mode_countdown"))) // 0:non-pocketRouter; 3: Router; 2:Bridge AP; 1:Bridge Client
	{
#if defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT)
		apmib_get( MIB_OP_MODE, (void *)&intVal);
		if(intVal == 1) //opmode is bridge
		{
			apmib_get( MIB_WLAN_MODE, (void *)&intVal);
			if(intVal == 0) //wlan is AP mode
				sprintf(buffer, "%s", "2" );
			else if(intVal == 1) //wlan is client mode
				sprintf(buffer, "%s", "1" );
			else
				sprintf(buffer, "%s", "0" );
		}
		else if(intVal == 0) //opmode is router
		{
			sprintf(buffer, "%s", "3" );
		}
		
#else
		sprintf(buffer, "%s", "0");
#endif		
		return websWrite(wp, buffer);
	}
	
	else if(!strcmp(name, T("countDownTime_wait"))) // 0:non-pocketRouter; 3: Router; 2:Bridge AP; 1:Bridge Client
	{
#if defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT)
		sprintf(buffer, "%d", WaitCountTime);
#else
		sprintf(buffer, "%s", "1");
#endif		
		return websWrite(wp, buffer);
	}
	else if(!strcmp(name, T("vlanOnOff")))
	{		
#if defined(VLAN_CONFIG_SUPPORTED)
		apmib_get( MIB_VLANCONFIG_ENABLED, (void *)&intVal);
		sprintf(buffer, "%d", intVal );
		return websWrite(wp, buffer);		
#else
		sprintf(buffer, "%d", 0 );
		return websWrite(wp, buffer);
#endif
	}
	else if(!strcmp(name, T("vlan_menu_onoff")))
	{
#if defined(VLAN_CONFIG_SUPPORTED)
		return websWrite(wp, "firewall.addItem('VLAN', 'vlan.asp', '', 'Setup VLAN');" );
#else
		return websWrite(wp,"");
#endif
}else if(!strcmp(name, T("route_menu_onoff")))
	{
#if defined(ROUTE_SUPPORT)
		return websWrite(wp, "menu.addItem(\"Route Setup\", \"route.asp\", \"\", \"Route Setup\");");
#else
		return websWrite(wp,"");
#endif
}
else if(!strcmp(name, T("status_menu")))
{
#if defined(CONFIG_NET_RADIO)
		return websWrite(wp, "manage.addItem(\"Status\", \"status.asp\", \"\", \"Display current status\");");
#else
		return websWrite(wp, "manage.addItem(\"Status\", \"status_nowlan.asp\", \"\", \"Display current status\");");
#endif
}
else if(!strcmp(name, T("statistic_menu")))
{
#if defined(CONFIG_NET_RADIO)
		return websWrite(wp, "manage.addItem(\"Statistics\", \"stats.asp\", \"\", \"Display packet statistics\");");
#else
		return websWrite(wp, "manage.addItem(\"Statistics\", \"stats_nowlan.asp\", \"\", \"Display packet statistics\");");
#endif
}
	else if ( !strcmp(name, T("wlanMode"))) {
		if ( !apmib_get( MIB_WLAN_MODE, (void *)&intVal) )
			return -1;
		sprintf(buffer, "%d", intVal);
		return websWrite(wp,buffer);
		return 0;
	}
#if defined(GW_QOS_ENGINE)
	else if(!strcmp(name, T("qos_root_menu"))){
		return websWrite(wp, 
				"menu.addItem('QoS', 'qos.asp', '', 'Setup QoS');" );
	}

	else if ( !strcmp(name, T("qosEnabled")) ) {
		if ( !apmib_get( MIB_QOS_ENABLED, (void *)&intVal) )
			return -1;
		if ( intVal == 0 )
			strcpy(buffer, "false");
		else
			strcpy(buffer, "true");
		return websWrite(wp, buffer);	}
	else if ( !strcmp(name, T("qosAutoUplinkSpeed")) ) {
		if ( !apmib_get( MIB_QOS_AUTO_UPLINK_SPEED, (void *)&intVal) )
			return -1;
		if ( intVal == 0 )
			strcpy(buffer, "false");
		else
			strcpy(buffer, "true");
		return websWrite(wp, buffer);	}
	else if ( !strcmp(name, T("qosManualUplinkSpeed")) ) {
		if ( !apmib_get( MIB_QOS_MANUAL_UPLINK_SPEED, (void *)&intVal) )
			return -1;
	       sprintf(buffer, "%d", intVal );
		return websWrite(wp, buffer);	}
	else if ( !strcmp(name, T("qosManualDownlinkSpeed")) ) {
		if ( !apmib_get( MIB_QOS_MANUAL_DOWNLINK_SPEED, (void *)&intVal) )
			return -1;
		
		sprintf(buffer, "%d", intVal );
		
		return websWrite(wp, buffer);	}

#elif defined(QOS_BY_BANDWIDTH)
	else if(!strcmp(name, T("qos_root_menu"))){
		return websWrite(wp, "menu.addItem('QoS', 'ip_qos.asp', '', 'Setup QoS');" );
	}
	else if ( !strcmp(name, T("qosEnabled")) ) {
		if ( !apmib_get( MIB_QOS_ENABLED, (void *)&intVal) )
			return -1;
		if ( intVal == 0 )
			strcpy(buffer, "false");
		else
			strcpy(buffer, "true");
		return websWrite(wp, buffer);	}
	else if ( !strcmp(name, T("qosAutoUplinkSpeed")) ) {
		if ( !apmib_get( MIB_QOS_AUTO_UPLINK_SPEED, (void *)&intVal) )
			return -1;
		if ( intVal == 0 )
			strcpy(buffer, "false");
		else
			strcpy(buffer, "true");
		return websWrite(wp, buffer);	}
	else if ( !strcmp(name, T("qosManualUplinkSpeed")) ) {
		if ( !apmib_get( MIB_QOS_MANUAL_UPLINK_SPEED, (void *)&intVal) )
			return -1;
	  
	  sprintf(buffer, "%d", intVal );
		return websWrite(wp, buffer);	}
	else if ( !strcmp(name, T("qosManualDownlinkSpeed")) ) {
		if ( !apmib_get( MIB_QOS_MANUAL_DOWNLINK_SPEED, (void *)&intVal) )
			return -1;
		
		sprintf(buffer, "%d", intVal );
		
		return websWrite(wp, buffer);	}
#else
	else if(!strcmp(name, T("qos_root_menu"))){
		return 0;
	}
#endif

#ifdef DOS_SUPPORT
	else if ( !strcmp(name, T("syssynFlood"))) {
		if ( !apmib_get( MIB_DOS_SYSSYN_FLOOD, (void *)&intVal) )
			return -1;
		sprintf(buffer, "%d", intVal );
		return websWrite(wp, buffer);	}
	else if ( !strcmp(name, T("sysfinFlood"))) {
		if ( !apmib_get( MIB_DOS_SYSFIN_FLOOD, (void *)&intVal) )
			return -1;
		sprintf(buffer, "%d", intVal );
		return websWrite(wp, buffer);	}
	else if ( !strcmp(name, T("sysudpFlood"))) {
		if ( !apmib_get( MIB_DOS_SYSUDP_FLOOD, (void *)&intVal) )
			return -1;
		sprintf(buffer, "%d", intVal );
		return websWrite(wp, buffer);	}
	else if ( !strcmp(name, T("sysicmpFlood"))) {
		if ( !apmib_get( MIB_DOS_SYSICMP_FLOOD, (void *)&intVal) )
			return -1;
		sprintf(buffer, "%d", intVal );
		return websWrite(wp, buffer);	}
	else if ( !strcmp(name, T("pipsynFlood"))) {
		if ( !apmib_get( MIB_DOS_PIPSYN_FLOOD, (void *)&intVal) )
			return -1;
		sprintf(buffer, "%d", intVal );
		return websWrite(wp, buffer);	}
	else if ( !strcmp(name, T("pipfinFlood"))) {
		if ( !apmib_get( MIB_DOS_PIPFIN_FLOOD, (void *)&intVal) )
			return -1;
		sprintf(buffer, "%d", intVal );
		return websWrite(wp, buffer);	}
	else if ( !strcmp(name, T("pipudpFlood"))) {
		if ( !apmib_get( MIB_DOS_PIPUDP_FLOOD, (void *)&intVal) )
			return -1;
		sprintf(buffer, "%d", intVal );
		return websWrite(wp, buffer);	}
	else if ( !strcmp(name, T("pipicmpFlood"))) {
		if ( !apmib_get( MIB_DOS_PIPICMP_FLOOD, (void *)&intVal) )
			return -1;
		sprintf(buffer, "%d", intVal );
		return websWrite(wp, buffer);	}
	else if ( !strcmp(name, T("blockTime"))) {
		if ( !apmib_get( MIB_DOS_BLOCK_TIME, (void *)&intVal) )
			return -1;
		sprintf(buffer, "%d", intVal );
		return websWrite(wp, buffer);	}
#endif // DOS_SUPPORT
	else if ( !strcmp(name, T("hostName"))) {
		if ( !apmib_get( MIB_HOST_NAME, (void *)&buffer) )
			return -1;
   		return websWrite(wp, buffer);
	}
#endif // HOME_GATEWAY
	else if ( !strcmp(name, T("rtLogServer"))) {
		if ( !apmib_get( MIB_REMOTELOG_SERVER,  (void *)buffer) )
			return -1;
		if (!memcmp(buffer, "\x0\x0\x0\x0", 4))
			return websWrite(wp, T(""));
   		return websWrite(wp, T("%s"), inet_ntoa(*((struct in_addr *)buffer)) );
	}
	else if ( !strcmp(name, T("domainName"))) {
		if ( !apmib_get( MIB_DOMAIN_NAME, (void *)&buffer) )
			return -1;
   		return websWrite(wp, buffer);
	}	
#ifdef CONFIG_RTK_MESH
       
    else if ( !strcmp(name, T("meshMaxNeightbor"))) {
            if ( !apmib_get( MIB_MESH_MAX_NEIGHTBOR, (void *)&intVal) )
                    return -1;
            sprintf(buffer, "%d", intVal );
            return websWrite(wp, buffer);
    }
    else if ( !strcmp(name, T("meshID"))) {
            if ( !apmib_get(MIB_MESH_ID,  (void *)buffer) )
                    return -1;
            translate_control_code(buffer);
            return websWrite(wp, T("%s"), buffer);
    }

#ifdef 	_11s_TEST_MODE_	

		else if ( !strcmp(name, T("meshTestParam1"))) {
  			if ( !apmib_get( MIB_MESH_TEST_PARAM1, (void *)&intVal) )
  				return -1;
  			sprintf(buffer, "%d", intVal );
	  		return websWrite(wp, buffer);
  		}
  		else if ( !strcmp(name, T("meshTestParam2"))) {
	  		if ( !apmib_get( MIB_MESH_TEST_PARAM2, (void *)&intVal) )
		  		return -1;
	  		sprintf(buffer, "%d", intVal );
	  		return websWrite(wp, buffer);
  		}
  		else if ( !strcmp(name, T("meshTestParam3"))) {
  			if ( !apmib_get( MIB_MESH_TEST_PARAM3, (void *)&intVal) )
	  			return -1;
	  		sprintf(buffer, "%d", intVal );
	  		return websWrite(wp, buffer);
  		}
  		else if ( !strcmp(name, T("meshTestParam4"))) {
  			if ( !apmib_get( MIB_MESH_TEST_PARAM4, (void *)&intVal) )
		  		return -1;
	  		sprintf(buffer, "%d", intVal );
	  		return websWrite(wp, buffer);
  		}
		else if ( !strcmp(name, T("meshTestParam5"))) {
	  		if ( !apmib_get( MIB_MESH_TEST_PARAM5, (void *)&intVal) )
	  			return -1;
	  		sprintf(buffer, "%d", intVal );
	  		return websWrite(wp, buffer);
  		}
  		else if ( !strcmp(name, T("meshTestParam6"))) {
	  		if ( !apmib_get( MIB_MESH_TEST_PARAM6, (void *)&intVal) )
	  		return -1;
	  		sprintf(buffer, "%d", intVal );
	  		return websWrite(wp, buffer);
  		}
  		else if ( !strcmp(name, T("meshTestParam7"))) {
	  		if ( !apmib_get( MIB_MESH_TEST_PARAM7, (void *)&intVal) )
	  			return -1;
	  		sprintf(buffer, "%d", intVal );
	  		return websWrite(wp, buffer);
  		}
  		else if ( !strcmp(name, T("meshTestParam8"))) {
	  		if ( !apmib_get( MIB_MESH_TEST_PARAM8, (void *)&intVal) )
		  		return -1;
	  		sprintf(buffer, "%d", intVal );
	  		return websWrite(wp, buffer);
  		}
  		else if ( !strcmp(name, T("meshTestParam9"))) {
	  		if ( !apmib_get( MIB_MESH_TEST_PARAM9, (void *)&intVal) )
		  		return -1;
	  		sprintf(buffer, "%d", intVal );
	  		return websWrite(wp, buffer);
  		}
  		else if ( !strcmp(name, T("meshTestParama"))) {
	  		if ( !apmib_get( MIB_MESH_TEST_PARAMA, (void *)&intVal) )
	  			return -1;
	  		sprintf(buffer, "%d", intVal );
	  		return websWrite(wp, buffer);
  		}
  		else if ( !strcmp(name, T("meshTestParamb"))) {
	  		if ( !apmib_get( MIB_MESH_TEST_PARAMB, (void *)&intVal) )
	 	 		return -1;
	  		sprintf(buffer, "%d", intVal );
	  		return websWrite(wp, buffer);
  		}
  		else if ( !strcmp(name, T("meshTestParamc"))) {
	  		if ( !apmib_get( MIB_MESH_TEST_PARAMC, (void *)&intVal) )
		  		return -1;
	  		sprintf(buffer, "%d", intVal );
	  		return websWrite(wp, buffer);
  		}
  		else if ( !strcmp(name, T("meshTestParamd"))) {
	  		if ( !apmib_get( MIB_MESH_TEST_PARAMD, (void *)&intVal) )
		  		return -1;
	  		sprintf(buffer, "%d", intVal );
	  		return websWrite(wp, buffer);
  		}
  		else if ( !strcmp(name, T("meshTestParame"))) {
	  		if ( !apmib_get( MIB_MESH_TEST_PARAME, (void *)&intVal) )
		  		return -1;
	  		sprintf(buffer, "%d", intVal );
	  		return websWrite(wp, buffer);
  		}
  		else if ( !strcmp(name, T("meshTestParamf"))) {
	  		if ( !apmib_get( MIB_MESH_TEST_PARAMF, (void *)&intVal) )
	  			return -1;
	  		sprintf(buffer, "%d", intVal );
	  		return websWrite(wp, buffer);
  		}
		else if ( !strcmp(name, T("meshTestParamStr1"))) {
			if ( !apmib_get( MIB_MESH_TEST_PARAMSTR1, (void *)buffer) )
				return -1;
	        translate_control_code(buffer);
	        return websWrite(wp, buffer);			
		}
#endif
        
#endif // CONFIG_RTK_MESH
#ifdef UNIVERSAL_REPEATER

	else if ( !strcmp(name, T("repeaterSSID"))) {
		if (wlan_idx == 0)
			intVal = MIB_REPEATER_SSID1;
		else
			intVal = MIB_REPEATER_SSID2;			
		apmib_get(intVal, (void *)buffer);		
		translate_control_code(buffer);
		return websWrite(wp, T("%s"), buffer);
   	}
#if 0	
   	else if ( !strcmp(name, T("repeaterEncrypt"))) {
 		ENCRYPT_T encrypt;
   		apmib_get( MIB_WLAN_ENCRYPT,  (void *)&encrypt);
		if (encrypt == ENCRYPT_DISABLED)
			strcpy( buffer, T("Disabled") );
		else if (encrypt == ENCRYPT_WEP) {
			apmib_get(MIB_WLAN_ENABLE_1X, &intVal);
			if (intVal == 0) {
       			apmib_get( MIB_WLAN_WEP,  (void *)&intVal);
				if ( intVal == WEP_DISABLED )
					strcpy( buffer, T("Disabled") );
				else if ( intVal == WEP64 )
					strcpy( buffer, T("WEP 64bits") );
				else if ( intVal == WEP128)
					strcpy( buffer, T("WEP 128bits") );
			}
			else
				strcpy( buffer, T("Disabled") );
		}
		else {
			apmib_get(MIB_WLAN_WPA_AUTH, &intVal);
			if (intVal == WPA_AUTH_PSK) {
				if (encrypt == ENCRYPT_WPA2 )
					strcpy( buffer, T("WPA2") );
				else
					strcpy( buffer, T("WPA") );
			}
			else
				strcpy( buffer, T("Disabled") );
		}
		return websWrite(wp, buffer);
   	}
#endif		
	else if ( !strcmp(name, T("repeaterState"))) {
		char *pMsg;
		if (wlan_idx == 0)
			strcpy(buffer, "wlan0-vxd");
		else
			strcpy(buffer, "wlan1-vxd");
		getWlBssInfo(buffer, &bss);
		switch (bss.state) {
		case STATE_DISABLED:
			pMsg = T("Disabled");
			break;
		case STATE_IDLE:
			pMsg = T("Idle");
			break;
		case STATE_STARTED:
			pMsg = T("Started");
			break;
		case STATE_CONNECTED:
			pMsg = T("Connected");
			break;
		case STATE_WAITFORKEY:
			pMsg = T("Waiting for keys");
			break;
		case STATE_SCANNING:
			pMsg = T("Scanning");
			break;
		default:
			pMsg=NULL;
		}
		return websWrite(wp, T("%s"), pMsg);
	}
 	else if ( !strcmp(name, T("repeaterClientnum"))) {
		if (wlan_idx == 0)
			strcpy(buffer, "wlan0-vxd");
		else
			strcpy(buffer, "wlan1-vxd");
 		if(getWlStaNum(buffer, &intVal)<0)
 			intVal=0;
		sprintf(buffer, "%d", intVal );
		return websWrite(wp, buffer);
	}
	else if ( !strcmp(name, T("repeaterSSID_drv"))) {		
#if defined(CONFIG_RTL8196C) && !defined(CONFIG_WLAN_REPEATER_MODE)// keith. disabled if no this mode in 96c
		return websWrite(wp, T("%s"), "e0:00:19:78:01:10");
#elif defined(CONFIG_RTL8198) && !defined(CONFIG_WLAN_REPEATER_MODE)// keith. disabled if no this mode in 96c
		return websWrite(wp, T("%s"), "e0:00:19:78:01:10");		
#else		
		if (wlan_idx == 0)
			strcpy(buffer, "wlan0-vxd");
		else
			strcpy(buffer, "wlan1-vxd");
		getWlBssInfo(buffer, &bss);
		return websWrite(wp, T("%s"), bss.ssid);
#endif		
	}
	else if ( !strcmp(name, T("repeaterBSSID"))) {
		if (wlan_idx == 0)
			strcpy(buffer, "wlan0-vxd");
		else
			strcpy(buffer, "wlan1-vxd");
		getWlBssInfo(buffer, &bss);
		return websWrite(wp, T("%02x:%02x:%02x:%02x:%02x:%02x"), bss.bssid[0], bss.bssid[1],
				bss.bssid[2], bss.bssid[3], bss.bssid[4], bss.bssid[5]);
	}
	else if ( !strcmp(name, T("wlanRepeaterTxPacketNum"))) {
		if (wlan_idx == 0)
			strcpy(buffer, "wlan0-vxd");
		else
			strcpy(buffer, "wlan1-vxd");
		if ( getStats(buffer, &stats) < 0)
			stats.tx_packets = 0;
		sprintf(buffer, "%d", (int)stats.tx_packets);
   		return websWrite(wp, buffer);

	}
	else if ( !strcmp(name, T("wlanRepeaterRxPacketNum"))) {
		if (wlan_idx == 0)
			strcpy(buffer, "wlan0-vxd");
		else
			strcpy(buffer, "wlan1-vxd");
		if ( getStats(buffer, &stats) < 0)
			stats.rx_packets = 0;
		sprintf(buffer, "%d", (int)stats.rx_packets);
   		return websWrite(wp, buffer);
	}
#endif	// UNIVERSAL_REPEATER
// added by rock /////////////////////////////////////////
#ifdef VOIP_SUPPORT 
	else if (!strncmp(name, T("voip_"), 5)) {
		return asp_voip_getInfo(eid, wp, argc, argv);
	}
#else
	else if (!strncmp(name, T("voip_"), 5)) {
   		return 0;
	}
#endif

/*+++++added by Jack for TR-069 configuration+++++*/ 
#ifdef CONFIG_CWMP_TR069
	else if(!strcmp(name, T("cwmp_tr069_menu"))){
#if 0		
		return websWrite(wp, 
				"menu.addItem('TR-069');" \
				"tr069 = new MTMenu();" \
				"tr069.addItem('TR-069 config', 'tr069config.asp', '', 'Setup TR-069 configuration');" \
				"menu.makeLastSubmenu(tr069);");
#else
		return websWrite(wp, "manage.addItem('TR-069 config', 'tr069config.asp', '', 'Setup TR-069 configuration');" );
#endif		
	}else if(!strcmp(name, T("tr069_nojs_menu"))){
		return websWrite(wp,
				"document.write('"\
				"<tr><td><b>cwmp_tr069_menu</b></td></tr>"\
				"<tr><td><a href=\"tr069config.asp\" target=\"view\">TR-069 config</a></td></tr>"\
				"')");
	}else if(!strcmp(name, T("acs_url"))){
		if ( !apmib_get( CWMP_ACS_URL, (void *)buffer) )
			return -1;
		return websWrite(wp, buffer);
		
	}else if(!strcmp(name, T("acs_username"))){
		if ( !apmib_get( CWMP_ACS_USERNAME, (void *)buffer) )
			return -1;
		return websWrite(wp, buffer);
		
	}else if(!strcmp(name, T("acs_password"))){
		if ( !apmib_get( CWMP_ACS_PASSWORD, (void *)buffer) )
			return -1;
		return websWrite(wp, buffer);
		
	}else if(!strcmp(name, T("tr069-inform-0"))){
		if ( !apmib_get( CWMP_INFORM_ENABLE, (void *)&intVal) )
			return -1;
		if(intVal == 1){
			return websWrite(wp, "");
		}else{
			return websWrite(wp, "checked");
		}
	}else if(!strcmp(name, T("tr069-inform-1"))){
		if ( !apmib_get( CWMP_INFORM_ENABLE, (void *)&intVal) )
			return -1;
		if(intVal == 1){
			return websWrite(wp, "checked");
		}else{
			return websWrite(wp, "");
		}
	}else if(!strcmp(name, T("inform_interval"))){
		if ( !apmib_get( CWMP_INFORM_INTERVAL, (void *)&intVal) )
			return -1;
		sprintf(buffer, "%d", intVal );
		return websWrite(wp, buffer);
		
	}else if(!strcmp(name, T("tr069_interval"))){
		if ( !apmib_get( CWMP_INFORM_ENABLE, (void *)&intVal) )
			return -1;
		if(intVal == 1){
			return websWrite(wp, "");
		}else{
			return websWrite(wp, "disabled");
		}
	}else if(!strcmp(name, T("conreq_name"))){
		if ( !apmib_get( CWMP_CONREQ_USERNAME, (void *)buffer) )
			return -1;
		return websWrite(wp, buffer);
		
	}else if(!strcmp(name, T("conreq_pw"))){
		if ( !apmib_get( CWMP_CONREQ_PASSWORD, (void *)buffer) )
			return -1;
		return websWrite(wp, buffer);
		
	}else if(!strcmp(name, T("conreq_path"))){
		if ( !apmib_get( CWMP_CONREQ_PATH, (void *)buffer) )
			return -1;
		return websWrite(wp, buffer);
		
	}else if(!strcmp(name, T("conreq_port"))){
		if ( !apmib_get( CWMP_CONREQ_PORT, (void *)&intVal) )
			return -1;
		sprintf(buffer, "%d", intVal );
		return websWrite(wp, buffer);
		
	}else if(!strcmp(name, T("tr069-dbgmsg-0"))){
		if ( !apmib_get( CWMP_FLAG, (void *)&intVal) )
			return -1;
		if(intVal & CWMP_FLAG_DEBUG_MSG){
			 return websWrite(wp,"");
		}else{
			return websWrite(wp,"checked");
		}
	}else if(!strcmp(name, T("tr069-dbgmsg-1"))){
		if ( !apmib_get( CWMP_FLAG, (void *)&intVal) )
			return -1;
		if(intVal & CWMP_FLAG_DEBUG_MSG){
			 return websWrite(wp,"checked");
		}else{
			 return websWrite(wp,"");
		}
	}else if(!strcmp(name, T("tr069-sendgetrpc-0"))){
		if ( !apmib_get( CWMP_FLAG, (void *)&intVal) )
			return -1;
		if(intVal & CWMP_FLAG_SENDGETRPC){
			return websWrite(wp,"");
		}else{
			return websWrite(wp,"checked");
		}
	}else if(!strcmp(name, T("tr069-sendgetrpc-1"))){
		if ( !apmib_get( CWMP_FLAG, (void *)&intVal) )
			return -1;
		if(intVal & CWMP_FLAG_SENDGETRPC){
			return websWrite(wp,"checked");
		}else{
			return websWrite(wp,"");
		}
	}else if(!strcmp(name, T("tr069-skipmreboot-0"))){
		if ( !apmib_get( CWMP_FLAG, (void *)&intVal) )
			return -1;
		if(intVal & CWMP_FLAG_SKIPMREBOOT){
			return websWrite(wp,"");
		}else{
			return websWrite(wp,"checked");
		}
	}else if(!strcmp(name, T("tr069-skipmreboot-1"))){
		if ( !apmib_get( CWMP_FLAG, (void *)&intVal) )
			return -1;
		if(intVal & CWMP_FLAG_SKIPMREBOOT){
			return websWrite(wp,"checked");
		}else{
			return websWrite(wp,"");
		}
	}else if(!strcmp(name, T("tr069-autoexec-0"))){
		if ( !apmib_get( CWMP_FLAG, (void *)&intVal) )
			return -1;
		if(intVal & CWMP_FLAG_AUTORUN){
			return websWrite(wp,"");
		}else{
			return websWrite(wp,"checked");
		}
	}else if(!strcmp(name, T("tr069-autoexec-1"))){
		if ( !apmib_get( CWMP_FLAG, (void *)&intVal) )
			return -1;
		if(intVal & CWMP_FLAG_AUTORUN){
			return websWrite(wp,"checked");
		}else{
			return websWrite(wp,"");
		}
	}else if(!strcmp(name, T("tr069-delay-0"))){
		if ( !apmib_get( CWMP_FLAG, (void *)&intVal) )
			return -1;
		if(intVal & CWMP_FLAG_DELAY){
			return websWrite(wp,"");
		}else{
			return websWrite(wp,"checked");
		}
	}else if(!strcmp(name, T("tr069-delay-1"))){
		if ( !apmib_get( CWMP_FLAG, (void *)&intVal) )
			return -1;
		if(intVal & CWMP_FLAG_DELAY){
			return websWrite(wp,"checked");
		}else{
			return websWrite(wp,"");
		}
	}
#else
	else if(!strcmp(name, T("cwmp_tr069_menu")) || !strcmp(name, T("tr069_nojs_menu")) ){
		return 0;
	}
#endif /*CONFIG_CWMP_TR069*/
/*-----end-----*/

#ifdef CONFIG_RTL_WAPI_SUPPORT
	else if(!strcmp(argv[0],T("wapiOption")))
	{
		websWrite(wp,"<option value=\"7\"> WAPI </option>");
		return 0;
	}
	else if(!strcmp(argv[0],T("wapiMenu")))
	{
		websWrite(wp,"menu.addItem(\"WAPI\");");
		websWrite(wp,"wapi = new MTMenu();");
		websWrite(wp,"wapi.addItem(\"Certification Install\", \"wlwapiinstallcert.asp\", \"\", \"Install Ceritification\");");
#ifdef CONFIG_RTL_WAPI_LOCAL_AS_SUPPORT
		websWrite(wp,"wapi.addItem(\"Certification Manage\", \"wlwapiCertManagement.asp\", \"\", \"Manage Ceritification\");");
#endif
		websWrite(wp,"wapi.addItem(\"Key Update\", \"wlwapiRekey.asp\", \"\", \"Key update\");");
		websWrite(wp,"menu.makeLastSubmenu(wapi);");
		return 0;
	}
	else if(!strcmp(argv[0], T("wapiUcastTime")))
	{
		if ( !apmib_get(MIB_WLAN_WAPI_UCAST_TIME,  (void*)&intVal))
			return -1;
		websWrite(wp, T("%d"),intVal);
		return 0;
	}else if(!strcmp(argv[0], T("wapiUcastPackets")))
	{
		if ( !apmib_get(MIB_WLAN_WAPI_UCAST_PACKETS, (void*)&intVal))
			return -1;
		websWrite(wp, T("%d"),intVal);
		return 0;
	}	else if(!strcmp(argv[0], T("wapiMcastTime")))
	{
		if ( !apmib_get(MIB_WLAN_WAPI_MCAST_TIME,  (void*)&intVal))
			return -1;
		websWrite(wp, T("%d"),intVal);
		return 0;
	}
	else if(!strcmp(argv[0], T("wapiMcastPackets")))
	{
		if ( !apmib_get(MIB_WLAN_WAPI_MCAST_PACKETS,  (void*)&intVal))
			return -1;
		websWrite(wp, T("%d"),intVal);
		return 0;
	}
	else if(!strcmp(argv[0], T("wapiPskValue")))
	{
		if ( !apmib_get(MIB_WLAN_WAPI_PSK,  (void*)buffer))
			return -1;
		websWrite(wp, T("%s"),buffer);
		return 0;
	}else if(!strcmp(argv[0], T("wapiASIp")))
	{
		if ( !apmib_get(MIB_WLAN_WAPI_ASIPADDR,  (void*)buffer))
			return -1;
		if (!memcmp(buffer, "\x0\x0\x0\x0", 4))
			return websWrite(wp, T(""));
   		return websWrite(wp, T("%s"), inet_ntoa(*((struct in_addr *)buffer)));
	}
	else if(!strcmp(argv[0],T("wapiCert")))
	{
		int index;
		int count;
		int i;
		struct stat status;
		char tmpbuf[10];

		CERTS_DB_ENTRY_Tp cert=(CERTS_DB_ENTRY_Tp)malloc(128*sizeof(CERTS_DB_ENTRY_T));
		//Search Index 1--all, 2--serial.no, 3--owner, 4--type, 5--status
		if (!apmib_get(MIB_WLAN_WAPI_SEARCH_CERTINDEX,  (void*)&index))
		{
			free(cert);
			return -1;
		}
		if(!apmib_get(MIB_WLAN_WAPI_SEARCH_CERTINFO,  (void*)buffer))
		{
			free(cert);
			return -1;
		}
		
		/*update wapiCertInfo*/
		system("openssl ca -updatedb 2>/dev/null");
		if (stat(WAPI_CERT_CHANGED, &status) == 0) { // file existed
			system("storeWapiFiles -allUser");
		}
		
		count=searchWapiCert(cert,index,buffer);
		if(count == 0)
			websWrite(wp, T("%s"),T("[]"));
		else
		{
			websWrite(wp, T("%s"),T("["));
			for(i=0;i<count;i++)
			{
				sprintf(tmpbuf, "%08X",cert[i].serial);
				websWrite(wp,T("['%s','%s','%d','%d',"),cert[i].userName,tmpbuf,cert[i].validDays,cert[i].validDaysLeft);
				if(0 == cert[i].certType)
				{
					websWrite(wp,T("'%s',"),T("X.509"));
				}
				if(0==cert[i].certStatus)
				{
					websWrite(wp,T("'%s'"),T("actived"));
				}else if(1 ==cert[i].certStatus)
				{
					websWrite(wp,T("'%s'"),T("expired"));
				}else if(2 ==cert[i].certStatus)
				{
					websWrite(wp,T("'%s'"),T("revoked"));
				}
				if(i ==(count-1))
					websWrite(wp, T("%s"),T("]"));
				else
					websWrite(wp, T("%s"),T("],"));
			}
			websWrite(wp, T("%s"),T("]"));	
		}
		free(cert);
		return 0;
	}
else if(!strcmp(argv[0],T("caCertExist")))
	{
		 struct stat status;
		 if (stat(CA_CERT, &status) < 0)
		 {
		 	intVal=0;	//CA_CERT not exist
		 }
		 else
		 {
		 	intVal=1;	//CA_CERT exists
		 }
		 websWrite(wp, T("%d"),intVal);
		return 0;
	}
	else if(!strcmp(argv[0],T("asCerExist")))
	{
		 struct stat status;
		 if (stat(AS_CER, &status) < 0)
		 {
		 	intVal=0;	//AS_CER not exist
		 }
		 else
		 {
		 	intVal=1;	//AS_CER exists
		 }
		 websWrite(wp, T("%d"),intVal);
		return 0;
	}
	else if(!strcmp(argv[0],T("notSyncSysTime")))
	{
		 struct stat status;
		 time_t  now;
	        struct tm *tnow;
		
		 if (stat(SYS_TIME_NOT_SYNC_CA, &status) < 0)
		 {
		 	//SYS_TIME_NOT_SYNC_CA not exist
		 	
		 	now=time(0);
                    	tnow=localtime(&now);
                 	//printf("now=%ld, %d %d %d %d %d %d, tm_isdst=%d\n",now, 1900+tnow->tm_year,tnow->tm_mon+1,tnow->tm_mday,tnow->tm_hour,tnow->tm_min,tnow->tm_sec, tnow->tm_isdst);//Added for test

			if(1900+tnow->tm_year < 2009)
			{
				intVal=1;	//current year of our system < 2009 which means our system hasn't sync time yet
			}
			else
			{
		 		intVal=0;	//SYS_TIME_NOT_SYNC_CA not exist and current time >= year 2009 which means our system has sync time already
			}
		 }
		 else
		 {
		 	intVal=1;	//SYS_TIME_NOT_SYNC_CA exists which means our system hasn't sync time yet
		 	sprintf(buffer, "rm -f %s 2>/dev/null", SYS_TIME_NOT_SYNC_CA);
			system(buffer);
		 }
		 websWrite(wp, T("%d"),intVal);
		return 0;
	}
	else if(!strcmp(argv[0],T("wapiLocalAsSupport")))
	{
#ifdef CONFIG_RTL_WAPI_LOCAL_AS_SUPPORT
		websWrite(wp,T("%s"),T("true"));
#else
		websWrite(wp,T("%s"),T("false"));
#endif
		return 0;
	}
#else
	else if(!strncmp(argv[0],T("wapi"),4))
	{
		/*if wapi not enabled*/
		return 0;
	}
	else if(!strcmp(argv[0],T("wapiLocalAsSupport")))
	{
		websWrite(wp,T("%s"),T("false"));
		return 0;
	}
#endif

#ifdef REBOOT_CHECK
	else if(!strcmp(argv[0],T("countDownTime")))
	{
		websWrite(wp, T("%d"),countDownTime);
		countDownTime = APPLY_COUNTDOWN_TIME;
		return 0;
	}
	
	else if(!strcmp(argv[0],T("okMsg")))
	{
		websWrite(wp, T("%s"), okMsg);
		memset(okMsg,0x00,sizeof(okMsg));
		return 0;
	}
	
	else if(!strcmp(argv[0],T("lastUrl")))
	{
		if(strlen(lastUrl) == 0)
			websWrite(wp, T("%s"), "/wizard.asp");
		else
			websWrite(wp, T("%s"), lastUrl);

		memset(lastUrl,0x00,sizeof(lastUrl));
		return 0;
	}
#endif
	else if(!strcmp(argv[0],T("status_warning")))
	{
#ifdef REBOOT_CHECK		
		if(needReboot == 1)
		{
			websWrite(wp, T("%s"), "<tr><td></td></tr><tr><td><font size=2><font color='#FF0000'> \
 															Below status shows currnt settings, but does not take effect. \
															</font></td></tr>");
		}
		else
#endif			
		{
			websWrite(wp, T("%s"), "");
		}
		
		return 0;
	}
	else if(!strcmp(argv[0],T("wlan_onoff_tkip")))
	{
		apmib_get(MIB_WLAN_11N_ONOFF_TKIP, (void *)&intVal);
		websWrite(wp, T("%d"),intVal);
		
		return 0;
	}
	else if(!strcmp(argv[0],T("onoff_tkip_comment_start")))
	{
		int wlanMode=0;
		
		apmib_get(MIB_WLAN_11N_ONOFF_TKIP, (void *)&intVal);
		apmib_get(MIB_WLAN_BAND, (void *)&wlanMode);
		if(intVal == 0 && (wlanMode==8 || wlanMode==10 || wlanMode==11))
			websWrite(wp, T("%s"),"<!--");
		else
			websWrite(wp, T("%s"),"");
		
		return 0;
	}
	else if(!strcmp(argv[0],T("onoff_tkip_comment_end")))
	{
		int wlanMode=0;
		
		apmib_get(MIB_WLAN_11N_ONOFF_TKIP, (void *)&intVal);
		apmib_get(MIB_WLAN_BAND, (void *)&wlanMode);
		if(intVal == 0 && (wlanMode==8 || wlanMode==10 || wlanMode==11))
			websWrite(wp, T("%s"),"-->");
		else
			websWrite(wp, T("%s"),"");
		
		return 0;
	}
	else if(!strcmp(argv[0],T("wlanband"))){
		apmib_get(MIB_WLAN_BAND, (void *)&intVal);
		
		websWrite(wp, T("%d"),intVal);
		
		return 0;
	}
	else if ( !strcmp(name, T("opMode")) ) {
		apmib_get( MIB_OP_MODE, (void *)&intVal);
		websWrite(wp, T("%d"),intVal);
		return 0;
	}
	else if ( !strcmp(name, T("pocketRouter_html_wan_hide_s")) ) {
		apmib_get( MIB_OP_MODE, (void *)&intVal);
#if defined(CONFIG_POCKET_ROUTER_SUPPORT)
		if(intVal == 0)
			websWrite(wp, T("%s"),"");
		else if(intVal == 1)
			websWrite(wp, T("%s"),"<!--");
#else
		websWrite(wp, T("%s"),"");
#endif			
		return 0;
	}	
	else if ( !strcmp(name, T("pocketRouter_html_wan_hide_e")) ) {
		apmib_get( MIB_OP_MODE, (void *)&intVal);
#if defined(CONFIG_POCKET_ROUTER_SUPPORT)
		if(intVal == 0)
			websWrite(wp, T("%s"),"");
		else if(intVal == 1)
			websWrite(wp, T("%s"),"-->");
#else
		websWrite(wp, T("%s"),"");
#endif			
		return 0;
	}
	else if ( !strcmp(name, T("pocketRouter_html_lan_hide_s")) ) {
		apmib_get( MIB_OP_MODE, (void *)&intVal);
#if defined(CONFIG_POCKET_ROUTER_SUPPORT)
		if(intVal == 1)
			websWrite(wp, T("%s"),"");
		else if(intVal == 0)
			websWrite(wp, T("%s"),"<!--");
#else
		websWrite(wp, T("%s"),"");
#endif			
		return 0;
	}	
	else if ( !strcmp(name, T("pocketRouter_html_lan_hide_e")) ) {
		apmib_get( MIB_OP_MODE, (void *)&intVal);
#if defined(CONFIG_POCKET_ROUTER_SUPPORT)
		if(intVal == 1)
			websWrite(wp, T("%s"),"");
		else if(intVal == 0)
			websWrite(wp, T("%s"),"-->");
#else
		websWrite(wp, T("%s"),"");
#endif			
		return 0;
	}
	else if(!strcmp(name, T("wlan_xTxR"))) // 0:non-pocketRouter; 3: Router; 2:Bridge AP; 1:Bridge Client
	{
		int chipVersion = getWLAN_ChipVersion();

		if(chipVersion == 1)
			return websWrite(wp, T("%s"),"1*1");
		else if(chipVersion == 2)
			return websWrite(wp, T("%s"),"2*2");
		else
			return websWrite(wp, T("%s"),"0*0");
	}
	else if ( !strcmp(name, T("ip-lan"))) {
		if ( getInAddr(BRIDGE_IF, IP_ADDR, (void *)&intaddr ) )
			return websWrite(wp, T("%s"), inet_ntoa(intaddr) );
		else{
			apmib_get( MIB_IP_ADDR,  (void *)buffer);
   			return websWrite(wp, T("%s"), inet_ntoa(*((struct in_addr *)buffer)));
		}
	}
	for(i=0 ;i < wlan_num ; i++){
		sprintf(buffer, "wlan%d-status", i);
		if ( !strcmp(name, buffer )) {
			wlan_idx = i ;
			sprintf(WLAN_IF, "wlan%d", i);
			return websWrite(wp,"");
		}
	}

 	return -1;
}

/////////////////////////////////////////////////////////////////////////////
int getIndex(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t *name, buffer[50];
	int chan, val;
	REG_DOMAIN_T domain;
	WEP_T wep;
	DHCP_T dhcp;
#ifdef HOME_GATEWAY
	OPMODE_T opmode=-1;
	char_t *iface=NULL;
#ifdef VPN_SUPPORT
	IPSECTUNNEL_T entry;
#endif
#endif

#ifdef UNIVERSAL_REPEATER
	int id;
#endif

   	if (ejArgs(argc, argv, T("%s"), &name) < 1) {
   		websError(wp, 400, T("Insufficient args\n"));
   		return -1;
   	}

   	if ( !strcmp(name, T("dhcp")) ) {
 		if ( !apmib_get( MIB_DHCP, (void *)&dhcp) )
			return -1;
		sprintf(buffer, "%d", (int)dhcp);
		ejSetResult(eid, buffer);
		return 0;
	}

  	else if ( !strcmp(name, T("dhcp-current")) ) {
   		if ( !apmib_get( MIB_DHCP, (void *)&dhcp) )
			return -1;
		if ( dhcp == DHCP_CLIENT && !isDhcpClientExist(BRIDGE_IF))
			dhcp = DHCP_DISABLED;	
		sprintf(buffer, "%d", (int)dhcp);
		ejSetResult(eid, buffer);
		return 0;
	}
 	else if ( !strcmp(name, T("stp")) ) {
   		if ( !apmib_get( MIB_STP_ENABLED, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}else if ( !strcmp(name, T("sch_enabled")) ) {
   		if ( !apmib_get( MIB_SCHEDULE_ENABLED, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
#ifdef HOME_GATEWAY
 	else if ( !strcmp(name, T("ntpEnabled")) ) {
   		if ( !apmib_get( MIB_NTP_ENABLED, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("DaylightSave")) ) {
   		if ( !apmib_get( MIB_DAYLIGHT_SAVE, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("ntpServerId")) ) {
   		if ( !apmib_get( MIB_NTP_SERVER_ID, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("wanDhcp")) ) {
		if ( !apmib_get( MIB_WAN_DHCP, (void *)&dhcp) )
			return -1;
		sprintf(buffer, "%d", (int)dhcp);
		ejSetResult(eid, buffer);
		return 0;
	}
#ifdef HOME_GATEWAY	
#ifdef ROUTE_SUPPORT
	else if ( !strcmp(name, T("nat_enabled")) ) {
		if ( !apmib_get( MIB_NAT_ENABLED, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
#endif	
#endif	
	else if ( !strcmp(name, T("wanDhcp-current")) ) {
		if ( !apmib_get( MIB_WAN_DHCP, (void *)&dhcp) )
			return -1;
  		if ( !apmib_get( MIB_OP_MODE, (void *)&opmode) )
			return -1;
		if(opmode == WISP_MODE)
			iface = WLAN_IF;
		else
			iface = WAN_IF;
		if ( dhcp == DHCP_CLIENT && !isDhcpClientExist(iface))
			dhcp = DHCP_DISABLED;
		sprintf(buffer, "%d", (int)dhcp);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("wanDNS")) ) {
		DNS_TYPE_T dns;
		if ( !apmib_get( MIB_WAN_DNS_MODE, (void *)&dns) )
			return -1;
		sprintf(buffer, "%d", (int)dns);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("pppConnectType")) ) {
		PPP_CONNECT_TYPE_T type;
		if ( !apmib_get( MIB_PPP_CONNECT_TYPE, (void *)&type) )
			return -1;
		sprintf(buffer, "%d", (int)type);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("pptpConnectType")) ) {
		PPP_CONNECT_TYPE_T type;
		if ( !apmib_get( MIB_PPTP_CONNECTION_TYPE, (void *)&type) )
			return -1;
		sprintf(buffer, "%d", (int)type);
		ejSetResult(eid, buffer);
		return 0;
	}
	
	else if ( !strcmp(name, T("l2tpConnectType")) ) {
		PPP_CONNECT_TYPE_T type;
		if ( !apmib_get( MIB_L2TP_CONNECTION_TYPE, (void *)&type) )
			return -1;
		sprintf(buffer, "%d", (int)type);
		ejSetResult(eid, buffer);
		return 0;
	}
    else if ( !strcmp(name, T("pppConnectStatus")) ) {
		sprintf(buffer, "%d", isConnectPPP());
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("portFwNum")) ) {
		if ( !apmib_get( MIB_PORTFW_NUM, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("ipFilterNum")) ) {
		if ( !apmib_get( MIB_IPFILTER_NUM, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("portFilterNum")) ) {
		if ( !apmib_get( MIB_PORTFILTER_NUM, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("macFilterNum")) ) {
		if ( !apmib_get( MIB_MACFILTER_NUM, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("urlFilterNum")) ) {
		if ( !apmib_get( MIB_URLFILTER_NUM, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("triggerPortNum")) ) {
		if ( !apmib_get( MIB_TRIGGERPORT_NUM, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}

#if defined(GW_QOS_ENGINE) || defined(QOS_BY_BANDWIDTH)
	else if ( !strcmp(name, T("qosEnabled"))) {
		if ( !apmib_get( MIB_QOS_ENABLED, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("qosAutoUplinkSpeed"))) {
		if ( !apmib_get( MIB_QOS_AUTO_UPLINK_SPEED, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("qosRuleNum")) ) {
		if ( !apmib_get( MIB_QOS_RULE_NUM, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("qosAutoDownlinkSpeed")) ) {
		if ( !apmib_get( MIB_QOS_AUTO_DOWNLINK_SPEED, (void *)&val) )
			return -1;
		
		if(val == 0)
			sprintf(buffer, "%s", "");
		else
			sprintf(buffer, "%s", "checked");
			
		return websWrite(wp, buffer);	
	}
#endif

#ifdef ROUTE_SUPPORT
	else if ( !strcmp(name, T("staticRouteNum")) ) {
		if ( !apmib_get( MIB_STATICROUTE_NUM, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
#endif
	else if ( !strcmp(name, T("portFwEnabled"))) {
		if ( !apmib_get( MIB_PORTFW_ENABLED, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("ipFilterEnabled"))) {
		if ( !apmib_get( MIB_IPFILTER_ENABLED, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("portFilterEnabled"))) {
		if ( !apmib_get( MIB_PORTFILTER_ENABLED, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("macFilterEnabled"))) {
		if ( !apmib_get( MIB_MACFILTER_ENABLED, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("triggerPortEnabled"))) {
		if ( !apmib_get( MIB_TRIGGERPORT_ENABLED, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
#ifdef ROUTE_SUPPORT
	else if ( !strcmp(name, T("staticRouteEnabled"))) {
		if ( !apmib_get( MIB_STATICROUTE_ENABLED, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
#endif
	else if ( !strcmp(name, T("dmzEnabled"))) {
		if ( !apmib_get( MIB_DMZ_ENABLED, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("upnpEnabled")) ) {
		if ( !apmib_get( MIB_UPNP_ENABLED, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", (int)val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("igmpproxyDisabled")) ) {
		if ( !apmib_get( MIB_IGMP_PROXY_DISABLED, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", (int)val);
		ejSetResult(eid, buffer);
		return 0;
	}
#ifdef ROUTE_SUPPORT
	else if ( !strcmp(name, T("ripEnabled")) ) {
                if ( !apmib_get( MIB_RIP_ENABLED, (void *)&val) )
                        return -1;
                sprintf(buffer, "%d", (int)val);
                ejSetResult(eid, buffer);
                return 0;
        }
	else if ( !strcmp(name, T("ripLanTx")) ) {
                if ( !apmib_get( MIB_RIP_LAN_TX, (void *)&val) )
                        return -1;
                sprintf(buffer, "%d", (int)val);
                ejSetResult(eid, buffer);
                return 0;
        }
	else if ( !strcmp(name, T("ripLanRx")) ) {
                if ( !apmib_get( MIB_RIP_LAN_RX, (void *)&val) )
                        return -1;
                sprintf(buffer, "%d", (int)val);
                ejSetResult(eid, buffer);
                return 0;
        }
#if 0 //unused        
	else if ( !strcmp(name, T("ripWanTx")) ) {
                if ( !apmib_get( MIB_RIP_WAN_TX, (void *)&val) )
                        return -1;
                sprintf(buffer, "%d", (int)val);
                ejSetResult(eid, buffer);
                return 0;
        }
	else if ( !strcmp(name, T("ripWanRx")) ) {
                if ( !apmib_get( MIB_RIP_WAN_RX, (void *)&val) )
                        return -1;
                sprintf(buffer, "%d", (int)val);
                ejSetResult(eid, buffer);
                return 0;
        }
#endif        
#endif //ROUTE
#endif
#ifdef HOME_GATEWAY
#ifdef VPN_SUPPORT
		else if ( !strcmp(name, T("ipsecTunnelNum")) ) {
                if ( !apmib_get( MIB_IPSECTUNNEL_NUM, (void *)&val) )
                        return -1;
                sprintf(buffer, "%d", val);
                ejSetResult(eid, buffer);
                return 0;
        }
        else if ( !strcmp(name, T("ipsecVpnEnabled"))) {
                if ( !apmib_get( MIB_IPSECTUNNEL_ENABLED, (void *)&val) )
                        return -1;
                sprintf(buffer, "%d", val);
                ejSetResult(eid, buffer);
                return 0;
        }
	else if ( !strcmp(name, T("ipsecNattEnabled"))) {
                if ( !apmib_get( MIB_IPSEC_NATT_ENABLED, (void *)&val) )
                        return -1;
                sprintf(buffer, "%d", val);
                ejSetResult(eid, buffer);
                return 0;
        }
	else if ( !strcmp(name, T("tunnelEnabled"))) {
                if ( getIpsecInfo(&entry) < 0)
			sprintf(buffer, "%d", 1); // default
		else
	        	sprintf(buffer, "%d", entry.enable );
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("ipsecLocalType"))) {
                if ( getIpsecInfo(&entry) < 0)
			sprintf(buffer, "%d", SUBNET_ADDR); // subnet Address default
		else
	        	sprintf(buffer, "%d", entry.lcType);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("ipsecRemoteType"))) {
                if ( getIpsecInfo(&entry) < 0)
			sprintf(buffer, "%d", SUBNET_ADDR); // subnet Address default
		else
	        	sprintf(buffer, "%d", entry.rtType);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("ipsecKeyMode"))) {
#if 0		//sc_yang
		int val ;
               if ((val= getVpnKeyMode()) != -1){
                       sprintf(buffer, "%d", (int) val ) ;
               } else{
#endif
		if ( getIpsecInfo(&entry) < 0)
			sprintf(buffer, "%d", IKE_MODE); // IKE mode
		else
			sprintf(buffer, "%d", entry.keyMode);

		ejSetResult(eid, buffer);
		return 0;
	}
/*
	else if ( !strcmp(name, T("ipsecEspAh"))) {
                if ( getIpsecInfo(&entry) < 0)
			sprintf(buffer, "%d", ESP_PROTO); // ESP
		else
	        	sprintf(buffer, "%d", entry.espAh);
		ejSetResult(eid, buffer);
		return 0;
	}
*/
	else if ( !strcmp(name, T("ipsecEspEncr"))) {
                if ( getIpsecInfo(&entry) < 0)
			sprintf(buffer, "%d", TRI_DES_ALGO); // 3DES
		else
	        	sprintf(buffer, "%d", entry.espEncr);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("ipsecEspAuth"))) {
                if ( getIpsecInfo(&entry) < 0)
			sprintf(buffer, "%d", MD5_ALGO); // MD5
		else
	        	sprintf(buffer, "%d", entry.espAuth);
		ejSetResult(eid, buffer);
		return 0;
	}
	/*else if ( !strcmp(name, T("ipsecAhAuth"))) {
                if ( getIpsecInfo(&entry) < 0)
			sprintf(buffer, "%d", MD5_ALGO); // MD5
		else
	        	sprintf(buffer, "%d", entry.ahAuth);
		ejSetResult(eid, buffer);
		return 0;
	}*/
	else if ( !strcmp(name, T("vpnConnectionType"))) {
                if ( getIpsecInfo(&entry) < 0)
			sprintf(buffer, "%d", RESPONDER); // responder
		else
	        	sprintf(buffer, "%d", entry.conType);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if( !strcmp(name, T("ikeConnectStatus"))){
                if ( getIpsecInfo(&entry) < 0){
			sprintf(buffer, "%d", 0);
		}
		else{
			if ( getConnStat(entry.connName) < 0)
				sprintf(buffer, "%d", 0);
			else
				sprintf(buffer, "%d",1);
		}
		ejSetResult(eid, buffer);
		return 0;
	}
	else if( !strcmp(name, T("ipsecLocalIdType"))){
                if ( getIpsecInfo(&entry) < 0)
			sprintf(buffer, "%d", 0);
		else
			sprintf(buffer, "%d",entry.lcIdType);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if( !strcmp(name, T("ipsecRemoteIdType"))){
                if ( getIpsecInfo(&entry) < 0)
			sprintf(buffer, "%d", 0);
		else
			sprintf(buffer, "%d",entry.rtIdType);

		ejSetResult(eid, buffer);
		return 0;
	}
	else if( !strcmp(name, T("ipsecAuthType"))){
                if ( getIpsecInfo(&entry) < 0)
			sprintf(buffer, "%d", 0);
		else
			sprintf(buffer, "%d", entry.authType);

		ejSetResult(eid, buffer);
		return 0;
	}
#endif
#endif
	else if ( !strcmp(name, T("channel")) ) {
		if ( !apmib_get( MIB_WLAN_CHAN_NUM, (void *)&chan) )
			return -1;
		sprintf(buffer, "%d", chan);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("regDomain")) ) {
		if ( !apmib_get( MIB_HW_REG_DOMAIN, (void *)&domain) )
			return -1;
		sprintf(buffer, "%d", (int)domain);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("wep")) ) {
		if ( !apmib_get( MIB_WLAN_WEP, (void *)&wep) )
			return -1;
		sprintf(buffer, "%d", (int)wep);
		ejSetResult(eid, buffer);
   	    	return 0;
	}
	else if ( !strcmp(name, T("defaultKeyId")) ) {
		if ( !apmib_get( MIB_WLAN_WEP_DEFAULT_KEY, (void *)&val) )
			return -1;
		val++;
		sprintf(buffer, "%d", (int)val) ;
		 ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("keyType")) ) {
		if ( !apmib_get( MIB_WLAN_WEP_KEY_TYPE, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", (int)val) ;
		 ejSetResult(eid, buffer);
		return 0;
	}
  	else if ( !strcmp(name, T("authType"))) {
		if ( !apmib_get( MIB_WLAN_AUTH_TYPE, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", (int)val) ;
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("operRate"))) {
		if ( !apmib_get( MIB_WLAN_SUPPORTED_RATE, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", (int)val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("basicRate"))) {
		if ( !apmib_get( MIB_WLAN_BASIC_RATE, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", (int)val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("preamble"))) {
		if ( !apmib_get( MIB_WLAN_PREAMBLE_TYPE, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("hiddenSSID"))) {
		if ( !apmib_get( MIB_WLAN_HIDDEN_SSID, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("wmFilterNum"))) {
		if ( !apmib_get( MIB_WLAN_AC_NUM, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("wlanDisabled"))) {
		if ( !apmib_get( MIB_WLAN_DISABLED, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("wlanAcNum")) ) {
		if ( !apmib_get( MIB_WLAN_AC_NUM, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("wlanAcEnabled"))) {
		if ( !apmib_get( MIB_WLAN_AC_ENABLED, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}

#if defined(CONFIG_RTK_MESH) && defined(_MESH_ACL_ENABLE_) // below code copy above ACL code
	else if ( !strcmp(name, T("meshAclNum")) ) {
		if ( !apmib_get( MIB_MESH_ACL_NUM, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("meshAclEnabled"))) {
		if ( !apmib_get( MIB_MESH_ACL_ENABLED, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
#endif

	else if ( !strcmp(name, T("rateAdaptiveEnabled"))) {
		if ( !apmib_get( MIB_WLAN_RATE_ADAPTIVE_ENABLED, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("wlanMode"))) {
		if ( !apmib_get( MIB_WLAN_MODE, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("networkType"))) {
		if ( !apmib_get( MIB_WLAN_NETWORK_TYPE, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("iappDisabled"))) {
		if ( !apmib_get( MIB_WLAN_IAPP_DISABLED, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("protectionDisabled"))) {
		if ( !apmib_get( MIB_WLAN_PROTECTION_DISABLED, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("encrypt"))) {
		if ( !apmib_get( MIB_WLAN_ENCRYPT, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("enable1X"))) {
		if ( !apmib_get( MIB_WLAN_ENABLE_1X, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("enableSuppNonWpa"))) {
		if ( !apmib_get( MIB_WLAN_ENABLE_SUPP_NONWPA, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("suppNonWpa"))) {
		if ( !apmib_get( MIB_WLAN_SUPP_NONWPA, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("wpaAuth"))) {
		if ( !apmib_get( MIB_WLAN_WPA_AUTH, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("wpaCipher"))) {
		if ( !apmib_get( MIB_WLAN_WPA_CIPHER_SUITE, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("wpa2Cipher"))) {
		if ( !apmib_get( MIB_WLAN_WPA2_CIPHER_SUITE, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("pskFormat"))) {
		if ( !apmib_get( MIB_WLAN_WPA_PSK_FORMAT, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("accountRsEnabled"))) {
		if ( !apmib_get( MIB_WLAN_ACCOUNT_RS_ENABLED, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("accountRsUpdateEnabled"))) {
		if ( !apmib_get( MIB_WLAN_ACCOUNT_UPDATE_ENABLED, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("enableMacAuth"))) {
		if ( !apmib_get( MIB_WLAN_ENABLE_MAC_AUTH, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("rsRetry")) ) {
		if ( !apmib_get( MIB_WLAN_RS_RETRY, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("accountRsRetry")) ) {
		if ( !apmib_get( MIB_WLAN_ACCOUNT_RS_RETRY, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("wlanWdsEnabled"))) {
		if ( !apmib_get( MIB_WLAN_WDS_ENABLED, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("wlanWdsNum"))) {
		if ( !apmib_get( MIB_WLAN_WDS_NUM, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("wdsEncrypt"))) {
		if ( !apmib_get( MIB_WLAN_WDS_ENCRYPT, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("wdsWepFormat"))) {
		if ( !apmib_get( MIB_WLAN_WDS_WEP_FORMAT, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("wdsPskFormat"))) {
		if ( !apmib_get( MIB_WLAN_WDS_PSK_FORMAT, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		 ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("RFType")) ) {
		if ( !apmib_get( MIB_HW_RF_TYPE, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", (int)val) ;
		 ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("band")) ) {
		if ( !apmib_get( MIB_WLAN_BAND, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", (int)val) ;
		 ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("fixTxRate")) ) {
		if ( !apmib_get( MIB_WLAN_FIX_RATE, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", (int)val) ;
		 ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("preAuth")) ) {
		if ( !apmib_get( MIB_WLAN_WPA2_PRE_AUTH, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", (int)val) ;
		 ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("turboMode")) ) {
		if ( !apmib_get( MIB_TURBO_MODE, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", (int)val) ;
		 ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("RFPower")) ) {
		if ( !apmib_get( MIB_WLAN_RF_POWER, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", (int)val) ;
		 ejSetResult(eid, buffer);
		return 0;
	}
	

#ifdef WLAN_EASY_CONFIG
	else if ( !strcmp(name, T("autoCfgEnabled"))) {
		if ( !apmib_get( MIB_WLAN_EASYCFG_ENABLED, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("autoCfgMode"))) {
		if ( !apmib_get( MIB_WLAN_EASYCFG_MODE, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("autoCfgKeyInstall"))) {
		char tmpbuf[100];
		if ( !apmib_get( MIB_WLAN_EASYCFG_KEY, (void *)&tmpbuf) )
			return -1;
		if (strlen(tmpbuf))
			val = 1;
		else
			val = 0;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("autoCfgDigestInstall"))) {
		char tmpbuf[100];
		int is_adhoc;
		if ( !apmib_get( MIB_WLAN_MODE, (void *)&val) )
			return -1;
		if (val == CLIENT_MODE) {
			apmib_get( MIB_WLAN_NETWORK_TYPE, (void *)&is_adhoc );
			if (is_adhoc) {
				apmib_get( MIB_WLAN_EASYCFG_MODE, (void *)&val);
				if (!(val & MODE_QUESTION))
					val = 2;
				else {
					apmib_get( MIB_WLAN_EASYCFG_DIGEST, (void *)&tmpbuf);
					if (strlen(tmpbuf))
						val = 1;
					else
						val = 0;
				}
			}
			else
				val = 2;
		}
		else {
			if ( !apmib_get( MIB_WLAN_EASYCFG_MODE, (void *)&val) )
				return -1;
			if (!(val & MODE_QUESTION))
				val = 2;
			else {
				if ( !apmib_get( MIB_WLAN_EASYCFG_DIGEST, (void *)&tmpbuf) )
					return -1;
				if (strlen(tmpbuf))
					val = 1;
				else
					val = 0;
			}
		}
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("autoCfgWlanMode"))) {
		if ( !apmib_get( MIB_WLAN_EASYCFG_WLAN_MODE, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
#endif // WLAN_EASY_CONFIG
#ifdef HOME_GATEWAY
	else if ( !strcmp(name, T("ddnsEnabled")) ) {
		if ( !apmib_get( MIB_DDNS_ENABLED, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", (int)val) ;
		 ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("ddnsType")) ) {
		if ( !apmib_get( MIB_DDNS_TYPE, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", (int)val) ;
		 ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("webWanAccess")) ) {
		if ( !apmib_get( WEB_WAN_ACCESS_ENABLED, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", (int)val) ;
		 ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("pingWanAccess")) ) {
		if ( !apmib_get( PING_WAN_ACCESS_ENABLED, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", (int)val) ;
		 ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("VPNPassThruIPsec")) ) {
		if ( !apmib_get( VPN_PASSTHRU_IPSEC_ENABLED, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", (int)val) ;
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("VPNPassThruPPTP")) ) {
		if ( !apmib_get( VPN_PASSTHRU_PPTP_ENABLED, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", (int)val) ;
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("VPNPassThruL2TP")) ) {
		if ( !apmib_get( VPN_PASSTHRU_L2TP_ENABLED, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", (int)val) ;
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("VPNPassThruIPv6")) ) {
		if ( !apmib_get( MIB_VPN_PASSTHRU_IPV6, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", (int)val) ;
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("urlFilterEnabled"))) {
		if ( !apmib_get( MIB_URLFILTER_ENABLED, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}

#endif
	else if ( !strcmp(name, T("wispWanId")) ) {
		if ( !apmib_get( MIB_WISP_WAN_ID, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", (int)val) ;
		 ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("opMode")) ) {
		if ( !apmib_get( MIB_OP_MODE, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", (int)val) ;
		 ejSetResult(eid, buffer);
		return 0;
	}

	else if ( !strcmp(name, T("wlan_num"))) {
		sprintf(buffer, "%d", wlan_num);
		ejSetResult(eid, buffer);
		return 0;
	}
#ifdef MBSSID	
	else if ( !strcmp(name, T("vwlan_num"))) {
		sprintf(buffer, "%d", vwlan_num);
		ejSetResult(eid, buffer);
		return 0;
	}
#endif	
	else if ( !strcmp(name, T("wlan_idx"))) {
		sprintf(buffer, "%d", wlan_idx);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("wlanMacClone"))) {
		if ( !apmib_get( MIB_WLAN_NAT25_MAC_CLONE, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("isWispDisplay"))) {
#if (defined(CONFIG_RTL8196C) && !defined(CONFIG_WLAN_CLIENT_MODE)) || defined(CONFIG_RTL8196C_CLIENT_ONLY)// keith. disabled if no this mode in 96c		
		sprintf(buffer,"%d", 0);
#elif (defined(CONFIG_RTL8198) && !defined(CONFIG_WLAN_CLIENT_MODE))// keith. disabled if no this mode in 96c				
		sprintf(buffer,"%d", 0);
#else		
		sprintf(buffer,"%d", 1);
#endif		
		ejSetResult(eid, buffer);
	}
	else if ( !strcmp(name, T("isRepeaterDisplay"))) {
#if (defined(CONFIG_RTL8196C) && !defined(CONFIG_WLAN_CLIENT_MODE)) || defined(CONFIG_RTL8196C_CLIENT_ONLY)// keith. disabled if no this mode in 96c
		sprintf(buffer,"%d", 0);
#elif (defined(CONFIG_RTL8198) && !defined(CONFIG_WLAN_CLIENT_MODE))// keith. disabled if no this mode in 96c
		sprintf(buffer,"%d", 0);		
#else
		sprintf(buffer,"%d", 1);
#endif
		ejSetResult(eid, buffer);
	}
else if ( !strcmp(name, T("isACLIncluded"))) {
#if defined(CONFIG_RTL8196C_CLIENT_ONLY)		
		sprintf(buffer,"%d", 0);
#else
		sprintf(buffer,"%d", 1);
#endif		
		ejSetResult(eid, buffer);
	}	
	else if ( !strcmp(name, T("isWDSDefined"))) {
#if defined(CONFIG_RTL8196C_CLIENT_ONLY)		
		sprintf(buffer,"%d", 0);
#else
		sprintf(buffer,"%d", 1);
#endif		
		ejSetResult(eid, buffer);
	}
	else if ( !strcmp(name, T("isWlanMenuStart"))) {
#if defined(CONFIG_NET_RADIO) // keith. disabled if no wlan
		sprintf(buffer,"%s", "");
#else
		sprintf(buffer,"%s", "/*");
#endif
		ejSetResult(eid, buffer);
	}
	else if ( !strcmp(name, T("isWlanMenuEnd"))) {
#if defined(CONFIG_NET_RADIO) // keith. disabled if no wlan
		sprintf(buffer,"%d", "");
#else
		sprintf(buffer,"%s", "*/");
#endif
		ejSetResult(eid, buffer);
	}
#ifdef CONFIG_RTK_MESH
	else if ( !strcmp(name, T("wlanMeshEnabled"))) {
				//new feature:Mesh enable/disable
                if ( !apmib_get( MIB_WLAN_MESH_ENABLE, (void *)&val) )
                        return -1;
                sprintf(buffer, "%d", val);
                ejSetResult(eid, buffer);
                return 0;
        }
        else if ( !strcmp(name, T("meshRootEnabled"))) {
                if ( !apmib_get( MIB_MESH_ROOT_ENABLE, (void *)&val) )
                        return -1;
                sprintf(buffer, "%d", val);
                ejSetResult(eid, buffer);
                return 0;
        }
		else if ( !strcmp(name, T("meshEncrypt"))) {
			if ( !apmib_get( MIB_MESH_ENCRYPT, (void *)&val) )
				return -1;
			sprintf(buffer, "%d", val);
			ejSetResult(eid, buffer);
			return 0;
		}
		else if ( !strcmp(name, T("meshPskFormat"))) {
			if ( !apmib_get( MIB_MESH_WPA_PSK_FORMAT, (void *)&val) )
				return -1;
			sprintf(buffer, "%d", val);
			ejSetResult(eid, buffer);
			return 0;
		}
	 	else if ( !strcmp(name, T("meshPskValue"))) {
			int i;
			buffer[0]='\0';
			if ( !apmib_get(MIB_MESH_WPA_PSK,  (void *)buffer) )
				return -1;
			for (i=0; i<strlen(buffer); i++)
				buffer[i]='*';
			buffer[i]='\0';
	   		return websWrite(wp, buffer);
		}
		else if ( !strcmp(name, T("meshWpaAuth"))) {
			if ( !apmib_get( MIB_MESH_WPA_AUTH, (void *)&val) )
				return -1;
			sprintf(buffer, "%d", val);
			ejSetResult(eid, buffer);
			return 0;
		}
		else if ( !strcmp(name, T("meshWpa2Cipher"))) {
			if ( !apmib_get( MIB_MESH_WPA2_CIPHER_SUITE, (void *)&val) )
				return -1;
			sprintf(buffer, "%d", val);
			ejSetResult(eid, buffer);
			return 0;
		}

#ifdef _MESH_ACL_ENABLE_
	else if ( !strcmp(name, T("meshAclEnabled"))) {
		if ( !apmib_get( MIB_MESH_ACL_ENABLED, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
#endif
#endif // CONFIG_RTK_MESH
	//indispensable!! MESH related , no matter mesh enable or not
	else if ( !strcmp(name, T("isMeshDefined"))) {
#ifdef CONFIG_RTK_MESH
		sprintf(buffer,"%d", 1);
#else
		sprintf(buffer,"%d", 0);
#endif
		ejSetResult(eid, buffer);
	}
	//indispensable!! MESH related , no matter mesh enable or not
	else if ( !strcmp(name, T("isNewMeshUI"))) {
#ifdef CONFIG_NEW_MESH_UI
		sprintf(buffer,"%d", 1);
#else
		sprintf(buffer,"%d", 0);
#endif
		ejSetResult(eid, buffer);
	}
	else if ( !strcmp(name, T("rtLogEnabled")) ) {
		if ( !apmib_get( MIB_REMOTELOG_ENABLED, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", (int)val) ;
		 ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("logEnabled")) ) {
		if ( !apmib_get( MIB_LOG_ENABLED, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", (int)val) ;
		 ejSetResult(eid, buffer);
		return 0;
	}
#ifdef TLS_CLIENT
	else if ( !strcmp(name, T("rootIdx"))) {
		if ( !apmib_get( MIB_ROOT_IDX, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("userIdx"))) {
		if ( !apmib_get( MIB_USER_IDX, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("rootNum"))) {
		if ( !apmib_get( MIB_CERTROOT_NUM, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("userNum"))) {
		if ( !apmib_get( MIB_CERTUSER_NUM, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}

#endif

#ifdef UNIVERSAL_REPEATER
	else if ( !strcmp(name, T("repeaterEnabled"))) {
		if (wlan_idx == 0)
			id = MIB_REPEATER_ENABLED1;
		else
			id = MIB_REPEATER_ENABLED2;
		if ( !apmib_get( id, (void *)&val) )
				return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("isRepeaterEnabled"))) {
#if 1
		int intVal, intVal2;
		if (wlan_idx == 0)
			apmib_get(MIB_REPEATER_ENABLED1, (void *)&intVal);
		else
			apmib_get(MIB_REPEATER_ENABLED2, (void *)&intVal);

		apmib_get(MIB_WLAN_NETWORK_TYPE, (void *)&intVal2);
		apmib_get(MIB_WLAN_MODE, (void *)&val);

		if (intVal != 0 && val != WDS_MODE && !(val==CLIENT_MODE && intVal2==ADHOC)) 
		{
			val = 1;
		}
		else
		{
			val = 0;
		}

#else		
		if (wlan_idx == 0)
			strcpy(buffer, "wlan0-vxd");
		else
			strcpy(buffer, "wlan1-vxd");
		if ( isVxdInterfaceExist(buffer))
			val = 1;
		else
			val = 0;
#endif
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("repeaterMode"))) {
		if ( !apmib_get( MIB_WLAN_MODE, (void *)&val) )
			return -1;
		if (val == AP_MODE || val == AP_WDS_MODE)
			val = CLIENT_MODE;
		else
			val = AP_MODE;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
#endif // UNIVERSAL_REPEATER
	else if ( !strcmp(name, T("WiFiTest"))) {
		apmib_get( MIB_WIFI_SPECIFIC, (void *)&val);
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
#ifdef HOME_GATEWAY
#ifdef DOS_SUPPORT
	else if ( !strcmp(name, T("dosEnabled"))) {
		if ( !apmib_get( MIB_DOS_ENABLED, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
#endif
	else if ( !strcmp(name, T("pptpSecurity")) ) {
		if ( !apmib_get( MIB_PPTP_SECURITY_ENABLED, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", (int)val) ;
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("pptpCompress")) ) {
		if ( !apmib_get( MIB_PPTP_MPPC_ENABLED, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", (int)val) ;
		ejSetResult(eid, buffer);
		return 0;
	}	
#endif

#ifdef WIFI_SIMPLE_CONFIG
	else if ( !strcmp(name, T("wscDisable"))) {
		apmib_get(MIB_WSC_DISABLE, (void *)&val);		
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}	
	else if ( !strcmp(name, T("wscConfig"))) {
		apmib_get(MIB_WSC_CONFIGURED, (void *)&val);		
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}	
	else if ( !strcmp(name, T("wps_by_reg"))) {
		apmib_get(MIB_WSC_CONFIG_BY_EXT_REG, (void *)&val);		
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("wps_auth"))) {
		apmib_get(MIB_WSC_AUTH, (void *)&val);		
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("wps_enc"))) {
		apmib_get(MIB_WSC_ENC, (void *)&val);		
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}	
#endif // WIFI_SIMPLE_CONFIG

// for WMM
	else if ( !strcmp(name, T("wmmEnabled"))) {
		if ( !apmib_get(MIB_WLAN_WMM_ENABLED, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
//for 11N
	else if ( !strcmp(name, T("ChannelBonding"))) {
		if ( !apmib_get(MIB_WLAN_CHANNEL_BONDING, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("ControlSideBand"))) {
		if ( !apmib_get(MIB_WLAN_CONTROL_SIDEBAND, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("aggregation"))) {
		if ( !apmib_get(MIB_WLAN_AGGREGATION, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("shortGIEnabled"))) {
		if ( !apmib_get(MIB_WLAN_SHORT_GI, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("static_dhcp"))) {
		if ( !apmib_get(MIB_DHCPRSVDIP_ENABLED, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("wlanAccess"))) {
		if ( !apmib_get(MIB_WLAN_ACCESS, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}	
	else if ( !strcmp(name, T("rf_used"))) {
		struct _misc_data_ misc_data;		
		if (getMiscData(WLAN_IF, &misc_data) < 0)
			return -1;
		sprintf(buffer, "%d", misc_data.mimo_tr_used);
		ejSetResult(eid, buffer);
		return 0;
	}	else if ( !strcmp(name, T("block_relay"))) {
		if ( !apmib_get( MIB_WLAN_BLOCK_RELAY, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}	else if ( !strcmp(name, T("tx_stbc"))) {
		if ( !apmib_get( MIB_WLAN_11N_STBC, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}	
	else if ( !strcmp(name, T("coexist"))) {
		if ( !apmib_get( MIB_WLAN_11N_COEXIST, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", val);
		ejSetResult(eid, buffer);
		return 0;
	}	
#ifdef MBSSID
	else if ( !strcmp(name, T("mssid_idx")) ) {
		sprintf(buffer, "%d", mssid_idx);
		ejSetResult(eid, buffer);
		return 0;
	}
#endif	
#if defined(CONFIG_RTL8196C_CLIENT_ONLY) && !defined(MBSSID)
	else if ( !strcmp(name, T("mssid_idx")) ) {
		sprintf(buffer, "%d", 0);
		ejSetResult(eid, buffer);
		return 0;
	}
#endif	
	else if ( !strcmp(name, T("wlan_mssid_num")) ) {
#if defined(CONFIG_RTL8196C) && !defined(CONFIG_WLAN_VAP_SUPPORT)// keith. disabled if no this mode in 96c
		int mssid_num=0; 
#elif defined(CONFIG_RTL8198) && !defined(CONFIG_WLAN_VAP_SUPPORT)// keith. disabled if no this mode in 96c
		int mssid_num=0;		
#else		
//		#if defined(CONFIG_RTL8196B)//we disable mssid first for 96b
//		int mssid_num=0;
//		#else
#if	defined(CONFIG_RTL8196B_GW_8M) || defined(CONFIG_RTL8196C_AP_ROOT)
		int mssid_num=1; 
#elif defined(CONFIG_RTL8196C_CLIENT_ONLY)		
		int mssid_num=0; 
#elif	defined(CONFIG_RTL8198_AP_ROOT)
		int mssid_num=4; 		
#else
		int mssid_num=4; 
#endif
//		#endif
#endif //#if defined(CONFIG_RTL8196C) && !defined(CONFIG_WLAN_VAP_SUPPORT)
		sprintf(buffer, "%d", mssid_num);
		ejSetResult(eid, buffer);
		return 0;
	}
#ifdef CONFIG_RTL_WAPI_SUPPORT
	else if ( !strcmp(name, T("wapiUcastReKeyType")) ) {
		if ( !apmib_get(MIB_WLAN_WAPI_UCAST_REKETTYPE, (void *)&val) )
			return -1;
		if(0 == val)
		{
			/*default should be off*/
			val = 1;
		}
		sprintf(buffer, "%d", (int)val) ;
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("wapiMcastReKeyType")) ) {
		if ( !apmib_get(MIB_WLAN_WAPI_MCAST_REKEYTYPE, (void *)&val) )
			return -1;
		if(0 == val)
		{
			/*default should be off*/
			val = 1;
		}
		sprintf(buffer, "%d", (int)val) ;
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("wapiAuth")) ) {
		if ( !apmib_get(MIB_WLAN_WAPI_AUTH, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", (int)val) ;
		ejSetResult(eid, buffer);
		return 0;
	}
	else if ( !strcmp(name, T("wapiPskFormat")) ) {
		if ( !apmib_get( MIB_WLAN_WAPI_PSK_FORMAT, (void *)&val) )
			return -1;
		sprintf(buffer, "%d", (int)val) ;
		 ejSetResult(eid, buffer);
		return 0;
	}
	else if(!strcmp(name,T("wapiSearchIndex"))){
		if(!apmib_get(MIB_WLAN_WAPI_SEARCH_CERTINDEX,(void *)&val))
			return -1;
		sprintf(buffer, "%d", (int)val) ;
		ejSetResult(eid, buffer);
		return 0;		
	}
#else
	else if(!strncmp(name,T("wapi"),4)){
		/*wapi not support*/
		return 0;		
	}
#endif	
	else
		return -1;

	return 0;
}

#ifdef MBSSID

int getVirtualIndex(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret, old;
	char WLAN_IF_old[40];

	old = vwlan_idx;
	vwlan_idx = atoi(argv[--argc]);

//#if defined(CONFIG_RTL8196B)
//	if (vwlan_idx == 5) { //rtl8196b support repeater mode only first, no mssid
//#else
	if (vwlan_idx > 0) {
//#endif
		strcpy(WLAN_IF_old, WLAN_IF);
		sprintf(WLAN_IF, "%s-va%d", WLAN_IF_old, vwlan_idx-1);
	}	

	ret = getIndex(eid, wp, argc, argv);

//#if defined(CONFIG_RTL8196B)
//	if (vwlan_idx == 5) 
//#else
	if (vwlan_idx > 0) 
//#endif
		strcpy(WLAN_IF, WLAN_IF_old);

	vwlan_idx = old;
	return ret;
}

int getVirtualInfo(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret, old;
	char WLAN_IF_old[40];

	old = vwlan_idx;
	vwlan_idx = atoi(argv[--argc]);
	
//#if defined(CONFIG_RTL8196B)
//	if (vwlan_idx == 5) { //rtl8196b support repeater mode only first, no mssid
//#else
	if (vwlan_idx > 0) {
//#endif
		strcpy(WLAN_IF_old, WLAN_IF);
		sprintf(WLAN_IF, "%s-va%d", WLAN_IF_old, vwlan_idx-1);
	}	

	ret = getInfo(eid, wp, argc, argv);

//#if defined(CONFIG_RTL8196B)
//	if (vwlan_idx == 5) 
//#else
	if (vwlan_idx > 0) 
//#endif
		strcpy(WLAN_IF, WLAN_IF_old);

	vwlan_idx = old;
	return ret;
}


#endif
#if defined(CONFIG_RTL8196C_CLIENT_ONLY) && !defined(MBSSID)
int getVirtualInfo(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret;
	
	ret = getInfo(eid, wp, argc, argv);


	return ret;
}

int getVirtualIndex(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret;
	ret = getIndex(eid, wp, argc, argv);
	return ret;
}
#endif // defined(CONFIG_RTL8196C_CLIENT_ONLY) && !defined(MBSSID)

#ifdef HOME_GATEWAY
/////////////////////////////////////////////////////////////////////////////
int isConnectPPP()
{
	struct stat status;

	if ( stat("/etc/ppp/link", &status) < 0)
		return 0;

	return 1;
}
#endif
int getDHCPModeCombobox(int eid, webs_t wp, int argc, char_t **argv)
{
	int val = 0;
	int lan_dhcp_mode=0;
	int operation_mode=0;
	apmib_get( MIB_WLAN_MODE, (void *)&val);
	apmib_get(MIB_DHCP,(void *)&lan_dhcp_mode);
	apmib_get( MIB_OP_MODE, (void *)&operation_mode);
#if defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT)	
        if((operation_mode==1 && (val==0 ||val==1)) || (operation_mode==0)){
	       if(lan_dhcp_mode == 0){
	 		return websWrite(wp,T("<option selected value=\"0\">Disabled</option>"
	 							"<option value=\"1\">Client</option>"
	 							 "<option value=\"2\">Server</option>"
	 							  "<option value=\"15\">Auto</option>"));
	      	  }
		if(lan_dhcp_mode == 1){
	 		return websWrite(wp,T("<option  value=\"0\">Disabled</option>"
	 							"<option selected value=\"1\">Client</option>"
	 							 "<option value=\"2\">Server</option>"
	 							  "<option value=\"15\">Auto</option>"));
	      	  }
		if(lan_dhcp_mode == 2){
	 		return websWrite(wp,T("<option  value=\"0\">Disabled</option>"
	 							"<option  value=\"1\">Client</option>"
	 							 "<option selected value=\"2\">Server</option>"
	 							  "<option value=\"15\">Auto</option>"));
	      	  }
	       if(lan_dhcp_mode == 15){
	 		return websWrite(wp,T("<option  value=\"0\">Disabled</option>"
	 							"<option  value=\"1\">Client</option>"
	 							 "<option value=\"2\">Server</option>"
	 							 "<option selected value=\"15\">Auto</option>"));
	      	  }
    	}
#else
 	if(lan_dhcp_mode == 0){
 		return websWrite(wp,T("<option selected value=\"0\">Disabled</option>"
 							"<option value=\"1\">Client</option>"
 							 "<option value=\"2\">Server</option>"));
      	  }
	if(lan_dhcp_mode == 1){
 		return websWrite(wp,T("<option  value=\"0\">Disabled</option>"
 							"<option selected value=\"1\">Client</option>"
 							 "<option value=\"2\">Server</option>"));
      	  }
	if(lan_dhcp_mode == 2){
 		return websWrite(wp,T("<option  value=\"0\">Disabled</option>"
 							"<option  value=\"1\">Client</option>"
 							 "<option selected value=\"2\">Server</option>"));
      	  }
#endif       
}
int getModeCombobox(int eid, webs_t wp, int argc, char_t **argv)
{
	int val = 0;
	int opmode;
	apmib_get( MIB_OP_MODE, (void *)&opmode);
	
	if ( !apmib_get( MIB_WLAN_MODE, (void *)&val) )
			return -1;
		
#ifdef CONFIG_RTK_MESH	
#ifdef CONFIG_NEW_MESH_UI
	  if ( val == 0 ) {
      	  	return websWrite(wp,T( "<option selected value=\"0\">AP</option>" 
   	  	 "<option value=\"1\">Client</option>"
   	  	 "<option value=\"2\">WDS</option>"
   	  	 "<option value=\"3\">AP+WDS</option>"
   	  	 "<option value=\"4\">AP+MESH</option>"
   	  	 "<option value=\"5\">MESH</option>"  ));
      	  }
	  if ( val == 1 ) {
     	  	 return websWrite(wp,T("<option value=\"0\">AP</option>" 
   	  	 "<option selected value=\"1\">Client </option>" 
   	  	 "<option value=\"2\">WDS</option>" 
   	  	 "<option value=\"3\">AP+WDS</option>" 
   	  	 "<option value=\"4\">AP+MESH</option>" 
   	  	 "<option value=\"5\">MESH</option>"  ));
      	  }
	  if ( val == 2 ) {
     	  	 return websWrite(wp,T("<option value=\"0\">AP</option>" 
   	  	 "<option value=\"1\">Client </option>" 
 	  	 "<option selected value=\"2\">WDS</option>" 
   	  	 "<option value=\"3\">AP+WDS</option>" 
   	  	 "<option value=\"4\">AP+MESH</option>" 
   	  	 "<option value=\"5\">MESH</option>"  ));
   	  }	
	  if ( val == 3 ) {
     	  	 return websWrite(wp,T("<option value=\"0\">AP</option>" 
   	  	 "<option value=\"1\">Client </option>" 
 	  	 "<option  value=\"2\">WDS</option>" 
   	  	 "<option selected value=\"3\">AP+WDS</option>"    	  	
   	  	 "<option value=\"4\">AP+MESH</option>" 
   	  	 "<option value=\"5\">MESH</option>"  ));
   	  } 
   	  if ( val == 4 ) {
		 return websWrite(wp,T("<option value=\"0\">AP</option>" 
   	  	 "<option value=\"1\">Client</option>" 
   	  	 "<option value=\"2\">WDS</option>" 
   	  	 "<option value=\"3\">AP+WDS</option>" 
   	  	 "<option selected value=\"4\">AP+MESH</option>" 
   	  	 "<option value=\"5\">MESH</option>"  ));
   	  }
   	  if ( val == 5 ) {
		 return websWrite(wp,T("<option value=\"0\">AP</option>" 
   	  	 "<option value=\"1\">Client</option>" 
   	  	 "<option value=\"2\">WDS</option>" 
   	  	 "<option value=\"3\">AP+WDS</option>" 
   	  	 "<option value=\"4\">AP+MESH</option>" 
   	  	 "<option selected value=\"5\">MESH</option>"  ));
   	  } 
	  else
	  return 0;

#else
  	if ( val == 0 ) {
      	  	return websWrite(wp,T( "<option selected value=\"0\">AP</option>" 
   	  	 "<option value=\"1\">Client</option>"
   	  	 "<option value=\"2\">WDS</option>"
   	  	 "<option value=\"3\">AP+WDS</option>"
   	  	 "<option value=\"4\">AP+MPP</option>"
   	  	 "<option value=\"5\">MPP</option>"
   	  	 "<option value=\"6\">MAP</option>"
   	  	 "<option value=\"7\">MP</option>" ));
      	  }
	  if ( val == 1 ) {
     	  	 return websWrite(wp,T("<option value=\"0\">AP</option>" 
   	  	 "<option selected value=\"1\">Client </option>" 
   	  	 "<option value=\"2\">WDS</option>" 
   	  	 "<option value=\"3\">AP+WDS</option>" 
   	  	 "<option value=\"4\">AP+MPP</option>" 
   	  	 "<option value=\"5\">MPP</option>" 
   	  	 "<option value=\"6\">MAP</option>" 
   	  	 "<option value=\"7\">MP</option>"  ));
      	  }
	  if ( val == 2 ) {
     	  	 return websWrite(wp,T("<option value=\"0\">AP</option>" 
   	  	 "<option value=\"1\">Client </option>" 
 	  	 "<option selected value=\"2\">WDS</option>" 
   	  	 "<option value=\"3\">AP+WDS</option>" 
   	  	 "<option value=\"4\">AP+MPP</option>" 
   	  	 "<option value=\"5\">MPP</option>" 
   	  	 "<option value=\"6\">MAP</option>" 
   	  	 "<option value=\"7\">MP</option>"  ));
   	  }	
	  if ( val == 3 ) {
     	  	 return websWrite(wp,T("<option value=\"0\">AP</option>" 
   	  	 "<option value=\"1\">Client </option>" 
 	  	 "<option  value=\"2\">WDS</option>" 
   	  	 "<option selected value=\"3\">AP+WDS</option>"    	  	
   	  	 "<option value=\"4\">AP+MPP</option>" 
   	  	 "<option value=\"5\">MPP</option>" 
   	  	 "<option value=\"6\">MAP</option>" 
   	  	 "<option value=\"7\">MP</option>"  ));
   	  } 
   	  if ( val == 4 ) {
		 return websWrite(wp,T("<option value=\"0\">AP</option>" 
   	  	 "<option value=\"1\">Client</option>" 
   	  	 "<option value=\"2\">WDS</option>" 
   	  	 "<option value=\"3\">AP+WDS</option>" 
   	  	 "<option selected value=\"4\">AP+MPP</option>" 
   	  	 "<option value=\"5\">MPP</option>" 
   	  	 "<option value=\"6\">MAP</option>" 
   	  	 "<option value=\"7\">MP</option>"  ));
   	  }
   	  if ( val == 5 ) {
		 return websWrite(wp,T("<option value=\"0\">AP</option>" 
   	  	 "<option value=\"1\">Client</option>" 
   	  	 "<option value=\"2\">WDS</option>" 
   	  	 "<option value=\"3\">AP+WDS</option>" 
   	  	 "<option value=\"4\">AP+MPP</option>" 
   	  	 "<option selected value=\"5\">MPP</option>" 
   	  	 "<option value=\"6\">MAP</option>" 
   	  	 "<option value=\"7\">MP</option>"  ));
   	  } 
   	   if ( val == 6 ) {
		 return websWrite(wp,T("<option value=\"0\">AP</option>" 
   	  	 "<option value=\"1\">Client</option>" 
   	  	 "<option value=\"2\">WDS</option>" 
   	  	 "<option value=\"3\">AP+WDS</option>" 
   	  	 "<option value=\"4\">AP+MPP</option>" 
   	  	 "<option value=\"5\">MPP</option>" 
   	  	 "<option selected value=\"6\">MAP</option>" 
   	  	 "<option value=\"7\">MP</option>"  ));	
   	  } 
   	   if ( val == 7 ) {
		 return websWrite(wp,T("<option value=\"0\">AP</option>" 
   	  	 "<option value=\"1\">Client</option>" 
   	  	 "<option value=\"2\">WDS</option>" 
   	  	 "<option value=\"3\">AP+WDS</option>" 
   	  	 "<option value=\"4\">AP+MPP</option>" 
   	  	 "<option value=\"5\">MPP</option>" 
   	  	 "<option value=\"6\">MAP</option>" 
   	  	 "<option selected  value=\"7\">MP</option>" ));	
   	}
	else
   	return 0;
#endif
#else

  	if ( val == 0 ) {
  		unsigned char tmp[300];
  		memset(tmp,0x00,sizeof(tmp));
  		sprintf(tmp,"%s",T("<option selected value=\"0\">AP</option>"));
#if defined(CONFIG_RTL8196C) && !defined(CONFIG_WLAN_CLIENT_MODE)// keith. disabled if no this mode in 96c				

#elif defined(CONFIG_RTL8198) && !defined(CONFIG_WLAN_CLIENT_MODE)// keith. disabled if no this mode in 96c				

#else

#if defined(CONFIG_POCKET_ROUTER_SUPPORT)
	if(opmode == BRIDGE_MODE && val == CLIENT_MODE)
	{
   		strcat(tmp,T("<option value=\"1\">Client</option>"));
	}
	else
	{

	}
#else
	strcat(tmp,T("<option value=\"1\">Client</option>"));
#endif //#if defined(CONFIG_POCKET_ROUTER_SUPPORT)

#endif   	  	 

#if defined(CONFIG_RTL8196C) && !defined(CONFIG_WLAN_WDS_SUPPORT)// keith. disabled if no this mode in 96c

#elif defined(CONFIG_RTL8198) && !defined(CONFIG_WLAN_WDS_SUPPORT)// keith. disabled if no this mode in 96c

#else
   	  strcat(tmp,T("<option value=\"2\">WDS</option>"
   	  	 "<option value=\"3\">AP+WDS</option>"    ));
#endif   	  	 
      return websWrite(wp,tmp);
      	  }
		
	  if ( val == 1 ) {
	  	unsigned char tmp[300];
  		memset(tmp,0x00,sizeof(tmp));
#if defined(CONFIG_RTL8196C_CLIENT_ONLY) 	
		sprintf(tmp,"%s",T("<option selected value=\"1\">Client</option>"));
#else
  		sprintf(tmp,"%s",T("<option value=\"0\">AP</option>"));
#if defined(CONFIG_RTL8196C) && !defined(CONFIG_WLAN_CLIENT_MODE)// keith. disabled if no this mode in 96c			 	

#elif defined(CONFIG_RTL8198) && !defined(CONFIG_WLAN_CLIENT_MODE)// keith. disabled if no this mode in 96c			 	

#else
   	  strcat(tmp,T("<option selected value=\"1\">Client</option>"));
#endif   	  	 

#if defined(CONFIG_RTL8196C) && !defined(CONFIG_WLAN_WDS_SUPPORT)// keith. disabled if no this mode in 96c

#elif defined(CONFIG_RTL8198) && !defined(CONFIG_WLAN_WDS_SUPPORT)// keith. disabled if no this mode in 96c

#else
   	  strcat(tmp,T("<option value=\"2\">WDS</option>"
   	  	 "<option value=\"3\">AP+WDS</option>"     ));
#endif   	  	 
#endif
      return websWrite(wp,tmp);
      	  }
     	  	 
	  if ( val == 2 ) {
     	unsigned char tmp[300];
  		memset(tmp,0x00,sizeof(tmp));
  		sprintf(tmp,"%s",T("<option value=\"0\">AP</option>"));
#if defined(CONFIG_RTL8196C) && !defined(CONFIG_WLAN_CLIENT_MODE)// keith. disabled if no this mode in 96c			 	

#elif defined(CONFIG_RTL8198) && !defined(CONFIG_WLAN_CLIENT_MODE)// keith. disabled if no this mode in 96c			 	

#else

#if defined(CONFIG_POCKET_ROUTER_SUPPORT)
	if(opmode == BRIDGE_MODE && val == CLIENT_MODE)
	{
   		strcat(tmp,T("<option value=\"1\">Client</option>"));	
	}
	else
	{

	}
#else
	strcat(tmp,T("<option value=\"1\">Client</option>"));
#endif //#if defined(CONFIG_POCKET_ROUTER_SUPPORT)

#endif   	  	 

#if defined(CONFIG_RTL8196C) && !defined(CONFIG_WLAN_WDS_SUPPORT)// keith. disabled if no this mode in 96c

#elif defined(CONFIG_RTL8198) && !defined(CONFIG_WLAN_WDS_SUPPORT)// keith. disabled if no this mode in 96c

#else
   	  strcat(tmp,T("<option selected value=\"2\">WDS</option>"
   	  	 "<option value=\"3\">AP+WDS</option>"    ));
#endif
      return websWrite(wp,tmp);
   	  }	
	  if ( val == 3 ) {
     	unsigned char tmp[300];
  		memset(tmp,0x00,sizeof(tmp));
  		sprintf(tmp,"%s",T("<option value=\"0\">AP</option>"));
#if defined(CONFIG_RTL8196C) && !defined(CONFIG_WLAN_CLIENT_MODE)// keith. disabled if no this mode in 96c			 	

#elif defined(CONFIG_RTL8198) && !defined(CONFIG_WLAN_CLIENT_MODE)// keith. disabled if no this mode in 96c			 	

#else			 	

#if defined(CONFIG_POCKET_ROUTER_SUPPORT)
	if(opmode == BRIDGE_MODE && val == CLIENT_MODE)
	{
   		strcat(tmp,T("<option value=\"1\">Client</option>"));	
	}
	else
	{

	}
#else
	strcat(tmp,T("<option value=\"1\">Client</option>"));
#endif //#if defined(CONFIG_POCKET_ROUTER_SUPPORT)

#endif   	  	 

#if defined(CONFIG_RTL8196C) && !defined(CONFIG_WLAN_WDS_SUPPORT)// keith. disabled if no this mode in 96c

#elif defined(CONFIG_RTL8198) && !defined(CONFIG_WLAN_WDS_SUPPORT)// keith. disabled if no this mode in 96c

#else
   	  strcat(tmp,T("<option value=\"2\">WDS</option>"
   	  	 "<option selected value=\"3\">AP+WDS</option>"   ));
#endif
      return websWrite(wp,tmp);
   	  } else  	  	
   	  	return 0;
#endif	  
}

