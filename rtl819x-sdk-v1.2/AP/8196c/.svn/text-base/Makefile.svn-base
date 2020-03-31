MAKE = make
CROSS_COMPILE=mips-uclibc-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
AR = $(CROSS_COMPILE)ar
include ../../linux-2.4.18/.config

ifeq ($(USE_RSDK_1_2_11),1)
export PATH
PATH:=/toolchain/rsdk-1.2.11/linux:$(PATH)
else
ifdef CONFIG_RTL8198
export PATH
PATH:= /toolchain/rsdk-96c/linux:$(PATH)
USE_TOOLCHAIN_98=1
export USE_TOOLCHAIN_98
else
export PATH
PATH:= /toolchain/rsdk-96c/linux:$(PATH)
endif
endif
USE_RSDK_UCLIBC_0930=1
#CONFIG_BUSYBOX101=1
ifdef CONFIG_RTL_WAPI_SUPPORT
##To test openssl
USE_OPENSSL=1
USE_WAPI=1
ifdef CONFIG_RTL_WAPI_LOCAL_AS_SUPPORT
CONFIG_RTL_WAPI_LOCAL_AS=1
endif
endif

RTL819X=RTL8196B

ifeq ($(CONFIG_RTL8196C),y)
RTL819X=RTL8196C
endif

ifeq ($(CONFIG_RTL8198),y)
RTL819X=RTL8198
endif




ifeq ($(CONFIG_NET_RADIO),y)
INCLUDE_WLAN=1
else
INCLUDE_WLAN=0
endif
ifeq ($(MOD),1)
BUSYBOX=busybox-1.1.3
else
ifeq ($(BUSYBOX_101),1)
BUSYBOX=busybox-1.01
else
BUSYBOX=busybox-1.00-pre8
endif
endif

ifneq ($(CONFIG_RTL_COMAPI_WLTOOLS),n)
ifneq ($(CONFIG_RTL_COMAPI_WLTOOLS),)
        WLTOOLS=wireless_tools.29
else
        WLTOOLS=wireless_tools.25
endif
endif

ifeq ($(CONFIG_RTL8196B_GW_8M),y)
WEB_PAGE=
else
WEB_PAGE=../goahead-2.1.1/LINUX/webpages-gw.bin
endif

ifeq ($(MP-AP),1)

else
ifeq ($(MP-GW),1)
	SUBDIRS = ../bridge-utils ../busybox-1.00-pre8 ../goahead-2.1.1/LINUX\
	../$(WLTOOLS) ../mp-daemon
	DBG=MP=1
else
	SUBDIRS = ../bridge-utils ../$(BUSYBOX) ../goahead-2.1.1/LINUX ../udhcp-0.9.9-pre \
	../$(WLTOOLS) ../auth/src ../auth/src/dlisten ../IAPP ../rc
	ifeq ($(AP),1)
		DBG=AP=1
		ifeq ($(CONFIG_DOMAIN_NAME_QUERY_SUPPORT),y)
			SUBDIRS += ../dnrd-2.12.1/src
		endif
	else
		ifeq ($(GW),1)
		DBG=GW=1
		else
		DBG=GW_VPN=1
		endif
ifeq ($(CONFIG_RTL8196C_CLIENT_ONLY),y)		
		SUBDIRS += ../ppp-2.4.2/pppd/plugins/rp-pppoe ../ppp-2.4.2/pppd ../dnrd-2.12.1/src ../updatedd-2.5
else		
		SUBDIRS += ../iptables-1.2.6a ../ppp-2.4.2/pppd/plugins/rp-pppoe ../ppp-2.4.2/pppd ../pptp-linux-1.3.1 ../ntpclient ../dnrd-2.12.1/src ../updatedd-2.5 ../igmpproxy ../routed
endif
	endif
endif
endif

ifeq ($(USE_OPENSSL),1)
	USE_4M=1
	SUBDIRS += ../openssl-0.9.8b
ifeq ($(USE_WAPI),1)
	SUBDIRS += ../wapi_utils/udp_sock
	SUBDIRS += ../wapi_utils/rwWapiFiles
	SUBDIRS += ../wapi_utils/other_tools
endif
endif

ifeq ($(GW),1)
SUBDIRS += ../iproute2
endif

ifeq ($(GW),1)
SUBDIRS += ../l2tpd
endif

ifeq ($(DEBUG),1)
DBG+=DEBUG=1
endif

ifeq ($(MOD),1)
BUSYBOX_DFGCONFIG=../$(BUSYBOX)/rtl8186/nm_config_module
else
ifeq ($(BUSYBOX_101),1)
BUSYBOX_DFGCONFIG=../$(BUSYBOX)/rtl8186/nm_config
else
ifeq ($(GW),1)
BUSYBOX_DFGCONFIG=../busybox-1.00-pre8/rtl8186/nm_config
endif
ifeq ($(AP),1)
BUSYBOX_DFGCONFIG=../busybox-1.00-pre8/rtl8186/nm_config_ap
endif
endif
endif

#
# Extra Feature Configuration
#
ifeq ($(CONFIG_SNMP),1)
EXTRA_CONFIG_CFLAGS += -DCONFIG_SNMP
APMIB_SHARED = 1
endif
export EXTRA_CONFIG_CFLAGS
export APMIB_SHARED
#for mesh
export MESH_EXTRA_CONFIG_FLAGS

#for mesh

# 11s extra
ifeq ($(CONFIG_MESH_ENABLE),y)
	SUBDIRS += ../11s-PathSelection 
	#MESH_EXTRA_CONFIG_FLAGS += -DCONFIG_RTK_MESH -D_MESH_ACL_ENABLE_ -DSNMP_SUPPORT
	MESH_EXTRA_CONFIG_FLAGS += -DCONFIG_RTK_MESH -D_MESH_ACL_ENABLE_  -DCONFIG_NEW_MESH_UI
	ifeq ($(CONFIG_TEST_PKG),y)
		SUBDIRS += ../test_traffic netperf-2.4.4 		
		TESTPKG=1
	endif
	ifeq ($(CONFIG_11S_TEST_MODE),y)
			MESH_EXTRA_CONFIG_FLAGS += -D_11s_TEST_MODE_
	endif
	ifeq ($(CONFIG_NMS),y)
		MESH_EXTRA_CONFIG_FLAGS += -DSNMP_SUPPORT
		SUBDIRS += ../net-snmp-5.2.3 nmsd burn_image 
		NMS=1
	endif
	TESTPKG?=0
	NMS?=0
endif	

# cwmp-tr069 may use voip related library.
ifdef CONFIG_CWMP_TR069
SUBDIRS += cwmp-tr069
APMIB_SHARED = 1
export APMIB_SHARED
endif


all:
	@echo "No rules to make target. You should choose:"
	@echo "make {ap | gw | mp-ap | mp-gw | clean}"

mp-gw:
	@make do-make MP-GW=1 USE_RSDK_1_2_11=1
	@./mkimg MP_G=1 INCLUDE_WLAN=$(INCLUDE_WLAN)
	
ap:
	@make do-make AP=1 RTL8196B=1 RTL8196C=1 NMS=$(NMS) BUSYBOX_101=$(CONFIG_BUSYBOX101)
	@./mkimg AP_ROOT=1 LZ=1 RTL8196B=1 USE_OPENSSL=$(USE_OPENSSL) USE_4M=$(USE_4M) USE_WAPI=$(USE_WAPI) USE_RSDK_UCLIBC_0930=$(USE_RSDK_UCLIBC_0930)  NMS=$(NMS) BUSYBOX_101=$(CONFIG_BUSYBOX101) INCLUDE_WLAN=$(INCLUDE_WLAN)
	@../goahead-2.1.1/LINUX/mgbin -c -o rtl8196c-ap-8m.bin ../root.bin ../../linux-2.4.18/rtkload/linux.bin

gw:
	@make do-make GW=1 RTL8196B=1 $(RTL819X)=1 NMS=$(NMS) BUSYBOX_101=$(CONFIG_BUSYBOX101)
	@./mkimg GW_ROOT=1 LZ=1 RTL8196B=1 USE_OPENSSL=$(USE_OPENSSL) USE_4M=$(USE_4M) USE_WAPI=$(USE_WAPI) USE_RSDK_UCLIBC_0930=$(USE_RSDK_UCLIBC_0930)  NMS=$(NMS) BUSYBOX_101=$(CONFIG_BUSYBOX101) INCLUDE_WLAN=$(INCLUDE_WLAN)
	@../goahead-2.1.1/LINUX/mgbin -c -o rtl8196c-gw.bin $(WEB_PAGE) ../root.bin ../../linux-2.4.18/rtkload/linux.bin

do-make:
	@if [ "$(MOD)" != 1 ];then \
	if [ "$(BUSYBOX_101)" = 1 ]; then \
	diff ../$(BUSYBOX)/.config $(BUSYBOX_DFGCONFIG) > diff.txt; \
			if [ -s diff.txt ]; then \
                                make -C  busybox-1.01 clean ;\
                                cp -f $(BUSYBOX_DFGCONFIG) ../busybox-1.01/.config; \
                                cp -f $(BUSYBOX_DFGCONFIG).h ../busybox-1.01/include/config.h; \
                        fi; rm -f diff.txt; \
        fi; if [ "$(BUSYBOX_101)" != 1 ]; then \
	diff ../$(BUSYBOX)/.config $(BUSYBOX_DFGCONFIG) > diff.txt; \
			if [ -s diff.txt ]; then \
				make -C  ../busybox-1.00-pre8 clean ;\
				cp -f $(BUSYBOX_DFGCONFIG) ../busybox-1.00-pre8/.config; \
				cp -f $(BUSYBOX_DFGCONFIG).h ../busybox-1.00-pre8/include/config.h; \
			fi; rm -f diff.txt; \
	fi; \
	fi
	@if [ "$(MOD)" = 1 ];then \
	diff ../$(BUSYBOX)/.config $(BUSYBOX_DFGCONFIG) > diff.txt; \
			if [ -s diff.txt ]; then \
				make -C  busybox-1.1.3 clean ;\
				cp -f $(BUSYBOX_DFGCONFIG) ../busybox-1.1.3/.config; \
				cp -f $(BUSYBOX_DFGCONFIG).h ../busybox-1.1.3/include/config.h; \
			fi; rm -f diff.txt; \
	fi
	@if [ "$(GW_VPN)" = 1  -a ! -f  gmp-4.1.2/Makefile ];then \
		 cd gmp-4.1.2; ./config.8186; \
	fi

	@set -e; \
		 for i in $(SUBDIRS); do \
			 if [ $$i = ../net-snmp-5.4.1 ]; then \
				 cd $$i; ./$(CONFIGURE_SNMP); cd -;\
			 fi; \
if [ $$i = ../openssl-0.9.8b ]; then \
				cd $$i; \
				if [ $(CONFIG_RTL_WAPI_LOCAL_AS) = 1 ]; then \
					if [ ! -f dlib_config_done ]; then \
						./Configure shared mips-linux; make depend; \
						rm -f slib_config_done; touch dlib_config_done; \
						make clean; \
						cp Makefile.DLibForWapi Makefile; \
						cp test/Makefile.wapi test/Makefile; \
					fi;\
				else\
					if [ ! -f slib_config_done ]; then \
						./Configure no-des no-dh no-dsa no-idea no-md2 no-md4 no-rsa no-threads no-hw no-shared mips-linux; make depend; \
						rm -f dlib_config_done; touch slib_config_done;\
						make clean; \
						cp Makefile.SLibForWapi Makefile; \
						cp test/Makefile.wapi test/Makefile; \
					fi;\
				fi;\
				cd -;\
			 fi; \
			 $(MAKE) -C $$i $(DBG); \
		 done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean; done
	@rm -rf ../squashfs-lzma.o ../rootfs ../ramdisk ../libstrip/*.so ../libstrip/ldscript ../*.o ../root.bin ../ramdisk.gz rtl8196c*.bin
