/*
 *	win.c
 *	Release $Name:  $
 *
 *	Microsoft Windows compatibility layer.
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

#include "../../matrixInternal.h"

#include <windows.h>
#include <wincrypt.h>
#include <process.h>
#include <limits.h>

#if WIN32

#define	MAX_INT			0x7FFFFFFF

static LARGE_INTEGER	hiresStart; /* zero-time */
static LARGE_INTEGER	hiresFreq; /* tics per second */

/* For backwards compatibility */
#ifndef CRYPT_SILENT
#define CRYPT_SILENT 0
#endif

static HCRYPTPROV		hProv;	/* Crypto context for random bytes */

int sslOpenOsdep()
{
	sslOpenMalloc(MAX_MEMORY_USAGE);
/*
	Hires time init
*/
	QueryPerformanceFrequency(&hiresFreq);
	QueryPerformanceCounter(&hiresStart);

	if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, 
			CRYPT_VERIFYCONTEXT | CRYPT_SILENT))  {
		return -1;
	}
	return 0;
}

int sslCloseOsdep()
{
	CryptReleaseContext(hProv, 0);
	sslCloseMalloc();
	return 0;
}

int sslGetEntropy(char *bytes, int size)
{
	if (CryptGenRandom(hProv, size, bytes)) {
		return size;
	}
	return -1;
}

#if DEBUG
void sslBreak()
{
	int	i = 0; i++;	/* Prevent the compiler optimizing this function away */

	DebugBreak();
}
#endif

int sslInitMsecs(sslTime_t *t)
{
	__int64		diff;
	int			d;

	QueryPerformanceCounter(t);
	diff = t->QuadPart - hiresStart.QuadPart;
	d = (int)((diff * 1000) / hiresFreq.QuadPart);
	return d;
}

/*
	Time comparison.  0 if 'a' is less than or equal.  1 if 'a' is greater
	The return codes were written this way specifically to make the
	'if' statement in which it is used more clean.
*/
int sslCompareTime(sslTime_t a, sslTime_t b)
{
	if (a.QuadPart <= b.QuadPart) {
		return 0;
	}
	return 1;
}

/******************************************************************************/

#endif /* WIN32 */

/******************************************************************************/
