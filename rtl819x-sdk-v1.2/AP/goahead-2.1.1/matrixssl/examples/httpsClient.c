/*
 *	httpClient.c
 *	Release $Name:  $
 *
 *	Simple example program for MatrixSSL
 *	Sends a HTTPS request and echos the response back to the sender.
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

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

/******************************************************************************/

#include "sslSocket.h"

/******************************************************************************/

#define HTTPS_PORT	4433
#define HTTPS_IP	"127.0.0.1"

static char CAfile[] = "CAcert.pem";

#define ITERATIONS	100 /* How many individual connections to make */
#define REQUESTS	1  /* How many requests per each connection */

static const char request[] = "GET / HTTP/1.1\r\n"
		"User-Agent: MatrixSSL httpClient\r\n"
		"Accept: */*\r\n"
		"\r\n";

static const char requestAgain[] = "GET /again HTTP/1.1\r\n"
		"User-Agent: MatrixSSL httpClient\r\n"
		"Accept: */*\r\n"
		"\r\n";

static const char quitString[] = "GET /quit";

/*
	Callback that is registered to receive server certificate 
	information for custom validation
*/
static int certChecker(sslCertInfo_t *cert);

/******************************************************************************/
/*
	Example ssl client that connects to a server and sends https messages
*/
int main(int argc, char **argv)
{
	sslSessionId_t		*sessionId;
	sslConn_t			*conn;
	sslKeys_t			*keys;
	WSADATA				wsaData;
	SOCKET				fd;
	short				cipherSuite;
	unsigned char		*ip, *c, *requestBuf;
	unsigned char		buf[1024];
	int					iterations, requests, connectAgain, status;
	int					quit, rc, bytes, i, j, err;
	time_t				t0, t1;

	conn = NULL;
/*
	First (optional) argument is ip address to connect to (port is hardcoded)
	Second (optional) argument is number of iterations to perform
	Third (optional) argument is number of keepalive HTTP requests
	Fourth (optional) argument is cipher suite number to use (0 for any)
*/
	ip = HTTPS_IP;
	iterations = ITERATIONS;
	requests = REQUESTS;
	cipherSuite = 0x0000;
	if (argc > 1) {
		ip = argv[1];
		if (argc > 2) {
			iterations = atoi(argv[2]);
			sslAssert(iterations > 0);
			if (argc > 3) {
				requests = atoi(argv[3]);
				sslAssert(requests > 0);
				if (argc > 4) {
					cipherSuite = (short)atoi(argv[4]);
				}
			}
		}
	}
/*
	Initialize Windows sockets (no-op on other platforms)
*/
	WSAStartup(MAKEWORD(1,1), &wsaData);
/*
	Initialize the MatrixSSL Library, and read in the certificate file
	used to validate the server.
*/
	matrixSslOpen();
	sessionId = NULL;
	if (matrixSslReadKeys(&keys, NULL, NULL, NULL, CAfile) < 0) {
		goto promptAndExit;
	}
/*
	Intialize loop control variables
*/
	quit = 0;
	connectAgain = 1;
	i = 1;
/*
	Just reuse the requestBuf and malloc to largest possible message size
*/
	requestBuf = malloc(sizeof(requestAgain));
	t0 = time(0);
/*
	Main ITERATIONS loop
*/
	while (!quit && (i < iterations)) {
/*
		sslConnect uses port and ip address to connect to SSL server.
		Generates a new session
*/
		if (connectAgain) {
			if ((fd = socketConnect(ip, HTTPS_PORT, &err)) == INVALID_SOCKET) {
				fprintf(stdout, "Error connecting to server %s:%d\n", ip, HTTPS_PORT);
				goto promptAndExit;
			}
			if (sslConnect(&conn, fd, keys, sessionId, cipherSuite, certChecker) < 0) {
				quit = 1;
				socketShutdown(fd);
				fprintf(stderr, "Error connecting to %s:%d\n", ip, HTTPS_PORT);
				continue;
			}
			i++;
			connectAgain = 0;
			j = 1;
		}
/*
		Copy the HTTP request header into the buffer, based of whether or
		not we want httpReflector to keep the socket open or not
*/
		if (j == requests) {
			bytes = (int)strlen(request);
			memcpy(requestBuf, request, bytes);
		} else {
			bytes = (int)strlen(requestAgain);
			memcpy(requestBuf, requestAgain, bytes);
		}
/*
		Send request.  
		< 0 return indicates an error.
		0 return indicates not all data was sent and we must retry
		> 0 indicates that all requested bytes were sent
*/
writeMore:
		rc = sslWrite(conn, requestBuf, bytes, &status);
		if (rc < 0) {
			fprintf(stdout, "Internal sslWrite error\n");
			socketShutdown(conn->fd);
			sslFreeConnection(&conn);
			continue;
		} else if (rc == 0) {
			goto writeMore;
		}
/*
		Read response
		< 0 return indicates an error.
		0 return indicates an EOF or CLOSE_NOTIFY in this situation
		> 0 indicates that some bytes were read.  Keep reading until we see
		the /r/n/r/n from the response header.  There may be data following
		this header, but we don't try too hard to read it for this example.
*/
		c = buf;
readMore:
		if ((rc = sslRead(conn, c, sizeof(buf) - (int)(c - buf), &status)) > 0) {
			c += rc;
			if (c - buf < 4 || memcmp(c - 4, "\r\n\r\n", 4) != 0) {
				goto readMore;
			}
		} else if (rc < 0) {
			fprintf(stdout, "sslRead error.  dropping connection.\n");
			socketShutdown(conn->fd);
			sslFreeConnection(&conn);
			continue;
		}
/*
		Determine if we want to do a pipelined HTTP request/response
*/
		if (j++ < requests) {
			fprintf(stdout, "R");
			continue;
		} else {
			fprintf(stdout, "C");
		}
/*
		Reuse the session.  Comment out these two lines to test the entire
		public key renegotiation each iteration
*/
		matrixSslFreeSessionId(sessionId);
		matrixSslGetSessionId(conn->ssl, &sessionId);
/*
		Send a closure alert for clean shutdown of remote SSL connection
		This is for good form, some implementations just close the socket
*/
		sslWriteClosureAlert(conn);
/*
		Session done.  Connect again if more iterations remaining
*/
		socketShutdown(conn->fd);
		sslFreeConnection(&conn);
		connectAgain = 1;
	}

	t1 = time(0);
	free(requestBuf);
	matrixSslFreeSessionId(sessionId);
	if (conn && conn->ssl) {
		socketShutdown(conn->fd);
		sslFreeConnection(&conn);
	}
	fprintf(stdout, "\n%d connections in %d seconds (%f c/s)\n", 
		i, t1 - t0, (double)i / (t1 - t0));
	fprintf(stdout, "\n%d requests in %d seconds (%f r/s)\n", 
		i * requests, t1 - t0, 
		(double)(i * requests) / (t1 - t0));
/*
	Close listening socket, free remaining items
*/
	matrixSslFreeKeys(keys);
	matrixSslClose();
	WSACleanup();
promptAndExit:
	fprintf(stdout, "Press return to exit...\n");
	getchar();

	return 0;
}

/******************************************************************************/
/*
	Stub for a user-level certificate validator.  Just using
	the default validation value here.
*/
static int certChecker(sslCertInfo_t *cert)
{
	return cert->verified;
}	

/******************************************************************************/



