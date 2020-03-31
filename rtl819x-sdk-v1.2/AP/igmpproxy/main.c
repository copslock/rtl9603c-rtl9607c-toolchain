#include "mclab.h"
#include "timeout.h"
#include <fcntl.h>
#include <signal.h>

#include "built_time"
#define VERSION_STR	"v1.2"

#define USE_STATIC_ENTRY_BUFFER 1
#define MAX_MFCT_ENTRY 128
#define MAX_MBR_ENTRY 512

#define DISPLAY_BANNER \
	printf("\nIGMP Proxy %s (%s).\n\n", VERSION_STR, BUILT_TIME)
	
// Enable to do the group-specific query periodically
#define PERIODICAL_QUERY
// Enable to maintain the group members in order to do immediately leave
#define KEEP_GROUP_MEMBER
#define CONFIG_CHECK_MULTICASTROUTE
/* IGMP timer and default values */
#define LAST_MEMBER_QUERY_INTERVAL	1	// second
#define LAST_MEMBER_QUERY_COUNT		2
// Kaohj --- group-specific query in periodical
#ifdef PERIODICAL_QUERY
#define MEMBER_QUERY_INTERVAL		20	// second
#define MEMBER_QUERY_COUNT		3
#endif

/* IGMP group address */
#define ALL_SYSTEMS		htonl(0xE0000001)	// General Query - 224.0.0.1
#define ALL_ROUTERS		htonl(0xE0000002)	// Leave - 224.0.0.2	
#define ALL_ROUTERS_V3	htonl(0xE0000016)	// Leave - 224.0.0.22
#define ALL_PRINTER		htonl(0xEFFFFFFA)	// notify all printer - 239.255.255.250
#define CLASS_D_MASK	0xE0000000		// the mask that defines IP Class D
#define IPMULTI_MASK	0x007FFFFF		// to get the low-order 23 bits

/* header length */
#define MIN_IP_HEADER_LEN	20
#define IGMP_MINLEN			8

/* IGMP v3 type */
#define IGMP_HOST_V3_MEMBERSHIP_REPORT	0x22
#ifdef CONFIG_CHECK_MULTICASTROUTE
/* Keep this in sync with /usr/src/linux/include/linux/route.h */
#define RTF_UP			0x0001          /* route usable                 */
#define RTF_GATEWAY		0x0002          /* destination is a gateway     */
#define	RTF_HOST	0x0004		/* host entry (net otherwise)	*/
#endif
static char *routefile = "/proc/net/route";
int igmp_query(__u32 dst, __u32 grp,__u8 mrt);

#if !defined(CONFIG_RTL8186_EC) && !defined(CONFIG_RTL8186_TR) && !defined(CONFIG_RTL865X_AC) && !defined(CONFIG_RTL865X_KLD)
#define IGMP_MCAST2UNI
#endif
#ifdef	IGMP_MCAST2UNI
static void Update_igmpProxyStateToKernel(int igmpProxyEnable)
{
	//printf("\nUpdate_igmpProxyStateToKernel()\n");
	char *br_igmpProxy_Proc = "/proc/br_igmpProxy";
	char valueBeWirte[2]="01";
	FILE *fp = fopen(br_igmpProxy_Proc, "r+");
	int success;
	if (!fp) {
		printf("igmpProxy-error:Open %s file error.\n", br_igmpProxy_Proc);
		return;
	}

	if(igmpProxyEnable==1){
		success = fputc('1', fp);
	}else{
		success = fputc('0', fp);				
	}
	if(success==EOF) 
		printf("igmpProxy-error:Update_igmpProxyState fail.\n");
	
	fclose(fp);
	
}
#endif

#if 0
static void pkt_debug(const char *buf)
{
int num2print = 20;
int i = 0;
	if(buf[0]==0x46)
		num2print = 24;
	for (i = 0; i < num2print; i++) {
		printf("%2.2x ", 0xff & buf[i]);
	}
	printf("\n");
	num2print = buf[3];
	for (; i < num2print; i++) {
		printf("%2.2x ", 0xff & buf[i]);
	}
	printf("\n");
}
#else
#define pkt_debug(buf)	do {} while (0)
#endif


static int  igmp_id = 0;

char igmp_down_if_name[IFNAMSIZ];
char igmp_down_if_idx;
char igmp_up_if_name[IFNAMSIZ];
char igmp_up_if_idx;

struct igmpv3hdr {
	__u8 type;
	__u8 code;		/* For newer IGMP */
	__u16 csum;
	__u16 reserved;
	__u16 num_grp;
	__u8 rcd_type;
	__u8 aux_len;
	__u16 num_src;
	__u32 ip[4];
};

// group member entry
struct mbr_entry {
	struct mbr_entry	*next;
	__u32			user_addr;
};

struct igmp_timer {
	int		type;			// timer type
	int		retry_left;		// retry counts left
	struct	callout	ch;
};


struct mcft_entry {
	struct mcft_entry *next;
	__u32	grp_addr;
	// group member count
	__u32	user_count;
	struct mbr_entry *grp_mbr;
	struct igmp_timer	timer;
};
#ifdef USE_STATIC_ENTRY_BUFFER
struct mcft_entry_en {
	int valid;
	struct mcft_entry entry_mcft_;
};

struct mbr_entry_en {
	int valid;
	struct mbr_entry entry_mbr_;
};
struct mcft_entry_en mcft_entry_tbl[MAX_MFCT_ENTRY];
struct mbr_entry_en mbr_entry_tbl[MAX_MBR_ENTRY];
#endif

#ifdef CONFIG_CHECK_MULTICASTROUTE
static int check_kernel_multicast_route(int entry)
{
	char buff[1024], iface[16];
	char net_addr[128], gate_addr[128], mask_addr[128];
	int num, iflags, refcnt, use, metric, mss, window, irtt;
	FILE *fp = fopen(routefile, "r");
	char *fmt;
	int found = 0;

	if (!fp) {
		printf("Open %s file error.\n", routefile);
		return;
	}

	fmt = "%16s %128s %128s %X %d %d %d %128s %d %d %d";

	while (fgets(buff, 1023, fp)) {
		num = sscanf(buff, fmt, iface, net_addr, gate_addr,
			&iflags, &refcnt, &use, &metric, mask_addr, &mss, &window, &irtt);
		if(entry ==1){	
			if (num < 10 || (iflags !=0x1) || strcmp(iface, "br0")){
				continue;
			}
		}else if(entry == 2){
			if (num < 10 || (iflags != 0x5)  || strcmp(iface, "br0")){
			continue;
			}
		}
		if(entry ==1){	
		if (!strcmp(net_addr, "E0000000")) {
			found = 1;
			break;
		}
	}
		if(entry ==2){	
			 if(!strcmp(net_addr, "FFFFFFFF")) {
				found = 2;
				break;
			}
		}
	}

	fclose(fp);
	return found;
}
#endif



struct mcft_entry *mcpq = NULL;

#ifdef USE_STATIC_ENTRY_BUFFER
struct mcft_entry * find_mcft_entry_from_tbl(void)
{
	int i;
	struct mcft_entry_en *valid_entry;
	
	for(i=0;i<MAX_MFCT_ENTRY;i++){
		valid_entry = &mcft_entry_tbl[i];
		if(valid_entry->valid==0){
			valid_entry->valid=1;
			return (&(valid_entry->entry_mcft_));
		}
	}
	//printf("find_mcft_entry_from_tbl fail\n");
	return 0;
}

int del_mcft_entry_from_tbl(struct mcft_entry *del_mcft_entry)
{
	
	int i;
	struct mcft_entry_en *valid_entry;
	struct mcft_entry *check_entry;
	for(i=0;i<MAX_MFCT_ENTRY;i++){
		valid_entry = &mcft_entry_tbl[i];
		check_entry = &(valid_entry->entry_mcft_);
		if(&(valid_entry->entry_mcft_)==del_mcft_entry){
			//printf("delmcft entry:group=%08X\n",check_entry->grp_addr);
			valid_entry->valid=0;
			return 1;
		}
	}
	//printf("del_mcft_entry_from_tbl fail\n");
	return 0;
	
}


struct mbr_entry * find_mbr_entry_from_tbl(void)
{
	int i;
	struct mbr_entry_en *valid_entry;
	
	for(i=0;i<MAX_MBR_ENTRY;i++){
		valid_entry = &mbr_entry_tbl[i];
		if(valid_entry->valid==0){
			valid_entry->valid=1;
			return (&(valid_entry->entry_mbr_));
		}
	}
	//printf("find_mbr_entry_from_tbl fail\n");
	return 0;
}

int del_mbr_entry_from_tbl(struct mbr_entry *del_mbr_entry)
{
	int i;
	struct mbr_entry_en *valid_entry;
	struct mbr_entry *check_entry;
	for(i=0;i<MAX_MBR_ENTRY;i++){
		valid_entry = &mbr_entry_tbl[i];
		check_entry = &(valid_entry->entry_mbr_);
		if(&(valid_entry->entry_mbr_)==del_mbr_entry){
			//printf("del mbr entry:user_addr=%08X\n",check_entry->user_addr);
			valid_entry->valid=0;
			return 1;
		}
	}
	//printf("del_mbr_entry_from_tbl fail\n");
	return 0;
}

#endif
struct mcft_entry * add_mcft(__u32 grp_addr, __u32 src_addr)
{
struct mcft_entry *mcp;
#ifdef KEEP_GROUP_MEMBER
	struct mbr_entry *gcp;
#endif
#ifndef USE_STATIC_ENTRY_BUFFER 

	mcp = malloc(sizeof(struct mcft_entry));
	if(!mcp)
		return 0;
#ifdef KEEP_GROUP_MEMBER
	gcp = malloc(sizeof(struct mbr_entry));
	if (!gcp) {
		free(mcp);
		return 0;
	}
#endif

#else//static buffer
	mcp =find_mcft_entry_from_tbl();
	if(!mcp){
		return 0;
	}
#ifdef KEEP_GROUP_MEMBER
	gcp = find_mbr_entry_from_tbl();
	if (!gcp) {
		return 0;
	}
#endif

#endif
	mcp->grp_addr = grp_addr;
	// Kaohj -- add the first member
#ifdef KEEP_GROUP_MEMBER
	mcp->user_count = 1;
	gcp->user_addr = src_addr;
	gcp->next = NULL;
	mcp->grp_mbr = gcp;
#endif
	mcp->next = mcpq;
	mcpq = mcp;
	//return 0;
	return mcp;
}

int del_mcft(__u32 grp_addr)
{
struct mcft_entry **q, *p;
#ifdef KEEP_GROUP_MEMBER
	struct mbr_entry *gt, *gc;
#endif


	/* Remove the entry from the  list. */
	for (q = &mcpq; (p = *q); q = &p->next) {
		if(p->grp_addr == grp_addr) {
			*q = p->next;
			// Kaohj -- free member list
#ifndef USE_STATIC_ENTRY_BUFFER 			
#ifdef KEEP_GROUP_MEMBER
			gc = p->grp_mbr;
			while (gc) {
				gt = gc->next;
				free(gc);
				gc = gt;
			}
#endif
			free(p);
			return 0;
#else//static buffer

#ifdef KEEP_GROUP_MEMBER
			gc = p->grp_mbr;
			while (gc) {
				gt = gc->next;
				//printf("del mcft user=%08X\n",gc->user_addr);
				del_mbr_entry_from_tbl(gc);
				gc = gt;
			}
#endif
			//printf("del mcft group=%08X\n",p->grp_addr);
			del_mcft_entry_from_tbl(p);
			
			return 0;
#endif			
		}
	}
	return -1;
}

int chk_mcft(__u32 grp_addr)
{
struct mcft_entry *mcp = mcpq;
	while(mcp) {
		if(mcp->grp_addr == grp_addr)
			return 1;
		mcp = mcp->next;
	}
	return 0;
}

struct mcft_entry * get_mcft(__u32 grp_addr)
{
struct mcft_entry *mcp = mcpq;
	while(mcp) {
		if(mcp->grp_addr == grp_addr)
			return mcp;
		mcp = mcp->next;
	}
	return NULL;
}

int num_mcft(void)
{
struct mcft_entry *mcp = mcpq;
int n = 0;
	while(mcp) {
		n++;
		mcp = mcp->next;
	}
	return n;
}

#ifdef KEEP_GROUP_MEMBER
// Kaohj -- attach user to group member list
//	0: fail
//	1: duplicate user
//	2: added successfully
int add_user(struct mcft_entry *mcp, __u32 src)
{
	struct mbr_entry *gcp;

	// check user
	gcp = mcp->grp_mbr;
	while (gcp) {
		if (gcp->user_addr == src)
			return 1;	// user exists
		gcp = gcp->next;
	}
#ifndef USE_STATIC_ENTRY_BUFFER 	
	// add user
	gcp = malloc(sizeof(struct mbr_entry));
#else
	gcp = find_mbr_entry_from_tbl();
#endif	
	if (!gcp) {
		return 0;
	}
	gcp->user_addr = src;
	gcp->next = mcp->grp_mbr;
	mcp->grp_mbr = gcp;
	mcp->user_count++;
}

// Kaohj -- remove user from group member list
// return: user count
int del_user(struct mcft_entry *mcp, __u32 src)
{
	struct mbr_entry **q, *p;
	
	/* Remove the entry from the  list. */
	q = &mcp->grp_mbr;
	p = *q;
	while (p) {
		if(p->user_addr == src) {
			*q = p->next;
#ifndef USE_STATIC_ENTRY_BUFFER 				
			free(p);
#else
			del_mbr_entry_from_tbl(p);
#endif			
			mcp->user_count--;
			return mcp->user_count;
		}
		q = &p->next;
		p = p->next;
	}
	
	return mcp->user_count;
}
#endif

/*
 * u_short in_cksum(u_short *addr, int len)
 *
 * Compute the inet checksum
 */
unsigned short in_cksum(unsigned short *addr, int len)
{
    int nleft = len;
    int sum = 0;
    unsigned short *w = addr;
    unsigned short answer = 0;

    while (nleft > 1) {
        sum += *w++;
        nleft -= 2;
    }
    if (nleft == 1) {
        *(unsigned char*)(&answer) = *(unsigned char*)w;
        sum += answer;
    }
    sum = (sum >> 16) + (sum & 0xffff);
    answer = ~sum;
    return (answer);
}

fd_set in_fds;		/* set of fds that wait_input waits for */
int max_in_fd;		/* highest fd set in in_fds */

/*
 * add_fd - add an fd to the set that wait_input waits for.
 */
void add_fd(int fd)
{
    FD_SET(fd, &in_fds);
    if (fd > max_in_fd)
	max_in_fd = fd;
}

/*
 * remove_fd - remove an fd from the set that wait_input waits for.
 */
void remove_fd(int fd)
{
    FD_CLR(fd, &in_fds);
}

/////////////////////////////////////////////////////////////////////////////
//	22/04/2004, Casey
/*
	Modified the following items:
	1.	delete all muticast router functions, xDSL router never use such function
	2.	igmp_handler only accept message for IGMP PROXY
	3.	IGMP proxy keep track on multicast address by mcft table, 
		not multicast router module.

	igmp_handler rule:
	1.	only accept IGMP query from upstream interface, and it trigger
		downstream interface to send IGMP query.
	2.	only accept IGMP report from downstream interface, and it trigger
		upstream interface to send IGMP report.
	3.	when received IGMP report, recorded its group address as forwarding rule.
	4.	only accept IGMP leave from downstream interface, downstream interface
		will send IGMP general query twice to make sure there is no other member.
		If it cannot find any member, upstream interface will send IGMP leave.
		
	forwarding rule:
	1.	system only forward multicast packets from upstream interface to downstream interface.
	2.	system only forward multicast packets which group address learned by IGMP report.
	
*/
/////////////////////////////////////////////////////////////////////////////
//



#define RECV_BUF_SIZE	2048
char *recv_buf, *send_buf;
int igmp_socket;	/* down */
int igmp_socket2;	/* up */

int igmp_inf_create(char *ifname)
{
	struct ip_mreq mreq;
	int i;
	int ret;
	struct IfDesc *dp;


	dp = getIfByName(ifname);
	
    if ((dp->sock = socket(AF_INET, SOCK_RAW, IPPROTO_IGMP)) < 0) 
		log(LOG_ERR, errno, "IGMP socket");

	/* init igmp */	
	/* Set reuseaddr, ttl, loopback and set outgoing interface */
	i = 1;
	ret = setsockopt(dp->sock, SOL_SOCKET, SO_REUSEADDR, (void*)&i, sizeof(i));
	if(ret)
		printf("setsockopt SO_REUSEADDR error!\n");
	i = 1;
	ret = setsockopt(dp->sock, IPPROTO_IP, IP_MULTICAST_TTL, 
		(void*)&i, sizeof(i));
	if(ret)
		printf("setsockopt IP_MULTICAST_TTL error!\n");
	//eddie disable LOOP
	i = 0;
	ret = setsockopt(dp->sock, IPPROTO_IP, IP_MULTICAST_LOOP, 
		(void*)&i, sizeof(i));
	if(ret)
		printf("setsockopt IP_MULTICAST_LOOP error!\n");
	ret = setsockopt(dp->sock, IPPROTO_IP, IP_MULTICAST_IF, 
		(void*)&dp->InAdr, sizeof(struct in_addr));
	if(ret)
		printf("setsockopt IP_MULTICAST_IF error!\n");

	/* In linux use IP_PKTINFO */
	//IP_RECVIF returns the interface of received datagram
	i = 1;
	ret = setsockopt(dp->sock, IPPROTO_IP, IP_PKTINFO, &i, sizeof(i));
	if(ret)
		printf("setsockopt IP_PKTINFO error!\n");

	//ret = fcntl(dp->sock, F_SETFL, O_NONBLOCK);
	//if(ret)
	//	printf("fcntl O_NONBLOCK error!\n");

	return 0;
	
}

int init_igmp(void)
{
int val;
    recv_buf = malloc(RECV_BUF_SIZE);
    send_buf = malloc(RECV_BUF_SIZE);

    FD_ZERO(&in_fds);
    max_in_fd = 0;

	igmp_inf_create(igmp_down_if_name);
	igmp_inf_create(igmp_up_if_name);

	return 0;
}

void shut_igmp_proxy(void)
{
	/* all interface leave multicast group */
}


int add_mr(__u32 group, __u32 src)
{
struct ip_mreq mreq;
struct MRouteDesc	mrd;
struct IfDesc *up_dp = getIfByName(igmp_up_if_name);
int ret;

	/* join multicast group */
	mreq.imr_multiaddr.s_addr = group;
	mreq.imr_interface.s_addr = up_dp->InAdr.s_addr;
	ret = setsockopt(up_dp->sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void*)&mreq, sizeof(mreq));
	if(ret) {
		printf("setsockopt IP_ADD_MEMBERSHIP %s error!\n", inet_ntoa(mreq.imr_multiaddr));
		return ret;
	}

	/* add multicast routing entry */
	// Kaohj
	//mrd.OriginAdr.s_addr = 0;
	mrd.OriginAdr.s_addr = src;
	mrd.McAdr.s_addr = group;
	mrd.InVif = igmp_up_if_idx;
	memset(mrd.TtlVc, 0, sizeof(mrd.TtlVc));
	mrd.TtlVc[igmp_down_if_idx] = 1;	
	//printf("add mroute %s \n", inet_ntoa(mrd.McAdr));
	return (addMRoute(&mrd));
}

int del_mr(__u32 group)
{
struct ip_mreq mreq;
struct MRouteDesc	mrd;
struct IfDesc *up_dp = getIfByName(igmp_up_if_name);
int ret;

	/* drop multicast group */
	mreq.imr_multiaddr.s_addr = group;
	mreq.imr_interface.s_addr = up_dp->InAdr.s_addr;
	ret = setsockopt(up_dp->sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (void*)&mreq, sizeof(mreq));
	if(ret)
		printf("setsockopt IP_DROP_MEMBERSHIP %s error!\n", inet_ntoa(mreq.imr_multiaddr));

	/* delete multicast routing entry */
	mrd.OriginAdr.s_addr = 0;
	mrd.McAdr.s_addr = group;
	mrd.InVif = igmp_up_if_idx;
	memset(mrd.TtlVc, 0, sizeof(mrd.TtlVc));
	//printf("del mroute %s \n", inet_ntoa(mrd.McAdr));
	delMRoute(&mrd);
	
	return ret;
}

void igmp_specific_timer_expired(void *arg)
{
struct mcft_entry *mcp = arg;

	//printf("igmp_specific_timer_expired()\n");
	if(!mcp)
		return;

	mcp->timer.retry_left--;
		
	if(mcp->timer.retry_left <= 0) {
		// Kaohj --- check if group has already been dropped
#ifdef KEEP_GROUP_MEMBER
		if (mcp->user_count != 0) {
#endif
		del_mr(mcp->grp_addr);    			
		del_mcft(mcp->grp_addr);
#ifdef KEEP_GROUP_MEMBER
		}
#endif
		untimeout(&mcp->timer.ch);
	}
	else {
		timeout(igmp_specific_timer_expired , mcp, LAST_MEMBER_QUERY_INTERVAL, &mcp->timer.ch);
		igmp_query(ALL_SYSTEMS, mcp->grp_addr, LAST_MEMBER_QUERY_INTERVAL);
	}
}

#ifdef CONFIG_CHECK_MULTICASTROUTE
int check_entry1=0;
int check_entry2=0;
int check_multicast_route=0;
#endif
/*
 * igmp_accept - handles the incoming IGMP packets
 *
 */
 
int igmp_accept(int recvlen, struct IfDesc *dp)
{
    register __u32 src, dst, group, group_src;
    struct iphdr *ip;
    struct igmphdr *igmp;
    struct igmpv3hdr *igmpv3;
    int ipdatalen, iphdrlen, igmpdatalen;
	struct mcft_entry *mymcp;
#ifdef CONFIG_CHECK_MULTICASTROUTE	
	int checkroute=0;	
#endif
    if (recvlen < sizeof(struct iphdr)) {
		log(LOG_WARNING, 0,
		    "received packet too short (%u bytes) for IP header", recvlen);
		return 0;
    }
	
    ip  = (struct iphdr *)recv_buf;
	src = ip->saddr;
	dst = ip->daddr;

	if(!IN_MULTICAST(dst))	/* It isn't a multicast */
		return -1; 
	if(chk_local(src)) 		/* It's our report looped back */
		return -1;
	if(dst == ALL_PRINTER)	/* It's MS-Windows UPNP all printers notify */
		return -1;
		
	pkt_debug(recv_buf);

    iphdrlen  = ip->ihl << 2;
    ipdatalen = ip->tot_len;

    igmp        = (struct igmphdr *)(recv_buf + iphdrlen);
   	group   = igmp->group;

    /* determine message type */
    switch (igmp->type) {
		case IGMP_HOST_MEMBERSHIP_QUERY:
			/* Linux Kernel will process local member query, it wont reach here */
			break;
	
		case IGMP_HOST_MEMBERSHIP_REPORT:
		case IGMP_HOST_NEW_MEMBERSHIP_REPORT:
			if(!IN_MULTICAST(group))
				break;
			/* check if it's protocol reserved group */
			//Brad disable the check20080619
			
			if((group&0xFFFFFF00)==0xE0000000)
				break;
				
			/* TBD */			
			/* should check if it's from downtream interface */
#ifdef CONFIG_CHECK_MULTICASTROUTE	
		if(check_multicast_route ==0){		
				checkroute=check_kernel_multicast_route(1);
				if(checkroute ==1){
					system("route del -net 224.0.0.0 netmask 240.0.0.0 dev br0 2> /dev/null");
					check_entry1=1;
				}
				checkroute = 0;
				checkroute=check_kernel_multicast_route(2);
				if(checkroute ==2){
					system("route del -net 255.255.255.255 netmask 255.255.255.255 dev br0 2> /dev/null");
					check_entry2=1;
				}
			check_multicast_route = 1;	
		}
#endif	

	    	if(!chk_mcft(group)) {
    		// Group does not exist on router, add multicast address into if_table
   				struct IfDesc *up_dp = getIfByName(igmp_up_if_name);
   				int ret;

				mymcp = add_mcft(group, src);
				if(!mymcp) {
	    			//printf("igmp_accept> add group to list fail!\n");
					break;
				}
				// Kaohj
				//add_mr(group);
				add_mr(group, src);
#ifdef PERIODICAL_QUERY
				mymcp->timer.retry_left = MEMBER_QUERY_COUNT;
				timeout(igmp_specific_timer_expired , mymcp, MEMBER_QUERY_INTERVAL, &mymcp->timer.ch);
#endif
			}
			else {
				mymcp = get_mcft(group);
				if (mymcp)
				{
					untimeout(&mymcp->timer.ch);
#ifdef KEEP_GROUP_MEMBER
					add_user(mymcp, src);
#endif
#ifdef PERIODICAL_QUERY
					mymcp->timer.retry_left = MEMBER_QUERY_COUNT;
					timeout(igmp_specific_timer_expired , mymcp, MEMBER_QUERY_INTERVAL, &mymcp->timer.ch);
#endif
				}
			}
			
			break;

 		case IGMP_HOST_V3_MEMBERSHIP_REPORT:
			/* TBD */			
			/* should check if it's from downtream interface */

 			#if 1
 			group = *(__u32 *)((char *)igmp+12);
 			igmp_query(ALL_SYSTEMS, 0, 1);
 			igmp_query(ALL_SYSTEMS, group, 1);
			#else
			igmpv3 = (struct igmpv3hdr *)(recv_buf + iphdrlen);

			if(igmpv3->rcd_type == 3) {
				if(igmpv3->num_grp && igmpv3->num_src) {
					group = igmpv3->ip[0];
					if(!IN_MULTICAST(group))
						break; 
		    		if(!chk_mcft(group)) {
						// Group does not exist on router, add multicast address into if_table
						if(!add_mcft(group, src)) {
	    					//printf("igmp_accept> add group to list fail!\n");
							break;
						}
						// Kaohj
						//add_mr(group);
						add_mr(group, src);
					}
					else {
						struct mcft_entry *mcp = get_mcft(group);
						if(mcp)
							untimeout(&mcp->timer.ch);
					}
				}
			}
			else if(igmpv3->rcd_type == 6) {
				if(igmpv3->num_grp && igmpv3->num_src) {
					group = igmpv3->ip[0];
					if(!IN_MULTICAST(group))
						break; 
		    		if(chk_mcft(group)) { 
						struct mcft_entry *mcp = get_mcft(group);
	    				// Group does exist on router
						if(mcp) {
							mcp->timer.retry_left = LAST_MEMBER_QUERY_COUNT;
							timeout(igmp_specific_timer_expired , mcp, LAST_MEMBER_QUERY_INTERVAL, &mcp->timer.ch);
		    				igmp_query(ALL_SYSTEMS, mcp->grp_addr, LAST_MEMBER_QUERY_INTERVAL);
		    			}
					}
				}
			}
			#endif			
			break;

		case IGMP_HOST_LEAVE_MESSAGE :
			if(!IN_MULTICAST(igmp->group)) {
    			//printf("igmp_accept> invalid multicast address or IGMP leave\n");
				break;		
			}	
			//printf("igmp_accept> receive IGMP Leave from %s,", inet_ntoa(ip->saddr));
			//printf("group = %s\n", inet_ntoa(igmp->group));
			/* TBD */			
			/* should check if it's from downtream interface */
    		if(chk_mcft(igmp->group)) { 
				mymcp = get_mcft(igmp->group);
    			// Group does exist on router
				if(mymcp) {
#ifdef KEEP_GROUP_MEMBER
					int count;
					count = del_user(mymcp, src);
					if (count == 0) {// no member, drop it!
						del_mr(mymcp->grp_addr);    			
						del_mcft(mymcp->grp_addr);
#ifdef CONFIG_CHECK_MULTICASTROUTE
						alarm(5);

#endif  			
					
					
					}
#endif
					mymcp->timer.retry_left = LAST_MEMBER_QUERY_COUNT;
					timeout(igmp_specific_timer_expired , mymcp, LAST_MEMBER_QUERY_INTERVAL, &mymcp->timer.ch);
					igmp_query(ALL_SYSTEMS, mymcp->grp_addr, LAST_MEMBER_QUERY_INTERVAL);
	    		}
			}
			break;
			
		default:
			//printf("igmp_accept> receive IGMP Unknown type [%x] from %s:", igmp->type, inet_ntoa(ip->saddr));
			//printf("%s\n", inet_ntoa(ip->daddr));
			break;
    }
    return 0;
}


/*
 * igmp_report - send an IGMP Report packet, directly to linkp->send(), not via ip
 *
 * int igmp_report( longword ina, int ifno )
 * Where:
 *	ina	the group address to report.
 *      ifno	interface number
 *
 * Returns:
 *	0	if unable to send report
 *	1	report was sent successfully
 */

int igmp_report(__u32 dst, int if_idx)
{
    struct iphdr *ip;
    struct igmphdr *igmp;
    struct sockaddr_in sdst;

    ip                      = (struct iphdr *)send_buf;
    ip->saddr       = getAddrByVifIx(if_idx);
    ip->daddr       = dst;
    ip->tot_len              = MIN_IP_HEADER_LEN + IGMP_MINLEN;
    if (IN_MULTICAST(ntohl(dst))) {
		ip->ttl = 1;
	    if (setsockopt(igmp_socket, IPPROTO_IP, IP_MULTICAST_IF,
		   	(char *)&ip->saddr, sizeof(struct in_addr)) < 0)
		   	printf("igmp_report> set multicast interface error\n");

	}
    else
		ip->ttl = MAXTTL;

    igmp                    = (struct igmphdr *)(send_buf + MIN_IP_HEADER_LEN);
    igmp->type         = IGMP_HOST_NEW_MEMBERSHIP_REPORT;
    igmp->code         = 0;
    igmp->group		   = dst;
    igmp->csum        = 0;
    igmp->csum        = in_cksum((u_short *)igmp, IGMP_MINLEN);

    bzero(&sdst, sizeof(sdst));
    sdst.sin_family = AF_INET;
    sdst.sin_addr.s_addr = dst;
    if (sendto(igmp_socket, send_buf,
			MIN_IP_HEADER_LEN + IGMP_MINLEN, 0,
			(struct sockaddr *)&sdst, sizeof(sdst)) < 0) {
		printf("igmp_report> sendto error, from %s ", inet_ntoa(ip->saddr));
		printf("to %s\n", inet_ntoa(ip->daddr));
    }

    return 1;
}




/*
 * igmp_query - send an IGMP Query packet to downstream interface
 *
 * int igmp_query(__u32 dst, __u32 grp,__u8 mrt)
 * Where:
 *  dst		destination address
 *  grp		query group address
 *  MRT		Max Response Time in IGMP header (in 1/10 second unit)
 *
 * Returns:
 *	0	if unable to send
 *	1	packet was sent successfully
 */

int igmp_query(__u32 dst, __u32 grp,__u8 mrt)
{
    struct iphdr *ip;
    struct igmphdr *igmp;
    struct sockaddr_in sdst;
	struct IfDesc *dp = getIfByName(igmp_down_if_name);
	
    ip              = (struct iphdr *)send_buf;
    ip->saddr       = dp->InAdr.s_addr;
    ip->daddr       = dst;
    ip->tot_len              = MIN_IP_HEADER_LEN + IGMP_MINLEN;
	ip->ttl = 1;

    igmp               = (struct igmphdr *)(send_buf + MIN_IP_HEADER_LEN);
   	igmp->type         = 0x11;
    igmp->code         = mrt;
    igmp->group 	   = grp;
    igmp->csum        = 0;
    igmp->csum        = in_cksum((u_short *)igmp, IGMP_MINLEN);

    bzero(&sdst, sizeof(struct sockaddr_in));
    sdst.sin_family = AF_INET;
    sdst.sin_addr.s_addr = dst;

    //printf("send igmp query\n");
    if (sendto(dp->sock, igmp, IGMP_MINLEN, 0,
			(struct sockaddr *)&sdst, sizeof(sdst)) < 0) {
		printf("igmp_query> sendto error, from %s ", inet_ntoa(ip->saddr));
		printf("to %s\n", inet_ntoa(ip->daddr));
    }

    return 0;
}

/*
 * igmp_leave - send an IGMP LEAVE packet, directly to linkp->send(), not via ip
 *
 * int igmp_leave( longword ina, int ifno )
 * Where:
 *  	ina	the IP address to leave
 *  	ifno	interface number
 *
 * Returns:
 *	0	if unable to send leave
 *	1	report was sent successfully
 */

int igmp_leave(__u32 grp, int if_idx)
{
    struct iphdr *ip;
    struct igmphdr *igmp;
    struct sockaddr_in sdst;
	struct IfDesc *dp = getIfByName(igmp_up_if_name);
	
    ip              = (struct iphdr *)send_buf;
    ip->saddr       = dp->InAdr.s_addr;
    ip->daddr       = ALL_ROUTERS;
    ip->tot_len              = MIN_IP_HEADER_LEN + IGMP_MINLEN;
	ip->ttl = 1;

    igmp               = (struct igmphdr *)(send_buf + MIN_IP_HEADER_LEN);
   	igmp->type         = 0x17;
    igmp->code         = 0;
    igmp->group 	   = grp;
    igmp->csum        = 0;
    igmp->csum        = in_cksum((u_short *)igmp, IGMP_MINLEN);

    bzero(&sdst, sizeof(struct sockaddr_in));
    sdst.sin_family = AF_INET;
    sdst.sin_addr.s_addr = ALL_ROUTERS;
    //printf("send igmp leave\n");
	
    if (sendto(dp->sock, igmp, IGMP_MINLEN, 0,
			(struct sockaddr *)&sdst, sizeof(sdst)) < 0) {
		printf("igmp_leave> sendto error, from %s ", inet_ntoa(ip->saddr));
		printf("to %s\n", inet_ntoa(ip->daddr));
    }

    return 0;
}






////////////////////////////////////////////////////////////////////////////////////


char* runPath = "/bin/igmpproxy";
char* pidfile = "/var/run/igmp_pid";

#if 0
static void clean(void)
/*
** Cleans up, i.e. releases allocated resources. Called via atexit().
** 
*/        
{
  log( LOG_DEBUG, 0, "clean handler called" );
  disableMRouter();

  unlink(pidfile);
  exit(EXIT_SUCCESS);
}
#endif

/*
 * On hangup, let everyone know we're going away.
 */
 
void hup(int signum)
{
	(void)signum;

  log( LOG_DEBUG, 0, "clean handler called" );
#ifdef	IGMP_MCAST2UNI  
  Update_igmpProxyStateToKernel(0);
#endif
  disableMRouter();

  unlink(pidfile);
  exit(EXIT_SUCCESS);

}
#ifdef CONFIG_CHECK_MULTICASTROUTE
void singnalAlrm(int signum)
{
	(void)signum;
	int checkroute=0;
	
	checkroute=check_kernel_multicast_route(1);
	if(check_entry1==1 && checkroute==0){
				system("route add -net 224.0.0.0 netmask 240.0.0.0 dev br0 2> /dev/null");
		check_entry1=0;
			}
	checkroute = 0;
	checkroute=check_kernel_multicast_route(2);
	if( check_entry2==1 && checkroute ==0){
		system("route add -net 255.255.255.255 netmask 255.255.255.255 dev br0 2> /dev/null");
		check_entry2=0;
		}
	if(check_multicast_route ==1)
		check_multicast_route= 0;
			
}
#endif

// Kaohj added
// Comes here because upstream or downstream interface ip changed
// Usually, it is used by dynamic interface to sync its interface with
// the igmpproxy local database.
void sigifup(int signum)
{
	(void)signum;
	struct ifreq IfVc[ MAX_IF  ];
	struct ifreq *IfEp, *IfPt;
	struct ifconf IoCtlReq;
	struct IfDesc *Dup, *Ddp;
	int Sock;
	
	// get information of all the interfaces
	if( (Sock = socket( AF_INET, SOCK_DGRAM, 0 )) < 0 )
		log( LOG_ERR, errno, "RAW socket open" );
	
	IoCtlReq.ifc_buf = (void *)IfVc;
	IoCtlReq.ifc_len = sizeof( IfVc );
	
	if( ioctl( Sock, SIOCGIFCONF, &IoCtlReq ) < 0 )
		log( LOG_ERR, errno, "ioctl SIOCGIFCONF" );
	
	close( Sock );
	IfEp = (void *)((char *)IfVc + IoCtlReq.ifc_len);
	
	// get descriptors of upstream and downstream interfaces
	Dup = getIfByName(igmp_up_if_name);
	Ddp = getIfByName(igmp_down_if_name);
	if (Dup == NULL || Ddp == NULL)
		return;
	
	// update upstream/downstream interface ip into local database
	for( IfPt = IfVc; IfPt < IfEp; IfPt++ ) {
		if (!strcmp(IfPt->ifr_name, Dup->Name)) {
			Dup->InAdr = ((struct sockaddr_in *)&IfPt->ifr_addr)->sin_addr;
			//printf("update upstream ip to %s\n", inet_ntoa(Dup->InAdr));
		}
		else if (!strcmp(IfPt->ifr_name, Ddp->Name)) {
			Ddp->InAdr = ((struct sockaddr_in *)&IfPt->ifr_addr)->sin_addr;
			//printf("update downstream ip to %s\n", inet_ntoa(Ddp->InAdr));
		}
	}
}

static int initMRouter(void)
/*
** Inits the necessary resources for MRouter.
**
*/
{
	int Err;
	int i;
	struct IfDesc *Ddp, *Dup;
	
  buildIfVc();    

  switch( Err = enableMRouter() ) {
    case 0: break;
    case EADDRINUSE: log( LOG_ERR, EADDRINUSE, "MC-Router API already in use" ); break;
    default: log( LOG_ERR, Err, "MRT_INIT failed" );
  }
      
	Ddp = getIfByName(igmp_down_if_name);
	Dup = getIfByName(igmp_up_if_name);
	if (Ddp==NULL || Dup==NULL)
		return 0;
	
	/* add downstream interface */
	igmp_down_if_idx = addVIF(Ddp);

	/* add upstream interface */
	igmp_up_if_idx = addVIF(Dup);

	signal(SIGTERM, hup);
  //atexit( clean );
  	return 1;
}

void
write_pid()
{
	FILE *fp = fopen(pidfile, "w+");
	if (fp) {
		fprintf(fp, "%d\n", getpid());
		fclose(fp);
	}
	else
	 	printf("Cannot create pid file\n");
}

void
clear_pid()
{
	FILE *fp = fopen(pidfile, "w+");
	if (fp) {
		fprintf(fp, "%d\n", 0);
		fclose(fp);
	}
	else
	 	printf("Cannot create pid file\n");
}


extern int MRouterFD;

int main(int argc, char **argv)
{
	int _argc = 0;
	char *_argv[5];
	pid_t pid;
	int execed = 0;
	char cmdBuffer[50];//Brad add 20080605
	struct IfDesc *IfDp;
	int flags;
	if (argc >= 5) {
		fprintf(stderr, "To many arguments \n");
		exit(1);
	}
	
	if (strcmp(argv[argc-1], "-D") == 0) {
		argc--;
		execed = 1;
	}
	
	if(argc < 2) {
		printf("Usage: igmpproxy <up interface> [down interface]\n\n");
		return;
	}

	if (!execed) {
		if ((pid = vfork()) < 0) {
			fprintf(stderr, "vfork failed\n");
			exit(1);
		} else if (pid != 0) {
			exit(0);
		}
		
		for (_argc=0; _argc < argc; _argc++ )
			_argv[_argc] = argv[_argc];
		_argv[0] = runPath;
		_argv[argc++] = "-D";
		_argv[argc++] = NULL;
		execv(_argv[0], _argv);
		/* Not reached */
		fprintf(stderr, "Couldn't exec\n");
		_exit(1);

	} else {
		setsid();
	}

	if(argc == 2)
		strcpy(igmp_down_if_name, "eth0");
	else
		strcpy(igmp_down_if_name, argv[2]);

	strcpy(igmp_up_if_name, argv[1]);
	//Brad add 20080605
	memset(cmdBuffer, '\0',50);
	sprintf(cmdBuffer, "echo %s > /var/igmp_up", igmp_up_if_name);
	system(cmdBuffer);
	write_pid();

	signal(SIGHUP, hup);
	signal(SIGTERM, hup);
	signal(SIGUSR1, sigifup);
#ifdef CONFIG_CHECK_MULTICASTROUTE	
	signal(SIGALRM,singnalAlrm);
#endif
	while (!initMRouter())
	{
		// Kaohj, polling every 2 seconds
		//printf("initMRouter fail\n");
		sleep(2);
	}
	init_igmp();
#ifdef	IGMP_MCAST2UNI	
	Update_igmpProxyStateToKernel(1);
#endif

	//hyking:recv the sock for avoid dst cache refcnt issue.
	//2010-8-3
	IfDp = getIfByName(igmp_down_if_name);
	if(IfDp->sock > 0)
	{
		add_fd(IfDp->sock);
		flags = fcntl(IfDp->sock, F_GETFL);
		if (flags == -1 || fcntl(IfDp->sock, F_SETFL, flags | O_NONBLOCK) == -1)
		   	printf("Couldn't set %s to nonblock\n",igmp_down_if_name);

	}
	
	IfDp = getIfByName(igmp_up_if_name);
	if(IfDp->sock > 0)
	{
		add_fd(IfDp->sock);
		flags = fcntl(IfDp->sock, F_GETFL);
		if (flags == -1 || fcntl(IfDp->sock, F_SETFL, flags | O_NONBLOCK) == -1)
		   	printf("Couldn't set sock of %s to nonblock\n",igmp_up_if_name);
	}
	
	if(MRouterFD>0)
	{
		add_fd(MRouterFD);
		flags = fcntl(MRouterFD, F_GETFL);
		if (flags == -1 || fcntl(MRouterFD, F_SETFL, flags | O_NONBLOCK) == -1)
		   	printf("Couldn't set MRouterFD to nonblock\n");

	}
#if defined(USE_STATIC_ENTRY_BUFFER)
	memset(mcft_entry_tbl, 0x00, sizeof(struct mcft_entry_en)*MAX_MFCT_ENTRY);
	memset(mbr_entry_tbl, 0x00, sizeof(struct mbr_entry_en)*MAX_MBR_ENTRY);
#endif
	DISPLAY_BANNER;
	/* process loop */
	/*2008-0919 add ,when l2pt disconnection or any reason ;when igmpProxy be restart
	should issue query first*/ 
	igmp_query(ALL_SYSTEMS, 0, 1);

	while(1)
	{
		fd_set in;
		struct timeval tv;
		int ret;
		int recvlen;

		calltimeout();		

		tv.tv_sec = 0;
		tv.tv_usec = 100000;
		
		in = in_fds;
		
		ret = select(max_in_fd+1, &in, NULL, NULL, &tv);

		if( ret <= 0 ){
			//printf("igmp: timeout\n");
			continue;
		} 	

		if(FD_ISSET(MRouterFD, &in_fds)) {
			recvlen = recvfrom(MRouterFD, recv_buf, RECV_BUF_SIZE,
				   0, NULL, &recvlen);
			if (recvlen > 0) {
				IfDp = getIfByName(igmp_down_if_name);
				igmp_accept(recvlen, IfDp);		
			}
			else
			{
		    		if (errno != EINTR && errno !=EAGAIN) log(LOG_ERR, errno, "recvfrom multicast route sock");
		    			
			}
			
	     }

		//hyking:recv the sock for avoid dst cache refcnt issue.
		//2010-8-3
		IfDp =  getIfByName(igmp_down_if_name);
		if(FD_ISSET(IfDp->sock, &in_fds))
		{
			recvlen = recvfrom(IfDp->sock,recv_buf,RECV_BUF_SIZE,
					   0, NULL, &recvlen);
			
			if (recvlen < 0) 
			{
		    		if (errno != EINTR && errno !=EAGAIN) log(LOG_ERR, errno, "recvfrom down interface");
			}
		}


		IfDp =  getIfByName(igmp_up_if_name);		
		if(FD_ISSET(IfDp->sock, &in_fds))
		{
			recvlen = recvfrom(IfDp->sock,recv_buf,RECV_BUF_SIZE,
				   0, NULL, &recvlen);
			if (recvlen < 0) 
			{
		    		if (errno != EINTR && errno !=EAGAIN) log(LOG_ERR, errno, "recvfrom up interface");
			}
		}
		
	}
	
	return 0;
}



