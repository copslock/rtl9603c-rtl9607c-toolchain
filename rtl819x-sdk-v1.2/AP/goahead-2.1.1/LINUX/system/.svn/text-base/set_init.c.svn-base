

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "../apmib.h"
#include "../mibtbl.h"
#include "sysconf.h"
#include "sys_utility.h"
#include "syswan.h"
//extern int wlan_idx;	// interface index
//extern int vwlan_idx;	// initially set interface index to root
extern int set_QoS(int operation, int wan_type, int wisp_wan_id);
extern int setbridge(char *argv);
extern int setWlan_Applications(char *action, char *argv);
extern int SetWlan_idx(char *wlan_iface_name);
extern int setFirewallIptablesRules(int argc, char** argv);
extern void set_lan_dhcpd(char *interface, int mode);
extern void wan_disconnect(char *option);
void set_log(void);
#if defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT)
extern void Init_Domain_Query_settings(int operation_mode, int wlan_mode, int lan_dhcp_mode, char *lan_mac);
extern void domain_query_start_dnrd(int wlan_mode, int start_dnrd);
#endif
/*

//eth0 eth1 eth2 eth3 eth4 wlan0 wlan0-msh wlan0-va0 wlan0-va1 wlan0-va2 wlan0-va3 wlan0-vxd
//wlan0-wds0 wlan0-wds1 wlan0-wds2 wlan0-wds3 wlan0-wds4 wlan0-wds5 wlan0-wds6 wlan0-wds7

WLAN=>>> wlan0
WLANVXD=>>>wlan0-vxd
WLANVIRTUAL=>> wlan0-va0 wlan0-va1 wlan0-va2 wlan0-va3 wlan0-vxd
NUM_=>>>1
VIRTUALNUL=>>>4

*/

int gateway=0;
int enable_wan=0;
int enable_br=0;
char br_interface[16]={0};
char br_lan1_interface[16]={0};
char br_lan2_interface[16]={0};
char wan_interface[16]={0};
char vlan_interface[32]={0};
char wlan_interface[16]={0};
char wlan_valid_interface[200]={0};
char wlan_virtual_interface[80]={0};
char wlan_vxd_interface[16]={0};
int num_wlan_interface=0;
int num_wlan_virtual_interface=0;
int num_wlan_vxd_interface=0;

void set_log(void)
{
	int intValue=0,  intValue1=0;
	char tmpBuffer[32];
	char syslog_para[32];

	apmib_get(MIB_LOG_ENABLED, (void*)&intValue);
	if(intValue !=0 && intValue !=2 && intValue !=4 && intValue !=6 && intValue !=8 &&
		intValue !=10 && intValue !=12 && intValue !=14) {
			apmib_get(MIB_REMOTELOG_ENABLED, (void*)&intValue1);
			if(intValue1 != 0){
				apmib_get(MIB_REMOTELOG_SERVER,  (void *)tmpBuffer);
				if (memcmp(tmpBuffer, "\x0\x0\x0\x0", 4)){
					sprintf(syslog_para, "%s", inet_ntoa(*((struct in_addr *)tmpBuffer)));
					RunSystemCmd(NULL_FILE, "syslogd", "-L", "-R", syslog_para, NULL_STR);
				}
			}else{
		#ifdef RINGLOG /* ring log */
				system("rm /var/log/log_split >/dev/null 2>&1");
				sprintf(tmpBuffer, "echo %d > /var/log/log_split", LOG_SPLIT);
				system(tmpBuffer);
				sprintf(tmpBuffer, "%d", MAX_LOG_SIZE);
				sprintf(syslog_para, "%d", LOG_SPLIT);
				fprintf(stderr, "syslog will use %dKB for log(%d rotate, 1 original, %dKB for each)\n",
					MAX_LOG_SIZE * (LOG_SPLIT+1), LOG_SPLIT, MAX_LOG_SIZE);
				RunSystemCmd(NULL_FILE, "syslogd", "-L", "-s", tmpBuffer, "-b", syslog_para, NULL_STR);
		#else
				RunSystemCmd(NULL_FILE, "syslogd", "-L", NULL_STR);
		#endif

			}
			RunSystemCmd(NULL_FILE, "klogd", NULL_STR);
		}


	return;
}

void start_wlanapp(int action)
{
	char tmpBuff[64];
	memset(tmpBuff, 0x00, 64);
	if(action==1){
		if(wlan_interface[0] && wlan_virtual_interface[0] && wlan_vxd_interface[0] && br_interface[0])
		sprintf(tmpBuff, "%s %s %s %s", wlan_interface, wlan_virtual_interface, wlan_vxd_interface, br_interface);
		else if(wlan_interface[0] && wlan_virtual_interface[0] && !wlan_vxd_interface[0] && br_interface[0])
			sprintf(tmpBuff, "%s %s %s", wlan_interface, wlan_virtual_interface, br_interface);
		else if(wlan_interface[0] && !wlan_virtual_interface[0] && wlan_vxd_interface[0] && br_interface[0])
			sprintf(tmpBuff, "%s %s %s", wlan_interface, wlan_vxd_interface, br_interface);
		else if(wlan_interface[0] && !wlan_virtual_interface[0] && !wlan_vxd_interface[0] && br_interface[0])
			sprintf(tmpBuff, "%s %s", wlan_interface, br_interface);
	}else {
		if(wlan_interface[0] && br_interface[0])
		sprintf(tmpBuff, "%s %s", wlan_interface, br_interface);
	}

	RunSystemCmd("/proc/gpio", "echo", "0", NULL_STR);
	if(tmpBuff[0])
	setWlan_Applications("start", tmpBuff);

}


void start_upnpd(int isgateway, int sys_op)
{
#ifdef   HOME_GATEWAY
	int intValue=0,  intValue1=0;
	if(SetWlan_idx("wlan0")){
		apmib_get(MIB_WSC_DISABLE, (void *)&intValue);
	}
	if(isgateway==1 && sys_op !=1)
		apmib_get(MIB_UPNP_ENABLED, (void *)&intValue1);
	else
		intValue1=0;
	if(intValue==0 && intValue1==0){//wps enabled and upnp igd is disabled
		RunSystemCmd(NULL_FILE, "mini_upnpd", "-wsc", "/tmp/wscd_config", "-daemon", NULL_STR);
		//system("mini_upnpd -wsc /tmp/wscd_config &");
	}else if(intValue==1 && intValue1==1){//wps is disabled, and upnp igd is enabled
		RunSystemCmd(NULL_FILE, "mini_upnpd", "-igd", "/tmp/igd_config", "-daemon", NULL_STR);
		//system("mini_upnpd -igd /tmp/igd_config &");
	}else if(intValue==0 && intValue1==1){//both wps and upnp igd are enabled
		RunSystemCmd(NULL_FILE, "mini_upnpd", "-wsc", "/tmp/wscd_config", "-igd", "/tmp/igd_config","-daemon",  NULL_STR);
		//system("mini_upnpd -wsc /tmp/wscd_config -igd /tmp/igd_config &");
	}else if(intValue==1 && intValue1==0){//both wps and upnp igd are disabled
		/*do nothing*/
	}
#endif
}
/*method to start reload is co-operate to parse rule of reload.c*/
void start_wlan_by_schedule()
{
	int intValue=0,  intValue1=0, i=0, entryNum=0;
	char tmp1[64]={0};
	SCHEDULE_T wlan_sched;

	apmib_get(MIB_WLAN_DISABLED, (void *)&intValue);
	if(intValue==0){
		apmib_get(MIB_SCHEDULE_ENABLED, (void *)&intValue1);
		apmib_get(MIB_SCHEDULE_NUM, (void *)&entryNum);
		if(intValue1==1 && entryNum > 0){
			for (i=1; i<=entryNum; i++) {
				*((char *)&wlan_sched) = (char)i;
				apmib_get(MIB_SCHEDULE, (void *)&wlan_sched);
			}

			sprintf(tmp1, "reload -e %d,%d,%d,%d,%s &", wlan_sched.eco, wlan_sched.fTime, wlan_sched.tTime, wlan_sched.day, wlan_sched.text);
			system(tmp1);
		}else{ /* do not care schedule*/
			system("reload &");
		}
	}else{
		/*wlan is disabled, we do not care wlan schedule*/
		system("reload &");
	}
}
void clean_process(int sys_opmode, int gateway, int enable_wan, char *lanInterface, char *wlanInterface, char *wanInterface)
{
	char strPID[10], tmpBuff[200];
	int pid=-1;
/*clean the process before take new setting*/
#ifdef   HOME_GATEWAY
	if(isFileExist(HW_NAT_FILE)){/*hw nat supported*/
		/*cleanup hardware tables*/
		if(sys_opmode==0)
			RunSystemCmd(HW_NAT_FILE, "echo", "0", NULL_STR);
		else
			RunSystemCmd(HW_NAT_FILE, "echo", "2", NULL_STR);

	}else{/*software nat supported*/
		if(sys_opmode==0)
			RunSystemCmd(SOFTWARE_NAT_FILE, "echo", "0", NULL_STR);
		if(sys_opmode==1)
			RunSystemCmd(SOFTWARE_NAT_FILE, "echo", "1", NULL_STR);
		if(sys_opmode==2)
			RunSystemCmd(SOFTWARE_NAT_FILE, "echo", "2", NULL_STR);
	}
#endif

		RunSystemCmd(NULL_FILE, "killall", "-15", "miniigd", NULL_STR);
		if(isFileExist(IGD_PID_FILE)){
			unlink(IGD_PID_FILE);
		}
		RunSystemCmd(NULL_FILE, "killall", "-15", "routed", NULL_STR);
		if(isFileExist(RIP_PID_FILE)){
			unlink(RIP_PID_FILE);
		}
		if(isFileExist(TR069_PID_FILE)){
			pid=getPid_fromFile(TR069_PID_FILE);
			if(pid != -1){
				sprintf(strPID, "%d", pid);
				RunSystemCmd(NULL_FILE, "kill", "-9", strPID, NULL_STR);
			}
			unlink(TR069_PID_FILE);
		}


		//RunSystemCmd(NULL_FILE, "killall", "-9", "pptp.sh", NULL_STR);
		//RunSystemCmd(NULL_FILE, "killall", "-9", "pppoe.sh", NULL_STR);
		//RunSystemCmd(NULL_FILE, "killall", "-9", "l2tp.sh", NULL_STR);
		RunSystemCmd(NULL_FILE, "rm", "-f", "/etc/ppp/first", NULL_STR);
		RunSystemCmd(NULL_FILE, "rm", "-f", "/etc/ppp/firstpptp", NULL_STR);
		RunSystemCmd(NULL_FILE, "rm", "-f", "/etc/ppp/firstl2tp", NULL_STR);
		RunSystemCmd(NULL_FILE, "rm", "-f", "/etc/ppp/firstdemand", NULL_STR);
		//RunSystemCmd(NULL_FILE, "disconnect.sh", "all", NULL_STR);
#ifdef   HOME_GATEWAY
		wan_disconnect("all");
#endif
	RunSystemCmd(NULL_FILE, "killall", "-9", "ntp_inet", NULL_STR);
	RunSystemCmd(NULL_FILE, "killall", "-9", "ddns_inet", NULL_STR);
	RunSystemCmd(NULL_FILE, "killall", "-9", "syslogd", NULL_STR);
	RunSystemCmd(NULL_FILE, "killall", "-9", "klogd", NULL_STR);
	RunSystemCmd(NULL_FILE, "killall", "-9", "mini_upnpd", NULL_STR);
	RunSystemCmd(NULL_FILE, "killall", "-9", "reload", NULL_STR);
	if(isFileExist(L2TPD_PID_FILE)){
			pid=getPid_fromFile(L2TPD_PID_FILE);
			if(pid != 0){
				sprintf(strPID, "%d", pid);
				RunSystemCmd(NULL_FILE, "kill", "-9", strPID, NULL_STR);
			}
			unlink(L2TPD_PID_FILE);
	}
/*kill dhcp client if br interface is dhcp client*/
	sprintf(tmpBuff, "/etc/udhcpc/udhcpc-%s.pid", lanInterface);
	if(isFileExist(tmpBuff)){
			pid=getPid_fromFile(tmpBuff);
			if(pid != 0){
				sprintf(strPID, "%d", pid);
				RunSystemCmd(NULL_FILE, "kill", "-9", strPID, NULL_STR);
			}
			unlink(tmpBuff);
	}
	sprintf(tmpBuff, "/etc/udhcpc/udhcpc-%s.pid", wanInterface);
	if(isFileExist(tmpBuff)){
			pid=getPid_fromFile(tmpBuff);
			if(pid !=0){
				sprintf(strPID, "%d", pid);
				RunSystemCmd(NULL_FILE, "kill", "-9", strPID, NULL_STR);
			}
			unlink(tmpBuff);
	}
	if(wlanInterface[0]){
	sprintf(tmpBuff, "/etc/udhcpc/udhcpc-%s.pid", wlanInterface);
	if(isFileExist(tmpBuff)){
			pid=getPid_fromFile(tmpBuff);
			if(pid != 0){
				sprintf(strPID, "%d", pid);
				RunSystemCmd(NULL_FILE, "kill", "-9", strPID, NULL_STR);
			}
			unlink(tmpBuff);
	}
	}
	if(isFileExist(DNRD_PID_FILE)){
			pid=getPid_fromFile(DNRD_PID_FILE);
			if(pid != 0){
				sprintf(strPID, "%d", pid);
				RunSystemCmd(NULL_FILE, "kill", "-9", strPID, NULL_STR);
			}
			unlink(DNRD_PID_FILE);
	}
	if(isFileExist(IGMPPROXY_PID_FILE)){
			pid=getPid_fromFile(IGMPPROXY_PID_FILE);
			if(pid != 0){
				sprintf(strPID, "%d", pid);
				RunSystemCmd(NULL_FILE, "kill", "-9", strPID, NULL_STR);
			}
			unlink(IGMPPROXY_PID_FILE);
	}
	if(isFileExist(LLTD_PID_FILE)){
			pid=getPid_fromFile(LLTD_PID_FILE);
			if(pid != 0){
				sprintf(strPID, "%d", pid);
				RunSystemCmd(NULL_FILE, "kill", "-9", strPID, NULL_STR);
			}
			unlink(LLTD_PID_FILE);
	}
	if(isFileExist(DHCPD_PID_FILE)){
			pid=getPid_fromFile(DHCPD_PID_FILE);
			if(pid != 0){
				sprintf(strPID, "%d", pid);
				RunSystemCmd(NULL_FILE, "kill", "-16", strPID, NULL_STR);/*inform dhcp server write lease table to file*/
				sleep(1);
				RunSystemCmd(NULL_FILE, "kill", "-9", strPID, NULL_STR);
			}
			unlink(DHCPD_PID_FILE);
	}

/*end of clean the process before take new setting*/
#if defined(CONFIG_POCKET_ROUTER_SUPPORT)
	/* Reset Firewall Rules*/
	system("iptables -F");
	system("iptables -F -t nat");
	system("iptables -F -t mangle");
	system("iptables -P INPUT ACCEPT");
	sprintf(tmpBuff, "iptables -A INPUT -i %s -j ACCEPT", lanInterface);

	system(tmpBuff);
	if(sys_opmode==1){
		system("iptables -P INPUT ACCEPT");
		system("iptables -P FORWARD ACCEPT");
	}
#endif

}


void set_br_interface(unsigned char *brif)
{

	int opmode=-1;
  apmib_get(MIB_OP_MODE,(void *)&opmode);

	if(br_interface[0]){
		strcat(brif, br_interface);
		strcat(brif, " ");
	}
	if(br_lan1_interface[0]){
		strcat(brif, br_lan1_interface);
		strcat(brif, " ");
	}
	if(vlan_interface[0]){
		strcat(brif, vlan_interface);
		strcat(brif, " ");
	}
	if(wlan_interface[0]){
		strcat(brif, wlan_interface);
		strcat(brif, " ");
	}
	if(wlan_virtual_interface[0]){
		strcat(brif, wlan_virtual_interface);
		strcat(brif, " ");
	}
	if(br_lan2_interface[0]){
		strcat(brif, br_lan2_interface);
		strcat(brif, " ");
	}
	if(wlan_vxd_interface[0]){
		strcat(brif, wlan_vxd_interface);
		strcat(brif, " ");
	}

	return;
}

int setinit(int argc, char** argv)
{
	int i, cmdRet=-1;
	int opmode=-1, v_wlan_app_enabled=1, intValue=0, intValue1=0;
	char cmdBuffer[100], tmpBuff[200];
	int repeater_enabled1=0, repeater_enabled2=0;
	char *token=NULL, *savestr1=NULL;
	char tmp_args[16];
	int wisp_wan_id=0;
	int lan_dhcp_mode=0;
	int wan_dhcp_mode=0;
	char Ip[32], Mask[32], Gateway[32];
	int wlan_mode_root=0, wlan_root_disabled=0;
#if defined(CONFIG_RTK_IPV6_PASSTHRU_SUPPORT)
	unsigned char ipv6_fast_bridge_buf[100];
	int vpnPassthruIPv6 = 0;
#endif
#ifdef CONFIG_RTL_WAPI_SUPPORT
#ifdef CONFIG_RTL_WAPI_LOCAL_AS_SUPPORT
	if(isFileExist(CA_CERT_FILE)==0){
		RunSystemCmd(NULL_FILE, "loadWapiFiles", NULL_STR);
	}
#else
	if(isFileExist(CA4AP_CERT_FILE)==0 && isFileExist(AP_CERT_FILE)==0 ){
		RunSystemCmd(NULL_FILE, "loadWapiFiles", NULL_STR);
	}
#endif

#endif
	#if defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT)
		system("echo 1 > /var/system/start_init");
	#endif
	printf("Init Start...\n");
	apmib_get(MIB_OP_MODE,(void *)&opmode);
	apmib_get(MIB_WISP_WAN_ID,(void *)&wisp_wan_id);
	apmib_get(MIB_DHCP,(void *)&lan_dhcp_mode);
#if defined(CONFIG_RTK_IPV6_PASSTHRU_SUPPORT)
	apmib_get(MIB_VPN_PASSTHRU_IPV6,(void *)&vpnPassthruIPv6);
#endif
#ifdef   HOME_GATEWAY
	apmib_get(MIB_WAN_DHCP,(void *)&wan_dhcp_mode);
#endif
	memset(br_lan2_interface, 0x00, 16);
	memset(vlan_interface, 0x00, 32);
	if(opmode==0)
		RunSystemCmd("/var/sys_op", "echo", "0", NULL_STR);
	if(opmode==1)
		RunSystemCmd("/var/sys_op", "echo", "1", NULL_STR);
	if(opmode==2)
		RunSystemCmd("/var/sys_op", "echo", "2", NULL_STR);

/* set interface name  start*/
	sprintf(tmp_args,"%s", argv[2]);
	if(!strcmp(tmp_args, "ap")){
		sprintf(br_interface, "%s", "br0");
		sprintf(br_lan1_interface, "%s" , "eth0");
#ifdef   HOME_GATEWAY
		if(opmode==2)
			sprintf(br_lan2_interface, "%s", "eth1");
#else
		sprintf(br_lan2_interface, "%s", "eth1");
#endif
		gateway=0;
	}
	if(!strcmp(tmp_args, "gw")){
		gateway=1;
		if(opmode==2)
			sprintf(wan_interface, "wlan%d", wisp_wan_id);
		else
			sprintf(wan_interface, "%s", "eth1");

		sprintf(br_interface, "%s", "br0");
		sprintf(br_lan1_interface, "%s" , "eth0");
		if(opmode == 2 ) {
			sprintf(br_lan2_interface, "%s", "eth1");
		}

#if defined(CONFIG_RTK_IPV6_PASSTHRU_SUPPORT)
		if(vpnPassthruIPv6 == 1)
		{
			char wanporttmp[20];

			memset(wanporttmp, 0x00, sizeof(wanporttmp));
			sprintf(wanporttmp,"4%d%s",opmode,wan_interface);
			RunSystemCmd("/proc/wan_port", "echo", wanporttmp, NULL_STR);
		}
		else
#endif
		{
			RunSystemCmd("/proc/wan_port", "echo", "4", NULL_STR);
		}
	}

	if(isFileExist(ETH_VLAN_SWITCH)){//vlan init file is exist
#if defined(VLAN_CONFIG_SUPPORTED) && defined(CONFIG_RTK_VLAN_ROUTETYPE)
 		sprintf(vlan_interface, "%s %s %s %s", "eth2", "eth3", "eth4", "eth5");
#else
		sprintf(vlan_interface, "%s %s %s", "eth2", "eth3", "eth4");
#endif
	}
	memset(wlan_interface, 0x00, 16);
	memset(wlan_virtual_interface, 0x00, 80);
	memset(wlan_vxd_interface, 0x00, 16);
	memset(wlan_valid_interface, 0x00, 200);

	for(i=0;i<NUM_WLAN_INTERFACE;i++){
		if(wlan_interface[0]==0x00)
			sprintf(wlan_interface, "wlan%d", i);
		else{
			sprintf(tmp_args, " wlan%d", i);
			strcat(wlan_interface, tmp_args);
		}
	}
	num_wlan_interface=NUM_WLAN_INTERFACE;
	num_wlan_virtual_interface=if_readlist_proc(wlan_virtual_interface, "va", 0);
	num_wlan_vxd_interface=if_readlist_proc(wlan_vxd_interface, "vxd", 0);
	//printf("got wlan_virtual_interface=%s, num=%d\n", wlan_virtual_interface, num_wlan_virtual_interface);
	//printf("got wlan_vxd_interface=%s, num=%d\n", wlan_vxd_interface, num_wlan_vxd_interface);
	intValue = if_readlist_proc(wlan_valid_interface, "wlan", 0);
	if(intValue==0)
		memset(wlan_interface, 0x00, 16);
/* set interface name  end*/

/*currently, we just support init gw/ap all */
	sprintf(tmp_args,"%s", argv[3]);

	if(!strcmp(tmp_args, "all")){
		enable_wan=1;
		enable_br=1;
	}else if(!strcmp(tmp_args, "wan")){
		enable_wan=1;
		enable_br=1;
	}else if(!strcmp(tmp_args, "bridge")){
		enable_wan=1;
		enable_br=1;
	}else if(!strcmp(tmp_args, "wlan_app")){
		start_wlanapp(0);
		return 0;
	}
/**/
#if defined(CONFIG_HW_PROTOCOL_VLAN_TBL)
	RunSystemCmd("/proc/custom_Passthru", "echo", "0", NULL_STR); // disable ipv6 passthru
#endif
	clean_process(opmode, gateway, enable_wan, br_interface, wlan_interface, wan_interface);
	/*save the last wan type*/
	sprintf(tmp_args, "%d", wan_dhcp_mode);
	RunSystemCmd("/var/system/last_wan", "echo", tmp_args, NULL_STR);
/* start to take new setting from MIB*/
	RunSystemCmd(NULL_FILE, "ifconfig", "eth0", "down", NULL_STR);
	RunSystemCmd(NULL_FILE, "ifconfig", "eth1", "down", NULL_STR);
	apmib_get(MIB_REPEATER_ENABLED1,(void *)&repeater_enabled1);
	apmib_get(MIB_REPEATER_ENABLED2,(void *)&repeater_enabled2);
	if(SetWlan_idx("wlan0")){
		apmib_get( MIB_WLAN_MODE, (void *)&wlan_mode_root);
		apmib_get( MIB_WLAN_DISABLED, (void *)&wlan_root_disabled);
//printf(" wlan_root_disabled=%d\n", wlan_root_disabled);
	}
	if(wlan_mode_root != 0 && wlan_mode_root != 3 && repeater_enabled1 == 0 && repeater_enabled2 == 0){
		v_wlan_app_enabled=0;
	}
	if((repeater_enabled1 == 0 && repeater_enabled2 == 0) ||(wlan_mode_root >= 4 && wlan_mode_root <=7) ||(wlan_mode_root == 2))
		memset(wlan_vxd_interface, 0x00, 16);


	apmib_get(MIB_ELAN_MAC_ADDR,  (void *)tmpBuff);
	if(!memcmp(tmpBuff, "\x00\x00\x00\x00\x00\x00", 6))
		apmib_get(MIB_HW_NIC0_ADDR,  (void *)tmpBuff);
	sprintf(cmdBuffer, "%02x%02x%02x%02x%02x%02x", (unsigned char)tmpBuff[0], (unsigned char)tmpBuff[1],
		(unsigned char)tmpBuff[2], (unsigned char)tmpBuff[3], (unsigned char)tmpBuff[4], (unsigned char)tmpBuff[5]);
	RunSystemCmd(NULL_FILE, "ifconfig", br_lan1_interface, "hw", "ether", cmdBuffer, NULL_STR);/*set eth0 mac address*/

	#if defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT)
		Init_Domain_Query_settings(opmode, wlan_mode_root, lan_dhcp_mode, cmdBuffer);
	#endif
	if(opmode == 1 || opmode == 2){
		apmib_get(MIB_ELAN_MAC_ADDR,  (void *)tmpBuff);
		if(!memcmp(tmpBuff, "\x00\x00\x00\x00\x00\x00", 6))
			apmib_get(MIB_HW_NIC1_ADDR,  (void *)tmpBuff);
		sprintf(cmdBuffer, "%02x%02x%02x%02x%02x%02x", (unsigned char)tmpBuff[0], (unsigned char)tmpBuff[1],
			(unsigned char)tmpBuff[2], (unsigned char)tmpBuff[3], (unsigned char)tmpBuff[4], (unsigned char)tmpBuff[5]);
		RunSystemCmd(NULL_FILE, "ifconfig", br_lan2_interface, "hw", "ether", cmdBuffer, NULL_STR);/*set eth1 mac address when bridge mode*/
	}
#ifdef   HOME_GATEWAY
	if(gateway ==1 && opmode != 1){
		apmib_get(MIB_WAN_MAC_ADDR,  (void *)tmpBuff);
		if(!memcmp(tmpBuff, "\x00\x00\x00\x00\x00\x00", 6)){
			if(opmode == 2)
				apmib_get(MIB_HW_WLAN_ADDR,  (void *)tmpBuff);
			else
				apmib_get(MIB_HW_NIC1_ADDR,  (void *)tmpBuff);
		}
		sprintf(cmdBuffer, "%02x%02x%02x%02x%02x%02x", (unsigned char)tmpBuff[0], (unsigned char)tmpBuff[1],
			(unsigned char)tmpBuff[2], (unsigned char)tmpBuff[3], (unsigned char)tmpBuff[4], (unsigned char)tmpBuff[5]);
		RunSystemCmd(NULL_FILE, "ifconfig", wan_interface, "hw", "ether", cmdBuffer, NULL_STR);/*set wan mac address when bridge mode*/
	}
#endif
	/*init wlan interface*/

		if(wlan_root_disabled==1){
			memset(wlan_interface, 0x00, 16);
		}

		if(wlan_interface[0]){
		cmdRet=RunSystemCmd(NULL_FILE, "flash", "set_mib", wlan_interface, NULL_STR);
		if(cmdRet != 0){
			//printf("WLAN MIB version mismatch!\n");
			return 0;
		}
		if(wlan_vxd_interface[0]){
			RunSystemCmd(NULL_FILE, "flash", "set_mib", wlan_vxd_interface, NULL_STR);/*set vxd wlan iface*/
		}
		if(wlan_virtual_interface[0]){
			token=NULL;
			savestr1=NULL;
			sprintf(tmpBuff, "%s", wlan_virtual_interface);
			token = strtok_r(tmpBuff," ", &savestr1);
			do{
				if (token == NULL){
					break;
				}else{
					RunSystemCmd(NULL_FILE, "flash", "set_mib", token, NULL_STR);/*set virtual wlan iface*/
				}
				token = strtok_r(NULL, " ", &savestr1);
			}while(token !=NULL);
		}
	}
	if(gateway==1){
		if(enable_br==1){
			/*init bridge interface*/
			memset(tmpBuff,0x00,sizeof(tmpBuff));
			set_br_interface(tmpBuff);

#if defined(CONFIG_RTK_IPV6_PASSTHRU_SUPPORT)
			if(vpnPassthruIPv6 == 1)
			{
				memset(ipv6_fast_bridge_buf, 0x00, sizeof(ipv6_fast_bridge_buf));

#if defined(VLAN_CONFIG_SUPPORTED) && !defined(CONFIG_RTK_VLAN_ROUTETYPE)
				sprintf(ipv6_fast_bridge_buf, " %s", "eth5");
#elif defined(VLAN_CONFIG_SUPPORTED) && defined(CONFIG_RTK_VLAN_ROUTETYPE)
				sprintf(ipv6_fast_bridge_buf, " %s", "eth6");
#else
				sprintf(ipv6_fast_bridge_buf, " %s", "eth2");
#endif
				strcat(tmpBuff, ipv6_fast_bridge_buf);
			}
#endif

			setbridge(tmpBuff);
			/* init log setting*/
			set_log();
			/* init lan dhcp setting*/
			if(lan_dhcp_mode==0){		/*DHCP disabled*/
				apmib_get( MIB_IP_ADDR,  (void *)tmpBuff);
				sprintf(Ip, "%s", inet_ntoa(*((struct in_addr *)tmpBuff)));
				apmib_get( MIB_SUBNET_MASK,  (void *)tmpBuff);
				sprintf(Mask, "%s", inet_ntoa(*((struct in_addr *)tmpBuff)));
				apmib_get(MIB_DEFAULT_GATEWAY,  (void *)tmpBuff);

				if (!memcmp(tmpBuff, "\x0\x0\x0\x0", 4))
					memset(Gateway, 0x00, 32);
				else
					sprintf(Gateway, "%s", inet_ntoa(*((struct in_addr *)tmpBuff)));

				RunSystemCmd(NULL_FILE, "ifconfig", br_interface, Ip, "netmask", Mask, NULL_STR);

				if(Gateway[0]){
					RunSystemCmd(NULL_FILE, "route", "del", "default", br_interface, NULL_STR);
					RunSystemCmd(NULL_FILE, "route", "add", "-nat", "default", "gw", Gateway, "dev", br_interface, NULL_STR);
				}
				start_wlanapp(v_wlan_app_enabled);
			}else
#ifdef CONFIG_DOMAIN_NAME_QUERY_SUPPORT
	if(lan_dhcp_mode==2 || lan_dhcp_mode==15)//dhcp disabled or server mode or auto
#else
	if(lan_dhcp_mode==2)
#endif

			{		/*DHCP server enabled*/
				intValue1=0;
				for(i=0;i<NUM_WLAN_INTERFACE;i++){
					sprintf(tmp_args, " wlan%d", i);
					if(SetWlan_idx(tmp_args)){
						apmib_get(MIB_WLAN_WDS_ENABLED, (void *)&intValue);
						//printf("wds enabled=%d\n",intValue);
						if(intValue!=0)
							intValue1=intValue1+5;
						else
							intValue1=intValue1+1;
					}
				}
				sleep(intValue1);/*wait wlan wds init */
				/*start dhcp server*/
				//RunSystemCmd(NULL_FILE, "dhcpd.sh", br_interface, "gw", NULL_STR);
		#ifdef CONFIG_DOMAIN_NAME_QUERY_SUPPORT
				if((lan_dhcp_mode==2) || (opmode != 1 && lan_dhcp_mode==15) )
					set_lan_dhcpd(br_interface, 2);
		#else
				set_lan_dhcpd(br_interface, 2);
		#endif

				start_wlanapp(v_wlan_app_enabled);
			}
		}/*for init bridge interface and wlan app*/

#ifdef CONFIG_DOMAIN_NAME_QUERY_SUPPORT
		/* start dnrd for check dns query with hostname */
		domain_query_start_dnrd(wlan_mode_root, 1);
#endif

		RunSystemCmd(NULL_FILE, "rm", "-f", "/var/eth1_ip", NULL_STR);
		RunSystemCmd(NULL_FILE, "rm", "-f", "/var/ntp_run", NULL_STR);
		RunSystemCmd("/proc/pptp_src_ip", "echo", "0 0", NULL_STR);
		RunSystemCmd(NULL_FILE, "ifconfig", wan_interface, "down", NULL_STR);
		RunSystemCmd(NULL_FILE, "ifconfig", wan_interface, "up", NULL_STR);
		if(enable_wan==1 && (opmode == 0 || opmode==2)){/*for init internet wan setting*/
			if(opmode==2){
				if(SetWlan_idx("wlan0")){
					apmib_get(MIB_WLAN_ENCRYPT, (void *)&intValue);
					if(intValue != 0){
						for(i=0;i<NUM_WLAN_INTERFACE;i++){
							sprintf(tmp_args, " wlan%d", i);
							RunSystemCmd(NULL_FILE, "iwpriv", tmp_args, "set_mib", "keep_rsnie=1", NULL_STR);
						}
					}
				}
			}
			RunSystemCmd(PROC_FASTNAT_FILE, "echo", "1", NULL_STR);

			if(wan_dhcp_mode==PPTP){
				RunSystemCmd(PROC_FASTPPTP_FILE, "echo", "1", NULL_STR);
				apmib_get(MIB_PPTP_CONNECTION_TYPE, (void *)&intValue);
				if(intValue==1){
					RunSystemCmd(PROC_PPTP_CONN_FILE, "echo", "3", NULL_STR);
				}else{
					RunSystemCmd(PROC_PPTP_CONN_FILE, "echo", "0", NULL_STR);
				}
			}else{
				RunSystemCmd(PROC_FASTPPTP_FILE, "echo", "0", NULL_STR);
			}

			if(wan_dhcp_mode==L2TP){
				RunSystemCmd(PROC_FASTL2TP_FILE, "echo", "1", NULL_STR);
			}else{
				RunSystemCmd(PROC_FASTL2TP_FILE, "echo", "0", NULL_STR);
			}
	#ifdef HOME_GATEWAY
			if(wan_dhcp_mode !=DHCP_SERVER && wan_dhcp_mode < 7){ /* */
				start_wan(wan_dhcp_mode, opmode, wan_interface, br_interface, wisp_wan_id, 1);
			}else
				printf("Invalid wan type:wan_dhcp_mode=%d\n", wan_dhcp_mode);
	#endif
		}
	}
	else
	{ /*gateway is 0, it is ap mode*/
		memset(tmpBuff,0x00,sizeof(tmpBuff));
		set_br_interface(tmpBuff);

		setbridge(tmpBuff);
		/* init log setting*/
			set_log();
		if(lan_dhcp_mode==2){
			sleep(1);
			//RunSystemCmd(NULL_FILE, "dhcpd.sh", br_interface, "ap", NULL_STR);
			set_lan_dhcpd(br_interface, 1);
		}
		if(lan_dhcp_mode==2 || lan_dhcp_mode==0
#ifdef CONFIG_DOMAIN_NAME_QUERY_SUPPORT
			|| lan_dhcp_mode==15	//dhcp disabled or server mode or auto
#endif
			){
			start_wlanapp(v_wlan_app_enabled);
		}
	}

#if 0 //defined(CONFIG_RTK_IPV6_PASSTHRU_SUPPORT)
	memset(ipv6_fast_bridge_buf, 0x00, sizeof(ipv6_fast_bridge_buf));
	if(vpnPassthruIPv6 == 1 || opmode == 1) // 1:bridge
		setIPv6WanPort(wan_interface, 1);
	else
		setIPv6WanPort(wan_interface, 0);

	system(ipv6_fast_bridge_buf);

#endif

	start_upnpd(gateway, opmode);
	if(gateway==1 && opmode != 1){
		if(isFileExist(LLTD_PROCESS_FILE)){
			RunSystemCmd(NULL_FILE, "lld2d", br_interface, NULL_STR);
		}
	}
	if(isFileExist(SNMPD_PROCESS_FILE)){
		RunSystemCmd(NULL_FILE, "snmpd.sh", "restart", NULL_STR);
		RunSystemCmd(NULL_FILE, "snmpd", "-c", SNMPD_CONF_FILE, "-p", SNMPD_PID_FILE,  NULL_STR);
	}
	if(isFileExist(NMSD_PROCESS_FILE)){
		RunSystemCmd(NULL_FILE, "nmsd", NULL_STR);
	}
	start_wlan_by_schedule();

	#if defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT)
		system("rm -f  /var/system/start_init 2> /dev/null");
	#endif

	return 0;
}

void Init_Internet(int argc, char** argv)
{
#ifdef   HOME_GATEWAY
	int wisp_id=0, wan_mode=0, opmode=0;
	char br_interface[16]={0};
	char wan_interface[16]={0};
	char tmp_args[16]={0};


	if(argc < 4)
		return;

	apmib_get(MIB_OP_MODE,(void *)&opmode);
	if(opmode == 1)
		return;

	apmib_get(MIB_WISP_WAN_ID,(void *)&wisp_id);

	sprintf(tmp_args, "%s",argv[1]);
	if(!strcmp(tmp_args, "pppoe"))
		wan_mode=3;
	else if(!strcmp(tmp_args, "pptp"))
		wan_mode=4;
	else if(!strcmp(tmp_args, "l2tp"))
		wan_mode=6;
	else{
		printf("Un-support wan type for init\n");
		return;
	}
	sprintf(br_interface, "%s", "br0");
	sprintf(wan_interface, "%s",argv[3]);
	start_wan(wan_mode, opmode, wan_interface, br_interface, wisp_id, 0);
#endif
}

void Init_QoS(int argc, char** argv)
{
#ifdef   HOME_GATEWAY
	int wisp_id=0, wan_mode=0, opmode=0;


	apmib_get(MIB_OP_MODE,(void *)&opmode);
	if(opmode == 1)
		return;

	apmib_get(MIB_WISP_WAN_ID,(void *)&wisp_id);
	apmib_get(MIB_WAN_DHCP,(void *)&wan_mode);
	set_QoS(opmode, wan_mode, wisp_id);
#endif

}







