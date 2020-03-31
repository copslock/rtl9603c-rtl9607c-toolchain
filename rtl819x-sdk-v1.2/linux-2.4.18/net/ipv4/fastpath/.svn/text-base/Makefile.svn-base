#
# Makefile for the fastpath modules on top of IPv4.
#

O_TARGET := fastpath.o

ifdef CONFIG_RTL_KERNEL_MIPS16_FASTPATH
	obj-y += fastpath_core.o16 filter.o16 fast_l2tp_core.o16
	
ifeq ($(CONFIG_RTL8196C),y)	
	obj-y += fast_pptp_core.o
else
	obj-y += fast_pptp_core.o16
endif
	
else
obj-y += fastpath_core.o filter.o  fast_pptp_core.o fast_l2tp_core.o
endif

EXTRA_CFLAGS += -Wall -O2

include $(TOPDIR)/Rules.make
