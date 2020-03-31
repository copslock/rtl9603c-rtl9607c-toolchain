#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "apmib.h"

#define MAXFNAME	60
#undef WEB_PAGE_OFFSET
#ifdef VOIP_SUPPORT
#define WEB_PAGE_OFFSET 0x20000
#else

#ifdef CONFIG_RTL_WEB_PAGES_OFFSET
#define WEB_PAGE_OFFSET CONFIG_RTL_WEB_PAGES_OFFSET
#else
#define WEB_PAGE_OFFSET 0x10000
#endif

#endif


#define DWORD_SWAP(v) ( (((v&0xff)<<24)&0xff000000) | ((((v>>8)&0xff)<<16)&0xff0000) | \
				((((v>>16)&0xff)<<8)&0xff00) | (((v>>24)&0xff)&0xff) )
#define WORD_SWAP(v) ((unsigned short)(((v>>8)&0xff) | ((v<<8)&0xff00)))
#define __PACK__	__attribute__ ((packed))

/*
typedef struct _file_entry {
	char name[MAXFNAME];
	unsigned long size;
} file_entry_T;
typedef struct _header {
	unsigned char signature[4];
	unsigned long addr;
	unsigned long burn_addr;
	unsigned long len;
} HEADER_T, *HEADER_Tp;
*/

/////////////////////////////////////////////////////////////////////////////
static int compress(char *inFile, char *outFile)
{
	char tmpBuf[100];
	sprintf(tmpBuf, "chmod 666 %s", inFile);
	system(tmpBuf);
	sprintf(tmpBuf, "bzip2 -9 -c %s > %s", inFile, outFile);
	system(tmpBuf);
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
#if 0
static unsigned char CHECKSUM(unsigned char *data, int len)
{
	int i;
	unsigned char sum=0;

	for (i=0; i<len; i++)
		sum += data[i];

	sum = ~sum + 1;
	return sum;
}
#endif

/////////////////////////////////////////////////////////////////////////////
static int lookfor_homepage_dir(FILE *lp, char *dirpath, int is_for_web)
{
	char file[MAXFNAME];
	char *p;
	struct stat sbuf;

	fseek(lp, 0L, SEEK_SET);
	dirpath[0] = '\0';

	while (fgets(file, sizeof(file), lp) != NULL) {
		if ((p = strchr(file, '\n')) || (p = strchr(file, '\r'))) {
			*p = '\0';
		}
		if (*file == '\0') {
			continue;
		}
		if (stat(file, &sbuf) == 0 && sbuf.st_mode & S_IFDIR) {
			continue;
		}
		if (is_for_web)
			p=strstr(file, "home.asp");

		else
			p=strrchr(file, '/');
		if (p) {

			*p = '\0';
			strcpy(dirpath, file);
// for debug
//printf("Found dir=%s\n", dirpath);
			return 0;
		}
	}
	return -1;
}

/////////////////////////////////////////////////////////////////////////////
static void strip_dirpath(char *file, char *dirpath)
{
	char *p, tmpBuf[MAXFNAME];

	if ((p=strstr(file, dirpath))) {
		strcpy(tmpBuf, &p[strlen(dirpath)]);
		strcpy(file, tmpBuf);
	}
// for debug
//printf("adding file %s\n", file);
}


/////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	char *outFile, *fileList;
	char *platform;
	char *tag;
	int fh;
	struct stat sbuf;
	FILE *lp;
	char file[MAXFNAME];
	char tmpFile[100], dirpath[100];
	char buf[512];
	FILE_ENTRY_T entry;
	unsigned char	*p;
	int i, len, fd, nFile, is_web=1, pad=0;
	IMG_HEADER_T head;


	platform = argv[1];
	fileList = argv[2];
	outFile = argv[3];
	if ( argc > 4)
		is_web = 0;

	if(!strcmp(platform, "vpn"))
#if defined(CONFIG_RTL8196B)
	#if defined(CONFIG_RTL8198)
		tag = "w6cv";
	#elif defined(CONFIG_RTL8196C)
		tag = "w6cv";
	#else
		tag = "w6bv";
	#endif
#else		
		tag = "webv";
#endif
	else if(!strcmp(platform, "gw"))
#if defined(CONFIG_RTL8196B)		
	#if defined(CONFIG_RTL8198)
		tag = "w6cg";
	#elif defined(CONFIG_RTL8196C)
		tag = "w6cg";
	#else
		tag = "w6bg";
	#endif
#else		
		tag = "webg";
#endif		
	else if(!strcmp(platform, "ap"))
#if defined(CONFIG_RTL8196B)		
	#if defined(CONFIG_RTL8198)
		tag = "w6ca";
	#elif defined(CONFIG_RTL8196C)
		tag = "w6ca";
	#else
		tag = "w6ba";
	#endif
#else		
		tag = "weba";
#endif

	else if(!strcmp(platform, "cl"))
#if defined(CONFIG_RTL8196B)		
	#if defined(CONFIG_RTL8198)
		tag = "w6cc";
	#elif defined(CONFIG_RTL8196C)
		tag = "w6cc";
	#else
		tag = "w6bc";
	#endif
#else		
		tag = "webc";
#endif		
	else{
		printf("unknow platform!\n");	
		return 0;
	}
		
	fh = open(outFile, O_RDWR|O_CREAT|O_TRUNC);
	if (fh == -1) {
		printf("Create output file error %s!\n", outFile );
		exit(1);
	}
	lseek(fh, 0L, SEEK_SET);

	if ((lp = fopen(fileList, "r")) == NULL) {
		printf("Can't open file list %s\n!", fileList);
		exit(1);
	}
	if (lookfor_homepage_dir(lp, dirpath, is_web)<0) {
		printf("Can't find home.asp page\n");
		fclose(lp);
		exit(1);
	}

	fseek(lp, 0L, SEEK_SET);
	nFile = 0;
	while (fgets(file, sizeof(file), lp) != NULL) {
		if ((p = strchr(file, '\n')) || (p = strchr(file, '\r'))) {
			*p = '\0';
		}
		if (*file == '\0') {
			continue;
		}
		if (stat(file, &sbuf) == 0 && sbuf.st_mode & S_IFDIR) {
			continue;
		}

		if ((fd = open(file, O_RDONLY)) < 0) {
			printf("Can't open file %s\n", file);
			exit(1);
		}
		lseek(fd, 0L, SEEK_SET);

		strip_dirpath(file, dirpath);

		strcpy(entry.name, file);
		entry.size = DWORD_SWAP(sbuf.st_size);

		if ( write(fh, (const void *)&entry, sizeof(entry))!=sizeof(entry) ) {
			printf("Write file failed!\n");
			exit(1);
		}

		i = 0;
		while ((len = read(fd, buf, sizeof(buf))) > 0) {
			if ( write(fh, (const void *)buf, len)!=len ) {
				printf("Write file failed!\n");
				exit(1);
			}
			i += len;
		}
		close(fd);
		if ( i != sbuf.st_size ) {
			printf("Size mismatch in file %s!\n", file );
		}

		nFile++;
	
	
	}

	fclose(lp);
	close(fh);
	sync();

// for debug -------------
#if 0
sprintf(tmpFile, "cp %s web.lst -f", outFile);
system(tmpFile);
#endif
//-------------------------

	sprintf(tmpFile, "%sXXXXXX",  outFile);
	mkstemp(tmpFile);

	if ( compress(outFile, tmpFile) < 0) {
		printf("compress file error!\n");
		exit(1);
	}

	// append header
	if (stat(tmpFile, &sbuf) != 0) {
		printf("Create file error!\n");
		exit(1);
	}
	if((sbuf.st_size+1)%2)
		pad = 1;
	p = malloc(sbuf.st_size + 1 + pad);
	memset(p, 0 , sbuf.st_size + 1);
	if ( p == NULL ) {
		printf("allocate buffer failed!\n");
		exit(1);
	}

	memcpy(head.signature, tag, 4);
	head.len = sbuf.st_size + 1 + pad;
	head.len = DWORD_SWAP(head.len);
	head.startAddr = DWORD_SWAP(WEB_PAGE_OFFSET);
	head.burnAddr = DWORD_SWAP(WEB_PAGE_OFFSET);

	if ((fd = open(tmpFile, O_RDONLY)) < 0) {
		printf("Can't open file %s\n", tmpFile);
		exit(1);
	}
	lseek(fd, 0L, SEEK_SET);
	if ( read(fd, p, sbuf.st_size) != sbuf.st_size ) {
		printf("read file error!\n");
		exit(1);
	}
	close(fd);

	p[sbuf.st_size + pad] = CHECKSUM(p, (sbuf.st_size+pad));

	fh = open(outFile, O_RDWR|O_CREAT|O_TRUNC);
	if (fh == -1) {
		printf("Create output file error %s!\n", outFile );
		exit(1);
	}

	if ( write(fh, &head, sizeof(head)) != sizeof(head)) {
		printf("write header failed!\n");
		exit(1);
	}

	if ( write(fh, p, (sbuf.st_size+1+pad) ) != (sbuf.st_size+1+pad)) {
		printf("write data failed!\n");
		exit(1);
	}

	close(fh);
	chmod(outFile,  DEFFILEMODE);

	sync();

	free(p);
	unlink(tmpFile);

	return 0;
}
