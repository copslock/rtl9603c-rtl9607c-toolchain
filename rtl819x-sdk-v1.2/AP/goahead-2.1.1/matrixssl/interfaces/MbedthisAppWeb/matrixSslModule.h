///
///	@file 	matrixSslModule.h
/// @brief 	Header for the Matrix SSL module
///
///	This module integrates support for MatrixSSL into AppWeb via a dynamically
///	loaded module.  The MatrixSSL library, including the latest version of
///	this file is available as source code at http://www.matrixssl.org
///
////////////////////////////////////////////////////////////////////////////////
//
//	Copyright (c) PeerSec Networks LLC, 2002-2004. All Rights Reserved.
//	Copyright (c) Mbedthis Software LLC, 2003-2004. All Rights Reserved.
//	The latest version of this code is available at http://www.matrixssl.org
//
//	This software is open source; you can redistribute it and/or modify it 
//	under the terms of the GNU General Public License as published by the 
//	Free Software Foundation; either version 2 of the License, or (at your 
//	option) any later version.
//
//	This program is distributed WITHOUT ANY WARRANTY; without even the 
//	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
//	See the GNU General Public License for more details at:
//	http://www.mbedthis.com/downloads/gplLicense.html
//	
//	This General Public License does NOT permit incorporating this software 
//	into proprietary programs. If you are unable to comply with the GPL, a 
//	commercial license for this software and support services are available
//	from Mbedthis Software at http://www.mbedthis.com
//
 
#ifndef _h_MATRIX_SSL_MODULE
#define _h_MATRIX_SSL_MODULE 1

////////////////////////////////// Includes ////////////////////////////////////

#include	"http/http.h"

#if MPR_FEATURE_MATRIX_SSL
#include	"http/modules/sslModule.h"
#include	<matrixSsl/matrixSsl.h>

////////////////////////////// Forward Definitions /////////////////////////////

class MatrixSslModule;

extern "C" {
	extern int mprMatrixSslInit(void *handle);
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////// MaMatrixSslModule ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class MaMatrixSslModule : public MaModule {
  private:
  public:
					MaMatrixSslModule(void *handle);
					~MaMatrixSslModule();
	int				start();
	void			stop();
};

////////////////////////////////////////////////////////////////////////////////

class MaMatrixSslProvider : public MaSslProvider {
  private:
	MprStr			name;
  public:
					MaMatrixSslProvider(char *name);
					~MaMatrixSslProvider();
	MaSslConfig		*newConfig(MaHost *host);
};

////////////////////////////////////////////////////////////////////////////////
//
//	Per host SSL configuration information
//

class MaMatrixSslConfig : public MaSslConfig {
  public:
	sslKeys_t		*keys;

  public:
					MaMatrixSslConfig(MaHost *host);
					~MaMatrixSslConfig();

	MprSocket 		*newSocket();
	int				start();
	void			stop();
};

////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// MaMatrixSslSocket //////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class MaMatrixSslSocket : public MaSslSocket {
  private:
	ssl_t			*ssl;
	sslBuf_t		insock;	//Cached ciphertext from socket
	sslBuf_t		inbuf;	//Cached (decoded) plaintext
	sslBuf_t		outsock;//Cached ciphertext to socket
	int				outBufferCount;	//Count of outgoing data we've buffered

  public:
					MaMatrixSslSocket(MaMatrixSslConfig *config);
					~MaMatrixSslSocket();
	void			close(int how);
	bool			dispose();
	int				flush();
	int				initConnection();
	MprSocket		*newSocket();
	int				read(char *buf, int bufsize);
	int				write(char *buf, int bufsize);
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#else

void mprMatrixSslDummy();

#endif // MPR_FEATURE_MATRIX_SSL
#endif // _h_MATRIX_SSL_MODULE

//
// Local variables:
// tab-width: 4
// c-basic-offset: 4
// End:
// vim:tw=78
// vim600: sw=4 ts=4 fdm=marker
// vim<600: sw=4 ts=4
//
