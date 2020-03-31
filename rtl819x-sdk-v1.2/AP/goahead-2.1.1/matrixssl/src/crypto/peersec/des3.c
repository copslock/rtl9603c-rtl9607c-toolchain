/*
 *	des3.c
 *	Release $Name:  $
 *
 *	3DES block cipher implementation for low memory usage
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

#ifdef USE_3DES

#define EN0 0 
#define DE1 1

static const ulong32 bytebit[8] =
{
	0200, 0100, 040, 020, 010, 04, 02, 01 
};

static const ulong32 bigbyte[24] =
{
    0x800000UL,  0x400000UL,  0x200000UL,  0x100000UL,
    0x80000UL,   0x40000UL,   0x20000UL,   0x10000UL,
    0x8000UL,    0x4000UL,    0x2000UL,    0x1000UL,
    0x800UL,     0x400UL,     0x200UL,     0x100UL,
    0x80UL,      0x40UL,      0x20UL,      0x10UL,
    0x8UL,       0x4UL,       0x2UL,       0x1L 
};

static const unsigned char pc1[56] = {
    56, 48, 40, 32, 24, 16,  8,  0, 57, 49, 41, 33, 25, 17,  
     9,  1, 58, 50, 42, 34, 26, 18, 10,  2, 59, 51, 43, 35, 
    62, 54, 46, 38, 30, 22, 14,  6, 61, 53, 45, 37, 29, 21,
    13,  5, 60, 52, 44, 36, 28, 20, 12,  4, 27, 19, 11,  3 
};

static const unsigned char pc2[48] = {
    13, 16, 10, 23,  0,  4,      2, 27, 14,  5, 20,  9,
    22, 18, 11,  3, 25,  7,     15,  6, 26, 19, 12,  1,
    40, 51, 30, 36, 46, 54,     29, 39, 50, 44, 32, 47,
    43, 48, 38, 55, 33, 52,     45, 41, 49, 35, 28, 31
};

static const unsigned char totrot[16] = {
    1,   2,  4,  6,
    8,  10, 12, 14, 
    15, 17, 19, 21, 
    23, 25, 27, 28
};

static const ulong32 SP1[64] =
{
    0x01010400UL, 0x00000000UL, 0x00010000UL, 0x01010404UL,
    0x01010004UL, 0x00010404UL, 0x00000004UL, 0x00010000UL,
    0x00000400UL, 0x01010400UL, 0x01010404UL, 0x00000400UL,
    0x01000404UL, 0x01010004UL, 0x01000000UL, 0x00000004UL,
    0x00000404UL, 0x01000400UL, 0x01000400UL, 0x00010400UL,
    0x00010400UL, 0x01010000UL, 0x01010000UL, 0x01000404UL,
    0x00010004UL, 0x01000004UL, 0x01000004UL, 0x00010004UL,
    0x00000000UL, 0x00000404UL, 0x00010404UL, 0x01000000UL,
    0x00010000UL, 0x01010404UL, 0x00000004UL, 0x01010000UL,
    0x01010400UL, 0x01000000UL, 0x01000000UL, 0x00000400UL,
    0x01010004UL, 0x00010000UL, 0x00010400UL, 0x01000004UL,
    0x00000400UL, 0x00000004UL, 0x01000404UL, 0x00010404UL,
    0x01010404UL, 0x00010004UL, 0x01010000UL, 0x01000404UL,
    0x01000004UL, 0x00000404UL, 0x00010404UL, 0x01010400UL,
    0x00000404UL, 0x01000400UL, 0x01000400UL, 0x00000000UL,
    0x00010004UL, 0x00010400UL, 0x00000000UL, 0x01010004UL
};

static const ulong32 SP2[64] =
{
    0x80108020UL, 0x80008000UL, 0x00008000UL, 0x00108020UL,
    0x00100000UL, 0x00000020UL, 0x80100020UL, 0x80008020UL,
    0x80000020UL, 0x80108020UL, 0x80108000UL, 0x80000000UL,
    0x80008000UL, 0x00100000UL, 0x00000020UL, 0x80100020UL,
    0x00108000UL, 0x00100020UL, 0x80008020UL, 0x00000000UL,
    0x80000000UL, 0x00008000UL, 0x00108020UL, 0x80100000UL,
    0x00100020UL, 0x80000020UL, 0x00000000UL, 0x00108000UL,
    0x00008020UL, 0x80108000UL, 0x80100000UL, 0x00008020UL,
    0x00000000UL, 0x00108020UL, 0x80100020UL, 0x00100000UL,
    0x80008020UL, 0x80100000UL, 0x80108000UL, 0x00008000UL,
    0x80100000UL, 0x80008000UL, 0x00000020UL, 0x80108020UL,
    0x00108020UL, 0x00000020UL, 0x00008000UL, 0x80000000UL,
    0x00008020UL, 0x80108000UL, 0x00100000UL, 0x80000020UL,
    0x00100020UL, 0x80008020UL, 0x80000020UL, 0x00100020UL,
    0x00108000UL, 0x00000000UL, 0x80008000UL, 0x00008020UL,
    0x80000000UL, 0x80100020UL, 0x80108020UL, 0x00108000UL
};

static const ulong32 SP3[64] =
{
    0x00000208UL, 0x08020200UL, 0x00000000UL, 0x08020008UL,
    0x08000200UL, 0x00000000UL, 0x00020208UL, 0x08000200UL,
    0x00020008UL, 0x08000008UL, 0x08000008UL, 0x00020000UL,
    0x08020208UL, 0x00020008UL, 0x08020000UL, 0x00000208UL,
    0x08000000UL, 0x00000008UL, 0x08020200UL, 0x00000200UL,
    0x00020200UL, 0x08020000UL, 0x08020008UL, 0x00020208UL,
    0x08000208UL, 0x00020200UL, 0x00020000UL, 0x08000208UL,
    0x00000008UL, 0x08020208UL, 0x00000200UL, 0x08000000UL,
    0x08020200UL, 0x08000000UL, 0x00020008UL, 0x00000208UL,
    0x00020000UL, 0x08020200UL, 0x08000200UL, 0x00000000UL,
    0x00000200UL, 0x00020008UL, 0x08020208UL, 0x08000200UL,
    0x08000008UL, 0x00000200UL, 0x00000000UL, 0x08020008UL,
    0x08000208UL, 0x00020000UL, 0x08000000UL, 0x08020208UL,
    0x00000008UL, 0x00020208UL, 0x00020200UL, 0x08000008UL,
    0x08020000UL, 0x08000208UL, 0x00000208UL, 0x08020000UL,
    0x00020208UL, 0x00000008UL, 0x08020008UL, 0x00020200UL
};

static const ulong32 SP4[64] =
{
    0x00802001UL, 0x00002081UL, 0x00002081UL, 0x00000080UL,
    0x00802080UL, 0x00800081UL, 0x00800001UL, 0x00002001UL,
    0x00000000UL, 0x00802000UL, 0x00802000UL, 0x00802081UL,
    0x00000081UL, 0x00000000UL, 0x00800080UL, 0x00800001UL,
    0x00000001UL, 0x00002000UL, 0x00800000UL, 0x00802001UL,
    0x00000080UL, 0x00800000UL, 0x00002001UL, 0x00002080UL,
    0x00800081UL, 0x00000001UL, 0x00002080UL, 0x00800080UL,
    0x00002000UL, 0x00802080UL, 0x00802081UL, 0x00000081UL,
    0x00800080UL, 0x00800001UL, 0x00802000UL, 0x00802081UL,
    0x00000081UL, 0x00000000UL, 0x00000000UL, 0x00802000UL,
    0x00002080UL, 0x00800080UL, 0x00800081UL, 0x00000001UL,
    0x00802001UL, 0x00002081UL, 0x00002081UL, 0x00000080UL,
    0x00802081UL, 0x00000081UL, 0x00000001UL, 0x00002000UL,
    0x00800001UL, 0x00002001UL, 0x00802080UL, 0x00800081UL,
    0x00002001UL, 0x00002080UL, 0x00800000UL, 0x00802001UL,
    0x00000080UL, 0x00800000UL, 0x00002000UL, 0x00802080UL
};

static const ulong32 SP5[64] =
{
    0x00000100UL, 0x02080100UL, 0x02080000UL, 0x42000100UL,
    0x00080000UL, 0x00000100UL, 0x40000000UL, 0x02080000UL,
    0x40080100UL, 0x00080000UL, 0x02000100UL, 0x40080100UL,
    0x42000100UL, 0x42080000UL, 0x00080100UL, 0x40000000UL,
    0x02000000UL, 0x40080000UL, 0x40080000UL, 0x00000000UL,
    0x40000100UL, 0x42080100UL, 0x42080100UL, 0x02000100UL,
    0x42080000UL, 0x40000100UL, 0x00000000UL, 0x42000000UL,
    0x02080100UL, 0x02000000UL, 0x42000000UL, 0x00080100UL,
    0x00080000UL, 0x42000100UL, 0x00000100UL, 0x02000000UL,
    0x40000000UL, 0x02080000UL, 0x42000100UL, 0x40080100UL,
    0x02000100UL, 0x40000000UL, 0x42080000UL, 0x02080100UL,
    0x40080100UL, 0x00000100UL, 0x02000000UL, 0x42080000UL,
    0x42080100UL, 0x00080100UL, 0x42000000UL, 0x42080100UL,
    0x02080000UL, 0x00000000UL, 0x40080000UL, 0x42000000UL,
    0x00080100UL, 0x02000100UL, 0x40000100UL, 0x00080000UL,
    0x00000000UL, 0x40080000UL, 0x02080100UL, 0x40000100UL
};

static const ulong32 SP6[64] =
{
    0x20000010UL, 0x20400000UL, 0x00004000UL, 0x20404010UL,
    0x20400000UL, 0x00000010UL, 0x20404010UL, 0x00400000UL,
    0x20004000UL, 0x00404010UL, 0x00400000UL, 0x20000010UL,
    0x00400010UL, 0x20004000UL, 0x20000000UL, 0x00004010UL,
    0x00000000UL, 0x00400010UL, 0x20004010UL, 0x00004000UL,
    0x00404000UL, 0x20004010UL, 0x00000010UL, 0x20400010UL,
    0x20400010UL, 0x00000000UL, 0x00404010UL, 0x20404000UL,
    0x00004010UL, 0x00404000UL, 0x20404000UL, 0x20000000UL,
    0x20004000UL, 0x00000010UL, 0x20400010UL, 0x00404000UL,
    0x20404010UL, 0x00400000UL, 0x00004010UL, 0x20000010UL,
    0x00400000UL, 0x20004000UL, 0x20000000UL, 0x00004010UL,
    0x20000010UL, 0x20404010UL, 0x00404000UL, 0x20400000UL,
    0x00404010UL, 0x20404000UL, 0x00000000UL, 0x20400010UL,
    0x00000010UL, 0x00004000UL, 0x20400000UL, 0x00404010UL,
    0x00004000UL, 0x00400010UL, 0x20004010UL, 0x00000000UL,
    0x20404000UL, 0x20000000UL, 0x00400010UL, 0x20004010UL
};

static const ulong32 SP7[64] =
{
    0x00200000UL, 0x04200002UL, 0x04000802UL, 0x00000000UL,
    0x00000800UL, 0x04000802UL, 0x00200802UL, 0x04200800UL,
    0x04200802UL, 0x00200000UL, 0x00000000UL, 0x04000002UL,
    0x00000002UL, 0x04000000UL, 0x04200002UL, 0x00000802UL,
    0x04000800UL, 0x00200802UL, 0x00200002UL, 0x04000800UL,
    0x04000002UL, 0x04200000UL, 0x04200800UL, 0x00200002UL,
    0x04200000UL, 0x00000800UL, 0x00000802UL, 0x04200802UL,
    0x00200800UL, 0x00000002UL, 0x04000000UL, 0x00200800UL,
    0x04000000UL, 0x00200800UL, 0x00200000UL, 0x04000802UL,
    0x04000802UL, 0x04200002UL, 0x04200002UL, 0x00000002UL,
    0x00200002UL, 0x04000000UL, 0x04000800UL, 0x00200000UL,
    0x04200800UL, 0x00000802UL, 0x00200802UL, 0x04200800UL,
    0x00000802UL, 0x04000002UL, 0x04200802UL, 0x04200000UL,
    0x00200800UL, 0x00000000UL, 0x00000002UL, 0x04200802UL,
    0x00000000UL, 0x00200802UL, 0x04200000UL, 0x00000800UL,
    0x04000002UL, 0x04000800UL, 0x00000800UL, 0x00200002UL
};

static const ulong32 SP8[64] =
{
    0x10001040UL, 0x00001000UL, 0x00040000UL, 0x10041040UL,
    0x10000000UL, 0x10001040UL, 0x00000040UL, 0x10000000UL,
    0x00040040UL, 0x10040000UL, 0x10041040UL, 0x00041000UL,
    0x10041000UL, 0x00041040UL, 0x00001000UL, 0x00000040UL,
    0x10040000UL, 0x10000040UL, 0x10001000UL, 0x00001040UL,
    0x00041000UL, 0x00040040UL, 0x10040040UL, 0x10041000UL,
    0x00001040UL, 0x00000000UL, 0x00000000UL, 0x10040040UL,
    0x10000040UL, 0x10001000UL, 0x00041040UL, 0x00040000UL,
    0x00041040UL, 0x00040000UL, 0x10041000UL, 0x00001000UL,
    0x00000040UL, 0x10040040UL, 0x00001000UL, 0x00041040UL,
    0x10001000UL, 0x00000040UL, 0x10000040UL, 0x10040000UL,
    0x10040040UL, 0x10000000UL, 0x00040000UL, 0x10001040UL,
    0x00000000UL, 0x10041040UL, 0x00040040UL, 0x10000040UL,
    0x10040000UL, 0x10001000UL, 0x10001040UL, 0x00000000UL,
    0x10041040UL, 0x00041000UL, 0x00041000UL, 0x00001040UL,
    0x00001040UL, 0x00040040UL, 0x10000000UL, 0x10041000UL
};

static void cookey(const ulong32 *raw1, ulong32 *keyout);
static void deskey(const unsigned char *key, short edf, ulong32 *keyout);
static int des3_setup(const unsigned char *key, int keylen, int num_rounds, 
					  des3_CBC *skey);
static void des3_ecb_encrypt(const unsigned char *pt, unsigned char *ct, 
							 des3_CBC *key);
static void des3_ecb_decrypt(const unsigned char *ct, unsigned char *pt, 
							 des3_CBC *key);

/******************************************************************************/
/*
	Init the 3DES block cipher context for EDE mode.
	IV should point to 8 bytes of initialization vector
	Key should point to 24 bytes of data
*/
int matrix3desInit(sslCipherContext_t *ctx, char *IV, char *key, int keylen)
{
	int x, err;

	if (IV == NULL || key == NULL || ctx == NULL || keylen != SSL_DES3_KEY_LEN){
		return -1;
	}
/*
	setup cipher
 */
	if ((err = des3_setup(key, keylen, 0, &ctx->des3)) != CRYPT_OK) {
		return -1;
	}
/*
	copy IV
 */
	ctx->des3.blocklen = SSL_DES3_IV_LEN;
	for (x = 0; x < ctx->des3.blocklen; x++) {
		ctx->des3.IV[x] = IV[x];
	}
	return 0;
}

/******************************************************************************/
/*
	Encrypt a buffer using 3DES-EDE-CBC
	(Encrypt Decrypt Encrypt and Cipher Block Chaining)
	len must be a multiple of blockLen (8 bytes)
*/
int matrix3desEncrypt(sslCipherContext_t *ctx, char *pt, char *ct, int len)
{
	int x, i;
	unsigned char tmp[MAXBLOCKSIZE];

	if (pt == NULL || ct == NULL || ctx == NULL || (len & 0x7) != 0) {
		return -1;
	}

	/* is blocklen valid? */
	if (ctx->des3.blocklen < 0 || ctx->des3.blocklen > 
		(int)sizeof(ctx->des3.IV)) {
		return -1;
	}

	for (i = 0; i < len; i += ctx->des3.blocklen) {
		/* xor IV against plaintext */
		for (x = 0; x < ctx->des3.blocklen; x++) {
			tmp[x] = pt[x] ^ ctx->des3.IV[x];
		}
		/* encrypt */
		des3_ecb_encrypt(tmp, ct, &ctx->des3);

		/* store IV [ciphertext] for a future block */
		for (x = 0; x < ctx->des3.blocklen; x++) {
			ctx->des3.IV[x] = ct[x];
		}
		ct += ctx->des3.blocklen;
		pt += ctx->des3.blocklen;
	}

#ifdef CLEAN_STACK
	zeromem(tmp, sizeof(tmp));
#endif
	return len;
}

/******************************************************************************/
/*
	Decrypt a buffer using 3DES-EDE-CBC
	(Encrypt Decrypt Encrypt and Cipher Block Chaining)
	len must be a multiple of blockLen (8 bytes)
*/
int matrix3desDecrypt(sslCipherContext_t *ctx, char *ct, char *pt, int len)
{
	int x, i;
	unsigned char tmp[MAXBLOCKSIZE], tmp2[MAXBLOCKSIZE];

	if (pt == NULL || ct == NULL || ctx == NULL || (len & 0x7) != 0) {
		return -1;
	}

	/* is blocklen valid? */
	if (ctx->des3.blocklen < 0 || ctx->des3.blocklen > 
		(int)sizeof(ctx->des3.IV)) {
		return -1;
	}
	for (i = 0; i < len; i += ctx->des3.blocklen) {
		/* decrypt the block from ct into tmp */
		des3_ecb_decrypt(ct, tmp, &ctx->des3);
		/* xor IV against the plaintext of the previous step */
		for (x = 0; x < ctx->des3.blocklen; x++) { 
			/* copy CT in case ct == pt */
			tmp2[x] = ct[x]; 
			/* actually decrypt the byte */
			pt[x] = tmp[x] ^ ctx->des3.IV[x]; 
		}
		/* replace IV with this current ciphertext */
		for (x = 0; x < ctx->des3.blocklen; x++) {
			ctx->des3.IV[x] = tmp2[x];
		}
		ct += ctx->des3.blocklen;
		pt += ctx->des3.blocklen;
	}
#ifdef CLEAN_STACK
	zeromem(tmp, sizeof(tmp));
	zeromem(tmp2, sizeof(tmp2));
#endif
	return len;
}

/******************************************************************************/
/*
	3DES implementation below
*/
static void cookey(const ulong32 *raw1, ulong32 *keyout)
{
	ulong32			*cook;
	const ulong32	*raw0;
	ulong32			dough[32];
	int				i;

	cook = dough;
	for(i=0; i < 16; i++, raw1++) {
		raw0 = raw1++;
		*cook    = (*raw0 & 0x00fc0000L) << 6;
		*cook   |= (*raw0 & 0x00000fc0L) << 10;
		*cook   |= (*raw1 & 0x00fc0000L) >> 10;
		*cook++ |= (*raw1 & 0x00000fc0L) >> 6;
		*cook    = (*raw0 & 0x0003f000L) << 12;
		*cook   |= (*raw0 & 0x0000003fL) << 16;
		*cook   |= (*raw1 & 0x0003f000L) >> 4;
		*cook++ |= (*raw1 & 0x0000003fL);
	}

	memcpy(keyout, dough, sizeof dough);
	burn_stack(sizeof(ulong32 *) * 2 + sizeof(ulong32)*32 + sizeof(int));
}


static void deskey(const unsigned char *key, short edf, ulong32 *keyout)
{
	ulong32				i, j, l, m, n, kn[32];
	unsigned char		pc1m[56], pcr[56];

	for (j=0; j < 56; j++) {
		l = (ulong32)pc1[j];
		m = l & 7;
		pc1m[j] = (unsigned char)((key[l >> 3U] & bytebit[m]) == 
			bytebit[m] ? 1 : 0);
	}

	for (i=0; i < 16; i++) {
		if (edf == DE1) {
			m = (15 - i) << 1;
		} else {
			m = i << 1;
		}
		n = m + 1;
		kn[m] = kn[n] = 0L;
		for (j=0; j < 28; j++) {
			l = j + (ulong32)totrot[i];
			if (l < 28) {
				pcr[j] = pc1m[l];
			} else {
				pcr[j] = pc1m[l - 28];
			}
		}
		for (/*j = 28*/; j < 56; j++) {
			l = j + (ulong32)totrot[i];
			if (l < 56) {
				pcr[j] = pc1m[l];
			} else {
				pcr[j] = pc1m[l - 28];
			}
		}
		for (j=0; j < 24; j++)  {
			if ((int)pcr[(int)pc2[j]] != 0) {
				kn[m] |= bigbyte[j];
			}
			if ((int)pcr[(int)pc2[j+24]] != 0) {
				kn[n] |= bigbyte[j];
			}
		}
	}
	cookey(kn, keyout);
	burn_stack(sizeof(int)*5 + sizeof(ulong32)*32 + 
		sizeof(unsigned char)*112);
}

static void desfunc(ulong32 *block, const ulong32 *keys)
{
	ulong32 work, right, leftt;
	int round;

	leftt = block[0];
	right = block[1];

#ifdef SMALL_CODE
	work = ((leftt >> 4)  ^ right) & 0x0f0f0f0fL;
	right ^= work;
	leftt ^= (work << 4);

	work = ((leftt >> 16) ^ right) & 0x0000ffffL;
	right ^= work;
	leftt ^= (work << 16);

	work = ((right >> 2)  ^ leftt) & 0x33333333L;
	leftt ^= work;
	right ^= (work << 2);

	work = ((right >> 8)  ^ leftt) & 0x00ff00ffL;
	leftt ^= work;
	right ^= (work << 8);

	right = ROL(right, 1);
	work = (leftt ^ right) & 0xaaaaaaaaL;

	leftt ^= work;
	right ^= work;
	leftt = ROL(leftt, 1);
#else
{
	ulong64 tmp;
	tmp = des_ip[0][byte(leftt, 0)] ^
			des_ip[1][byte(leftt, 1)] ^
			des_ip[2][byte(leftt, 2)] ^
			des_ip[3][byte(leftt, 3)] ^
			des_ip[4][byte(right, 0)] ^
			des_ip[5][byte(right, 1)] ^
			des_ip[6][byte(right, 2)] ^
			des_ip[7][byte(right, 3)];
	leftt = (ulong32)(tmp >> 32);
	right = (ulong32)(tmp & 0xFFFFFFFFUL);
}
#endif

	for (round = 0; round < 8; round++) {
		work  = ROR(right, 4) ^ *keys++;
		leftt ^= SP7[work        & 0x3fL]
              ^ SP5[(work >>  8) & 0x3fL]
              ^ SP3[(work >> 16) & 0x3fL]
              ^ SP1[(work >> 24) & 0x3fL];
        work  = right ^ *keys++;
        leftt ^= SP8[ work        & 0x3fL]
              ^  SP6[(work >>  8) & 0x3fL]
              ^  SP4[(work >> 16) & 0x3fL]
              ^  SP2[(work >> 24) & 0x3fL];

        work = ROR(leftt, 4) ^ *keys++;
        right ^= SP7[ work        & 0x3fL]
              ^  SP5[(work >>  8) & 0x3fL]
              ^  SP3[(work >> 16) & 0x3fL]
              ^  SP1[(work >> 24) & 0x3fL];
        work  = leftt ^ *keys++;
        right ^= SP8[ work        & 0x3fL]
              ^  SP6[(work >>  8) & 0x3fL]
              ^  SP4[(work >> 16) & 0x3fL]
              ^  SP2[(work >> 24) & 0x3fL];
    }

#ifdef SMALL_CODE    
	right = ROR(right, 1);
	work = (leftt ^ right) & 0xaaaaaaaaL;
	leftt ^= work;
	right ^= work;
	leftt = ROR(leftt, 1);
	work = ((leftt >> 8) ^ right) & 0x00ff00ffL;
	right ^= work;
	leftt ^= (work << 8);
	
	work = ((leftt >> 2) ^ right) & 0x33333333L;
	right ^= work;
	leftt ^= (work << 2);
	work = ((right >> 16) ^ leftt) & 0x0000ffffL;
	leftt ^= work;
	right ^= (work << 16);
	work = ((right >> 4) ^ leftt) & 0x0f0f0f0fL;
	leftt ^= work;
	right ^= (work << 4);
#else 
   {
      ulong64 tmp;
      tmp = des_fp[0][byte(leftt, 0)] ^
            des_fp[1][byte(leftt, 1)] ^
            des_fp[2][byte(leftt, 2)] ^
            des_fp[3][byte(leftt, 3)] ^
            des_fp[4][byte(right, 0)] ^
            des_fp[5][byte(right, 1)] ^
            des_fp[6][byte(right, 2)] ^
            des_fp[7][byte(right, 3)];
      leftt = (ulong32)(tmp >> 32);
      right = (ulong32)(tmp & 0xFFFFFFFFUL);
   }
#endif
    
    block[0] = right;
    block[1] = leftt;
	burn_stack(sizeof(ulong32) * 4 + sizeof(int));
}

static int des3_setup(const unsigned char *key, int keylen, int num_rounds, 
					  des3_CBC *skey)
{
	if (key == NULL || skey == NULL) {
		return -1;
	}

	if( num_rounds != 0 && num_rounds != 16) {
		return CRYPT_INVALID_ROUNDS;
	}

	if (keylen != 24) {
		return CRYPT_INVALID_KEYSIZE;
	}

	deskey(key,    EN0, skey->key.ek[0]);
	deskey(key+8,  DE1, skey->key.ek[1]);
	deskey(key+16, EN0, skey->key.ek[2]);

	deskey(key,    DE1, skey->key.dk[2]);
	deskey(key+8,  EN0, skey->key.dk[1]);
	deskey(key+16, DE1, skey->key.dk[0]);

	return CRYPT_OK;
}

static void des3_ecb_encrypt(const unsigned char *pt, unsigned char *ct, 
							 des3_CBC *key)
{
    ulong32 work[2];
    
    LOAD32H(work[0], pt+0);
    LOAD32H(work[1], pt+4);
    desfunc(work, key->key.ek[0]);
    desfunc(work, key->key.ek[1]);
    desfunc(work, key->key.ek[2]);
    STORE32H(work[0],ct+0);
    STORE32H(work[1],ct+4);
}

static void des3_ecb_decrypt(const unsigned char *ct, unsigned char *pt, 
							 des3_CBC *key)
{
	ulong32 work[2];

	LOAD32H(work[0], ct+0);
	LOAD32H(work[1], ct+4);
	desfunc(work, key->key.dk[0]);
	desfunc(work, key->key.dk[1]);
	desfunc(work, key->key.dk[2]);
	STORE32H(work[0],pt+0);
	STORE32H(work[1],pt+4);
}

#endif /* USE_3DES */

/******************************************************************************/

