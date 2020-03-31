# Generated automatically from Makefile.in by configure.
#***********************************************************************
#
# Makefile
#
# Makefile for Roaring Penguin's Linux PPPoE plugin.
#
# Copyright (C) 2001 Roaring Penguin Software Inc.
#
# This program may be distributed according to the terms of the GNU
# General Public License, version 2 or (at your option) any later version.
#
# $Id: Makefile,v 1.7 2010/03/10 05:30:09 keith_huang Exp $
#***********************************************************************

# Version is set ONLY IN THE MAKEFILE!  Don't delete this!
VERSION=3.3

CFLAGS+=-I../../../include/linux --static -s -DCOMPACK_SIZE
ifeq ($(RTL8186_KB),1)
CFLAGS+=-DCONFIG_RTL8186_KB
endif

ifeq ($(RTL8186_TR),1)
CFLAGS+=-DCONFIG_RTL8186_TR 
endif

ifeq ($(RTL8196C_EC),1)
CFLAGS+=-DCONFIG_RTL8196C_EC
endif

ifeq ($(RTL865X_AC),1)
CFLAGS+=-DCONFIG_RTL865X_AC 
endif

ifeq ($(RTL865X_KLD),1)
CFLAGS+=-DCONFIG_RTL865X_KLD
endif

ifeq ($(CONFIG_RTL865X_SC),1)
CFLAGS+=-DCONFIG_RTL865X_SC
endif


all: libplugin.a

rp-pppoe.so: libplugin.a plugin.o
	gcc -o rp-pppoe.so -shared plugin.o libplugin.a

install: all
	$(INSTALL) -d -m 755 $(LIBDIR)
	$(INSTALL) -s -c -m 4550 rp-pppoe.so $(LIBDIR)

clean:
	rm -f *.o *.so

plugin.o: plugin.c
	mips-uclibc-gcc '-DRP_VERSION="$(VERSION)"' $(CFLAGS) -I../../..  -c -o plugin.o  plugin.c

libplugin.a: discovery.o if.o common.o debug.o plugin.o
	mips-uclibc-ar -rc $@ $^

discovery.o: discovery.c
	mips-uclibc-gcc $(CFLAGS) '-DVERSION="$(VERSION)"' -c -o discovery.o  discovery.c

if.o: if.c
	mips-uclibc-gcc $(CFLAGS) '-DVERSION="$(VERSION)"' -c -o if.o  if.c

debug.o: debug.c
	mips-uclibc-gcc $(CFLAGS) '-DVERSION="$(VERSION)"' -c -o debug.o  debug.c

common.o: common.c
	mips-uclibc-gcc $(CFLAGS) '-DVERSION="$(VERSION)"' -c -o common.o  common.c

