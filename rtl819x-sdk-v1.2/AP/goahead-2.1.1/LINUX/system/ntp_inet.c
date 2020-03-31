/* 
 */
 
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "sysconf.h"
#include "sys_utility.h"


#define NTPTMP_FILE "/tmp/ntp_tmp"
#define TZ_FILE "/etc/TZ"

static int isDaemon=0;



int main(int argc, char *argv[])
{
	int i;
	unsigned char	ntp_server[40];
	unsigned char command[100];
	unsigned short fail_wait_time = 300;
	unsigned int succ_wait_time = 86400;
	unsigned char daylight_save_str[5];

	for(i=1; i<argc; i++)
	{
		if(argv[i][0]!='-')
		{
			fprintf(stderr, "%s: Unknown option\n", argv[i]);
		}
		else 
			switch(argv[i][1])
			{
				case 'x':
					isDaemon = 1;
					break;
				
				default:
					fprintf(stderr, "%s: Unknown option\n", argv[i]);
			}
	}

	sprintf(ntp_server, "%s", argv[2]);
	sprintf(command, "%s", argv[3]);
	sprintf(daylight_save_str, "%s", argv[4]);
	
	if(isDaemon==1){
		if (daemon(0, 1) == -1) {
			perror("ntp_inet fork error");
			return 0;
		}
	}
	
	
	for (;;) {
		int ret=1;
		unsigned char cmdBuffer[100];
		
		RunSystemCmd(NULL_FILE, "rm", "/tmp/ntp_tmp", NULL_STR);
		RunSystemCmd(NULL_FILE, "rm", "/var/TZ", NULL_STR);

		ret = RunSystemCmd(NTPTMP_FILE, "ntpclient", "-s", "-h", ntp_server, "-i", "5", ">", NULL_STR);
		
		if(ret == 0 && isFileExist(NTPTMP_FILE))
		{
			FILE *fp=NULL;	
			unsigned char ntptmp_str[100];
			memset(ntptmp_str,0x00,sizeof(ntptmp_str));
			
			fp=fopen(NTPTMP_FILE, "r");
			if(fp!=NULL){
				fgets(ntptmp_str,sizeof(ntptmp_str),fp);
				fclose(fp);

				if(strlen(ntptmp_str) != 0)
				{
					
					// success

					RunSystemCmd(TZ_FILE, "echo", command, NULL_STR);
					if(strcmp(daylight_save_str, "1") == 0)
					{
						sprintf(cmdBuffer,"date > %s",  NTPTMP_FILE);
						system(cmdBuffer);
						//RunSystemCmd(NTPTMP_FILE, "date", ">", NULL_STR);
					}
					RunSystemCmd(NULL_FILE, "echo", "ntp client success", NULL_STR);
					sleep(succ_wait_time);
					
				}
				else
				{
					//RunSystemCmd(NULL_FILE, "echo", "ntp client fail", NULL_STR);
					sleep(5);
				}									
			}
			else
			{
				RunSystemCmd(NULL_FILE, "echo", "Can't connect ntp server!!", NULL_STR);
				sleep(fail_wait_time);
			}
		}
		else
		{
			//RunSystemCmd(NULL_FILE, "echo", "Can't create ntp tmp file!!", NULL_STR);
			sleep(fail_wait_time);
		}
		
	}
	
	
	return 0;
}



