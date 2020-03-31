/*
 *	linux.c
 *	Release $Name:  $
 *
 *	Linux compatibility layer
 *	Other UNIX like operating systems should also be able to use this
 *	implementation without change.
 *
 *	Copyright (c) PeerSec Networks, 2002-2004. All Rights Reserved.
 *	The latest version of this code is available at http://www.matrixssl.org
 *
 *	This software is open source; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This General Public License does NOT permit incorporating this software 
 *	into proprietary programs.  If you are unable to comply with the GPL, a 
 *	commercial license for this software may be purchased from PeerSec Networks
 *	at http://www.peersec.com
 *	
 *	This program is distributed in WITHOUT ANY WARRANTY; without even the 
 *	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 *	See the GNU General Public License for more details.
 *	
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *	http://www.gnu.org/copyleft/gpl.html
 */
/******************************************************************************/
#if LINUX
#include <fcntl.h>
#include <errno.h>
#include <sys/times.h>
#include <time.h>

#include "../../matrixInternal.h"

#ifdef __i386__
#define rdtscll(val) __asm__ __volatile__("rdtsc" : "=A" (val))
static sslTime_t	hiresStart; 	/* zero-time */
static sslTime_t	hiresFreq; 		/* tics per second */
#else /* __i386__ */
static unsigned	int prevTicks; 		/* Check wrap */
static sslTime_t	elapsedTime; 	/* Last elapsed time */
#endif

#ifdef USE_MULTITHREADING
#include <pthread.h>
static pthread_mutexattr_t	attr;
#endif

/* max sure we don't retry reads forever */
#define	MAX_RAND_READS		1024

static 	int	urandfd = -1;
static 	int	randfd	= -1;

int sslOpenOsdep()
{
	FILE		*cpuInfo;
	double		mhz;
	char		line[80];
	char		*tmpstr;
	int 		c;
/*
	Open /dev/random access non-blocking.
*/
	if ((randfd = open("/dev/random", O_RDONLY | O_NONBLOCK)) < 0) {
		return -1;
	}
	if ((urandfd = open("/dev/urandom", O_RDONLY)) < 0) {
		close(randfd);
		return -1;
	}
/*
	Initialize times
*/
#ifdef __i386__
	if ((cpuInfo = fopen ("/proc/cpuinfo","r")) == NULL) {
		matrixStrDebugMsg("Error opening /proc/cpuinfo\n", NULL);
		return -2;
	}

	while ((!feof(cpuInfo)) && (strncasecmp(line,"cpu MHz",7) != 0)){
		fgets(line,79,cpuInfo);
	}

	if (strncasecmp(line,"cpu MHz",7) == 0){ 
		tmpstr = strchr(line,':');
		tmpstr++;
		c = strspn(tmpstr, " \t");
		tmpstr +=c;
		c = strcspn(tmpstr, " \t\n\r");
		tmpstr[c] = '\0';
		mhz = 1000000 * atof(tmpstr);
		hiresFreq = (int)mhz;
		fclose (cpuInfo);	
	} else {
		fclose (cpuInfo);
		hiresStart = 0;
		return -3;
	}
	rdtscll(hiresStart);
#endif /* __i386__ */
/*
	FUTURE - Solaris doesn't support recursive mutexes!
	We don't use them internally anyway, so this is not an issue,
	but we like to set this if we can because it's silly for a thread to lock
	itself, rather than error or recursive lock
*/
#ifdef USE_MULTITHREADING
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
	pthread_mutexattr_init(&attr);
#endif
	sslOpenMalloc(MAX_MEMORY_USAGE);
	return 0;
}

int sslCloseOsdep()
{
	sslCloseMalloc();
#ifdef USE_MULTITHREADING
	pthread_mutexattr_destroy(&attr);
#endif
	close(randfd);
	close(urandfd);
	return 0;
}

/*
	Read from /dev/random non-blocking first, then from urandom if it would 
	block.  Also, handle file closure case and re-open.
*/

int sslGetEntropy(char *bytes, int size)
{
	int				rc, sanity, retry;
	unsigned char 	*where = bytes;

	sanity = retry = 0;

	while (size) {
		if ((rc = read(randfd, where, size)) < 0 || sanity > MAX_RAND_READS) {
			if (errno == EINTR) {
				if (sanity > MAX_RAND_READS) {
					return -1;
				}
				sanity++;
				continue;
			} else if (errno == EAGAIN) {
				break;
			} else if (errno == EBADF && retry == 0) {
				close(randfd);
				if ((randfd = open("/dev/random", O_RDONLY | O_NONBLOCK)) < 0) {
					break;
				}
				retry++;
				continue;
			} else {
				break;
			}
		}
		where += rc;
		size -= rc;
	}


	sanity = retry = 0;	
	while (size) {
		if ((rc = read(urandfd, where, size)) < 0 || sanity > MAX_RAND_READS) {
			if (errno == EINTR) {
				if (sanity > MAX_RAND_READS) {
					return -1;
				}
				sanity++;
				continue;
			} else if (errno == EBADF && retry == 0) {
				close(urandfd);
				if ((urandfd = 
					open("/dev/urandom", O_RDONLY | O_NONBLOCK)) < 0) {
					return -1;
				}
				retry++;
				continue;
			} else {
				return -1;
			}
		}
		where += rc;
		size -= rc;
	}
	return rc;
}

#if DEBUG
void sslBreak()
{
	abort();
}
#endif

/******************************************************************************/

#ifdef USE_MULTITHREADING

int sslCreateMutex(sslMutex_t *mutex)
{

	if (pthread_mutex_init(mutex, &attr) != 0) {
		return -1;
	}	
	return 0;
}

int sslLockMutex(sslMutex_t *mutex)
{
	if (pthread_mutex_lock(mutex) != 0) {
		return -1;
	}
	return 0;
}

int sslUnlockMutex(sslMutex_t *mutex)
{
	if (pthread_mutex_unlock(mutex) != 0) {
		return -1;
	}
	return 0;
}

void sslDestroyMutex(sslMutex_t *mutex)
{
	pthread_mutex_destroy(mutex);
}
#endif /* USE_MULTITHREADING */

/*****************************************************************************/
/*
	Use a platform specific high resolution timer
*/
#ifdef __i386__

int sslInitMsecs(sslTime_t *t)
{
	unsigned long long	diff;
	int					d;
	
	rdtscll(*t);
	diff = *t - hiresStart;
	d = (int)((diff * 1000) / hiresFreq);
	return d;
}

/*
	Time comparison.  0 if 'a' is less than or equal.  1 if 'a' is greater
	The return codes were written this way specifically to make the
	'if' statement in which it is used more clean.
*/
int	sslCompareTime(sslTime_t a, sslTime_t b)
{
	if (a <= b) {
		return 0;
	}
	return 1;
}

#else /* __i386__ */

int sslInitMsecs(sslTime_t *timePtr)
{
	struct tms		tbuff;
	unsigned int	t, deltat, deltaticks;
                                                                                
/*
 *	times() returns the number of clock ticks since the system
 *	was booted.  If it is less than the last time we did this, the
 *	clock has wrapped around 0xFFFFFFFF, so compute the delta, otherwise
 *	the delta is just the difference between the new ticks and the last
 *	ticks.  Convert the elapsed ticks to elapsed msecs using rounding.
 */
	if ((t = times(&tbuff)) >= prevTicks) {
		deltaticks = t - prevTicks;
	} else {
		deltaticks = (0xFFFFFFFF - prevTicks) + 1 + t;
	}
	deltat = ((deltaticks * 1000) + (CLK_TCK / 2)) / CLK_TCK;
                                                                     
/*
 *	Add the delta to the previous elapsed time.
 */
	elapsedTime.usec += ((deltat % 1000) * 1000);
	if (elapsedTime.usec >= 1000000) {
		elapsedTime.usec -= 1000000;
		deltat += 1000;
	}
	elapsedTime.sec += (deltat / 1000);
	prevTicks = t;
                                                                                
/*
 *	Return the current elapsed time.
 */
	timePtr->usec = elapsedTime.usec;
	timePtr->sec = elapsedTime.sec;
	return (timePtr->usec / 1000) + timePtr->sec * 1000;
}

/*
	Time comparison.  0 if 'a' is less than or equal.  1 if 'a' is greater
	The return codes were written this way specifically to make the
	'if' statement in which it is used more clean.
*/
int	sslCompareTime(sslTime_t a, sslTime_t b)
{
	if (a.sec < b.sec) {
		return 0;
	} else if (a.sec == b.sec) {
		if (a.usec <= b.usec) {
			return 0;
		} else {
			return 1;
		}
	}	
	return 1;
}

#endif /* __i386__ */


#endif /* LINUX */

/******************************************************************************/
