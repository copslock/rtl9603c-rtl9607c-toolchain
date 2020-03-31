/*
 *	cryptoLayer.h
 *	Release $Name:  $
 *
 *	Cryptography provider layered header.  This layer decouples
 *	the cryptography implementation from the SSL protocol implementation.
 *	Contributors adding new providers must implement all functions 
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

#ifndef _h_CRYPTO_LAYER
#define _h_CRYPTO_LAYER

/*
	Define the default crypto provider here
*/
#define	USE_PEERSEC_CRYPTO

#include "../matrixConfig.h"

#ifdef __cplusplus
extern "C" {
#endif


#define SSL_MD5_HASH_SIZE		16
#define SSL_SHA1_HASH_SIZE		20

/*
	Enable the algorithms used for each cipher suite
*/
#ifdef USE_SSL_RSA_WITH_RC4_128_SHA
#define USE_ARC4
#define USE_SHA1_MAC
#define USE_RSA
#endif

#ifdef USE_SSL_RSA_WITH_RC4_128_MD5
#define USE_ARC4
#define USE_MD5_MAC
#define USE_RSA
#endif

#ifdef USE_SSL_RSA_WITH_3DES_EDE_CBC_SHA
#define USE_3DES
#define USE_SHA1_MAC
#define USE_RSA
#endif

/*
	Support for optionally encrypted private key files. These are
	usually encrypted with 3DES.
*/
#ifdef USE_ENCRYPTED_PRIVATE_KEYS
#define USE_3DES
#endif

/*
	Support for client side SSL
*/
#ifdef USE_CLIENT_SIDE_SSL
#define USE_X509
#define USE_RSA_PUBLIC_ENCRYPT
/* #define USE_FULL_CERT_PARSE */
#endif


#ifdef USE_PEERSEC_CRYPTO
#include "peersec/pscrypto.h"
#endif


#ifdef USE_ARC4
extern void matrixArc4Init(sslCipherContext_t *ctx, unsigned char *key, int keylen);
extern int matrixArc4(sslCipherContext_t *ctx, char *in, char *out, int len);
#endif /* USE_ARC4 */

#ifdef USE_3DES
extern int matrix3desInit(sslCipherContext_t *ctx, char *IV, char *key, int keylen);
extern int matrix3desEncrypt(sslCipherContext_t *ctx, char *pt, char *ct, int len);
extern int matrix3desDecrypt(sslCipherContext_t *ctx, char *ct, char *pt, int len);
#endif /* USE_3DES */

extern void matrixMd5Init(sslMd5Context_t *ctx);
extern void matrixMd5Update(sslMd5Context_t *ctx, const unsigned char *buf, unsigned long len);
extern int matrixMd5Final(sslMd5Context_t *ctx, unsigned char *hash);

extern void matrixSha1Init(sslSha1Context_t *ctx);
extern void matrixSha1Update(sslSha1Context_t *ctx, const unsigned char *buf, unsigned long len);
extern int matrixSha1Final(sslSha1Context_t *ctx, unsigned char *hash);

#ifdef USE_RSA
#ifdef USE_FILE_SYSTEM
extern int matrixRsaReadCert(char *fileName, unsigned char **out, int *outLen);
extern int matrixRsaReadPrivKey(char *fileName, char *password, sslRsaKey_t **key);
#endif /* USE_FILE_SYSTEM */

extern int matrixRsaReadPrivKeyMem(char *keyBuf, int keyBufLen, char *password, rsa_key **key);
extern int matrixRsaReadCertMem(char *certBuf, int certLen, unsigned char **out, int *outLen);

extern void matrixRsaFreeKey(sslRsaKey_t *key);

extern int matrixRsaEncryptPub(sslRsaKey_t *key, char *in, int inlen, char *out, int outlen);
extern int matrixRsaDecryptPriv(sslRsaKey_t *key, char *in, int inlen, char *out, int outlen);
#endif /* USE_RSA */

#ifdef USE_AES
extern int matrixAesInit(sslCipherContext_t *ctx, char *IV, char *key, int keylen);
extern int matrixAesEncrypt(sslCipherContext_t *ctx, char *pt, char *ct, int len);
extern int matrixAesDecrypt(sslCipherContext_t *ctx, char *ct, char *pt, int len);
#endif /* USE_AES */

/*
	Any change to these cert structs must be reflected in
	matrixSsl.h for public use
*/
typedef struct {
	char	*country;
	char	*state;
	char	*locality;
	char	*organization;
	char	*orgUnit;
	char	*commonName;
} sslDistinguishedName_t;

typedef struct {
	char	*dns;
	char	*uri;
	char	*email;
} sslSubjectAltName_t;

typedef struct {
	int						verified;
	unsigned char			*serialNumber;
	int						serialNumberLen;
	char					*notBefore;
	char					*notAfter;
	sslSubjectAltName_t		subjectAltName;
	sslDistinguishedName_t	subject;
	sslDistinguishedName_t	issuer;
} sslCertInfo_t;

#ifdef USE_X509

extern int matrixX509ParseCert(unsigned char **certBuf, int certlen, sslRsaCert_t **cert);
extern void matrixX509FreeCert(sslRsaCert_t *cert);
extern int matrixX509ValidateCert(sslRsaCert_t *subjectCert, sslRsaCert_t *issuerCert);
extern int matrixX509UserValidator(sslRsaCert_t *subjectCert,
								   int (*certValidator)(sslCertInfo_t *t));
#endif /* USE_X509 */

#ifdef __cplusplus
   }
#endif

#endif /* _h_CRYPTO_LAYER */

/******************************************************************************/
