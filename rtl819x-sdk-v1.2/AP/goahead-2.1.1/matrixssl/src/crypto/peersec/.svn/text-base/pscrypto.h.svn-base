/*
 *	pscrypto.h
 *	Release $Name:  $
 *
 *	Internal definitions for PeerSec Networks MatrixSSL cryptography provider
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

#ifndef _h_PSCRYPTO
#define _h_PSCRYPTO

#ifdef __cplusplus
extern "C" {
#endif

/*
	PeerSec crypto-specific defines.
 */
#define SMALL_CODE
#define CLEAN_STACK

/******************************************************************************/

#ifdef USE_RSA
#include "mpi.h"

#if LINUX
	#define _stat stat
#endif

/* this is the "32-bit at least" data type 
 * Re-define it to suit your platform but it must be at least 32-bits 
 */
typedef unsigned long ulong32;

/*
	RSA
*/
#define OID_RSA_MD5		648
#define OID_RSA_SHA1	649

typedef struct {
	mp_int	e, d, N, qP, pQ, dP, dQ, p, q;
	int		size;	/* Size of the key in bytes */
	int		optimized; /* 1 for optimized */
} rsa_key;

typedef rsa_key sslRsaKey_t;

#define RSA_KEY_TYPE_PUBLIC 0
#define RSA_KEY_TYPE_PRIVATE 1

/*
	8 bit bit masks for ASN.1 tag field
*/
#define ASN_PRIMITIVE			0x0
#define ASN_CONSTRUCTED			0x20

#define ASN_UNIVERSAL			0x0
#define ASN_APPLICATION			0x40
#define ASN_CONTEXT_SPECIFIC	0x80
#define ASN_PRIVATE				0xC0

/*
	ASN.1 primitive data types
*/
enum {
	ASN_BOOLEAN = 1,
	ASN_INTEGER,
	ASN_BIT_STRING,
	ASN_OCTET_STRING,
	ASN_NULL,
	ASN_OID,
	ASN_UTF8STRING = 12,
	ASN_SEQUENCE = 16,
	ASN_SET,
	ASN_PRINTABLESTRING = 19,
	ASN_T61STRING,
	ASN_IA5STRING = 22,
	ASN_UTCTIME,
	ASN_GENERALIZEDTIME
};

/*
 *	APIs
 */
extern int asnParsePrivateKey(unsigned char **pp, int size, rsa_key *key);

#ifdef USE_X509

enum {
	ALG_RSA = 645,
	ALG_MD2_RSA,
	ALG_MD5_RSA = 648,
	ALG_SHA1_RSA
};

typedef struct {
	char	*country;
	char	*state;
	char	*locality;
	char	*organization;
	char	*orgUnit;
	char	*commonName;
	char	hash[SSL_SHA1_HASH_SIZE];
} DNattributes_t;

typedef struct {
	int	ca;
	int	pathLenConstraint;
} extBasicConstraints_t;

typedef struct {
	unsigned char	*dns;
	unsigned char	*uri;
	unsigned char	*email;
} extSubjectAltName_t;

typedef struct {
	int				len;
	unsigned char	*id;
} extSubjectKeyId_t;

typedef struct {
	int				keyLen;
	unsigned char	*keyId;
	DNattributes_t	attribs;
	int				serialNum;
} extAuthKeyId_t;
/*
	FUTURE:  add support for the other extensions
*/
typedef struct {
	extBasicConstraints_t	bc;
	extSubjectAltName_t		san;
#ifdef USE_FULL_CERT_PARSE
	extSubjectKeyId_t		sk;
	extAuthKeyId_t			ak;
	char					keyUsage;
#endif /* USE_FULL_CERT_PARSE */
} v3extensions_t;

typedef struct sslRsaCert {
	int				version;
	int				valid;
	unsigned char	*serialNumber;
	int				serialNumberLen;
	DNattributes_t	issuer;
	DNattributes_t	subject;
	char			*notBefore;
	char			*notAfter;
	sslRsaKey_t		publicKey;
	int				certAlgorithm;
	int				sigAlgorithm;
	int				pubKeyAlgorithm;
	unsigned char	*signature;
	int				signatureLen;
	unsigned char	sigHash[SSL_SHA1_HASH_SIZE];
	unsigned char	*uniqueUserId;
	int				uniqueUserIdLen;
	unsigned char	*uniqueSubjectId;
	int				uniqueSubjectIdLen;
	v3extensions_t	extensions;
	struct sslRsaCert	*next;
} sslRsaCert_t;


extern int asnConfirmSignature(sslRsaCert_t *subjectCert,
						unsigned char *sigOut, int sigLen);
#endif /* USE_X509 */
#endif /* USE_RSA */

/*
 *	Private
 */
extern int base64_decode(const unsigned char *in,  unsigned int len, 
                        unsigned char *out, unsigned int *outlen);

/*
 *	Memory routines
 */
extern void zeromem(void *dst, size_t len);
extern void burn_stack(unsigned long len);

/* version */
#define CRYPT   0x0091
#define SCRYPT  "0.91"

/* max size of either a cipher/hash block or symmetric key [largest of the two] */
#define MAXBLOCKSIZE           24

/* ch1-01-1 */
/* error codes [will be expanded in future releases] */
enum {
   CRYPT_OK=0,             /* Result OK */
   CRYPT_ERROR,            /* Generic Error */
   CRYPT_NOP,              /* Not a failure but no operation was performed */

   CRYPT_INVALID_KEYSIZE,  /* Invalid key size given */
   CRYPT_INVALID_ROUNDS,   /* Invalid number of rounds */
   CRYPT_FAIL_TESTVECTOR,  /* Algorithm failed test vectors */

   CRYPT_BUFFER_OVERFLOW,  /* Not enough space for output */
   CRYPT_INVALID_PACKET,   /* Invalid input packet given */

   CRYPT_INVALID_PRNGSIZE, /* Invalid number of bits for a PRNG */
   CRYPT_ERROR_READPRNG,   /* Could not read enough from PRNG */

   CRYPT_INVALID_CIPHER,   /* Invalid cipher specified */
   CRYPT_INVALID_HASH,     /* Invalid hash specified */
   CRYPT_INVALID_PRNG,     /* Invalid PRNG specified */

   CRYPT_MEM,              /* Out of memory */

   CRYPT_PK_TYPE_MISMATCH, /* Not equivalent types of PK keys */
   CRYPT_PK_NOT_PRIVATE,   /* Requires a private PK key */

   CRYPT_INVALID_ARG,      /* Generic invalid argument */
   CRYPT_FILE_NOTFOUND,    /* File Not Found */

   CRYPT_PK_INVALID_TYPE,  /* Invalid type of PK key */
   CRYPT_PK_INVALID_SYSTEM,/* Invalid PK system specified */
   CRYPT_PK_DUP,           /* Duplicate key already in key ring */
   CRYPT_PK_NOT_FOUND,     /* Key not found in keyring */
   CRYPT_PK_INVALID_SIZE,  /* Invalid size input for PK parameters */

   CRYPT_INVALID_PRIME_SIZE/* Invalid size of prime requested */
};
/* ch1-01-1 */

/* ---- HASH FUNCTIONS ---- */

struct sha1_state {
    ulong64 length;
    unsigned long state[5], curlen;
    unsigned char buf[64];
};

struct md5_state {
    ulong64 length;
    unsigned long state[4], curlen;
    unsigned char buf[64];
};

typedef union {
    struct sha1_state   sha1;
    struct md5_state    md5;
} hash_state;

typedef hash_state sslSha1Context_t;
typedef hash_state sslMd5Context_t;

/*
 * RC4
 */
#ifdef USE_ARC4
typedef struct {
	unsigned char state[256];
	unsigned int byteCount;
	unsigned char x;
	unsigned char y;
} rc4_key;
#endif /* USE_ARC4 */

#ifdef USE_3DES
#define SSL_DES3_KEY_LEN 24
#define SSL_DES3_IV_LEN 8

typedef struct {
    ulong32 ek[3][32], dk[3][32];
} des3_key;

/* A block cipher CBC structure */
typedef struct {
   int				blocklen;
   unsigned char	IV[8];
   des3_key			key;
} des3_CBC;
#endif /* USE_3DES */

#ifdef USE_AES
typedef struct {
	ulong32 eK[64], dK[64];
	int Nr;
} aes_key;

typedef struct {
	int				blocklen;
	unsigned char	IV[16];
	aes_key			key;
} aes_CBC;
#endif /* USE_AES */

typedef union {
#ifdef USE_ARC4
    rc4_key		arc4;
#endif
#ifdef USE_3DES
    des3_CBC	des3;
#endif
#ifdef USE_AES
	aes_CBC		aes;
#endif
} sslCipherContext_t;

/* Controls endianess and size of registers.  Leave uncommented to get platform neutral [slower] code */
/* detect x86-32 machines somewhat */
#if (defined(_MSC_VER) && defined(WIN32)) || (defined(__GNUC__) && (defined(__DJGPP__) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__i386__)))
   #define ENDIAN_LITTLE
   #define ENDIAN_32BITWORD
#endif

/* detects MIPS R5900 processors (PS2) */
#if (defined(__R5900) || defined(R5900) || defined(__R5900__)) && (defined(_mips) || defined(__mips__) || defined(mips))
   #define ENDIAN_LITTLE
   #define ENDIAN_64BITWORD
#endif

/* #define ENDIAN_LITTLE */
//sc_yang
#ifndef __i386__
//#define ENDIAN_BIG

//#define ENDIAN_32BITWORD
//#define ENDIAN_NEUTRAL
#endif
/* #define ENDIAN_64BITWORD */

#if (defined(ENDIAN_BIG) || defined(ENDIAN_LITTLE)) && !(defined(ENDIAN_32BITWORD) || defined(ENDIAN_64BITWORD))
    #error You must specify a word size as well as endianess in mycrypt_cfg.h
#endif

#if !(defined(ENDIAN_BIG) || defined(ENDIAN_LITTLE))
   #define ENDIAN_NEUTRAL
#endif

/* packet code */
#if defined(USE_RSA) || defined(MDH) || defined(MECC)
    #define PACKET

    /* size of a packet header in bytes */
    #define PACKET_SIZE            4

    /* Section tags */
    #define PACKET_SECT_RSA        0
    #define PACKET_SECT_DH         1
    #define PACKET_SECT_ECC        2
    #define PACKET_SECT_DSA        4

    /* Subsection Tags for the first three sections */
    #define PACKET_SUB_KEY         0
    #define PACKET_SUB_ENCRYPTED   1
    #define PACKET_SUB_SIGNED      2
    #define PACKET_SUB_ENC_KEY     3
#endif

/* fix for MSVC ...evil! */
#ifdef WIN32
#ifdef _MSC_VER
   #define CONST64(n) n ## ui64
   typedef unsigned __int64 ulong64;
#else
   #define CONST64(n) n ## ULL
   typedef unsigned long long ulong64;
#endif
#endif /* WIN32 */

/* ---- HELPER MACROS ---- */
#ifdef ENDIAN_NEUTRAL

#define STORE32L(x, y)                                                                     \
     { (y)[3] = (unsigned char)(((x)>>24)&255); (y)[2] = (unsigned char)(((x)>>16)&255);   \
       (y)[1] = (unsigned char)(((x)>>8)&255); (y)[0] = (unsigned char)((x)&255); }

#define LOAD32L(x, y)                            \
     { x = ((unsigned long)((y)[3] & 255)<<24) | \
           ((unsigned long)((y)[2] & 255)<<16) | \
           ((unsigned long)((y)[1] & 255)<<8)  | \
           ((unsigned long)((y)[0] & 255)); }

#define STORE64L(x, y)                                                                     \
     { (y)[7] = (unsigned char)(((x)>>56)&255); (y)[6] = (unsigned char)(((x)>>48)&255);   \
       (y)[5] = (unsigned char)(((x)>>40)&255); (y)[4] = (unsigned char)(((x)>>32)&255);   \
       (y)[3] = (unsigned char)(((x)>>24)&255); (y)[2] = (unsigned char)(((x)>>16)&255);   \
       (y)[1] = (unsigned char)(((x)>>8)&255); (y)[0] = (unsigned char)((x)&255); }

#define LOAD64L(x, y)                                                       \
     { x = (((ulong64)((y)[7] & 255))<<56)|(((ulong64)((y)[6] & 255))<<48)| \
           (((ulong64)((y)[5] & 255))<<40)|(((ulong64)((y)[4] & 255))<<32)| \
           (((ulong64)((y)[3] & 255))<<24)|(((ulong64)((y)[2] & 255))<<16)| \
           (((ulong64)((y)[1] & 255))<<8)|(((ulong64)((y)[0] & 255))); }

#define STORE32H(x, y)                                                                     \
     { (y)[0] = (unsigned char)(((x)>>24)&255); (y)[1] = (unsigned char)(((x)>>16)&255);   \
       (y)[2] = (unsigned char)(((x)>>8)&255); (y)[3] = (unsigned char)((x)&255); }

#define LOAD32H(x, y)                            \
     { x = ((unsigned long)((y)[0] & 255)<<24) | \
           ((unsigned long)((y)[1] & 255)<<16) | \
           ((unsigned long)((y)[2] & 255)<<8)  | \
           ((unsigned long)((y)[3] & 255)); }

#define STORE64H(x, y)                                                                     \
   { (y)[0] = (unsigned char)(((x)>>56)&255); (y)[1] = (unsigned char)(((x)>>48)&255);     \
     (y)[2] = (unsigned char)(((x)>>40)&255); (y)[3] = (unsigned char)(((x)>>32)&255);     \
     (y)[4] = (unsigned char)(((x)>>24)&255); (y)[5] = (unsigned char)(((x)>>16)&255);     \
     (y)[6] = (unsigned char)(((x)>>8)&255); (y)[7] = (unsigned char)((x)&255); }

#define LOAD64H(x, y)                                                      \
   { x = (((ulong64)((y)[0] & 255))<<56)|(((ulong64)((y)[1] & 255))<<48) | \
         (((ulong64)((y)[2] & 255))<<40)|(((ulong64)((y)[3] & 255))<<32) | \
         (((ulong64)((y)[4] & 255))<<24)|(((ulong64)((y)[5] & 255))<<16) | \
         (((ulong64)((y)[6] & 255))<<8)|(((ulong64)((y)[7] & 255))); }

#endif /* ENDIAN_NEUTRAL */

#ifdef ENDIAN_LITTLE

#define STORE32H(x, y)                                                                     \
     { (y)[0] = (unsigned char)(((x)>>24)&255); (y)[1] = (unsigned char)(((x)>>16)&255);   \
       (y)[2] = (unsigned char)(((x)>>8)&255); (y)[3] = (unsigned char)((x)&255); }

#define LOAD32H(x, y)                            \
     { x = ((unsigned long)((y)[0] & 255)<<24) | \
           ((unsigned long)((y)[1] & 255)<<16) | \
           ((unsigned long)((y)[2] & 255)<<8)  | \
           ((unsigned long)((y)[3] & 255)); }

#define STORE64H(x, y)                                                                     \
   { (y)[0] = (unsigned char)(((x)>>56)&255); (y)[1] = (unsigned char)(((x)>>48)&255);     \
     (y)[2] = (unsigned char)(((x)>>40)&255); (y)[3] = (unsigned char)(((x)>>32)&255);     \
     (y)[4] = (unsigned char)(((x)>>24)&255); (y)[5] = (unsigned char)(((x)>>16)&255);     \
     (y)[6] = (unsigned char)(((x)>>8)&255); (y)[7] = (unsigned char)((x)&255); }

#define LOAD64H(x, y)                                                      \
   { x = (((ulong64)((y)[0] & 255))<<56)|(((ulong64)((y)[1] & 255))<<48) | \
         (((ulong64)((y)[2] & 255))<<40)|(((ulong64)((y)[3] & 255))<<32) | \
         (((ulong64)((y)[4] & 255))<<24)|(((ulong64)((y)[5] & 255))<<16) | \
         (((ulong64)((y)[6] & 255))<<8)|(((ulong64)((y)[7] & 255))); }

#ifdef ENDIAN_32BITWORD 

#define STORE32L(x, y)        \
     { unsigned long __t = (x); memcpy(y, &__t, 4); }

#define LOAD32L(x, y)         \
     memcpy(&(x), y, 4);

#define STORE64L(x, y)                                                                     \
     { (y)[7] = (unsigned char)(((x)>>56)&255); (y)[6] = (unsigned char)(((x)>>48)&255);   \
       (y)[5] = (unsigned char)(((x)>>40)&255); (y)[4] = (unsigned char)(((x)>>32)&255);   \
       (y)[3] = (unsigned char)(((x)>>24)&255); (y)[2] = (unsigned char)(((x)>>16)&255);   \
       (y)[1] = (unsigned char)(((x)>>8)&255); (y)[0] = (unsigned char)((x)&255); }

#define LOAD64L(x, y)                                                       \
     { x = (((ulong64)((y)[7] & 255))<<56)|(((ulong64)((y)[6] & 255))<<48)| \
           (((ulong64)((y)[5] & 255))<<40)|(((ulong64)((y)[4] & 255))<<32)| \
           (((ulong64)((y)[3] & 255))<<24)|(((ulong64)((y)[2] & 255))<<16)| \
           (((ulong64)((y)[1] & 255))<<8)|(((ulong64)((y)[0] & 255))); }

#else /* 64-bit words then  */

#define STORE32L(x, y)        \
     { unsigned long __t = (x); memcpy(y, &__t, 4); }

#define LOAD32L(x, y)         \
     { memcpy(&(x), y, 4); x &= 0xFFFFFFFF; }

#define STORE64L(x, y)        \
     { ulong64 __t = (x); memcpy(y, &__t, 8); }

#define LOAD64L(x, y)         \
    { memcpy(&(x), y, 8); }

#endif /* ENDIAN_64BITWORD */

#endif /* ENDIAN_LITTLE */

#ifdef ENDIAN_BIG
#define STORE32L(x, y)                                                                     \
     { (y)[0] = (unsigned char)(((x)>>24)&255); (y)[1] = (unsigned char)(((x)>>16)&255);   \
       (y)[2] = (unsigned char)(((x)>>8)&255); (y)[3] = (unsigned char)((x)&255); }

#define LOAD32L(x, y)                            \
     { x = ((unsigned long)((y)[0] & 255)<<24) | \
           ((unsigned long)((y)[1] & 255)<<16) | \
           ((unsigned long)((y)[2] & 255)<<8)  | \
           ((unsigned long)((y)[3] & 255)); }

#define STORE64L(x, y)                                                                     \
   { (y)[0] = (unsigned char)(((x)>>56)&255); (y)[1] = (unsigned char)(((x)>>48)&255);     \
     (y)[2] = (unsigned char)(((x)>>40)&255); (y)[3] = (unsigned char)(((x)>>32)&255);     \
     (y)[4] = (unsigned char)(((x)>>24)&255); (y)[5] = (unsigned char)(((x)>>16)&255);     \
     (y)[6] = (unsigned char)(((x)>>8)&255); (y)[7] = (unsigned char)((x)&255); }

#define LOAD64L(x, y)                                                      \
   { x = (((ulong64)((y)[0] & 255))<<56)|(((ulong64)((y)[1] & 255))<<48) | \
         (((ulong64)((y)[2] & 255))<<40)|(((ulong64)((y)[3] & 255))<<32) | \
         (((ulong64)((y)[4] & 255))<<24)|(((ulong64)((y)[5] & 255))<<16) | \
         (((ulong64)((y)[6] & 255))<<8)|(((ulong64)((y)[7] & 255))); }

#ifdef ENDIAN_32BITWORD 

#define STORE32H(x, y)        \
     { unsigned long __t = (x); memcpy(y, &__t, 4); }

#define LOAD32H(x, y)         \
     memcpy(&(x), y, 4);

#define STORE64H(x, y)                                                                     \
     { (y)[7] = (unsigned char)(((x)>>56)&255); (y)[6] = (unsigned char)(((x)>>48)&255);   \
       (y)[5] = (unsigned char)(((x)>>40)&255); (y)[4] = (unsigned char)(((x)>>32)&255);   \
       (y)[3] = (unsigned char)(((x)>>24)&255); (y)[2] = (unsigned char)(((x)>>16)&255);   \
       (y)[1] = (unsigned char)(((x)>>8)&255); (y)[0] = (unsigned char)((x)&255); }

#define LOAD64H(x, y)                                                       \
     { x = (((ulong64)((y)[7] & 255))<<56)|(((ulong64)((y)[6] & 255))<<48)| \
           (((ulong64)((y)[5] & 255))<<40)|(((ulong64)((y)[4] & 255))<<32)| \
           (((ulong64)((y)[3] & 255))<<24)|(((ulong64)((y)[2] & 255))<<16)| \
           (((ulong64)((y)[1] & 255))<<8)|(((ulong64)((y)[0] & 255))); }

#else /* 64-bit words then  */

#define STORE32H(x, y)        \
     { unsigned long __t = (x); memcpy(y, &__t, 4); }

#define LOAD32H(x, y)         \
     { memcpy(&(x), y, 4); x &= 0xFFFFFFFF; }

#define STORE64H(x, y)        \
     { ulong64 __t = (x); memcpy(y, &__t, 8); }

#define LOAD64H(x, y)         \
    { memcpy(&(x), y, 8); }

#endif /* ENDIAN_64BITWORD */
#endif /* ENDIAN_BIG */

#define BSWAP(x)  ( ((x>>24)&0x000000FFUL) | ((x<<24)&0xFF000000UL)  | \
                    ((x>>8)&0x0000FF00UL)  | ((x<<8)&0x00FF0000UL) )

#ifdef _MSC_VER

/* instrinsic rotate */
#include <stdlib.h>
#pragma intrinsic(_lrotr,_lrotl)
#define ROR(x,n) _lrotr(x,n)
#define ROL(x,n) _lrotl(x,n)
/*
#elif defined(__GNUC__) && defined(__i386__)

static inline unsigned long ROL(unsigned long word, int i)
{
	__asm__("roll %%cl,%0"
		:"=r" (word)
		:"0" (word),"c" (i));
	return word;
}

static inline unsigned long ROR(unsigned long word, int i)
{
	__asm__("rorl %%cl,%0"
		:"=r" (word)
		:"0" (word),"c" (i));
	return word;
}
*/
#else

/* rotates the hard way */
#define ROL(x, y) ( (((unsigned long)(x)<<(unsigned long)((y)&31)) | (((unsigned long)(x)&0xFFFFFFFFUL)>>(unsigned long)(32-((y)&31)))) & 0xFFFFFFFFUL)
#define ROR(x, y) ( ((((unsigned long)(x)&0xFFFFFFFFUL)>>(unsigned long)((y)&31)) | ((unsigned long)(x)<<(unsigned long)(32-((y)&31)))) & 0xFFFFFFFFUL)

#endif

#define ROL64(x, y) \
    ( (((x)<<((ulong64)(y)&63)) | \
      (((x)&CONST64(0xFFFFFFFFFFFFFFFF))>>((ulong64)64-((y)&63)))) & CONST64(0xFFFFFFFFFFFFFFFF))

#define ROR64(x, y) \
    ( ((((x)&CONST64(0xFFFFFFFFFFFFFFFF))>>((ulong64)(y)&CONST64(63))) | \
      ((x)<<((ulong64)(64-((y)&CONST64(63)))))) & CONST64(0xFFFFFFFFFFFFFFFF))

#undef MAX
#undef MIN
#define MAX(x, y) ( ((x)>(y))?(x):(y) )
#define MIN(x, y) ( ((x)<(y))?(x):(y) )

/* extract a byte portably */
/* This MSC code causes runtime errors in VS.NET, always use the other */
/*
#ifdef _MSC_VER
   #define byte(x, n) ((unsigned char)((x) >> (8 * (n))))
#else
*/
   #define byte(x, n) (((x) >> (8 * (n))) & 255)
/*
#endif 
*/
#ifdef __cplusplus
   }
#endif

#endif /* _h_PSCRYPTO */

/******************************************************************************/

