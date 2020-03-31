/*
 * security.c -- Security handler
 *
 * Copyright (c) GoAhead Software Inc., 1995-2000. All Rights Reserved.
 *
 * See the file "license.txt" for usage and redistribution license requirements
 */

/******************************** Description *********************************/

/*
 *	This module provides a basic security policy.
 */

/********************************* Includes ***********************************/

#include	"wsIntrn.h"
#include	"um.h"
#ifdef DIGEST_ACCESS_SUPPORT
#include	"websda.h"
#endif

/********************************** Defines ***********************************/
/*
 *	The following #defines change the behaviour of security in the absence 
 *	of User Management.
 *	Note that use of User management functions require prior calling of
 *	umInit() to behave correctly
 */

#ifndef USER_MANAGEMENT_SUPPORT
#define umGetAccessMethodForURL(url) AM_FULL
#define umUserExists(userid) 0
#define umUserCanAccessURL(userid, url) 1
#define umGetUserPassword(userid) websGetPassword()
#define umGetAccessLimitSecure(accessLimit) 0
#define umGetAccessLimit(url) NULL
#endif

/******************************** Local Data **********************************/

static char_t	websPassword[WEBS_MAX_PASS];	/* Access password (decoded) */
// marked by david
#if 0
#ifdef _DEBUG
static int		debugSecurity = 1;
#else
static int		debugSecurity = 0;
#endif
#endif
//sc_yang
#define LOGIN_TIMEOUT (60000*5) //(5 min)
static time_t last_access_time = {0};
int login=0;
extern int logout;
/*********************************** Code *************************************/
/*
 *	Determine if this request should be honored
 */
#ifdef LOGIN_URL
#include "LINUX/apform.h"
#include "LINUX/apmib.h"

int websSecurityHandler(webs_t wp, char_t *urlPrefix, char_t *webDir, int arg,
						char_t *url, char_t *path, char_t *query)
{
	char tmpbuf[100];
	int ret;

	apmib_get(MIB_USER_NAME, tmpbuf);
	if (tmpbuf[0] == '\0')
		return 0;

	if (!strcmp(url, "/") || !strcmp(url, "/login.asp") || !memcmp(url, "/goform", 7))
		return 0;
	ret = is_valid_user(wp);
	if (ret == 0) {
		OK_MSG1("ERROR: Access denied!", "/login.asp");
		return 1;
	}
	if (ret < 0) { // timeout
		websRedirect(wp, T("login.asp"));
		return 1;
	}

	return 0;
}

#else
int websSecurityHandler(webs_t wp, char_t *urlPrefix, char_t *webDir, int arg,
						char_t *url, char_t *path, char_t *query)
{
	char_t			*type, *userid, *password, *accessLimit;
	int				flags, nRet;
	accessMeth_t	am;

	a_assert(websValid(wp));
	a_assert(url && *url);
	a_assert(path && *path);
/*
 *	Get the critical request details
 */
	type = websGetRequestType(wp);
	password = websGetRequestPassword(wp);
	userid = websGetRequestUserName(wp);
	flags = websGetRequestFlags(wp);
/*
 *	Get the access limit for the URL.  Exit if none found.
 */
	accessLimit = umGetAccessLimit(path);
	if (accessLimit == NULL) {
		return 0;
	}
		 
/*
 *	Check to see if URL must be encrypted
 */
#ifdef WEBS_SSL_SUPPORT
	nRet = umGetAccessLimitSecure(accessLimit);
	if (nRet && ((flags | WEBS_SECURE) == 0)) {
		websStats.access++;
		websError(wp, 200, T("Access Denied\nSecure access is required."));
		trace(3, T("SEC: Non-secure access attempted on <%s>\n"), path);
		return 1;
	}
#endif

/*
 *	Get the access limit for the URL
 */
	am = umGetAccessMethodForURL(accessLimit);

	nRet = 0;
	
// for debug, david /////////////////////////////////
#if 0	
	if ((flags & WEBS_LOCAL_REQUEST) && (debugSecurity == 0)) {
/*
 *		Local access is always allowed (defeat when debugging)
 */
	} else if (am == AM_NONE) {
#endif
	if (am == AM_NONE) {

/////////////////////////////////////////////////////		
	
/*
 *		URL is supposed to be hidden!  Make like it wasn't found.
 */
		websStats.access++;
		websError(wp, 400, T("Page Not Found"));
		nRet = 1;
	} else 	if (userid && *userid) {
		if (!umUserExists(userid)) {
			websStats.access++;
// for debug			
//			websError(wp, 200, T("Access Denied\nUnknown User"));
websError(wp, 401, T("Access Denied\nUnknown User"));

			trace(3, T("SEC: Unknown user <%s> attempted to access <%s>\n"), 
				userid, path);
			nRet = 1;
		} else if (!umUserCanAccessURL(userid, accessLimit)) {
			websStats.access++;
			websError(wp, 403, T("Access Denied\nProhibited User"));
			nRet = 1;
		} else if (password && * password) {
			char_t * userpass = umGetUserPassword(userid);
			if (userpass) {
				if (gstrcmp(password, userpass) != 0) {
					websStats.access++;
// for debug					
//					websError(wp, 200, T("Access Denied\nWrong Password"));
websError(wp, 401, T("Access Denied\nWrong Password"));
					trace(3, T("SEC: Password fail for user <%s>")
								T("attempt to access <%s>\n"), userid, path);
					nRet = 1;
				}
				else if((login && 
					((time(0) - last_access_time)*1000 > LOGIN_TIMEOUT))
					|| logout){
					websError(wp, 401, T("Access Timeout\nPlease login again"));
					login=0;
					logout=0;
					nRet = 1;
					
				} else {
					last_access_time = time(0);
					login=1;
/*
 *					User and password check out.
 */
				}

				bfree (B_L, userpass);
			}
#ifdef DIGEST_ACCESS_SUPPORT
		} else if (flags & WEBS_AUTH_DIGEST) {

			char_t *digestCalc;

/*
 *			Check digest for equivalence
 */
			wp->password = umGetUserPassword(userid);

			a_assert(wp->digest);
			a_assert(wp->nonce);
			a_assert(wp->password);
							 
			digestCalc = websCalcDigest(wp);
			a_assert(digestCalc);

			if (gstrcmp(wp->digest, digestCalc) != 0) {
				websStats.access++;
				websError(wp, 200, T("Access Denied\nWrong Password"));
				nRet = 1;
			}

			bfree (B_L, digestCalc);
#endif
		} else {
/*
 *			No password has been specified
 */
#ifdef DIGEST_ACCESS_SUPPORT
			if (am == AM_DIGEST) {
				wp->flags |= WEBS_AUTH_DIGEST;
			}
#endif
			websStats.errors++;
			websError(wp, 401, 
				T("Access to this document requires a password"));
			nRet = 1;
		}
	} else if (am != AM_FULL) {
/*
 *		This will cause the browser to display a password / username
 *		dialog
 */
#ifdef DIGEST_ACCESS_SUPPORT
		if (am == AM_DIGEST) {
			wp->flags |= WEBS_AUTH_DIGEST;
		}
#endif
		websStats.errors++;
		websError(wp, 401, T("Access to this document requires a User ID"));
		nRet = 1;
	}

	bfree(B_L, accessLimit);

	return nRet;
}

#endif // LOGIN_URL

/******************************************************************************/
/*
 *	Delete the default security handler
 */
#ifndef DOWN_SIZE
void websSecurityDelete()
{
	websUrlHandlerDelete(websSecurityHandler);
}
#endif

/******************************************************************************/
/*
 *	Store the new password, expect a decoded password. Store in websPassword in 
 *	the decoded form.
 */

void websSetPassword(char_t *password)
{
	a_assert(password);

	gstrncpy(websPassword, password, TSZ(websPassword));
}

/******************************************************************************/
/*
 *	Get password, return the decoded form
 */

char_t *websGetPassword()
{
	return bstrdup(B_L, websPassword);
}

/******************************************************************************/

