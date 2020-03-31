/*
 *	cipherSuite.c
 *	Release $Name:  $
 *
 *	Wrappers for the various cipher suites.
 *	Contributors should add additional cipher suites here.
 *	Enable specific suites at compile time in matrixConfig.h
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

#include "matrixInternal.h"

/******************************************************************************/
/*
	Using void *ssl rather than ssl_t *ssl is a workaround for a chicken-egg
	issue in matrixInternal.h where these function prototypes are used
	before ssl_t is defined.
*/
#ifdef USE_SHA1_MAC
static int sha1GenerateMac(void *ssl, unsigned char type, char *data, 
							int len, char *mac);
static int sha1VerifyMac(void *ssl, unsigned char type, char *data, 
							int len, char *mac);
#endif

#ifdef USE_MD5_MAC
static int md5GenerateMac(void *ssl, unsigned char type, char *data, 
							int len, char *mac);
static int md5VerifyMac(void *ssl, unsigned char type, char *data, 
							int len, char *mac);
#endif

#ifdef USE_SSL_RSA_WITH_RC4_128_MD5
static int matrixCipher4Init(sslSec_t *sec);
#endif /* USE_SSL_RSA_WITH_RC4_128_MD5 */

#ifdef USE_SSL_RSA_WITH_RC4_128_SHA
static int matrixCipher5Init(sslSec_t *sec);
#endif /* USE_SSL_RSA_WITH_RC4_128_SHA */

#ifdef USE_SSL_RSA_WITH_3DES_EDE_CBC_SHA
static int matrixCipherAInit(sslSec_t *sec);
#endif /* USE_SSL_RSA_WITH_3DES_EDE_CBC_SHA */

static int nullInit(sslSec_t *sec);
static int nullEncrypt(sslCipherContext_t *ctx, char *in, char *out, int len);
static int nullDecrypt(sslCipherContext_t *ctx, char *in, char *out, int len);
static int nullEncryptPub(sslRsaKey_t *key, char *in, int inlen, char *out, 
						  int outlen);
static int nullDecryptPriv(sslRsaKey_t *key, char *in, int inlen, char *out, 
						   int outlen);
static int nullGenerateMac(void *ssl, unsigned char type, char *data, 
						   int len, char *mac);
static int nullVerifyMac(void *ssl, unsigned char type, char *data, 
						 int len, char *mac);

/******************************************************************************/

static sslCipherSpec_t	supportedCiphers[] = {
/*
	New ciphers should be added here, similar to the ones below
	These ciphers should be in order of the most desireable to the 
	least desireable ciphers to negotiate.
*/
#ifdef USE_SSL_RSA_WITH_3DES_EDE_CBC_SHA
	{SSL_RSA_WITH_3DES_EDE_CBC_SHA,
		20,			/* macSize */
		24,			/* keySize */
		8,			/* ivSize */
		8,			/* blocksize */
		matrixCipherAInit,
		matrix3desEncrypt, 
		matrix3desDecrypt, 
		matrixRsaEncryptPub, 
		matrixRsaDecryptPriv, 
		sha1GenerateMac, 
		sha1VerifyMac},
#endif /* USE_SSL_RSA_WITH_3DES_EDE_CBC_SHA */
#ifdef USE_SSL_RSA_WITH_RC4_128_SHA
	{SSL_RSA_WITH_RC4_128_SHA,
		20,			/* macSize */
		16,			/* keySize */
		0,			/* ivSize */
		1,			/* blocksize */
		matrixCipher5Init,
		matrixArc4, 
		matrixArc4, 
		matrixRsaEncryptPub, 
		matrixRsaDecryptPriv, 
		sha1GenerateMac, 
		sha1VerifyMac},
#endif /* USE_SSL_RSA_WITH_RC4_128_SHA */
#ifdef USE_SSL_RSA_WITH_RC4_128_MD5
	{SSL_RSA_WITH_RC4_128_MD5,
		16,			/* macSize */
		16,			/* keySize */
		0,			/* ivSize */
		1,			/* blocksize */
		matrixCipher4Init,
		matrixArc4, 
		matrixArc4, 
		matrixRsaEncryptPub, 
		matrixRsaDecryptPriv, 
		md5GenerateMac, 
		md5VerifyMac},
#endif /* USE_SSL_RSA_WITH_RC4_128_MD5 */
/*
	The NULL Cipher suite must exist and be the last in this list
*/
	{SSL_NULL_WITH_NULL_NULL,
		0,
		0,
		0,
		0,
		nullInit,
		nullEncrypt, 
		nullDecrypt, 
		nullEncryptPub, 
		nullDecryptPriv, 
		nullGenerateMac, 
		nullVerifyMac}
};

/******************************************************************************/
/*
	Lookup the given cipher spec ID and return a pointer to the structure
	if found.  This is used when negotiating security, to find out what suites
	we support.
*/
sslCipherSpec_t *sslGetCipherSpec(int id)
{
	int		i;

	i = 0;
	do {
		if (supportedCiphers[i].id == id) {
			return &supportedCiphers[i];
		}
	} while (supportedCiphers[i++].id != SSL_NULL_WITH_NULL_NULL) ;

	return NULL;
}

/******************************************************************************/
/*
	Write out a list of the supported cipher suites to the caller's buffer
	First 2 bytes are the number of cipher suite bytes, the remaining bytes are
	the cipher suites, as two byte, network byte order values.
*/
int sslGetCipherSpecList(unsigned char *c, int len)
{
	unsigned char	*end, *p;
	unsigned short	i;

	if (len < 4) {
		return -1;
	}
	end = c + len;
	p = c; c += 2;
	for (i = 0; supportedCiphers[i].id != SSL_NULL_WITH_NULL_NULL; i++) {
		if (end - c < 2) {
			return -1;
		}
		*c = (supportedCiphers[i].id & 0xFF00) >> 8; c++;
		*c = supportedCiphers[i].id & 0xFF; c++;
	}
	i *= 2;
	*p = (unsigned char)(i >> 8); p++;
	*p = (unsigned char)(i & 0xFF);
	return i + 2;
}

/******************************************************************************/
/*
	Return the length of the cipher spec list, including initial length bytes
*/
int sslGetCipherSpecListLen()
{
	int		i;

	for (i = 0; supportedCiphers[i].id != SSL_NULL_WITH_NULL_NULL; i++) {
	}
	return (i * 2) + 2;
}

/******************************************************************************/
/*
*/
#ifdef USE_SHA1_MAC
static int sha1GenerateMac(void *sslv, unsigned char type, char *data, 
							int len, char *mac)
{
	ssl_t	*ssl = (ssl_t*)sslv;

	return ssl3HMACSha1(ssl->sec.writeMAC, ssl->sec.seq, type, data, len, mac);
}

static int sha1VerifyMac(void *sslv, unsigned char type, char *data, int len, 
				  char *mac)
{
	ssl_t	*ssl = (ssl_t*)sslv;
	char	buf[SSL_SHA1_HASH_SIZE];

	ssl3HMACSha1(ssl->sec.readMAC, ssl->sec.remSeq, type, data, len, buf);

	if (memcmp(buf, mac, SSL_SHA1_HASH_SIZE) == 0) {
		return 0;
	}
	return -1;
}
#endif /* USE_SHA1_MAC */

/******************************************************************************/
/*
*/
#ifdef USE_MD5_MAC
static int md5GenerateMac(void *sslv, unsigned char type, char *data, int len, 
					char *mac)
{
	ssl_t	*ssl = (ssl_t*)sslv;

	return ssl3HMACMd5(ssl->sec.writeMAC, ssl->sec.seq, type, data, len, mac);
}

static int md5VerifyMac(void *sslv, unsigned char type, char *data, int len, 
				  char *mac)
{
	ssl_t	*ssl = (ssl_t*)sslv;
	char	buf[SSL_MD5_HASH_SIZE];

	ssl3HMACMd5(ssl->sec.readMAC, ssl->sec.remSeq, type, data, len, buf);

	if (memcmp(buf, mac, SSL_MD5_HASH_SIZE) == 0) {
		return 0;
	}
	return -1;
}

#endif /* USE_MD5_MAC */

/******************************************************************************/
/*
	SSL_RSA_WITH_RC4_128_SHA cipher init
*/
#ifdef USE_SSL_RSA_WITH_RC4_128_MD5
static int matrixCipher4Init(sslSec_t *sec)
{
	matrixArc4Init(&(sec->encryptCtx), sec->writeKey, 16);
	matrixArc4Init(&(sec->decryptCtx), sec->readKey, 16);

	return 0;
}
#endif /* USE_SSL_RSA_WITH_RC4_128_MD5 */

/******************************************************************************/
/*
	SSL_RSA_WITH_RC4_128_SHA cipher init
*/
#ifdef USE_SSL_RSA_WITH_RC4_128_SHA
static int matrixCipher5Init(sslSec_t *sec)
{
	matrixArc4Init(&(sec->encryptCtx), sec->writeKey, 16);
	matrixArc4Init(&(sec->decryptCtx), sec->readKey, 16);

	return 0;
}
#endif /* USE_SSL_RSA_WITH_RC4_128_SHA */

/******************************************************************************/
/*
	SSL_RSA_WITH_3DES_EDE_CBC_SHA cipher init
*/
#ifdef USE_SSL_RSA_WITH_3DES_EDE_CBC_SHA
static int matrixCipherAInit(sslSec_t *sec)
{
	if (matrix3desInit(&(sec->encryptCtx), sec->writeIV, sec->writeKey, 
		SSL_DES3_KEY_LEN) < 0) {
		return -1;
	}
	if (matrix3desInit(&(sec->decryptCtx), sec->readIV, sec->readKey, 
		SSL_DES3_KEY_LEN) < 0) {
		return -1;
	}
	return 0;
}
#endif /* USE_SSL_RSA_WITH_3DES_EDE_CBC_SHA */

/******************************************************************************/
/*
	SSL_NULL_WITH_NULL_NULL cipher functions
	Used in handshaking before SSL_RECORD_TYPE_CHANGE_CIPHER_SPEC message
	FUTURE - remove the memcpy to support in-situ decryption
*/
static int nullInit(sslSec_t *sec)
{
	return 0;
}

static int nullEncrypt(sslCipherContext_t *ctx, char *in, char *out, int len)
{
	memcpy(out, in, len);
	return len;
}

static int nullDecrypt(sslCipherContext_t *ctx, char *in, char *out, int len)
{
	memcpy(out, in, len);
	return len;
}

static int nullEncryptPub(sslRsaKey_t *key, char *in, int inlen, char *out, 
						  int outlen)
{
	if (inlen <= outlen) {
		matrixStrDebugMsg("Error: output buffer too small for NULL encrypt\n",
			NULL);
		return -1;
	}
	memcpy(out, in, inlen);
	return inlen;
}

static int nullDecryptPriv(sslRsaKey_t *key, char *in, int inlen, char *out, 
						   int outlen)
{
	if (inlen <= outlen) {
		matrixStrDebugMsg("Error: output buffer too small for NULL decrypt\n",
			NULL);
		return -1;
	}
	memcpy(out, in, inlen);
	return inlen;
}

static int nullGenerateMac(void *ssl, unsigned char type, char *data, 
						   int len, char *mac)
{
	return 0;
}

static int nullVerifyMac(void *ssl, unsigned char type, char *data, 
						 int len, char *mac)
{
	return 0;
}

/******************************************************************************/
