#for mesh
#include ./.config 
MAKE = make
ifeq ($(USE_RSDK_1_2_11),1)
export PATH
PATH:=/toolchain/rsdk-1.2.11/linux:$(PATH)
endif
USE_RSDK_UCLIBC_0930=1
#CONFIG_BUSYBOX101=1
include ../linux-2.4.18/.config


ifdef CONFIG_RTL_WAPI_SUPPORT
##To test openssl
CONFIG_OPENSSL=1
CONFIG_WAPI=1
ifdef CONFIG_RTL_WAPI_LOCAL_AS_SUPPORT
CONFIG_RTL_WAPI_LOCAL_AS=1
endif
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

ifeq ($(MP-AP),1)
	SUBDIRS =bridge-utils busybox-1.00-pre8 goahead-2.1.1/LINUX\
	wireless_tools.25 mp-daemon
	DBG=MP=1
else
ifeq ($(MP-GW),1)
	SUBDIRS =bridge-utils busybox-1.00-pre8 goahead-2.1.1/LINUX\
	wireless_tools.25 mp-daemon
	DBG=MP=1
else
	SUBDIRS = bridge-utils $(BUSYBOX) goahead-2.1.1/LINUX udhcp-0.9.9-pre \
	wireless_tools.25 rc IAPP
	ifeq ($(AP),1)
	DBG=AP=1
	else

		ifeq ($(GW),1)
		DBG=GW=1
		else
		DBG=GW_VPN=1
		endif
		SUBDIRS += iptables-1.2.6a ppp-2.4.2/pppd/plugins/rp-pppoe ppp-2.4.2/pppd   pptp-linux-1.3.1 ntpclient dnrd-2.12.1/src updatedd-2.5
		SUBDIRS += igmpproxy
	endif
endif
endif

ifeq ($(CONFIG_OPENSSL),1)
	SUBDIRS += openssl-0.9.8b
ifeq ($(CONFIG_WAPI),1)
	SUBDIRS += wapi_utils/udp_sock
	SUBDIRS += wapi_utils/rwWapiFiles
	SUBDIRS += wapi_utils/other_tools
endif
endif

ifeq ($(GW_VPN),1)
SUBDIRS += mawk-1.3.3  openswan-1.0.1   gmp-4.1.2
endif


ifeq ($(GW),1)
SUBDIRS += iproute2
endif

# keith: add l2tp support. 20080515
ifeq ($(GW),1)
SUBDIRS += l2tpd
endif

ifeq ($(DEBUG),1)
DBG+=DEBUG=1
endif

ifeq ($(GW_VPN),1)
BUSYBOX_DFGCONFIG=busybox-1.00-pre8/rtl8186/vpn_config
else
ifeq ($(MOD),1)
BUSYBOX_DFGCONFIG=$(BUSYBOX)/rtl8186/nm_config_module
else
ifeq ($(BUSYBOX_101),1)
BUSYBOX_DFGCONFIG=$(BUSYBOX)/rtl8186/nm_config
else
BUSYBOX_DFGCONFIG=busybox-1.00-pre8/rtl8186/nm_config
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

# rock: add voip support
-include ../linux-2.4.18/.config
ifneq ($(CONFIG_RTK_VOIP),n)
ifneq ($(CONFIG_RTK_VOIP),)
VOIP_SUPPORT=1
endif
endif
#for mesh

# 11s extra
ifeq ($(CONFIG_MESH_ENABLE),y)
	SUBDIRS += 11s-PathSelection 
	#MESH_EXTRA_CONFIG_FLAGS += -DCONFIG_RTK_MESH -D_MESH_ACL_ENABLE_ -DSNMP_SUPPORT
	MESH_EXTRA_CONFIG_FLAGS += -DCONFIG_RTK_MESH -D_MESH_ACL_ENABLE_  -DCONFIG_NEW_MESH_UI
	ifeq ($(CONFIG_TEST_PKG),y)
		SUBDIRS += test_traffic netperf-2.4.4 		
		TESTPKG=1
	endif
	ifeq ($(CONFIG_11S_TEST_MODE),y)
			MESH_EXTRA_CONFIG_FLAGS += -D_11s_TEST_MODE_
	endif
	ifeq ($(CONFIG_NMS),y)
		MESH_EXTRA_CONFIG_FLAGS += -DSNMP_SUPPORT
		SUBDIRS += net-snmp-5.2.3 nmsd burn_image 
		NMS=1
	endif
	TESTPKG?=0
	NMS?=0
endif	

ifdef VOIP_SUPPORT
SUBDIRS += rtk_voip cleshell #net-snmp-5.2.3
BUSYBOX_DFGCONFIG = busybox-1.00-pre8/rtl8186/voip_config
endif

ifeq ($(CONFIG_RTL8196B_GW_8M),y)
ifeq ($(BUSYBOX_101),1)
BUSYBOX_DFGCONFIG = busybox-1.01/rtl8186/nm_config_8m
else
BUSYBOX_DFGCONFIG = busybox-1.00-pre8/rtl8186/nm_config_8m
endif
endif

# cwmp-tr069 may use voip related library.
ifdef CONFIG_CWMP_TR069
SUBDIRS += cwmp-tr069
APMIB_SHARED = 1
export APMIB_SHARED
endif


all:
ifeq ($(CONFIG_RTL8196C),y)

	make -C ./8196c/

else

ifeq ($(CONFIG_RTL8198),y)
	make -C ./8196c/
else
	@echo "No rules to make target. You should choose:"
	@echo "make {ap | gw | clean | mp-ap | mp-gw }"
endif
endif


mp-gw:
ifeq ($(CONFIG_RTL8196C),y)
	make -C ./8196c/ mp-gw
else

ifeq ($(CONFIG_RTL8198),y)
	make -C ./8196c/ mp-gw
else

	@make do-make MP-GW=1 USE_RSDK_1_2_11=1
	@./mkimg MP_G=1 INCLUDE_WLAN=$(INCLUDE_WLAN)
endif
endif

ap:
ifeq ($(CONFIG_RTL8196C),y)

	make -C ./8196c/ ap

else
	
ifeq ($(CONFIG_RTL8198),y)
	make -C ./8196c/ ap
else

ifeq ($(CONFIG_RTL8196B),y)
	@make do-make AP=1 RTL8196B=1 NMS=$(NMS)
	@./mkimg AP_ROOT=1 LZ=1 RTL8196B=1 USE_RSDK_UCLIBC_0930=$(USE_RSDK_UCLIBC_0930) NMS=$(NMS)
else
	@make do-make AP=1 RTL8196B=0 NMS=$(NMS)
	@./mkimg AP=1 NMS=$(NMS) TESTPKG=$(TESTPKG)
endif
endif
endif

gw:
ifeq ($(CONFIG_RTL8196C),y)

	make -C ./8196c/ gw

else

ifeq ($(CONFIG_RTL8198),y)
	make -C ./8196c/ gw
else

ifeq ($(CONFIG_RTL8196B),y)
	@make do-make GW=1 RTL8196B=1 NMS=$(NMS) TLD=$(CONFIG_TLD) BUSYBOX_101=$(CONFIG_BUSYBOX101)
ifeq ($(CONFIG_OPENSSL),1)
ifeq ($(CONFIG_WAPI),1)
	@./mkimg GW_ROOT=1 LZ=1 RTL8196B=1 USE_OPENSSL=1 USE_4M=1 USE_WAPI=1 USE_RSDK_UCLIBC_0930=$(USE_RSDK_UCLIBC_0930)  NMS=$(NMS) BUSYBOX_101=$(CONFIG_BUSYBOX101)
else
       @./mkimg GW_ROOT=1 LZ=1 RTL8196B=1 USE_OPENSSL=1 USE_4M=1 USE_RSDK_UCLIBC_0930=$(USE_RSDK_UCLIBC_0930)  NMS=$(NMS) BUSYBOX_101=$(CONFIG_BUSYBOX101)
endif
else
	@./mkimg GW_ROOT=1 LZ=1 RTL8196B=1 USE_RSDK_UCLIBC_0930=$(USE_RSDK_UCLIBC_0930)  NMS=$(NMS) BUSYBOX_101=$(CONFIG_BUSYBOX101)
endif
else
	@make do-make GW=1 RTL8196B=0  NMS=$(NMS) BUSYBOX_101=$(CONFIG_BUSYBOX101)
	@./mkimg GW_ROOT=1  NMS=$(NMS) TESTPKG=$(TESTPKG) BUSYBOX_101=$(CONFIG_BUSYBOX101) USE_RSDK_UCLIBC_0930=1
endif
endif
endif

gw-spi:
	@make do-make GW=1 RTL8196B=1 BUSYBOX_101=$(CONFIG_BUSYBOX101)
	@./mkimg GW_ROOT=1 USE_SAMBA=0 SPI=1 BUSYBOX_101=$(CONFIG_BUSYBOX101)

gw-mod:
	@make do-make GW=1 MOD=1
	./mkimg GW_ROOT=1 MOD=1 LZ=1
gw-vpn:
	make do-make GW_VPN=1
	@./mkimg GW_VPN=1

gw-snmp:
	@make do-make GW=1 CONFIG_SNMP=1
ifeq ($(CONFIG_RTL8196B),y)
	@./mkimg GW_ROOT=1 APMIB_SHARED=1 LZ=1 CONFIG_SNMP=1 RTL8196B=1 BUSYBOX_101=$(CONFIG_BUSYBOX101)
else	
	@./mkimg GW_ROOT=1 APMIB_SHARED=1 LZ=1 CONFIG_SNMP=1 BUSYBOX_101=$(CONFIG_BUSYBOX101)
endif

gw-clean:
	make clean GW=1 CONFIG_SNMP=1 BUSYBOX_101=$(CONFIG_BUSYBOX101)



do-make:
	@if [ "$(MOD)" != 1 ];then \
	if [ "$(BUSYBOX_101)" = 1 ]; then \
	diff $(BUSYBOX)/.config $(BUSYBOX_DFGCONFIG) > diff.txt; \
			if [ -s diff.txt ]; then \
                                make -C  busybox-1.01 clean ;\
                                cp -f $(BUSYBOX_DFGCONFIG) busybox-1.01/.config; \
                                cp -f $(BUSYBOX_DFGCONFIG).h busybox-1.01/include/config.h; \
                        fi; rm -f diff.txt; \
        fi; if [ "$(BUSYBOX_101)" != 1 ]; then \
	diff $(BUSYBOX)/.config $(BUSYBOX_DFGCONFIG) > diff.txt; \
			if [ -s diff.txt ]; then \
				make -C  busybox-1.00-pre8 clean ;\
				cp -f $(BUSYBOX_DFGCONFIG) busybox-1.00-pre8/.config; \
				cp -f $(BUSYBOX_DFGCONFIG).h busybox-1.00-pre8/include/config.h; \
			fi; rm -f diff.txt; \
	fi; \
	fi
	@if [ "$(MOD)" = 1 ];then \
	diff $(BUSYBOX)/.config $(BUSYBOX_DFGCONFIG) > diff.txt; \
			if [ -s diff.txt ]; then \
				make -C  busybox-1.1.3 clean ;\
				cp -f $(BUSYBOX_DFGCONFIG) busybox-1.1.3/.config; \
				cp -f $(BUSYBOX_DFGCONFIG).h busybox-1.1.3/include/config.h; \
			fi; rm -f diff.txt; \
	fi
	@if [ "$(GW_VPN)" = 1  -a ! -f  gmp-4.1.2/Makefile ];then \
		 cd gmp-4.1.2; ./config.8186; \
	fi

	@set -e; \
		 for i in $(SUBDIRS); do \
			 if [ $$i = net-snmp-5.4.1 ]; then \
				 cd $$i; ./$(CONFIGURE_SNMP); cd -;\
			 fi; \
if [ $$i = openssl-0.9.8b ]; then \
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

# rock: add dep rule for voip support
dep:
ifdef VOIP_SUPPORT
	ln -fs ../../linux-2.4.18/.config rtk_voip/.linux_config
	ln -fs ../../../linux-2.4.18/include/linux/autoconf.h rtk_voip/includes/kernel_config.h
	ln -fs ../../../linux-2.4.18/rtk_voip/include/rtk_voip.h rtk_voip/includes/rtk_voip.h
	ln -fs ../../../linux-2.4.18/rtk_voip/include/voip_control.h rtk_voip/includes/voip_control.h
	ln -fs ../../../linux-2.4.18/rtk_voip/include/voip_params.h rtk_voip/includes/voip_params.h
	ln -fs ../../../linux-2.4.18/rtk_voip/include/voip_feature.h rtk_voip/includes/voip_feature.h
	ln -fs ../../../linux-2.4.18/acmw_lx/include/userdef.h rtk_voip/includes/acmw_userdef.h
ifdef CONFIG_RTK_VOIP_IP_PHONE
	make -C rtk_voip/ui dep
endif
endif



clean:
ifeq ($(CONFIG_RTL8196C),y)

	make -C ./8196c/ clean

else
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean; done
	@rm -rf rootfs ramdisk libstrip/*.so libstrip/ldscript *.o root.bin ramdisk.gz fw.bin
endif