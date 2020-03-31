/*
 *      Web server handler routines for TCP/IP stuffs
 *
 *      Authors: David Hsu	<davidhsu@realtek.com.tw>
 *
 *      $Id: fmtcpip.c,v 1.33.2.1 2011/01/17 08:26:53 keith_huang Exp $
 *
 */

/*-- System inlcude files --*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <time.h>
#include <net/route.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <net/if.h>

/*-- Local inlcude files --*/
#include "../webs.h"
#ifdef HOME_GATEWAY
#include "../wsIntrn.h"
#endif
#include "apmib.h"
#include "apform.h"
#include "utility.h"

#ifdef __i386__
#define _LITTLE_ENDIAN_
#endif

#define _DHCPD_PROG_NAME	T("udhcpd")
#define _DHCPD_PID_PATH		T("/var/run")
#define _DHCPC_PROG_NAME	T("udhcpc")
#define _DHCPC_PID_PATH		T("/etc/udhcpc")
#define _PATH_DHCPS_LEASES	T("/var/lib/misc/udhcpd.leases")


/*-- Macro declarations --*/
#ifdef _LITTLE_ENDIAN_
#define ntohdw(v) ( ((v&0xff)<<24) | (((v>>8)&0xff)<<16) | (((v>>16)&0xff)<<8) | ((v>>24)&0xff) )

#else
#define ntohdw(v) (v)
#endif

#define RECONNECT_MSG(url) { \
	websHeader(wp); \
	websWrite(wp, T("<body><blockquote><h4>Change setting successfully!<BR><BR>If IP address was modified, you have to re-connect the WebServer" \
		"<BR>with the new address.<BR><BR>" \
                "<form><input type=button value=\"  OK  \" OnClick=window.location.replace(\"%s\")></form></blockquote></body>"), url);\
	websFooter(wp); \
	websDone(wp, 200); \
}

/*-- Forward declarations --*/
static int getOneDhcpClient(char **ppStart, unsigned long *size, char *ip, char *mac, char *liveTime);

#if 0
static DHCP_T wanDhcpTmp=(DHCP_T)-1;
#endif

//////////////////////////////////////////////////////////////////////////////
static int isValidName(char *str)
{
	int i, len=strlen(str);

	for (i=0; i<len; i++) {
		if (str[i] == ' ' || str[i] == '"' || str[i] == '\x27' || str[i] == '\x5c' || str[i] == '$')
			return 0;
	}
	return 1;
}

//////////////////////////////////////////////////////////////////////////////
static int getOneDhcpClient(char **ppStart, unsigned long *size, char *ip, char *mac, char *liveTime)
{
	struct dhcpOfferedAddr {
        	u_int8_t chaddr[16];
        	u_int32_t yiaddr;       /* network order */
        	u_int32_t expires;      /* host order */
	};

	struct dhcpOfferedAddr entry;
	 u_int8_t empty_haddr[16]; 
    
     	memset(empty_haddr, 0, 16); 
	if ( *size < sizeof(entry) )
		return -1;

	entry = *((struct dhcpOfferedAddr *)*ppStart);
	*ppStart = *ppStart + sizeof(entry);
	*size = *size - sizeof(entry);

	if (entry.expires == 0)
		return 0;

	if(!memcmp(entry.chaddr, empty_haddr, 16)){
		//fprintf(stderr, "got a unavailable entry for ip=%s\n",inet_ntoa(*((struct in_addr *)&entry.yiaddr)));
		return 0;
	}
	strcpy(ip, inet_ntoa(*((struct in_addr *)&entry.yiaddr)) );
	snprintf(mac, 20, "%02x:%02x:%02x:%02x:%02x:%02x",
			entry.chaddr[0],entry.chaddr[1],entry.chaddr[2],entry.chaddr[3],
			entry.chaddr[4], entry.chaddr[5]);

	if(entry.expires == 0xffffffff)
		sprintf(liveTime,"%s", "Always");
	else
		snprintf(liveTime, 10, "%lu", (unsigned long)ntohl(entry.expires));

	return 1;
}


///////////////////////////////////////////////////////////
int getPid(char *filename)
{
	struct stat status;
	char buff[100];
	FILE *fp;

	if ( stat(filename, &status) < 0)
		return -1;
	fp = fopen(filename, "r");
	if (!fp) {
        	error(E_L, E_LOG, T("Read pid file error!\n"));
		return -1;
   	}
	fgets(buff, 100, fp);
	fclose(fp);

	return (atoi(buff));
}
int tcpipLanHandler(webs_t wp, char *tmpBuf)
{
	char_t	*strIp, *strMask, *strGateway, *strDhcp, *strStp, *strMac, *strDNS, *strDomain, *strDhcpLeaseTime;
	struct in_addr inIp, inMask, inGateway;
	DHCP_T dhcp, curDhcp;	
	
	int stp;
	char_t	*strdhcpRangeStart, *strdhcpRangeEnd;
	struct in_addr dhcpRangeStart, dhcpRangeEnd;
	struct in_addr dns1, dns2, dns3;	
	int call_from_wizard = 0;
	int lan_dhcp_mode=0;
	strDhcp = websGetVar(wp, T("dhcp"), T(""));
	if (!strDhcp[0])
		call_from_wizard = 1;

	// Set STP
	strStp = websGetVar(wp, T("stp"), T(""));
	if (strStp[0]) {
		if (strStp[0] == '0')
			stp = 0;
		else
			stp = 1;
		if ( !apmib_set(MIB_STP_ENABLED, (void *)&stp)) {
			strcpy(tmpBuf, T("Set STP mib error!"));
			goto setErr_tcpip;
		}
	}

#if 0 // Move to formStaticDHCP
	// Set static DHCP
	strStp = websGetVar(wp, T("static_dhcp"), T(""));
	if (strStp[0]) {
		if (strStp[0] == '0')
			stp = 0;
		else
			stp = 1;
		if ( !apmib_set(MIB_DHCPRSVDIP_ENABLED, (void *)&stp)) {
			strcpy(tmpBuf, T("Set static DHCP mib error!"));
			goto setErr_tcpip;
		}
	}
#endif

	// Set clone MAC address
	strMac = websGetVar(wp, T("lan_macAddr"), T(""));
	if (strMac[0]) {
		if (strlen(strMac)!=12 || !string_to_hex(strMac, tmpBuf, 12)) {
			strcpy(tmpBuf, T("Error! Invalid MAC address."));
			goto setErr_tcpip;
		}
		if ( !apmib_set(MIB_ELAN_MAC_ADDR, (void *)tmpBuf)) {
			strcpy(tmpBuf, T("Set MIB_ELAN_MAC_ADDR mib error!"));
			goto setErr_tcpip;
		}
		if ( !apmib_set(MIB_WLAN_MAC_ADDR, (void *)tmpBuf)) {
			strcpy(tmpBuf, T("Set MIB_WLAN_MAC_ADDR mib error!"));
			goto setErr_tcpip;
		}
	}

	// Read current DHCP setting for reference later
	if ( !apmib_get( MIB_DHCP, (void *)&curDhcp) ) {
		strcpy(tmpBuf, T("Get DHCP MIB error!"));
		goto setErr_tcpip;
	}

	strDhcp = websGetVar(wp, T("dhcp"), T(""));
	if ( strDhcp[0] ) {
		lan_dhcp_mode = atoi(strDhcp);
		
		if(lan_dhcp_mode != 0 && lan_dhcp_mode != 1 && lan_dhcp_mode != 2 && lan_dhcp_mode != 15){
			strcpy(tmpBuf, T("Invalid DHCP value!"));
			goto setErr_tcpip;
		}

		if ( !apmib_set(MIB_DHCP, (void *)&lan_dhcp_mode)) {
	  		strcpy(tmpBuf, T("Set DHCP error!"));
			goto setErr_tcpip;
		}
		dhcp = lan_dhcp_mode;
	}
	else
		dhcp = curDhcp;

	if ( dhcp == DHCP_DISABLED || dhcp == DHCP_SERVER || dhcp == DHCP_AUTO ) {
		strIp = websGetVar(wp, T("lan_ip"), T(""));
		if ( strIp[0] ) {
			if ( !inet_aton(strIp, &inIp) ) {
				strcpy(tmpBuf, T("Invalid IP-address value!"));
				goto setErr_tcpip;
			}
			if ( !apmib_set( MIB_IP_ADDR, (void *)&inIp)) {
				strcpy(tmpBuf, T("Set IP-address error!"));
				goto setErr_tcpip;
			}
		}
		else { // get current used IP
			if ( !getInAddr(BRIDGE_IF, IP_ADDR, (void *)&inIp) ) {
				strcpy(tmpBuf, T("Get IP-address error!"));
				goto setErr_tcpip;
			}
		}

		strMask = websGetVar(wp, T("lan_mask"), T(""));
		if ( strMask[0] ) {
			if ( !inet_aton(strMask, &inMask) ) {
				strcpy(tmpBuf, T("Invalid subnet-mask value!"));
				goto setErr_tcpip;
			}
			if ( !apmib_set(MIB_SUBNET_MASK, (void *)&inMask)) {
				strcpy(tmpBuf, T("Set subnet-mask error!"));
				goto setErr_tcpip;
			}
		}
		else { // get current used netmask
			if ( !getInAddr(BRIDGE_IF, SUBNET_MASK, (void *)&inMask )) {
				strcpy(tmpBuf, T("Get subnet-mask error!"));
				goto setErr_tcpip;
			}
		}
		strGateway = websGetVar(wp, T("lan_gateway"), T(""));
		if ( (dhcp == DHCP_DISABLED && strGateway[0]) || 
			(dhcp == DHCP_SERVER && strGateway[0])	) {
			if ( !inet_aton(strGateway, &inGateway) ) {
				strcpy(tmpBuf, T("Invalid default-gateway value!"));
				goto setErr_tcpip;
			}
			if ( !apmib_set(MIB_DEFAULT_GATEWAY, (void *)&inGateway)) {
				strcpy(tmpBuf, T("Set default-gateway error!"));
				goto setErr_tcpip;
			}
		}

		if ( dhcp == DHCP_SERVER|| dhcp == DHCP_AUTO ) {
			// Get/Set DHCP client range
			strdhcpRangeStart = websGetVar(wp, T("dhcpRangeStart"), T(""));
			if ( strdhcpRangeStart[0] ) {
				if ( !inet_aton(strdhcpRangeStart, &dhcpRangeStart) ) {
					strcpy(tmpBuf, T("Invalid DHCP client start address!"));
					goto setErr_tcpip;
				}
				if ( !apmib_set(MIB_DHCP_CLIENT_START, (void *)&dhcpRangeStart)) {
					strcpy(tmpBuf, T("Set DHCP client start address error!"));
					goto setErr_tcpip;
				}
			}
			strdhcpRangeEnd = websGetVar(wp, T("dhcpRangeEnd"), T(""));
			if ( strdhcpRangeEnd[0] ) {
				if ( !inet_aton(strdhcpRangeEnd, &dhcpRangeEnd) ) {
					strcpy(tmpBuf, T("Invalid DHCP client end address!"));
					goto setErr_tcpip;
				}
				if ( !apmib_set(MIB_DHCP_CLIENT_END, (void *)&dhcpRangeEnd)) {
					strcpy(tmpBuf, T("Set DHCP client end address error!"));
					goto setErr_tcpip;
				}
			}

			if ( strdhcpRangeStart[0] && strdhcpRangeEnd[0] ) {
				unsigned long start, end, mask, ip;
				int diff;

				start = *((unsigned long *)&dhcpRangeStart);
				end = *((unsigned long *)&dhcpRangeEnd);
				diff = (int) ( ntohdw(end) - ntohdw(start) );
				ip = *((unsigned long *)&inIp);
				mask = *((unsigned long *)&inMask);
				if (diff <= 0 ||
					diff > 256*3 ||
					(ip&mask) != (start&mask) ||
					(ip&mask) != (end& mask) ) {
					strcpy(tmpBuf, T("Invalid DHCP client range!"));
					goto setErr_tcpip;
				}
			}

			// If DHCP server is enabled in LAN, update dhcpd.conf
			strDNS = websGetVar(wp, T("dns1"), T(""));
			if ( strDNS[0] ) {
				if ( !inet_aton(strDNS, &dns1) ) {
					strcpy(tmpBuf, T("Invalid DNS address value!"));
					goto setErr_tcpip;
				}

				if ( !apmib_set(MIB_WAN_DNS1, (void *)&dns1)) {
	  				strcpy(tmpBuf, T("Set DNS MIB error!"));
					goto setErr_tcpip;
				}
			}

			strDNS = websGetVar(wp, T("dns2"), T(""));
			if ( strDNS[0] ) {
				if ( !inet_aton(strDNS, &dns2) ) {
					strcpy(tmpBuf, T("Invalid DNS address value!"));
					goto setErr_tcpip;
				}
				if ( !apmib_set(MIB_WAN_DNS2, (void *)&dns2)) {
	  				strcpy(tmpBuf, T("Set DNS MIB error!"));
					goto setErr_tcpip;
				}
			}

			strDNS = websGetVar(wp, T("dns3"), T(""));
			if ( strDNS[0] ) {
				if ( !inet_aton(strDNS, &dns3) ) {
					strcpy(tmpBuf, T("Invalid DNS address value!"));
					goto setErr_tcpip;
				}
				if ( !apmib_set(MIB_WAN_DNS3, (void *)&dns3)) {
	  				strcpy(tmpBuf, T("Set DNS MIB error!"));
					goto setErr_tcpip;
				}
			}

			if (!call_from_wizard) {
				strDhcpLeaseTime = websGetVar(wp, T("dhcpLeaseTime"), T(""));
				if ( strDhcpLeaseTime ) {
					int dhcpLeaseTime = atoi(strDhcpLeaseTime)*60;							
					if ( !apmib_set(MIB_DHCP_LEASE_TIME, (void *)&dhcpLeaseTime)) {
	  					strcpy(tmpBuf, T("Set MIB_DHCP_LEASE_TIME MIB error!"));
						goto setErr_tcpip;
					}
				}
				
				strDomain = websGetVar(wp, T("domainName"), T(""));
				if ( strDomain ) {
					if (!isValidName(strDomain)) {
  						strcpy(tmpBuf, T("Invalid Domain Name! Please enter characters in A(a)~Z(z) or 0-9 without spacing."));
						goto setErr_tcpip;				
					}							
					if ( !apmib_set(MIB_DOMAIN_NAME, (void *)strDomain)) {
	  					strcpy(tmpBuf, T("Set MIB_DOMAIN_NAME MIB error!"));
						goto setErr_tcpip;
					}
				}else{
					 if ( !apmib_set(MIB_DOMAIN_NAME, (void *)"")){
	  					strcpy(tmpBuf, T("\"Set MIB_DOMAIN_NAME MIB error!\""));
						goto setErr_tcpip;
					}	
				}
			}			
		}
	}
	return 0 ;
setErr_tcpip:
	return -1 ;	
}

///////////////////////////////////////////////////////////////////
extern int update_linkchain(int fmt, void *Entry_old, void *Entry_new, int type_size);
void formTcpipSetup(webs_t wp, char_t *path, char_t *query)
{

	char tmpBuf[100];
	char buffer[200];
	char_t *submitUrl ;

	struct in_addr inLanaddr_orig, inLanaddr_new;
	struct in_addr inLanmask_orig, inLanmask_new;
	struct in_addr private_host, tmp_private_host, update;
	int	entryNum_resvdip, i;
	DHCPRSVDIP_T entry_resvdip, checkentry_resvdip;
	int link_type;
#if defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT)
	int opmode=0, wlan0_mode=0, check_flag=0;
	int lan_dhcp_mode_orig=0;
	int lan_dhcp_mode=0;
	char lan_domain_name[	MAX_NAME_LEN]={0};
	char lan_domain_name_orig[	MAX_NAME_LEN]={0};
#endif	
	apmib_get( MIB_IP_ADDR,  (void *)buffer); //save the orig lan subnet
	memcpy((void *)&inLanaddr_orig, buffer, 4);
	
	apmib_get( MIB_SUBNET_MASK,  (void *)buffer); //save the orig lan mask
	memcpy((void *)&inLanmask_orig, buffer, 4);
	
#if defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT)
	apmib_get( MIB_DHCP, (void *)&lan_dhcp_mode_orig);
	apmib_get( MIB_DOMAIN_NAME, (void *)lan_domain_name_orig);
#endif

	if(tcpipLanHandler(wp, tmpBuf) < 0)
		goto setErr_end ;

#if defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT)
	apmib_get(MIB_OP_MODE,(void *)&opmode);
	apmib_get( MIB_WLAN_MODE, (void *)&wlan0_mode);
	if(opmode ==1 && (wlan0_mode == 1 || wlan0_mode == 0)){ //when wlan is client mode or ap mode, user change lan setting
		check_flag=1;
	}
	apmib_set(MIB_AUTO_DISCOVERY_ENABLED,(void *)&check_flag); //lan ipaddress has been changed from web page 
#endif

	apmib_update_web(CURRENT_SETTING);	// update configuration to flash

	
	apmib_get( MIB_IP_ADDR,  (void *)buffer); //check the new lan subnet
	memcpy((void *)&inLanaddr_new, buffer, 4);
	
	apmib_get( MIB_SUBNET_MASK,  (void *)buffer); //check the new lan mask
	memcpy((void *)&inLanmask_new, buffer, 4);

#if defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT)
	apmib_get( MIB_DHCP, (void *)&lan_dhcp_mode);
	apmib_get( MIB_DOMAIN_NAME, (void *)lan_domain_name);
#endif
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
		apmib_update_web(CURRENT_SETTING);	// update configuration to flash
	}


#ifndef NO_ACTION
#if defined(VOIP_SUPPORT) && defined(ATA867x)
	run_init_script("all");
#else
	run_init_script("bridge");
#endif
#endif

	submitUrl = websGetVar(wp, T("submit-url"), T(""));

#ifdef REBOOT_CHECK
#if !defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT)
	if(memcmp(&inLanaddr_orig,&inLanaddr_new,4) == 0)
#else
	if((memcmp(&inLanaddr_orig,&inLanaddr_new,4) == 0) && (lan_dhcp_mode_orig==lan_dhcp_mode) && (lan_domain_name[0] && !strcmp(lan_domain_name, lan_domain_name_orig)))
#endif
	{
		OK_MSG(submitUrl);
	}
	else
	{
		char tmpBuf[200];
		char lan_ip_buf[30], lan_ip[30];

		//apmib_reinit();
			
		//apmib_update_web(CURRENT_SETTING);	// update configuration to flash
		run_init_script_flag = 1;	
#ifndef NO_ACTION
		run_init_script("all");
#endif		
		apmib_get( MIB_IP_ADDR,  (void *)lan_ip_buf) ;
  	sprintf(lan_ip,"%s",inet_ntoa(*((struct in_addr *)lan_ip_buf)) );
  	
  	sprintf(tmpBuf,"%s","<h4>Change setting successfully!<BR><BR>Do not turn off or reboot the Device during this time.</h4>");
		OK_MSG_FW(tmpBuf, submitUrl,APPLY_COUNTDOWN_TIME,lan_ip);
	}
#else
	RECONNECT_MSG(submitUrl);	// display reconnect msg to remote
#endif


	return;

setErr_end:
	ERR_MSG(tmpBuf);
}


#ifdef HOME_GATEWAY
int tcpipWanHandler(webs_t wp, char * tmpBuf, int *dns_changed)
{
	
	char_t	*strIp, *strMask, *strGateway, *strDNS, *strMode, *strConnect, *strMac;
	char_t  *strVal, *strType;
	int intVal;
	struct in_addr inIp, inMask,dns1, dns2, dns3, inGateway;
	DHCP_T dhcp, curDhcp;
#if defined(ROUTE_SUPPORT)	
	int orig_nat=0;
	int curr_nat=0;
#endif	
	DNS_TYPE_T dns, dns_old;

	char_t *submitUrl;
#ifndef NO_ACTION
	int pid;
#endif
	int buttonState=0, call_from_wizard=0;

//displayPostDate(wp->postData);

	strVal = websGetVar(wp, T("lan_ip"), T(""));
	if (strVal[0])
		call_from_wizard = 1;	
	
	strVal = websGetVar(wp, T("isPocketWizard"), T(""));
	if (strVal[0])
	{
		if ( atoi(strVal) == 1 )
		{
			call_from_wizard = 1;
		}
	}
			
	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page

	strConnect = websGetVar(wp, T("pppConnect"), T(""));
	if (strConnect && strConnect[0]) {
		buttonState = 1;
		strMode = T("ppp");
		goto set_ppp;
	}

        strConnect = websGetVar(wp, T("pppDisconnect"), T(""));
	if (strConnect && strConnect[0]) {
		buttonState = 2;
		strMode = T("ppp");
		goto set_ppp;
	}
	strConnect = websGetVar(wp, T("pptpConnect"), T(""));
	if (strConnect && strConnect[0]) {
		buttonState = 1;
		strMode = T("pptp");
		goto set_ppp;
	}

        strConnect = websGetVar(wp, T("pptpDisconnect"), T(""));
	if (strConnect && strConnect[0]) {
		buttonState = 2;
		strMode = T("pptp");
		goto set_ppp;
	}
	strConnect = websGetVar(wp, T("l2tpConnect"), T(""));
	if (strConnect && strConnect[0]) {
		buttonState = 1;
		strMode = T("l2tp");
		goto set_ppp;
	}

        strConnect = websGetVar(wp, T("l2tpDisconnect"), T(""));
	if (strConnect && strConnect[0]) {
		buttonState = 2;
		strMode = T("l2tp");
		goto set_ppp;
	}
#if 0 //sc_yang
	strVal = websGetVar(wp, T("save"), T(""));
	if (!strVal || !strVal[0]) { // not save, wan type is changed
		strVal = websGetVar(wp, T("wanType"), T(""));
		wanDhcpTmp = (DHCP_T)(strVal[0] - '0');

		if (submitUrl && submitUrl[0])
			websRedirect(wp, submitUrl);
		return;
	}
#endif
 	// Set clone MAC address
	strMac = websGetVar(wp, T("wan_macAddr"), T(""));
	if (strMac[0]) {
		if (strlen(strMac)!=12 || !string_to_hex(strMac, tmpBuf, 12)) {
			strcpy(tmpBuf, T("Error! Invalid MAC address."));
			goto setErr_tcpip;
		}
		if ( !apmib_set(MIB_WAN_MAC_ADDR, (void *)tmpBuf)) {
			strcpy(tmpBuf, T("Set MIB_WAN_MAC_ADDR mib error!"));
			goto setErr_tcpip;
		}
	}

	strMode = websGetVar(wp, T("dnsMode"), T(""));
	if ( strMode && strMode[0] ) {
		if (!strcmp(strMode, T("dnsAuto")))
			dns = DNS_AUTO;
		else if (!strcmp(strMode, T("dnsManual")))
			dns = DNS_MANUAL;
		else {
			strcpy(tmpBuf, T("Invalid DNS mode value!"));
			goto setErr_tcpip;
		}

		if ( !apmib_get(MIB_WAN_DNS_MODE, (void *)&dns_old)) {
	  		strcpy(tmpBuf, T("Get DNS MIB error!"));
			goto setErr_tcpip;
		}
		if (dns != dns_old)
			*dns_changed = 1;

		// Set DNS to MIB
		if ( !apmib_set(MIB_WAN_DNS_MODE, (void *)&dns)) {
	  		strcpy(tmpBuf, T("Set DNS MIB error!"));
			goto setErr_tcpip;
		}

		if ( dns == DNS_MANUAL ) {
			struct in_addr dns1_old, dns2_old, dns3_old;
			if ( !apmib_get(MIB_WAN_DNS1, (void *)&dns1_old)) {
	  			strcpy(tmpBuf, T("Get DNS1 MIB error!"));
				goto setErr_tcpip;
			}
			if ( !apmib_get(MIB_WAN_DNS2, (void *)&dns2_old)) {
	  			strcpy(tmpBuf, T("Get DNS1 MIB error!"));
				goto setErr_tcpip;
			}
			if ( !apmib_get(MIB_WAN_DNS3, (void *)&dns3_old)) {
	  			strcpy(tmpBuf, T("Get DNS1 MIB error!"));
				goto setErr_tcpip;
			}

			// If DHCP server is enabled in LAN, update dhcpd.conf
			strDNS = websGetVar(wp, T("dns1"), T(""));
			if ( strDNS[0] ) {
				if ( !inet_aton(strDNS, &dns1) ) {
					strcpy(tmpBuf, T("Invalid DNS address value!"));
					goto setErr_tcpip;
				}

				if ( !apmib_set(MIB_WAN_DNS1, (void *)&dns1)) {
	  				strcpy(tmpBuf, T("Set DNS MIB error!"));
					goto setErr_tcpip;
				}
			}
			else {
				if ( !apmib_get(MIB_WAN_DNS1, (void *)&dns1) ) {
					strcpy(tmpBuf, T("Get DNS1 MIB error!"));
					goto setErr_tcpip;
				}
			}
			strDNS = websGetVar(wp, T("dns2"), T(""));
			if ( strDNS[0] ) {
				if ( !inet_aton(strDNS, &dns2) ) {
					strcpy(tmpBuf, T("Invalid DNS address value!"));
					goto setErr_tcpip;
				}
				if ( !apmib_set(MIB_WAN_DNS2, (void *)&dns2)) {
	  				strcpy(tmpBuf, T("Set DNS MIB error!"));
					goto setErr_tcpip;
				}
			}
			else {
				if ( !apmib_get(MIB_WAN_DNS2, (void *)&dns2) ) {
					strcpy(tmpBuf, T("Get DNS2 MIB error!"));
					goto setErr_tcpip;
				}
			}
			strDNS = websGetVar(wp, T("dns3"), T(""));
			if ( strDNS[0] ) {
				if ( !inet_aton(strDNS, &dns3) ) {
					strcpy(tmpBuf, T("Invalid DNS address value!"));
					goto setErr_tcpip;
				}
				if ( !apmib_set(MIB_WAN_DNS3, (void *)&dns3)) {
	  				strcpy(tmpBuf, T("Set DNS MIB error!"));
					goto setErr_tcpip;
				}
			}
			else {
				if ( !apmib_get(MIB_WAN_DNS3, (void *)&dns3) ) {
					strcpy(tmpBuf, T("Get DNS3 MIB error!"));
					goto setErr_tcpip;
				}
			}

			if ( *((long *)&dns1) != *((long *)&dns1_old) ||
				 *((long *)&dns2) != *((long *)&dns2_old) ||
				  *((long *)&dns3) != *((long *)&dns3_old) )
				*dns_changed = 1;
		}
	}

	// Read current ip mode setting for reference later
	if ( !apmib_get( MIB_WAN_DHCP, (void *)&curDhcp) ) {
		strcpy(tmpBuf, T("Get WAN DHCP MIB error!"));
		goto setErr_tcpip;
	}
#if defined(ROUTE_SUPPORT)	
	if ( !apmib_get( MIB_NAT_ENABLED, (void *)&orig_nat) ) {
		strcpy(tmpBuf, T("Get NAT MIB error!"));
		goto setErr_tcpip;
	}
	
#endif	
	//sc_yang
	//strMode = websGetVar(wp, T("ipMode"), T(""));
	strMode = websGetVar(wp, T("wanType"), T(""));

set_ppp:
	if ( strMode && strMode[0] ) {
		if ( !strcmp(strMode, T("autoIp")))
			dhcp = DHCP_CLIENT;
		else if ( !strcmp(strMode, T("fixedIp")))
			dhcp = DHCP_DISABLED;
		else if ( !strcmp(strMode, T("ppp"))) {
			char_t	*strName, *strPassword, *strService;
			dhcp = PPPOE;
  			strName = websGetVar(wp, T("pppUserName"), T(""));
			if ( strName[0] ) {
				if ( apmib_set(MIB_PPP_USER, (void *)strName) == 0) {
					strcpy(tmpBuf, T("Set PPP user name MIB error!"));
					goto setErr_tcpip;
				}
			}

 			strPassword = websGetVar(wp, T("pppPassword"), T(""));
			if ( strPassword[0] ) {
				if ( apmib_set(MIB_PPP_PASSWORD, (void *)strPassword) == 0) {
					strcpy(tmpBuf, T("Set PPP user password MIB error!"));
					goto setErr_tcpip;
				}
			}

			strService = websGetVar(wp, T("pppServiceName"), T(""));
			if ( strService[0] ) {
				if ( apmib_set(MIB_PPP_SERVICE, (void *)strService) == 0) {
					strcpy(tmpBuf, T("Set PPP serice name MIB error!"));
					goto setErr_tcpip;
				}
			}else{
				if ( apmib_set(MIB_PPP_SERVICE, (void *)"") == 0) {
					strcpy(tmpBuf, T("Set PPP serice name MIB error!"));
					goto setErr_tcpip;
				}
			}
			strType = websGetVar(wp, T("pppConnectType"), T(""));
			if ( strType[0] ) {
				PPP_CONNECT_TYPE_T type;
				if ( strType[0] == '0' )
					type = CONTINUOUS;
				else if ( strType[0] == '1' )
					type = CONNECT_ON_DEMAND;
				else if ( strType[0] == '2' )
					type = MANUAL;
				else {
					strcpy(tmpBuf, T("Invalid PPP type value!"));
					goto setErr_tcpip;
				}
				if ( apmib_set(MIB_PPP_CONNECT_TYPE, (void *)&type) == 0) {
   					strcpy(tmpBuf, T("Set PPP type MIB error!"));
					goto setErr_tcpip;
				}
				if (type != CONTINUOUS) {
					char_t *strTime;
					strTime = websGetVar(wp, T("pppIdleTime"), T(""));
					if ( strTime[0] ) {
						int time;
 						time = strtol(strTime, (char**)NULL, 10) * 60;
						if ( apmib_set(MIB_PPP_IDLE_TIME, (void *)&time) == 0) {
   							strcpy(tmpBuf, T("Set PPP idle time MIB error!"));
							goto setErr_tcpip;
						}
					}
				}
			}
			strVal = websGetVar(wp, T("pppMtuSize"), T(""));
			if ( strVal[0] ) {
				int mtuSize;
 				mtuSize = strtol(strVal, (char**)NULL, 10);
				if ( apmib_set(MIB_PPP_MTU_SIZE, (void *)&mtuSize) == 0) {
					strcpy(tmpBuf, T("Set PPP mtu size MIB error!"));
					goto setErr_tcpip;
				}
			}
		}
		else if ( !strcmp(strMode, T("pptp"))) {
			char_t	*strName, *strPassword;
			dhcp = PPTP;
  			strName = websGetVar(wp, T("pptpUserName"), T(""));
			if ( strName[0] ) {
				if ( apmib_set(MIB_PPTP_USER, (void *)strName) == 0) {
					strcpy(tmpBuf, T("Set PPTP user name MIB error!"));
					goto setErr_tcpip;
				}
			}
 			strPassword = websGetVar(wp, T("pptpPassword"), T(""));
			if ( strPassword[0] ) {
				if ( apmib_set(MIB_PPTP_PASSWORD, (void *)strPassword) == 0) {
					strcpy(tmpBuf, T("Set PPTP user password MIB error!"));
					goto setErr_tcpip;
				}
			}
			
			strIp = websGetVar(wp, T("pptpIpAddr"), T(""));
			if ( strIp[0] ) {
				if ( !inet_aton(strIp, &inIp) ) {
					strcpy(tmpBuf, T("Invalid IP-address value!"));
					goto setErr_tcpip;
				}
				if ( !apmib_set(MIB_PPTP_IP, (void *)&inIp)) {
					strcpy(tmpBuf, T("Set IP-address error!"));
					goto setErr_tcpip;
				}
			}

			strMask = websGetVar(wp, T("pptpSubnetMask"), T(""));
			if ( strMask[0] ) {
				if ( !inet_aton(strMask, &inMask) ) {
					strcpy(tmpBuf, T("Invalid subnet-mask value!"));
					goto setErr_tcpip;
				}
				if ( !apmib_set(MIB_PPTP_SUBNET_MASK, (void *)&inMask)) {
					strcpy(tmpBuf, T("Set subnet-mask error!"));
					goto setErr_tcpip;
				}
			}

			strGateway = websGetVar(wp, T("pptpServerIpAddr"), T(""));
			if ( strGateway[0] ) {
				if ( !inet_aton(strGateway, &inGateway) ) {
					strcpy(tmpBuf, T("Invalid pptp server ip value!"));
					goto setErr_tcpip;
				}
				if ( !apmib_set(MIB_PPTP_SERVER_IP, (void *)&inGateway)) {
					strcpy(tmpBuf, T("Set pptp server ip error!"));
					goto setErr_tcpip;
				}
			}

		strType = websGetVar(wp, T("pptpConnectType"), T(""));
			if ( strType[0] ) {
				PPP_CONNECT_TYPE_T type;
				if ( strType[0] == '0' )
					type = CONTINUOUS;
				else if ( strType[0] == '1' )
					type = CONNECT_ON_DEMAND;
				else if ( strType[0] == '2' )
					type = MANUAL;
				else {
					strcpy(tmpBuf, T("Invalid PPTP type value!"));
					goto setErr_tcpip;
				}
				if ( apmib_set(MIB_PPTP_CONNECTION_TYPE, (void *)&type) == 0) {
   					strcpy(tmpBuf, T("Set PPTP type MIB error!"));
					goto setErr_tcpip;
				}
				if (type != CONTINUOUS) {
					char_t *strTime;
					strTime = websGetVar(wp, T("pptpIdleTime"), T(""));
					if ( strTime[0] ) {
						int time;
 						time = strtol(strTime, (char**)NULL, 10) * 60;
						if ( apmib_set(MIB_PPTP_IDLE_TIME, (void *)&time) == 0) {
   							strcpy(tmpBuf, T("Set PPTP idle time MIB error!"));
							goto setErr_tcpip;
						}
					}
				}
			}
			strVal = websGetVar(wp, T("pptpMtuSize"), T(""));
			if ( strVal[0] ) {
				int mtuSize;
 				mtuSize = strtol(strVal, (char**)NULL, 10);
				if ( apmib_set(MIB_PPTP_MTU_SIZE, (void *)&mtuSize) == 0) {
					strcpy(tmpBuf, T("Set PPTP mtu size MIB error!"));
					goto setErr_tcpip;
				}
			}
			if (!call_from_wizard) { // not called from wizard
				strVal = websGetVar(wp, T("pptpSecurity"), T(""));
				if ( !gstrcmp(strVal, T("ON")))
					intVal = 1;
				else
					intVal = 0;
				apmib_set(MIB_PPTP_SECURITY_ENABLED, (void *)&intVal);

				strVal = websGetVar(wp, T("pptpCompress"), T(""));
				if ( !gstrcmp(strVal, T("ON")))
					intVal = 1;
				else
					intVal = 0;
				apmib_set(MIB_PPTP_MPPC_ENABLED, (void *)&intVal);				
			}			
		}
		/* # keith: add l2tp support. 20080515 */
		else if ( !strcmp(strMode, T("l2tp"))) {
			char_t	*strName, *strPassword;
			dhcp = L2TP;
  			strName = websGetVar(wp, T("l2tpUserName"), T(""));
			if ( strName[0] ) {
				if ( apmib_set(MIB_L2TP_USER, (void *)strName) == 0) {
					strcpy(tmpBuf, T("Set L2TP user name MIB error!"));
					goto setErr_tcpip;
				}
			}
 			strPassword = websGetVar(wp, T("l2tpPassword"), T(""));
			if ( strPassword[0] ) {
				if ( apmib_set(MIB_L2TP_PASSWORD, (void *)strPassword) == 0) {
					strcpy(tmpBuf, T("Set L2TP user password MIB error!"));
					goto setErr_tcpip;
				}
			}
			
			strIp = websGetVar(wp, T("l2tpIpAddr"), T(""));
			if ( strIp[0] ) {
				if ( !inet_aton(strIp, &inIp) ) {
					strcpy(tmpBuf, T("Invalid IP-address value!"));
					goto setErr_tcpip;
				}
				if ( !apmib_set(MIB_L2TP_IP, (void *)&inIp)) {
					strcpy(tmpBuf, T("Set IP-address error!"));
					goto setErr_tcpip;
				}
			}

			strMask = websGetVar(wp, T("l2tpSubnetMask"), T(""));
			if ( strMask[0] ) {
				if ( !inet_aton(strMask, &inMask) ) {
					strcpy(tmpBuf, T("Invalid subnet-mask value!"));
					goto setErr_tcpip;
				}
				if ( !apmib_set(MIB_L2TP_SUBNET_MASK, (void *)&inMask)) {
					strcpy(tmpBuf, T("Set subnet-mask error!"));
					goto setErr_tcpip;
				}
			}

			strGateway = websGetVar(wp, T("l2tpServerIpAddr"), T(""));
			if ( strGateway[0] ) {
				if ( !inet_aton(strGateway, &inGateway) ) {
					strcpy(tmpBuf, T("Invalid l2tp server ip value!"));
					goto setErr_tcpip;
				}
				if ( !apmib_set(MIB_L2TP_SERVER_IP, (void *)&inGateway)) {
					strcpy(tmpBuf, T("Set pptp server ip error!"));
					goto setErr_tcpip;
				}
			}

		strType = websGetVar(wp, T("l2tpConnectType"), T(""));
			if ( strType[0] ) {
				PPP_CONNECT_TYPE_T type;
				if ( strType[0] == '0' )
					type = CONTINUOUS;
				else if ( strType[0] == '1' )
					type = CONNECT_ON_DEMAND;
				else if ( strType[0] == '2' )
					type = MANUAL;
				else {
					strcpy(tmpBuf, T("Invalid L2TP type value!"));
					goto setErr_tcpip;
				}
				if ( apmib_set(MIB_L2TP_CONNECTION_TYPE, (void *)&type) == 0) {
   					strcpy(tmpBuf, T("Set L2TP type MIB error!"));
					goto setErr_tcpip;
				}
				if (type != CONTINUOUS) {
					char_t *strTime;
					strTime = websGetVar(wp, T("l2tpIdleTime"), T(""));
					if ( strTime[0] ) {
						int time;
 						time = strtol(strTime, (char**)NULL, 10) * 60;
						if ( apmib_set(MIB_L2TP_IDLE_TIME, (void *)&time) == 0) {
   							strcpy(tmpBuf, T("Set L2TP idle time MIB error!"));
							goto setErr_tcpip;
						}
					}
				}
			}
			strVal = websGetVar(wp, T("l2tpMtuSize"), T(""));
			if ( strVal[0] ) {
				int mtuSize;
 				mtuSize = strtol(strVal, (char**)NULL, 10);
				if ( apmib_set(MIB_L2TP_MTU_SIZE, (void *)&mtuSize) == 0) {
					strcpy(tmpBuf, T("Set L2TP mtu size MIB error!"));
					goto setErr_tcpip;
				}
			}
				
		}

		else {
			strcpy(tmpBuf, T("Invalid IP mode value!"));
			goto setErr_tcpip;
		}
		if ( !apmib_set(MIB_WAN_DHCP, (void *)&dhcp)) {
	  		strcpy(tmpBuf, T("Set DHCP error!"));
			goto setErr_tcpip;
		}
#if defined(ROUTE_SUPPORT)		
	if ( (dhcp == PPPOE) || (dhcp == PPTP) || (dhcp == L2TP) ) { 
		curr_nat=1;
		
		if(curr_nat !=orig_nat){//force NAT is enabled when pppoe/pptp/l2tp
			if ( !apmib_set( MIB_NAT_ENABLED, (void *)&curr_nat) ) {
				strcpy(tmpBuf, T("Get NAT MIB error!"));
				goto setErr_tcpip;
			}
			intVal=0;
			if (apmib_set( MIB_RIP_LAN_TX, (void *)&intVal) == 0) {
					strcpy(tmpBuf, T("\"Set RIP LAN Tx error!\""));
					goto setErr_tcpip;
			}
			if (apmib_set( MIB_RIP_WAN_TX, (void *)&intVal) == 0) {
					strcpy(tmpBuf, T("\"Set RIP WAN Tx error!\""));
					goto setErr_tcpip;
			}
			if (!apmib_set(MIB_IGMP_PROXY_DISABLED, (void *)&intVal)) {
				strcpy(tmpBuf, T("Set MIB_IGMP_PROXY_DISABLED error!"));
				goto setErr_tcpip;
			}
		}
	}
#endif	
		if ( (dhcp == PPPOE && buttonState == 1) || (dhcp == PPTP && buttonState == 1) || (dhcp == L2TP && buttonState == 1) ) { // connect button is pressed
			int wait_time=30;
			int opmode=0;
			apmib_update_web(CURRENT_SETTING);	// update to flash
			apmib_get(MIB_OP_MODE, (void *)&opmode);
			
			if(opmode==2)
				WAN_IF = T("wlan0");
			else if(opmode ==0)
				WAN_IF = T("eth1");

			system("killall -9 igmpproxy 2> /dev/null");
			system("killall -9 dnrd 2> /dev/null");
			if(dhcp == PPPOE || dhcp == PPTP)
				system("killall -15 pppd 2> /dev/null");
			else
				system("killall -9 pppd 2> /dev/null");
				
				system("disconnect.sh option");
#ifndef NO_ACTION
			pid = fork();
        		if (pid)
	        		waitpid(pid, NULL, 0);
			else if (pid == 0) {
				if(dhcp == PPPOE){
					snprintf(tmpBuf, 100, "%s/%s", _CONFIG_SCRIPT_PATH, _PPPOE_SCRIPT_PROG);
					execl( tmpBuf, _PPPOE_SCRIPT_PROG, "connect", WAN_IF, NULL);
				}else if(dhcp == PPTP){
					snprintf(tmpBuf, 100, "%s/%s", _CONFIG_SCRIPT_PATH, _PPTP_SCRIPT_PROG);
					execl( tmpBuf, _PPTP_SCRIPT_PROG, "connect", WAN_IF, NULL);
				}else if(dhcp == L2TP){
					system("killall -9 l2tpd 2> /dev/null");
					system("rm -f /var/run/l2tpd.pid 2> /dev/null");
					snprintf(tmpBuf, 100, "%s/%s", _CONFIG_SCRIPT_PATH, _L2TP_SCRIPT_PROG);
					execl( tmpBuf, _L2TP_SCRIPT_PROG, "connect", WAN_IF, NULL);
				}
       				exit(1);
			}
			while (wait_time-- >0) {
				if (isConnectPPP()){
					printf("PPP is connected\n");
					break;
				}
				sleep(1);
			}
			if (isConnectPPP())
				strcpy(tmpBuf, T("Connected to server successfully.\n"));
			else
				strcpy(tmpBuf, T("Connect to server failed!\n"));

			OK_MSG1(tmpBuf, submitUrl);
#endif
			return 1;
		}

		if ( (dhcp == PPPOE && buttonState == 2) || (dhcp == PPTP && buttonState == 2) || (dhcp == L2TP && buttonState == 2) ) { // connect button is pressed

			apmib_update_web(CURRENT_SETTING);	// update to flash
#ifndef NO_ACTION
		if(dhcp != PPTP){
			pid = fork();
        		if (pid)
	             		waitpid(pid, NULL, 0);
        		else if (pid == 0) {
				snprintf(tmpBuf, 100, "%s/%s", _CONFIG_SCRIPT_PATH, _PPPOE_DC_SCRIPT_PROG);
				execl( tmpBuf, _PPPOE_DC_SCRIPT_PROG, "all", NULL);
                		exit(1);
        		}
        	}else{
        		system("killall -15 pppd 2> /dev/null");
        	}
        		if(dhcp == PPPOE)	
			strcpy(tmpBuf, T("PPPoE disconnected.\n"));
			if(dhcp == PPTP)	
			strcpy(tmpBuf, T("PPTP disconnected.\n"));
			if(dhcp == L2TP)	
			strcpy(tmpBuf, T("L2TP disconnected.\n"));
			OK_MSG1(tmpBuf, submitUrl);
#endif
			return 1;
		}
	}
	else
		dhcp = curDhcp;

	if ( dhcp == DHCP_DISABLED ) {
		strIp = websGetVar(wp, T("wan_ip"), T(""));
		if ( strIp[0] ) {
			if ( !inet_aton(strIp, &inIp) ) {
				strcpy(tmpBuf, T("Invalid IP-address value!"));
				goto setErr_tcpip;
			}
			if ( !apmib_set(MIB_WAN_IP_ADDR, (void *)&inIp)) {
				strcpy(tmpBuf, T("Set IP-address error!"));
				goto setErr_tcpip;
			}
		}

		strMask = websGetVar(wp, T("wan_mask"), T(""));
		if ( strMask[0] ) {
			if ( !inet_aton(strMask, &inMask) ) {
				strcpy(tmpBuf, T("Invalid subnet-mask value!"));
				goto setErr_tcpip;
			}
			if ( !apmib_set(MIB_WAN_SUBNET_MASK, (void *)&inMask)) {
				strcpy(tmpBuf, T("Set subnet-mask error!"));
				goto setErr_tcpip;
			}
		}

		strGateway = websGetVar(wp, T("wan_gateway"), T(""));
		if ( strGateway[0] ) {
			if ( !inet_aton(strGateway, &inGateway) ) {
				strcpy(tmpBuf, T("Invalid default-gateway value!"));
				goto setErr_tcpip;
			}
			if ( !apmib_set(MIB_WAN_DEFAULT_GATEWAY, (void *)&inGateway)) {
				strcpy(tmpBuf, T("Set default-gateway error!"));
				goto setErr_tcpip;
			}
		}

		strVal = websGetVar(wp, T("fixedIpMtuSize"), T(""));
		if ( strVal[0] ) {
			int mtuSize;
			mtuSize = strtol(strVal, (char**)NULL, 10);
			if ( apmib_set(MIB_FIXED_IP_MTU_SIZE, (void *)&mtuSize) == 0) {
				strcpy(tmpBuf, T("Set FIXED-IP mtu size MIB error!"));
				goto setErr_tcpip;
			}
		}		
	}
	
	if (!call_from_wizard) { // not called from wizard
		if (dhcp == DHCP_CLIENT) {
			strVal = websGetVar(wp, T("dhcpMtuSize"), T(""));
			if ( strVal ) {
				int mtuSize;
				mtuSize = strtol(strVal, (char**)NULL, 10);
				if ( apmib_set(MIB_DHCP_MTU_SIZE, (void *)&mtuSize) == 0) {
					strcpy(tmpBuf, T("Set DHCP mtu size MIB error!"));
					goto setErr_tcpip;
				}
			}
			
			strVal = websGetVar(wp, T("hostName"), T(""));
			if (strVal) {
				if (!isValidName(strVal)) {
  					strcpy(tmpBuf, T("Invalid Host Name! Please enter characters in A(a)~Z(z) or 0-9 without spacing."));
					goto setErr_tcpip;				
				}			
				if ( !apmib_set(MIB_HOST_NAME, (void *)strVal)) {
  					strcpy(tmpBuf, T("Set MIB_HOST_NAME MIB error!"));
					goto setErr_tcpip;
				}
			}else{
				 if ( !apmib_set(MIB_HOST_NAME, (void *)"")){
	  					strcpy(tmpBuf, T("\"Set MIB_HOST_NAME MIB error!\""));
						goto setErr_tcpip;
				}	
			}					
		}	

		strVal = websGetVar(wp, T("upnpEnabled"), T(""));
		if ( !gstrcmp(strVal, T("ON")))
				intVal = 1;
			else
				intVal = 0;
		if ( !apmib_set(MIB_UPNP_ENABLED, (void *)&intVal)) {
			strcpy(tmpBuf, T("Set MIB_UPNP_ENABLED error!"));
			goto setErr_tcpip;
		}
//Brad add for igmpproxy
		strVal = websGetVar(wp, T("igmpproxyEnabled"), T(""));
		if ( !gstrcmp(strVal, T("ON")))
			intVal = 0;
		else
			intVal = 1;
		if ( !apmib_set(MIB_IGMP_PROXY_DISABLED, (void *)&intVal)) {
			strcpy(tmpBuf, T("Set MIB_IGMP_PROXY_DISABLED error!"));
			goto setErr_tcpip;
		}
//Brad add end
		strVal = websGetVar(wp, T("webWanAccess"), T(""));

		if ( !gstrcmp(strVal, T("ON")))
			intVal = 1;
		else
			intVal = 0;
		if ( !apmib_set(WEB_WAN_ACCESS_ENABLED, (void *)&intVal)) {
			strcpy(tmpBuf, T("Set WEB_WAN_ACCESS_ENABLED error!"));
			goto setErr_tcpip;
		}
				
		strVal = websGetVar(wp, T("pingWanAccess"), T(""));

		if ( !gstrcmp(strVal, T("ON")))
			intVal = 1;
		else
			intVal = 0;
		if ( !apmib_set(PING_WAN_ACCESS_ENABLED, (void *)&intVal)) {
			strcpy(tmpBuf, T("Set PING_WAN_ACCESS_ENABLED error!"));
			goto setErr_tcpip;
		}
			
		strVal = websGetVar(wp, T("WANPassThru1"), T(""));
		if ( !gstrcmp(strVal, T("ON")))
			intVal = 1;
		else
			intVal = 0;	
		if ( !apmib_set(VPN_PASSTHRU_IPSEC_ENABLED, (void *)&intVal)) {
			strcpy(tmpBuf, T("Set VPN_PASSTHRU_IPSEC_ENABLED error!"));
			goto setErr_tcpip;
		}
		
		strVal = websGetVar(wp, T("WANPassThru2"), T(""));
		if ( !gstrcmp(strVal, T("ON")))
			intVal = 1;
		else
			intVal = 0;
		if ( !apmib_set(VPN_PASSTHRU_PPTP_ENABLED, (void *)&intVal)) {
			strcpy(tmpBuf, T("Set VPN_PASSTHRU_PPTP_ENABLED error!"));
			goto setErr_tcpip;
		}
						
		strVal = websGetVar(wp, T("WANPassThru3"), T(""));
		if ( !gstrcmp(strVal, T("ON")))
			intVal = 1;
		else
			intVal = 0;
		if ( !apmib_set(VPN_PASSTHRU_L2TP_ENABLED, (void *)&intVal)) {
			strcpy(tmpBuf, T("Set VPN_PASSTHRU_L2TP_ENABLED error!"));
			goto setErr_tcpip;
		}
				
		strVal = websGetVar(wp, T("WANPassThruIPv6"), T(""));
		if ( !gstrcmp(strVal, T("ON")))
			intVal = 1;
		else
			intVal = 0;
		if ( !apmib_set(MIB_VPN_PASSTHRU_IPV6, (void *)&intVal)) {
			strcpy(tmpBuf, T("Set MIB_VPN_PASSTHRU_IPV6 error!"));
			goto setErr_tcpip;
		}
	}
	return 0 ;
setErr_tcpip:
	return -1 ;	
}	



///////1/////////////////////////////////////////////////////////////////////////
void formWanTcpipSetup(webs_t wp, char_t *path, char_t *query)
{


	char tmpBuf[100];
	int dns_changed=0;
	char *arg;
	char_t *submitUrl;
	int val ;
		
	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page

	if((val = tcpipWanHandler(wp, tmpBuf, &dns_changed)) < 0 )
		goto setErr_end ;
	else if (val == 1) // return ok
		return ;

	apmib_update_web(CURRENT_SETTING);	// update to flash

	// run script
	if ( dns_changed )
		arg = "all";
	else
		arg = "wan";

#ifdef UNIVERSAL_REPEATER
	apmib_get(MIB_REPEATER_ENABLED1, (void *)&val);
	if (val) 
		arg = "all";	
#endif	

#ifndef NO_ACTION
	run_init_script(arg);                
#endif

	OK_MSG(submitUrl);

	return;
setErr_end:
	ERR_MSG(tmpBuf);
}

/////////////////////////////////////////////////////////////////////////////
#if 0
int showWanPage(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t *p1, *p2, *p3, *p4, *lpath, *cp, *tmp1, *curPage=NULL;
	DHCP_T dhcp;
	int n;

	n = ejArgs(argc, argv, T("%s %s %s %s"), &p1, &p2, &p3, &p4);

	if (wanDhcpTmp == -1)
		apmib_get(MIB_WAN_DHCP, (void *)&dhcp);
	else
		dhcp = wanDhcpTmp;

	if (dhcp == DHCP_DISABLED && n > 0)
		curPage = p1;
	else if (dhcp == DHCP_CLIENT && n > 1)
		curPage = p2;
	else if (dhcp == PPPOE && n > 2)
		curPage = p3;
	else if (dhcp == PPTP && n > 3)
		curPage = p4;

	if (curPage == NULL)
		return -1;

	lpath = websGetRequestLpath(wp);

	if ((cp = gstrrchr(lpath, '/')) != NULL)
		*cp = '\0';

	tmp1 = balloc(B_L, strlen(lpath)+100);
	if (tmp1 == NULL) {
		ERR_MSG("allocate buffer failed!");
		return 0;
	}
	gstrcpy(tmp1, lpath);
	gstrcat(tmp1, curPage);

	bfree(B_L, wp->lpath);
	bfree(B_L, wp->path);
	bfree(B_L, wp->url);

	lpath = tmp1;
	wp->lpath = lpath;
	wp->path = bstrdup(B_L, curPage);
	wp->url = bstrdup(B_L, curPage);

	if (websPageOpen(wp, lpath, curPage, SOCKET_RDONLY | SOCKET_BINARY, 0666) < 0) {
		websError(wp, 400, T("Cannot open URL <b>%s</b>"), p1);
		return -1;
	}
	if (websAspRequest(wp, lpath) < 0)
		return 1;

	return 0;
}
#endif
#endif
//////////////////////////////////////////////////////////////////////////////
//Static DHCP 
void formStaticDHCP(webs_t wp, char_t *path, char_t *query)
{
	char_t *strStp, *strIp, *strHostName, *strAddRsvIP, *strDelRsvIP, *strDelAllRsvIP, *strVal, *submitUrl;
	char tmpBuf[100];
	char buffer[100];
	int entryNum, i, stp;
	DHCPRSVDIP_T staticIPEntry, delEntry;
	struct in_addr inIp;
	struct in_addr inLanaddr_orig;
	struct in_addr inLanmask_orig;
	strAddRsvIP = websGetVar(wp, T("addRsvIP"), T(""));
	strDelRsvIP = websGetVar(wp, T("deleteSelRsvIP"), T(""));
	strDelAllRsvIP = websGetVar(wp, T("deleteAllRsvIP"), T(""));

//displayPostDate(wp->postData);

	apmib_get( MIB_IP_ADDR,  (void *)buffer); //save the orig lan subnet
	memcpy((void *)&inLanaddr_orig, buffer, 4);
	
	apmib_get( MIB_SUBNET_MASK,  (void *)buffer); //save the orig lan mask
	memcpy((void *)&inLanmask_orig, buffer, 4);
	
	// Set static DHCP
	strStp = websGetVar(wp, T("static_dhcp"), T(""));
	if (strStp[0]) {
		if (strStp[0] == '0')
			stp = 0;
		else
			stp = 1;
		if ( !apmib_set(MIB_DHCPRSVDIP_ENABLED, (void *)&stp)) {
			strcpy(tmpBuf, T("Set static DHCP mib error!"));
			goto setErr_rsv;
		}
	}
	
	if (strAddRsvIP[0]) {
		memset(&staticIPEntry, '\0', sizeof(staticIPEntry));	
		strHostName = (char *)websGetVar(wp, T("hostname"), "");	
		if (strHostName[0])
			strcpy(staticIPEntry.hostName, strHostName);				
		strIp = websGetVar(wp,T( "ip_addr"), T(""));
		if (strIp[0]) {
			inet_aton(strIp, &inIp);
			memcpy(staticIPEntry.ipAddr, &inIp, 4);
		}
		strVal = websGetVar(wp, T("mac_addr"), T(""));
		if ( !strVal[0] ) {
	//		strcpy(tmpBuf, T("Error! No mac address to set."));
			goto setac_ret;
		}
		if (strlen(strVal)!=12 || !string_to_hex(strVal, staticIPEntry.macAddr, 12)) {
			strcpy(tmpBuf, T("Error! Invalid MAC address."));
			goto setErr_rsv;
		}
		if ( !apmib_get(MIB_DHCPRSVDIP_NUM, (void *)&entryNum)) {
			strcpy(tmpBuf, T("Get entry number error!"));
			goto setErr_rsv;
		}
		if ( (entryNum + 1) > MAX_DHCP_RSVD_IP_NUM) {
			strcpy(tmpBuf, T("Cannot add new entry because table is full!"));
			goto setErr_rsv;
		}
		if((inLanaddr_orig.s_addr & inLanmask_orig.s_addr) != (inIp.s_addr & inLanmask_orig.s_addr)){
			strcpy(tmpBuf, T("Cannot add new entry because the ip is not the same subnet as LAN network!"));
			goto setErr_rsv;
		}
	
		
		
		// set to MIB. try to delete it first to avoid duplicate case
		apmib_set(MIB_DHCPRSVDIP_DEL, (void *)&staticIPEntry);
		if ( apmib_set(MIB_DHCPRSVDIP_ADD, (void *)&staticIPEntry) == 0) {
			strcpy(tmpBuf, T("Add table entry error!"));
			goto setErr_rsv;
		}
	}

	/* Delete entry */
	if (strDelRsvIP[0]) {
		if ( !apmib_get(MIB_DHCPRSVDIP_NUM, (void *)&entryNum)) {
			strcpy(tmpBuf, T("Get entry number error!"));
			goto setErr_rsv;
		}
		for (i=entryNum; i>0; i--) {
			snprintf(tmpBuf, 20, "select%d", i);
			memset(&delEntry, '\0', sizeof(delEntry));	
			strVal = websGetVar(wp, tmpBuf, T(""));
			if ( !gstrcmp(strVal, T("ON")) ) {

				*((char *)&delEntry) = (char)i;
				if ( !apmib_get(MIB_DHCPRSVDIP, (void *)&delEntry)) {
					strcpy(tmpBuf, T("Get table entry error!"));
					goto setErr_rsv;
				}
				if ( !apmib_set(MIB_DHCPRSVDIP_DEL, (void *)&delEntry)) {
					strcpy(tmpBuf, T("Delete table entry error!"));
					goto setErr_rsv;
				}
			}
		}
	}

	/* Delete all entry */
	if ( strDelAllRsvIP[0]) {
		if ( !apmib_set(MIB_DHCPRSVDIP_DELALL, (void *)&staticIPEntry)) {
			strcpy(tmpBuf, T("Delete all table error!"));
			goto setErr_rsv;
		}
	}

setac_ret:
	apmib_update_web(CURRENT_SETTING);

#ifndef NO_ACTION
	run_init_script("all");
#endif

	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	OK_MSG( submitUrl );
  	return;

setErr_rsv:
	ERR_MSG(tmpBuf);
}


int dhcpRsvdIp_List(int eid, webs_t wp, int argc, char_t **argv)
{
	int	entryNum, i;
	int nBytesSent=0;
	DHCPRSVDIP_T entry;
	char macaddr[30];
	apmib_get(MIB_DHCPRSVDIP_NUM, (void *)&entryNum);
	nBytesSent += websWrite(wp, T("<tr>"
      	"<td align=center width=\"30%%\" bgcolor=\"#808080\"><font size=\"2\"><b>IP Address</b></font></td>\n"
      	"<td align=center width=\"30%%\" bgcolor=\"#808080\"><font size=\"2\"><b>MAC Address</b></font></td>\n"
      	"<td align=center width=\"30%%\" bgcolor=\"#808080\"><font size=\"2\"><b>Comment</b></font></td>\n"
      	"<td align=center width=\"10%%\" bgcolor=\"#808080\"><font size=\"2\"><b>Select</b></font></td></tr>\n"));
	for (i=1; i<=entryNum; i++) {
		*((char *)&entry) = (char)i;
		apmib_get(MIB_DHCPRSVDIP, (void *)&entry);
		if (!memcmp(entry.macAddr, "\x0\x0\x0\x0\x0\x0", 6))
			macaddr[0]='\0';
		else			
			sprintf(macaddr," %02x-%02x-%02x-%02x-%02x-%02x", entry.macAddr[0], entry.macAddr[1], entry.macAddr[2], entry.macAddr[3], entry.macAddr[4], entry.macAddr[5]);
		nBytesSent += websWrite(wp, T("<tr>"
			"<td align=center width=\"30%%\" bgcolor=\"#C0C0C0\"><font size=\"2\">%s</td>\n"
			"<td align=center width=\"30%%\" bgcolor=\"#C0C0C0\"><font size=\"2\">%s</td>\n"
      			"<td align=center width=\"30%%\" bgcolor=\"#C0C0C0\"><font size=\"2\">%s</td>\n"
       			"<td align=center width=\"10%%\" bgcolor=\"#C0C0C0\"><input type=\"checkbox\" name=\"select%d\" value=\"ON\"></td></tr>\n"),
			inet_ntoa(*((struct in_addr*)entry.ipAddr)), macaddr,entry.hostName, i);	
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
int dhcpClientList(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fp;
	int nBytesSent=0;
	int element=0, ret;
	char ipAddr[40], macAddr[40], liveTime[80], *buf=NULL, tmpBuf[100];
	unsigned char *ptr;
	struct stat status;
	int pid;

	// siganl DHCP server to update lease file
	snprintf(tmpBuf, 100, "%s/%s.pid", _DHCPD_PID_PATH, _DHCPD_PROG_NAME);
	pid = getPid(tmpBuf);
	if ( pid > 0)
		kill(pid, SIGUSR1);
	usleep(1000);

	if ( stat(_PATH_DHCPS_LEASES, &status) < 0 )
		goto err;

	buf = malloc(status.st_size);
	if ( buf == NULL )
		goto err;
	fp = fopen(_PATH_DHCPS_LEASES, "r");
	if ( fp == NULL )
		goto err;

	fread(buf, 1, status.st_size, fp);

	fclose(fp);

	ptr = buf;
	
	while (1) {
		ret = getOneDhcpClient(&ptr, &status.st_size, ipAddr, macAddr, liveTime);

		if (ret < 0)
			break;
		if (ret == 0)
			continue;
			
			
		nBytesSent += websWrite(wp,
			T("<tr bgcolor=#b7b7b7><td><font size=2>%s</td><td><font size=2>%s</td><td><font size=2>%s</td></tr>"),
			ipAddr, macAddr, liveTime);
		element++;
	}
err:
	if (element == 0) {
		nBytesSent += websWrite(wp,
			T("<tr bgcolor=#b7b7b7><td><font size=2>None</td><td><font size=2>----</td><td><font size=2>----</td></tr>"));
	}
	if (buf)
		free(buf);

	return nBytesSent;
}

/////////////////////////////////////////////////////////////////////////////
void formReflashClientTbl(webs_t wp, char_t *path, char_t *query)
{
	char_t *submitUrl;

	submitUrl = websGetVar(wp, T("submit-url"), T(""));
	if (submitUrl[0])
		websRedirect(wp, submitUrl);
}


//////////////////////////////////////////////////////////////////////////////
int isDhcpClientExist(char *name)
{
	char tmpBuf[100];
	struct in_addr intaddr;

	if ( getInAddr(name, IP_ADDR, (void *)&intaddr ) ) {
		snprintf(tmpBuf, 100, "%s/%s-%s.pid", _DHCPC_PID_PATH, _DHCPC_PROG_NAME, name);
		if ( getPid(tmpBuf) > 0)
			return 1;
	}
	return 0;
}



