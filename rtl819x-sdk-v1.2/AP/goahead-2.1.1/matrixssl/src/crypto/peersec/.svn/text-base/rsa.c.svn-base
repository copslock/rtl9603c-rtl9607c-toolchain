/*
 *	rsa.c
 *	Release $Name:  $
 *
 *	RSA key and cert reading, RSA padding and RSA math wrappers
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
#if VXWORKS
#include <types/vxTypesOld.h>
#endif
#include <sys/stat.h>
#include <signal.h>

#include "../cryptoLayer.h"
#include "../../os/osLayer.h"

/******************************************************************************/

static int ssl_rsa_crypt(const unsigned char *in,  unsigned int inlen,
						unsigned char *out, unsigned int *outlen, rsa_key *key,
						int type);

static int sslUnpadRSA(unsigned char *in, int inlen, unsigned char *out, 
						int outlen, int decryptType);
static int sslPadRSA(unsigned char *in, int inlen, unsigned char *out,
						int outlen);
#ifdef USE_3DES
static int hexToBinary(unsigned char *hex, unsigned char *bin, int binlen);
static void generate3DESKey(unsigned char *pass, int passlen, 
							unsigned char *salt, unsigned char *key);
#endif /* USE_3DES */

/******************************************************************************/

#define RSA_PUB_DECRYPT			0x01
#define RSA_PRIV_DECRYPT		0x02
#define	RSA_PUB_ENCRYPT			0x03


#ifdef USE_3DES
static const char encryptHeader[23] = "DEK-Info: DES-EDE3-CBC,";
#endif

/******************************************************************************/
/*
 *	Public API to return a binary buffer from a cert.  Suitable to send
 *	over the wire.  Caller must free 'out' if this function returns success (0)
 *	Parse .pem files according to http://www.faqs.org/rfcs/rfc1421.html
 *	FUTURE - Support multiple certificates in a single file.
 *	FUTURE SECURITY - Make parsing of pem format more robust!
 */
#ifdef USE_FILE_SYSTEM
int matrixRsaReadCert(char *fileName, unsigned char **out, int *outLen)
{
	FILE			*fp;
	struct			stat	fstat;
	int				certBufLen, rc;
	size_t			tmp;
	char			*certBuf;

	rc = certBufLen = 0;

	if (fileName == NULL) {
		return 0;
	}

	if ((stat(fileName, &fstat) != 0) ||  (fp = fopen(fileName, "r")) == NULL) {
		return -7; /* FILE_NOT_FOUND */
	}

	certBuf = sslMalloc(fstat.st_size);
	while ((tmp = fread(certBuf + certBufLen, sizeof(char), 512, fp)) > 0) { 
		certBufLen += (int)tmp;
	}
	fclose(fp);
	rc = matrixRsaReadCertMem(certBuf, certBufLen, out, outLen);
	sslFree(certBuf);

	return rc;
}
#endif /* USE_FILE_SYSTEM */

/******************************************************************************/
/*
/*	In memory version of matrixRsaReadCert.
*/
int matrixRsaReadCertMem(char *certBuf, int certLen, unsigned char **out, int *outLen)
{
	char	*start, *end;

	if (certBuf == NULL) {
		return 0;
	}

	if (((start = strstr(certBuf, "-----BEGIN")) != NULL) && 
		((start = strstr(certBuf, "CERTIFICATE-----")) != NULL) &&
		(end = strstr(start, "-----END")) != NULL) {
		start += strlen("CERTIFICATE-----");
		end--;
		certLen = (int)(end - start);
	} else {
		return -1;
	}
	*out = sslMalloc(certLen);
	memset(*out, '\0', certLen);
	*outLen = certLen;

	if (base64_decode(start, certLen, *out, outLen) != 0) {
		sslFree(*out);
		matrixStrDebugMsg("Unable to base64 decode certificate\n", NULL);
		return -1;
	}
	return 0;
}

/******************************************************************************/
/*
 *	Public API to return an RSA key from a PEM private key file
 *	http://www.faqs.org/rfcs/rfc1421.html
 *
 *	If password is provided, we only deal with 3des cbc encryption
 *	Function allows allocates key on success.  User must free.
 *	FUTURE SECURITY - Make parsing of private key more robust!
 */
#ifdef USE_FILE_SYSTEM
int matrixRsaReadPrivKey(char *fileName, char *password, rsa_key **key)
{
	FILE	*fp;
	struct	stat	fstat;
	char	*keyBuf;
	int		keyBufLen, rc;
	size_t	tmp = 0;

	if (fileName == NULL) {
		return 0;
	}

	rc = keyBufLen = 0;

	if ((stat(fileName, &fstat) != 0) ||  (fp = fopen(fileName, "r")) == NULL) {
		return -7; /* FILE_NOT_FOUND */
	}

	keyBuf = sslMalloc(fstat.st_size);
	while ((tmp = fread(keyBuf + keyBufLen, sizeof(char), 512, fp)) > 0) { 
		keyBufLen += (int)tmp;
	}
	fclose(fp);
	
	rc = matrixRsaReadPrivKeyMem(keyBuf, keyBufLen, password, key);
	sslFree(keyBuf);
	return rc;
}
#endif /* USE_FILE_SYSTEM */

/******************************************************************************/
/*
 *	In memory version of matrixRsaReadPrivKeyMem.
 */
int matrixRsaReadPrivKeyMem(char *keyBuf, int keyBufLen, char *password, rsa_key **key)
{
	unsigned char	*DERout, *asnp;
	char			*start, *end;
	int				DERlen, ret, PEMlen = 0;
#ifdef USE_3DES
	sslCipherContext_t	ctx;
	unsigned char		passKey[SSL_DES3_KEY_LEN];
	unsigned char		*cipherIV = NULL;
	int					tmp, len = 0;
#endif /* USE_3DES */

	start = end = NULL;
	if (keyBuf == NULL) {
		return -1;
	}

/*
 *	Check header and encryption parameters.
 */
	if ((start = strstr(keyBuf, "-----BEGIN RSA PRIVATE KEY-----\n")) == NULL) {
		matrixStrDebugMsg("Error parsing private key buffer\n", NULL);
		return -1;
	}
	start += strlen("-----BEGIN RSA PRIVATE KEY-----\n");
/*
	FUTURE - We should do this in reverse order.  If we see the ENCRYPTED
	tag, we should check / prompt for password.
*/
	if (password) {
#ifdef USE_3DES
		if ((strstr(keyBuf, "Proc-Type:") == NULL) || 
			(strstr(keyBuf, "4,ENCRYPTED") == NULL)) {
			matrixStrDebugMsg("Unrecognized private key file encoding\n", NULL);
			return -1;
		}
		if ((start = strstr(keyBuf, encryptHeader)) == NULL) {
			matrixStrDebugMsg("Unrecognized private key file encoding\n", NULL);
			return -1;
		}
		start += strlen(encryptHeader);
		cipherIV = sslMalloc(SSL_DES3_IV_LEN);
		/* SECURITY - we assume here that header points to at least 16 bytes of data */
		tmp = hexToBinary(start, cipherIV, SSL_DES3_IV_LEN);
		if (tmp < 0) {
			matrixStrDebugMsg("Invalid private key file salt\n", NULL);
			return -1;
		}
		start += tmp;
		generate3DESKey(password, (int)strlen(password), cipherIV, passKey);
#else  /* !USE_3DES */
/*
 *		The private key is encrypted, but 3DES support has been turned off
 */
		matrixStrDebugMsg("3DES has been disabled for private key decrypt\n", NULL);
		return -1;  
#endif /* USE_3DES */
	}
	ret = 0;
	if ((end = strstr(keyBuf, "-----END RSA PRIVATE KEY-----")) == NULL) {
		matrixStrDebugMsg("Error parsing private key buffer\n", NULL);
#ifdef USE_3DES
		if (cipherIV) {
			sslFree(cipherIV);
		}
#endif
		return -1;
	}
	PEMlen = (int)(end - start);

/*
 *	Take the raw input and do a base64 decode
 */
	DERout = sslMalloc(PEMlen);
	DERlen = PEMlen;
	if (base64_decode(start, PEMlen, DERout, &DERlen) != 0) {
		sslFree(DERout);
#ifdef USE_3DES
		if (cipherIV) {
			sslFree(cipherIV);
		}
#endif
		matrixStrDebugMsg("Unable to base64 decode private key\n", NULL);
		return -1;
	}

#ifdef USE_3DES
/*
 *	Decode
 */
	if (password) {
		matrix3desInit(&ctx, cipherIV, passKey, SSL_DES3_KEY_LEN);
		matrix3desDecrypt(&ctx, DERout, DERout, DERlen);
		sslFree(cipherIV);
	}
#endif /* USE_3DES */

/*
 *	Now have the DER stream to extract from in asnp
 */
	*key = sslMalloc(sizeof(rsa_key));
	memset(*key, 0x0, sizeof(rsa_key));

	asnp = DERout;
	if (asnParsePrivateKey(&asnp, DERlen, *key) < 0) {
		matrixRsaFreeKey(*key);
		*key = NULL;
		memset(DERout, 0x0, PEMlen);
		sslFree(DERout);
		if (password) {
			matrixStrDebugMsg(
				"Unable to ASN parse private key; password may be incorrect.\n",
				NULL);
		} else {
			matrixStrDebugMsg("Unable to ASN parse private key.\n", NULL);
		}
		return -1;
	}
	memset(DERout, 0x0, PEMlen);
	sslFree(DERout);
	return 0;
}

#ifdef USE_3DES
/******************************************************************************/
/*
	Convert an ASCII hex representation to a binary buffer.
	Decode enough data out of 'hex' buffer to produce 'binlen' bytes in 'bin'
	Two digits of ASCII hex map to the high and low nybbles (in that order),
	so this function assumes that 'hex' points to 2x 'binlen' bytes of data.
	Return the number of bytes processed from hex (2x binlen) or < 0 on error.
*/
static int hexToBinary(unsigned char *hex, unsigned char *bin, int binlen)
{
	unsigned char	*end, c, highOrder;

	highOrder = 1;
	for (end = hex + binlen * 2; hex < end; hex++) {
		c = *hex;
		if ('0' <= c && c <='9') {
			c -= '0';
		} else if ('a' <= c && c <='f') {
			c -= ('a' - 10);
		} else if ('A' <= c && c <='F') {
			c -= ('A' - 10);
		} else {
			return -1;
		}
		if (highOrder++ & 0x1) {
			*bin = c << 4;
		} else {
			*bin |= c;
			bin++;
		}
	}
	return binlen * 2;
}

/******************************************************************************/
/*
	Generate a 3DES key given a password and salt value.
	We use PKCS#5 2.0 PBKDF1 key derivation format with MD5 and count == 1 per:
	http://www.rsasecurity.com/rsalabs/pkcs/pkcs-5/index.html

	This key is compatible with the algorithm used by OpenSSL to encrypt keys
	generated with 'openssl genrsa'.  If other encryption formats are used
	(for example PBKDF2), or an iteration count > 0 is used, they are not 
	compatible with this simple implementation.  OpenSSL provides many options
	for converting key formats to the one used here.

	A 3DES key is 24 bytes long, to generate it with this algorithm,
	we md5 hash the password and salt for the first 16 bytes.  We then 
	hash these first 16 bytes with the password and salt again, generating 
	another 16 bytes.  We take the first 16 bytes and 8 of the second 16 to 
	form the 24 byte key.

	salt is assumed to point to 8 bytes of data
	key is assumed to point to 24 bytes of data
*/
static void generate3DESKey(unsigned char *pass, int passlen, unsigned char *salt, 
					unsigned char *key)
{
	sslMd5Context_t		state;
	unsigned char		md5[SSL_MD5_HASH_SIZE];

	matrixMd5Init(&state);
	matrixMd5Update(&state, pass, passlen);
	matrixMd5Update(&state, salt, SSL_DES3_IV_LEN);
	matrixMd5Final(&state, md5);
	memcpy(key, md5, SSL_MD5_HASH_SIZE);

	matrixMd5Init(&state);
	matrixMd5Update(&state, md5, SSL_MD5_HASH_SIZE);
	matrixMd5Update(&state, pass, passlen);
	matrixMd5Update(&state, salt, SSL_DES3_IV_LEN);
	matrixMd5Final(&state, md5);
	memcpy(key + SSL_MD5_HASH_SIZE, md5, SSL_DES3_KEY_LEN - SSL_MD5_HASH_SIZE);
}
#endif /* USE_3DES */


/******************************************************************************/

static int ssl_rsa_crypt(const unsigned char *in,  unsigned int inlen,
                      unsigned char *out, unsigned int *outlen, rsa_key *key,
					  int type)
{
	mp_int tmp, tmpa, tmpb;
	unsigned long x;
	int res;

	if (in == NULL || out == NULL || outlen == NULL || key == NULL) {
		return -1;
	}

    /* init and copy into tmp */
	if (mp_init_multi(&tmp, &tmpa, &tmpb, NULL) != MP_OKAY) {
		matrixStrDebugMsg("ssl_rsa_crypt error: mp_init_multi\n", NULL);
		goto error;
	}
	if (mp_read_unsigned_bin(&tmp, (unsigned char *)in, (int)inlen) != 
			MP_OKAY) {
		matrixStrDebugMsg("ssl_rsa_crypt error: mp_read_unsigned_bin\n", NULL);
		goto error; 
	}
	/* sanity check on the input */
	if (mp_cmp(&key->N, &tmp) == MP_LT) {
		res = -1;
		goto done;
	}
	if (type == RSA_PRIV_DECRYPT) {
		if (key->optimized) {
			if (mp_exptmod(&tmp, &key->dP, &key->p, &tmpa) != MP_OKAY) {
				matrixStrDebugMsg("decrypt error: mp_exptmod dP, p\n", NULL);
				goto error;
			}
			if (mp_exptmod(&tmp, &key->dQ, &key->q, &tmpb) != MP_OKAY) { 
				matrixStrDebugMsg("decrypt error: mp_exptmod dQ, q\n", NULL);
				goto error;
			}
			if (mp_mul(&tmpa, &key->qP, &tmpa) != MP_OKAY) {
				matrixStrDebugMsg("decrypt error: mp_mul qP \n", NULL);
				goto error;
			}
			if (mp_mul(&tmpb, &key->pQ, &tmpb) != MP_OKAY) {
				matrixStrDebugMsg("decrypt error: mp_mul pQ\n", NULL);
				goto error;
			}
			if (mp_addmod(&tmpa, &tmpb, &key->N, &tmp) != MP_OKAY) {
				matrixStrDebugMsg("decrypt error: mp_addmod N\n", NULL);
				goto error;
			}
		} else {
			if (mp_exptmod(&tmp, &key->d, &key->N, &tmp) != MP_OKAY) { // #3
				matrixStrDebugMsg("ssl_rsa_crypt error: mp_exptmod\n", NULL);
				goto error;
			}
		}
	} else if (type == RSA_PUB_ENCRYPT || type == RSA_PUB_DECRYPT) {
		if (mp_exptmod(&tmp, &key->e, &key->N, &tmp) != MP_OKAY) {
			matrixStrDebugMsg("ssl_rsa_crypt error: mp_exptmod\n", NULL);
			goto error;
		}
	} else {
		matrixStrDebugMsg("ssl_rsa_crypt error: invalid type param\n", NULL);
		goto error;
	}
	/* read it back */
	x = (unsigned long)mp_unsigned_bin_size(&tmp);
	if (x > *outlen) {
		res = -1;
		matrixStrDebugMsg("ssl_rsa_crypt error: mp_unsigned_bin_size\n", NULL);
		goto done;
	}
/*
	We want the encrypted value to always be the key size.  Pad with 0x0
*/
	while (x < (unsigned long)key->size) {
		*out++ = 0x0;
		x++;
	}

	*outlen = x;
	/* convert it */
	if (mp_to_unsigned_bin(&tmp, out) != MP_OKAY) {
		matrixStrDebugMsg("ssl_rsa_crypt error: mp_to_unsigned_bin\n", NULL);
		goto error;
	}
	/* clean up and return */
	res = 0;
	goto done;
error:
	res = -1;
done:
	mp_clear_multi(&tmp, &tmpa, &tmpb, NULL);
	return res;
}

#ifdef USE_RSA_PUBLIC_ENCRYPT
/******************************************************************************/
/*
	Pad a value to be encrypted by RSA, according to PKCS#1 v1.5
	http://www.rsasecurity.com/rsalabs/pkcs/pkcs-1/
	When encrypting a value with RSA, the value is first padded to be 
	equal to the public key size using the following method:
		00 02 <random data (min 8 bytes)> 00 <value to be encrypted>
	Random data is however many non-zero bytes it takes to pad the 
	value to the key length (randomLen = keyLen - 3 - valueLen).
	There must be at least 8 bytes of random data.
*/
static int sslPadRSA(unsigned char *in, int inlen, unsigned char *out,
			int outlen)
{
	unsigned char *c;
	int	randomLen;
	
	randomLen = outlen - 3 - inlen;
	if (randomLen < 8) {
		matrixIntDebugMsg("RSA encryption data too large: %d\n", inlen);
		return -1;
	}
	c = out;
	*c = 0x00;
	c++;
	*c = 0x02;
	c++;
	if (sslGetEntropy(c, randomLen) < 0) {
		matrixStrDebugMsg("Error gathering RSA pad entropy\n", NULL);
		return -1;
	}
/*
	SECURITY:  Read through the random data and change all 0x0 to 0x01.
	This is necessary to ensure the Unpad on decryption doesn't falsely
	stop walking the random number on 0x0
*/
	while (randomLen-- > 0) {
		if (*c == 0x0) {
			*c = 0x01;
		}
		c++;
	}
	*c = 0x00;
	c++;
	memcpy(c, in, inlen);
	
	return outlen;
}

/******************************************************************************/
/*
	RSA public encryption.  
	The outlen param must be set to the strength of the key:  key->size
*/
int matrixRsaEncryptPub(sslRsaKey_t *key, char *in, int inlen, char *out, 
						int outlen)
{
	char		*tmpIn;
	int			size;

	size = key->size;
	if (outlen < size) {
		return -1;
	}
	tmpIn = sslMalloc(size);
	if (sslPadRSA(in, inlen, tmpIn, size) < 0) {
		sslFree(tmpIn);
		return -1;
	}
	if (ssl_rsa_crypt(tmpIn, size, out, &outlen, key, RSA_PUB_ENCRYPT) < 0 ||
			outlen != size) {
		sslFree(tmpIn);
		return -1;
	}
	sslFree(tmpIn);
	return size;
}

#else  /* USE_RSA_PUBLIC_ENCRYPT - Keeps the cipher suite definition clean */
int matrixRsaEncryptPub(sslRsaKey_t *key, char *in, int inlen, char *out, 
						int outlen)
{
	if (inlen > outlen) {
		return -1;
	}
	memcpy(out, in, inlen);
	return inlen;
}
#endif /* USE_RSA_PUBLIC_ENCRYPT */

/******************************************************************************/
/*
	Unpad a value decrypted by RSA, according to PKCS#1 v1.5
	http://www.rsasecurity.com/rsalabs/pkcs/pkcs-1/
	
	When decrypted, the data will look like the pad, including the inital
	byte (00).  Form:
		00 <decryptType> <random data (min 8 bytes)> 00 <value to be encrypted>

	We don't worry about v2 rollback issues because we don't support v2
*/
static int sslUnpadRSA(unsigned char *in, int inlen, unsigned char *out, 
					   int outlen, int decryptType)
{
	unsigned char	*c, *end;

	if (inlen < outlen + 10) {
		return -1;
	}
	c = in;
	end = in + inlen;
/*
	Verify the first byte (block type) is correct.
*/
	if (*c++ != 0x00 || *c != decryptType) {
		return -1;
	}
	c++;
/*
	Skip over the random, non-zero bytes used as padding
*/
	while (c < end && *c != 0x0) {
		c++;
	}
	c++;
/*
	The length of the remaining data should be equal to what was expected
	Combined with the initial length check, there must be >= 8 bytes of pad
	ftp://ftp.rsa.com/pub/pdfs/bulletn7.pdf
*/
	if (end - c != outlen) {
		return -1;
	}
/*
	Copy the value bytes to the out buffer
*/
	while (c < end) {
		*out = *c;
		out++; c++;
	}
	return outlen;
}

/******************************************************************************/

int matrixRsaDecryptPriv(sslRsaKey_t *key, char *in, int inlen, char *out, 
						 int outlen)
{
	char		*tmpOut;
	int			ptLen;

	tmpOut = sslMalloc(inlen);
	if (ssl_rsa_crypt(in, inlen, tmpOut, &inlen, key, RSA_PRIV_DECRYPT) < 0) {
		sslFree(tmpOut);
		return -1;
	}
	ptLen = sslUnpadRSA(tmpOut, inlen, out, outlen, RSA_PRIV_DECRYPT);
	memset(tmpOut, 0x0, inlen);
	sslFree(tmpOut);
	return ptLen;
}

int matrixRsaDecryptPub(sslRsaKey_t *key, char *in, int inlen, char *out,
						int outlen)
{
	char	*tmpOut;
	int		ptLen;

	tmpOut = sslMalloc(inlen);
	memset(tmpOut, '\0', inlen);
	if (ssl_rsa_crypt(in, inlen, tmpOut, &inlen, key, RSA_PUB_DECRYPT) < 0) {
		sslFree(tmpOut);
		return -1;
	}
	ptLen = sslUnpadRSA(tmpOut, inlen, out, outlen, RSA_PUB_DECRYPT);
	sslFree(tmpOut);
	return 0;
}

#ifdef USE_X509
/******************************************************************************/
/*
	A signature validation for certificates.  -1 return is an error.  The success
	of the validation is returned in the 'valid' param of the subjectCert.
	1 if the issuerCert	signed the subject cert. -1 if not
*/
int matrixX509ValidateCert(sslRsaCert_t *subjectCert, sslRsaCert_t *issuerCert)
{
	sslRsaCert_t	*ic;
	unsigned char	*sigOut;
	int				sigLen;

	subjectCert->valid = -1;
/*
	Supporting a one level chain or a self-signed cert.  If the issuer
	is NULL, the self-signed test is done.
*/
	if (issuerCert == NULL) {
		ic = subjectCert;
	} else {
		ic = issuerCert;
	}
/*
	Path confirmation
	Only supporting one level of chaining for now. 
	The issuer must be a CA	if provided.
*/
	while (ic) {
		if (subjectCert != ic) {
			if (issuerCert->extensions.bc.ca <= 0) {
				ic = ic->next;
				continue;
			}
/*
			Use sha1 hash of issuer fields computed at parse time to compare
*/
			if (memcmp(subjectCert->issuer.hash, ic->subject.hash,
					SSL_SHA1_HASH_SIZE) != 0) {
				ic = ic->next;
				continue;
			}
		}
/*
		Signature confirmation
		The sigLen is the ASN.1 size in bytes for encoding the hash.
		The magic 10 is comprised of the SEQUENCE and ALGORITHM ID overhead.
		The magic 8 and 5 are the OID lengths of the corresponding algorithm.
*/
		if (subjectCert->sigAlgorithm ==  OID_RSA_MD5) {
			sigLen = 10 + SSL_MD5_HASH_SIZE + 8;
		} else if (subjectCert->sigAlgorithm == OID_RSA_SHA1) {
			sigLen = 10 + SSL_SHA1_HASH_SIZE + 5;
		} else {
			matrixStrDebugMsg("Unsupported signature algorithm\n", NULL);
			return -1;
		}
		sigOut = sslMalloc(sigLen);
		matrixRsaDecryptPub(&(ic->publicKey), subjectCert->signature,
			subjectCert->signatureLen, sigOut, sigLen);
		if (asnConfirmSignature(subjectCert, sigOut, sigLen) < 0) {
			sslFree(sigOut);
			ic = ic->next;
			continue;
		}
		sslFree(sigOut);
/*
		Fall through to here only if passed signature check.
*/
		subjectCert->valid = 1;
		break;
	}
	return 0;
}

/******************************************************************************/
/*
	Calls a user defined callback to allow for manual validation of the
	certificate.
*/
int matrixX509UserValidator(sslRsaCert_t *subjectCert,
			int (*certValidator)(sslCertInfo_t *t))
{
	sslCertInfo_t	*cert;
	int				rc;

	if (certValidator == NULL) {
		return 0;
	}
	cert = sslMalloc(sizeof(sslCertInfo_t));

	cert->issuer.commonName = subjectCert->issuer.commonName;
	cert->issuer.country = subjectCert->issuer.country;
	cert->issuer.locality = subjectCert->issuer.locality;
	cert->issuer.organization = subjectCert->issuer.organization;
	cert->issuer.orgUnit = subjectCert->issuer.orgUnit;
	cert->issuer.state = subjectCert->issuer.state;

	cert->subject.commonName = subjectCert->subject.commonName;
	cert->subject.country = subjectCert->subject.country;
	cert->subject.locality = subjectCert->subject.locality;
	cert->subject.organization = subjectCert->subject.organization;
	cert->subject.orgUnit = subjectCert->subject.orgUnit;
	cert->subject.state = subjectCert->subject.state;

	cert->serialNumber = subjectCert->serialNumber;
	cert->serialNumberLen = subjectCert->serialNumberLen;
	cert->verified = subjectCert->valid;
	cert->notBefore = subjectCert->notBefore;
	cert->notAfter = subjectCert->notAfter;

	cert->subjectAltName.dns = subjectCert->extensions.san.dns;
	cert->subjectAltName.uri = subjectCert->extensions.san.uri;
	cert->subjectAltName.email = subjectCert->extensions.san.email;
	
	rc = certValidator(cert);
	sslFree(cert);
	return rc;
}
#endif /* USE_X509 */

/******************************************************************************/
/*
 *	Free an RSA key.  mp_clear will zero the memory of each element and free it.
 */

void matrixRsaFreeKey(sslRsaKey_t *key)
{
	mp_clear(&(key->N));
	mp_clear(&(key->e));
	mp_clear(&(key->d));
	mp_clear(&(key->p));
	mp_clear(&(key->q));
	mp_clear(&(key->dP));
	mp_clear(&(key->dQ));
	mp_clear(&(key->qP));
	mp_clear(&(key->pQ));
	sslFree(key);
}

/******************************************************************************/

