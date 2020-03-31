/*
 *	matrixSsl.h
 *	Release $Name:  $
 *	
 *	Public header file for MatrixSSL
 *	Implementations interacting with the matrixssl library should
 *	only use the APIs and definitions used in this file.
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

#ifndef _h_MATRIXSSL
#define _h_MATRIXSSL

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/*
	Maximum SSL record size, per specification
	16K record plus 5 byte header
*/
#define		SSL_MAX_RECORD_SIZE		0x3FFF + 0x5

/*
	Return codes from public apis
	Not all apis return all codes.  See documentation for more details.
*/
#define		SSL_SUCCESS			0	/* Generic success */
#define		SSL_ERROR			-1	/* generic ssl error, see error code */
#define		SSL_FULL			-2	/* must call sslRead before decoding */
#define		SSL_PARTIAL			-3	/* more data reqired to parse full msg */
#define		SSL_SEND_RESPONSE	-4	/* decode produced output data */
#define		SSL_PROCESS_DATA	-5	/* succesfully decoded application data */
#define		SSL_ALERT			-6	/* we've decoded an alert */
#define		SSL_FILE_NOT_FOUND	-7	/* File not found */

/*
	SSL Alert levels and descriptions
	This implementation treats all alerts as fatal
*/
#define SSL_ALERT_LEVEL_WARNING				1
#define SSL_ALERT_LEVEL_FATAL				2

#define SSL_ALERT_CLOSE_NOTIFY				0
#define SSL_ALERT_UNEXPECTED_MESSAGE		10
#define SSL_ALERT_BAD_RECORD_MAC			20
#define SSL_ALERT_DECOMPRESSION_FAILURE		30
#define SSL_ALERT_HANDSHAKE_FAILURE			40
#define SSL_ALERT_NO_CERTIFICATE			41
#define SSL_ALERT_BAD_CERTIFICATE			42
#define SSL_ALERT_UNSUPPORTED_CERTIFICATE	43
#define SSL_ALERT_CERTIFICATE_REVOKED		44
#define SSL_ALERT_CERTIFICATE_EXPIRED		45
#define SSL_ALERT_CERTIFICATE_UNKNOWN		46
#define SSL_ALERT_ILLEGAL_PARAMETER			47

/******************************************************************************/
/*
	Typdefs required for public apis.  From an end user perspective, the 
	sslBuf_t and sslCertInfo_t types have internal fields that are public,
	but ssl_t, sslKeys_t, and sslSessionId_t do not.  Defining those as 'int'
	requires it to be treated as an opaque data type to be passed to public apis
*/
#ifndef _h_MATRIXINTERNAL
typedef struct {
	unsigned char	*buf;	/* Pointer to the start of the buffer */
	unsigned char	*start;	/* Pointer to start of valid data */
	unsigned char	*end;	/* Pointer to first byte of invalid data */
	int				size;	/* Size of buffer in bytes */
} sslBuf_t;

/*
	Information provided to user callback for validating certificates.
	Register callback with call to matrixSslSetCertValidator
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

typedef int		ssl_t;
typedef int		sslKeys_t;
typedef int		sslSessionId_t;

/*
	Flag indicating server session in matrixSslNewSession
*/
#define	SSL_FLAGS_SERVER		0x1

/******************************************************************************/
/*
	Explicitly import these apis on Windows.  If we're being included from the
	internal header, we export them instead!
*/
#ifdef WIN32
#define SSLPUBLIC extern __declspec(dllimport)
#endif /* WIN */
#else /* _h_MATRIXINTERNAL */
#ifdef WIN32
#define SSLPUBLIC extern __declspec(dllexport)
#endif /* WIN */
#endif /* _h_MATRIXINTERNAL */
#ifndef WIN32
#define SSLPUBLIC extern
#endif /* !WIN */

/******************************************************************************/
/*
 *	Public API set
 */
SSLPUBLIC int	matrixSslOpen();
SSLPUBLIC void	matrixSslClose();

SSLPUBLIC int	matrixSslReadKeys(sslKeys_t **keys, char *certFile,
						char *privFile, char *privPass, char *trustedCAFile);
SSLPUBLIC void	matrixSslFreeKeys(sslKeys_t *keys);

SSLPUBLIC int	matrixSslNewSession(ssl_t **ssl, sslKeys_t *keys,
						sslSessionId_t *session, int flags);
SSLPUBLIC void	matrixSslDeleteSession(ssl_t *ssl);

SSLPUBLIC int	matrixSslDecode(ssl_t *ssl, sslBuf_t *in, sslBuf_t *out, 
						unsigned char *error, unsigned char *alertLevel,
						unsigned char *alertDescription);
SSLPUBLIC int	matrixSslEncode(ssl_t *ssl, unsigned char *in, int inlen,
						sslBuf_t *out);
SSLPUBLIC int	matrixSslEncodeClosureAlert(ssl_t *ssl, sslBuf_t *out);

SSLPUBLIC int	matrixSslHandshakeIsComplete(ssl_t *ssl);

/*
	Client side APIs
*/
SSLPUBLIC int	matrixSslEncodeClientHello(ssl_t *ssl, sslBuf_t *out,
						unsigned short cipherSpec);

SSLPUBLIC void	matrixSslSetCertValidator(ssl_t *ssl,
						int (*certValidator)(sslCertInfo_t *));

SSLPUBLIC int	matrixSslGetSessionId(ssl_t *ssl, sslSessionId_t **sessionId);
SSLPUBLIC void	matrixSslFreeSessionId(sslSessionId_t *sessionId);

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _h_MATRIXSSL */

/******************************************************************************/
