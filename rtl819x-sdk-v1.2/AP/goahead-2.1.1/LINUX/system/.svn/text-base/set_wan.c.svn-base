

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
extern int setFirewallIptablesRules(int argc, char** argv);
extern int Last_WAN_Mode;
void start_dns_relay(void);
void start_igmpproxy(char *wan_iface, char *lan_iface);
void del_routing(void);

#define DHCPD_CONF_FILE "/var/udhcpd.conf"

int avoid_confliction_ip(char *wanIp, char *wanMask)
{
	char line_buffer[100]={0};
	char *strtmp=NULL;
	char tmp1[64]={0};
	unsigned int tmp1Val;
	struct in_addr inIp, inMask, inGateway;
	struct in_addr myIp, myMask, mask;
	unsigned int inIpVal, inMaskVal, myIpVal, myMaskVal, maskVal;
	char tmpBufIP[64]={0}, tmpBufMask[64]={0};
	
	if ( !inet_aton(wanIp, &inIp) ) {
		printf("\r\n Invalid IP-address value!__[%s-%u]\r\n",__FILE__,__LINE__);
		return 0;
	}
	
	if ( !inet_aton(wanMask, &inMask) ) {
		printf("\r\n Invalid IP-address value!__[%s-%u]\r\n",__FILE__,__LINE__);
		return 0;
	}
	
	memcpy(&inIpVal, &inIp, 4);
	memcpy(&inMaskVal, &inMask, 4);


	getInAddr("br0", IP_ADDR_T, (void *)&myIp );	
	getInAddr("br0", NET_MASK_T, (void *)&myMask );
		
	
	memcpy(&myIpVal, &myIp, 4);
	memcpy(&myMaskVal, &myMask, 4);

//printf("\r\n inIpVal=[0x%x],__[%s-%u]\r\n",inIpVal,__FILE__,__LINE__);
//printf("\r\n inMaskVal=[0x%x],__[%s-%u]\r\n",inMaskVal,__FILE__,__LINE__);
//printf("\r\n myIpVal=[0x%x],__[%s-%u]\r\n",myIpVal,__FILE__,__LINE__);
//printf("\r\n myMaskVal=[0x%x],__[%s-%u]\r\n",myMaskVal,__FILE__,__LINE__);

	memcpy(&maskVal,myMaskVal>inMaskVal?&inMaskVal:&myMaskVal,4);
	
//printf("\r\n maskVal=[0x%x],__[%s-%u]\r\n",maskVal,__FILE__,__LINE__);
		
	if((inIpVal & maskVal) == (myIpVal & maskVal)) //wan ip conflict lan ip 
	{
		int i=0, j=0;
//printf("\r\n wan ip conflict lan ip!,__[%s-%u]\r\n",__FILE__,__LINE__);

		for(i=0; i<32; i++)
		{
			if((maskVal & (1<<i)) != 0)
				break;
		}
		
		if((myIpVal & (1<<i)) == 0)
		{
			myIpVal = myIpVal+(1<<i);
		}
		else
		{
			myIpVal = myIpVal-(1<<i);
		}
		
		memcpy(&myIp, &myIpVal, 4);
				
						
		for(j=0; j<32; j++)
		{
			if((myMaskVal & (1<<j)) != 0)
				break;
		}
		
		j=(32-j)/8;

		
		system("killall -9 udhcpd 2> /dev/null");
		system("rm -f /var/run/udhcpd.pid 2> /dev/null");
		system("rm -f /var/udhcpd.conf");
		
		sprintf(line_buffer,"interface %s\n","br0");
		write_line_to_file(DHCPD_CONF_FILE, 1, line_buffer);
		
		apmib_get(MIB_DHCP_CLIENT_START,  (void *)tmp1);		
		memcpy(tmp1, &myIpVal,  j);
		strtmp= inet_ntoa(*((struct in_addr *)tmp1));
//printf("\r\n start ip=[%s],__[%s-%u]\r\n",strtmp,__FILE__,__LINE__);		
		sprintf(line_buffer,"start %s\n",strtmp);
		write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
		
		apmib_get(MIB_DHCP_CLIENT_END,  (void *)tmp1);		
		memcpy(tmp1, &myIpVal,  j);
		strtmp= inet_ntoa(*((struct in_addr *)tmp1));
//printf("\r\n end ip=[%s],__[%s-%u]\r\n",strtmp,__FILE__,__LINE__);		
		sprintf(line_buffer,"end %s\n",strtmp);
		write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
	
//printf("\r\n subnet mask=[%s],__[%s-%u]\r\n",inet_ntoa(myMask),__FILE__,__LINE__);			
		sprintf(line_buffer,"opt subnet %s\n",inet_ntoa(myMask));
		write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);

//printf("\r\n gateway ip=[%s],__[%s-%u]\r\n",inet_ntoa(myIp),__FILE__,__LINE__);					
		sprintf(line_buffer,"opt router %s\n",inet_ntoa(myIp));
		write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);

//printf("\r\n dns ip=[%s],__[%s-%u]\r\n",inet_ntoa(myIp),__FILE__,__LINE__);							
		sprintf(line_buffer,"opt dns %s\n",inet_ntoa(myIp)); /*now strtmp is ip address value */
		write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
		
		memset(tmp1,0x00,sizeof(tmp1));
		apmib_get( MIB_DOMAIN_NAME, (void *)&tmp1);
		if(tmp1[0]){
			sprintf(line_buffer,"opt domain %s\n",tmp1);
			write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
		}
		
		memset(tmp1,0x00,sizeof(tmp1));
		memcpy(tmp1, &myIpVal,  4);
		strtmp= inet_ntoa(*((struct in_addr *)tmp1));
		sprintf(tmpBufIP,"%s",strtmp);
//printf("\r\n tmpBufIP=[%s],__[%s-%u]\r\n",tmpBufIP,__FILE__,__LINE__);

		memset(tmp1,0x00,sizeof(tmp1));
		memcpy(tmp1, &myMaskVal,  4);
		strtmp= inet_ntoa(*((struct in_addr *)tmp1));
		sprintf(tmpBufMask,"%s",strtmp);
//printf("\r\n tmpBufMask=[%s],__[%s-%u]\r\n",tmpBufMask,__FILE__,__LINE__);

		memset(line_buffer,0x00,sizeof(line_buffer));
		sprintf(line_buffer, "ifconfig br0 %s netmask %s", tmpBufIP, tmpBufMask);
//printf("\r\n line_buffer=[%s],__[%s-%u]\r\n",line_buffer,__FILE__,__LINE__);									
		system(line_buffer);

		sprintf(line_buffer, "udhcpd %s", DHCPD_CONF_FILE);
		system(line_buffer);
		//start_dnrd();
		return 1;
	}
	
	return 0;

}

void wan_connect(char *interface, char *option)
{
	char line[128], arg_buff[200];
	char *cmd_opt[16];
	int cmd_cnt = 0, intValue=0, x, dns_mode=0, index=0;
	int dns_found=0, wan_type=0, conn_type=0, ppp_mtu=0;
	struct in_addr wanaddr, lanaddr;
	char *strtmp=NULL;
	char wanip[32]={0}, mask[32]={0},remoteip[32]={0};
	char nameserver[32], nameserver_ip[32];
	char dns_server[5][32];
	char tmp_args[16]={0};
	char *token=NULL, *savestr1=NULL;
	FILE *fp1;
	unsigned char domanin_name[MAX_NAME_LEN]={0};
	unsigned char cmdBuffer[100]={0};
	unsigned char tmpBuff[200]={0};
	unsigned char dynip[32]={0};
	int ret=0;
//printf("wan_connect option=%s\n", option);
//printf("wan_connect interface=%s\n", interface);
	
	apmib_get(MIB_WAN_DHCP,(void *)&wan_type);
	apmib_get( MIB_WAN_DNS_MODE, (void *)&dns_mode);
	
	
	

	if(!strcmp(interface, "ppp0")){

#if 1//AVOID_CONFLICTION_IP
		getInAddr("ppp0", IP_ADDR_T, (void *)&wanaddr);
		strtmp = inet_ntoa(wanaddr);
		sprintf(wanip, "%s",strtmp); 
//printf("\r\n wanip=[%s],__[%s-%u]\r\n",wanip,__FILE__,__LINE__);
		getInAddr("ppp0", NET_MASK_T, (void *)&wanaddr);
		strtmp = inet_ntoa(wanaddr);
		sprintf(mask, "%s",strtmp); 
//printf("\r\n mask=[%s],__[%s-%u]\r\n",mask,__FILE__,__LINE__);
		ret = avoid_confliction_ip(wanip,mask);
#endif
		
		
		if(wan_type==PPTP){
			apmib_get(MIB_PPTP_CONNECTION_TYPE, (void *)&conn_type);
			if(intValue==1){
				RunSystemCmd(PROC_PPTP_CONN_FILE, "echo", "5", NULL_STR);
			}else{
				RunSystemCmd(PROC_PPTP_CONN_FILE, "echo", "0", NULL_STR);
			}
			intValue = getInAddr("ppp0", 0, (void *)&wanaddr);
			if(intValue==1){
				strtmp = inet_ntoa(wanaddr);
				sprintf(remoteip, "%s",strtmp); 
				RunSystemCmd(NULL_FILE, "route", "add", "-net", "default", "gw", remoteip, "dev", "ppp0", NULL_STR);
			}
			
		}
		if(wan_type==PPTP || wan_type==L2TP){
			token=NULL;
			savestr1=NULL;	     
			sprintf(arg_buff, "%s", option);
		
			token = strtok_r(arg_buff," ", &savestr1);
			x=0;
			do{
				if (token == NULL){/*check if the first arg is NULL*/
					break;
				}else{   
					if(x==1){
						ppp_mtu = atoi(token);
						break;
					}
					if(!strcmp(token, "mtu"))
						x=1;
				}
			
				token = strtok_r(NULL, " ", &savestr1);
			}while(token !=NULL);  
		
		}
		if(wan_type==PPTP){
			apmib_get(MIB_PPTP_MTU_SIZE, (void *)&intValue);
			if(ppp_mtu > 0 && intValue > ppp_mtu)
				intValue = ppp_mtu;
			sprintf(tmp_args, "%d", intValue);
		}else if(wan_type==L2TP){
			apmib_get(MIB_L2TP_MTU_SIZE, (void *)&intValue);
			if(ppp_mtu > 0 && intValue > ppp_mtu)
				intValue = ppp_mtu;
			sprintf(tmp_args, "%d", intValue);
		}else if(wan_type==PPPOE){
			apmib_get(MIB_PPP_MTU_SIZE, (void *)&intValue);
			sprintf(tmp_args, "%d", intValue);
			
		}
		RunSystemCmd(NULL_FILE, "ifconfig", "ppp0", "mtu", tmp_args, "txqueuelen", "25",NULL_STR);
		if(dns_mode==1){
			start_dns_relay();
		}else{
			fp1= fopen(PPP_RESOLV_FILE, "r");
			if (fp1 != NULL){
				for (x=0;x<5;x++){
					memset(dns_server[x], '\0', 32);
				}
				while (fgets(line, sizeof(line), fp1) != NULL) {
						memset(nameserver_ip, '\0', 32);
						dns_found = 0;
						sscanf(line, "%s %s", nameserver, nameserver_ip);
						for(x=0;x<5;x++){
							if(dns_server[x][0] != '\0'){
								if(!strcmp(dns_server[x],nameserver_ip)){
									dns_found = 1; 
									break;
								}
							}
						}
						if(dns_found ==0){
							for(x=0;x<5;x++){
								if(dns_server[x][0] == '\0'){
									sprintf(dns_server[x], "%s", nameserver_ip);
									break;
								}
							}
						}
					
				}
				fclose(fp1);
			}
			//for (x=0;x<5;x++){
			//	if(dns_server[x]){
			//		fprintf(stderr, "name server=%s\n", dns_server[x]);
			//	}
			//}
			
			RunSystemCmd(NULL_FILE, "killall", "dnrd", NULL_STR);
			if(isFileExist(DNRD_PID_FILE)){
				unlink(DNRD_PID_FILE);
			}
			apmib_get( MIB_DOMAIN_NAME,  (void *)domanin_name);
						
			getInAddr("br0", IP_ADDR_T, (void *)&lanaddr);
			strtmp = inet_ntoa(lanaddr);
			sprintf(dynip, "%s",strtmp); 						
			
			RunSystemCmd(NULL_FILE, "rm", "-f", "/etc/hosts", NULL_STR);
			memset(cmdBuffer, 0x00, sizeof(cmdBuffer));
			
			if(domanin_name[0])
			{
				sprintf(cmdBuffer,"%s\\%s%s%s%s", dynip, domanin_name, "AP.com|",domanin_name, "AP.net");				
			}
			else
			{
				sprintf(cmdBuffer,"%s\\%s%s%s%s", dynip, "realtek", "AP.com|","realtek", "AP.net");
			}
			RunSystemCmd("/etc/hosts", "echo",cmdBuffer,NULL_STR);
	
			cmd_opt[cmd_cnt++]="dnrd";
			cmd_opt[cmd_cnt++]="--cache=off";
			for(x=0;x<5;x++){
				if(dns_server[x][0] != '\0'){
					cmd_opt[cmd_cnt++]="-s";
					cmd_opt[cmd_cnt++]=&dns_server[x][0];
				}
			}
			cmd_opt[cmd_cnt++] = 0;
			//for (x=0; x<cmd_cnt;x++)
			//	fprintf(stderr, "cmd index=%d, opt=%s \n", x, cmd_opt[x]);
			
			
			RunSystemCmd(NULL_FILE, "cp", PPP_RESOLV_FILE, "/var/resolv.conf", NULL_STR);	
			DoCmd(cmd_opt, NULL_FILE);
		}
	}else if(strcmp(interface, "ppp0")){
		for (x=0;x<5;x++){
			memset(dns_server[x], '\0', 32);
		}
		token=NULL;
		savestr1=NULL;	     
		sprintf(arg_buff, "%s", option);
	
		token = strtok_r(arg_buff," ", &savestr1);
		index=1;
		do{
			dns_found=0;
			if (token == NULL){/*check if the first arg is NULL*/
				break;
			}else{   
				if(index==2)
					sprintf(wanip, "%s", token); /*wan ip address */
				if(index==3)
					sprintf(mask, "%s", token); /*subnet mask*/
				if(index==4)
					sprintf(remoteip, "%s", token); /*gateway ip*/			
				if(index > 4){
					for(x=0;x<5;x++){
						if(dns_server[x][0] != '\0'){
							if(!strcmp(dns_server[x], token)){
								dns_found = 1; 
								break;
							}
						}
					}
					if(dns_found ==0){
						for(x=0;x<5;x++){
							if(dns_server[x][0] == '\0'){
								sprintf(dns_server[x], "%s", token);
								break;
							}
						}
					}
				}
			}
			index++;
			token = strtok_r(NULL, " ", &savestr1);
		}while(token !=NULL);  
		
		RunSystemCmd(NULL_FILE, "ifconfig", interface, wanip, "netmask", mask, NULL_STR);	
		RunSystemCmd(NULL_FILE, "route", "add", "-net", "default", "gw", remoteip, "dev", interface, NULL_STR);
		
#if 1//AVOID_CONFLICTION_IP
		ret = avoid_confliction_ip(wanip,mask);

#endif

			
		if(dns_mode==1){
			start_dns_relay();
		}else{
			RunSystemCmd(NULL_FILE, "killall", "dnrd", NULL_STR);
			if(isFileExist(DNRD_PID_FILE)){
				unlink(DNRD_PID_FILE);
			}
			apmib_get( MIB_DOMAIN_NAME,  (void *)domanin_name);
						
			getInAddr("br0", IP_ADDR_T, (void *)&lanaddr);
			strtmp = inet_ntoa(lanaddr);
			sprintf(dynip, "%s",strtmp); 						
			
			RunSystemCmd(NULL_FILE, "rm", "-f", "/etc/hosts", NULL_STR);
			memset(cmdBuffer, 0x00, sizeof(cmdBuffer));
			
			if(domanin_name[0])
			{
				sprintf(cmdBuffer,"%s\\%s%s%s%s", dynip, domanin_name, "AP.com|",domanin_name, "AP.net");				
			}
			else
			{
				sprintf(cmdBuffer,"%s\\%s%s%s%s", dynip, "realtek", "AP.com|","realtek", "AP.net");
			}
			RunSystemCmd("/etc/hosts", "echo",cmdBuffer,NULL_STR);
			
			cmd_opt[cmd_cnt++]="dnrd";
			cmd_opt[cmd_cnt++]="--cache=off";
			for(x=0;x<5;x++){
				if(dns_server[x][0] != '\0'){
					cmd_opt[cmd_cnt++]="-s";
					cmd_opt[cmd_cnt++]=&dns_server[x][0];
					sprintf(line,"nameserver %s\n", dns_server[x]);
					if(x==0)
						write_line_to_file(RESOLV_CONF, 1, line);
					else
						write_line_to_file(RESOLV_CONF, 2, line);
				}
			}
			cmd_opt[cmd_cnt++] = 0;
			//for (x=0; x<cmd_cnt;x++)
			//	printf("cmd index=%d, opt=%s \n", x, cmd_opt[x]);
			 if(strcmp(interface, "br0")){
			 	DoCmd(cmd_opt, NULL_FILE);	
			}
			
		}
		 if(strcmp(interface, "br0")){	
		 	setFirewallIptablesRules(0, NULL);  
		}
		 
	}
	 if(strcmp(interface, "br0")){
#if	!defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT)
	 	if(ret == 1)
#endif	 		
	 	{
			system("ifconfig eth0 down");
			system("ifconfig wlan0 down");
	
	//printf("\r\n __[%s-%u]\r\n",__FILE__,__LINE__);	
			sleep(5);
			
			system("ifconfig eth0 up");
			system("ifconfig wlan0 up");
		}
//printf("\r\n __[%s-%u]\r\n",__FILE__,__LINE__);		


		system("echo \"WAN Connected\n\" > var/wanlink");
		
		printf("WAN Connected\n");
		start_ntp();
		start_ddns();
		start_igmpproxy(interface, "br0");
		del_routing();
		start_routing(interface);
	}
}

void wan_disconnect(char *option)
{
	int intValue=0;
	int wan_type=0;
	int Last_WAN_Mode=0;
	FILE *fp;

//printf("WAN Disconnect option=%s\n", option);
	if(isFileExist(LAST_WAN_TYPE_FILE)){
		fp= fopen(LAST_WAN_TYPE_FILE, "r");
		if (!fp) {
	        	printf("can not /var/system/last_wan\n");
			return; 
	   	}
		fscanf(fp,"%d",&Last_WAN_Mode);
		fclose(fp);
	}
	RunSystemCmd("/var/disc", "echo", "enter", NULL_STR); 
	
	apmib_get(MIB_WAN_DHCP,(void *)&wan_type);
	
	RunSystemCmd(NULL_FILE, "killall", "-15", "routed", NULL_STR); 
	
	RunSystemCmd(NULL_FILE, "killall", "-9", "ntp_inet", NULL_STR);
	if(isFileExist("/var/ntp_run")){
		unlink("/var/ntp_run");
	} 
	
	RunSystemCmd(NULL_FILE, "killall", "-15", "ddns_inet", NULL_STR); 
	RunSystemCmd(NULL_FILE, "killall", "-9", "updatedd", NULL_STR);
	RunSystemCmd(NULL_FILE, "killall", "-9", "ntpclient", NULL_STR);
	RunSystemCmd("/proc/pptp_src_ip", "echo", "0 0", NULL_STR);
	
#if	defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT)
	if(!strcmp(option, "all")){				
		RunSystemCmd(NULL_FILE, "killall", "-9", "dnrd", NULL_STR);
		if(isFileExist(DNRD_PID_FILE)){
			unlink(DNRD_PID_FILE);
		}										
	}
	else if(!strcmp(option, "dhcpc"))
	{
		unsigned char dynip[32]={0};
		struct in_addr	intaddr;
		unsigned char cmdBuffer[100]={0};
		
		if ( getInAddr("eth1", IP_ADDR_T, (void *)&intaddr ) )
			sprintf(dynip,"%s",inet_ntoa(intaddr));
		else
			sprintf(dynip,"%s","0.0.0.0");
			
		if(strcmp(dynip, "0.0.0.0") != 0) //do nothing at first time
		{
			system("echo \"WAN Disconnected\n\" > var/wanlink");
			system("killall -9 dnrd 2> /dev/null");
			system("rm -f /var/hosts 2> /dev/null");
			
			if ( getInAddr("br0", IP_ADDR_T, (void *)&intaddr ) )
				sprintf(dynip,"%s",inet_ntoa(intaddr));
			else
				sprintf(dynip,"%s","0.0.0.0");
							
			sprintf(cmdBuffer,"%s\\%s", dynip, "AlwaysHost");

			RunSystemCmd("/etc/hosts", "echo",cmdBuffer,NULL_STR);
			
			system("ifconfig eth0 down");
			system("ifconfig wlan0 down");
	
			sleep(10);
			
			system("ifconfig eth0 up");
			system("ifconfig wlan0 up");
			
			system("dnrd --cache=off -s 168.95.1.1");
		}
	}
	else
	{
			if(isFileExist(PPPLINKFILE)){ //Last state, ppp0 is not connected, we do not kill dnrd
				RunSystemCmd(NULL_FILE, "killall", "-9", "dnrd", NULL_STR);
				if(isFileExist(DNRD_PID_FILE)){
					unlink(DNRD_PID_FILE);
				}
			}
		}
	#else
		RunSystemCmd(NULL_FILE, "killall", "-9", "dnrd", NULL_STR);
		if(isFileExist(DNRD_PID_FILE)){
			unlink(DNRD_PID_FILE);
		}
	#endif
	
	RunSystemCmd(NULL_FILE, "killall", "-9", "igmpproxy", NULL_STR);
	if(isFileExist(IGMPPROXY_PID_FILE)){
		unlink(IGMPPROXY_PID_FILE);
	}
	//printf("Last_WAN_Mode==%d\n", Last_WAN_Mode);
	if(isFileExist("/var/pppdkilled")==0){
		RunSystemCmd("/var/pppdkilled", "echo", "killed", NULL_STR);
		
	if(Last_WAN_Mode==PPPOE){
			RunSystemCmd(NULL_FILE, "killall", "-15", "pppd", NULL_STR);
		}else{
			if(Last_WAN_Mode==L2TP &&!strcmp(option, "all")){ 
				RunSystemCmd(NULL_FILE, "killall", "-9", "pppd", NULL_STR);
			}else if(Last_WAN_Mode==L2TP &&!strcmp(option, "option")){
				RunSystemCmd(NULL_FILE, "killall", "-15", "pppd", NULL_STR);
			}else{
				RunSystemCmd(NULL_FILE, "killall", "-9", "pppd", NULL_STR);
			}
		}
	}
	if(!strcmp(option, "all"))
		RunSystemCmd(NULL_FILE, "killall", "-9", "ppp_inet", NULL_STR); 
		
	RunSystemCmd(NULL_FILE, "killall", "-9", "pptp", NULL_STR);
	RunSystemCmd(NULL_FILE, "killall", "-9", "pppoe", NULL_STR);
	if(isFileExist(PPPD_PID_FILE)){
		unlink(PPPD_PID_FILE);
	} 

	if(wan_type==L2TP && !strcmp(option, "option") && isFileExist(PPPLINKFILE)){
		apmib_get( MIB_L2TP_CONNECTION_TYPE, (void *)&intValue);
		if(intValue==1){
			if(isFileExist("/var/disc_l2tp")){
				system("echo\"d client\" > /var/run/l2tp-control &");
				system("echo \"l2tpdisc\" > /var/disc_l2tp");
			}
		}
	}
/*clean pptp_info in fastpptp*/
	if(wan_type==PPTP)
		system("echo 1 > /proc/fast_pptp");

	if(isFileExist(FIRSTDDNS)){
	 	unlink(FIRSTDDNS);
	}

	if(!strcmp(option, "option") && isFileExist(PPPLINKFILE)){
		RunSystemCmd(NULL_FILE, "rm", "-f", "/etc/ppp/first", NULL_STR);
		RunSystemCmd(NULL_FILE, "rm", "-f", "/etc/ppp/firstpptp", NULL_STR);
		RunSystemCmd(NULL_FILE, "rm", "-f", "/etc/ppp/firstl2tp", NULL_STR);
		RunSystemCmd(NULL_FILE, "rm", "-f", "/etc/ppp/firstdemand", NULL_STR);
	}
	if(isFileExist(PPPLINKFILE)){
	 	unlink(PPPLINKFILE);
	}
	/*in PPPOE and PPTP mode do this in pppd , not here !!*/
	if((wan_type !=L2TP && wan_type !=PPPOE) || strcmp(option, "option")){
		if(isFileExist(PPP_CONNECT_FILE)){
	 		unlink(PPP_CONNECT_FILE);
		}
	}
	if(wan_type==PPTP){
		apmib_get(MIB_PPTP_CONNECTION_TYPE, (void *)&intValue);
		if(intValue==1){
			RunSystemCmd(PROC_PPTP_CONN_FILE, "echo", "3", NULL_STR);
		}else{
			RunSystemCmd(PROC_PPTP_CONN_FILE, "echo", "0", NULL_STR);
		}
	}
	RunSystemCmd(NULL_FILE, "rm", "-f", "/var/disc", NULL_STR);
	RunSystemCmd(NULL_FILE, "rm", "-f", "/var/disc_l2tp", NULL_STR);
	RunSystemCmd(NULL_FILE, "rm", "-f", "/var/pppdkilled", NULL_STR);
}

/*write dns server ip address to resolv.conf file and start dnrd
* 
*/
void start_dns_relay(void)
{
	char tmpBuff1[32]={0}, tmpBuff2[32]={0}, tmpBuff3[32]={0};
	int intValue=0;
	char line_buffer[100]={0};
	char tmp1[32]={0}, tmp2[32]={0}, tmp3[32]={0};
	char *strtmp=NULL;
	
	RunSystemCmd(NULL_FILE, "killall", "-9", "dnrd", NULL_STR);
	apmib_get( MIB_WAN_DNS1,  (void *)tmpBuff1);
	apmib_get( MIB_WAN_DNS2,  (void *)tmpBuff2);
	apmib_get( MIB_WAN_DNS3,  (void *)tmpBuff3);
	
	if (memcmp(tmpBuff1, "\x0\x0\x0\x0", 4))
		intValue++;
	if (memcmp(tmpBuff2, "\x0\x0\x0\x0", 4))
		intValue++;
	if (memcmp(tmpBuff3, "\x0\x0\x0\x0", 4))
		intValue++;	
			
	if(intValue==1){
		strtmp= inet_ntoa(*((struct in_addr *)tmpBuff1));
		sprintf(tmp1,"%s",strtmp);
		sprintf(line_buffer,"nameserver %s\n",strtmp);
		write_line_to_file(RESOLV_CONF,1, line_buffer);
		RunSystemCmd(NULL_FILE, "dnrd", "--cache=off", "-s", tmp1, NULL_STR);
		
	}else if(intValue==2){
		strtmp= inet_ntoa(*((struct in_addr *)tmpBuff1));
		sprintf(tmp1,"%s",strtmp);
		sprintf(line_buffer,"nameserver %s\n",strtmp);
		write_line_to_file(RESOLV_CONF,1, line_buffer);
		
		strtmp= inet_ntoa(*((struct in_addr *)tmpBuff2));
		sprintf(tmp2,"%s",strtmp);
		sprintf(line_buffer,"nameserver %s\n", strtmp);
		write_line_to_file(RESOLV_CONF,2, line_buffer);
		RunSystemCmd(NULL_FILE, "dnrd", "--cache=off", "-s", tmp1, "-s", tmp2, NULL_STR);
	}else if(intValue==3){
		strtmp= inet_ntoa(*((struct in_addr *)tmpBuff1));
		sprintf(tmp1,"%s",strtmp);
		sprintf(line_buffer,"nameserver %s\n",strtmp);
		write_line_to_file(RESOLV_CONF,1, line_buffer);
		
		strtmp= inet_ntoa(*((struct in_addr *)tmpBuff2));
		sprintf(tmp2,"%s",strtmp);
		sprintf(line_buffer,"nameserver %s\n", strtmp);
		write_line_to_file(RESOLV_CONF, 2, line_buffer);
		
		strtmp= inet_ntoa(*((struct in_addr *)tmpBuff3));
		sprintf(tmp3,"%s",strtmp);
		sprintf(line_buffer,"nameserver %s\n", strtmp);
		write_line_to_file(RESOLV_CONF, 2, line_buffer);
		
		RunSystemCmd(NULL_FILE, "dnrd", "--cache=off", "-s", tmp1, "-s", tmp2, "-s", tmp3, NULL_STR);
	}else{
		printf("Invalid DNS server setting\n");
	}	
}
void start_upnp_igd(int wantype, int sys_opmode, int wisp_id, char *lan_interface)
{
	int intValue=0;
	char tmp1[16]={0};
	char tmp2[16]={0};
	apmib_get(MIB_UPNP_ENABLED, (void *)&intValue);
	RunSystemCmd(NULL_FILE, "killall", "-15", "miniigd", NULL_STR); 
	if(intValue==1){
		RunSystemCmd(NULL_FILE, "route", "del", "-net", "239.255.255.250", "netmask", "255.255.255.255", lan_interface, NULL_STR); 
		RunSystemCmd(NULL_FILE, "route", "add", "-net", "239.255.255.250", "netmask", "255.255.255.255", lan_interface, NULL_STR); 
		sprintf(tmp1, "%d", wantype);
		sprintf(tmp2, "%d", wisp_id);
		if(sys_opmode==2)
			RunSystemCmd(NULL_FILE, "miniigd", "-e", tmp1, "-i", lan_interface, "-w", tmp2, NULL_STR); 
		else	
			RunSystemCmd(NULL_FILE, "miniigd", "-e", tmp1, "-i", lan_interface, NULL_STR); 
		
	}
	
}
void start_ddns(void)
{
	unsigned int ddns_onoff;
	unsigned int ddns_type;
	unsigned char ddns_domanin_name[MAX_DOMAIN_LEN];
	unsigned char ddns_user_name[MAX_DOMAIN_LEN];
	unsigned char ddns_password[MAX_DOMAIN_LEN];
	
	RunSystemCmd(NULL_FILE, "killall", "-9", "ddns_inet", NULL_STR);
	
	apmib_get( MIB_DDNS_ENABLED,  (void *)&ddns_onoff);

	if(ddns_onoff == 1)
	{
		apmib_get( MIB_DDNS_TYPE,  (void *)&ddns_type);

		apmib_get( MIB_DDNS_DOMAIN_NAME,  (void *)ddns_domanin_name);

		apmib_get( MIB_DDNS_USER,  (void *)ddns_user_name);

		apmib_get( MIB_DDNS_PASSWORD,  (void *)ddns_password);		

		if(ddns_type == 0) // 0:ddns; 1:tzo
			RunSystemCmd(NULL_FILE, "ddns_inet", "-x", "dyndns", ddns_user_name, ddns_password, ddns_domanin_name, NULL_STR);
		else if(ddns_type == 1)
			RunSystemCmd(NULL_FILE, "ddns_inet", "-x", "tzo", ddns_user_name, ddns_password, ddns_domanin_name, NULL_STR);


	}

}

#define NTPTMP_FILE "/tmp/ntp_tmp"
#define TZ_FILE "/var/TZ"
void start_ntp(void)
{
	unsigned int ntp_onoff=0;
	unsigned char buffer[500];

	unsigned int ntp_server_id;
	unsigned char	ntp_server[40];
	unsigned int daylight_save = 1;
	unsigned char daylight_save_str[5];
	unsigned char time_zone[8];

	unsigned char command[100], str_datnight[100];
	unsigned char *str_tz1;
	
	apmib_get(MIB_NTP_ENABLED, (void *)&ntp_onoff);
	RunSystemCmd(NULL_FILE, "killall", "-9", "ntp_inet", NULL_STR);
	RunSystemCmd(NULL_FILE, "rm", NTPTMP_FILE, NULL_STR);
	RunSystemCmd(NULL_FILE, "rm", TZ_FILE, NULL_STR);
	if(ntp_onoff == 1)
	{
		RunSystemCmd(NULL_FILE, "echo", "Start NTP daemon", NULL_STR);
		/* prepare requested info for ntp daemon */
		apmib_get( MIB_NTP_SERVER_ID,  (void *)&ntp_server_id);

		if(ntp_server_id == 0)
			apmib_get( MIB_NTP_SERVER_IP1,  (void *)buffer);
		else
			apmib_get( MIB_NTP_SERVER_IP2,  (void *)buffer);

		sprintf(ntp_server, "%s", inet_ntoa(*((struct in_addr *)buffer)));

		apmib_get( MIB_DAYLIGHT_SAVE,  (void *)&daylight_save);
		memset(daylight_save_str, 0x00, sizeof(daylight_save_str));
		sprintf(daylight_save_str,"%u",daylight_save);
		
		apmib_get( MIB_NTP_TIMEZONE,  (void *)&time_zone);

		if(daylight_save == 0)
			sprintf( str_datnight, "%s", "");
		else if(strcmp(time_zone,"9 1") == 0)
			sprintf( str_datnight, "%s", "PDT,M4.1.0/02:00:00,M10.5.0/02:00:00");
		else if(strcmp(time_zone,"8 1") == 0)
			sprintf( str_datnight, "%s", "PDT,M4.1.0/02:00:00,M10.5.0/02:00:00");
		else if(strcmp(time_zone,"7 2") == 0)
                        sprintf( str_datnight, "%s", "PDT,M4.1.0/02:00:00,M10.5.0/02:00:00");
               else if(strcmp(time_zone,"6 1") == 0)
                        sprintf( str_datnight, "%s", "PDT,M4.1.0/02:00:00,M10.5.0/02:00:00");
               else if(strcmp(time_zone,"6 2") == 0)
                        sprintf( str_datnight, "%s", "PDT,M4.1.0/02:00:00,M10.5.0/02:00:00");
               else if(strcmp(time_zone,"5 2") == 0)
                        sprintf( str_datnight, "%s", "PDT,M4.1.0/02:00:00,M10.5.0/02:00:00");
               else if(strcmp(time_zone,"5 3") == 0)
                        sprintf( str_datnight, "%s", "PDT,M4.1.0/02:00:00,M10.5.0/02:00:00");
               else if(strcmp(time_zone,"4 3") == 0)
                        sprintf( str_datnight, "%s", "PDT,M10.2.0/00:00:00,M3.2.0/00:00:00");
               else if(strcmp(time_zone,"3 1") == 0)
                        sprintf( str_datnight, "%s", "PDT,M4.1.0/00:00:00,M10.5.0/00:00:00");
               else if(strcmp(time_zone,"3 2") == 0)
                        sprintf( str_datnight, "%s", "PDT,M2.2.0/00:00:00,M10.2.0/00:00:00");
               else if(strcmp(time_zone,"1 1") == 0)
                        sprintf( str_datnight, "%s", "PDT,M3.5.0/00:00:00,M10.5.0/01:00:00");
               else if(strcmp(time_zone,"0 2") == 0)
                        sprintf( str_datnight, "%s", "PDT,M3.5.0/01:00:00,M10.5.0/02:00:00");
               else if(strcmp(time_zone,"-1") == 0)
                        sprintf( str_datnight, "%s", "PDT,M3.5.0/02:00:00,M10.5.0/03:00:00");
               else if(strcmp(time_zone,"-2 1") == 0)
                        sprintf( str_datnight, "%s", "PDT,M3.5.0/02:00:00,M10.5.0/03:00:00");
               else if(strcmp(time_zone,"-2 2") == 0)
                        sprintf( str_datnight, "%s", "PDT,M3.5.0/03:00:00,M10.5.0/04:00:00");
               else if(strcmp(time_zone,"-2 3") == 0)
                        sprintf( str_datnight, "%s", "PDT,M4.5.5/00:00:00,M9.5.5/00:00:00");
               else if(strcmp(time_zone,"-2 5") == 0)
                        sprintf( str_datnight, "%s", "PDT,M3.5.0/03:00:00,M10.5.5/04:00:00");
               else if(strcmp(time_zone,"-2 6") == 0)
                        sprintf( str_datnight, "%s", "PDT,M3.5.5/02:00:00,M10.1.0/02:00:00");
               else if(strcmp(time_zone,"-3 2") == 0)
                        sprintf( str_datnight, "%s", "PDT,M3.5.0/02:00:00,M10.5.0/03:00:00");
               else if(strcmp(time_zone,"-4 2") == 0)
                        sprintf( str_datnight, "%s", "PDT,M3.5.0/04:00:00,M10.5.0/05:00:00");
               else if(strcmp(time_zone,"-9 4") == 0)
                        sprintf( str_datnight, "%s", "PDT,M10.5.0/02:00:00,M4.1.0/03:00:00");
               else if(strcmp(time_zone,"-10 2") == 0)
                        sprintf( str_datnight, "%s", "PDT,M10.5.0/02:00:00,M4.1.0/03:00:00");
               else if(strcmp(time_zone,"-10 4") == 0)
                        sprintf( str_datnight, "%s", "PDT,M10.1.0/02:00:00,M4.1.0/03:00:00");
               else if(strcmp(time_zone,"-10 5") == 0)
                        sprintf( str_datnight, "%s", "PDT,M3.5.0/02:00:00,M10.5.0/03:00:00");
               else if(strcmp(time_zone,"-12 1") == 0)
                        sprintf( str_datnight, "%s", "PDT,M3.2.0/03:00:00,M10.1.0/02:00:00");
               else
                        sprintf( str_datnight, "%s", "");

		str_tz1 = gettoken(time_zone, 0, ' ');
		
		if(strcmp(time_zone,"3 1") == 0 ||
			strcmp(time_zone,"-3 4") == 0 ||
		 	strcmp(time_zone,"-4 3") == 0 ||
		 	strcmp(time_zone,"-5 3") == 0 ||
		 	strcmp(time_zone,"-9 4") == 0 ||
		 	strcmp(time_zone,"-9 5") == 0
		)
		{
                       sprintf( command, "GMT%s:30%s", str_tz1, str_datnight);
		}
		else
			sprintf( command, "GMT%s%s", str_tz1, str_datnight);          

		RunSystemCmd(NULL_FILE, "ntp_inet", "-x", ntp_server, command, daylight_save_str, NULL_STR);
}



}

void del_routing(void)
{
	int intValue=0, i;
	char	ip[32], netmask[32], gateway[32], *tmpStr=NULL;	
	int entry_Num=0;
	STATICROUTE_T entry;
	
	apmib_get(MIB_STATICROUTE_NUM, (void *)&entry_Num);
	if(entry_Num > 0){
		for (i=1; i<=entry_Num; i++) {
			*((char *)&entry) = (char)i;
			apmib_get(MIB_STATICROUTE, (void *)&entry);
	
			tmpStr = inet_ntoa(*((struct in_addr *)entry.dstAddr));
			sprintf(ip, "%s", tmpStr);
			tmpStr = inet_ntoa(*((struct in_addr *)entry.netmask));
			sprintf(netmask, "%s", tmpStr);
			tmpStr = inet_ntoa(*((struct in_addr *)entry.gateway));
			sprintf(gateway, "%s", tmpStr);
			
			RunSystemCmd(NULL_FILE, "route", "del", "-net", ip, "netmask", netmask, "gw",  gateway, NULL_STR);
		}
	}
}

void start_routing(char *interface)
{
	int intValue=0, i;
	char line_buffer[64]={0};
	char tmp_args[16]={0};
	char	ip[32], netmask[32], gateway[32], *tmpStr=NULL;	
	int entry_Num=0;
	STATICROUTE_T entry;
	int nat_enabled=0, rip_enabled=0, rip_wan_tx=0;
	int rip_wan_rx=0, rip_lan_tx=0, rip_lan_rx=0;
	int start_routed=1;
	
	RunSystemCmd(NULL_FILE, "killall", "-15", "routed", NULL_STR); 
	apmib_get(MIB_NAT_ENABLED, (void *)&nat_enabled);
	apmib_get(MIB_RIP_ENABLED, (void *)&rip_enabled);
	apmib_get(MIB_RIP_LAN_TX, (void *)&rip_lan_tx);
	apmib_get(MIB_RIP_LAN_RX, (void *)&rip_lan_rx);
	apmib_get(MIB_RIP_WAN_TX, (void *)&rip_wan_tx);
	apmib_get(MIB_RIP_WAN_RX, (void *)&rip_wan_rx);
	line_buffer[0]=0x0d;
	line_buffer[1]=0x0a;
	write_line_to_file(ROUTED_CONF_FILE,1, line_buffer);
	memset(line_buffer, 0x00, 64);
	if(nat_enabled==0){
		if(rip_lan_tx !=0 && rip_lan_rx==0){
			sprintf(line_buffer,"network br0 0 %d\n",rip_lan_tx);
			write_line_to_file(ROUTED_CONF_FILE, 2 , line_buffer);
			sprintf(line_buffer,"network %s 0 %d\n",interface, rip_lan_tx);
			write_line_to_file(ROUTED_CONF_FILE, 2 , line_buffer);
			
		}else if(rip_lan_tx !=0 && rip_lan_rx !=0){
				sprintf(line_buffer,"network br0 %d %d\n",rip_lan_rx, rip_lan_tx);
				write_line_to_file(ROUTED_CONF_FILE, 2 , line_buffer);
				sprintf(line_buffer,"network %s %d %d\n",interface, rip_lan_rx, rip_lan_tx);
				write_line_to_file(ROUTED_CONF_FILE, 2 , line_buffer);
			
		}else{
			if( rip_lan_rx !=0){
				sprintf(line_buffer,"network br0 %d 0\n",rip_lan_rx);
				write_line_to_file(ROUTED_CONF_FILE, 2 , line_buffer);
				sprintf(line_buffer,"network %s %d 0\n",interface, rip_lan_rx);
				write_line_to_file(ROUTED_CONF_FILE, 2 , line_buffer);
			}else
				start_routed=0;
		}
	}else{
		if( rip_lan_rx !=0){
			sprintf(line_buffer,"network br0 %d 0\n",rip_lan_rx);
			write_line_to_file(ROUTED_CONF_FILE, 2 , line_buffer);
			sprintf(line_buffer,"network %s %d 0\n",interface, rip_lan_rx);
			write_line_to_file(ROUTED_CONF_FILE, 2 , line_buffer);
		}else
			start_routed=0;
	}
	apmib_get(MIB_STATICROUTE_ENABLED, (void *)&intValue);
	apmib_get(MIB_STATICROUTE_NUM, (void *)&entry_Num);
	if(intValue > 0 && entry_Num > 0){
		for (i=1; i<=entry_Num; i++) {
			*((char *)&entry) = (char)i;
			apmib_get(MIB_STATICROUTE, (void *)&entry);
	
			tmpStr = inet_ntoa(*((struct in_addr *)entry.dstAddr));
			sprintf(ip, "%s", tmpStr);
			tmpStr = inet_ntoa(*((struct in_addr *)entry.netmask));
			sprintf(netmask, "%s", tmpStr);
			tmpStr = inet_ntoa(*((struct in_addr *)entry.gateway));
			sprintf(gateway, "%s", tmpStr);
			sprintf(tmp_args, "%d", entry.metric);
			if(!strcmp(interface, "ppp0")){
				if(entry._interface_==1){//wan interface
					RunSystemCmd(NULL_FILE, "route", "add", "-net", ip, "netmask", netmask, "metric", tmp_args, "dev", interface,  NULL_STR);
				}else{
					RunSystemCmd(NULL_FILE, "route", "add", "-net", ip, "netmask", netmask, "gw",  gateway, "metric", tmp_args, "dev", "br0",  NULL_STR);
				}
			}else{
				if(entry._interface_==1){//wan interface
					RunSystemCmd(NULL_FILE, "route", "add", "-net", ip, "netmask", netmask, "gw",  gateway, "metric", tmp_args, "dev", interface,  NULL_STR);
				}else if(entry._interface_==0){
					RunSystemCmd(NULL_FILE, "route", "add", "-net", ip, "netmask", netmask, "gw",  gateway, "metric", tmp_args, "dev", "br0",  NULL_STR);
				}
			}
		}
	}
	
	if(rip_enabled !=0 && start_routed==1)
		RunSystemCmd(NULL_FILE, "routed", "-s",  NULL_STR);
	
	if(nat_enabled==0){
		if(isFileExist(IGMPPROXY_PID_FILE)){
			unlink(IGMPPROXY_PID_FILE);
		}
		RunSystemCmd(NULL_FILE, "killall", "-9", "igmpproxy", NULL_STR);
	}
}
void start_igmpproxy(char *wan_iface, char *lan_iface)
{
	int intValue=0;
	apmib_get(MIB_IGMP_PROXY_DISABLED, (void *)&intValue);
	RunSystemCmd(NULL_FILE, "killall", "-9", "igmpproxy", NULL_STR);
	if(intValue==0)
		RunSystemCmd(NULL_FILE, "igmpproxy", wan_iface, lan_iface, NULL_STR);
	
}
void start_wan_dhcp_client(char *iface)
{
	char hostname[100];
	char cmdBuff[200];
	char script_file[100], deconfig_script[100], pid_file[100];
	
	sprintf(script_file, "/usr/share/udhcpc/%s.sh", iface); /*script path*/
	sprintf(deconfig_script, "/usr/share/udhcpc/%s.deconfig", iface);/*deconfig script path*/
	sprintf(pid_file, "/etc/udhcpc/udhcpc-%s.pid", iface); /*pid path*/
	Create_script(deconfig_script, iface, WAN_NETWORK, 0, 0, 0);
	memset(hostname, 0x00, 100);
	apmib_get( MIB_HOST_NAME, (void *)&hostname);
	
	if(hostname[0]){
		sprintf(cmdBuff, "udhcpc -i %s -p %s -s %s -h %s -a 30 &", iface, pid_file, script_file, hostname);
		//RunSystemCmd(NULL_FILE, "udhcpc", "-i", iface, "-p", pid_file, "-s", script_file,  "-a", "30", "-h", hostname,  NULL_STR);
	}else{
		sprintf(cmdBuff, "udhcpc -i %s -p %s -s %s -a 30 &", iface, pid_file, script_file);
		//RunSystemCmd(NULL_FILE, "udhcpc", "-i", iface, "-p", pid_file, "-s", script_file,  "-a", "30", NULL_STR);
	}
	system(cmdBuff);
}
void set_staticIP(int sys_op, char *wan_iface, char *lan_iface, int wisp_id, int act_source)
{
	int intValue=0;
	char tmpBuff[200];
	char tmp_args[16];
	char Ip[32], Mask[32], Gateway[32];
	
	apmib_get( MIB_WAN_IP_ADDR,  (void *)tmpBuff);
	sprintf(Ip, "%s", inet_ntoa(*((struct in_addr *)tmpBuff)));
	apmib_get( MIB_WAN_SUBNET_MASK,  (void *)tmpBuff);
	sprintf(Mask, "%s", inet_ntoa(*((struct in_addr *)tmpBuff)));
	apmib_get(MIB_WAN_DEFAULT_GATEWAY,  (void *)tmpBuff);
				
	if (!memcmp(tmpBuff, "\x0\x0\x0\x0", 4))
		memset(Gateway, 0x00, 32);
	else
		sprintf(Gateway, "%s", inet_ntoa(*((struct in_addr *)tmpBuff)));
			
	RunSystemCmd(NULL_FILE, "ifconfig", wan_iface, Ip, "netmask", Mask, NULL_STR);
		
	if(Gateway[0]){
		RunSystemCmd(NULL_FILE, "route", "del", "default", wan_iface, NULL_STR);
		RunSystemCmd(NULL_FILE, "route", "add", "-net", "default", "gw", Gateway, "dev", wan_iface, NULL_STR);
	}	
		apmib_get(MIB_FIXED_IP_MTU_SIZE, (void *)&intValue);
		sprintf(tmp_args, "%d", intValue);
		RunSystemCmd(NULL_FILE, "ifconfig", wan_iface, "mtu", tmp_args, NULL_STR);
		//RunSystemCmd(NULL_FILE, "killall", "-9", "dnrd", NULL_STR);
		start_dns_relay();
		start_upnp_igd(DHCP_DISABLED, sys_op, wisp_id, lan_iface);
		setFirewallIptablesRules(0, NULL);
		
		start_ntp();
		start_ddns();
		start_igmpproxy(wan_iface, lan_iface);
		del_routing();
		start_routing(wan_iface);
}
void set_dhcp_client(int sys_op, char *wan_iface, char *lan_iface, int wisp_id, int act_source)
{
	int intValue=0;
	char tmp_args[16];
	
	apmib_get(MIB_DHCP_MTU_SIZE, (void *)&intValue);
	sprintf(tmp_args, "%d", intValue);
	RunSystemCmd(NULL_FILE, "ifconfig", wan_iface, "mtu", tmp_args, NULL_STR);
	start_wan_dhcp_client(wan_iface);
	start_upnp_igd(DHCP_CLIENT, sys_op, wisp_id, lan_iface);
}
void set_pppoe(int sys_op, char *wan_iface, char *lan_iface, int wisp_id, int act_source)
{
	int intValue=0, cmdRet=-1;
	char line_buffer[100]={0};
	char tmp_args[64]={0};
	char tmp_args1[32]={0};
	int connect_type=0, idle_time=0;
	
	RunSystemCmd(NULL_FILE, "ifconfig", wan_iface, "0.0.0.0", NULL_STR);
	RunSystemCmd(NULL_FILE, "route", "del", "default", "gw", "0.0.0.0", NULL_STR);
	cmdRet = RunSystemCmd(NULL_FILE, "flash", "gen-pppoe", PPP_OPTIONS_FILE, PPP_PAP_FILE, PPP_CHAP_FILE,NULL_STR);
	
	if(cmdRet==0){
		sprintf(line_buffer,"%s\n", "noauth");
		write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
		sprintf(line_buffer,"%s\n", "noipdefault");
		write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
		sprintf(line_buffer,"%s\n", "hide-password");
		write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
		sprintf(line_buffer,"%s\n", "defaultroute");
		write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
		sprintf(line_buffer,"%s\n", "persist");
		write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
		sprintf(line_buffer,"%s\n", "ipcp-accept-remote");
		write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
		sprintf(line_buffer,"%s\n", "ipcp-accept-local");
		write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
		sprintf(line_buffer,"%s\n", "nodetach");
		write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
		sprintf(line_buffer,"%s\n", "usepeerdns");
		write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
		
		apmib_get(MIB_PPP_MTU_SIZE, (void *)&intValue);
		sprintf(line_buffer,"mtu %d\n", intValue);
		write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
		sprintf(line_buffer,"mru %d\n", intValue);
		write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
		sprintf(line_buffer,"%s\n", "lcp-echo-interval 20");
		write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
		sprintf(line_buffer,"%s\n", "lcp-echo-failure 3");
		write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
		sprintf(line_buffer,"%s\n", "wantype 3");
		write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
		sprintf(line_buffer,"%s\n", "holdoff 10");
		write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
		apmib_get( MIB_PPP_SERVICE,  (void *)tmp_args);
		if(tmp_args[0]){
			//sprintf(line_buffer,"plugin /etc/ppp/plubins/libplugin.a rp_pppoe_ac 62031090091393-Seednet_240_58 rp_pppoe_service %s %s\n",tmp_args, wan_iface);
			sprintf(line_buffer,"plugin /etc/ppp/plubins/libplugin.a rp_pppoe_service %s %s\n",tmp_args, wan_iface);
		}else{
			sprintf(line_buffer,"plugin /etc/ppp/plubins/libplugin.a %s\n", wan_iface);
		}
		write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
		
		apmib_get(MIB_PPP_CONNECT_TYPE, (void *)&connect_type);
		if(connect_type==1){
			apmib_get(MIB_PPP_IDLE_TIME, (void *)&idle_time);
			sprintf(line_buffer,"%s\n", "demand");
			write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
			sprintf(line_buffer,"idle %d\n", idle_time);
			write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
		}else if(connect_type==2 && act_source==1) //manual mode we do not dial up from init.sh
				return;
			
	#if 0
		apmib_get( MIB_WAN_DNS_MODE, (void *)&intValue1);
		if(intValue1==1){
			start_dns_relay();
		}else{
			RunSystemCmd(NULL_FILE, "dnrd", "--cache=off", "-s", "168.95.1.1",NULL_STR);
		}
	#endif
		if(isFileExist(PPP_FILE)){
			unlink(PPP_FILE);
		} 
		sprintf(tmp_args, "%s", "3");/*wan type*/
		sprintf(tmp_args1, "%d", connect_type);/*connect type*/
		RunSystemCmd(NULL_FILE, "ppp_inet", "-t", tmp_args,  "-c", tmp_args1, "-x", NULL_STR);
		start_upnp_igd(PPPOE, sys_op, wisp_id, lan_iface);
		
	}
}
void set_pptp(int sys_op, char *wan_iface, char *lan_iface, int wisp_id, int act_source)
{
	int intValue=0, intValue1=0, cmdRet=-1;
	char line_buffer[100]={0};
	char tmp_args[64]={0};
	char tmp_args1[32]={0};
	char Ip[32], Mask[32], ServerIp[32];
	int connect_type=0, idle_time=0;
	char *strtmp=NULL;
	
	apmib_get(MIB_PPTP_IP,  (void *)tmp_args);
	strtmp= inet_ntoa(*((struct in_addr *)tmp_args));
	sprintf(Ip, "%s", strtmp);
	
	apmib_get(MIB_PPTP_SUBNET_MASK,  (void *)tmp_args);
	strtmp= inet_ntoa(*((struct in_addr *)tmp_args));
	sprintf(Mask, "%s", strtmp);
	
	apmib_get(MIB_PPTP_SERVER_IP,  (void *)tmp_args);
	strtmp= inet_ntoa(*((struct in_addr *)tmp_args));
	sprintf(ServerIp, "%s", strtmp);
	
	RunSystemCmd(NULL_FILE, "ifconfig", wan_iface, "addr", Ip, "netmask", Mask, NULL_STR);
	RunSystemCmd(NULL_FILE, "route", "del", "default", "gw", "0.0.0.0", NULL_STR);
	cmdRet = RunSystemCmd(NULL_FILE, "flash", "gen-pptp", PPP_OPTIONS_FILE, PPP_PAP_FILE, PPP_CHAP_FILE,NULL_STR);
	
	if(cmdRet==0){
		sprintf(line_buffer,"%s\n", "lock");
		write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
		
		sprintf(line_buffer,"%s\n", "noauth");
		write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
		
		sprintf(line_buffer,"%s\n", "nobsdcomp");
		write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
		
		sprintf(line_buffer,"%s\n", "nodeflate");
		write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
		
		sprintf(line_buffer,"%s\n", "usepeerdns");
		write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
		
		sprintf(line_buffer,"%s\n", "lcp-echo-interval 20");
		write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
		
		sprintf(line_buffer,"%s\n", "lcp-echo-failure 3");
		write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
		
		sprintf(line_buffer,"%s\n", "wantype 4");
		write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
		
		apmib_get(MIB_PPTP_MTU_SIZE, (void *)&intValue);
		sprintf(line_buffer,"mtu %d\n", intValue);
		write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
		
		sprintf(line_buffer,"%s\n", "holdoff 2");
		write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
		
		
		
		sprintf(line_buffer,"%s\n", "remotename PPTP");
		write_line_to_file(PPTP_PEERS_FILE,1, line_buffer);
		
		sprintf(line_buffer,"%s\n", "linkname PPTP");
		write_line_to_file(PPTP_PEERS_FILE,2, line_buffer);
		
		sprintf(line_buffer,"%s\n", "ipparam PPTP");
		write_line_to_file(PPTP_PEERS_FILE,2, line_buffer);
		
		sprintf(tmp_args, "pty \"pptp %s --nolaunchpppd\"", ServerIp);
		sprintf(line_buffer,"%s\n", tmp_args);
		write_line_to_file(PPTP_PEERS_FILE,2, line_buffer);
		
		apmib_get( MIB_PPTP_USER,  (void *)tmp_args);
		sprintf(line_buffer,"name %s\n", tmp_args);
		write_line_to_file(PPTP_PEERS_FILE,2, line_buffer);
		
		apmib_get( MIB_PPTP_SECURITY_ENABLED, (void *)&intValue);
		if(intValue==1){
			sprintf(line_buffer,"%s\n", "+mppe required,stateless");
			write_line_to_file(PPTP_PEERS_FILE,2, line_buffer);
			
			//sprintf(line_buffer,"%s\n", "+mppe no128,stateless");/*disable 128bit encrypt*/
			//write_line_to_file(PPTP_PEERS_FILE,2, line_buffer);
			//sprintf(line_buffer,"%s\n", "+mppe no56,stateless");/*disable 56bit encrypt*/
			//write_line_to_file(PPTP_PEERS_FILE,2, line_buffer);
			
		}
		apmib_get( MIB_PPTP_MPPC_ENABLED, (void *)&intValue1);
		if(intValue1==1){
			sprintf(line_buffer,"%s\n", "mppc");
			write_line_to_file(PPTP_PEERS_FILE,2, line_buffer);
			sprintf(line_buffer,"%s\n", "stateless");
			write_line_to_file(PPTP_PEERS_FILE,2, line_buffer);
		}else{
			sprintf(line_buffer,"%s\n", "nomppc");
			write_line_to_file(PPTP_PEERS_FILE,2, line_buffer);
		}
		if(intValue ==0 && intValue1==0){
			sprintf(line_buffer,"%s\n", "noccp");
			write_line_to_file(PPTP_PEERS_FILE,2, line_buffer);
		}
		
		sprintf(line_buffer,"%s\n", "persist");
		write_line_to_file(PPTP_PEERS_FILE,2, line_buffer);
		
		sprintf(line_buffer,"%s\n", "noauth");
		write_line_to_file(PPTP_PEERS_FILE,2, line_buffer);
		
		sprintf(line_buffer,"%s\n", "file /etc/ppp/options");
		write_line_to_file(PPTP_PEERS_FILE,2, line_buffer);
		
		sprintf(line_buffer,"%s\n", "nobsdcomp");
		write_line_to_file(PPTP_PEERS_FILE,2, line_buffer);
		
		sprintf(line_buffer,"%s\n", "nodetach");
		write_line_to_file(PPTP_PEERS_FILE,2, line_buffer);
		
		sprintf(line_buffer,"%s\n", "novj");
		write_line_to_file(PPTP_PEERS_FILE,2, line_buffer);
		
		
		apmib_get(MIB_PPTP_CONNECTION_TYPE, (void *)&connect_type);
		if(connect_type==1){
			
			sprintf(line_buffer,"%s\n", "persist");
			write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
			
			sprintf(line_buffer,"%s\n", "nodetach");
			write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
			
			sprintf(line_buffer,"%s\n", "connect /etc/ppp/true");
			write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
			
			sprintf(line_buffer,"%s\n", "demand");
			write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
			
			apmib_get(MIB_PPTP_IDLE_TIME, (void *)&idle_time);
			sprintf(line_buffer,"idle %d\n", idle_time);
			write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
			
			sprintf(line_buffer,"%s\n", "ktune");
			write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
			
			sprintf(line_buffer,"%s\n", "ipcp-accept-remote");
			write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
			
			sprintf(line_buffer,"%s\n", "ipcp-accept-local");
			write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
			
			sprintf(line_buffer,"%s\n", "noipdefault");
			write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
			
			sprintf(line_buffer,"%s\n", "hide-password");
			write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
			
			sprintf(line_buffer,"%s\n", "defaultroute");
			write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
		}else if(connect_type==2 && act_source==1) //manual mode we do not dial up from init.sh
				return;
			
	#if 0	
		apmib_get( MIB_WAN_DNS_MODE, (void *)&intValue1);
		if(intValue1==1){
			start_dns_relay();
		}else{
			RunSystemCmd(NULL_FILE, "dnrd", "--cache=off", "-s", "168.95.1.1",NULL_STR);
		}
	#endif	
		if(isFileExist(PPP_FILE)){
			unlink(PPP_FILE);
		} 
		sprintf(tmp_args, "%s", "4");/*wan type*/
		sprintf(tmp_args1, "%d", connect_type);/*connect type*/
		RunSystemCmd(NULL_FILE, "ppp_inet", "-t", tmp_args,  "-c", tmp_args1, "-x", NULL_STR);
	}
	start_upnp_igd(PPTP, sys_op, wisp_id, lan_iface);
}

void set_l2tp(int sys_op, char *wan_iface, char *lan_iface, int wisp_id, int act_source)
{
	int intValue=0;
	char line_buffer[100]={0};
	char tmp_args[64]={0};
	char tmp_args1[32]={0};
	char Ip[32], Mask[32], ServerIp[32];
	int connect_type=0, idle_time=0;
	char *strtmp=NULL;
	int pwd_len=0;
	apmib_get(MIB_L2TP_IP,  (void *)tmp_args);
	strtmp= inet_ntoa(*((struct in_addr *)tmp_args));
	sprintf(Ip, "%s", strtmp);
	
	apmib_get(MIB_L2TP_SUBNET_MASK,  (void *)tmp_args);
	strtmp= inet_ntoa(*((struct in_addr *)tmp_args));
	sprintf(Mask, "%s", strtmp);
	
	apmib_get(MIB_L2TP_SERVER_IP,  (void *)tmp_args);
	strtmp= inet_ntoa(*((struct in_addr *)tmp_args));
	sprintf(ServerIp, "%s", strtmp);
	
	RunSystemCmd(NULL_FILE, "ifconfig", wan_iface, "addr", Ip, "netmask", Mask, NULL_STR);
	RunSystemCmd(NULL_FILE, "route", "del", "default", "gw", "0.0.0.0", NULL_STR);
	
	apmib_get( MIB_L2TP_USER,  (void *)tmp_args);
	apmib_get( MIB_L2TP_PASSWORD,  (void *)tmp_args1);
	pwd_len = strlen(tmp_args1);
	/*options file*/
	sprintf(line_buffer,"user \"%s\"\n",tmp_args);
	write_line_to_file(PPP_OPTIONS_FILE, 1, line_buffer);
	
	/*secrets files*/
	sprintf(line_buffer,"%s\n","#################################################");
	write_line_to_file(PPP_PAP_FILE, 1, line_buffer);
	
	sprintf(line_buffer, "\"%s\"	*	\"%s\"\n",tmp_args, tmp_args1);
	write_line_to_file(PPP_PAP_FILE, 2, line_buffer);
	
	sprintf(line_buffer,"%s\n","#################################################");
	write_line_to_file(PPP_CHAP_FILE, 1, line_buffer);
	
	sprintf(line_buffer, "\"%s\"	*	\"%s\"\n",tmp_args, tmp_args1);
	write_line_to_file(PPP_CHAP_FILE, 2, line_buffer);
	
	sprintf(line_buffer,"%s\n", "lock");
	write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
	sprintf(line_buffer,"%s\n", "noauth");
	write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
	sprintf(line_buffer,"%s\n", "defaultroute");
	write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
	sprintf(line_buffer,"%s\n", "usepeerdns");
	write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
	sprintf(line_buffer,"%s\n", "lcp-echo-interval 0");
	write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
	sprintf(line_buffer,"%s\n", "wantype 6");
	write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
	
	sprintf(line_buffer,"%s\n", "holdoff 2");
	write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
	
	apmib_get(MIB_L2TP_MTU_SIZE, (void *)&intValue);
	sprintf(line_buffer,"mtu %d\n", intValue);
	write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
	
	apmib_get( MIB_L2TP_USER,  (void *)tmp_args);
	sprintf(line_buffer,"name %s\n", tmp_args);
	write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
	
	sprintf(line_buffer,"%s\n", "noauth");
	write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
	
	sprintf(line_buffer,"%s\n", "nodeflate");
	write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
	
	sprintf(line_buffer,"%s\n", "nobsdcomp");
	write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
	
	sprintf(line_buffer,"%s\n", "nodetach");
	write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
	
	sprintf(line_buffer,"%s\n", "novj");
	write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
	
	sprintf(line_buffer,"%s\n", "default-asyncmap");
	write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
	
	sprintf(line_buffer,"%s\n", "nopcomp");
	write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
	
	sprintf(line_buffer,"%s\n", "noaccomp");
	write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
	
	sprintf(line_buffer,"%s\n", "noccp");
	write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
	
	sprintf(line_buffer,"%s\n", "novj");
	write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
	
	if(pwd_len > 35){
		sprintf(line_buffer,"%s\n", "-mschap");
		write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
		
		sprintf(line_buffer,"%s\n", "-mschap-v2");
		write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
	}
	
	sprintf(line_buffer,"%s\n", "[global]");
	write_line_to_file(L2TPCONF,1, line_buffer);
	
	sprintf(line_buffer,"%s\n", "port = 1701");
	write_line_to_file(L2TPCONF,2, line_buffer);
	
	sprintf(line_buffer,"auth file = %s\n", PPP_CHAP_FILE);
	write_line_to_file(L2TPCONF,2, line_buffer);
	
	sprintf(line_buffer,"%s\n", "[lac client]");
	write_line_to_file(L2TPCONF,2, line_buffer);
	
	sprintf(line_buffer,"lns=%s\n", ServerIp);
	write_line_to_file(L2TPCONF,2, line_buffer);

	sprintf(line_buffer,"%s\n", "require chap = yes");
	write_line_to_file(L2TPCONF,2, line_buffer);
	
	apmib_get( MIB_L2TP_USER,  (void *)tmp_args);
	sprintf(line_buffer,"name = %s\n", tmp_args);
	write_line_to_file(L2TPCONF,2, line_buffer);
	
	sprintf(line_buffer,"%s\n", "pppoptfile = /etc/ppp/options");
	write_line_to_file(L2TPCONF, 2, line_buffer);
	
	apmib_get(MIB_L2TP_CONNECTION_TYPE, (void *)&connect_type);
	
	//RunSystemCmd(NULL_FILE, "l2tpd", NULL_STR);	
	sprintf(tmp_args, "l2tpd -m %d", connect_type);
	system(tmp_args);
	
	
	if(connect_type==1){
			
		sprintf(line_buffer,"%s\n", "connect /etc/ppp/true");
		write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
			
		sprintf(line_buffer,"%s\n", "demand");
		write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
			
		apmib_get(MIB_L2TP_IDLE_TIME, (void *)&idle_time);
		sprintf(line_buffer,"idle %d\n", idle_time);
		write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
			
		}else if(connect_type==2 && act_source==1) //manual mode we do not dial up from init.sh
				return;
			
	#if 0	
		apmib_get( MIB_WAN_DNS_MODE, (void *)&intValue1);
		if(intValue1==1){
			start_dns_relay();
		}else{
			RunSystemCmd(NULL_FILE, "dnrd", "--cache=off", "-s", "168.95.1.1",NULL_STR);
		}
	#endif	
		if(isFileExist(PPP_FILE)){
			unlink(PPP_FILE);
		} 
		sprintf(tmp_args, "%s", "6");/*wan type*/
		sprintf(tmp_args1, "%d", connect_type);/*connect type*/
		RunSystemCmd(NULL_FILE, "ppp_inet", "-t", tmp_args,  "-c", tmp_args1, "-x", NULL_STR);
		start_upnp_igd(L2TP, sys_op, wisp_id, lan_iface);
}
int start_wan(int wan_mode, int sys_op, char *wan_iface, char *lan_iface, int wisp_id, int act_source)
{

	printf("Init WAN Interface...\n");
	if(wan_mode == DHCP_DISABLED)
		set_staticIP(sys_op, wan_iface, lan_iface, wisp_id, act_source);
	else if(wan_mode == DHCP_CLIENT)
		set_dhcp_client(sys_op, wan_iface, lan_iface, wisp_id, act_source);
	else if(wan_mode == PPPOE){
		//RunSystemCmd(NULL_FILE, "pppoe.sh", "all", wan_iface, NULL_STR);
		set_pppoe(sys_op, wan_iface, lan_iface, wisp_id, act_source);
	}else if(wan_mode == PPTP){
		set_pptp(sys_op, wan_iface, lan_iface, wisp_id, act_source);
		//RunSystemCmd(NULL_FILE, "pptp.sh", wan_iface, NULL_STR);
	}else if(wan_mode == L2TP){
		//RunSystemCmd(NULL_FILE, "l2tp.sh", wan_iface, NULL_STR);
		set_l2tp(sys_op, wan_iface, lan_iface, wisp_id, act_source);
	}	
	return 0;
}

 
 
 
 
 
 
 
 
 
 
 
