#
#	MatrixSSL example applications makefile
#	Builds optimized by default, see comments below for debug.
#	Copyright (c) PeerSec Networks, 2002-2004. All Rights Reserved
#
CC 	= mips-uclibc-gcc
O 	= .o
SO  = .a
E	=

#
#   Debug flags and C flags
#
DFLAGS  = -O3
#
# Uncomment the DFLAGS line below to build the debug version of the
# example application
#
#DFLAGS = -g -DDEBUG
CFLAGS  = -DLINUX $(DFLAGS)
LDFLAGS = -Wl -lc -lpthread

all: compile

REF_EXE 	= httpsReflector$(E)
REF_OBJS 	= httpsReflector$(O) 

CLIENT_EXE	= httpsClient$(E)
CLIENT_OBJS = httpsClient$(O) 

LIBRARIES	= ../src/libmatrixssl$(SO)

compile: $(REF_OBJS) $(REF_EXE) $(CLIENT_EXE) $(CLIENT_OBJS)

#
# Manual dependencies
#
*.o: ../matrixSsl.h $(LIBRARIES) Makefile

$(REF_EXE):$(REF_OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBRARIES)

$(CLIENT_EXE):$(CLIENT_OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBRARIES)
#
#	Clean up workspace
#
clean:
	rm -f $(REF_EXE) $(REF_OBJS) $(CLIENT_EXE) $(CLIENT_OBJS)
