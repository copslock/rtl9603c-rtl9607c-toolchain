/*
 *	socketLayer.h
 *	Release $Name:  $
 *
 *	Sample SSL socket layer header for MatrixSSL
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

#ifndef _h_SSLSOCKET
#define _h_SSLSOCKET

#ifdef __cplusplus
extern "C" {
#endif

#include "matrixssl/matrixSsl.h"

/*
	OS specific macros
*/
#if WIN32
	#include	<windows.h>
	#define		fcntl(A, B, C)
	#define		MSG_NOSIGNAL	0
	#define		WOULD_BLOCK	WSAEWOULDBLOCK
	#define		getSocketError()  WSAGetLastError()
#elif LINUX
	#include	<netinet/in.h>
	#include	<netinet/tcp.h>
	#include	<fcntl.h>
	#include	<errno.h>
	#define		SOCKET_ERROR -1
	#define		getSocketError() errno 
	#define		WOULD_BLOCK	EAGAIN
	#define		closesocket close
	#define		MAKEWORD(A, B)
	#define		WSAStartup(A, B)
	#define		WSACleanup()
	#define		INVALID_SOCKET -1
	typedef int	WSADATA;
	typedef int	SOCKET;
#endif /* OS macros */

extern void breakpoint();
#define sslAssert(C) if (C) ; else {printf("%s:%d sslAssert(%s)\n",\
						__FILE__, __LINE__, #C); breakpoint(); }
#ifndef min
#define min(a,b)	(((a) < (b)) ? (a) : (b))
#endif /* min */

/*
	sslRead and sslWrite status values
*/
#define SSLSOCKET_EOF			0x1
#define SSLSOCKET_CLOSE_NOTIFY	0x2

/*
	Connection structure
*/
typedef struct {
	ssl_t				*ssl;
	sslBuf_t			inbuf;
	sslBuf_t			insock;
	sslBuf_t			outsock;
	int					outBufferCount;
	SOCKET				fd;
} sslConn_t;

/*
	Secure Socket apis
*/
extern int	sslConnect(sslConn_t **cp, SOCKET fd, sslKeys_t *keys, sslSessionId_t *id, 
					   short cipherSuite, int (*certValidator)(sslCertInfo_t *t));
extern int	sslAccept(sslConn_t **cp, SOCKET fd, sslKeys_t *keys);
extern void	sslFreeConnection(sslConn_t **cp);

extern int	sslRead(sslConn_t *cp, char *buf, int len, int *status);
extern int	sslWrite(sslConn_t *cp, char *buf, int len, int *status);
extern void	sslWriteClosureAlert(sslConn_t *cp);

/*
	Socket apis
*/
extern SOCKET	socketListen(short port, int *err);
extern SOCKET	socketAccept(SOCKET listenfd, int *err);
extern SOCKET	socketConnect(char *ip, short port, int *err);
extern void		socketShutdown(SOCKET sock);

extern void setSocketBlock(SOCKET sock);
extern void setSocketNonblock(SOCKET sock);
extern void setSocketNodelay(SOCKET sock);

#ifdef __cplusplus
}
#endif

#endif /* _h_SSLSOCKET */

/******************************************************************************/
