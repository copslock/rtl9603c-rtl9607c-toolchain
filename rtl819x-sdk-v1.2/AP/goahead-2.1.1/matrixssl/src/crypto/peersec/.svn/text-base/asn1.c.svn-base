/*
 *	asn1.c
 *	Release $Name:  $
 *
 *	DER/BER coding
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

#include "../cryptoLayer.h"

static int asnParseLength(unsigned char **p, int size, int *valLen);
static int getBig(unsigned char **pp, int len, mp_int *big);
static int getSerialNum(unsigned char **pp, int len, unsigned char **sn,
						int *snLen);
static int getInteger(unsigned char **pp, int len, int *val);
static int getSequence(unsigned char **pp, int len, int *outLen);
static int getSet(unsigned char **pp, int len, int *outLen);
static int getExplicitVersion(unsigned char **pp, int len, int expVal,
							  int *outLen);
static int getAlgorithmIdentifier(unsigned char **pp, int len, int *oi,
								  int isPubKey);
static int getValidity(unsigned char **pp, int len, char **notBefore,
					   char **notAfter);
static int getSignature(unsigned char **pp, int len,
						unsigned char **sig, int *sigLen);
static int getImplicitBitString(unsigned char **pp, int len, int impVal,
						unsigned char **bitString, int *bitLen);
static int getPubKey(unsigned char **pp, int len, rsa_key *pubKey);

#ifdef USE_X509
static int getExplicitExtensions(unsigned char **pp, int len, int expVal,
								 v3extensions_t *extensions, int *outLen);
static int getDNAttributes(unsigned char **pp, int len,
						   DNattributes_t *attribs);
#endif /* USE_X509 */

#define ATTRIB_COUNTRY_NAME		6
#define ATTRIB_LOCALITY			7
#define ATTRIB_ORGANIZATION		10
#define ATTRIB_ORG_UNIT			11
#define ATTRIB_DN_QUALIFIER		46
#define ATTRIB_STATE_PROVINCE	8
#define ATTRIB_COMMON_NAME		3

#define IMPLICIT_ISSUER_ID	1
#define IMPLICIT_SUBJECT_ID	2
#define EXPLICIT_EXTENSION	3

#define EXT_CERT_POLICIES		146
#define EXT_BASIC_CONSTRAINTS	133
#define EXT_AUTH_KEY_ID			149
#define	EXT_KEY_USAGE			129
#define EXT_ALT_SUBJECT_NAME	131
#define EXT_ALT_ISSUER_NAME		132
#define EXT_SUBJ_KEY_ID			128

#define OID_SHA1		88
#define OID_MD5			649

/******************************************************************************/
/*
	Parse a a private key structure in DER formatted ASN.1
	Per ftp://ftp.rsasecurity.com/pub/pkcs/pkcs-1/pkcs-1v2-1.pdf
	RSAPrivateKey ::= SEQUENCE {
		version Version,
		modulus INTEGER, -- n
		publicExponent INTEGER, -- e
		privateExponent INTEGER, -- d
		prime1 INTEGER, -- p
		prime2 INTEGER, -- q
		exponent1 INTEGER, -- d mod (p-1)
		exponent2 INTEGER, -- d mod (q-1)
		coefficient INTEGER, -- (inverse of q) mod p
		otherPrimeInfos OtherPrimeInfos OPTIONAL
	}
	Version ::= INTEGER { two-prime(0), multi(1) }
	  (CONSTRAINED BY {-- version must be multi if otherPrimeInfos present --})

	Which should look something like this in hex (pipe character 
	is used as a delimiter):
	ftp://ftp.rsa.com/pub/pkcs/ascii/layman.asc
	30	Tag in binary: 00|1|10000 -> UNIVERSAL | CONSTRUCTED | SEQUENCE (16)
	82	Length in binary: 1 | 0000010 -> LONG LENGTH | LENGTH BYTES (2)
	04 A4	Length Bytes (1188)
	02	Tag in binary: 00|0|00010 -> UNIVERSAL | PRIMITIVE | INTEGER (2)
	01	Length in binary: 0|0000001 -> SHORT LENGTH | LENGTH (1)
	00	INTEGER value (0) - RSAPrivateKey.version
	02	Tag in binary: 00|0|00010 -> UNIVERSAL | PRIMITIVE | INTEGER (2)
	82	Length in binary: 1 | 0000010 -> LONG LENGTH | LENGTH BYTES (2)
	01 01	Length Bytes (257)
	[]	257 Bytes of data - RSAPrivateKey.modulus (2048 bit key)
	02	Tag in binary: 00|0|00010 -> UNIVERSAL | PRIMITIVE | INTEGER (2)
	03	Length in binary: 0|0000011 -> SHORT LENGTH | LENGTH (3)
	01 00 01	INTEGER value (65537) - RSAPrivateKey.publicExponent
	...

	OtherPrimeInfos is not supported in this routine, and an error will be
	returned if they are present
*/

int asnParsePrivateKey(unsigned char **pp, int size, sslRsaKey_t *key)
{
	unsigned char	*p, *end, *seq;
	int				version, seqlen;

	key->optimized = 0;
	p = *pp;
	end = p + size;
	if (getSequence(&p, size, &seqlen) < 0) {
		matrixStrDebugMsg("ASN sequence parse error\n", NULL);
		return -1;
	}
	seq = p;
	if (getInteger(&p, (int)(end - p), &version) < 0 || version != 0 ||
		getBig(&p, (int)(end - p), &(key->N)) < 0 ||
		getBig(&p, (int)(end - p), &(key->e)) < 0 ||
		getBig(&p, (int)(end - p), &(key->d)) < 0 ||
		getBig(&p, (int)(end - p), &(key->p)) < 0 ||
		getBig(&p, (int)(end - p), &(key->q)) < 0 ||
		getBig(&p, (int)(end - p), &(key->dP)) < 0 ||
		getBig(&p, (int)(end - p), &(key->dQ)) < 0 ||
		getBig(&p, (int)(end - p), &(key->qP)) < 0 ||
		(int)(p - seq) != seqlen) {
		matrixStrDebugMsg("ASN key extract parse error\n", NULL);
		return -1;
	}
	if (mp_shrink(&key->e) != MP_OKAY) { goto done; }
	if (mp_shrink(&key->d) != MP_OKAY) { goto done; }
	if (mp_shrink(&key->N) != MP_OKAY) { goto done; }
	if (mp_shrink(&key->p) != MP_OKAY) { goto done; }
	if (mp_shrink(&key->q) != MP_OKAY) { goto done; }

/*
	Compute the optimization members.  The qP is stored in the file but
	does not match the optimization algorithms used by libTomCrypt, so
	recompute it here along with adding pQ.
*/	
	if (mp_invmod(&key->q, &key->p, &key->qP) != MP_OKAY) {
		goto done;
	}
	if (mp_mulmod(&key->qP, &key->q, &key->N, &key->qP) != MP_OKAY) {
		goto done;
	}
	if (mp_invmod(&key->p, &key->q, &key->pQ) != MP_OKAY) { 
		goto done;
	}
	if (mp_mulmod(&key->pQ, &key->p, &key->N, &key->pQ)) { 
		goto done;
	}
	
	if (mp_shrink(&key->dQ) != MP_OKAY) { goto done; }
	if (mp_shrink(&key->dP) != MP_OKAY) { goto done; }
	if (mp_shrink(&key->qP) != MP_OKAY) { goto done; }
	if (mp_shrink(&key->pQ) != MP_OKAY) { goto done; }
/*
	If we made it here, the key is ready for optimized decryption
*/
	key->optimized = 1;

done:
	*pp = p;
/*
	Set the key length of the key
*/
	key->size = mp_unsigned_bin_size(&key->N);
	return 0;
}

#ifdef USE_X509
/******************************************************************************/
/*
	Parse an X509 ASN.1 certificate stream
	http://www.faqs.org/rfcs/rfc2459.html section 4.1
*/
int matrixX509ParseCert(unsigned char **pp, int size, sslRsaCert_t **outcert)
{
	sslRsaCert_t		*cert;
	sslMd5Context_t		md5Ctx;
	sslSha1Context_t	sha1Ctx;
	unsigned char		*p, *end, *certStart, *certEnd;
	int					certLen, len;
/*
	Allocate the cert structure right away.  User MUST always call
	matrixX509FreeCert regardless of whether this function succeeds.
	memset is important because the test for NULL is what is used
	to determine what to free
*/
	*outcert = cert = sslMalloc(sizeof(sslRsaCert_t));
	memset(cert, '\0', sizeof(sslRsaCert_t));

	p = *pp;
	end = p + size;
/*
	Certificate  ::=  SEQUENCE  {
        tbsCertificate       TBSCertificate,
        signatureAlgorithm   AlgorithmIdentifier,
        signatureValue       BIT STRING	}
*/
	if (getSequence(&p, (int)(end - p), &len) < 0) {
		matrixStrDebugMsg("Initial cert parse error\n", NULL);
		return -1;
	}
	certStart = p;
/*	
	TBSCertificate  ::=  SEQUENCE  {
		version         [0]  EXPLICIT Version DEFAULT v1,
		serialNumber         CertificateSerialNumber,
		signature            AlgorithmIdentifier,
		issuer               Name,
		validity             Validity,
		subject              Name,
		subjectPublicKeyInfo SubjectPublicKeyInfo,
		issuerUniqueID  [1]  IMPLICIT UniqueIdentifier OPTIONAL,
                             -- If present, version shall be v2 or v3
		subjectUniqueID [2]  IMPLICIT UniqueIdentifier OPTIONAL,
                             -- If present, version shall be v2 or v3
		extensions      [3]  EXPLICIT Extensions OPTIONAL
                             -- If present, version shall be v3	}
*/
	if (getSequence(&p, (int)(end - p), &len) < 0) {
		matrixStrDebugMsg("ASN sequence parse error\n", NULL);
		return -1;
	}
	certEnd = p + len;
	certLen = (int)(certEnd - certStart);

/*
	Version  ::=  INTEGER  {  v1(0), v2(1), v3(2)  }
*/
	if (getExplicitVersion(&p, (int)(end - p), 0, &cert->version) < 0) {
		matrixStrDebugMsg("ASN version parse error\n", NULL);
		return -1;
	}
	if (cert->version != 2) {
		matrixIntDebugMsg("Unsupported certificate version: %d\n",
			cert->version);
		return -1;
	}
/*
	CertificateSerialNumber  ::=  INTEGER
*/
	if (getSerialNum(&p, (int)(end - p), &cert->serialNumber,
			&cert->serialNumberLen) < 0) {
		matrixStrDebugMsg("ASN serial number parse error\n", NULL);
		return -1;
	}
/*
	AlgorithmIdentifier  ::=  SEQUENCE  {
        algorithm               OBJECT IDENTIFIER,
        parameters              ANY DEFINED BY algorithm OPTIONAL }
*/
	if (getAlgorithmIdentifier(&p, (int)(end - p),
			&cert->certAlgorithm, 0) < 0) {
		return -1;
	}
/*
	Name ::= CHOICE {
		RDNSequence }

	RDNSequence ::= SEQUENCE OF RelativeDistinguishedName

	RelativeDistinguishedName ::= SET OF AttributeTypeAndValue

	AttributeTypeAndValue ::= SEQUENCE {
		type     AttributeType,
		value    AttributeValue }

	AttributeType ::= OBJECT IDENTIFIER

	AttributeValue ::= ANY DEFINED BY AttributeType
*/
	if (getDNAttributes(&p, (int)(end - p), &cert->issuer) < 0) {
		return -1;
	}
/*
	Validity ::= SEQUENCE {
        notBefore      Time,
        notAfter       Time	}
*/
	if (getValidity(&p, (int)(end - p), &cert->notBefore,
			&cert->notAfter) < 0) {
		return -1;
	}
/*
	Subject DN
*/
	if (getDNAttributes(&p, (int)(end - p), &cert->subject) < 0) {
		return -1;
	}
/*
	SubjectPublicKeyInfo  ::=  SEQUENCE  {
        algorithm            AlgorithmIdentifier,
        subjectPublicKey     BIT STRING  }
*/
	if (getSequence(&p, (int)(end - p), &len) < 0) {
		return -1;
	}
	if (getAlgorithmIdentifier(&p, (int)(end - p),
			&cert->pubKeyAlgorithm, 1) < 0) {
		return -1;
	}
	if (getPubKey(&p, (int)(end - p), &cert->publicKey) < 0) {
		return -1;
	}
/*
	As the next three values are optional, we can do a specific test here
*/
	if (*p != (ASN_SEQUENCE | ASN_CONSTRUCTED)) {
		if (getImplicitBitString(&p, (int)(end - p), IMPLICIT_ISSUER_ID,
				&cert->uniqueUserId, &cert->uniqueUserIdLen) < 0 ||
			getImplicitBitString(&p, (int)(end - p), IMPLICIT_SUBJECT_ID,
				&cert->uniqueSubjectId, &cert->uniqueSubjectIdLen) < 0 ||
			getExplicitExtensions(&p, (int)(end - p), EXPLICIT_EXTENSION,
				&cert->extensions, &len) < 0) {
			return -1;
		}
	}
/*
	This is the end of the cert.  Do a check here to be certain
*/
	if (certEnd != p) {
		return -1;
	}
/*
	Certificate signature info
*/
	if (getAlgorithmIdentifier(&p, (int)(end - p),
			&cert->sigAlgorithm, 0) < 0) {
		return -1;
	}
/*
	Signature algorithm must match that specified in TBS cert
*/
	if (cert->certAlgorithm != cert->sigAlgorithm) {
		matrixStrDebugMsg("Parse error: mismatched signature type\n", NULL);
		return -1; 
	}
/*
	Compute the hash of the cert here for CA validation
*/
	if (cert->certAlgorithm == OID_RSA_MD5) {
		matrixMd5Init(&md5Ctx);
		matrixMd5Update(&md5Ctx, certStart, certLen);
		matrixMd5Final(&md5Ctx, cert->sigHash);
	} else if (cert->certAlgorithm == OID_RSA_SHA1) {
		matrixSha1Init(&sha1Ctx);
		matrixSha1Update(&sha1Ctx, certStart, certLen);
		matrixSha1Final(&sha1Ctx, cert->sigHash);
	}
	if (getSignature(&p, (int)(end - p), &cert->signature,
			&cert->signatureLen) < 0) {
		return -1;
	}

	if (p != end) {
		matrixStrDebugMsg("Warning: Cert parse did not reach end of bufffer\n",
			NULL);
	}
	*pp = p;
	return 0;
}


/******************************************************************************/
/*
	User must call after all calls to matrixX509ParseCert
	(we violate the coding standard a bit here for clarity)
*/
void matrixX509FreeCert(sslRsaCert_t *cert)
{
	if (cert) {
		if (cert->issuer.country) sslFree(cert->issuer.country);
		if (cert->issuer.state) sslFree(cert->issuer.state);
		if (cert->issuer.locality) sslFree(cert->issuer.locality);
		if (cert->issuer.organization) sslFree(cert->issuer.organization);
		if (cert->issuer.orgUnit) sslFree(cert->issuer.orgUnit);
		if (cert->issuer.commonName) sslFree(cert->issuer.commonName);
		if (cert->subject.country) sslFree(cert->subject.country);
		if (cert->subject.state) sslFree(cert->subject.state);
		if (cert->subject.locality) sslFree(cert->subject.locality);
		if (cert->subject.organization) sslFree(cert->subject.organization);
		if (cert->subject.orgUnit) sslFree(cert->subject.orgUnit);
		if (cert->subject.commonName) sslFree(cert->subject.commonName);
		if (cert->serialNumber) sslFree(cert->serialNumber);
		if (cert->notBefore) sslFree(cert->notBefore);
		if (cert->notAfter) sslFree(cert->notAfter);
		if (cert->publicKey.N.dp) mp_clear(&(cert->publicKey.N));
		if (cert->publicKey.e.dp) mp_clear(&(cert->publicKey.e));
		if (cert->signature) sslFree(cert->signature);
		if (cert->uniqueUserId) sslFree(cert->uniqueUserId);
		if (cert->uniqueSubjectId) sslFree(cert->uniqueSubjectId);
		if (cert->extensions.san.dns) sslFree(cert->extensions.san.dns);
		if (cert->extensions.san.uri) sslFree(cert->extensions.san.uri);
		if (cert->extensions.san.email) sslFree(cert->extensions.san.email);
#ifdef USE_FULL_CERT_PARSE
		if (cert->extensions.sk.id) sslFree(cert->extensions.sk.id);
		if (cert->extensions.ak.keyId) sslFree(cert->extensions.ak.keyId);
		if (cert->extensions.ak.attribs.commonName)
			sslFree(cert->extensions.ak.attribs.commonName);
		if (cert->extensions.ak.attribs.country)
			sslFree(cert->extensions.ak.attribs.country);
		if (cert->extensions.ak.attribs.state)
			sslFree(cert->extensions.ak.attribs.state);
		if (cert->extensions.ak.attribs.locality)
			sslFree(cert->extensions.ak.attribs.locality);
		if (cert->extensions.ak.attribs.organization)
			sslFree(cert->extensions.ak.attribs.organization);
		if (cert->extensions.ak.attribs.orgUnit)
			sslFree(cert->extensions.ak.attribs.orgUnit);
#endif /* SSL_FULL_CERT_PARSE */
		sslFree(cert);
	}
}	

/******************************************************************************/
/*
	Do the signature validation for a subject certificate against a
	known CA certificate
*/
int asnConfirmSignature(sslRsaCert_t *subjectCert,
						unsigned char *sigOut, int sigLen)
{
	unsigned char	*end, *p = sigOut;
	unsigned char	hash[SSL_SHA1_HASH_SIZE];
	int				len, oi;

	end = p + sigLen;
/*
	DigestInfo ::= SEQUENCE {
		digestAlgorithm DigestAlgorithmIdentifier,
		digest Digest }

	DigestAlgorithmIdentifier ::= AlgorithmIdentifier

	Digest ::= OCTET STRING
*/
	if (getSequence(&p, (int)(end - p), &len) < 0) {
		return -1;
	}
/*
	Could be MD5 or SHA1
 */
	if (getAlgorithmIdentifier(&p, (int)(end - p), &oi, 0) < 0) {
		return -1;
	}
	if ((*p++ != ASN_OCTET_STRING) ||
			asnParseLength(&p, (int)(end - p), &len) < 0 || (end - p) <  len) {
		return -1;
	}
	memcpy(hash, p, len);
	if (oi == OID_MD5) {
		if (len != SSL_MD5_HASH_SIZE) {
			return -1;
		}
	} else if (oi == OID_SHA1) {
		if (len != SSL_SHA1_HASH_SIZE) {
			return -1;
		}
	} else {
		return -1;
	}
/*
	hash should match sigHash
*/
	if (memcmp(hash, subjectCert->sigHash, len) != 0) {
		return -1;
	}
	return 0;
}

/******************************************************************************/
/*
	Implementations of this specification MUST be prepared to receive
	the following standard attribute types in issuer names:
	country, organization, organizational-unit, distinguished name qualifier,
	state or province name, and common name 
*/
static int getDNAttributes(unsigned char **pp, int len, DNattributes_t *attribs)
{
	sslSha1Context_t	hash;
	unsigned char		*p = *pp;
	unsigned char		*dnEnd;
	int					llen, setlen, arcLen, id, stringType;
	char				*stringOut;

	if (getSequence(&p, len, &llen) < 0) {
		return -1;
	}
	dnEnd = p + llen;

	matrixSha1Init(&hash);
	while (p < dnEnd) {
		if (getSet(&p, (int)(dnEnd - p), &setlen) < 0) {
			return -1;
		}
		if (getSequence(&p, (int)(dnEnd - p), &llen) < 0) {
			return -1;
		}
		if (dnEnd <= p || (*(p++) != ASN_OID) ||
				asnParseLength(&p, (int)(dnEnd - p), &arcLen) < 0 || 
				(dnEnd - p) < arcLen) {
			return -1;
		}
/*
		id-at   OBJECT IDENTIFIER       ::=     {joint-iso-ccitt(2) ds(5) 4}
		id-at-commonName        OBJECT IDENTIFIER       ::=     {id-at 3}
		id-at-countryName       OBJECT IDENTIFIER       ::=     {id-at 6}
		id-at-localityName      OBJECT IDENTIFIER       ::=     {id-at 7}
		id-at-stateOrProvinceName     OBJECT IDENTIFIER ::= {id-at 8}
		id-at-organizationName        OBJECT IDENTIFIER ::= {id-at 10}
		id-at-organizationalUnitName  OBJECT IDENTIFIER ::= {id-at 11}
*/
		*pp = p;
/*
		FUTURE: Currently skipping OIDs not of type {joint-iso-ccitt(2) ds(5) 4}
		However, we could be dealing with an OID we MUST support per RFC.
		domainComponent is one such example.
*/
		if (dnEnd - p < 2) {
			return -1;
		}
		if ((*p++ != 85) || (*p++ != 4) ) {
			p = *pp;
/*
			Move past the OID and string type, get data size, and skip it.
			NOTE:  Have had problems parsing older certs in this area.
*/
			if (dnEnd - p < arcLen + 1) {
				return -1;
			}
			p += arcLen + 1;
			if (asnParseLength(&p, (int)(dnEnd - p), &llen) < 0 || 
					dnEnd - p < llen) {
				return -1;
			}
			p = p + llen;
			continue;
		}
/*
		Next are the id of the attribute type and the ASN string type
*/
		if (arcLen != 3 || dnEnd - p < 2) {
			return -1;
		}
		id = (int)*p++;
/*
		Done with OID parsing
*/
		stringType = (int)*p++;

		asnParseLength(&p, (int)(dnEnd - p), &llen);
		if (dnEnd - p < llen) {
			return -1;
		}
		switch (stringType) {
			case ASN_PRINTABLESTRING:
			case ASN_UTF8STRING:
			case ASN_T61STRING:
				stringOut = sslMalloc(llen + 1);
				memcpy(stringOut, p, llen);
				stringOut[llen] = '\0';
				p = p + llen;
				break;
			default:
				matrixStrDebugMsg("Parsing untested DN attrib type\n", NULL);
				return -1;
		}

		switch (id) {
			case ATTRIB_COUNTRY_NAME:
				attribs->country = stringOut;
				break;
			case ATTRIB_STATE_PROVINCE:
				attribs->state = stringOut;
				break;
			case ATTRIB_LOCALITY:
				attribs->locality = stringOut;
				break;
			case ATTRIB_ORGANIZATION:
				attribs->organization = stringOut;
				break;
			case ATTRIB_ORG_UNIT:
				attribs->orgUnit = stringOut;
				break;
			case ATTRIB_COMMON_NAME:
				attribs->commonName = stringOut;
				break;
/*
			Not a MUST support
*/
			default:
				sslFree(stringOut);
				return -1;
		}
/*
		Hash up the DN.  Nice for validation later
*/
		if (stringOut != NULL) {
			matrixSha1Update(&hash, stringOut,
				(unsigned long)strlen(stringOut));
		}
	}

	matrixSha1Final(&hash, attribs->hash);
	*pp = p;
	return 0;
}

/******************************************************************************/
/*
	X509v3 extensions
*/
static int getExplicitExtensions(unsigned char **pp, int inlen, int expVal,
								 v3extensions_t *extensions, int *outLen)
{
	unsigned char	*p = *pp, *end;
	unsigned char	*extEnd;
	int				len, oid, tmpLen;

	end = p + inlen;
	if (inlen < 1) {
		return -1;
	}
/*
	Not treating this as an error because it is optional
*/
	if (*p != (ASN_CONTEXT_SPECIFIC | ASN_CONSTRUCTED | expVal)) {
		return 0;
	}
	*p++;
	if (asnParseLength(&p, (int)(end - p), &len) < 0 || (end - p) < len) {
		return -1;
	}
/*
	Extensions  ::=  SEQUENCE SIZE (1..MAX) OF Extension

	Extension  ::=  SEQUENCE  {
        extnID      OBJECT IDENTIFIER,
        extnValue   OCTET STRING }
*/
	if (getSequence(&p, (int)(end - p), &len) < 0) {  /* 48 */
		return -1;
	}
	extEnd = p + len;
	while ((p != extEnd) && *p == (ASN_SEQUENCE | ASN_CONSTRUCTED)) {
		if (getSequence(&p, (int)(extEnd - p), &len) < 0) {
			return -1;
		}
/*
		Conforming CAs MUST support key identifiers, basic constraints,
		key usage, and certificate policies extensions
	
		id-ce-authorityKeyIdentifier OBJECT IDENTIFIER ::=  { id-ce 35 }
		id-ce-basicConstraints OBJECT IDENTIFIER ::=  { id-ce 19 }
		id-ce-keyUsage OBJECT IDENTIFIER ::=  { id-ce 15 }
		id-ce-certificatePolicies OBJECT IDENTIFIER ::=  { id-ce 32 }
		id-ce-subjectAltName OBJECT IDENTIFIER ::=  { id-ce 17 }

*/
		if (extEnd - p < 1 || *p++ != ASN_OID) {
			return -1;
		}
		oid = 0;
		if (asnParseLength(&p, (int)(extEnd - p), &len) < 0 || 
				(extEnd - p) < len) {
			return -1;
		}
		while (len-- > 0) {
			oid += (int)*p++;
		}
		if (extEnd - p < 1 || (*p++ != ASN_OCTET_STRING) ||
				asnParseLength(&p, (int)(extEnd - p), &len) < 0 || 
				extEnd - p < len) {
			matrixStrDebugMsg("Expecting OCTET STRING in ext parse\n", NULL);
			matrixStrDebugMsg("Seen this parse error on older certs\n", NULL);
			return -1;
		}

		switch (oid) {
/*
			 BasicConstraints ::= SEQUENCE {
				cA                      BOOLEAN DEFAULT FALSE,
				pathLenConstraint       INTEGER (0..MAX) OPTIONAL }
*/
			case EXT_BASIC_CONSTRAINTS:
				if (getSequence(&p, (int)(extEnd - p), &len) < 0) {
					return -1;
				}
/*
				"This goes against PKIX guidelines but some CAs do it and some
				software requires this to avoid interpreting an end user
				certificate as a CA."
					- OpenSSL certificate configuration doc

				basicConstraints=CA:FALSE
*/
				if (len == 0) {
					break;
				}
				if (extEnd - p < 3) {
					return -1;
				}
				if (*p++ != ASN_BOOLEAN) {
					return -1;
				}
				if (*p++ != 1) {
					return -1;
				}
				extensions->bc.ca = *p++;
/*
				Now need to check if there is a path constraint. Only makes
				sense if cA is true.  If it's missing, there is no limit to
				the cert path
*/
				if (*p == ASN_INTEGER) {
					if (getInteger(&p, (int)(extEnd - p),
							&(extensions->bc.pathLenConstraint)) < 0) {
						return -1;
					}
				} else {
					extensions->bc.pathLenConstraint = -1;
				}
				break;
			case EXT_ALT_SUBJECT_NAME:
				if (getSequence(&p, (int)(extEnd - p), &len) < 0) {
					return -1;
				}
/*
				Looking only for DNS, URI, and email here to support
				FQDN for Web clients

				FUTURE:  Support all subject alt name members
				GeneralName ::= CHOICE {
					otherName                       [0]     OtherName,
					rfc822Name                      [1]     IA5String,
					dNSName                         [2]     IA5String,
					x400Address                     [3]     ORAddress,
					directoryName                   [4]     Name,
					ediPartyName                    [5]     EDIPartyName,
					uniformResourceIdentifier       [6]     IA5String,
					iPAddress                       [7]     OCTET STRING,
					registeredID                    [8]     OBJECT IDENTIFIER }
*/
				while (len > 0) {
					if (*p == (ASN_CONTEXT_SPECIFIC | ASN_PRIMITIVE | 2)) {
						p++;
						tmpLen = *p++;
						extensions->san.dns = sslMalloc(tmpLen + 1);
						memset(extensions->san.dns, 0x0, tmpLen + 1);
						memcpy(extensions->san.dns, p, tmpLen);
					} else if (*p == (ASN_CONTEXT_SPECIFIC | ASN_PRIMITIVE | 6)) {
						p++;
						tmpLen = *p++;
						extensions->san.uri = sslMalloc(tmpLen + 1);
						memset(extensions->san.uri, 0x0, tmpLen + 1);
						memcpy(extensions->san.uri, p, tmpLen);
					} else if (*p == (ASN_CONTEXT_SPECIFIC | ASN_PRIMITIVE | 1)) {
						p++;
						tmpLen = *p++;
						extensions->san.email = sslMalloc(tmpLen + 1);
						memset(extensions->san.email, 0x0, tmpLen + 1);
						memcpy(extensions->san.email, p, tmpLen);
					} else {
						matrixStrDebugMsg("Unsupported subjectAltName type.n",
							NULL);
						p++;
						tmpLen = *p++;	
					}
					p = p + tmpLen;
					len -= tmpLen + 2; /* the magic 2 is the type and length */
				}
				break;
#ifdef USE_FULL_CERT_PARSE
			case EXT_AUTH_KEY_ID:
/*
				AuthorityKeyIdentifier ::= SEQUENCE {
				keyIdentifier             [0] KeyIdentifier           OPTIONAL,
				authorityCertIssuer       [1] GeneralNames            OPTIONAL,
				authorityCertSerialNumber [2] CertificateSerialNumber OPTIONAL }

				KeyIdentifier ::= OCTET STRING
*/
				if (getSequence(&p, (int)(extEnd - p), &len) < 0 || len < 1) {
					return -1;
				}
/*
				All memebers are optional
*/
				if (*p == (ASN_CONTEXT_SPECIFIC | ASN_PRIMITIVE | 0)) {
					*p++;
					if (asnParseLength(&p, (int)(extEnd - p), 
							&extensions->ak.keyLen) < 0 ||
							extEnd - p < extensions->ak.keyLen) {
						return -1;
					}
					extensions->ak.keyId = sslMalloc(extensions->ak.keyLen);
					memcpy(extensions->ak.keyId, p, extensions->ak.keyLen);
					p = p + extensions->ak.keyLen;
				}
				if (*p == (ASN_CONTEXT_SPECIFIC | ASN_CONSTRUCTED | 1)) {
					*p++;
					if (asnParseLength(&p, (int)(extEnd - p), &len) < 0 ||
							len < 1 || extEnd - p < len) {
						return -1;
					}
					if ((*p ^ ASN_CONTEXT_SPECIFIC ^ ASN_CONSTRUCTED) != 4) {
/*
						FUTURE: support other name types
						We are just dealing with DN formats here
*/
						matrixIntDebugMsg("Error auth key-id name type: %d\n",
							*p ^ ASN_CONTEXT_SPECIFIC ^ ASN_CONSTRUCTED);
						return -1;
					}
					p++;
					if (asnParseLength(&p, (int)(extEnd - p), &len) < 0 || 
							extEnd - p < len) {
						return -1;
					}
					if (getDNAttributes(&p, (int)(extEnd - p),
							&(extensions->ak.attribs)) < 0) {
						return -1;
					}
				}
				if (*p == (ASN_CONTEXT_SPECIFIC | ASN_PRIMITIVE | 2)) {
/*
					Standard getInteger doesn't like CONTEXT_SPECIFIC tag
*/
					*p &= ASN_INTEGER;
					if (getInteger(&p, (int)(extEnd - p), 
							&(extensions->ak.serialNum)) < 0) {
						return -1;
					}
				}

				break;

			case EXT_KEY_USAGE:
/*
				KeyUsage ::= BIT STRING {
					digitalSignature        (0),
					nonRepudiation          (1),
					keyEncipherment         (2),
					dataEncipherment        (3),
					keyAgreement            (4),
					keyCertSign             (5),

					cRLSign                 (6),
					encipherOnly            (7),
					decipherOnly            (8) }
*/
				if (*p++ != ASN_BIT_STRING) {
					return -1;
				}
				if (asnParseLength(&p, (int)(extEnd - p), &len) < 0 || 
						extEnd - p < len) {
					return -1;
				}
				if (len != 2) {
					return -1;
				}
/*
				Assure all unused bits are 0 and store away
*/
				extensions->keyUsage = (*(p + 1)) & ~((1 << *p) -1);
				p = p + len;
				break;
			case EXT_SUBJ_KEY_ID:
/*
				The value of the subject key identifier MUST be the value
				placed in the key identifier field of the Auth Key Identifier
				extension of certificates issued by the subject of
				this certificate.
*/
				if (*p++ != ASN_OCTET_STRING || asnParseLength(&p,
						(int)(extEnd - p), &(extensions->sk.len)) < 0 ||
						extEnd - p < extensions->sk.len) {
					return -1;
				}
				extensions->sk.id = sslMalloc(extensions->sk.len);
				memcpy(extensions->sk.id, p, extensions->sk.len);
				p = p + extensions->sk.len;
				break;
#endif /*  USE_FULL_CERT_PARSE */
/*
			Unsupported or skipping because USE_FULL_CERT_PARSE is undefined
*/
			default:
				*p++;
				if (asnParseLength(&p, (int)(extEnd - p), &len) < 0 ||
						extEnd - p < len) {
					return -1;
				}
				p = p + len;
				break;
		}
	}
	*pp = p;
	return 0;
}

#endif /* USE_X509 */

/******************************************************************************/
/*
	On success, p will be updated to point to first character of value and
	valLen will contain number of bytes in value
	Return:
		0			Success
		< 0			Error
*/
static int asnParseLength(unsigned char **p, int size, int *valLen)
{
	unsigned char	*c, *end;
	int				len, olen;

	c = *p;
	end = c + size;
	if (end - c < 1) {
		return -1;
	}
/*
	If the length byte has high bit only set, it's an indefinite length
	We don't support this!
*/
	if (*c == 0x80) {
		*valLen = -1;
		matrixStrDebugMsg("Unsupported: ASN indefinite length\n", NULL);
		return -1;
	}
/*
	If the high bit is set, the lower 7 bits represent the number of 
	bytes that follow and represent length
	If the high bit is not set, the lower 7 represent the actual length
*/
	len = *c & 0x7F;
	if (*(c++) & 0x80) {
/*
		Make sure there aren't more than 4 bytes of length specifier,
		and that we have that many bytes left in the buffer
*/
		if (len > sizeof(int) || len == 0x7f || (end - c) < len) {
			return -1;
		}
		olen = 0;
		while (len-- > 0) {
			olen = (olen << 8) | *c;
			c++;
		}
		if (olen < 0 || olen > INT_MAX) {
			return -1;
		}
		len = olen;
	}
	*p = c;
	*valLen = len;
	return 0;
}

/******************************************************************************/
/*
	Callback to extract a big int (stream of bytes) from the DER stream
*/
static int getBig(unsigned char **pp, int len, mp_int *big)
{
	unsigned char	*p = *pp;
	int				vlen;

	if (len < 1 || *(p++) != ASN_INTEGER ||
			asnParseLength(&p, len - 1, &vlen) < 0) {
		matrixStrDebugMsg("ASN getBig failed\n", NULL);
		return -1;
	}
	mp_init(big);
	if (mp_read_unsigned_bin(big, p, vlen) != 0) {
		mp_clear(big);
		matrixStrDebugMsg("ASN getBig failed\n", NULL);
		return -1;
	}
	p += vlen;
	*pp = p;
	return 0;
}

/******************************************************************************/
/*
	Although a certificate serial number is encoded as an integer type, that
	doesn't prevent it from being abused as containing a variable length
	binary value.  Get it here.
*/	
static int getSerialNum(unsigned char **pp, int len, unsigned char **sn,
						int *snLen)
{
	unsigned char	*p = *pp;
	int				vlen;

	if (len < 1 || *(p++) != ASN_INTEGER ||
			asnParseLength(&p, len - 1, &vlen) < 0) {
		matrixStrDebugMsg("ASN getSerialNumber failed\n", NULL);
		return -1;
	}
	*snLen = vlen;
	*sn = sslMalloc(vlen);
	memcpy(*sn, p, vlen);
	p += vlen;
	*pp = p;
	return 0;
}

/******************************************************************************/
/*
	Callback to extract a sequence length from the DER stream
	Verifies that 'len' bytes are >= 'seqlen'
	Move pp to the first character in the sequence
*/
static int getSequence(unsigned char **pp, int len, int *seqlen)
{
	unsigned char	*p = *pp;

	if (len < 1 || *(p++) != (ASN_SEQUENCE | ASN_CONSTRUCTED) || 
			asnParseLength(&p, len - 1, seqlen) < 0 || len < *seqlen) {
		return -1;
	}
	*pp = p;
	return 0;
}

/******************************************************************************/
/*
	Extract a set length from the DER stream
*/
static int getSet(unsigned char **pp, int len, int *setlen)
{
	unsigned char	*p = *pp;

	if (len < 1 || *(p++) != (ASN_SET | ASN_CONSTRUCTED) || 
			asnParseLength(&p, len - 1, setlen) < 0 || len < *setlen) {
		return -1;
	}
	*pp = p;
	return 0;
}

/******************************************************************************/
/*
	Explicit value encoding has an additional tag layer
*/
static int getExplicitVersion(unsigned char **pp, int len, int expVal, int *val)
{
	unsigned char	*p = *pp;
	int				exLen;

	if (len < 1) {
		return -1;
	}
/*
	This is an optional value, so don't error if not present.  The default
	value is version 1
*/	
	if (*p != (ASN_CONTEXT_SPECIFIC | ASN_CONSTRUCTED | expVal)) {
		*val = 0;
		return 0;
	}
	*p++;
	if (asnParseLength(&p, len - 1, &exLen) < 0 || (len - 1) < exLen) {
		return -1;
	}
	if (getInteger(&p, exLen, val) < 0) {
		return -1;
	}
	*pp = p;
	return 0;
}

/******************************************************************************/
/*
	Implementation specific OID parser for suppported RSA algorithms
*/
static int getAlgorithmIdentifier(unsigned char **pp, int len, int *oi,
								  int isPubKey)
{
	unsigned char	*p = *pp, *end;
	int				arcLen, llen;

	end = p + len;
	if (len < 1 || getSequence(&p, len, &llen) < 0) {
		return -1;
	}
	if (end - p < 1) {
		return -1;
	}
	if (*(p++) != ASN_OID || asnParseLength(&p, (int)(end - p), &arcLen) < 0 ||
			llen < arcLen) {
		return -1;
	}
/*
	List of expected (currently supported) OIDs
	algorithm				  summed	length		hex
	sha1						 88       05       2b0e03021a
	md5							649		  08	   2a864886f70d0205
	rsaEncryption				645		  09	   2a864886f70d010101
	md2WithRSAEncryption:       646       09       2a864886f70d010102
	md5WithRSAEncryption        648       09       2a864886f70d010104
	sha-1WithRSAEncryption      649       09       2a864886f70d010105

	Yes, the summing isn't ideal (as can be seen with the duplicate 649),
	but the specific implementation makes this ok.
*/
	if (end - p < 2) {
		return -1;
	}
	if (isPubKey && (*p != 0x2a) && (*(p + 1) != 0x86)) {
/*
		Expecting DSA here if not RSA, but OID doesn't always match
*/
		matrixStrDebugMsg("Unsupported algorithm identifier\n", NULL);
		return -1;
	}
	*oi = 0;
	while (arcLen-- > 0) {
		*oi += (int)*p++;
	}
/*
	Each of these cases should have a trailing NULL parameter.  Skip it
*/
	if (end - p < 2) {
		return -1;
	}
	if (*p != ASN_NULL) {
		return -1;
	}
	*pp = p + 2;
	return 0;
}

/******************************************************************************/
/*
	Implementation specific date parser.
	Does not actually verify the date
*/
static int getValidity(unsigned char **pp, int len,
					   char **notBefore, char **notAfter)
{
	unsigned char	*p = *pp, *end;
	int				seqLen, timeLen;

	end = p + len;
	if (len < 1 || *(p++) != (ASN_SEQUENCE | ASN_CONSTRUCTED) || 
			asnParseLength(&p, len - 1, &seqLen) < 0 || (end - p) < seqLen) {
		return -1;
	}
/*
	Have notBefore and notAfter times in UTCTime or GeneralizedTime formats
*/
	if ((end - p) < 1 || (*p != ASN_UTCTIME) && (*p != ASN_GENERALIZEDTIME)) {
		return -1;
	}
	*p++;
/*
	Allocate them as null terminated strings
*/
	if (asnParseLength(&p, seqLen, &timeLen) < 0 || (end - p) < timeLen) {
		return -1;
	}
	*notBefore = sslMalloc(timeLen + 1);
	memcpy(*notBefore, p, timeLen);
	(*notBefore)[timeLen] = '\0';
	p = p + timeLen;
	if ((end - p) < 1 || (*p != ASN_UTCTIME) && (*p != ASN_GENERALIZEDTIME)) {
		return -1;
	}
	*p++;
	if (asnParseLength(&p, seqLen - timeLen, &timeLen) < 0 || 
			(end - p) < timeLen) {
		return -1;
	}
	*notAfter = sslMalloc(timeLen + 1);
	memcpy(*notAfter, p, timeLen);
	(*notAfter)[timeLen] = '\0';
	p = p + timeLen;

	*pp = p;
	return 0;
}

/******************************************************************************/
/*
	Get the BIT STRING key and plug into RSA structure
*/
static int getPubKey(unsigned char **pp, int len, rsa_key *pubKey)
{
	unsigned char	*p = *pp;
	int				pubKeyLen, ignore_bits, seqLen;

	if (len < 1 || (*(p++) != ASN_BIT_STRING) ||
			asnParseLength(&p, len - 1, &pubKeyLen) < 0 || 
			(len - 1) < pubKeyLen) {
		return -1;
	}
	
	ignore_bits = *p++;
/*
	We assume this is always zero
*/
	sslAssert(ignore_bits == 0);

	if (getSequence(&p, pubKeyLen, &seqLen) < 0 ||
			getBig(&p, seqLen, &pubKey->N) < 0 ||
			getBig(&p, seqLen, &pubKey->e) < 0) {
		return -1;
	}
	pubKey->size = mp_unsigned_bin_size(&pubKey->N);

	*pp = p;
	return 0;
}

/******************************************************************************/
/*
	Currently just returning the raw BIT STRING and size in bytes
*/
static int getSignature(unsigned char **pp, int len,
						unsigned char **sig, int *sigLen)
{
	unsigned char	*p = *pp, *end;
	int				ignore_bits, llen;
	
	end = p + len;
	if (len < 1 || (*(p++) != ASN_BIT_STRING) ||
			asnParseLength(&p, len - 1, &llen) < 0 || (end - p) < llen) {
		return -1;
	}
	ignore_bits = *p++;
/*
	We assume this is always 0.
*/
	sslAssert(ignore_bits == 0);
/*
	Length included the ignore_bits byte
*/
	*sigLen = llen - 1;
	*sig = sslMalloc(*sigLen);
	memcpy(*sig, p, *sigLen);
	*pp = p + *sigLen;
	return 0;
}

/******************************************************************************/
/*
	Could be optional.  If the tag doesn't contain the value from the left
	of the IMPLICIT keyword we don't have a match and we don't incr the pointer.
*/
static int getImplicitBitString(unsigned char **pp, int len, int impVal,
						unsigned char **bitString, int *bitLen)
{
	unsigned char *p = *pp;
	int			ignore_bits;

	if (len < 1) {
		return -1;
	}
/*
	We don't treat this case as an error, because of the optional nature.
*/	
	if (*p != (ASN_CONTEXT_SPECIFIC | ASN_CONSTRUCTED | impVal)) {
		return 0;
	}

	*p++;
	if (asnParseLength(&p, len, bitLen) < 0) {
		return -1;
	}
	ignore_bits = *p++;
	sslAssert(ignore_bits == 0);

	*bitString = sslMalloc(*bitLen);
	memcpy(*bitString, p, *bitLen);
	*pp = p + *bitLen;
	return 0;
}

/******************************************************************************/
/*
	Get an integer
*/
static int getInteger(unsigned char **pp, int len, int *val)
{
	unsigned char	*p = *pp, *end;
	unsigned int	ui;
	int				vlen;

	end = p + len;
	if (len < 1 || *(p++) != ASN_INTEGER ||
			asnParseLength(&p, len - 1, &vlen) < 0) {
		matrixStrDebugMsg("ASN getInteger failed\n", NULL);
		return -1;
	}
/*
	This check prevents us from having a big positive integer where the 
	high bit is set because it will be encoded as 5 bytes (with leading 
	blank byte).  If that is required, a getUnsigned routine should be used
*/
	if (vlen > sizeof(int) || end - p < vlen) {
		matrixStrDebugMsg("ASN getInteger failed\n", NULL);
		return -1;
	}
	ui = 0;
/*
	If high bit is set, it's a negative integer, so perform the two's compliment
	Otherwise do a standard big endian read (most likely case for RSA)
*/
	if (*p & 0x80) {
		while (vlen-- > 0) {
			ui = (ui << 8) | (*p ^ 0xFF);
			p++;
		}
		vlen = (int)ui;
		vlen++;
		vlen = -vlen;
		*val = vlen;
	} else {
		while (vlen-- > 0) {
			ui = (ui << 8) | *p;
			p++;
		}
		*val = (int)ui;
	}
	*pp = p;
	return 0;
}

/******************************************************************************/



