/*
 *
 *
 */

/* System include files */
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <netdb.h>
#include <stdarg.h>
#include <errno.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/wait.h>
/* Local include files */
#include "../apmib.h"
#include "../mibtbl.h"
#include "sysconf.h"
#include "sys_utility.h"
//
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <net/if.h>
#include <stddef.h>		/* offsetof */
#include <net/if_arp.h>
#include <linux/if_ether.h>
int apmib_initialized = 0;
extern int setinit(int argc, char** argv);
extern int Init_Internet(int argc, char** argv);
extern int setbridge(char *argv);
extern int setFirewallIptablesRules(int argc, char** argv);
extern int setWlan_Applications(char *action, char *argv);
extern void wan_disconnect(char *option);
extern void wan_connect(char *interface, char *option);

extern int Init_QoS(int argc, char** argv);
extern void start_lan_dhcpd(char *interface);

#ifdef CONFIG_DOMAIN_NAME_QUERY_SUPPORT
extern void wan_connect_pocket(char *interface, char *option);
extern int Check_setting_default(int opmode, int wlan_mode);
extern int Check_setting(int type);
extern void start_upnpd(int isgateway, int sys_op);
#endif
//////////////////////////////////////////////////////////////////////

#ifdef CONFIG_POCKET_ROUTER_SUPPORT
#define POCKETAP_HW_SET_FLAG "/proc/pocketAP_hw_set_flag"
#define AP_CLIENT_ROU_FILE "/proc/ap_client_rou"
#define DC_PWR_FILE "/proc/dc_pwr"

static void set_wlan_low_power()
{
//fprintf(stderr,"\r\n __[%s_%u]\r\n",__FILE__,__LINE__);
	system("iwpriv wlan0 set_mib txPowerPlus_cck_1=0");
	system("iwpriv wlan0 set_mib txPowerPlus_cck_2=0");
	system("iwpriv wlan0 set_mib txPowerPlus_cck_5=0");
	system("iwpriv wlan0 set_mib txPowerPlus_cck_11=0");
	system("iwpriv wlan0 set_mib txPowerPlus_ofdm_6=0");
	system("iwpriv wlan0 set_mib txPowerPlus_ofdm_9=0");
	system("iwpriv wlan0 set_mib txPowerPlus_ofdm_12=0");
	system("iwpriv wlan0 set_mib txPowerPlus_ofdm_18=0");
	system("iwpriv wlan0 set_mib txPowerPlus_ofdm_24=0");
	system("iwpriv wlan0 set_mib txPowerPlus_ofdm_36=0");
	system("iwpriv wlan0 set_mib txPowerPlus_ofdm_48=0");
	system("iwpriv wlan0 set_mib txPowerPlus_ofdm_54=0");
	system("iwpriv wlan0 set_mib txPowerPlus_mcs_0=0");
	system("iwpriv wlan0 set_mib txPowerPlus_mcs_1=0");
	system("iwpriv wlan0 set_mib txPowerPlus_mcs_2=0");
	system("iwpriv wlan0 set_mib txPowerPlus_mcs_3=0");
	system("iwpriv wlan0 set_mib txPowerPlus_mcs_4=0");
	system("iwpriv wlan0 set_mib txPowerPlus_mcs_5=0");
	system("iwpriv wlan0 set_mib txPowerPlus_mcs_6=0");
	system("iwpriv wlan0 set_mib txPowerPlus_mcs_7=0");
	system("iwpriv wlan0 set_mib txPowerPlus_mcs_8=0");
	system("iwpriv wlan0 set_mib txPowerPlus_mcs_9=0");
	system("iwpriv wlan0 set_mib txPowerPlus_mcs_10=0");
	system("iwpriv wlan0 set_mib txPowerPlus_mcs_11=0");
	system("iwpriv wlan0 set_mib txPowerPlus_mcs_12=0");
	system("iwpriv wlan0 set_mib txPowerPlus_mcs_13=0");
	system("iwpriv wlan0 set_mib txPowerPlus_mcs_14=0");
	system("iwpriv wlan0 set_mib txPowerPlus_mcs_15=0");
//fprintf(stderr,"\r\n __[%s-%u]\r\n",__FILE__,__LINE__);
}

static void reinit_webs()
{
	FILE *fp=NULL;
	char *webPid = "/var/run/webs.pid";
	pid_t pid=0;
	char line[20];
	unsigned char cmdBuffer[100];

	if ((fp = fopen(webPid, "r")) != NULL)
	{
		fgets(line, sizeof(line), fp);
		fclose(fp);
		if ( sscanf(line, "%d", &pid) )
		{
			if (pid > 1)
			{
				printf("reinit webs \n");
				sprintf(cmdBuffer,"kill %u",pid);
				system(cmdBuffer);
				kill(pid, SIGTERM);

				sprintf(cmdBuffer,"webs&");
				system(cmdBuffer);
			}
		}
	}

}

/* Fix whan device is change wlan mode from client to AP or Router. *
  * The CIPHER_SUITE of wpa or wpa2 can't be tkip                           */
static int check_wpa_cipher_suite()
{
	int wlan_band, wlan_onoff_tkip, wlan_encrypt, wpaCipher, wpa2Cipher, wdsEncrypt;

	apmib_get( MIB_WLAN_BAND, (void *)&wlan_band) ;
	apmib_get( MIB_WLAN_11N_ONOFF_TKIP, (void *)&wlan_onoff_tkip) ;
	apmib_get( MIB_WLAN_ENCRYPT, (void *)&wlan_encrypt);
	apmib_get( MIB_WLAN_WDS_ENCRYPT, (void *)&wdsEncrypt);
	if(wlan_onoff_tkip == 0) //Wifi request
	{
		if(wlan_band == 8 || wlan_band == 10 || wlan_band == 11)//8:n; 10:gn; 11:bgn
		{
			if(wlan_encrypt ==ENCRYPT_WPA || wlan_encrypt ==ENCRYPT_WPA2){
				wpaCipher =  WPA_CIPHER_AES;
				apmib_set(MIB_WLAN_WPA_CIPHER_SUITE, (void *)&wpaCipher);

				wpa2Cipher =  WPA_CIPHER_AES;
				apmib_set(MIB_WLAN_WPA2_CIPHER_SUITE, (void *)&wpa2Cipher);
			}
			if(wdsEncrypt == WDS_ENCRYPT_TKIP)
			{
				wdsEncrypt = WDS_ENCRYPT_DISABLED;
				apmib_set( MIB_WLAN_WDS_ENCRYPT, (void *)&wdsEncrypt);
			}

		}

	}


}
static int pocketAP_bootup()
{
	char	pocketAP_hw_set_flag = 0;

	if(isFileExist(DC_PWR_FILE))
	{
		FILE *fp=NULL;
		unsigned char dcPwr_str[100];
		memset(dcPwr_str,0x00,sizeof(dcPwr_str));

		fp=fopen(DC_PWR_FILE, "r");
		if(fp!=NULL)
		{
			fgets(dcPwr_str,sizeof(dcPwr_str),fp);
			fclose(fp);

			if(strlen(dcPwr_str) != 0)
			{
				dcPwr_str[1]='\0';
				if(strcmp(dcPwr_str,"2") == 0)
				{
					set_wlan_low_power();
				}
			}
		}
	}


	if(isFileExist(POCKETAP_HW_SET_FLAG))
	{
		FILE *fp=NULL;
		unsigned char pocketAP_hw_set_flag_str[10];
		memset(pocketAP_hw_set_flag_str,0x00,sizeof(pocketAP_hw_set_flag_str));

		fp=fopen(POCKETAP_HW_SET_FLAG, "r");
		if(fp!=NULL)
		{
			fgets(pocketAP_hw_set_flag_str,sizeof(pocketAP_hw_set_flag_str),fp);
			fclose(fp);

			if(strlen(pocketAP_hw_set_flag_str) != 0)
			{
				pocketAP_hw_set_flag_str[1]='\0';
				if(strcmp(pocketAP_hw_set_flag_str,"1") == 0)
				{
					pocketAP_hw_set_flag = 1;
				}
				else
				{
					pocketAP_hw_set_flag = 0;
					system("echo 1 > proc/pocketAP_hw_set_flag");
				}
			}
		}

	}

	if(pocketAP_hw_set_flag == 0 && isFileExist(AP_CLIENT_ROU_FILE))
	{
		FILE *fp=NULL;
		unsigned char ap_cli_rou_str[10];
		unsigned char kill_webs_flag = 0;
		memset(ap_cli_rou_str,0x00,sizeof(ap_cli_rou_str));

		fp=fopen(AP_CLIENT_ROU_FILE, "r");
		if(fp!=NULL)
		{
			fgets(ap_cli_rou_str,sizeof(ap_cli_rou_str),fp);
			fclose(fp);

			if(strlen(ap_cli_rou_str) != 0)
			{
				int cur_op_mode;
				int wlan0_mode;
				int lan_dhcp;
				ap_cli_rou_str[1]='\0';
				int ret = 0;

				apmib_get( MIB_OP_MODE, (void *)&cur_op_mode);
				apmib_get( MIB_WLAN_MODE, (void *)&wlan0_mode);

				if((cur_op_mode != 1 || wlan0_mode == CLIENT_MODE) && strcmp(ap_cli_rou_str,"2") == 0) //AP
				{
					cur_op_mode = 1;
					wlan0_mode = 0;
					lan_dhcp = 2;

					apmib_set( MIB_OP_MODE, (void *)&cur_op_mode);
					apmib_set( MIB_WLAN_MODE, (void *)&wlan0_mode);
					//apmib_set( MIB_DHCP, (void *)&lan_dhcp);

					check_wpa_cipher_suite();
				#if defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT)
					Check_setting(2);//ap
				#endif
					if(apmib_update(CURRENT_SETTING) == 1)
						save_cs_to_file();

					reinit_webs();
					//RunSystemCmd(NULL_FILE, "webs&", NULL_STR);
				}
				else if((cur_op_mode != 1 || wlan0_mode != CLIENT_MODE) && strcmp(ap_cli_rou_str,"1") == 0) //CLIENT
				{
					cur_op_mode = 1;
					wlan0_mode = 1;
					lan_dhcp = 0;

					apmib_set( MIB_OP_MODE, (void *)&cur_op_mode);
					apmib_set( MIB_WLAN_MODE, (void *)&wlan0_mode);
					//apmib_set( MIB_DHCP, (void *)&lan_dhcp);
				#if defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT)
					Check_setting(1);//client
				#endif
					if(apmib_update(CURRENT_SETTING) == 1)
						save_cs_to_file();
					reinit_webs();

				}
				else if(cur_op_mode != 0 && strcmp(ap_cli_rou_str,"3") == 0) //router
				{
					cur_op_mode = 0;
					wlan0_mode = 0;
					lan_dhcp = 2;

					apmib_set( MIB_OP_MODE, (void *)&cur_op_mode);
					apmib_set( MIB_WLAN_MODE, (void *)&wlan0_mode);
					//apmib_set( MIB_DHCP, (void *)&lan_dhcp);

					check_wpa_cipher_suite();
				#if defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT)
					Check_setting(3);//router
				#endif
					if(apmib_update(CURRENT_SETTING) == 1)
						save_cs_to_file();
					reinit_webs();
					//RunSystemCmd(NULL_FILE, "webs&", NULL_STR);
				}
				else
				{
					#if defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT)
					ret=Check_setting_default(cur_op_mode, wlan0_mode);
					if(ret==1){
						if(apmib_update(CURRENT_SETTING) == 1)
							save_cs_to_file();
						reinit_webs();
					}
					#endif
				}
			}
		}
	}



}
#endif

int main(int argc, char** argv)
{
	char	line[300];
	char action[16];
	int i;
	//printf("start.......:%s\n",argv[1]);



	if ( !apmib_init()) {
		printf("Initialize AP MIB failed !\n");
		return -1;
	}
	apmib_initialized = 1;
	memset(line,0x00,300);

#ifdef CONFIG_POCKET_ROUTER_SUPPORT
	if(strcmp(argv[1], "init")==0)
	pocketAP_bootup();
#endif

	if(argv[1] && (strcmp(argv[1], "init")==0)){
		setinit(argc,argv);
		return 0;
	} else if(argv[1] && (strcmp(argv[1], "br")==0)){
		for(i=0;i<argc;i++){
			if( i>2 )
				string_casecade(line, argv[i]);
		}
		setbridge(line);
	}
	else if(argv[1] && (strcmp(argv[1], "conn")==0)){
		if(argc < 4){
			printf("sysconf conn Invalid agrments!\n");
			return 0;
		}
		sprintf(action, "%s",argv[3]);
		for(i=0;i<argc;i++){
			if( i>2 )
				string_casecade(line, argv[i]);
		}
#if defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT)
		if(!strcmp(action, "br0"))
			wan_connect_pocket(action, line);
		else
#if	defined(HOME_GATEWAY)
			wan_connect(action, line);
#else
;
#endif

#else //#if defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT)
#if	defined(HOME_GATEWAY)
			wan_connect(action, line);
#endif
#endif
	}
	else if(argv[1] && (strcmp(argv[1], "wlanapp")==0)){
		for(i=0;i<argc;i++){
			if( i>2 )
				string_casecade(line, argv[i]);
			if(i==2)
				sprintf(action, "%s",argv[i]);
		}
		setWlan_Applications(action, line);
	}
#ifdef   HOME_GATEWAY
	else if(argv[1] && (strcmp(argv[1], "firewall")==0)){
		setFirewallIptablesRules(argc,argv);
	}
	else if(argv[1] && (strcmp(argv[1], "disc")==0)){
		sprintf(line, "%s", argv[2]);
		wan_disconnect(line);
	}else if(argv[1] && (strcmp(argv[1], "pppoe")==0)){
		Init_Internet(argc,argv);
	}else if(argv[1] && (strcmp(argv[1], "pptp")==0)){
		Init_Internet(argc,argv);
	}else if(argv[1] && (strcmp(argv[1], "l2tp")==0)){
		Init_Internet(argc,argv);
	}else if(argv[1] && (strcmp(argv[1], "setQos")==0)){
		Init_QoS(argc,argv);
	}else if(argv[1] && (strcmp(argv[1], "dhcpd")==0)){
		sprintf(action, "%s",argv[2]);
		start_lan_dhcpd(action);
	}
#if defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT)
	else if(argv[1] && (strcmp(argv[1], "upnpd")==0)){
		if(argc < 4){
			printf("sysconf upnpd Invalid agrments!\n");
			return 0;
		}
		start_upnpd(atoi(argv[2]),atoi(argv[3]));
	}
#endif
#if defined(RINGLOG)
	else if(argv[1] && (strcmp(argv[1], "log")==0)){
		if (argc == 6 && !strcmp(argv[2], "-s") && !strcmp(argv[4], "-b"))
		{
			system("killall syslogd >/dev/null 2>&1");
			system("rm /var/log/log_split >/dev/null 2>&1");
			sprintf(line, "echo %s > /var/log/log_split", argv[5]);
			system(line);
			RunSystemCmd(NULL_FILE, "syslogd", "-L", "-s", argv[3], "-b", argv[5], NULL_STR);
			fprintf(stderr, "syslog will use %dKB for log(%s rotate, 1 original, %sKB for each).\n",
				atoi(argv[3]) * ((atoi(argv[5]))+1), argv[5], argv[3]);
		}
	/*
		else if (argc == 4 && !strcmp(argv[2], "-R"))
		{
			RunSystemCmd(NULL_FILE, "killall syslogd", NULL_STR);
			RunSystemCmd(NULL_FILE, "syslogd", "-L", "-R", argv[3], NULL_STR);
		}
	*/
		else
		{
			fprintf(stderr, "usage:\n");
			fprintf(stderr, "sysconf log -s size -b number-of-rotate-backup\n");
			//fprintf(stderr, "sysconf log -R IP\n");
		}
	}
#endif


#endif


	return 0;
}
////////////////////////////////////////////////////////////////////////

