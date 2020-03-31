/*
 *	malloc.c
 *	Release $Name:  $
 *
 *	Custom malloc implementation designed for low memory environments
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

#include "../matrixInternal.h"

#ifdef USE_PEERSEC_MALLOC

typedef struct psMemHeader {
	struct psMemHeader	*next;
	long				size;	/* long on purpose for 64 bit compatibility */
} sslMemHeader_t;

int		memAlloced;
int		highWater;

/*
	FUTURE - Provide block caching for all sizes.  This 256 byte block cache is 
	just an implementation example.
*/
#pragma message("*** USE_PEERSEC_MALLOC for debugging only ***")
sslMemHeader_t	*cache256;

int sslOpenMalloc(int poolSize)
{
	memAlloced = 0;
	highWater = 0;
	cache256 = NULL;
	return 0;
}

void sslCloseMalloc()
{
	void	*p;

	matrixIntDebugMsg("highWater=%d\n", highWater);
	while (cache256 != NULL) {
		p = cache256->next;
		free(cache256);
		cache256 = p;
	}
	sslAssert(memAlloced == 0);
}

void *sslMalloc(size_t size)
{
	sslMemHeader_t	*p;

	memAlloced += (int)size;
	if (size == 256 && cache256 != NULL) {
		p = cache256;
		cache256 = p->next;
		matrixIntDebugMsg("C memAlloced=%d", memAlloced);
	} else {
		p = malloc(size + sizeof(sslMemHeader_t));
		matrixIntDebugMsg("M memAlloced=%d", memAlloced);
	}
	matrixIntDebugMsg("(+ %d)\n", (int)size);
	p->next = NULL;
	p->size = (int)size;
	if (memAlloced > highWater) {
		highWater = memAlloced;
	}
	return p + 1;
}

void sslFree(void *ptr)
{
	sslMemHeader_t	*hdr;

	hdr = ptr;
	hdr--;
	memAlloced -= hdr->size;
	matrixIntDebugMsg("F memAlloced=%d", memAlloced);
	matrixIntDebugMsg("(- %d)\n", hdr->size);
	if (hdr->size == 256) {
		hdr->next = cache256;
		cache256 = hdr;
	} else {
		free(hdr);
	}
}

void *sslCalloc(size_t n, size_t size)
{
	void	*p;
	
	p = sslMalloc(n * size);
	memset(p, 0x0, n * size);
	return p;
}

void *sslRealloc(void *ptr, size_t n)
{
	sslMemHeader_t	*hdr;
	void			*p;
	
	hdr = ptr;
	hdr--;
	p = sslMalloc(n);
	memcpy(p, ptr, min((long)n, hdr->size));
/*
	SECURITY - We zero the old memory block here, before we allow it
	to be reused.
*/
	memset(ptr, 0x0, hdr->size);
	memAlloced -= (int)hdr->size;
	matrixIntDebugMsg("R memAlloced=%d", memAlloced);
	matrixIntDebugMsg("(- %d)\n", (int)hdr->size);
	free(hdr);
	return p;
}

#endif /* USE_PEERSEC_MALLOC */

/******************************************************************************/
