/*
 *	sha1.c
 *	Release $Name:  $
 *
 *	SHA1 hash implementation
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

#include "../cryptoLayer.h"

#define F0(x,y,z)	(z ^ (x & (y ^ z)))
#define F1(x,y,z)	(x ^ y ^ z)
#define F2(x,y,z)	((x & y) | (z & (x | y)))
#define F3(x,y,z)	(x ^ y ^ z)

#ifdef CLEAN_STACK
static void _sha1_compress(hash_state *md)
#else
static void sha1_compress(hash_state *md)
#endif /* CLEAN STACK */
{
	unsigned long a,b,c,d,e,W[80],i;

	sslAssert(md != NULL);

	/* copy the state into 512-bits into W[0..15] */
	for (i = 0; i < 16; i++) {
		LOAD32H(W[i], md->sha1.buf + (4*i));
	}

	/* copy state */
	a = md->sha1.state[0];
	b = md->sha1.state[1];
	c = md->sha1.state[2];
	d = md->sha1.state[3];
	e = md->sha1.state[4];

	/* expand it */
	for (i = 16; i < 80; i++) {
		W[i] = ROL(W[i-3] ^ W[i-8] ^ W[i-14] ^ W[i-16], 1); 
	}

	/* compress */
	/* round one */
	#define FF0(a,b,c,d,e,i) e = (ROL(a, 5) + F0(b,c,d) + e + W[i] + 0x5a827999UL); b = ROL(b, 30);
	#define FF1(a,b,c,d,e,i) e = (ROL(a, 5) + F1(b,c,d) + e + W[i] + 0x6ed9eba1UL); b = ROL(b, 30);
	#define FF2(a,b,c,d,e,i) e = (ROL(a, 5) + F2(b,c,d) + e + W[i] + 0x8f1bbcdcUL); b = ROL(b, 30);
	#define FF3(a,b,c,d,e,i) e = (ROL(a, 5) + F3(b,c,d) + e + W[i] + 0xca62c1d6UL); b = ROL(b, 30);

	for (i = 0; i < 20; ) {
		FF0(a,b,c,d,e,i++);
		FF0(e,a,b,c,d,i++);
		FF0(d,e,a,b,c,i++);
		FF0(c,d,e,a,b,i++);
		FF0(b,c,d,e,a,i++);
	}

	/* round two */
	for (i = 20; i < 40; ) {
		FF1(a,b,c,d,e,i++);
		FF1(e,a,b,c,d,i++);
		FF1(d,e,a,b,c,i++);
		FF1(c,d,e,a,b,i++);
		FF1(b,c,d,e,a,i++);
	}

	/* round three */
	for (i = 40; i < 60; ) {
		FF2(a,b,c,d,e,i++);
		FF2(e,a,b,c,d,i++);
		FF2(d,e,a,b,c,i++);
		FF2(c,d,e,a,b,i++);
		FF2(b,c,d,e,a,i++);
	}

	/* round four */
	for (i = 60; i < 80; ) {
		FF3(a,b,c,d,e,i++);
		FF3(e,a,b,c,d,i++);
		FF3(d,e,a,b,c,i++);
		FF3(c,d,e,a,b,i++);
		FF3(b,c,d,e,a,i++);
		}

	#undef FF0
	#undef FF1
	#undef FF2
	#undef FF3

	/* store */
	md->sha1.state[0] = md->sha1.state[0] + a;
	md->sha1.state[1] = md->sha1.state[1] + b;
	md->sha1.state[2] = md->sha1.state[2] + c;
	md->sha1.state[3] = md->sha1.state[3] + d;
	md->sha1.state[4] = md->sha1.state[4] + e;
}

#ifdef CLEAN_STACK
static void sha1_compress(hash_state *md)
{
	_sha1_compress(md);
	burn_stack(sizeof(unsigned long) * 87);
}
#endif /* CLEAN_STACK */

void matrixSha1Init(hash_state * md)
{
	sslAssert(md != NULL);
	md->sha1.state[0] = 0x67452301UL;
	md->sha1.state[1] = 0xefcdab89UL;
	md->sha1.state[2] = 0x98badcfeUL;
	md->sha1.state[3] = 0x10325476UL;
	md->sha1.state[4] = 0xc3d2e1f0UL;
	md->sha1.curlen = 0;
	md->sha1.length = 0;
}

void matrixSha1Update(hash_state * md, const unsigned char *buf, unsigned long len)
{
	unsigned long n;
	sslAssert(md != NULL);
	sslAssert(buf != NULL);

	while (len > 0) {
		n = MIN(len, (64 - md->sha1.curlen));
		memcpy(md->sha1.buf + md->sha1.curlen, buf, (size_t)n);
		md->sha1.curlen		+= n;
		buf					+= n;
		len					-= n;

		/* is 64 bytes full? */
		if (md->sha1.curlen == 64) {
			sha1_compress(md);
			md->sha1.length += 512;
			md->sha1.curlen = 0;
		}
	}
}

int matrixSha1Final(hash_state * md, unsigned char *hash)
{
	int i;

	sslAssert(md != NULL);
	if (hash == NULL) {
		return -1;
	}

	/* increase the length of the message */
	md->sha1.length += md->sha1.curlen * 8;

	/* append the '1' bit */
	md->sha1.buf[md->sha1.curlen++] = (unsigned char)0x80;

	/* if the length is currently above 56 bytes we append zeros
	 * then compress.  Then we can fall back to padding zeros and length
	 * encoding like normal.
	 */
	if (md->sha1.curlen > 56) {
		while (md->sha1.curlen < 64) {
			md->sha1.buf[md->sha1.curlen++] = (unsigned char)0;
		}
		sha1_compress(md);
		md->sha1.curlen = 0;
	}

	/* pad upto 56 bytes of zeroes */
	while (md->sha1.curlen < 56) {
		md->sha1.buf[md->sha1.curlen++] = (unsigned char)0;
	}

	/* store length */
	STORE64H(md->sha1.length, md->sha1.buf+56);
	sha1_compress(md);

	/* copy output */
	for (i = 0; i < 5; i++) {
		STORE32H(md->sha1.state[i], hash+(4*i));
	}
#ifdef CLEAN_STACK
	zeromem(md, sizeof(hash_state));
#endif /* CLEAN_STACK */
	return 20;
}

/******************************************************************************/

