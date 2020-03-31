///
///	@file 	matrixSslModule.cpp
/// @brief 	Support for secure sockets via PeerSec Networks MatrixSSL
///
///	This module integrates support for MatrixSSL into AppWeb via a dynamically
///	loaded module. The MatrixSSL library is available at 
/// http://www.matrixssl.org
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
////////////////////////////////// Includes ////////////////////////////////////

#include	"matrixSslModule.h"

#if MPR_FEATURE_MATRIX_SSL && \
	(MPR_FEATURE_STATIC_LINK_MATRIX_SSL || MAKING_MODULES)

//////////////////////////////////// Locals ////////////////////////////////////

static MaMatrixSslModule *matrixSslModule;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////// MaMatrixSslModule ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
//	DLL entrypoint for this module
//
int mprMatrixSslInit(void *handle)
{
	new MaMatrixSslModule(handle);
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
//	Module constructor
//
MaMatrixSslModule::MaMatrixSslModule(void *handle) : 
	MaModule("matrixSsl", handle)
{
	matrixSslModule = this;
	new MaMatrixSslProvider("MatrixSSL");
}

////////////////////////////////////////////////////////////////////////////////
//
//	Module destructor
//
MaMatrixSslModule::~MaMatrixSslModule()
{
}

////////////////////////////////////////////////////////////////////////////////
//
//	Module start is called by AppWeb after modules have been loaded
//	We will be called by the module mechanism and by 
//	MatrixSslConfig::start() below. But we only want to run once.
//
int MaMatrixSslModule::start()
{
	static int	once = 0;
	if (once++ == 0) {
		if (matrixSslOpen() < 0) {
			return -1;
		}
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
//	Stop the module, called before destructor.
//
void MaMatrixSslModule::stop()
{
	static int	once = 0;
	if (once++ == 0) {
		matrixSslClose();
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////// MaMatrixSslProvider /////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
//	Boilerplate from other SSL providers.
//
MaMatrixSslProvider::MaMatrixSslProvider(char *name) : MaSslProvider(name)
{
}

////////////////////////////////////////////////////////////////////////////////

MaMatrixSslProvider::~MaMatrixSslProvider()
{
}

////////////////////////////////////////////////////////////////////////////////

MaSslConfig *MaMatrixSslProvider::newConfig(MaHost *host)
{
	return new MaMatrixSslConfig(host);
}

////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// MaMatrixSslConfig //////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
// An instance of the configuration is created for each SSL server
//
MaMatrixSslConfig::MaMatrixSslConfig(MaHost *host) : MaSslConfig(host)
{
	keys = NULL;
}

////////////////////////////////////////////////////////////////////////////////
//
//	Destructor
//
MaMatrixSslConfig::~MaMatrixSslConfig()
{
	mprAssert(keys == NULL);
}

////////////////////////////////////////////////////////////////////////////////
//
//	Called to accept new connections
//
MprSocket *MaMatrixSslConfig::newSocket()
{
	MaMatrixSslSocket	*socket;

	socket = new MaMatrixSslSocket(this);
	return (MprSocket*) socket;
}

////////////////////////////////////////////////////////////////////////////////
//
//	The configuration file has been parsed, and we act on the configuration
//	items specific to the SSL module.
//	Each virtual server shares the same MatrixSSL context and session cache,
//	but the keys used to provide security are unique to the virtual server.
//	This means that different virtual servers can use its own certificate 
//	and private key file.
//
int MaMatrixSslConfig::start()
{
	char	*password;
	//
	//	We will get called by sslModule::start() before MatrixSslModule::start 
	//	has run -- so we must initialize here.
	//
	matrixSslModule->start();
	//
	//	Read the certificate and the key file for this server
	//	FUTURE - If using encrypted private keys, AppWeb should prompt through
	//	a dialog box or on the console, for the user to enter the password
	//	rather than using NULL as the password here
	//
	password = NULL;
	mprAssert(keys == NULL);
	if (matrixSslReadKeys(&keys, certFile, keyFile, password, NULL) < 0) {
		mprError(MPR_L, MPR_USER, 
			"SSL: Could not read or decode certificate or key file."); 
		return MPR_ERR_CANT_INITIALIZE;
	}
	//
	//	Select the required protocols
	//	MatrixSSL supports only SSLv3.
	//
	if (protocols & MPR_HTTP_PROTO_SSLV2) {
		mprError(MPR_L, MPR_USER, "SSLv2 unsupported"); 
		return MPR_ERR_CANT_INITIALIZE;
	}
	if (!(protocols & MPR_HTTP_PROTO_SSLV3)) {
		mprError(MPR_L, MPR_USER, "SSLv3 not enabled, unable to continue"); 
		return MPR_ERR_CANT_INITIALIZE;
	}
	if (protocols & MPR_HTTP_PROTO_TLSV1) {
		mprLog(3, "Warning, TLSv1 not supported.  Using SSLv3 only.\n");
	}
	//
	//	The Apache format for this SSLCipherSuite directive is quite complex
	//	Since the MatrixSSL ciphersuites are included at compile time,
	//	the configuration options given don't affect the cipher suites
	//	that are supported
	//
	mprLog(4, "SSL: %s: Ignoring requested ciphers %s\n", host->getName(), 
		ciphers);

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
//	Stop this virtual server.  Free the keys.
//
void MaMatrixSslConfig::stop()
{
	if (keys) {
		matrixSslFreeKeys(keys);
		keys = NULL;
	}
}

////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// MaMatrixSslSocket //////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
//	Basic inititalization in the constructor
//
MaMatrixSslSocket::MaMatrixSslSocket(MaMatrixSslConfig *config) : 
	MaSslSocket(config)
{
	ssl = NULL;
	insock.buf = NULL;
	outsock.buf = NULL;
	inbuf.buf = NULL;
	outBufferCount = 0;
}

////////////////////////////////////////////////////////////////////////////////
//
//	Destructor
//
MaMatrixSslSocket::~MaMatrixSslSocket()
{
	mprAssert(ssl == NULL);
}

////////////////////////////////////////////////////////////////////////////////
//
//	Called to accept new connections
//
MprSocket *MaMatrixSslSocket::newSocket()
{
	MaMatrixSslSocket	*socket;

	socket = new MaMatrixSslSocket((MaMatrixSslConfig*) config);
	return (MprSocket*) socket;
}

////////////////////////////////////////////////////////////////////////////////
//
//	Init connection must return -1 on errors. It must also be able to reject
//	subsequent calls to read()/write(). 
//
int MaMatrixSslSocket::initConnection()
{
/*
	If we've already initialized, just return
*/
	if (ssl) {
		return 0;
	}
/*
	Associate a new ssl session with this socket.  The session represents
	the state of the ssl protocol over this socket.  Session caching is
	handled automatically by this api.
*/
 	if (matrixSslNewSession(&ssl,((MaMatrixSslConfig*)config)->keys, NULL,
			SSL_FLAGS_SERVER) < 0) {
		mprAssert(ssl);
		return -1;
	}
/*
	MatrixSSL doesn't provide buffers for data internally.  Define them here to 
	support buffered reading and writing for non-blocking sockets.
	Although it causes quite a bit more work, we support dynamically growing
	the buffers as needed.  Alternately, we could define 16K buffers here and
	not worry about growing them.
*/
	insock.size = 1024;
	insock.start = insock.end = insock.buf = 
		(unsigned char *)mprMalloc(insock.size);
	outsock.size = 1024;
	outsock.start = outsock.end = outsock.buf = 
		(unsigned char *)mprMalloc(outsock.size);
	inbuf.size = 0;
	inbuf.start = inbuf.end = inbuf.buf = NULL;
	outBufferCount = 0;

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
//	Free up the elements we allocated in initConnection
//
bool MaMatrixSslSocket::dispose()
{
	if (ssl) {
		matrixSslDeleteSession(ssl);
		ssl = NULL;
		mprFree(insock.buf);
		mprFree(outsock.buf);
		if (inbuf.buf) {
			mprFree(inbuf.buf);
		}
	}
	return this->MprSocket::dispose();
}

////////////////////////////////////////////////////////////////////////////////
//
//	Append a closure alert to any buffered output data, and try to send it.
//	Don't bother retrying or blocking, we're just closing anyway.
//
void MaMatrixSslSocket::close(int how)
{
	if (!(flags & MPR_SOCKET_EOF)) {
		matrixSslEncodeClosureAlert(ssl, &outsock);
		if (outsock.start < outsock.end) {
			this->MprSocket::write((char *)outsock.start, 
				outsock.end - outsock.start);
		}
	}
	this->MprSocket::close(how);
}

////////////////////////////////////////////////////////////////////////////////
//
//	Encode caller's data buffer into an SSL record and write to socket.
//	The encoded data will always be bigger than the incoming data because
//	of the record header (5 bytes) and MAC (16 bytes MD5 / 20 bytes SHA1)
//	This would be fine if we were using blocking sockets, but non-blocking
//	presents an interesting problem.  Example:
//		A 100 byte input record is encoded to a 125 byte SSL record
//		We can send 124 bytes without blocking, leaving one buffered byte
//		We can't return 124 to the caller because it's more than they requested
//		We can't return 100 to the caller because they would assume all data
//		has been written, and we wouldn't get re-called to send the last byte
//	We handle the above case by returning 0 to the caller if the entire
//	encoded record could not be sent.  Returning 0 will prompt AppWeb to
//	select this socket for write events, and we'll be called again when the
//	socket is writable.  We'll use this mechanism to flush the remaining 
//	encoded data, ignoring the bytes sent in, as they have already been
//	encoded.  When it is completely flushed, we return the originally 
//	requested length, and resume normal processing.
//
int	MaMatrixSslSocket::write(char *buf, int len)
{
	int		rc;
/*
	Pack the buffered socket data (if any) so that start is at zero.
*/
	if (outsock.buf < outsock.start) {
		if (outsock.start == outsock.end) {
			outsock.start = outsock.end = outsock.buf;
		} else {
			memmove(outsock.buf, outsock.start, outsock.end - outsock.start);
			outsock.end -= (outsock.start - outsock.buf);
			outsock.start = outsock.buf;
		}
	}
/*
	If there is buffered output data, the caller must be trying to
	send the same amount of data as last time.  We don't support 
	sending additional data until the original buffered request has
	been completely sent.
*/
	if (outBufferCount > 0 && len != outBufferCount) {
		mprAssert(len != outBufferCount);
		return -1;
	}
/*
	If we don't have buffered data, encode the caller's data
*/
	if (outBufferCount == 0) {
retryEncode:
		rc = matrixSslEncode(ssl, (unsigned char *)buf, len, &outsock);
		switch (rc) {
		case SSL_ERROR:
			return -1;
		case SSL_FULL:
			if (outsock.size > SSL_MAX_RECORD_SIZE) {
				return -1;
			}
			outsock.size *= 2;
			outsock.buf = 
				(unsigned char *)mprRealloc(outsock.buf, outsock.size);
			outsock.end = outsock.buf + (outsock.end - outsock.start);
			outsock.start = outsock.buf;
			goto retryEncode;
		}
	}
/*
	We've got data to send.  Try to write it all out.
*/
	rc = this->MprSocket::write((char *)outsock.start, 
		outsock.end - outsock.start);
	if (rc <= 0) {
		return rc;
	}
	outsock.start += rc;
/*
	If we wrote it all return the length, otherwise remember the number of
	bytes passed in, and return 0 to be called again later.
*/
	if (outsock.start == outsock.end) {
		outBufferCount = 0;
		return len;
	}
	outBufferCount = len;
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
//	FUTURE - This is currently not called by AppWeb
//
int MaMatrixSslSocket::flush()
{
	mprAssert(0);
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
//	Read and decode SSL record data.
//	The various return codes of matrixSslDecode() are relatively simple to
//	handle on their own, but this code is complicated by the use of:
//		non-blocking sockets
//		fixed caller buffer size
//		use of caller's buffer as direct output unless a larger buffer is reqd.
//		record oriented protocol of SSL being accessed in a streamlike fashion
//		use of buffers that can grow to fit larger records.
//
//	Here's a scenario that illustrates some of the difficulties handled:
//	1	Caller passes in a buffer of length 1024 bytes
//	2	Insock buffer is also 1024 bytes initially
//	3	Socket read returns 1024 bytes, filling entire insock buffer
//	4	Decode returns SSL_PARTIAL - the record is bigger than 1K.
//	5	insock is doubled to 2K, socket is read again.
//	6	Another 1024K is read from the socket (2K total in insock)
//	7	Decode returns SSL_FULL - the caller's buffer is too small for record
//	8	Allocate inbuf to twice the length of caller's buffer, decode again.
//	9	Decode returns SSL_PROCESS_DATA
//	10	Length of decoded data (in inbuf) is 1.5K
//	11	Copy 1K of data to caller's buffer
//	12	Return 1K as length read
//
//	This scenario has a few side effects.  The insock buffer still contains
//	512 bytes of encrypted data, inbuf still contains 512 bytes of decrypted
//	data.  So we have two layers of buffering.  To complicate matters, if 
//	there is no more data in the socket itself, we will not get select
//	read events to prompt AppWeb to call read again to get our buffered data.
//
//	Two ways to handle this are to have a isBufferedData() method called
//	by the select layer to determine that there is more data available.
//	AppWeb does not currently support this method.
//
//	The other option for AppWeb is to continue calling read() until
//	0 bytes are returned, or an error (< 0) is returned.  We rely on this
//	method to flush both of the incoming buffers.
//
int	MaMatrixSslSocket::read(char *b, int len)
{
	int				bytes, rc;
	unsigned char	*buf, error, alertLevel, alertDescription;

	if (ssl == NULL) {
		return -1;
	}
	buf = (unsigned char *)b;
/*
	If inbuf is valid, then we have previously decoded data that must be
	returned, return as much as possible.  Once all buffered data is
	returned, free the inbuf.
*/
	if (inbuf.buf) {
		if (inbuf.start < inbuf.end) {
			bytes = min(len, inbuf.end - inbuf.start);
			memcpy(buf, inbuf.start, bytes);
			inbuf.start += bytes;
			return len;
		}
		mprFree(inbuf.buf);
		inbuf.buf = NULL;
	}
/*
	Pack the buffered socket data (if any) so that start is at zero.
*/
	if (insock.buf < insock.start) {
		if (insock.start == insock.end) {
			insock.start = insock.end = insock.buf;
		} else {
			memmove(insock.buf, insock.start, insock.end - insock.start);
			insock.end -= (insock.start - insock.buf);
			insock.start = insock.buf;
		}
	}
/*
	Read up to as many bytes as there are remaining in the buffer.  We could
	Have encrypted data already cached in insock, but might as well read more
	if we can.
*/
	if (insock.end - insock.start < insock.size) {
		bytes = this->MprSocket::read((char *)insock.end, 
			(insock.buf + insock.size) - insock.end);
		if (bytes <= 0) {
			return bytes;
		}
		insock.end += bytes;
	}
/*
	Define a temporary sslBuf to point into the user's passed in buffer
	This means we decode the record directly into the callers buffer
	If the caller's buffer isn't big enough, we end up using a temp buffer.
	This causes some complications with freeing memory properly on return,
	but gives a performance boost in the most common case.
*/
	inbuf.start = inbuf.end = inbuf.buf = (unsigned char *)buf;
	inbuf.size = len;
/*
	Decode the data we just read from the socket
*/
decodeMore:
	rc = matrixSslDecode(ssl, &insock, &inbuf, &error, &alertLevel, 
		&alertDescription);
	switch (rc) {
/*
	Parsed a handshake record, just go back and try to decode more
*/
	case SSL_SUCCESS:
		goto decodeMore;
/*
	Successfully decoded an application data record, and placed in tmp buf
*/
	case SSL_PROCESS_DATA:
/*
		If the tmp buf is still the caller's buffer, then all the data fit in
		the caller's buffer and we didn't have to write any of the decoded 
		data in inbuf.
*/
		if (inbuf.buf == buf) {
			inbuf.buf = NULL;
			return inbuf.end - inbuf.start;
		}
/*
		Copy as much as we can from the temp buffer into the caller's buffer
		and leave the remainder in inbuf until the next call to read
*/
		mprAssert(len < inbuf.end - inbuf.start);
		memcpy(buf, inbuf.start, len);
		inbuf.start += len;
		return len;
/*
	We've decoded a record that requires a response into tmp
	If there is no data to be flushed in the out buffer, we can write out
	the contents of the tmp buffer.  Otherwise, we need to append the data 
	to the outgoing data buffer and flush it out.
*/
	case SSL_SEND_RESPONSE:
		bytes = this->MprSocket::write((char *)inbuf.start, 
			inbuf.end - inbuf.start);
		inbuf.start += bytes;
		if (inbuf.start < inbuf.end) {
			this->MprSocket::setBlockingMode(true);
			while (inbuf.start < inbuf.end) {
				bytes = this->MprSocket::write((char *)inbuf.start, 
					inbuf.end - inbuf.start);
				if (bytes < 0) {
					goto readError;
				}
				inbuf.start += bytes;
			}
			this->MprSocket::setBlockingMode(false);
		}
		inbuf.start = inbuf.end = inbuf.buf;
		goto decodeMore;
/*
	There was an error decoding the data, or encoding the out buffer.
	There may be a response data in the out buffer, so try to send.
	We try a single hail-mary send of the data, and then close the socket.
	Since we're closing on error, we don't worry too much about a clean flush.
*/
	case SSL_ERROR:
		mprLog(4, "SSL: Closing on protocol error %d\n", error);
		if (inbuf.start < inbuf.end) {
			bytes = this->MprSocket::write((char *)inbuf.start, 
				inbuf.end - inbuf.start);
		}
		goto readError;
/*
	We've decoded an alert.  The level and description passed into
	matrixSslDecode are filled in with the specifics.
*/
	case SSL_ALERT:
		if (alertDescription == SSL_ALERT_CLOSE_NOTIFY) {
			goto readZero;
		}
		mprLog(4, "SSL: Closing on client alert %d: %d\n",
			alertLevel, alertDescription);
		goto readError;
/*
	We have a partial record, we need to read more data off the socket.
	If we have a completely full insock buffer, we'll need to grow it
	here so that we CAN read more data when called the next time.
*/
	case SSL_PARTIAL:
		if (insock.start == insock.buf && insock.end == 
				(insock.buf + insock.size)) {
			if (insock.size > SSL_MAX_RECORD_SIZE) {
				goto readError;
			}
			insock.size *= 2;
			insock.start = insock.buf = 
				(unsigned char *)mprRealloc(insock.buf, insock.size);
			insock.end = insock.buf + (insock.size / 2);
		}
		goto readZero;
/*
	The out buffer is too small to fit the decoded or response
	data.  Increase the size of the buffer and call decode again
*/
	case SSL_FULL:
		mprAssert(inbuf.start == inbuf.end);
		inbuf.size *= 2;
		if (inbuf.buf != buf) {
			mprFree(inbuf.buf);
		}
		inbuf.start = inbuf.end = inbuf.buf = 
			(unsigned char *)mprMalloc(inbuf.size);
		goto decodeMore;
	}
/*
	We consolidated some of the returns here because we must ensure
	that inbuf is cleared if pointing at caller's buffer, otherwise
	it will be freed later on.
*/
readZero:
	if (inbuf.buf == buf) {
		inbuf.buf = NULL;
	}
	return 0;
readError:
	flags |= MPR_SOCKET_EOF;
	if (inbuf.buf == buf) {
		inbuf.buf = NULL;
	}
	return -1;
}

////////////////////////////////////////////////////////////////////////////////
#else
void mprMatrixSslModuleDummy() {}

#endif // MPR_FEATURE_MATRIX_SSL && (MPR_FEATURE_STATIC_LINK_MATRIX_SSL ...

//
// Local variables:
// tab-width: 4
// c-basic-offset: 4
// End:
// vim:tw=78
// vim600: sw=4 ts=4 fdm=marker
// vim<600: sw=4 ts=4
//

