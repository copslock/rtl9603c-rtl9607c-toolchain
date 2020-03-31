/* 
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "sysconf.h"
#include "sys_utility.h"

static int isDaemon=0;
static int WanType=0;
static int ConnectType=0;

int main(int argc, char *argv[])
{
	int i;
	int re_try=0;
	
	for(i=1; i<argc; i++)
	{
		if(argv[i][0]!='-')
		{
			fprintf(stderr, "%s: Unknown option\n", argv[i]);
		}
		else switch(argv[i][1])
		{
		case 'c':
			ConnectType = atoi(argv[++i]);
			break;	
		case 't':
			WanType = atoi(argv[++i]);
			break;
		case 'x':
			isDaemon = 1;
			break;
		default:
			fprintf(stderr, "%s: Unknown option\n", argv[i]);
		}
	}
	if(isDaemon==1){
		if (daemon(0, 1) == -1) {
			perror("ppp_inet fork error");
			return 0;
		}
	}
	
	
	
	for (;;) {
		if(isFileExist(PPP_CONNECT_FILE)==0){
			
			if(WanType==3){
				/* Make sure old pppd is finishd, then run a new pppd */ 				
				while(find_pid_by_name("pppd") > 0)
				{
					printf("\r\n pppd is still alive.__[%s-%u]",__FILE__,__LINE__);
					//system("ps");
					sleep(2);
				}
				//system("ifconfig");

				RunSystemCmd(PPP_CONNECT_FILE, "echo", "pass", NULL_STR);
				system("pppd &");														
			}
			
			if(WanType==4){
				while(find_pid_by_name("pppd") > 0)
				{
					printf("\r\n pppd is still alive.__[%s-%u]",__FILE__,__LINE__);
					//system("ps");
					sleep(2);
				}
					RunSystemCmd(PPP_CONNECT_FILE, "echo", "pass", NULL_STR);
					system("pppd call rpptp &");
			}
			
			if(WanType==6){
				if(isFileExist("/var/disc")==0){
						usleep(1200000); //wait l2tpd init finish
					RunSystemCmd(PPP_CONNECT_FILE, "echo", "pass", NULL_STR);
					system("echo \"c client\" > /var/run/l2tp-control &");
				}
			}
			
		}else{
				if(WanType==6){
					if(isFileExist(PPPD_PID_FILE)==0){
				  		unlink(PPP_CONNECT_FILE); /*force start pppd*/
		  			}
	  		}
  		}
  		
		if(ConnectType==2) 
			break;
		sleep(5);
	}
	return 0;
}



