/*
 *	matrixSsl.c
 *	Release $Name:  $
 *
 *	Secure Sockets Layer message parsing, handshaking, and session management
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
#include <time.h>

#define SSL2_HEADER_LEN				2
#define SSL3_HEADER_LEN				5
#define SSL3_HANDSHAKE_HEADER_LEN	4

#define SSL_MAX_IGNORED_MESSAGE_COUNT	1024

/*
	These are defines rather than enums because we want to store them as char,
	not int (enum size)
*/
#define SSL_RECORD_TYPE_CHANGE_CIPHER_SPEC	20
#define SSL_RECORD_TYPE_ALERT				21
#define SSL_RECORD_TYPE_HANDSHAKE			22
#define SSL_RECORD_TYPE_APPLICATION_DATA	23

#define SSL_HS_HELLO_REQUEST		0
#define SSL_HS_CLIENT_HELLO			1
#define SSL_HS_SERVER_HELLO			2
#define SSL_HS_CERTIFICATE			11
#define SSL_HS_SERVER_KEY_EXCHANGE	12
#define SSL_HS_CERTIFICATE_REQUEST	13
#define SSL_HS_SERVER_HELLO_DONE	14
#define SSL_HS_CERTIFICATE_VERIFY	15
#define SSL_HS_CLIENT_KEY_EXCHANGE	16
#define SSL_HS_FINISHED				20
#define SSL_HS_DONE					255	/* Handshake complete (internal) */

/******************************************************************************/

static int sslResetContext(ssl_t *ssl);
static int parseSSLHandshake(ssl_t *ssl, char *inbuf, int len);

static int sslEncodeResponse(ssl_t *ssl, sslBuf_t *out);

static int writeCertificate(ssl_t *ssl, sslBuf_t *out);
static int writeChangeCipherSpec(ssl_t *ssl, sslBuf_t *out);
static int writeFinished(ssl_t *ssl, sslBuf_t *out);
static int writeAlert(ssl_t *ssl, unsigned char level, 
						unsigned char description, sslBuf_t *out);
#ifdef USE_CLIENT_SIDE_SSL
static int writeClientKeyExchange(ssl_t *ssl, sslBuf_t *out);
#endif /* USE_CLIENT_SIDE_SSL */

#ifdef USE_SERVER_SIDE_SSL
static int writeServerHello(ssl_t *ssl, sslBuf_t *out);
static int writeServerHelloDone(ssl_t *ssl, sslBuf_t *out);
static int registerSession(ssl_t *ssl);
static int resumeSession(ssl_t *ssl);
static int updateSession(ssl_t *ssl);
#endif /* USE_SERVER_SIDE_SSL */

static int writeRecordHeader(ssl_t *ssl, unsigned char type, int len, char *c);
static int writeHandshakeHeader(ssl_t *ssl, unsigned char type, int len, 
								char *c);
static int sslWritePad(unsigned char *p, unsigned char padLen);

static int sslInitHSHash(ssl_t *ssl);
static int sslUpdateHSHash(ssl_t *ssl, unsigned char *in, int len);
static int sslSnapshotHSHash(ssl_t *ssl, char *out, int senderFlag);

static int sslActivateReadCipher(ssl_t *ssl);
static int sslActivateWriteCipher(ssl_t *ssl);
static int sslActivatePublicCipher(ssl_t *ssl);

#ifdef USE_SERVER_SIDE_SSL
/*
	Static session table for session cache and lock for multithreaded env
*/
static sslMutex_t			sessionTableLock;
static sslSessionEntry_t	sessionTable[SSL_SESSION_TABLE_SIZE];
#endif /* USE_SERVER_SIDE_SSL */

/******************************************************************************/
/*
	Open and close the SSL module.  These routines are called once in the 
	lifetime of the application and initialize and clean up the library 
	respectively.
*/
int matrixSslOpen()
{
	if (sslOpenOsdep() < 0) {
		matrixStrDebugMsg("Osdep open failure\n", NULL);
		return -1;
	}
#ifdef USE_SERVER_SIDE_SSL
	memset(sessionTable, 0x0, 
		sizeof(sslSessionEntry_t) * SSL_SESSION_TABLE_SIZE);
	sslCreateMutex(&sessionTableLock);
#endif /* USE_SERVER_SIDE_SSL */
	return 0;
}

void matrixSslClose()
{
#ifdef USE_SERVER_SIDE_SSL
	int		i;

	sslLockMutex(&sessionTableLock);
	for (i = 0; i < SSL_SESSION_TABLE_SIZE; i++) {
		if (sessionTable[i].inUse == 1) {
			matrixStrDebugMsg("Warning: closing while session still in use\n",
				NULL);
		}
	}
	memset(sessionTable, 0x0, 
		sizeof(sslSessionEntry_t) * SSL_SESSION_TABLE_SIZE);
	sslUnlockMutex(&sessionTableLock);
	sslDestroyMutex(&sessionTableLock);
#endif /* USE_SERVER_SIDE_SSL */
	sslCloseOsdep();
}

/******************************************************************************/
/*
	Read in the certificate and private keys from the given file
	If privPass is non-NULL, it will be used to decode an encrypted private
	key file.
	The certificate is stored internally as a pointer to DER encoded X.509
	The private key is stored in a crypto provider specific structure
*/
#ifdef USE_FILE_SYSTEM
int matrixSslReadKeys(sslKeys_t **keys, char *certFile, char *privFile,
					  char *privPass, char *trustedCAFiles)
{
	sslKeys_t	*lkeys;
	int			rc;
#ifdef USE_CLIENT_SIDE_SSL
	sslRsaCert_t	*currCert, *prevCert;
	char			sep[] = ";";
	unsigned char	*caCert, *tmp, *caFile, *origList;
	int				caCertLen, i;
#endif /* USE_CLIENT_SIDE_SSL */

	*keys = lkeys = sslMalloc(sizeof(sslKeys_t));
	memset(lkeys, 0x0, sizeof(sslKeys_t));
/*
	Certificate file to send
*/
	rc = matrixRsaReadCert(certFile, &lkeys->cert, &lkeys->certLen);
	if (rc < 0 ) {
		matrixStrDebugMsg("Error reading cert file\n", NULL);
		matrixSslFreeKeys(lkeys);
		return rc;
	}
	rc = matrixRsaReadPrivKey(privFile, privPass, &lkeys->privKey);
	if (rc < 0) {
		matrixStrDebugMsg("Error reading private key file\n", NULL);
		matrixSslFreeKeys(lkeys);
		return rc;
	}
#ifdef USE_CLIENT_SIDE_SSL
	caFile = NULL;
	origList = trustedCAFiles;
	if (origList != NULL) {
		caFile = strtok(origList, sep);
	}
	i = 0;
	while (caFile != NULL) {
		caCert = NULL;
		currCert = NULL;
/*
	Certificate used to validate others
*/
		if (matrixRsaReadCert(caFile, &caCert, &caCertLen) < 0 ||
				caCert == NULL) {
			matrixStrDebugMsg("Error reading CA cert file %s\n", caFile);
			caFile = strtok(NULL, sep);
			continue;
		}

		tmp = caCert;
		if (matrixX509ParseCert(&caCert, caCertLen, &currCert) < 0) {
			matrixStrDebugMsg("Error parsing CA cert %s\n", caFile);
			sslFree(tmp);
			caFile = strtok(NULL, sep);
			continue;
		}
		sslFree(tmp);
		if (i++ == 0) {
			lkeys->caCerts = currCert;
		} else {
			prevCert->next = currCert;
		}
		prevCert = currCert;
		currCert = NULL;
		caFile = strtok(NULL, sep);
	}
/*
	Check to see that if a set of certs were passed in at least
	one ended up being valid.
*/
	if (trustedCAFiles != NULL && lkeys->caCerts == NULL) {
		matrixStrDebugMsg("No valid CA certs in %s\n", trustedCAFiles);
		matrixSslFreeKeys(lkeys);
		return -1;
	}
#endif /* USE_CLIENT_SIDE_SSL */

	return 0; 
}
#else /* USE_FILE_SYSTEM */
int matrixSslReadKeys(sslKeys_t **keys, char *certFile, char *privFile,
					 char *privPass, char *trustedCAFile)
{
	matrixStrDebugMsg("Error: Calling matrixSslReadKeys against a library " \
					  "built without USE_FILE_SYSTEM defined\n", NULL);
	return -1;
}
#endif /* USE_FILE_SYSTEM */

/******************************************************************************/
/*
	In memory version of matrixSslReadKeys.
*/
int	matrixSslReadKeysMem(sslKeys_t **keys, char *certBuf, int certLen,
			char *privBuf, int privLen, char *privPass, char *trustedCABuf,
			int trustedCALen)
{
	sslKeys_t	*lkeys;
#ifdef USE_CLIENT_SIDE_SSL
	unsigned char	*caCert, *tmp;
	int				caCertLen;
#endif /* USE_CLIENT_SIDE_SSL */

	*keys = lkeys = sslMalloc(sizeof(sslKeys_t));
	memset(lkeys, 0x0, sizeof(sslKeys_t));
/*
	Certificate file to send
*/
	if (matrixRsaReadCertMem(certBuf, certLen, &lkeys->cert, &lkeys->certLen) < 0 ||
			matrixRsaReadPrivKeyMem(privBuf, privLen, privPass, &lkeys->privKey) < 0) {

		matrixStrDebugMsg("Error reading key files\n", NULL);
		matrixSslFreeKeys(lkeys);
		return -1;
	}
#ifdef USE_CLIENT_SIDE_SSL
	caCert = NULL;
/*
	Certificate used to validate others
*/
	if (matrixRsaReadCertMem(trustedCABuf, trustedCALen, &caCert, &caCertLen) < 0) {
		matrixStrDebugMsg("Error reading CA cert file\n", NULL);
		matrixSslFreeKeys(lkeys);
		return -1;
	}
	if (caCert == NULL) {
		return 0;
	}
	tmp = caCert;
	if (matrixX509ParseCert(&caCert, caCertLen, &lkeys->caCerts) < 0) {
		matrixStrDebugMsg("Error parsing CA cert\n", NULL);
		sslFree(tmp);
		matrixSslFreeKeys(lkeys);
		return -1;
	}
	sslFree(tmp);
#endif /* USE_CLIENT_SIDE_SSL */

	return 0;
}

/******************************************************************************/
/*
	Free private key and cert and zero memory allocated by matrixSslReadKeys.
*/
void matrixSslFreeKeys(sslKeys_t *keys)
{
#ifdef USE_CLIENT_SIDE_SSL
	sslRsaCert_t	*cert, *nextCert;
#endif /* USE_CLIENT_SIDE_SSL */

	if (keys) {
		if (keys->cert) {
			memset(keys->cert, 0x0, keys->certLen);
			sslFree(keys->cert);
		}
		if (keys->privKey) {
			matrixRsaFreeKey(keys->privKey);
		}
#ifdef USE_CLIENT_SIDE_SSL
		if (keys->caCerts) {
			cert = keys->caCerts->next;
			matrixX509FreeCert(keys->caCerts);
			while (cert) {
				nextCert = cert->next;
				matrixX509FreeCert(cert);
				cert = nextCert;
			}
		}
#endif /* USE_CLIENT_SIDE_SSL */
		sslFree(keys);
	}
}

/******************************************************************************/
/*
	New SSL protocol context
	This structure is associated with a single SSL connection.  Each socket
	using SSL should be associated with a new SSL context.

	certBuf and privKey ARE NOT duplicated within the server context, in order
	to minimize memory usage with multiple simultaneous requests.  They must 
	not be deleted by caller until all server contexts using them are deleted.
*/
int matrixSslNewSession(ssl_t **ssl, sslKeys_t *keys, sslSessionId_t *session,
						int flags)
{
	unsigned char	*cert = NULL;
	sslRsaKey_t		*privKey = NULL;
	ssl_t			*lssl;

/*
	First API level chance to make sure a user is not attempting to use
	client or server support that was not built into this library compile
*/
#ifndef USE_SERVER_SIDE_SSL
	if (flags & SSL_FLAGS_SERVER) {
		matrixStrDebugMsg("MatrixSSL lib not compiled with server support\n",
			NULL);
		return -1;
	}
#endif
#ifndef USE_CLIENT_SIDE_SSL
	if (!(flags & SSL_FLAGS_SERVER)) {
		matrixStrDebugMsg("MatrixSSL lib not compiled with client support\n",
			NULL);
		return -1;
	}
#endif

	if (flags & SSL_FLAGS_SERVER) {
		if (keys == NULL) {
			matrixStrDebugMsg("NULL keys in  matrixSslNewSession\n", NULL);
			return -1;
		}
		if (session != NULL) {
			matrixStrDebugMsg("Server session must be NULL\n", NULL);
			return -1;
		}
	}
	*ssl = lssl = sslMalloc(sizeof(ssl_t));
	memset(lssl, 0x0, sizeof(ssl_t));

	lssl->cipher = sslGetCipherSpec(SSL_NULL_WITH_NULL_NULL);
	sslActivateReadCipher(lssl);
	sslActivateWriteCipher(lssl);
	sslActivatePublicCipher(lssl);

	if (flags & SSL_FLAGS_SERVER) {
		lssl->flags |= SSL_FLAGS_SERVER;
		lssl->hsState = SSL_HS_CLIENT_HELLO;
	} else {
		lssl->majVer = SSL3_MAJ_VER;
		lssl->minVer = SSL3_MIN_VER;
		lssl->hsState = SSL_HS_SERVER_HELLO;
		if (session != NULL && session->cipherId != SSL_NULL_WITH_NULL_NULL) {
			lssl->cipher = sslGetCipherSpec(session->cipherId);
			if (lssl->cipher == NULL) {
				matrixStrDebugMsg("Invalid session id to matrixSslNewSession\n",
					NULL);
			} else {
				memcpy(lssl->sec.masterSecret, session->masterSecret, 
					SSL_HS_MASTER_SIZE);
				lssl->sessionIdLen = SSL_MAX_SESSION_ID_SIZE;
				lssl->sessionId = sslMalloc(SSL_MAX_SESSION_ID_SIZE);
				memcpy(lssl->sessionId, session->id, SSL_MAX_SESSION_ID_SIZE);
			}
		}
	}
	lssl->err = SSL_ALERT_NONE;

	lssl->keys = keys;

	sslInitHSHash(lssl);

	return 0;
}

/******************************************************************************/
/*
	Delete an SSL session.  Some information on the session may stay around
	in the session resumption cache.
	SECURITY - We memset relevant values to zero before freeing to reduce 
	the risk of our keys floating around in memory after we're done.
*/
void matrixSslDeleteSession(ssl_t *ssl)
{
	if (ssl == NULL) {
		return;
	}
	ssl->flags |= SSL_FLAGS_CLOSED;
/*
	If we have a sessionId, for servers we need to clear the inUse flag in 
	the session cache so the ID can be replaced if needed.  In the client case
	the caller should have called matrixSslGetSessionId already to copy the
	master secret and sessionId, so free it now.

	In all cases except a successful updateSession call on the server, the
	master secret must be freed.
*/
#ifdef USE_SERVER_SIDE_SSL
	if (ssl->sessionIdLen > 0 && (ssl->flags & SSL_FLAGS_SERVER)) {
		updateSession(ssl);
	}
#endif /* USE_SERVER_SIDE_SSL */
	memset(ssl->sec.masterSecret, 0x0, SSL_HS_MASTER_SIZE);
	if (ssl->sessionId) {
		memset(ssl->sessionId, 0x0, ssl->sessionIdLen);
		sslFree(ssl->sessionId);
		ssl->sessionId = NULL;
	}
	
	if (ssl->sec.keyBlock) {
		memset(ssl->sec.keyBlock, 0x0, 2 * ssl->cipher->macSize + 
			2 * ssl->cipher->keySize + 2 * ssl->cipher->ivSize);
		sslFree(ssl->sec.keyBlock);
		ssl->sec.keyBlock = NULL;
	}
	if (ssl->sec.clientRandom) {
		memset(ssl->sec.clientRandom, 0x0, SSL_HS_RANDOM_SIZE);
		sslFree(ssl->sec.clientRandom);
		ssl->sec.clientRandom = NULL;
	}
	if (ssl->sec.serverRandom) {
		memset(ssl->sec.serverRandom, 0x0, SSL_HS_RANDOM_SIZE);
		sslFree(ssl->sec.serverRandom);
		ssl->sec.serverRandom = NULL;
	}
	if (ssl->sec.premaster) {
		memset(ssl->sec.premaster, 0x0, SSL_HS_PREMASTER_SIZE);
		sslFree(ssl->sec.premaster);
		ssl->sec.premaster = NULL;
	}
#ifdef USE_CLIENT_SIDE_SSL
	if (ssl->sec.cert) {
		matrixX509FreeCert(ssl->sec.cert);
		ssl->sec.cert = NULL;
	}
#endif /* USE_CLIENT_SIDE_SSL */
/*
	The cipher and mac contexts are inline in the ssl structure, so
	clearing the structure clears those states as well.
*/
	memset(ssl, 0x0, sizeof(ssl_t));
	sslFree(ssl);
}

/******************************************************************************/
/*
	Reset an SSL context.  This is used internally when rehandshaking, if the
	client sends a ClientHello on an already negotiated session.
*/
static int sslResetContext(ssl_t *ssl)
{
	/* FUTURE - Implement rehandshake */
	matrixStrDebugMsg("Renegotiation not available, ignoring request.\n", NULL);
	return -1;
}

/******************************************************************************/
/*
	Parse incoming data per http://wp.netscape.com/eng/ssl3
*/
int matrixSslDecode(ssl_t *ssl, sslBuf_t *in, sslBuf_t *out, 
						   unsigned char *error, unsigned char *alertLevel, 
						   unsigned char *alertDescription)
{
	unsigned char	*c, *p, *end, *pend, *oend;
	unsigned char	*mac, macError;
	int				rc;
	unsigned char	padLen;
/*
	If we've had a protocol error, don't allow further use of the session
*/
	if (ssl->flags & SSL_FLAGS_ERROR || ssl->flags & SSL_FLAGS_CLOSED) {
		return SSL_ERROR;
	}
/*
	This flag is set if the previous call to this routine returned an SSL_FULL
	error from encodeResponse, indicating that there is data to be encoded, 
	but the out buffer was not big enough to handle it.  If we fall in this 
	case, the user has increased the out buffer size and is re-calling this 
	routine
*/
	if (ssl->flags & SSL_FLAGS_NEED_ENCODE) {
		ssl->flags &= ~SSL_FLAGS_NEED_ENCODE;
		goto encodeResponse;
	}

	c = in->start;
	end = in->end;
	oend = out->end;
/*
	Processing the SSL Record header:
	If the high bit of the first byte is set and this is the first 
	message we've seen, we parse the request as an SSLv2 request
	http://wp.netscape.com/eng/security/SSL_2.html
	SSLv2 also supports a 3 byte header when padding is used, but this should 
	not be required for the initial plaintext message, so we don't support it
	v3 Header:
		1 byte type
		1 byte major version
		1 byte minor version
		2 bytes length
	v2 Header
		2 bytes length (ignore high bit)
*/
decodeMore:
	sslAssert(out->end == oend);
	if (end - c < SSL2_HEADER_LEN) {
		return SSL_PARTIAL;
	}
	if (ssl->majVer != 0 || (*c & 0x80) == 0) {
		if (end - c < SSL3_HEADER_LEN) {
			return SSL_PARTIAL;
		}
		ssl->rec.type = *c; c++;
		ssl->rec.majVer = *c; c++;
		ssl->rec.minVer = *c; c++;
		ssl->rec.len = *c << 8; c++;
		ssl->rec.len += *c; c++;
	} else {
		ssl->rec.type = SSL_RECORD_TYPE_HANDSHAKE;
		ssl->rec.majVer = 2;
		ssl->rec.minVer = 0;
		ssl->rec.len = (*c & 0x7f) << 8; c++;
		ssl->rec.len += *c; c++;
	}
/*
	Validate the various record headers.  The type must be valid,
	the major and minor versions must match the negotiated versions (if we're
	past ClientHello) and the length must be < 16K and > 0
*/
	if (ssl->rec.type != SSL_RECORD_TYPE_CHANGE_CIPHER_SPEC &&
		ssl->rec.type != SSL_RECORD_TYPE_ALERT &&
		ssl->rec.type != SSL_RECORD_TYPE_HANDSHAKE &&
        ssl->rec.type != SSL_RECORD_TYPE_APPLICATION_DATA) {
		ssl->err = SSL_ALERT_UNEXPECTED_MESSAGE;
		matrixIntDebugMsg("Record header type not valid: %d\n", ssl->rec.type);
		goto encodeResponse;
	}
/*
	Verify the record version numbers unless this is the first record we're
	reading.
*/
	if (ssl->hsState != SSL_HS_SERVER_HELLO && ssl->hsState != SSL_HS_CLIENT_HELLO) {
		if (ssl->rec.majVer != ssl->majVer || ssl->rec.minVer != ssl->minVer) {
			ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
			matrixStrDebugMsg("Record header version not valid\n", NULL);
			goto encodeResponse;
		}
	}
/*
	Verify max and min record lengths
*/
	if (ssl->rec.len > 0x3FFF || ssl->rec.len == 0) {
		ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
		matrixIntDebugMsg("Record header length not valid: %d\n", ssl->rec.len);
		goto encodeResponse;
	}
/*
	This implementation requires the entire SSL record to be in the 'in' buffer
	before we parse it.  This is because we need to MAC the entire record before
	allowing it to be used by the caller.  The only alternative would be to 
	copy the partial record to an internal buffer, but that would require more
	memory usage, which we're trying to keep low.
*/
	if (end - c < ssl->rec.len) {
		return SSL_PARTIAL;
	}
/*
	Make sure we have enough room to hold the decoded record
*/
	if ((out->buf + out->size) - out->end < ssl->rec.len) {
		return SSL_FULL;
	}
/*
	Decrypt the entire record contents.  The record length should be
	a multiple of block size, or decrypt will return an error
	If we're still handshaking and sending plaintext, the decryption 
	callback will point to a null provider that passes the data unchanged
*/
	if (ssl->decrypt(&ssl->sec.decryptCtx, c, out->end, ssl->rec.len) < 0) {
		ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
		goto encodeResponse;
	}
	c += ssl->rec.len;
/*
	If we're reading a secure message, we need to validate the MAC and 
	padding (if using a block cipher).  Insecure messages do not have 
	a trailing MAC or any padding.

	SECURITY - There are several vulnerabilities in block cipher padding
	that we handle in the below code.  For more information see:
	http://www.openssl.org/~bodo/tls-cbc.txt
*/
	if (ssl->flags & SSL_FLAGS_READ_SECURE) {
/*
		Verify the record is at least as big as the MAC
		Start tracking MAC errors, rather then immediately catching them to
		stop timing and alert description attacks that differentiate between
		a padding error and a MAC error.
*/
		if (ssl->rec.len < ssl->cipher->macSize) {
			ssl->err = SSL_ALERT_BAD_RECORD_MAC;
			matrixStrDebugMsg("Record length too short for MAC\n", NULL);
			goto encodeResponse;
		}
		macError = 0;
/*
		Decode padding only if blocksize is > 0 (we're using a block cipher),
		otherwise no padding will be present, and the mac is the last 
		macSize bytes of the record.
*/
		if (ssl->cipher->blockSize <= 1) {
			mac = out->end + ssl->rec.len - ssl->cipher->macSize;
		} else {
/*
			Verify the pad data for block ciphers
			c points within the cipher text, p points within the plaintext
			The last byte of the record is the pad length
*/
			p = out->end + ssl->rec.len;
			padLen = *(p - 1);
/*
			SSL3.0 requires the pad length to be less than blockSize
*/
			if (ssl->majVer == SSL3_MAJ_VER && ssl->minVer == SSL3_MIN_VER && 
				padLen >= ssl->cipher->blockSize) {
				macError++;
			}
/*
			The minimum record length is the size of the mac, plus pad bytes
			plus one length byte
*/
			if (ssl->rec.len < ssl->cipher->macSize + padLen + 1) {
				macError++;
			}
/*
			The mac starts macSize bytes before the padding and length byte.
			If we have a macError, just fake the mac as the last macSize bytes
			of the record, so we are sure to have enough bytes to verify against,
			we'll fail anyway, so the actual contents don't matter.
*/
			if (!macError) {
				mac = p - padLen - 1 - ssl->cipher->macSize;
			} else {
				mac = out->end + ssl->rec.len - ssl->cipher->macSize;
			}
		}
/*
		Verify the MAC of the message by calculating our own MAC of the message
		and comparing it to the one in the message.  We do this step regardless
		of whether or not we've already set macError to stop timing attacks.
		Clear the mac in the callers buffer if we're successful
*/
		if (ssl->verifyMac(ssl, ssl->rec.type, out->end, 
			(int)(mac - out->end), mac) < 0 || macError) {
			ssl->err = SSL_ALERT_BAD_RECORD_MAC;
			matrixStrDebugMsg("Couldn't verify MAC or pad of record data \n", NULL);
			goto encodeResponse;
		}
		memset(mac, 0x0, ssl->cipher->macSize);
/*
		Record data starts at out->end and ends at mac
*/
		p = out->end;
		pend = mac;
	} else {
/*
		The record data is the entire record as there is no MAC or padding
*/
		p = out->end;
		pend = mac = out->end + ssl->rec.len;
	}

/*
	Take action based on the actual record type we're dealing with
	'p' points to the start of the data, and 'pend' points to the end
*/
	switch (ssl->rec.type) {
	case SSL_RECORD_TYPE_CHANGE_CIPHER_SPEC:
/*
		Body is single byte with value 1 to indicate that the next message
		will be encrypted using the negotiated cipher suite
*/
		if (pend - p < 1) {
			ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
			matrixStrDebugMsg("Invalid length for CipherSpec\n", NULL);
			goto encodeResponse;
		}
		if (*p == 1) {
			p++;
		} else {
			ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
			matrixStrDebugMsg("Invalid value for CipherSpec\n", NULL);
			goto encodeResponse;
		}
/*
		If we're expecting finished, then this is the right place to get
		this record.  It is really part of the handshake but it has its
		own record type.
		Activate the read cipher callbacks, so we will decrypt incoming
		data from now on.
*/
		if (ssl->hsState == SSL_HS_FINISHED) {
			sslActivateReadCipher(ssl);
		} else {
			ssl->err = SSL_ALERT_UNEXPECTED_MESSAGE;
			matrixIntDebugMsg("Invalid CipherSpec order: %d\n", ssl->hsState);
			goto encodeResponse;
		}
		in->start = c;
		goto decodeMore;

	case SSL_RECORD_TYPE_ALERT:
/*
		1 byte alert level (warning or fatal)
		1 byte alert description corresponding to SSL_ALERT_*
*/
		if (pend - p < 2) {
			ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
			matrixStrDebugMsg("Error in length of alert record\n", NULL);
			goto encodeResponse;
		}
		*alertLevel = *p; p++;
		*alertDescription = *p; p++;
/*
		If the alert is fatal, or is a close message (usually a warning),
		flag the session with ERROR so it cannot be used anymore.
		Caller can decide whether or not to close on other warnings.
*/
		if (*alertLevel == SSL_ALERT_LEVEL_FATAL) { 
			ssl->flags |= SSL_FLAGS_ERROR;
		}
		if (*alertDescription == SSL_ALERT_CLOSE_NOTIFY) {
			ssl->flags |= SSL_FLAGS_CLOSED;
		}
		return SSL_ALERT;

	case SSL_RECORD_TYPE_HANDSHAKE:
/*
		We've got one or more handshake messages in the record data.
		The handshake parsing function will take care of all messages
		and return an error if there is any problem.
		If there is a response to be sent (either a return handshake
		or an error alert, send it).  If the message was parsed, but no
		response is needed, loop up and try to parse another message
*/
		rc = parseSSLHandshake(ssl, p, (int)(pend - p));
		switch (rc) {
		case SSL_SUCCESS:
			in->start = c;
			return SSL_SUCCESS;
		case SSL_PROCESS_DATA:
			in->start = c;
			goto encodeResponse;
		case SSL_ERROR:
			goto encodeResponse;
		}
		break;

	case SSL_RECORD_TYPE_APPLICATION_DATA:
/*
		Data is in the out buffer, let user handle it
		Don't allow application data until handshake is complete, and we are
		secure.
*/
		if (ssl->hsState != SSL_HS_DONE || !(ssl->flags & SSL_FLAGS_READ_SECURE)) {
			ssl->err = SSL_ALERT_UNEXPECTED_MESSAGE;
			matrixIntDebugMsg("Incomplete handshake: %d\n", ssl->hsState);
			goto encodeResponse;
		}
/*
		SECURITY - If the mac is at the current out->end, then there is no data 
		in the record.  These records are valid, but are usually not sent by
		the application layer protocol.  Rather, they are initiated within the 
		remote SSL protocol implementation to avoid some types of attacks when
		using block ciphers.  For more information see:
		http://www.openssl.org/~bodo/tls-cbc.txt

		We eat these records here rather than passing them on to the caller.
		The rationale behind this is that if the caller's application protocol 
		is depending on zero length SSL messages, it will fail anyway if some of 
		those messages are initiated within the SSL protocol layer.  Also
		this clears up any confusion where the caller might interpret a zero
		length read as an end of file (EOF) or would block (EWOULDBLOCK) type
		scenario.

		SECURITY - Looping back up and ignoring the message has the potential for
		denial of service, because we are not changing the state of the system in
		any way when processing these messages.  To counteract this, we maintain
		a counter that we share with other types of ignored messages.
*/
		in->start = c;
		if (out->end == mac) {
			if (ssl->ignoredMessageCount++ < SSL_MAX_IGNORED_MESSAGE_COUNT) {
				goto decodeMore;
			}
			ssl->err = SSL_ALERT_UNEXPECTED_MESSAGE;
			matrixIntDebugMsg("Exceeded limit on ignored messages: %d\n", 
				SSL_MAX_IGNORED_MESSAGE_COUNT);
			goto encodeResponse;
		}
		out->end = mac;
		return SSL_PROCESS_DATA;
	}
/*
	Should not get here
*/
	matrixIntDebugMsg("Invalid record type in matrixSslDecode: %d\n",
		ssl->rec.type);
	return SSL_ERROR;

encodeResponse:
/*
	We decoded a record that needs a response, either a handshake response
	or an alert if we've detected an error.  
	SECURITY - Clear the decoded incoming record from outbuf before encoding
	the response into outbuf.  rec.len could be invalid, clear the minimum 
	of rec.len and remaining outbuf size
*/
	rc = min (ssl->rec.len, (int)((out->buf + out->size) - out->end));
	if (rc > 0) {
		memset(out->end, 0x0, rc);
	}
	rc = sslEncodeResponse(ssl, out);
	if (rc == SSL_SUCCESS) {
		if (ssl->err != SSL_ALERT_NONE) {
			*error = (unsigned char)ssl->err;
			ssl->flags |= SSL_FLAGS_ERROR;
			return SSL_ERROR;
		}
		return SSL_SEND_RESPONSE;
	}
	if (rc == SSL_FULL) {
		ssl->flags |= SSL_FLAGS_NEED_ENCODE;
		return SSL_FULL;
	}
	return SSL_ERROR;
}

/******************************************************************************/
/*
	Parameters:
	ssl - ssl context
	inbuf - buffer to read handshake message from
	len - data length for the current ssl record.  The ssl record
		can contain multiple handshake messages, so we may need to parse
		them all here.
	Return:
		SSL_SUCCESS
		SSL_PROCESS_DATA
		SSL_ERROR - see ssl->err for details
*/
static int parseSSLHandshake(ssl_t *ssl, char *inbuf, int len)
{
	unsigned char	*c;
	unsigned char	*end;
	unsigned char	hsType;
	int				hsLen, rc;
	unsigned int	cipher;
	unsigned char	*hsMsgHash;
#ifdef USE_SERVER_SIDE_SSL
	unsigned char	*p;
		int			suiteLen, challengeLen, pubKeyLen, extLen;
#endif /* USE_SERVER_SIDE_SSL */
#ifdef USE_CLIENT_SIDE_SSL
	int				certLen, certChainLen, sessionIdLen;
#endif /* USE_CLIENT_SIDE_SSL */

	rc = SSL_SUCCESS;
	c = inbuf;
	end = inbuf + len;

parseHandshake:
	if (end - c < 1) {
		ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
		matrixStrDebugMsg("Invalid length of handshake message\n", NULL);
		return SSL_ERROR;
	}
	hsType = *c; c++;
/*
	ssl->hsState is the expected handshake type.  If it doesn't match,
	or we aren't doing a rehandshake, error
*/
	if (hsType != ssl->hsState && 
			(hsType != SSL_HS_CLIENT_HELLO || ssl->hsState != SSL_HS_DONE)) {
		ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
		matrixIntDebugMsg("Invalid type of handshake message: %d\n", hsType);
		return SSL_ERROR;
	}
/*
	We need to get a copy of the message hashes to compare to those sent
	in the finished message (which does not include a hash of itself)
	before we update the handshake hashes
*/
	if (ssl->hsState == SSL_HS_FINISHED) {
		hsMsgHash = sslMalloc(SSL_MD5_HASH_SIZE + SSL_SHA1_HASH_SIZE);
		sslSnapshotHSHash(ssl, hsMsgHash, 
			(ssl->flags & SSL_FLAGS_SERVER) ? 0 : SSL_FLAGS_SERVER);
	}
/*
	Process the handshake header and update the ongoing handshake hash
	SSLv3:
		1 byte type
		3 bytes length
	SSLv2:
		1 byte type
*/
	if (ssl->rec.majVer == SSL3_MAJ_VER) {
		if (end - c < 3) {
			ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
			matrixStrDebugMsg("Invalid length of handshake message\n", NULL);
			return SSL_ERROR;
		}
		hsLen = *c << 16; c++;
		hsLen += *c << 8; c++;
		hsLen += *c; c++;
		if (end - c < hsLen) {
			ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
			matrixStrDebugMsg("Invalid handshake length\n", NULL);
			return SSL_ERROR;
		}
		sslUpdateHSHash(ssl, c - SSL3_HANDSHAKE_HEADER_LEN,
			hsLen + SSL3_HANDSHAKE_HEADER_LEN);
	} else if (ssl->rec.majVer == SSL2_MAJ_VER) {
/*
		Assume that the handshake len is the same as the incoming ssl record
		length minus 1 byte (type), this is verified in SSL_HS_CLIENT_HELLO
*/
		hsLen = len - 1;
		sslUpdateHSHash(ssl, inbuf, len);
	} else {
		ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
		matrixIntDebugMsg("Invalid record version: %d\n", ssl->rec.majVer);
		return SSL_ERROR;
	}
/*
	Process each type of handshake message.  Note we switch on the
	ssl->hsState, not the message hsState because we support internal
	states beyond those defined in SSL (namely SSL_HS_DONE).
*/
	switch(ssl->hsState) {
#ifdef USE_SERVER_SIDE_SSL
	case SSL_HS_CLIENT_HELLO:
/*
		First two bytes are the highest supported major and minor SSL versions
		We support only 3.0 (other options are 2.0 or 3.1)
*/
		if (end - c < 2) {
			ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
			matrixStrDebugMsg("Invalid ssl header version length\n", NULL);
			return SSL_ERROR;
		}
		ssl->reqMajVer = *c; c++;
		ssl->reqMinVer = *c; c++;

		if (ssl->reqMajVer == SSL3_MAJ_VER) {
			ssl->majVer = SSL3_MAJ_VER;
			ssl->minVer = SSL3_MIN_VER;
		} else {
			ssl->err = SSL_ALERT_HANDSHAKE_FAILURE;
			matrixIntDebugMsg("Unsupported ssl version: %d\n", ssl->reqMajVer);
			return SSL_ERROR;
		}
/*
		Support SSLv3 and SSLv2 ClientHello messages.  Browsers usually send v2
		messages for compatibility
*/
		if (ssl->rec.majVer > SSL2_MAJ_VER) {
/*
			Next is a 32 bytes of random data for key generation
			and a single byte with the session ID length
*/
			if (end - c < SSL_HS_RANDOM_SIZE + 1) {
				ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
				matrixStrDebugMsg("Invalid length of random data\n", NULL);
				return SSL_ERROR;
			}
			ssl->sec.clientRandom = sslMalloc(SSL_HS_RANDOM_SIZE);
			memcpy(ssl->sec.clientRandom, c, SSL_HS_RANDOM_SIZE);
			c += SSL_HS_RANDOM_SIZE;
			ssl->sessionIdLen = *c; c++;
/*
			If a session length was specified, the client is asking to
			resume a previously established session to speed up the handshake
*/
			if (ssl->sessionIdLen > 0) {
				if (ssl->sessionIdLen > SSL_MAX_SESSION_ID_SIZE || 
						end - c < ssl->sessionIdLen) {
					ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
					return SSL_ERROR;
				}
				ssl->sessionId = sslMalloc(ssl->sessionIdLen);
				memcpy(ssl->sessionId, c, ssl->sessionIdLen);
				c += ssl->sessionIdLen;
/*
				Look up the session id for ssl session resumption.  If found, we
				load the pre-negotiated masterSecret and cipher.
				A resumed request must meet the following restrictions:
					The id must be present in the lookup table
					The requested version must match the original version
					The cipher suite list must contain the original cipher suite
*/
				if (resumeSession(ssl) >= 0) {
					ssl->flags |= SSL_FLAGS_RESUMED;
				} else {
					sslFree(ssl->sessionId);
					ssl->sessionId = NULL;
					ssl->sessionIdLen = 0;
				}
			}
/*
			Next is the two byte cipher suite list length, network byte order.  
			It must not be zero, and must be a multiple of two.
*/
			if (end - c < 2) {
				ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
				matrixStrDebugMsg("Invalid cipher suite list length\n", NULL);
				return SSL_ERROR;
			}
			suiteLen = *c << 8; c++;
			suiteLen += *c; c++;
			if (suiteLen == 0 || suiteLen & 1) {
				ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
				matrixIntDebugMsg("Unable to parse cipher suite list: %d\n", 
					suiteLen);
				return SSL_ERROR;
			}
/*
			Now is 'suiteLen' bytes of the supported cipher suite list,
			listed in order of preference.  Loop through and find the 
			first cipher suite we support.
*/
			if (end - c < suiteLen) {
				ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
				return SSL_ERROR;
			}
			p = c + suiteLen;
			while (c < p) {
				cipher = *c << 8; c++;
				cipher += *c; c++;
/*
				A resumed session can only match the cipher originally 
				negotiated. Otherwise, match the first cipher that we support
*/
				if (ssl->flags & SSL_FLAGS_RESUMED) {
					sslAssert(ssl->cipher);
					if (ssl->cipher->id == cipher) {
						c = p;
						break;
					}
				} else {
					if ((ssl->cipher = sslGetCipherSpec(cipher)) != NULL) {
						c = p;
						break;
					}
				}
			}
/*
			If we fell to the default cipher suite, we didn't have
			any in common with the client, or the client is being bad
			and requesting the null cipher!
*/
			if (ssl->cipher == NULL || ssl->cipher->id != cipher || 
					cipher == SSL_NULL_WITH_NULL_NULL) {
				ssl->cipher = sslGetCipherSpec(SSL_NULL_WITH_NULL_NULL);
				ssl->err = SSL_ALERT_HANDSHAKE_FAILURE;
				matrixStrDebugMsg("Can't support requested cipher\n", NULL);
				return SSL_ERROR;
			}
/*
			Decode the compression parameters.  Always length one (first byte)
			and value 0 (second byte).  There are no compression schemes defined
			for SSLv3
*/
			if (end - c < 1) {
				ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
				matrixStrDebugMsg("Invalid compression header length\n", NULL);
				return SSL_ERROR;
			}
			extLen = *c++;
			if (end - c < extLen) {
				ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
				matrixStrDebugMsg("Invalid compression header length\n", NULL);
				return SSL_ERROR;
			}
			c += extLen;

			if (ssl->reqMinVer == SSL3_MIN_VER && extLen != 1) {
				ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
				matrixStrDebugMsg("Invalid compression header\n", NULL);
				return SSL_ERROR;
			}
		} else {
/*
			Parse a SSLv2 ClientHello message.  The same information is 
			conveyed but the order and format is different.
			First get the cipher suite length, session id length and challenge
			(client random) length - all two byte values, network byte order.
*/
			if (end - c < 6) {
				ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
				matrixStrDebugMsg("Can't parse hello message\n", NULL);
				return SSL_ERROR;
			}
			suiteLen = *c << 8; c++;
			suiteLen += *c; c++;
			if (suiteLen == 0 || suiteLen % 3 != 0) {
				ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
				matrixStrDebugMsg("Can't parse hello message\n", NULL);
				return SSL_ERROR;
			}
			ssl->sessionIdLen = *c << 8; c++;
			ssl->sessionIdLen += *c; c++;
/*
			A resumed session would use a SSLv3 ClientHello, not SSLv2.
*/
			if (ssl->sessionIdLen != 0) {
				ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
				matrixStrDebugMsg("Bad resumption request\n", NULL);
				return SSL_ERROR;
			}
			challengeLen = *c << 8; c++;
			challengeLen += *c; c++;
			if (challengeLen < 16 || challengeLen > 32) {
				matrixStrDebugMsg("Bad challenge length\n", NULL);
				ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
				return SSL_ERROR;
			}
/*
			Validate the three lengths that were just sent to us, don't
			want any buffer overflows while parsing the remaining data
*/
			if (end - c != suiteLen + ssl->sessionIdLen + challengeLen) {
				ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
				return SSL_ERROR;
			}
/*
			Parse the cipher suite list similar to the SSLv3 method, except
			each suite is 3 bytes, instead of two bytes.  We define the suite
			as an integer value, so either method works for lookup.
			We don't support session resumption from V2 handshakes, so don't 
			need to worry about matching resumed cipher suite.
*/
			p = c + suiteLen;
			while (c < p) {
				cipher = *c << 16; c++;
				cipher += *c << 8; c++;
				cipher += *c; c++;
				if ((ssl->cipher = sslGetCipherSpec(cipher)) != NULL) {
					c = p;
					break;
				}
			}
			if (ssl->cipher == NULL || ssl->cipher->id == SSL_NULL_WITH_NULL_NULL) {
				ssl->cipher = sslGetCipherSpec(SSL_NULL_WITH_NULL_NULL);
				ssl->err = SSL_ALERT_HANDSHAKE_FAILURE;
				matrixStrDebugMsg("Can't support requested cipher\n", NULL);
				return SSL_ERROR;
			}
/*
			We don't allow session IDs for v2 ClientHellos
*/
			if (ssl->sessionIdLen > 0) {
				ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
				matrixStrDebugMsg("SSLv2 sessions not allowed\n", NULL);
				return SSL_ERROR;
			}
/*
			The client random (between 16 and 32 bytes) fills the least 
			significant bytes in the (always) 32 byte SSLv3 client random field.
*/
			ssl->sec.clientRandom = sslMalloc(SSL_HS_RANDOM_SIZE);
			memset(ssl->sec.clientRandom, 0x0, SSL_HS_RANDOM_SIZE);
			memcpy(ssl->sec.clientRandom + (SSL_HS_RANDOM_SIZE - challengeLen), 
				c, challengeLen);
			c += challengeLen;
		}
/*
		ClientHello should be the only one in the record.
*/
		if (c != end) {
			ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
			matrixStrDebugMsg("Invalid final client hello length\n", NULL);
			return SSL_ERROR;
		}
/*
		If we're resuming a handshake, then the next handshake message we
		expect is the finished message.  Otherwise we do the full handshake.
*/
		if (ssl->flags & SSL_FLAGS_RESUMED) {
			ssl->hsState = SSL_HS_FINISHED;
		} else {
			ssl->hsState = SSL_HS_CLIENT_KEY_EXCHANGE;
		}
/*
		Now that we've parsed the ClientHello, we need to tell the caller that
		we have a handshake response to write out.
		The caller should call sslWrite upon receiving this return code.
*/
		rc = SSL_PROCESS_DATA;
		break;

	case SSL_HS_CLIENT_KEY_EXCHANGE:
/*
		This message contains the premaster secret encrypted with the 
		server's public key (from the Certificate).  The premaster
		secret is 48 bytes of random data, but the message may be longer
		than that because the 48 bytes are padded before encryption 
		according to PKCS#1v1.5.  After encryption, we should have the 
		correct length.
*/
		if (end - c < hsLen) {
			ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
			matrixStrDebugMsg("Invalid ClientKeyExchange length\n", NULL);
			return SSL_ERROR;
		}
		sslActivatePublicCipher(ssl);
		ssl->sec.premaster = sslMalloc(SSL_HS_PREMASTER_SIZE);

		pubKeyLen = hsLen;
/*
		FUTURE SECURITY- Wrap this call with RSA blinding 
*/
		if (ssl->decryptPriv(ssl->keys->privKey, c, pubKeyLen,
				ssl->sec.premaster, SSL_HS_PREMASTER_SIZE) != 
				SSL_HS_PREMASTER_SIZE) {
			ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
			matrixStrDebugMsg("Failed to decrypt premaster\n", NULL);
			return SSL_ERROR;
		}
/*
		The first two bytes of the decrypted message should be the client's 
		requested version number (which may not be the same as the final 
		negotiated version). The other 46 bytes - pure random!
		SECURITY - 
		Some SSL clients (Including Microsoft IE 6.0) incorrectly set the first
		two bytes to the negotiated version rather than the requested version.
		This is known in OpenSSL as the SSL_OP_TLS_ROLLBACK_BUG
		We allow this to slide since we don't support TLS, TLS was requested
		and the negotiated versions match.
*/
		if (*ssl->sec.premaster != ssl->reqMajVer) {
			ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
			matrixStrDebugMsg("Incorrect version in ClientKeyExchange\n", NULL);
			return SSL_ERROR;
		}
		if (*(ssl->sec.premaster + 1) != ssl->reqMinVer) {
			if (ssl->reqMinVer != 1 || *(ssl->sec.premaster + 1) != ssl->minVer) {
				ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
				matrixStrDebugMsg("Incorrect version in ClientKeyExchange\n", NULL);
				return SSL_ERROR;
			}
		}
/*
		Now that we've got the premaster secret, derive the various symmetric
		keys using it and the client and server random values
*/
		sslDeriveKeys(ssl);
/*
		At this point, we've generated the masterSecret, so update the cached
		session (if found) with the masterSecret and negotiated cipher.
*/
		updateSession(ssl);

		c += hsLen;
		ssl->hsState = SSL_HS_FINISHED;
		break;
#endif /* USE_SERVER_SIDE_SSL */
	case SSL_HS_FINISHED:
/*
		Before the finished handshake message, we should have seen the
		CHANGE_CIPHER_SPEC message come through in the record layer, which
		would have activated the read cipher, and set the READ_SECURE flag.
		This is the first handshake message that was sent securely.
*/
		if (!(ssl->flags & SSL_FLAGS_READ_SECURE)) {
			sslFree(hsMsgHash);
			ssl->err = SSL_ALERT_UNEXPECTED_MESSAGE;
			matrixStrDebugMsg("Finished before ChangeCipherSpec\n", NULL);
			return SSL_ERROR;
		}
/*
		The contents of the finished message is a 16 byte MD5 hash followed
		by a 20 byte sha1 hash of all the handshake messages so far, to verify
		that nothing has been tampered with while we were still insecure.
		Compare the message to the value we calculated at the beginning of
		this function.
*/
		if (hsLen != SSL_MD5_HASH_SIZE + SSL_SHA1_HASH_SIZE) {
			ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
			matrixStrDebugMsg("Invalid Finished length\n", NULL);
			return SSL_ERROR;
		}

		if (end - c < hsLen) {
			ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
			matrixStrDebugMsg("Invalid Finished length\n", NULL);
			return SSL_ERROR;
		}
		if (memcmp(c, hsMsgHash, hsLen) != 0) {
			sslFree(hsMsgHash);
			ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
			matrixStrDebugMsg("Invalid handshake msg hash\n", NULL);
			return SSL_ERROR;
		}
		sslFree(hsMsgHash);
		c += hsLen;
		ssl->hsState = SSL_HS_DONE;
/*
		Now that we've parsed the Finished message, if we're a resumed 
		connection, we're done with handshaking, otherwise, we return
		SSL_PROCESS_DATA to get our own cipher spec and finished messages
		sent out by the caller.
*/
		if (ssl->flags & SSL_FLAGS_SERVER) {
			if (!(ssl->flags & SSL_FLAGS_RESUMED)) {
				rc = SSL_PROCESS_DATA;
			}
		} else {
			if (ssl->flags & SSL_FLAGS_RESUMED) {
				rc = SSL_PROCESS_DATA;
			}
		}
		break;

	case SSL_HS_DONE:
/*
		The only way we get here is if we've completed the handshake, but
		the client sent a new ClientHello message anyway in order to
		renegotiate random values and ciphers.  Rehandshake case.
*/
		if (hsType != SSL_HS_CLIENT_HELLO) {
			ssl->err = SSL_ALERT_UNEXPECTED_MESSAGE;
			matrixStrDebugMsg("Received unexpected done message\n", NULL);
			return SSL_ERROR;
		}
/*
		Reset the context to expect the ClientHello, and we'll loop 
		back up to process the ClientHello message.
		SECURITY - Per spec, we just ignore the message. But any incoming
		data really should be processed by our layer or the application
		layer to avoid denial of service.  Put another way, the state of the
		system should change in some way for every message processed.
		To this end, we ignore failed renegotiations to a point, but will
		eventually close the connection if they exceed a sanity value.
*/
		if (sslResetContext(ssl) < 0) {
			if (ssl->ignoredMessageCount++ >= SSL_MAX_IGNORED_MESSAGE_COUNT) {
				ssl->err = SSL_ALERT_UNEXPECTED_MESSAGE;
				matrixIntDebugMsg("Exceeded limit on ignored messages: %d\n", 
					SSL_MAX_IGNORED_MESSAGE_COUNT);
				return SSL_ERROR;
			}
		}
		c += hsLen;
		break;

	case SSL_HS_HELLO_REQUEST:
		/* FUTURE - implement for server initiated rehandshake */
#ifdef USE_CLIENT_SIDE_SSL
	case SSL_HS_SERVER_HELLO: 

/*
		First two bytes are the negotiated SSL version
		We support only 3.0 (other options are 2.0 or 3.1)
*/
		if (end - c < 2) {
			ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
			matrixStrDebugMsg("Invalid ssl header version length\n", NULL);
			return SSL_ERROR;
		}
		ssl->reqMajVer = *c; c++;
		ssl->reqMinVer = *c; c++;
/*
		Check for TLS support
*/
		if (ssl->reqMajVer != ssl->majVer) {
			ssl->err = SSL_ALERT_HANDSHAKE_FAILURE;
			matrixIntDebugMsg("Unsupported ssl version: %d\n", ssl->reqMajVer);
			return SSL_ERROR;
		}	
/*
		Next is a 32 bytes of random data for key generation
		and a single byte with the session ID length
*/
		if (end - c < SSL_HS_RANDOM_SIZE + 1) {
			ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
			matrixStrDebugMsg("Invalid length of random data\n", NULL);
			return SSL_ERROR;
		}
		ssl->sec.serverRandom = sslMalloc(SSL_HS_RANDOM_SIZE);
		memcpy(ssl->sec.serverRandom, c, SSL_HS_RANDOM_SIZE);
		c += SSL_HS_RANDOM_SIZE;
		sessionIdLen = *c; c++;
		if (sessionIdLen > SSL_MAX_SESSION_ID_SIZE || 
				end - c < sessionIdLen) {
			ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
			return SSL_ERROR;
		}
/*
		If a session length was specified, the server has sent us a
		session Id.  We may have requested a specific session, and the
		server may or may not agree to use that session.
*/
		if (sessionIdLen > 0) {
			if (ssl->sessionIdLen > 0) {
				if (memcmp(ssl->sessionId, c, sessionIdLen) == 0) {
					ssl->flags |= SSL_FLAGS_RESUMED;
				} else {
					ssl->cipher = sslGetCipherSpec(SSL_NULL_WITH_NULL_NULL);
					memset(ssl->sec.masterSecret, 0x0, SSL_HS_MASTER_SIZE);
					ssl->sessionIdLen = sessionIdLen;
					memcpy(ssl->sessionId, c, sessionIdLen);
				}
			} else {
				ssl->sessionIdLen = sessionIdLen;
				ssl->sessionId = sslMalloc(sessionIdLen);
				memcpy(ssl->sessionId, c, sessionIdLen);
			}
			c += sessionIdLen;
		} else {
			if (ssl->sessionIdLen > 0) {
				ssl->cipher = sslGetCipherSpec(SSL_NULL_WITH_NULL_NULL);
				memset(ssl->sec.masterSecret, 0x0, SSL_HS_MASTER_SIZE);
				ssl->sessionIdLen = 0;
				sslFree(ssl->sessionId);
				ssl->sessionId = NULL;
			}
		}
/*
		Next is the two byte cipher suite
*/
		if (end - c < 2) {
			ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
			matrixStrDebugMsg("Invalid cipher suite length\n", NULL);
			return SSL_ERROR;
		}
		cipher = *c << 8; c++;
		cipher += *c; c++;
/*
		A resumed session can only match the cipher originally 
		negotiated. Otherwise, match the first cipher that we support
*/
		if (ssl->flags & SSL_FLAGS_RESUMED) {
			sslAssert(ssl->cipher);
			if (ssl->cipher->id != cipher) {
				ssl->err = SSL_ALERT_HANDSHAKE_FAILURE;
				matrixStrDebugMsg("Can't support resumed cipher\n", NULL);
				return SSL_ERROR;
			}
		} else {
			if ((ssl->cipher = sslGetCipherSpec(cipher)) == NULL) {
				ssl->err = SSL_ALERT_HANDSHAKE_FAILURE;
				matrixStrDebugMsg("Can't support requested cipher\n", NULL);
				return SSL_ERROR;
			}
		}
/*
		Decode the compression parameters.  Always zero.
		There are no compression schemes defined for SSLv3
*/
		if (end - c < 1 || *c != 0) {
			ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
			matrixStrDebugMsg("Invalid compression value\n", NULL);
			return SSL_ERROR;
		}
/*
		At this point, if we're resumed, we have all the required info
		to derive keys.  The next handshake message we expect is
		the Finished message.
*/
		c++;
		if (ssl->flags & SSL_FLAGS_RESUMED) {
			sslDeriveKeys(ssl);
			ssl->hsState = SSL_HS_FINISHED;
		} else {
			ssl->hsState = SSL_HS_CERTIFICATE;
		}
		break;
#endif /* USE_CLIENT_SIDE_SSL */

#if defined(USE_CLIENT_SIDE_SSL) || defined(USE_CLIENT_AUTH)
	case SSL_HS_CERTIFICATE: 

		if (end - c < 6) {
			ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
			matrixStrDebugMsg("Invalid Certificate message\n", NULL);
			return SSL_ERROR;
		}
		certChainLen = *c << 16; c++;
		certChainLen |= *c << 8; c++;
		certChainLen |= *c; c++;
		certLen = *c << 16; c++;
		certLen |= *c << 8; c++;
		certLen |= *c; c++;
/*
		We currently do not support more than one certificate in the sequence
*/
		if (certLen != certChainLen - 3) {
			ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
			matrixStrDebugMsg("Unsupported certificate format\n", NULL);
			return SSL_ERROR;
		}
		if (end - c < certLen) {
			ssl->err = SSL_ALERT_ILLEGAL_PARAMETER;
			matrixStrDebugMsg("Invalid certificate length\n", NULL);
			return SSL_ERROR;
		}
		if (matrixX509ParseCert(&c, certLen, &ssl->sec.cert) < 0) {
			ssl->err = SSL_ALERT_BAD_CERTIFICATE;
			matrixStrDebugMsg("Can't parse certificate\n", NULL);
			return SSL_ERROR;
		}
/*
		There may not be a caCert set by the client.  The validate
		implemenation will just take the subject cert and make sure
		it is a self signed cert.
*/
		if (matrixX509ValidateCert(ssl->sec.cert, 
			ssl->keys == NULL ? NULL : ssl->keys->caCerts) < 0) {
			ssl->err = SSL_ALERT_BAD_CERTIFICATE;
			matrixStrDebugMsg("Error validating certificate\n", NULL);
			return SSL_ERROR;
		}
		if (ssl->sec.cert->valid < 0) {
			matrixStrDebugMsg(
				"Warning: Cert did not pass default validation checks\n", NULL);
		}
		if (matrixX509UserValidator(ssl->sec.cert, ssl->sec.validateCert) < 0) {
			ssl->err = SSL_ALERT_BAD_CERTIFICATE;
			return SSL_ERROR;
		}
		ssl->hsState = SSL_HS_SERVER_HELLO_DONE;
		break;
#endif /* USE_CLIENT_SIDE_SSL || USE_CLIENT_AUTH */

#ifdef USE_CLIENT_SIDE_SSL
	case SSL_HS_SERVER_HELLO_DONE: 
		if (hsLen != 0) {
			ssl->err = SSL_ALERT_BAD_CERTIFICATE;
			matrixStrDebugMsg("Can't validate certificate\n", NULL);
			return SSL_ERROR;
		}
		ssl->hsState = SSL_HS_FINISHED;
		rc = SSL_PROCESS_DATA;
		break;
#endif /* USE_CLIENT_SIDE_SSL */

	case SSL_HS_SERVER_KEY_EXCHANGE: 
		/* FUTURE - implement for Diffie or Ephemeral RSA */
		ssl->err = SSL_ALERT_UNEXPECTED_MESSAGE;
		return SSL_ERROR;
	case SSL_HS_CERTIFICATE_REQUEST: 
		/* FUTURE - implement for client authentication */
		ssl->err = SSL_ALERT_UNEXPECTED_MESSAGE;
		return SSL_ERROR;
	case SSL_HS_CERTIFICATE_VERIFY: 
		/* FUTURE - implement for client authentication */
		ssl->err = SSL_ALERT_UNEXPECTED_MESSAGE;
		return SSL_ERROR;
	default:
		ssl->err = SSL_ALERT_UNEXPECTED_MESSAGE;
		return SSL_ERROR;
	}
/*
	if we've got more data in the record, the sender has packed
	multiple handshake messages in one record.  Parse the next one.
*/
	if (c < end) {
		goto parseHandshake;
	}
	return rc;
}

/******************************************************************************/
/*
	Returns 1 if we've completed the SSL handshake.  0 if we're in process.
*/
int matrixSslHandshakeIsComplete(ssl_t *ssl)
{
	return (ssl->hsState == SSL_HS_DONE) ? 1 : 0;
}

/******************************************************************************/
/*
	We indicate to the caller through return codes in sslDecode when we need
	to write internal data to the remote host.  The caller will call this 
	function to generate a message appropriate to our state.
*/
static int sslEncodeResponse(ssl_t *ssl, sslBuf_t *out)
{
	unsigned char	padLen;
	int				messageSize;
	int				rc = SSL_ERROR;
/*
	We may be trying to encode an alert response if there is an error marked
	on the connection.
*/
	if (ssl->err != SSL_ALERT_NONE) {
		rc = writeAlert(ssl, SSL_ALERT_LEVEL_FATAL, ssl->err, out);
		if (rc == SSL_ERROR) {
			ssl->flags |= SSL_FLAGS_ERROR;
		}
		return rc;
	}
/*
	We encode a set of response messages based on our current state
	We have to pre-verify the size of the outgoing buffer against
	all the messages to make the routine transactional.  If the first
	write succeeds and the second fails because of size, we cannot
	rollback the state of the cipher and MAC.

	FUTURE - handle roundup to block cipher size in all calculations
	FUTURE - need to factor in current macSize if re-negotiating session
*/
	switch (ssl->hsState) {
/*
	If we're waiting for the ClientKeyExchange message, then we need to
	send the messages that would prompt that result on the client
*/
#ifdef USE_SERVER_SIDE_SSL
	case SSL_HS_CLIENT_KEY_EXCHANGE:
		messageSize =
			3 * SSL3_HEADER_LEN +
			3 * SSL3_HANDSHAKE_HEADER_LEN +
			38 + SSL_MAX_SESSION_ID_SIZE +
			6 + ssl->keys->certLen;	
		if ((out->buf + out->size) - out->end < messageSize) {
			return SSL_FULL;
		}
		rc = writeServerHello(ssl, out);
		if (rc == SSL_SUCCESS) {
			rc = writeCertificate(ssl, out);
		}
		if (rc == SSL_SUCCESS) {
			rc = writeServerHelloDone(ssl, out);
		}
		break;
#endif /* USE_SERVER_SIDE_SSL */

/*
	If we're not waiting for any message from client, then we need to
	send our finished message
	FUTURE - The above isn't true for server HelloRequest message
*/
	case SSL_HS_DONE:
		messageSize =
			SSL3_HANDSHAKE_HEADER_LEN +
			SSL_MD5_HASH_SIZE + SSL_SHA1_HASH_SIZE +
			ssl->cipher->macSize;
		padLen = sslPadLenPwr2(messageSize, ssl->cipher->blockSize);
		messageSize += padLen + SSL3_HEADER_LEN;
		messageSize +=
			SSL3_HEADER_LEN +
			1;
		if ((out->buf + out->size) - out->end < messageSize) {
			return SSL_FULL;
		}
		rc = writeChangeCipherSpec(ssl, out);
		if (rc == SSL_SUCCESS) {
			rc = writeFinished(ssl, out);
		}
		break;
/*
	If we're expecting a Finished message, as a server we're doing 
	session resumption.  As a client, we're completing a normal
	handshake
*/
	case SSL_HS_FINISHED:
#ifdef USE_SERVER_SIDE_SSL
		if (ssl->flags & SSL_FLAGS_SERVER) {
			messageSize =
				3 * SSL3_HEADER_LEN +
				2 * SSL3_HANDSHAKE_HEADER_LEN +
				38 + SSL_MAX_SESSION_ID_SIZE +
				1 +
				SSL_MD5_HASH_SIZE + SSL_SHA1_HASH_SIZE +
				ssl->cipher->macSize;
			if ((out->buf + out->size) - out->end < messageSize) {
				return SSL_FULL;
			}
			rc = writeServerHello(ssl, out);
			if (rc == SSL_SUCCESS) {
				rc = writeChangeCipherSpec(ssl, out);
			}
			if (rc == SSL_SUCCESS) {
				rc = writeFinished(ssl, out);
			}
		}
#endif /* USE_SERVER_SIDE_SSL */
#ifdef USE_CLIENT_SIDE_SSL
		if (!(ssl->flags & SSL_FLAGS_SERVER)) {
			if (ssl->sec.cert == NULL) {
				ssl->flags |= SSL_FLAGS_ERROR;
				return SSL_ERROR;
			}
			messageSize =
				3 * SSL3_HEADER_LEN +
				2 * SSL3_HANDSHAKE_HEADER_LEN +
				ssl->sec.cert->publicKey.size +
				1 +
				SSL_MD5_HASH_SIZE + SSL_SHA1_HASH_SIZE +
				ssl->cipher->macSize;
			if ((out->buf + out->size) - out->end < messageSize) {
				return SSL_FULL;
			}
			rc = writeClientKeyExchange(ssl, out);
			if (rc == SSL_SUCCESS) {
				rc = writeChangeCipherSpec(ssl, out);
			}
			if (rc == SSL_SUCCESS) {
				rc = writeFinished(ssl, out);
			}
		}
#endif /* USE_CLIENT_SIDE_SSL */
		break;
	}
	if (rc == SSL_ERROR) {
		ssl->flags |= SSL_FLAGS_ERROR;
	}
	return rc;
}

/******************************************************************************/
/*
	Encode the incoming data into the out buffer for sending to remote peer.

	FUTURE SECURITY - If sending the first application data record, we could
	prepend it with a blank SSL record to avoid a timing attack.  We're fine
	for now, because this is an impractical attack and the solution is 
	incompatible with some SSL implementations (including some versions of IE).
	http://www.openssl.org/~bodo/tls-cbc.txt
*/
int matrixSslEncode(ssl_t *ssl, unsigned char *in, int inlen, sslBuf_t *out)
{
	unsigned char	*c, *end, *encryptStart, padLen;
	int				messageSize;
/*
	If we've had a protocol error, don't allow further use of the session
	Also, don't allow a application data record to be encoded unless the
	handshake is complete.
*/
	if (ssl->flags & SSL_FLAGS_ERROR || ssl->hsState != SSL_HS_DONE ||
			ssl->flags & SSL_FLAGS_CLOSED) {
		return SSL_ERROR;
	}
	c = out->end;
	end = out->buf + out->size;
	messageSize =
		inlen +
		ssl->cipher->macSize;
	padLen = sslPadLenPwr2(messageSize, ssl->cipher->blockSize);
	messageSize += padLen + SSL3_HEADER_LEN;
	if (end - c < messageSize) {
		return SSL_FULL;
	}
	c += writeRecordHeader(ssl, SSL_RECORD_TYPE_APPLICATION_DATA, 
		messageSize - SSL3_HEADER_LEN, c);
	encryptStart = c;
	memcpy(c, in, inlen);
	c += inlen;
	c += ssl->generateMac(ssl, SSL_RECORD_TYPE_APPLICATION_DATA, 
		encryptStart, (int)(c - encryptStart), c);
	c += sslWritePad(c, padLen);
	if (ssl->encrypt(&ssl->sec.encryptCtx, encryptStart, encryptStart, 
			(int)(c - encryptStart)) < 0) {
		matrixStrDebugMsg("Error encrypting message\n", NULL);
		ssl->flags |= SSL_FLAGS_ERROR;
		return SSL_ERROR;
	}
	out->end = c;
	return (int)(out->end - out->start);
}

/******************************************************************************/
/*
	Write out a closure alert message (the only user initiated alert)
	The user would call this when about to initate a socket close
*/
int matrixSslEncodeClosureAlert(ssl_t *ssl, sslBuf_t *out)
{
/*
	If we've had a protocol error, don't allow further use of the session
*/
	if (ssl->flags & SSL_FLAGS_ERROR || ssl->flags & SSL_FLAGS_CLOSED) {
		return SSL_ERROR;
	}
	return writeAlert(ssl, SSL_ALERT_LEVEL_WARNING, SSL_ALERT_CLOSE_NOTIFY, out);
}

#ifdef USE_SERVER_SIDE_SSL
/******************************************************************************/
/*
	Write out the ServerHello message
*/
static int writeServerHello(ssl_t *ssl, sslBuf_t *out)
{
	unsigned char	*c, *end, *dataStart;
	int				messageSize;
	time_t			t;

	c = out->end;
	end = out->buf + out->size;
/*
	Calculate the size of the message up front, and verify we have room
	We assume there will be a sessionId in the message, and make adjustments
	below if there is no sessionId.
*/
	messageSize =
		SSL3_HEADER_LEN +
		SSL3_HANDSHAKE_HEADER_LEN +
		38 + SSL_MAX_SESSION_ID_SIZE;
	if (end - c < messageSize) {
		return SSL_FULL;
	}
/*
	First 4 bytes of the serverRandom are the unix time to prevent replay
	attacks, the rest are random
*/
	ssl->sec.serverRandom = sslMalloc(SSL_HS_RANDOM_SIZE);
	t = time(0);
	ssl->sec.serverRandom[0] = (unsigned char)((t & 0xFF000000) >> 24);
	ssl->sec.serverRandom[1] = (unsigned char)((t & 0xFF0000) >> 16);
	ssl->sec.serverRandom[2] = (unsigned char)((t & 0xFF00) >> 8);
	ssl->sec.serverRandom[3] = (unsigned char)(t & 0xFF);
	if (sslGetEntropy(ssl->sec.serverRandom + 4, SSL_HS_RANDOM_SIZE - 4) < 0) {
		matrixStrDebugMsg("Error gathering serverRandom entropy\n", NULL);
		return SSL_ERROR;
	}
/*
	We register session here because at this point the serverRandom value is
	populated.  If we are able to register the session, the sessionID and
	sessionIdLen fields will be non-NULL, otherwise the session couldn't
	be registered.
*/
	if (!(ssl->flags & SSL_FLAGS_RESUMED)) {
		registerSession(ssl);
	}
	messageSize -= (SSL_MAX_SESSION_ID_SIZE - ssl->sessionIdLen);
/*
	Write the SSL record header and the handshake message header
	Track the start of the handshake header in dataStart so we can 
	accumulate the hash of the handshake messages for later verification
*/
	c += writeRecordHeader(ssl, SSL_RECORD_TYPE_HANDSHAKE, 
		SSL3_HANDSHAKE_HEADER_LEN + 38 + ssl->sessionIdLen, c);
	dataStart = c;
	c += writeHandshakeHeader(ssl, SSL_HS_SERVER_HELLO,
		38 + ssl->sessionIdLen, c);
/*
	First two fields in the ServerHello message are the major and minor
	SSL protocol versions we agree to talk with
*/
	*c = ssl->majVer; c++;
	*c = ssl->minVer; c++;
/*
	The next 32 bytes are the server's random value, to be combined with
	the client random and premaster for key generation later
*/
	memcpy(c, ssl->sec.serverRandom, SSL_HS_RANDOM_SIZE);
	c += SSL_HS_RANDOM_SIZE;
/*
	The next data is a single byte containing the session ID length,
	and up to 32 bytes containing the session id.
	First register the session, which will give us a session id and length
	if not all session slots in the table are used
*/
	*c = ssl->sessionIdLen; c++;
	if (ssl->sessionIdLen > 0) {
        memcpy(c, ssl->sessionId, ssl->sessionIdLen);
		c += ssl->sessionIdLen;
	}
/*
	Two byte cipher suite we've chosen based on the list sent by the client
	and what we support.
	One byte compression method (always zero)
*/
	*c = (ssl->cipher->id & 0xFF00) >> 8; c++;
	*c = ssl->cipher->id & 0xFF; c++;
	*c = 0; c++;
/*
	Update the handshake message hash, which includes the entire handshake
	message, but not the 5 byte SSL record header
*/
	sslUpdateHSHash(ssl, dataStart, (int)(c - dataStart));
/*
	If we're resuming a session, we now have the clientRandom, master and 
	serverRandom, so we can derive keys which we'll be using shortly.
*/
	if (ssl->flags & SSL_FLAGS_RESUMED) {
		sslDeriveKeys(ssl);
	}
/*
	Verify that we've calculated the messageSize correctly, really this
	should never fail; it's more of an implementation verification
*/
	if (c - out->end != messageSize) {
		return SSL_ERROR;
	}
	out->end = c;
	return SSL_SUCCESS;
}

/******************************************************************************/
/*
	ServerHelloDone message is a blank handshake message
*/
static int writeServerHelloDone(ssl_t *ssl, sslBuf_t *out)
{
	unsigned char	*c, *end, *dataStart;
	int				messageSize;

	c = out->end;
	end = out->buf + out->size;
	messageSize =
		SSL3_HEADER_LEN +
		SSL3_HANDSHAKE_HEADER_LEN;
	if (end - c < messageSize) {
		return SSL_FULL;
	}

	c += writeRecordHeader(ssl, SSL_RECORD_TYPE_HANDSHAKE, 
		SSL3_HANDSHAKE_HEADER_LEN, c);
	dataStart = c;
	c += writeHandshakeHeader(ssl, SSL_HS_SERVER_HELLO_DONE, 0, c);

	sslUpdateHSHash(ssl, dataStart, (int)(c - dataStart));

	if (c - out->end != messageSize) {
		matrixStrDebugMsg("Error generating hello done for write\n", NULL);
		return SSL_ERROR;
	}
	out->end = c;
	return SSL_SUCCESS;
}

#endif /* USE_SERVER_SIDE_SSL */

/******************************************************************************/
/*
	Write a Certificate message.
	The encoding of the message is as follows:
		3 byte length of certificate data (network byte order)
		If there is one certificate,
			3 byte length of certificate
			certificate data
		For more than one certificate:
			3 byte length of all certificate data
			3 byte length of first certificate
			first certificate data
			3 byte length of second certificate
			second certificate data
	Certificate data is the base64 section of an X.509 certificate file
	in PEM format decoded to binary.  No additional interpretation is required.
*/
static int writeCertificate(ssl_t *ssl, sslBuf_t *out)
{
	unsigned char	*c, *end, *dataStart;
	int				messageSize, certLen;

	c = out->end;
	end = out->buf + out->size;
	certLen = ssl->keys->certLen;
	messageSize =
		SSL3_HEADER_LEN +
		SSL3_HANDSHAKE_HEADER_LEN +
		6 + certLen;
	if (end - c < messageSize) {
		return SSL_FULL;
	}
	c += writeRecordHeader(ssl, SSL_RECORD_TYPE_HANDSHAKE, 
		SSL3_HANDSHAKE_HEADER_LEN + 6 + certLen, c);
	dataStart = c;
	c += writeHandshakeHeader(ssl, SSL_HS_CERTIFICATE,
		6 + certLen, c);
/*
	This code supports sending a single certificate in binary DER format
	FUTURE - Add support multiple certificates
*/
	*c = (certLen + 3 & 0xFF0000) >> 16; c++;
	*c = (certLen + 3 & 0xFF00) >> 8; c++;
	*c = (certLen + 3 & 0xFF); c++;

	*c = (certLen & 0xFF0000) >> 16; c++;
	*c = (certLen & 0xFF00) >> 8; c++;
	*c = (certLen & 0xFF); c++;

	memcpy(c, ssl->keys->cert, certLen);
	c += certLen;

	sslUpdateHSHash(ssl, dataStart, (int)(c - dataStart));

	if (c - out->end != messageSize) {
		matrixStrDebugMsg("Error parsing certificate for write\n", NULL);
		return SSL_ERROR;
	}
	out->end = c;
	return SSL_SUCCESS;
}

/******************************************************************************/
/*
	Write the ChangeCipherSpec message.  It has its own message type
	and contains just one byte of value one.  It is not a handshake 
	message, so it isn't included in the handshake hash.
*/
static int writeChangeCipherSpec(ssl_t *ssl, sslBuf_t *out)
{
	unsigned char	*c, *end;
	int				messageSize;

	c = out->end;
	end = out->buf + out->size;
	messageSize =
		SSL3_HEADER_LEN +
		1;
	if (end - c < messageSize) {
		return SSL_FULL;
	}
	c += writeRecordHeader(ssl, SSL_RECORD_TYPE_CHANGE_CIPHER_SPEC, 1, c);
	*c = 1; c++;
/*
	FUTURE - need to encode the message if renegotiating
*/
/*
	After the client parses the ChangeCipherSpec message, it will expect
	the next message to be encrypted, so activate encryption on outgoing
	data now
*/
	sslActivateWriteCipher(ssl);

	if (c - out->end != messageSize) {
		matrixStrDebugMsg("Error generating change cipher for write\n", NULL);
		return SSL_ERROR;
	}
	out->end = c;
	return SSL_SUCCESS;
}

/******************************************************************************/
/*
	Write the Finished message
	The message contains the 36 bytes, the 16 byte MD5 and 20 byte SHA1 hash
	of all the handshake messages so far (excluding this one!)
*/
static int writeFinished(ssl_t *ssl, sslBuf_t *out)
{
	unsigned char	*c, *end, *encryptStart, padLen;
	int				messageSize, verifyLen;

	c = out->end;
	end = out->buf + out->size;
	verifyLen = SSL_MD5_HASH_SIZE + SSL_SHA1_HASH_SIZE;

	messageSize =
		SSL3_HANDSHAKE_HEADER_LEN +
		verifyLen +
		ssl->cipher->macSize;
	padLen = sslPadLenPwr2(messageSize, ssl->cipher->blockSize);
	messageSize += padLen + SSL3_HEADER_LEN;
	if (end - c < messageSize) {
		return SSL_FULL;
	}
	c += writeRecordHeader(ssl, SSL_RECORD_TYPE_HANDSHAKE, 
		messageSize - SSL3_HEADER_LEN, c);
	encryptStart = c;
	c += writeHandshakeHeader(ssl, SSL_HS_FINISHED,
		verifyLen, c);
/*
	Output the hash of messages we've been collecting so far into the buffer
*/
	c += sslSnapshotHSHash(ssl, c, ssl->flags & SSL_FLAGS_SERVER);
/*
	Generate the MAC of the message we just wrote.  The mac is at record level
	so it includes the 4 byte handshake header and the 36 byte message
	Add padding to the message if using a block cipher
	Encrypt the message using the current cipher
*/
	sslUpdateHSHash(ssl, encryptStart, (int)(c - encryptStart));
	c += ssl->generateMac(ssl, SSL_RECORD_TYPE_HANDSHAKE, encryptStart, 
		(int)(c - encryptStart), c);
	c += sslWritePad(c, padLen);
	if (ssl->encrypt(&ssl->sec.encryptCtx, encryptStart, encryptStart, 
			(int)(c - encryptStart)) < 0 ||	c - out->end != messageSize) {
		matrixStrDebugMsg("Error encrypting final message for write\n", NULL);
		return SSL_ERROR;
	}
	out->end = c;
	return SSL_SUCCESS;
}

/******************************************************************************/
/*
	Write an Alert message
	The message contains two bytes: AlertLevel and AlertDescription
*/
static int writeAlert(ssl_t *ssl, unsigned char level, 
						unsigned char description, sslBuf_t *out)
{
	unsigned char	*c, *end, *encryptStart, padLen;
	int				messageSize;

	c = out->end;
	end = out->buf + out->size;
	/* FUTURE - need a better method of adding in macSize */
	messageSize = 2 +
		((ssl->flags & SSL_FLAGS_WRITE_SECURE) ? ssl->cipher->macSize : 0);
	padLen = sslPadLenPwr2(messageSize, ssl->cipher->blockSize);
	messageSize += padLen + SSL3_HEADER_LEN;
	if (end - c < messageSize) {
		return SSL_FULL;
	}
	c += writeRecordHeader(ssl, SSL_RECORD_TYPE_ALERT, 
		messageSize - SSL3_HEADER_LEN, c);
	encryptStart = c;
	*c = level; c++;
	*c = description; c++;
	c += ssl->generateMac(ssl, SSL_RECORD_TYPE_ALERT, encryptStart, 
		(int)(c - encryptStart), c);
	c += sslWritePad(c, padLen);
	if (ssl->encrypt(&ssl->sec.encryptCtx, encryptStart, encryptStart, 
			(int)(c - encryptStart)) < 0 ||	(c - out->end) != messageSize) {
		matrixStrDebugMsg("Error encrypting alert\n", NULL);
		ssl->flags |= SSL_FLAGS_ERROR;
		return SSL_ERROR;
	}
	out->end = c;
	return SSL_SUCCESS;
}

#ifdef USE_CLIENT_SIDE_SSL
/******************************************************************************/
/*
	Write out the ClientHello message
*/
int matrixSslEncodeClientHello(ssl_t *ssl, sslBuf_t *out,
							   unsigned short cipherSpec)
{
	unsigned char	*c, *end, *dataStart;
	int				messageSize, rc, cipherLen;
	time_t			t;

	if (ssl->flags & SSL_FLAGS_ERROR || ssl->flags & SSL_FLAGS_CLOSED) {
		return SSL_ERROR;
	}
	if (ssl->flags & SSL_FLAGS_SERVER || ssl->hsState != SSL_HS_SERVER_HELLO) {
		return SSL_ERROR;
	}
	
/*
	If a cipher is specified it is two bytes length and two bytes data
*/
	if (cipherSpec == 0) {
		cipherLen = sslGetCipherSpecListLen();
	} else {
		if (sslGetCipherSpec(cipherSpec) == NULL) {
			matrixIntDebugMsg("Cipher suite not supported: %d\n", cipherSpec);
			return SSL_ERROR;
		}
		cipherLen = 4;
	}

	c = out->end;
	end = out->buf + out->size;
/*
	Calculate the size of the message up front, and verify we have room
*/
	messageSize =
		SSL3_HEADER_LEN +
		SSL3_HANDSHAKE_HEADER_LEN +
		5 + SSL_HS_RANDOM_SIZE + ssl->sessionIdLen + cipherLen;
	if (end - c < messageSize) {
		return SSL_FULL;
	}
/*
	First 4 bytes of the serverRandom are the unix time to prevent replay
	attacks, the rest are random
*/
	ssl->sec.clientRandom = sslMalloc(SSL_HS_RANDOM_SIZE);
	t = time(0);
	ssl->sec.clientRandom[0] = (unsigned char)((t & 0xFF000000) >> 24);
	ssl->sec.clientRandom[1] = (unsigned char)((t & 0xFF0000) >> 16);
	ssl->sec.clientRandom[2] = (unsigned char)((t & 0xFF00) >> 8);
	ssl->sec.clientRandom[3] = (unsigned char)(t & 0xFF);
	if (sslGetEntropy(ssl->sec.clientRandom + 4, SSL_HS_RANDOM_SIZE - 4) < 0) {
		matrixStrDebugMsg("Error gathering clientRandom entropy\n", NULL);
		return SSL_ERROR;
	}
/*
	Write the SSL record header and the handshake message header
	Track the start of the handshake header in dataStart so we can 
	accumulate the hash of the handshake messages for later verification
*/
	c += writeRecordHeader(ssl, SSL_RECORD_TYPE_HANDSHAKE, 
		messageSize - SSL3_HEADER_LEN, c);
	dataStart = c;
	c += writeHandshakeHeader(ssl, SSL_HS_CLIENT_HELLO,
		messageSize - SSL3_HEADER_LEN - SSL3_HANDSHAKE_HEADER_LEN, c);
/*
	First two fields in the ClientHello message are the maximum major 
	and minor SSL protocol versions we support
*/
	*c = ssl->majVer; c++;
	*c = ssl->minVer; c++;
/*
	The next 32 bytes are the server's random value, to be combined with
	the client random and premaster for key generation later
*/
	memcpy(c, ssl->sec.clientRandom, SSL_HS_RANDOM_SIZE);
	c += SSL_HS_RANDOM_SIZE;
/*
	The next data is a single byte containing the session ID length,
	and up to 32 bytes containing the session id.
	If we are asking to resume a session, then the sessionId would have
	been set at session creation time.
*/
	*c = ssl->sessionIdLen; c++;
	if (ssl->sessionIdLen > 0) {
        memcpy(c, ssl->sessionId, ssl->sessionIdLen);
		c += ssl->sessionIdLen;
	}
/*
	Write out the length and ciphers we support
	Client can request a single specific cipher in the cipherSpec param
*/
	if (cipherSpec == 0) {
		if ((rc = sslGetCipherSpecList(c, (int)(end - c))) < 0) {
			return SSL_FULL;
		}
		c += rc;
	} else {
		if ((int)(end - c) < 4) {
			return SSL_FULL;
		}
		*c = 0; c++;
		*c = 2; c++;
		*c = (cipherSpec & 0xFF00) >> 8; c++;
		*c = cipherSpec & 0xFF; c++;
	}	
/*
	Followed by two bytes (len and compression method (always zero))
*/
	*c = 1; c++;
	*c = 0; c++;
/*
	Update the handshake message hash, which includes the entire handshake
	message, but not the 5 byte SSL record header
*/
	sslUpdateHSHash(ssl, dataStart, (int)(c - dataStart));
/*
	Verify that we've calculated the messageSize correctly, really this
	should never fail; it's more of an implementation verification
*/
	if (c - out->end != messageSize) {
		return SSL_ERROR;
	}
	out->end = c;
	return SSL_SUCCESS;
}

/******************************************************************************/
/*
	Write a ClientKeyExchange message.

*/
static int writeClientKeyExchange(ssl_t *ssl, sslBuf_t *out)
{
	unsigned char	*c, *end, *dataStart;
	int				messageSize, keyLen;

	c = out->end;
	end = out->buf + out->size;
	keyLen = ssl->sec.cert->publicKey.size;
	messageSize = 
		SSL3_HEADER_LEN +
		SSL3_HANDSHAKE_HEADER_LEN +
		keyLen;

	if (end - c < messageSize) {
		return SSL_FULL;
	}
	c += writeRecordHeader(ssl, SSL_RECORD_TYPE_HANDSHAKE, 
		messageSize - SSL3_HEADER_LEN, c);
	dataStart = c;
	c += writeHandshakeHeader(ssl, SSL_HS_CLIENT_KEY_EXCHANGE,
		messageSize - SSL3_HEADER_LEN - SSL3_HANDSHAKE_HEADER_LEN, c);
/*
	ClientKeyExchange message contains the encrypted premaster secret.
	The base premaster is the original SSL protocol version we asked for
	followed by 46 bytes of random data.
	These 48 bytes are padded to the current RSA key length and encrypted
	with the RSA key.
*/
	ssl->sec.premaster = sslMalloc(SSL_HS_PREMASTER_SIZE);
	ssl->sec.premaster[0] = ssl->majVer;
	ssl->sec.premaster[1] = ssl->minVer;
	if (sslGetEntropy(ssl->sec.premaster + 2, SSL_HS_PREMASTER_SIZE - 2) < 0) {
		matrixStrDebugMsg("Error gathering premaster entropy\n", NULL);
		return SSL_ERROR;
	}
	sslActivatePublicCipher(ssl);
	if (ssl->encryptPub(&(ssl->sec.cert->publicKey), ssl->sec.premaster, 
			SSL_HS_PREMASTER_SIZE, c, (int)(end - c)) != keyLen) {
		matrixStrDebugMsg("Error encrypting premaster\n", NULL);
		return SSL_FULL;
	}
	c += keyLen;

	sslUpdateHSHash(ssl, dataStart, (int)(c - dataStart));

	if (c - out->end != messageSize) {
		matrixStrDebugMsg("Invalid ClientKeyExchange length\n", NULL);
		return SSL_ERROR;
	}
/*
	Now that we've got the premaster secret, derive the various symmetric
	keys using it and the client and server random values
*/
	sslDeriveKeys(ssl);

	out->end = c;
	return SSL_SUCCESS;
}

void matrixSslSetCertValidator(ssl_t *ssl, int (*certValidator)(sslCertInfo_t *t))
{
	if (certValidator) {
		ssl->sec.validateCert = certValidator;
	}
	
}
#else /* USE_CLIENT_SIDE_SSL */
/*
	Stub out this function rather than ifdef it out in the public header
*/
int matrixSslEncodeClientHello(ssl_t *ssl, sslBuf_t *out,
							   unsigned short cipherSpec)
{
	matrixStrDebugMsg("Library not built with USE_CLIENT_SIDE_SSL\n", NULL);
	return -1;
}
void matrixSslSetCertValidator(ssl_t *ssl, int (*certValidator)(sslCertInfo_t *t))
{
	matrixStrDebugMsg("Library not built with USE_CLIENT_SIDE_SSL\n", NULL);
}
#endif /* USE_CLIENT_SIDE_SSL */

/******************************************************************************/
/*
	Write out a SSLv3 record header.
	Assumes 'c' points to a buffer of at least SSL3_HEADER_LEN bytes
		1 byte type (SSL_RECORD_TYPE_*)
		1 byte major version
		1 byte minor version
		2 bytes length (network byte order)
	Returns the number of bytes written
*/
static int writeRecordHeader(ssl_t *ssl, unsigned char type, int len, char *c)
{
	*c = type; c++;
	*c = ssl->majVer; c++;
	*c = ssl->minVer; c++;
	*c = (len & 0xFF00) >> 8; c++;
	*c = (len & 0xFF);

	return SSL3_HEADER_LEN;
}

/******************************************************************************/
/*
	Write out an ssl handshake message header.
	Assumes 'c' points to a buffer of at least SSL3_HANDSHAKE_HEADER_LEN bytes
		1 byte type (SSL_HS_*)
		3 bytes length (network byte order)
	Returns the number of bytes written
*/
static int writeHandshakeHeader(ssl_t *ssl, unsigned char type, int len, 
								char *c)
{
	*c = type; c++;
	*c = (len & 0xFF0000) >> 16; c++;
	*c = (len & 0xFF00) >> 8; c++;
	*c = (len & 0xFF);

	return SSL3_HANDSHAKE_HEADER_LEN;
}

/***************************************************************************************/
/*
	Write pad bytes and pad length per the spec.  Most block cipher padding fills 
	each byte with the number of padding bytes, but SSL/TLS pretends one of these bytes 
	is a pad length, and the remaining bytes are filled with that length.  The end result 
	is that the padding is identical to standard padding except the values are one less.  
	For SSLv3 we are not required to have any specific pad values, but they don't hurt.

	PadLen	Result
	0
	1		00
	2		01 01
	3		02 02 02
	4		03 03 03 03
	5		04 04 04 04 04
	6		05 05 05 05 05 05
	7		06 06 06 06 06 06 06
	8		07 07 07 07 07 07 07 07

	We calculate the length of padding required for a record using sslPadLenPwr2()

*/
static int sslWritePad(unsigned char *p, unsigned char padLen)
{
	unsigned char c = padLen;

	while (c-- > 0) {
		*p++ = padLen - 1;
	}
	return padLen;
}

/***************************************************************************************/
/*
	Initialize the SHA1 and MD5 hash contexts for the handshake messages
*/
static int sslInitHSHash(ssl_t *ssl)
{
	matrixSha1Init(&ssl->sec.msgHashSha1);
	matrixMd5Init(&ssl->sec.msgHashMd5);
	return 0;
}

/***************************************************************************************/
/*
	Add the given data to the running hash of the handshake messages
*/
static int sslUpdateHSHash(ssl_t *ssl, unsigned char *in, int len)
{
	matrixMd5Update(&ssl->sec.msgHashMd5, in, len);
	matrixSha1Update(&ssl->sec.msgHashSha1, in, len);
	return 0;
}

/***************************************************************************************/
/*
	Snapshot is called by the receiver of the finished message to produce
	a hash of the preceeding handshake messages for comparison to incoming
	message.
*/
static int sslSnapshotHSHash(ssl_t *ssl, char *out, int senderFlag)
{
	sslMd5Context_t		md5;
	sslSha1Context_t	sha1;

/*
	Use a backup of the message hash-to-date because we don't want
	to destroy the state of the handshaking until truly complete
*/
	md5 = ssl->sec.msgHashMd5;
	sha1 = ssl->sec.msgHashSha1;

	return sslGenerateFinishedHash(&md5, &sha1, ssl->sec.masterSecret, out, 
		senderFlag);
}

/******************************************************************************/
/*
	Cipher suites are chosen before they are activated with the 
	ChangeCipherSuite message.  Additionally, the read and write cipher suites
	are activated at different times in the handshake process.  The following
	APIs activate the selected cipher suite callback functions.
*/
static int sslActivateReadCipher(ssl_t *ssl)
{
	ssl->decrypt = ssl->cipher->decrypt;
	ssl->verifyMac = ssl->cipher->verifyMac;
	if (ssl->cipher->id != SSL_NULL_WITH_NULL_NULL) {
		ssl->flags |= SSL_FLAGS_READ_SECURE;
	}
	return 0;
}

static int sslActivateWriteCipher(ssl_t *ssl)
{
	ssl->encrypt = ssl->cipher->encrypt;
	ssl->generateMac = ssl->cipher->generateMac;
	if (ssl->cipher->id != SSL_NULL_WITH_NULL_NULL) {
		ssl->flags |= SSL_FLAGS_WRITE_SECURE;
	}
	return 0;
}

static int sslActivatePublicCipher(ssl_t *ssl)
{
	ssl->decryptPriv = ssl->cipher->decryptPriv;
	ssl->encryptPub = ssl->cipher->encryptPub;
	if (ssl->cipher->id != SSL_NULL_WITH_NULL_NULL) {
		ssl->flags |= SSL_FLAGS_PUBLIC_SECURE;
	}
	return 0;
}

#ifdef USE_SERVER_SIDE_SSL
/******************************************************************************/
/*
	Register a session in the session resumption cache.  If successful (rc >=0),
	the ssl sessionId and sessionIdLength fields will be non-NULL upon
	return.
*/
static int registerSession(ssl_t *ssl)
{
	unsigned int			i, j;
	sslTime_t	t;

	if (!(ssl->flags & SSL_FLAGS_SERVER)) {
		return -1;
	}
/*
	Iterate the session table, looking for an empty entry (cipher null), and the
	oldest entry that is not in use
*/
	sslLockMutex(&sessionTableLock);
	j = SSL_SESSION_TABLE_SIZE;
	t = sessionTable[0].accessTime;
	for (i = 0; i < SSL_SESSION_TABLE_SIZE; i++) {
		if (sessionTable[i].cipher == NULL) {
			break;
		}
		if (sslCompareTime(sessionTable[i].accessTime, t) &&
				sessionTable[i].inUse == 0) {
			t = sessionTable[i].accessTime;
			j = i;
		}
	}
/*
	If there were no empty entries, get the oldest unused entry.
	If all entries are in use, return -1, meaning we can't cache the
	session at this time
*/
	if (i >= SSL_SESSION_TABLE_SIZE) {
		if (j < SSL_SESSION_TABLE_SIZE) {
			i = j;
		} else {
			sslUnlockMutex(&sessionTableLock);
			return -1;
		}
	}
/*
	Register the incoming masterSecret and cipher, which could still be null, 
	depending on when we're called.
*/
	memcpy(sessionTable[i].masterSecret, ssl->sec.masterSecret,
		SSL_HS_MASTER_SIZE);
	sessionTable[i].cipher = ssl->cipher;
	sessionTable[i].inUse = 1;
	sslUnlockMutex(&sessionTableLock);
/*
	The sessionId is the current serverRandom value, with the first 4 bytes
	replaced with the current cache index value for quick lookup later.
	FUTURE SECURITY - Should generate more random bytes here for the session
	id.  We re-use the server random as the ID, which is OK, since it is
	sent plaintext on the network, but an attacker listening to a resumed
	connection will also be able to determine part of the original server
	random used to generate the master key, even if he had not seen it
	initially.
*/
	memcpy(sessionTable[i].id, ssl->sec.serverRandom, 
		min(SSL_HS_RANDOM_SIZE, SSL_MAX_SESSION_ID_SIZE));
	ssl->sessionIdLen = SSL_MAX_SESSION_ID_SIZE;
	ssl->sessionId = sslMalloc(SSL_MAX_SESSION_ID_SIZE);
	sessionTable[i].id[0] = (unsigned char)(i & 0xFF);
	sessionTable[i].id[1] = (unsigned char)((i & 0xFF00) >> 8);
	sessionTable[i].id[2] = (unsigned char)((i & 0xFF0000) >> 16);
	sessionTable[i].id[3] = (unsigned char)((i & 0xFF000000) >> 24);
	memcpy(ssl->sessionId, sessionTable[i].id, SSL_MAX_SESSION_ID_SIZE);
/*
	startTime is used to check expiry of the entry
	accessTime is used to for cache replacement logic
	The versions are stored, because a cached session must be reused 
	with same SSL version.
*/
	sslInitMsecs(&sessionTable[i].startTime);
	sessionTable[i].accessTime = sessionTable[i].startTime;
	sessionTable[i].majVer = ssl->majVer;
	sessionTable[i].minVer = ssl->minVer;

	return i;
}

/******************************************************************************/
/*
	Look up a session ID in the cache.  If found, set the ssl masterSecret
	and cipher to the pre-negotiated values
*/
static int resumeSession(ssl_t *ssl)
{
	unsigned char	*id;
	unsigned int	i;

	if (!(ssl->flags & SSL_FLAGS_SERVER)) {
		return -1;
	}
	if ((id = ssl->sessionId) == NULL) {
		return -1;
	}
	i = (id[3] << 24) + (id[2] << 16) + (id[1] << 8) + id[0];
	sslLockMutex(&sessionTableLock);
	if (i >= SSL_SESSION_TABLE_SIZE || i < 0 ||
			sessionTable[i].cipher == NULL ||
			memcmp(sessionTable[i].id, id, 
				min(ssl->sessionIdLen, SSL_MAX_SESSION_ID_SIZE)) != 0 ||
			/* FUTURE - verify startTime is not too old (session expired) */
			sessionTable[i].inUse ||
			sessionTable[i].majVer != ssl->majVer ||
			sessionTable[i].minVer != ssl->minVer) {
		sslUnlockMutex(&sessionTableLock);
		return -1;
	}
	memcpy(ssl->sec.masterSecret, sessionTable[i].masterSecret,
		SSL_HS_MASTER_SIZE);
	ssl->cipher = sessionTable[i].cipher;
	sslInitMsecs(&sessionTable[i].accessTime);
	sessionTable[i].inUse = 1;
	sslUnlockMutex(&sessionTableLock);
	return 0;
}

/******************************************************************************/
/*
	Update session information in the cache.
	This is called when we've determined the master secret and when we're
	closing the connection to update various values in the cache.
*/
static int updateSession(ssl_t *ssl)
{
	unsigned char	*id;
	unsigned int	i;

	if (!(ssl->flags & SSL_FLAGS_SERVER)) {
		return -1;
	}
	if ((id = ssl->sessionId) == NULL) {
		return -1;
	}
	i = (id[3] << 24) + (id[2] << 16) + (id[1] << 8) + id[0];
	if (i < 0 || i >= SSL_SESSION_TABLE_SIZE) {
		return -1;
	}
/*
	If there is an error on the session, invalidate for any future use
*/
	sslLockMutex(&sessionTableLock);
	sessionTable[i].inUse = ssl->flags & SSL_FLAGS_CLOSED ? 0 : 1;
	if (ssl->flags & SSL_FLAGS_ERROR) {
		memset(sessionTable[i].masterSecret, 0x0, SSL_HS_MASTER_SIZE);
		sessionTable[i].cipher = NULL;
		sslUnlockMutex(&sessionTableLock);
		return -1;
	}
	memcpy(sessionTable[i].masterSecret, ssl->sec.masterSecret,
		SSL_HS_MASTER_SIZE);
	sessionTable[i].cipher = ssl->cipher;
	sslUnlockMutex(&sessionTableLock);
	return 0;
}
#endif /* USE_SERVER_SIDE_SSL */

/******************************************************************************/
/*
	Get session information from the ssl structure and populate the given
	session structure.  Session will contain a copy of the relevant session
	information, suitable for creating a new, resumed session.
*/
int matrixSslGetSessionId(ssl_t *ssl, sslSessionId_t **session)
{
	sslSessionId_t *lsession;

	if (ssl == NULL || ssl->flags & SSL_FLAGS_SERVER) {
		return -1;
	}

	if (ssl->cipher != NULL && ssl->cipher->id != SSL_NULL_WITH_NULL_NULL && 
			ssl->sessionIdLen == SSL_MAX_SESSION_ID_SIZE && 
			ssl->sec.masterSecret != NULL) {
		*session = lsession = sslMalloc(sizeof(sslSessionId_t));
		lsession->cipherId = ssl->cipher->id;
		memcpy(lsession->id, ssl->sessionId, ssl->sessionIdLen);
		memcpy(lsession->masterSecret, ssl->sec.masterSecret, 
			SSL_HS_MASTER_SIZE);
		return 0;
	}
	return -1;
}

void matrixSslFreeSessionId(sslSessionId_t *sessionId)
{
	if (sessionId != NULL) {
		sslFree(sessionId);
	}
}
/******************************************************************************/
/*
	Debugging APIs
*/
#if DEBUG
static int mallocCount;

void matrixStrDebugMsg(char *message, char *value)
{
	if (value) {
		printf(message, value);
	} else {
		printf(message);
	}
}
void matrixIntDebugMsg(char *message, int value)
{
	printf(message, value);
}

#endif /* DEBUG */

/******************************************************************************/


