/*
 * form.c -- Form processing (in-memory CGI) for the GoAhead Web server
 *
 * Copyright (c) GoAhead Software Inc., 1995-2000. All Rights Reserved.
 *
 * See the file "license.txt" for usage and redistribution license requirements
 */

/********************************** Description *******************************/

/*
 *	This module implements the /goform handler. It emulates CGI processing
 *	but performs this in-process and not as an external process. This enables
 *	a very high performance implementation with easy parsing and decoding 
 *	of query strings and posted data.
 */

/*********************************** Includes *********************************/

#include	"wsIntrn.h"

/************************************ Locals **********************************/

static sym_fd_t	formSymtab = -1;			/* Symbol table for form handlers */

/************************************* Code ***********************************/
/*
 *	Process a form request. Returns 1 always to indicate it handled the URL
 */

#ifdef ASP_SECURITY_PATCH
#include <time.h>

#define EXPIRE_TIME					360	// in sec, more than LOGIN_TIMEOUT
#define MAX_TBL_SIZE				4

struct goform_entry {
	int	valid;		
	char name[80];
	time_t time;
};

struct goform_entry  security_tbl[MAX_TBL_SIZE] = {\
		{0}, {0}, {0}, {0}};

void log_goform(char *form)
{
	int i, oldest_entry=-1;
	time_t t, oldest_time=	-1;

	for (i=0; i<MAX_TBL_SIZE; i++) {
		if (!security_tbl[i].valid ||
				(security_tbl[i].valid && 
					(time(&t) - security_tbl[i].time) > EXPIRE_TIME) ||
					(security_tbl[i].valid && !strcmp(form, security_tbl[i].name))) {	
			break;					
		}	
		else {
			if (security_tbl[i].valid) {
				if (oldest_entry == -1 || security_tbl[i].time < oldest_time) {
					oldest_entry = i;
					oldest_time = security_tbl[i].time;
				}				
			}			
		}		
	}

	if ((i < MAX_TBL_SIZE) || (i == MAX_TBL_SIZE && oldest_entry != -1)) {		
		if (i == MAX_TBL_SIZE)
			i = oldest_entry;
		
		strcpy(security_tbl[i].name, form);

		security_tbl[i].time = time(&t);
		security_tbl[i].valid = 1;		
	}
}

static void delete_goform(char *form)
{
	int i;
	for (i=0; i<MAX_TBL_SIZE; i++) {
		if (security_tbl[i].valid && !strcmp(form, security_tbl[i].name)) {
			security_tbl[i].valid = 0;
			break;
		}		
	}
}

static int is_valid_goform(char *form) 
{
	int i, valid=0;
	time_t t;
	
	for (i=0; i<MAX_TBL_SIZE; i++) {
		if (security_tbl[i].valid && !strcmp(form, security_tbl[i].name)) {
			if	((time(&t) - security_tbl[i].time) > EXPIRE_TIME) {					
				security_tbl[i].valid = 0;
				break;
			}				
			valid = 1;
			break;
		}
	}	
	return valid;
}

static int is_any_log()
{
	int i;
	for (i=0; i<MAX_TBL_SIZE; i++) {
		if (security_tbl[i].valid)
			return 1;
	}
	return 0;
}
#endif // ASP_SECURITY_PATCH

int websFormHandler(webs_t wp, char_t *urlPrefix, char_t *webDir, int arg, 
	char_t *url, char_t *path, char_t *query)
{
	sym_t		*sp;
	char_t		formBuf[FNAMESIZE];
	char_t		*cp, *formName;
	int			(*fn)(void *sock, char_t *path, char_t *args);

	a_assert(websValid(wp));
	a_assert(url && *url);
	a_assert(path && *path == '/');

	websStats.formHits++;

/*
 *	Extract the form name
 */
	gstrncpy(formBuf, path, TSZ(formBuf));
	if ((formName = gstrchr(&formBuf[1], '/')) == NULL) {
		websError(wp, 200, T("Missing form name"));
		return 1;
	}
	formName++;
	if ((cp = gstrchr(formName, '/')) != NULL) {
		*cp = '\0';
	}

/*
 *	Lookup the C form function first and then try tcl (no javascript support 
 *	yet).
 */
	sp = symLookup(formSymtab, formName);
//fprintf(stderr,"\r\n formName=%s",formName);
#ifdef ASP_SECURITY_PATCH
//	extern	char redirect_url[];
	if (sp == NULL ||
			(!strstr(formName, "formWlanRedirect") &&
				!strstr(formName, "formUpload") &&		
				!strstr(formName, "formRebootCheck") &&	
#ifdef CONFIG_RTK_MESH
				!strstr(formName, "formMeshProxy") &&	
#endif
#ifdef CONFIG_RTL_WAPI_SUPPORT
				!strstr(formName, "formWapiCertManagement") &&
#endif				
				!(is_any_log() && is_valid_goform(formName)))	
//				!(form_hander_in_asp[0] &&
//					strstr(formName, form_hander_in_asp)	))
		) {
			if(is_valid_goform(formName) == 0){
				websTimeoutMsg(wp, 200, T("%s"), "Please refresh web page again from navigation on the left!");
			}else
#else
	if (sp == NULL) {
#endif		
		websError(wp, 200, T("Form %s is not defined"), formName);
	} else {

#ifdef ASP_SECURITY_PATCH
//		form_hander_in_asp[0] = '\0';
	delete_goform(formName);
#endif
	
		fn = (int (*)(void *, char_t *, char_t *)) sp->content.value.integer;
		a_assert(fn);
		if (fn) {
/*
 *			For good practice, forms must call websDone()
 */
			(*fn)((void*) wp, formName, query);

/*
 *			Remove the test to force websDone, since this prevents
 *			the server "push" from a form>
 */
#if 0 /* push */
			if (websValid(wp)) {
				websError(wp, 200, T("Form didn't call websDone"));
			}
#endif /* push */
		}
	}
	return 1;
}

/******************************************************************************/
/*
 *	Define a form function in the "form" map space.
 */

int websFormDefine(char_t *name, void (*fn)(webs_t wp, char_t *path, 
	char_t *query))
{
	a_assert(name && *name);
	a_assert(fn);

	if (fn == NULL) {
		return -1;
	}

	symEnter(formSymtab, name, valueInteger((int) fn), (int) NULL);
	return 0;
}

/******************************************************************************/
/*
 *	Open the symbol table for forms.
 */

void websFormOpen()
{
	formSymtab = symOpen(WEBS_SYM_INIT);
}

/******************************************************************************/
/*
 *	Close the symbol table for forms.
 */

void websFormClose()
{
	if (formSymtab != -1) {
		symClose(formSymtab);
		formSymtab = -1;
	}
}

/******************************************************************************/
/*
 *	Write a webs header. This is a convenience routine to write a common
 *	header for a form back to the browser.
 */

void websHeader(webs_t wp)
{
	a_assert(websValid(wp));

	websWrite(wp, T("HTTP/1.0 200 OK\n"));

/*
 *	By license terms the following line of code must not be modified
 */
	websWrite(wp, T("Server: %s\r\n"), WEBS_NAME);

	websWrite(wp, T("Pragma: no-cache\n"));
	websWrite(wp, T("Cache-control: no-cache\n"));
	websWrite(wp, T("Content-Type: text/html\n"));
	websWrite(wp, T("\n"));
	websWrite(wp, T("<html>\n"));
}

/******************************************************************************/
/*
 *	Write a webs footer
 */

void websFooter(webs_t wp)
{
	a_assert(websValid(wp));

	websWrite(wp, T("</html>\n"));
}

/******************************************************************************/
