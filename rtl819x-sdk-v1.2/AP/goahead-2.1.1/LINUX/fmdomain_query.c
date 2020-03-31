/*
 *      Web server handler routines for management (password, save config, f/w update)
 *
 *      Authors: sc_yang <sc_yang@realtek.com.tw>
 *
 *      $Id
 *
 */

#ifdef ROUTE_SUPPORT
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/reboot.h>
#include <unistd.h>
#include <net/route.h>
#include "../webs.h"
#include "../um.h"
#include "apmib.h"
#include "apform.h"
#include "utility.h"

//#define DNQ_DEBUG(fmt, args...) printf("[%s]%s %d:"fmt,__FILE__,__FUNCTION__ , __LINE__ , ## args)
#define DNQ_DEBUG(fmt, args...)

//#define DNQ_BR_DEBUG(fmt, args...) printf("[%s]%s %d:"fmt,__FILE__,__FUNCTION__ , __LINE__ , ## args)
#define DNQ_BR_DEBUG(fmt, args...)


#if 1//def HOME_GATEWAY

#if defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT)
typedef enum { LAN_NETWORK=0, WAN_NETWORK } DHCPC_NETWORK_TYPE_T;
typedef enum { WAIT_IP_STATE=0, MODIFY_DNS_STATE=1} WAIT_IP_STATE_T;
#define DHCPD_CONF_FILE "/var/udhcpd.conf"
#define DHCPD_PID_FILE "/var/run/udhcpd.pid"
#define DHCPD_LEASE_FILE "/var/lib/misc/udhcpd.leases"
#define WAIT_TIME 10
#define DHCPC_WAIT_TIME 20
static int CurrentTime=0;
int Start_Domain_Query_Process=1;
int WLAN_State=0; 
int DHCPD_State=1; 
int DHCPC_State=1; 
int Restore_IptablesRule=0;
int check_count=0;
//int State=0;
int is_vxd_connected=0;
int vxd_disable_period=0;
#define DEF_VXD_DISABLE_PERIOD 3
int Confirm_Time=0;
int Confirm_DHCP_Time=0;
int wlan_iface_state=0;
int confirm_wlan_iface_state=0;
char Last_SSID[SSID_LEN+1]={0};
int Renew_State=0;
int Last_Connect_Reason=0;
#if defined(LOGDEBUG_ENABLED)
#include <syslog.h> 
#endif


int getFilter_Type(void)
{
	FILE *fp;
	char *filter_conf_file = "/proc/pocket/filter_conf";
	char ip_addr[8];
	char mac_addr[12];
	int filter_type=0;
	
	fp= fopen(filter_conf_file, "r");
	if (!fp) {
        	printf("can not /proc/pocket/filter_conf\n");
		return -1;
   	}
	fscanf(fp,"%s %s %d",ip_addr,mac_addr, &filter_type);
	fclose(fp);
	
	return filter_type;
}

int write_line_to_file(char *filename, int mode, char *line_data)
{
	unsigned char tmpbuf[512];
	int fh=0;

	if(mode == 1) {/* write line datato file */
		
		fh = open(filename, O_RDWR|O_CREAT|O_TRUNC);
		
	}else if(mode == 2){/*append line data to file*/
		
		fh = open(filename, O_RDWR|O_APPEND);	
	}
	
	
	if (fh < 0) {
		fprintf(stderr, "Create %s error!\n", filename);
		return 0;
	}


	sprintf(tmpbuf, "%s", line_data);
	write(fh, tmpbuf, strlen(tmpbuf));



	close(fh);
	return 1;
}

int check_anyactive_stainfo(char *iface)
{
	FILE *stream;
	int result=0;
	char path[100];

	sprintf(path,"/proc/%s/sta_info",iface);
	stream = fopen ( path, "r" );
	if ( stream != NULL ) {		
		char *strtmp;
		char line[100];
		while (fgets(line, sizeof(line), stream))
		{
			unsigned char *p;
			strtmp = line;
			while(*strtmp == ' ')
				strtmp++;
				
			if(strstr(strtmp,"active") != 0){
				unsigned char str1[10];
						
				//-- STA info table -- (active: 1)
				sscanf(strtmp, "%*[^:]:%[^)]",str1);
						
				p = str1;
				while(*p == ' ')
					p++;										
				if(strcmp(p,"0") == 0){
					result=0;
				}else{
					result=1;						
				}										
				break;
			}
					
		}
		fclose(stream );	
	}
	return result;
}

int Check_Wlan_isConnected(int mode)
{
	int result=0;
	int LinkType=0, repeater_enabled=0;
	bss_info bss;

	apmib_get(MIB_REPEATER_ENABLED1 ,(void *)&repeater_enabled);
	
	if(mode==1){ //client mode
		result = check_anyactive_stainfo("wlan0");
	}else{
		if( repeater_enabled ) {
		//ap mode with repeater enabled ,we check vxd link status
		
			result = check_anyactive_stainfo("wlan0-vxd");
		} else {
		//ap mode , we check ethernet port phy link status
			if(getWanLink("eth1") < 0){
				//printf("ethernet is disconnect\n");
				result=0;
			}else {
				result=1;
			}
		}
	}

	if(mode==1){
		if(result==1){
			 getWlBssInfo("wlan0", &bss);
			 if(bss.state != STATE_CONNECTED){
			 	result=0;
			}
		}
	}
	else
	{
		if( repeater_enabled ) 
		{
		//ap mode with repeater enabled ,we check vxd link status
			//if(result==1)
			{
				getWlBssInfo("wlan0-vxd", &bss);

				if(bss.state != STATE_CONNECTED)
			 	{
			 		result=0;
				}
				else if(bss.state == STATE_CONNECTED)
				{
					result=1;
				}
			}
		}				
	}
	
	 if(mode==1 && CurrentTime >= WAIT_TIME){
	 	if(isFileExist("/etc/udhcpc/udhcpc-br0.pid")){
	 		LinkType = getFilter_Type();
	 		
			if(LinkType==0 && Confirm_DHCP_Time >= 5){
				Last_Connect_Reason=1; ///wlan can not got ip address, we modify phy link status, we treat it as phy link down
				#if defined(LOGDEBUG_ENABLED)
					syslog(LOG_INFO, "klogd: DNQP: DHCP Client started, but CanNot get Ip, treat link down\n");
					printf("DNQP: DHCP Client started, but CanNOT get Ip, treat link down\n");
				#endif
				
			}else if(LinkType==1
#if 0// mac clone for poacket ap client mode
 || LinkType == 2
#endif
){
				Last_Connect_Reason=0;///wlan can got ip address, we do not modify wlan phy link status 
				#if defined(LOGDEBUG_ENABLED)
					syslog(LOG_INFO, "klogd: DNQP: DHCP Client started, and got Ip, DONT Modify phy link status\n");
					printf("DNQP: DHCP Client started, and got Ip, DONT Modify phy link status\n");
				#endif
			}
	 	}
	 	if(Last_Connect_Reason==1){
	 		#if defined(LOGDEBUG_ENABLED)
					syslog(LOG_INFO, "klogd: DNQP: LastState CanNot get Ip, treat link down\n");
					printf("DNQP: LastState CanNOT get Ip, treat link down\n");
			#endif
	 		result=0;
	 	}
	}	
	return result;
}
void Create_script(char *script_path, char *iface, int network, char *ipaddr, char *mask, char *gateway)
{
	
	unsigned char tmpbuf[100];
	int fh;
	
	fh = open(script_path, O_RDWR|O_CREAT|O_TRUNC, S_IRWXO|S_IRWXG);	
	if (fh < 0) {
		fprintf(stderr, "Create %s file error!\n", script_path);
		return;
	}
	if(network==LAN_NETWORK){
		sprintf(tmpbuf, "%s", "#!/bin/sh\n");
		write(fh, tmpbuf, strlen(tmpbuf));
		//sprintf(tmpbuf, "%s\n", "echo \"br0 defconfig\"");
		//write(fh, tmpbuf, strlen(tmpbuf));
		//sprintf(tmpbuf, "ifconfig %s %s netmask %s 2> /dev/null\n", iface, ipaddr, mask);
		//write(fh, tmpbuf, strlen(tmpbuf));
		//sprintf(tmpbuf, "while route del default dev %s 2> /dev/null\n", iface);
		//write(fh, tmpbuf, strlen(tmpbuf));
		//sprintf(tmpbuf, "%s\n", "do :");
		//write(fh, tmpbuf, strlen(tmpbuf));
		//sprintf(tmpbuf, "%s\n", "done");
		//write(fh, tmpbuf, strlen(tmpbuf));
		//sprintf(tmpbuf, "route add -net default gw %s dev %s 2> /dev/null\n", gateway, iface);
	//	write(fh, tmpbuf, strlen(tmpbuf));
	//	sprintf(tmpbuf, "%s\n", "sysconf dhcpc deconfig br0");
	//	write(fh, tmpbuf, strlen(tmpbuf));
	}
	close(fh);
}

void set_lan_dhcpc(char *iface)
{
	char script_file[100], deconfig_script[100], pid_file[100];
	char *strtmp=NULL;
	char tmp[32], Ip[32], Mask[32], Gateway[32];
	char cmdBuff[200];
	unsigned char host_name[MAX_NAME_LEN]={0};

	sprintf(script_file, "/usr/share/udhcpc/%s.sh", iface); /*script path*/
	sprintf(deconfig_script, "/usr/share/udhcpc/%s.deconfig", iface);/*deconfig script path*/
	sprintf(pid_file, "/etc/udhcpc/udhcpc-%s.pid", iface); /*pid path*/
	apmib_get( MIB_IP_ADDR,  (void *)tmp);
	strtmp= inet_ntoa(*((struct in_addr *)tmp));
	sprintf(Ip, "%s",strtmp);
	
	apmib_get( MIB_SUBNET_MASK,  (void *)tmp);
	strtmp= inet_ntoa(*((struct in_addr *)tmp));
	sprintf(Mask, "%s",strtmp);
	
	apmib_get( MIB_DEFAULT_GATEWAY,  (void *)tmp);
	strtmp= inet_ntoa(*((struct in_addr *)tmp));
	sprintf(Gateway, "%s",strtmp);
	 
		
	Create_script(deconfig_script, iface, LAN_NETWORK, Ip, Mask, Gateway);
#if 1//#ifdef HOME_GATEWAY
	apmib_get( MIB_HOST_NAME,  (void *)host_name);
	if(host_name[0])
		sprintf(cmdBuff, "udhcpc -i %s -p %s -s %s -h %s &", iface, pid_file, script_file, host_name);
	else
#endif		
		sprintf(cmdBuff, "udhcpc -i %s -p %s -s %s &", iface, pid_file, script_file);

	system(cmdBuff);
}
void start_dnrd()
{
	unsigned char Ip[32], cmdBuffer[100], tmpBuff[200];
	unsigned char domanin_name[MAX_NAME_LEN];
	int wlan_mode=0;
	
	apmib_get( MIB_WLAN_MODE, (void *)&wlan_mode);

	system("killall -9 dnrd 2> /dev/null");
	apmib_get( MIB_IP_ADDR,  (void *)tmpBuff);
	sprintf(Ip, "%s", inet_ntoa(*((struct in_addr *)tmpBuff)));

	apmib_get( MIB_DOMAIN_NAME,  (void *)domanin_name);
	
	system("rm -f /var/hosts 2> /dev/null");
	memset(cmdBuffer, 0x00, sizeof(cmdBuffer));
	if(strlen(domanin_name) == 0)
	{
		if(getFilter_Type() != 1) //1:client
		{
			sprintf(cmdBuffer,"%s\\%s\n", Ip, "AlwaysHost");
		}
		else
		{
		if(wlan_mode==0){//ap mode	
				sprintf(cmdBuffer,"%s\\%s\n", Ip, "RealTekAP.com|RealTekAP.net");
		}else if(wlan_mode==1){//client mode
				sprintf(cmdBuffer,"%s\\%s\n", Ip, "RealTekCL.com|RealTekCL.net");
			}
		}
		write_line_to_file("/etc/hosts", 1, cmdBuffer);

	}
	else
	{
		if(getFilter_Type() != 1) //1:client
		{
			sprintf(cmdBuffer,"%s\\%s\n", Ip, "AlwaysHost");
		}
		else
		{
		if(wlan_mode==0){//ap mode	
				sprintf(cmdBuffer,"%s\\%s%s%s%s\n", Ip, domanin_name, "AP.com|",domanin_name, "AP.net");
		}else if(wlan_mode==1){
				sprintf(cmdBuffer,"%s\\%s%s%s%s\n", Ip, domanin_name, "CL.com|",domanin_name, "CL.net");
			}
		}
		write_line_to_file("/etc/hosts", 1, cmdBuffer);
		
	}
	system("dnrd --cache=off -s 168.95.1.1");

}
void set_lan_dhcpd(char *interface, int mode)
{
	char tmpBuff1[32]={0}, tmpBuff2[32]={0};
	int intValue=0, dns_mode=0;
	char line_buffer[100]={0};
	char tmp1[64]={0};
	char tmp2[64]={0};
	char *strtmp=NULL, *strtmp1=NULL;
	//DHCPRSVDIP_T entry;
	//int i, entry_Num=0;
#if 1//#ifdef   HOME_GATEWAY
	char tmpBuff3[32]={0};
#endif
	sprintf(line_buffer,"interface %s\n",interface);
	write_line_to_file(DHCPD_CONF_FILE, 1, line_buffer);
	
	apmib_get(MIB_DHCP_CLIENT_START,  (void *)tmp1);
	strtmp= inet_ntoa(*((struct in_addr *)tmp1));
	sprintf(line_buffer,"start %s\n",strtmp);
	write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
	
	apmib_get(MIB_DHCP_CLIENT_END,  (void *)tmp1);
	strtmp= inet_ntoa(*((struct in_addr *)tmp1));
	sprintf(line_buffer,"end %s\n",strtmp);
	write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
	
	apmib_get(MIB_SUBNET_MASK,  (void *)tmp1);
	strtmp= inet_ntoa(*((struct in_addr *)tmp1));
	sprintf(line_buffer,"opt subnet %s\n",strtmp);
	write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
	
	if(mode==1){//ap
		apmib_get( MIB_DEFAULT_GATEWAY,  (void *)tmp2);
		if (memcmp(tmp2, "\x0\x0\x0\x0", 4)){
			strtmp= inet_ntoa(*((struct in_addr *)tmp2));
			sprintf(line_buffer,"opt router %s\n",strtmp);
			write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
		}
		
		
	}else{
		apmib_get(MIB_IP_ADDR,  (void *)tmp1);
		strtmp= inet_ntoa(*((struct in_addr *)tmp1));
		sprintf(line_buffer,"opt router %s\n",strtmp);
		write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
#if defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT)		
		dns_mode = 0;
#if defined(HOME_GATEWAY)
		apmib_get( MIB_WAN_DNS_MODE, (void *)&dns_mode);
#endif		
		if(dns_mode==0)
		{
			sprintf(line_buffer,"opt dns %s\n",strtmp); /*now strtmp is ip address value */
			write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
		}
#endif
	}	
	if((mode==1) ||(mode==2 && dns_mode==1)){
#if 1//#ifdef   HOME_GATEWAY		
		apmib_get( MIB_WAN_DNS1,  (void *)tmpBuff1);
		apmib_get( MIB_WAN_DNS2,  (void *)tmpBuff2);
		apmib_get( MIB_WAN_DNS3,  (void *)tmpBuff3);
	
		if (memcmp(tmpBuff1, "\x0\x0\x0\x0", 4)){
			strtmp= inet_ntoa(*((struct in_addr *)tmpBuff1));
			sprintf(line_buffer,"opt dns %s\n",strtmp);
			write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
			intValue++;
		}
		if (memcmp(tmpBuff2, "\x0\x0\x0\x0", 4)){
			strtmp= inet_ntoa(*((struct in_addr *)tmpBuff2));
			sprintf(line_buffer,"opt dns %s\n",strtmp);
			write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
			intValue++;
		}
		if (memcmp(tmpBuff3, "\x0\x0\x0\x0", 4)){
			strtmp= inet_ntoa(*((struct in_addr *)tmpBuff3));
			sprintf(line_buffer,"opt dns %s\n",strtmp);
			write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
			intValue++;
		}
#endif		
		if(intValue==0){ /*no dns option for dhcp server, use default gatewayfor dns opt*/
			
			if(mode==1){
				apmib_get( MIB_DEFAULT_GATEWAY,  (void *)tmp2);
				if (memcmp(tmp2, "\x0\x0\x0\x0", 4)){
					strtmp= inet_ntoa(*((struct in_addr *)tmp2));
					sprintf(line_buffer,"opt dns %s\n",strtmp);
					write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
				}
			}else {
				apmib_get( MIB_IP_ADDR,  (void *)tmp2);
				if (memcmp(tmp2, "\x0\x0\x0\x0", 4)){
					strtmp= inet_ntoa(*((struct in_addr *)tmp2));
					sprintf(line_buffer,"opt dns %s\n",strtmp);
					write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
				}
			}
		}
	}
	memset(tmp1, 0x00, 64);
	apmib_get( MIB_DOMAIN_NAME, (void *)&tmp1);
	if(tmp1[0]){
		sprintf(line_buffer,"opt domain %s\n",tmp1);
		write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
	}

	/* may not need to set ip again*/
	apmib_get(MIB_IP_ADDR,  (void *)tmp1);
	strtmp= inet_ntoa(*((struct in_addr *)tmp1));
	sprintf(tmpBuff1, "%s", strtmp);
	apmib_get(MIB_SUBNET_MASK,  (void *)tmp2);
	strtmp1= inet_ntoa(*((struct in_addr *)tmp2));
	sprintf(tmpBuff2, "%s", strtmp1);
	
	sprintf(line_buffer, "ifconfig %s %s netmask %s", interface, tmpBuff1, tmpBuff2);
	system(line_buffer);
	
	sprintf(line_buffer, "udhcpd %s", DHCPD_CONF_FILE);
	system(line_buffer);
	start_dnrd();
}
int getLan_MacAddress(unsigned char *dst)
{
	struct sockaddr hwaddr;
	unsigned char *pMacAddr;
	 if(getInAddr("br0", HW_ADDR, (void *)&hwaddr)){ 
		pMacAddr = hwaddr.sa_data;
		sprintf(dst, "%02X%02X%02X%02X%02X%02X",pMacAddr[0], pMacAddr[1], pMacAddr[2], pMacAddr[3],pMacAddr[4],pMacAddr[5]); 
		return 1;
	}
	return 0;
}

#if 0// mac clone for poacket ap client mode
int getWlan_MacAddress(unsigned char *dst)
{
	struct sockaddr hwaddr;
	unsigned char *pMacAddr;
	 if(getInAddr("wlan0", HW_ADDR, (void *)&hwaddr)){ 
		pMacAddr = hwaddr.sa_data;
		sprintf(dst, "%02X%02X%02X%02X%02X%02X",pMacAddr[0], pMacAddr[1], pMacAddr[2], pMacAddr[3],pMacAddr[4],pMacAddr[5]); 
		return 1;
	}
	return 0;
}
#endif

extern void translate_control_code(char *buffer);
int check_ssid()
{
	char buffer[128];
	bss_info bss;
	int ret=0;
		if(getWlBssInfo("wlan0", &bss) < 0)
			return -1;
		memcpy(buffer, bss.ssid, SSID_LEN+1);
		translate_control_code(buffer);
		if(Last_SSID[0]){
			if(memcmp(Last_SSID, buffer,SSID_LEN+1)){
				memcpy(Last_SSID, buffer, SSID_LEN+1);
				ret =1;
			}
		}else{
			if(buffer[0])
				memcpy(Last_SSID, buffer, SSID_LEN+1);
			else
				memset(Last_SSID, 0x00, SSID_LEN+1);
		}
		
	return ret;
}

void Kill_Wlan_Applications(void)
{
	system("killall -9 wscd 2> /dev/null");
	system("killall -9 iwcontrol 2> /dev/null");
	system("killall -9 auth 2> /dev/null");
	system("killall -9 disc_server 2> /dev/null");
	system("killall -9 iapp 2> /dev/null");	
	system("killall -9 mini_upnpd 2> /dev/null");
}

void Start_Wlan_Applications(void)
{
	
	int repeater_enabled=0;
	apmib_get(MIB_REPEATER_ENABLED1, (void *)&repeater_enabled);
	if(repeater_enabled == 1)
		system("sysconf wlanapp start wlan0 wlan0-vxd br0");
	else
	system("sysconf wlanapp start wlan0 br0");
		
	sleep(1);
	/*sysconf upnpd 1(isgateway) 1(opmode is bridge)*/
	system("sysconf upnpd 1 1");
	sleep(1);
}




int Domain_query_Process()
{
	int i;
	int Operation_Mode=0;
	int WLAN_Mode=0;
	unsigned char LAN_Mac[12];
	char cmdBuffer[100];
	int lan_dhcp=0;
	char lan_domain_name[MAX_NAME_LEN]={0};
	unsigned char Confirm_Threshold=0;
	int Type=0, repeater_enabled=0;
	int Check_status=0;
	
	apmib_get( MIB_DOMAIN_NAME, (void *)lan_domain_name);
		
	if(Start_Domain_Query_Process==0 ||isFileExist("/var/system/start_init") || !lan_domain_name[0]){//during init procedure 
		//printf("web init return directly\n");
		return 0;
	}

	apmib_get( MIB_OP_MODE, (void *)&Operation_Mode);
	apmib_get( MIB_WLAN_MODE, (void *)&WLAN_Mode);
	apmib_get( MIB_DHCP, (void *)&lan_dhcp);
	apmib_get(MIB_REPEATER_ENABLED1, (void *)&repeater_enabled);
	if(
#if defined(CONFIG_RTL8196C_AP_ROOT)
#else
		Operation_Mode==1 &&
#endif
		lan_dhcp==15 && ((WLAN_Mode == 1) || (WLAN_Mode == 0))){ //in bridge mode and wlan ap/client mode
		if(CurrentTime < WAIT_TIME){ //during this period(20s),keep checking wlan connected or not
			/*if wlan is connected will not start dhcp server*/
			Check_status = Check_Wlan_isConnected(WLAN_Mode);
			if( Check_status==1 && DHCPD_State==1){ //wlan connetced
				DNQ_DEBUG("Check_status=1 && DHCPD_State=1\n");
				//printf("wlan connected within WAIT_TIME and start dhcpc\n");
				system("killall -9 udhcpd 2> /dev/null");
				system("killall -9 udhcpc 2> /dev/null");
				#if defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT)
				system("echo 1 > /proc/pocket/en_filter"); //start to filter dhcp discover in bridge 
				if(getLan_MacAddress(LAN_Mac)){
					sprintf(cmdBuffer, "echo \"00000000 %s 0\" > /proc/pocket/filter_conf", LAN_Mac);
					system(cmdBuffer);
				}else{
					system("echo \"00000000 000000000000 0\" > /proc/pocket/filter_conf");
				}
				#endif	//CONFIG_DOMAIN_NAME_QUERY_SUPPORT
				system("rm -f /var/run/udhcpd.pid 2> /dev/null");
				system("rm -f /etc/udhcpc/udhcpc-br0.pid 2> /dev/null");
				
				#if defined(LOGDEBUG_ENABLED)
				if(WLAN_Mode == 1){
					syslog(LOG_INFO, "klogd: DNQP: Connected within WAIT_TIME, Start DHCP Client\n");
					printf("DNQP: Connected within WAIT_TIME, Start DHCP Client\n");
				}
				#endif
				set_lan_dhcpc("br0");
				DHCPC_State=2; // start DHCP client
				DHCPD_State=0;	//bypass connectivity checcking then

				WLAN_State=1;
				if(WLAN_Mode == 0){
				#if defined(LOGDEBUG_ENABLED)
					syslog(LOG_INFO, "klogd: DNQP: AP mode, within WAIT_TIME, Start DHCP Client, shutdown wlan first\n");
					printf("DNQP: AP mode, within WAIT_TIME, Start DHCP Client, shutdown wlan first\n");
				#endif
					//turn off root interface/AP-mode while repeater enabled before auto IP decision
					if( repeater_enabled )
						system("iwpriv wlan0 set_mib func_off=255");
					else
						system("ifconfig wlan0 down");

					Kill_Wlan_Applications();
				}
				wlan_iface_state=1;
			}else if(Check_status ==0 && DHCPD_State==1){
					DNQ_DEBUG("Check_status=0 && DHCPD_State=1\n");
					if(isFileExist(DHCPD_PID_FILE)==0){
						system("killall -9 udhcpd 2> /dev/null");
						system("rm -f /var/run/udhcpd.pid 2> /dev/null");
						#if defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT)
						system("echo 1 > /proc/pocket/en_filter"); //start to filter dhcp discover in bridge 
						if(getLan_MacAddress(LAN_Mac)){
							sprintf(cmdBuffer, "echo \"00000000 %s 2\" > /proc/pocket/filter_conf", LAN_Mac);
							system(cmdBuffer);
						}else{
							system("echo \"00000000 000000000000 2\" > /proc/pocket/filter_conf");
						}
						#endif	//CONFIG_DOMAIN_NAME_QUERY_SUPPORT
						#if defined(LOGDEBUG_ENABLED)
						if(WLAN_Mode == 1){
							syslog(LOG_INFO, "klogd: DNQP: Disconnected within WAIT_TIME, Start DHCP Server\n");
							printf("DNQP: Disconnected within WAIT_TIME, Start DHCP Server\n");
						}
						#endif	
						set_lan_dhcpd("br0", 2);
						
						for(i=0;i<3;i++){
							 if(isFileExist(DHCPD_PID_FILE)){
							 	break;
							 }else{
							 	sleep(1);
							}
						}
						if(WLAN_Mode == 1){
							system("ifconfig eth0 down");
							system("ifconfig eth1 down");
							sleep(5);
							system("ifconfig eth0 up");
							system("ifconfig eth1 up");
						}
						if(WLAN_Mode == 0){
							#if defined(LOGDEBUG_ENABLED)
								syslog(LOG_INFO, "klogd: DNQP: AP mode, within WAIT_TIME, Start DHCP Server, shutdown wlan first\n");
								printf("DNQP: AP mode, within WAIT_TIME, Start DHCP Server, shutdown wlan first\n");
							#endif
							//turn off root interface/AP-mode while repeater enabled before auto IP decision
							if( repeater_enabled )
								system("iwpriv wlan0 set_mib func_off=255");
							else
								system("ifconfig wlan0 down");

							Kill_Wlan_Applications();
						}
						wlan_iface_state=1;
#if defined(CONFIG_RTL8196C_AP_ROOT)
#else
						system("iptables -F INPUT");
#endif
						WLAN_State=0;					
					}
			}
	
		}else if(CurrentTime >= WAIT_TIME){ //it's time to check wlan connect or not
			Check_status = Check_Wlan_isConnected(WLAN_Mode);
			if(Check_status ==0){ //wlan/eth1  is not connetced
				DNQ_BR_DEBUG("CurrentTime:%d >= %d && Check_status=0\n",CurrentTime,WAIT_TIME);
				//disconnect state
				if(isFileExist(DHCPD_PID_FILE)==0){ //dhcp server is not running
					DNQ_DEBUG("DHCPD is NOT running...\n");
					if(WLAN_Mode==0) {
						Confirm_Threshold=0;
						if( repeater_enabled )
							Confirm_Threshold=5;
					}

					if(WLAN_Mode==1)
						Confirm_Threshold=20;

					if(WLAN_State==1 && Confirm_Time < Confirm_Threshold){
						Confirm_Time++;
						goto ToNext;
					}
#if 0// mac clone for poacket ap client mode					
printf("\r\n wlan/eth1  is not connetced!!__[%s-%u]\r\n",__FILE__,__LINE__);

if(getLan_MacAddress(LAN_Mac))
{
	unsigned char wlan0Mac[12];
	
printf("\r\n LAN_Mac=[%s]__[%s-%u]\r\n",LAN_Mac,__FILE__,__LINE__);
	
	if(getWlan_MacAddress(wlan0Mac))
	{
		
printf("\r\n wlan0Mac=[%s]__[%s-%u]\r\n",wlan0Mac,__FILE__,__LINE__);		
		if(memcmp(wlan0Mac, LAN_Mac, sizeof(LAN_Mac)) != 0)
		{
			sprintf(cmdBuffer, "ifconfig wlan0 hw ether %s", LAN_Mac);
			system(cmdBuffer);
		}
	}
}
#endif //#if 0// mac clone for poacket ap client mode
					
					Confirm_Time=0;
					if(isFileExist("/etc/udhcpc/udhcpc-br0.pid")){
						system("killall -9 udhcpc 2> /dev/null");
						system("rm -f /etc/udhcpc/udhcpc-br0.pid 2> /dev/null");	
					}
					sleep(1);
					system("killall -9 udhcpd 2> /dev/null");
					system("rm -f /var/run/udhcpd.pid 2> /dev/null");
					#if defined(LOGDEBUG_ENABLED)
					if(WLAN_Mode == 1){
						syslog(LOG_INFO, "klogd: DNQP: Disconnected after WAIT_TIME, Start DHCP Server\n");
						DNQ_DEBUG("DNQP: Disconnected after WAIT_TIME, Start DHCP Server\n");
					}
					#endif
					set_lan_dhcpd("br0", 2);
					
					for(i=0;i<3;i++){
						 if(isFileExist(DHCPD_PID_FILE)){
						 	break;
						 }else{
						 	sleep(1);
						}
					}
					#if defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT)
					system("echo 1 > /proc/pocket/en_filter"); //start to filter dhcp discover in bridge 
					if(getLan_MacAddress(LAN_Mac)){
						sprintf(cmdBuffer, "echo \"00000000 %s 2\" > /proc/pocket/filter_conf", LAN_Mac);
						system(cmdBuffer);
					}else{
						system("echo \"00000000 000000000000 2\" > /proc/pocket/filter_conf");
					}
					#endif	//CONFIG_DOMAIN_NAME_QUERY_SUPPORT
					if(WLAN_Mode == 1){
						system("ifconfig eth0 down");
						system("ifconfig eth1 down");
						sleep(5);
						system("ifconfig eth0 up");
						system("ifconfig eth1 up");
					}
					if(WLAN_Mode == 0){
						#if defined(LOGDEBUG_ENABLED)
							syslog(LOG_INFO, "klogd: DNQP: AP mode, after WAIT_TIME, Start DHCP Server, shutdown wlan first\n");
							DNQ_DEBUG("DNQP: AP mode, after WAIT_TIME, Start DHCP Server, shutdown wlan first\n");
						#endif
						//turn off root interface/AP-mode while repeater enabled before auto IP decision
						if( repeater_enabled )
							system("iwpriv wlan0 set_mib func_off=255");
						else
							system("ifconfig wlan0 down");

						Kill_Wlan_Applications();
					}
					wlan_iface_state=1;	
#if defined(CONFIG_RTL8196C_AP_ROOT)
#else
					system("iptables -F INPUT");
#endif
					//if(WLAN_Mode == 0)
					{
						WLAN_State=0;
					}

				}else{
					DNQ_BR_DEBUG("DHCPD is running...\n");
					//dhcp server is running, and not connected
						if(isFileExist("/etc/udhcpc/udhcpc-br0.pid")){
							DNQ_DEBUG(",but DHCP-Client is running...\n");
							system("killall -9 udhcpc 2> /dev/null");
							system("rm -f /etc/udhcpc/udhcpc-br0.pid 2> /dev/null");
							#if defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT)
							if(getLan_MacAddress(LAN_Mac)){
								sprintf(cmdBuffer, "echo \"00000000 %s 2\" > /proc/pocket/filter_conf", LAN_Mac);
								system(cmdBuffer);
							}else{
								system("echo \"00000000 000000000000 2\" > /proc/pocket/filter_conf");
							}
							#endif	//CONFIG_DOMAIN_NAME_QUERY_SUPPORT
						}else{
							DNQ_BR_DEBUG("DHCP-Client is NOT running...\n");
							Type = getFilter_Type();
							#if defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT)
							if(Type==0){
								#if defined(LOGDEBUG_ENABLED)
									syslog(LOG_INFO, "klogd: DNQP: Disconnected after WAIT_TIME, Filter state is 0, reset to 2 in DHCP server state\n");
									DNQ_DEBUG("DNQP: AP mode, Disconnected after WAIT_TIME, Filter state is 0, reset to 2 in DHCP server state\n");
								#endif
								system("echo \"00000000 000000000000 2\" > /proc/pocket/filter_conf");
							}else if(Type==2 && WLAN_State==1){
							#else
							if(Type==2 && WLAN_State==1){
							#endif	//CONFIG_DOMAIN_NAME_QUERY_SUPPORT
								Renew_State++;
							}
							if(WLAN_Mode == 0 && Renew_State >= 1 && WLAN_State==1){
								WLAN_State=0;
								Renew_State=0;
								#if defined(LOGDEBUG_ENABLED)
									syslog(LOG_INFO, "klogd: DNQP: AP mode, Disconnected after WAIT_TIME, LastState is Connected, clean WLAN_State to cause start DHCP client when Connect again\n");
									DNQ_DEBUG("DNQP: AP mode, Disconnected after WAIT_TIME, LastState is Connected, clean WLAN_State to cause start DHCP client when Connect again\n");
								#endif
							}					
						}
				}
				//turn off root interface/AP for a while(3s) after vxd link down detected
				if( is_vxd_connected != Check_status && WLAN_Mode == 0 ){
					if( repeater_enabled ) {
						DNQ_DEBUG("Disable eth/wlan to make wireless/wired client leave becuz of disconnected vdx\n");
						system("ifconfig wlan0 down");
						system("ifconfig eth0 down");
						system("ifconfig eth1 down");
						vxd_disable_period = DEF_VXD_DISABLE_PERIOD;
						is_vxd_connected = Check_status;
						system("iwpriv wlan0 set_mib func_off=255;");
					}
				}
			} else {
				DNQ_BR_DEBUG("CurrentTime:%d >= %d && Check_status=1\n",CurrentTime,WAIT_TIME);
				//connected state
				if(WLAN_State ==0){
					system("killall -9 udhcpd 2> /dev/null");
					system("killall -9 udhcpc 2> /dev/null");
					system("rm -f /var/run/udhcpd.pid 2> /dev/null");
					system("rm -f /etc/udhcpc/udhcpc-br0.pid 2> /dev/null");
					#if defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT)
					system("echo 1 > /proc/pocket/en_filter"); //start to filter dhcp discover in bridge 
					if(getLan_MacAddress(LAN_Mac)){
						sprintf(cmdBuffer, "echo \"00000000 %s 0\" > /proc/pocket/filter_conf", LAN_Mac);
						system(cmdBuffer);
					}else{
						system("echo \"00000000 000000000000 0\" > /proc/pocket/filter_conf");
					}
					#endif	//CONFIG_DOMAIN_NAME_QUERY_SUPPORT

					#if defined(LOGDEBUG_ENABLED)
					if(WLAN_Mode == 1){
						syslog(LOG_INFO, "klogd: DNQP: Connected after WAIT_TIME, Start DHCP Client\n");
						DNQ_DEBUG("DNQP: Connected after WAIT_TIME, Start DHCP Client\n");
					}
					#endif
					set_lan_dhcpc("br0");
					DHCPC_State=2; // start DHCP client					
					
#if defined(CONFIG_RTL8196C_AP_ROOT)
#else
					system("iptables -A INPUT -p icmp --icmp-type echo-request -i br0 -j DROP");
#endif
//					sleep(1);
					WLAN_State=1;
					Restore_IptablesRule=1;
					check_count=0;
					if(WLAN_Mode == 0){
						#if defined(LOGDEBUG_ENABLED)
							syslog(LOG_INFO, "klogd: DNQP: AP mode, after WAIT_TIME, Start DHCP Client, shutdown wlan first\n");
							DNQ_DEBUG("DNQP: AP mode, after WAIT_TIME, Start DHCP Client, shutdown wlan first\n");
						#endif
						//turn off root interface/AP-mode while repeater enabled before auto IP decision
						if( repeater_enabled )
							system("iwpriv wlan0 set_mib func_off=255");
						else
							system("ifconfig wlan0 down");
						Kill_Wlan_Applications();
					}
						wlan_iface_state=1;
				}else{
					if(Restore_IptablesRule==1){
						if(isFileExist(DHCPD_PID_FILE)){
							unlink(DHCPD_PID_FILE);
						}
						if(check_count > 5){
#if defined(CONFIG_RTL8196C_AP_ROOT)
#else
							system("iptables -F INPUT");
#endif
							Restore_IptablesRule=0;
						}else{
							check_count++;
						}
					}
					
					if(getFilter_Type()==0){
						if(Confirm_DHCP_Time >= DHCPC_WAIT_TIME && (DHCPC_State==1 || DHCPC_State==2)){
							system("killall -9 udhcpd 2> /dev/null");
							system("killall -9 udhcpc 2> /dev/null");
							sleep(1);
							system("rm -f /var/run/udhcpd.pid 2> /dev/null");
							system("rm -f /etc/udhcpc/udhcpc-br0.pid 2> /dev/null");
							
							
						#if defined(LOGDEBUG_ENABLED)
						if(WLAN_Mode == 1){
							syslog(LOG_INFO, "klogd: DNQP: Connected after WAIT_TIME,  DHCP Client start, and CanNOT get ip after 20 seconds, Start DHCP Server\n");
							DNQ_DEBUG("DNQP: Connected after WAIT_TIME,  DHCP Client start, and CanNOT get ip after 20 seconds, Start DHCP Server\n");
						}
						#endif	
							
							set_lan_dhcpd("br0", 2);
					
							for(i=0;i<3;i++){
							 if(isFileExist(DHCPD_PID_FILE)){
						 		break;
							 }else{
						 		sleep(1);
								}
							}
							#if defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT)
							system("echo 1 > /proc/pocket/en_filter"); //start to filter dhcp discover in bridge 
							if(getLan_MacAddress(LAN_Mac)){
							sprintf(cmdBuffer, "echo \"00000000 %s 2\" > /proc/pocket/filter_conf", LAN_Mac);
							system(cmdBuffer);
							}else{
								system("echo \"00000000 000000000000 2\" > /proc/pocket/filter_conf");
							}
							#endif	//CONFIG_DOMAIN_NAME_QUERY_SUPPORT
							if(WLAN_Mode == 1){
							system("ifconfig eth0 down");
							system("ifconfig eth1 down");
							sleep(5);
							system("ifconfig eth0 up");
							system("ifconfig eth1 up");
							}
							if(WLAN_Mode == 0){
						#if defined(LOGDEBUG_ENABLED)
							syslog(LOG_INFO, "klogd: DNQP: AP mode, Connected after WAIT_TIME,  DHCP Client start, and CanNOT get ip after 20 seconds, Start DHCP Server, shutdown wlan first\n");
							DNQ_DEBUG("DNQP: AP mode, Connected after WAIT_TIME,  DHCP Client start, and CanNOT get ip after 20 seconds, Start DHCP Server, shutdown wlan first\n");
						#endif
								//turn off root interface/AP-mode while repeater enabled before auto IP decision
								if( repeater_enabled )
									system("iwpriv wlan0 set_mib func_off=255");
								else
									system("ifconfig wlan0 down");
								Kill_Wlan_Applications();
							}
							wlan_iface_state=1;	
#if defined(CONFIG_RTL8196C_AP_ROOT)
#else
							system("iptables -F INPUT");
#endif
							if(WLAN_State==0)
								WLAN_State=1;
							DHCPC_State=0;
						}else
							Confirm_DHCP_Time++;
					}else{

#if 0// mac clone for poacket ap client mode
printf("\r\n wlan0_clone_mac=[%s]__[%s-%u]\r\n",wlan0_clone_mac,__FILE__,__LINE__);

if(getLan_MacAddress(LAN_Mac))
{
	unsigned char wlan0Mac[12];
	
printf("\r\n LAN_Mac=[%s]__[%s-%u]\r\n",LAN_Mac,__FILE__,__LINE__);
	
	if(getWlan_MacAddress(wlan0Mac))
	{
printf("\r\n wlan0Mac=[%s]__[%s-%u]\r\n",wlan0Mac,__FILE__,__LINE__);		

		if(strlen(wlan0_clone_mac)==0 && memcmp(wlan0Mac,LAN_Mac, sizeof(LAN_Mac))!=0)
		{
printf("\r\n __[%s-%u]\r\n",__FILE__,__LINE__);			
			strncpy(wlan0_clone_mac,wlan0Mac, sizeof(wlan0Mac));
printf("\r\n LAN_Mac=[%s]__[%s-%u]\r\n",LAN_Mac,__FILE__,__LINE__);					
			strncpy(br0_mac, LAN_Mac, sizeof(LAN_Mac));
printf("\r\n br0_mac=[%s],__[%s-%u]\r\n",br0_mac,__FILE__,__LINE__);			
		}
		else
		{
printf("\r\n __[%s-%u]\r\n",__FILE__,__LINE__);			
			if(strlen(wlan0_clone_mac)!=0 && memcmp(wlan0Mac,wlan0_clone_mac, sizeof(wlan0Mac))!=0)
			{
printf("\r\n wlan0_clone_mac=[%s],__[%s-%u]\r\n",wlan0_clone_mac,__FILE__,__LINE__);				
printf("\r\n br0_mac=[%s],__[%s-%u]\r\n",br0_mac,__FILE__,__LINE__);				
				sprintf(cmdBuffer, "ifconfig wlan0 hw ether %s", wlan0_clone_mac);
printf("\r\n cmdBuffer=[%s],__[%s-%u]\r\n",cmdBuffer,__FILE__,__LINE__);								
				system(cmdBuffer);
				sprintf(cmdBuffer, "ifconfig br0 hw ether %s", br0_mac);
printf("\r\n cmdBuffer=[%s],__[%s-%u]\r\n",cmdBuffer,__FILE__,__LINE__);								
				system(cmdBuffer);

			}
			
		}		
	}
	
}
#endif //#if 0// mac clone for poacket ap client mode

						Confirm_DHCP_Time=0;
						if (DHCPC_State == 2){
							if(WLAN_Mode == 1){
								system("ifconfig eth0 down");
								system("ifconfig eth1 down");
								sleep(5);
								system("ifconfig eth0 up");
								system("ifconfig eth1 up");
							}
							
							if(WLAN_Mode == 0){
								//turn on root interface/AP while repeater enabled after auto IP decision
								DNQ_DEBUG("Turn on root interface/AP while repeater enabled after auto IP decision\n");
								if( repeater_enabled )
									system("iwpriv wlan0 set_mib func_off=0");
								else
									system("ifconfig wlan0 down");
								Kill_Wlan_Applications();
							}
							wlan_iface_state=1;
#if defined(CONFIG_RTL8196C_AP_ROOT)
#else
							system("iptables -F INPUT");
#endif
							
						}						
						DHCPC_State=1;
					}
						
				}
				
				if(WLAN_Mode ==1){
					if(WLAN_State==1 && (Confirm_Time >= 3 && Confirm_Time < 20) &&getFilter_Type()==1){
						//printf("client ever disconnect for 3~20 seconds, we should update our ip address\n");
						#if defined(LOGDEBUG_ENABLED)
							syslog(LOG_INFO, "klogd: DNQP: Client mode, client ever disconnect for 3~20 seconds, we should update our ip address\n");
							printf("DNQP: Client mode, client ever disconnect for 3~20 seconds, we should update our ip address\n");
						#endif
						WLAN_State=0;
						Confirm_Time=0;
					}
				}
			}
			//turn off root interface/AP for a while(3s) after vxd link down detected
			if( is_vxd_connected != Check_status && WLAN_Mode == 0 ){
				if( repeater_enabled ) {
					DNQ_DEBUG("Disable eth/wlan to make wired/wireless client join becuz of connected vxd\n");
					system("ifconfig wlan0 down");
					system("ifconfig eth0 down");
					system("ifconfig eth1 down");
					vxd_disable_period = DEF_VXD_DISABLE_PERIOD;
					is_vxd_connected = Check_status;
					system("iwpriv wlan0 set_mib func_off=255;");
				}
			}
		}
ToNext:		
		if(CurrentTime > WAIT_TIME+5)
			CurrentTime=WAIT_TIME;
		else
			CurrentTime++;

			if( vxd_disable_period > 0 ) {
				vxd_disable_period--;

				if( vxd_disable_period == 0 ) {
					DNQ_DEBUG("Enable eth/wlan to make wired/wireless client join becuz of connected vdx\n");
					system("ifconfig eth0 up");
					system("ifconfig eth1 up");
					system("ifconfig wlan0 up");
				}
			}
			
			if(wlan_iface_state==1){
				Type = getFilter_Type();
				if(Type==2 ||Type==1 ){ //dhcp server state, we wait 8 seconds, type=2:server state, type=1:dhcp client and got ip address
					confirm_wlan_iface_state++;
					if(confirm_wlan_iface_state >=8){
						if(WLAN_Mode == 0){
						#if defined(LOGDEBUG_ENABLED)
							if(Type==2){
								syslog(LOG_INFO, "klogd: DNQP: AP mode, Up wlan interface when dhcp server started\n");
								printf("DNQP: AP mode, Up wlan interface when dhcp server started\n");
							}else if (Type==1){
								syslog(LOG_INFO, "klogd: DNQP: AP mode, Up wlan interface when dhcp client got ip\n");
								printf("DNQP: AP mode, Up wlan interface when dhcp client got ip\n");
							}
						#endif
							//turn on root interface/AP-mode while repeater enabled after auto IP decision
							if( repeater_enabled )
								system("iwpriv wlan0 set_mib func_off=0");
							else
								system("ifconfig wlan0 up");
							sleep(1);
							Start_Wlan_Applications();
						}
							wlan_iface_state=0;
							confirm_wlan_iface_state=0;
					}
				}
			}
			
		#if 1
			if(WLAN_State==1 && check_ssid() && CurrentTime >= WAIT_TIME){
				WLAN_State=0;//restart all procedure
#if defined(CONFIG_RTL8196C_AP_ROOT)
#else
				system("iptables -F INPUT");//to avoid add iptables rule duplicate
#endif
			}
		#endif
	}
#if defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT)
	else{
		system("echo 0 > /proc/pocket/en_filter");
	}
#endif	//CONFIG_DOMAIN_NAME_QUERY_SUPPORT

	return 0;
}


void Confirm_Chld_termniated(void)
{
	if(Start_Domain_Query_Process==0)
	{
		Start_Domain_Query_Process=1;
		WLAN_State=0;//restart all procedure
		Last_Connect_Reason=0;
#if defined(CONFIG_RTL8196C_AP_ROOT)
#else
		system("iptables -F INPUT");//to avoid add iptables rule duplicate
#endif
	}
}
void Stop_Domain_Query_Process(void)
{
	Start_Domain_Query_Process=0;
}

void Reset_Domain_Query_Setting(void)
{
	int opmode=-1;
	int lan_dhcp_mode=0;
	int wlan_mode_root=0;
	struct sockaddr hwaddr;
	unsigned char *pMacAddr;
	unsigned char cmdBuffer[100];

	char lan_domain_name[	MAX_NAME_LEN]={0};
	
	
	apmib_get( MIB_DOMAIN_NAME, (void *)lan_domain_name);
	apmib_get(MIB_OP_MODE,(void *)&opmode);
	apmib_get(MIB_DHCP,(void *)&lan_dhcp_mode);
	apmib_get( MIB_WLAN_MODE, (void *)&wlan_mode_root); 
	if(opmode==1 &&((wlan_mode_root==1 && lan_dhcp_mode==15) || (wlan_mode_root==0 && lan_dhcp_mode==15)) && lan_domain_name[0]){
		system("echo 1 > /proc/pocket/en_filter"); 
		 if(getInAddr("br0", HW_ADDR, (void *)&hwaddr)){ 
			pMacAddr = hwaddr.sa_data;
			sprintf(cmdBuffer, "echo \"%s %02X%02X%02X%02X%02X%02X 0\" > /proc/pocket/filter_conf","00000000", pMacAddr[0], pMacAddr[1],pMacAddr[2], pMacAddr[3], pMacAddr[4], pMacAddr[5]); 
		}else{
			sprintf(cmdBuffer, "echo \"%s 000000000000 0\" > /proc/pocket/filter_conf","00000000"); 
		}
			system(cmdBuffer);
	}else{
			system("echo 0 > /proc/pocket/en_filter"); 
	}
}	
#endif 

#endif	//HOME_GATEWAY

#endif	//ROUTER_SUPPORT
