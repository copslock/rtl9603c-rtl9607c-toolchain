/*
 *	osLayer.h
 *	Release $Name:  $
 *	
 *	Layered header for OS specific functions
 *	Contributors adding new OS support must implement all functions 
 *	externed below.
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

#ifndef _h_OS_LAYER
#define _h_OS_LAYER

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
	Functions defined at OS level
*/
extern int	sslOpenOsdep();
extern int	sslCloseOsdep();
extern int	sslGetEntropy(char *bytes, int size);

/*
	Defines to make library multithreading safe
*/
#ifdef USE_MULTITHREADING

#if WIN32
#include <windows.h>

typedef CRITICAL_SECTION sslMutex_t;
#define sslCreateMutex(M)	InitializeCriticalSection((CRITICAL_SECTION *) M);
#define sslLockMutex(M)		EnterCriticalSection((CRITICAL_SECTION *) M);
#define sslUnlockMutex(M)	LeaveCriticalSection((CRITICAL_SECTION *) M);
#define sslDestroyMutex(M)	DeleteCriticalSection((CRITICAL_SECTION *) M);

#elif LINUX
#include <pthread.h>

typedef pthread_mutex_t sslMutex_t;
extern int	sslCreateMutex(sslMutex_t *mutex);
extern int	sslLockMutex(sslMutex_t *mutex);
extern int	sslUnlockMutex(sslMutex_t *mutex);
extern void	sslDestroyMutex(sslMutex_t *mutex);
#endif

#else /* USE_MULTITHREADING */

typedef int	sslMutex_t;
#define sslCreateMutex(M)
#define sslLockMutex(M)
#define sslUnlockMutex(M)
#define sslDestroyMutex(M)

#endif /* USE_MULTITHREADING */

/*
	Make sslTime_t an opaque time value.
	FUTURE - use high res time instead of time_t
*/
#if WIN32
typedef LARGE_INTEGER sslTime_t;
#elif __i386__
typedef unsigned long long LARGE_INTEGER;
typedef LARGE_INTEGER sslTime_t; 
#elif VXWORKS
typedef time_t sslTime_t;
#else
typedef struct {
		long sec;
		long usec;
	} sslTime_t;
#endif

extern int	sslInitMsecs(sslTime_t *t);
extern int	sslCompareTime(sslTime_t a, sslTime_t b);

#ifdef __cplusplus
}
#endif

#endif /* _h_OS_LAYER */

/******************************************************************************/
