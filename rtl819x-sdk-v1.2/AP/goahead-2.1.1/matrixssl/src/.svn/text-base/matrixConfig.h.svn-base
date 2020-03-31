/*
 *	matrixConfig.h
 *	Release $Name:  $
 *
 *	Configuration settings for building the MatrixSSL library.
 *	These options affect the size and algorithms present in the library.
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

#ifndef _h_MATRIXCONFIG
#define _h_MATRIXCONFIG

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/*
	Define the number of sessions to cache here.
	Minimum value is 1
	Session caching provides such an increase in performance that it isn't
	an option to disable.
*/
#define SSL_SESSION_TABLE_SIZE	32

/******************************************************************************/
/*
	Define the following to enable various cipher suites
	At least one of these must be defined.  If multiple are defined,
	the handshake will determine which is best for the connection.
*/
#define USE_SSL_RSA_WITH_RC4_128_MD5
#define USE_SSL_RSA_WITH_RC4_128_SHA
/* #define USE_SSL_RSA_WITH_3DES_EDE_CBC_SHA */

/******************************************************************************/
/*
	Support for encrypted private key files, using 3DES
*/
#define USE_ENCRYPTED_PRIVATE_KEYS

/******************************************************************************/
/*
	Support for client side SSL
*/
//#define USE_CLIENT_SIDE_SSL
//sc_yang
#define USE_SERVER_SIDE_SSL

/******************************************************************************/
/*
	Support for multithreading environment.  This should be enabled
	if multiple SSL sessions will be active at the same time in 
	different threads.  The library will still be single threaded,
	but will serialize access to the session cache with a mutex.
*/
//sc_yang
//#define USE_MULTITHREADING

/******************************************************************************/
/*
	Support for file system.
*/
#define USE_FILE_SYSTEM

/******************************************************************************/
/*
	Custom memory allocators can be defined here
*/
/* #define USE_PEERSEC_MALLOC */

#ifdef USE_PEERSEC_MALLOC
#include <stddef.h>
#define MAX_MEMORY_USAGE	1024 * 1024	/* FUTURE */
extern int sslOpenMalloc(int poolSize);
extern void sslCloseMalloc();
extern void *sslMalloc(size_t size);
extern void sslFree(void *ptr);
extern void *sslCalloc(size_t n, size_t size);
extern void *sslRealloc(void *ptr, size_t n);
#else
#define MAX_MEMORY_USAGE	0
#define sslOpenMalloc(A)
#define sslCloseMalloc()
#define sslMalloc	malloc
#define sslRealloc	realloc
#define sslCalloc	calloc
#define sslFree		free
#endif


/******************************************************************************/
/*
	Debugging functionality.  
	
	If DEBUG is defined matrixStrDebugMsg and matrixIntDebugMsg messages are
	output to stdout, sslAsserts go to stderror and call sslBreak.

	In non-DEBUG builds matrixStrDebugMsg and matrixIntDebugMsg are 
	compiled out.  sslAsserts still go to stderr, but sslBreak is not called.

*/

#if DEBUG
extern void	sslBreak();
extern void matrixStrDebugMsg(char *message, char *arg);
extern void matrixIntDebugMsg(char *message, int arg);
#define sslAssert(C)  if (C) ; else \
	{fprintf(stderr, "%s:%d sslAssert(%s)\n",__FILE__, __LINE__, #C); sslBreak(); }
#else
#define matrixStrDebugMsg(x, y)
#define matrixIntDebugMsg(x, y)
#define sslAssert(C)  if (C) ; else \
	{fprintf(stderr, "%s:%d sslAssert(%s)\n",__FILE__, __LINE__, #C); }
#endif

#ifdef __cplusplus
}
#endif

#endif /* _h_MATRIXCONFIG */

/******************************************************************************/



