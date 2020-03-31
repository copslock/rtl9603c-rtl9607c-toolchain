#
# Makefile for the GoAhead web server reference source base
#  for the Linux OS
#
# Copyright (c) GoAhead Software, Inc. 1995-2000
#
# $Id: Makefile,v 1.53.2.3 2010/12/16 05:32:20 bruce Exp $
#

ifndef APMIB_SHARED
APMIB_SHARED = 0
endif

all: compile

ARCH	= libwebs.a

SYSCONFIG_DIR=./system

NAME_VPN = webs-vpn
NAME_GW	= webs-gw
NAME_AP = webs-ap
FLASH_VPN = flash-vpn
FLASH_GW = flash-gw
FLASH_AP = flash-ap

CC	= mips-uclibc-gcc

AR	= mips-uclibc-ar

# User Management switch
UMSW	= -DUSER_MANAGEMENT_SUPPORT

# TLS client mode support
#TLS= -DTLS_CLIENT

# routing support
ROUTE = -DROUTE_SUPPORT

# WLAN support
#-DWLAN_EASY_CONFIG
WLANSUPP= -DUNIVERSAL_REPEATER -DWLAN_WPA -DWLAN_WDS -DWLAN_8185AG -DWLAN_WPA2 $(TLS)
WLANSUPP += -DWIFI_SIMPLE_CONFIG

# Digest Access switch
DASW	= -DDIGEST_ACCESS_SUPPORT

#Record which files need not include
EXCLUDE_ASP = 

# Gateway flag
GATEWAY = -DHOME_GATEWAY -DDOS_SUPPORT
ifeq ($(GW),1)
GATEWAY += -DROOT_FS
endif

# have VPN support
VPN = -DVPN_SUPPORT
# SSL switches (enable all to activate)
# SSLINC = /RSASSL/library/include
# SSLLIB = /RSASSL/library/lib/libsslc.a
# SSLSW	= -DWEBS_SSL_SUPPORT -I$(SSLINC)
# SSLPATCHFILE = ../websSSL.o

# If-Modified-Support switches (requires math library, libm.a)
# IFMODSW = -DWEBS_IF_MODIFIED_SUPPORT
# IFMODLIB = /usr/lib/libm.a

include ../../../linux-2.4.18/.config
ifeq ($(CONFIG_RTK_VLAN_SUPPORT),y)
VLAN_CONFIG_SUPPORTED = 1
ifeq ($(CONFIG_RTK_VLAN_ROUTETYPE),y)
RTK_VLAN_ROUTETYPE = 1
endif
endif
ifeq ($(CONFIG_RTL8196C),y)
INCLUDE_SYSCONF = 1
endif

ifeq ($(CONFIG_RTL8198),y)
INCLUDE_SYSCONF = 1
endif

ifeq ($(INCLUDE_SYSCONF),1)
SYSCONFIG_GW = sysconf-gw
SYSCONFIG_AP = sysconf-ap
INTERNET_PPP=ppp_inet
INTERNET_NTP=ntp_inet
INTERNET_DDNS=ddns_inet
else
SYSCONFIG_GW = 
SYSCONFIG_AP = 
INTERNET_PPP = 
INTERNET_NTP= 
INTERNET_DDNS=

endif

# for 802.11s
ifndef MESH_EXTRA_CONFIG_FLAGS
	ifeq ($(CONFIG_MESH_ENABLE),y)
		MESH_EXTRA_CONFIG_FLAGS += -DCONFIG_RTK_MESH -D_MESH_ACL_ENABLE_
		ifeq ($(CONFIG_11S_TEST_MODE),y)
			MESH_EXTRA_CONFIG_FLAGS += -D_11s_TEST_MODE_
		endif
		ifeq ($(CONFIG_NMS),y)
			MESH_EXTRA_CONFIG_FLAGS += -DSNMP_SUPPORT
		endif
	else
		EXCLUDE_ASP = ! -name buffalo.asp ! -name bull.asp ! -name wlmesh.asp ! -name wlmeshinfo.asp ! -name wlmeshACL.asp ! -name wlmeshproxy.asp
	endif
endif

ifndef VLAN_CONFIG_SUPPORTED
	EXCLUDE_ASP +=  ! -name vlan.asp
endif

ifneq ($(CONFIG_PCIE_POWER_SAVING),y)	
	EXCLUDE_ASP +=  ! -name powerConsumption.asp ! -name powerConsumption_data.asp ! -name powerConsumption_proc.sxsl
	EXCLUDE_ASP +=  ! -name num-0.gif ! -name num-1.gif ! -name num-2.gif ! -name num-3.gif ! -name num-4.gif ! -name num-5.gif
	EXCLUDE_ASP +=  ! -name num-6.gif ! -name num-7.gif ! -name num-8.gif ! -name num-9.gif ! -name dot.gif ! -name num-0.gif
	EXCLUDE_ASP +=  ! -name xml_data.js
endif

ifneq ($(CONFIG_NET_RADIO),y)
EXCLUDE_ASP +=! -name wlactrl.asp ! -name wladvanced.asp ! -name wlan_schedule.asp ! -name wlbasic.asp ! -name wlmultipleap.asp ! -name wlsecurity.asp
EXCLUDE_ASP +=! -name wlsecurity_all.asp ! -name wlstatbl.asp ! -name wlstatbl_vap.asp ! -name wlsurvey.asp ! -name wlwds.asp ! -name wlwdsenp.asp
EXCLUDE_ASP +=! -name wlwdstbl.asp ! -name wlwps.asp
else
EXCLUDE_ASP +=! -name stats_nowlan.asp ! -name status_nowlan.asp
endif

ifndef CONFIG_CWMP_TR069
EXCLUDE_ASP +=! -name share.js ! -name tr069config.asp
endif

WLANSUPP +=  $(MESH_EXTRA_CONFIG_FLAGS)

ifeq ($(CONFIG_RTL8196B_GW_8M),y)
	WLANSUPP += -DCONFIG_RTL8196B_GW_8M
endif

ifeq ($(CONFIG_RTL8196B_TLD),y)
	WLANSUPP += -DCONFIG_RTL8196B_TLD
endif

ifeq ($(CONFIG_RTL8196C),y)
        WLANSUPP += -DCONFIG_RTL8196C
endif

ifeq ($(CONFIG_RTL8198),y)
        WLANSUPP += -DCONFIG_RTL8198
endif

ifeq ($(CONFIG_RTL8192CD),y)
	WLANSUPP += -DCONFIG_RTL8192CD
endif

ifeq ($(CONFIG_RTL8196C_AP_ROOT),y)
	WLANSUPP += -DCONFIG_RTL8196C_AP_ROOT
endif

ifeq ($(CONFIG_RTL8198_AP_ROOT),y)
	WLANSUPP += -DCONFIG_RTL8198_AP_ROOT
endif

ifeq ($(CONFIG_RTL8196C_CLIENT_ONLY),y)
	WLANSUPP += -DCONFIG_RTL8196C_CLIENT_ONLY
	WLANSUPP += -DMOVE_OUT_DEFAULT_SETTING_FROM_FLASH
endif

ifeq ($(GW),1)

ifneq ($(CONFIG_NET_QOS),n)
ifneq ($(CONFIG_NET_QOS),)
	GATEWAY += -DENABLE_QOS

	ifneq ($(CONFIG_QOS_UI_BY_BANDWIDTH),n)
	ifneq ($(CONFIG_QOS_UI_BY_BANDWIDTH),)
		GATEWAY += -DQOS_BY_BANDWIDTH
	endif
	endif

endif
endif

endif

# goahead SSL support
#OPENSSL = 1
ifeq ($(CONFIG_USER_HTTPS_WITH_MATRIXSSL),y)
MATRIXSSL = 1
endif

ifeq ($(strip $(OPENSSL)),1)
SSLINC = ../../auth/Package/openssl-0.9.7b/include
SSLLIB =  ../../auth/Package/libssl.a  ../../auth/Package/libcrypto.a
#-lkrb5
SSLSW	= -DOPENSSL -DWEBS_SSL_SUPPORT -I$(SSLINC)
SSLPATCHFILE = ../websSSL.o
endif

ifeq ($(strip $(MATRIXSSL)),1)
SSLINC = ../matrixssl/examples
SSLLIB =  ../matrixssl/src/libmatrixssl.a #-lpthread
SSLSW	= -DMATRIX_SSL  -DWEBS_SSL_SUPPORT -I$(SSLINC)
SSLPATCHFILE = ../matrix_ssl.o
MATRIXSSL = libmatrixssl.a
endif

# Dependencies
DEPEND_FILES	= ../asp.o ../balloc.o ../base64.o ../cgi.o ../default.o  \
				  ../ejlex.o ../ejparse.o ../form.o \
				  ../h.o ../handler.o ../mime.o ../misc.o ../page.o  \
				  ../ringq.o ../rom.o \
				  ../sock.o ../sockGen.o $(SSLPATCHFILE) \
				  ../security.o ../sym.o ../uemf.o ../url.o ../value.o \
				  ../md5c.o ../um.o ../umui.o ../websda.o ../emfdb.o \
				  ../webrom.o ../webs.o ../websuemf.o
ifeq ($(APMIB_SHARED),1)
REALTEK_GW_FILES = fmtcpip_gw.o utility_gw.o fmfwall_gw.o \
			fmget_gw.o fmwlan_gw.o fmmgmt_gw.o version_gw.o fmddns_gw.o fmroute_gw.o fmpocketap_gw.o

REALTEK_VPN_FILES = fmtcpip_vpn.o utility_vpn.o fmfwall_vpn.o \
			fmget_vpn.o fmwlan_vpn.o fmmgmt_vpn.o version_vpn.o fmvpn.o fmddns_vpn.o fmroute_vpn.o

REALTEK_AP_FILES = fmtcpip_ap.o utility_ap.o \
			fmget_ap.o fmwlan_ap.o fmmgmt_ap.o version_ap.o
else
REALTEK_GW_FILES = apmib_gw.o fmtcpip_gw.o utility_gw.o mibtbl_gw.o fmfwall_gw.o \
			fmget_gw.o fmwlan_gw.o fmmgmt_gw.o version_gw.o fmddns_gw.o fmroute_gw.o fmpocketap_gw.o

REALTEK_VPN_FILES = apmib_vpn.o fmtcpip_vpn.o utility_vpn.o mibtbl_vpn.o fmfwall_vpn.o \
			fmget_vpn.o fmwlan_vpn.o fmmgmt_vpn.o version_vpn.o fmvpn.o fmddns_vpn.o fmroute_vpn.o

REALTEK_AP_FILES = apmib_ap.o fmtcpip_ap.o utility_ap.o mibtbl_ap.o \
			fmget_ap.o fmwlan_ap.o fmmgmt_ap.o version_ap.o
			
			
			
			
			
			
ifeq ($(INCLUDE_SYSCONF),1)			
REALTEK_SYS_GW_FILES = set_init_gw.o set_firewall_gw.o set_bridge_gw.o set_wlanapp_gw.o sys_utility_gw.o	set_wan_gw.o
REALTEK_SYS_AP_FILES = set_init_ap.o set_bridge_ap.o set_wlanapp_ap.o sys_utility_ap.o 	
REALTEK_INTERNET_PPP_FILES = sys_utility_gw.o
REALTEK_INTERNET_NTP_FILES = sys_utility_gw.o
REALTEK_INTERNET_DDNS_FILES = sys_utility_gw.o

else
REALTEK_SYS_GW_FILES = 
REALTEK_SYS_AP_FILES =  	
REALTEK_INTERNET_PPP_FILES = 
REALTEK_INTERNET_NTP_FILES = 
REALTEK_INTERNET_DDNS_FILES = 

endif
endif



CFLAGS	= -fno-strict-aliasing -DWEBS -DUEMF -DOS="LINUX" -DLINUX $(WLANSUPP) $(UMSW) $(DASW) $(SSLSW) $(IFMODSW) -Os -DDOWN_SIZE $(ROUTE)
OTHERS	= -DB_STATS -DB_FILL -DDEBUG
DEBUG	= -g -Wall
IFLAGS 	= -I..
LDFLAGS	= $(SSLLIB) $(IFMODLIB)

#
# Extra Feature Configuration
#

CFLAGS += $(EXTRA_CONFIG_CFLAGS)


ifeq ($(CONFIG_NET_RADIO),y)
CFLAGS += -DCONFIG_NET_RADIO
endif

ifeq ($(CONFIG_RTL8196C),y)

ifeq ($(CONFIG_POCKET_ROUTER_SUPPORT),y)
CFLAGS += -DCONFIG_POCKET_ROUTER_SUPPORT
endif

ifeq ($(CONFIG_DOMAIN_NAME_QUERY_SUPPORT),y)
CFLAGS += -DCONFIG_DOMAIN_NAME_QUERY_SUPPORT
endif

ifeq ($(CONFIG_RTL_CLIENT_MODE_SUPPORT),y)
CFLAGS += -DCONFIG_WLAN_CLIENT_MODE
endif

ifeq ($(CONFIG_RTL_REPEATER_MODE_SUPPORT),y)
CFLAGS += -DCONFIG_WLAN_REPEATER_MODE
endif

ifeq ($(CONFIG_RTL_WDS_SUPPORT),y)
CFLAGS += -DCONFIG_WLAN_WDS_SUPPORT
endif

ifeq ($(CONFIG_RTL_VAP_SUPPORT),y)
CFLAGS += -DCONFIG_WLAN_VAP_SUPPORT
endif

ifeq ($(CONFIG_HW_PROTOCOL_VLAN_TBL),y)
CFLAGS += -DCONFIG_HW_PROTOCOL_VLAN_TBL
endif

ifeq ($(CONFIG_RTK_IPV6_PASSTHRU_SUPPORT),y)
CFLAGS += -DCONFIG_RTK_IPV6_PASSTHRU_SUPPORT
endif

endif

ifeq ($(CONFIG_RTL8198),y)

ifeq ($(CONFIG_RTL_CLIENT_MODE_SUPPORT),y)
CFLAGS += -DCONFIG_WLAN_CLIENT_MODE
endif

ifeq ($(CONFIG_RTL_REPEATER_MODE_SUPPORT),y)
CFLAGS += -DCONFIG_WLAN_REPEATER_MODE
endif

ifeq ($(CONFIG_RTL_WDS_SUPPORT),y)
CFLAGS += -DCONFIG_WLAN_WDS_SUPPORT
endif

ifeq ($(CONFIG_RTL_VAP_SUPPORT),y)
CFLAGS += -DCONFIG_WLAN_VAP_SUPPORT
endif

endif

ifeq ($(CONFIG_PCIE_POWER_SAVING),y)
CFLAGS += -DPOWER_CONSUMPTION_SUPPORT
endif


ifeq ($(CONFIG_DOMAIN_NAME_QUERY_SUPPORT),y)

REALTEK_GW_FILES +=fmdomain_query_gw.o
REALTEK_SYS_GW_FILES +=set_domain_query_gw.o

REALTEK_AP_FILES +=fmdomain_query_ap.o
REALTEK_SYS_AP_FILES +=set_domain_query_ap.o
endif


ifeq ($(SLINK),1)
CFLAGS  += -DCOMPACK_SIZE -ffunction-sections -fdata-sections
LDFLAGS += --static -Wl,--gc-sections
TARGET = $(NAME_AP) $(FLASH_AP)
NEWADDEDCFLAGS = -DCOMPACK_SIZE -ffunction-sections -fdata-sections
else

ifeq ($(AP),1)
TARGET = $(NAME_AP) $(FLASH_AP) $(SYSCONFIG_AP) webpages-ap.bin
else
ifeq ($(GW),1)
TARGET = $(NAME_GW) $(FLASH_GW) $(SYSCONFIG_GW) $(INTERNET_PPP) $(INTERNET_NTP) $(INTERNET_DDNS) webpages-gw.bin
else
ifeq ($(VPN),1)
TARGET = $(NAME_VPN) $(FLASH_VPN) webpages-vpn.bin
else
TARGET = $(NAME_GW) $(NAME_AP) $(NAME_VPN) $(FLASH_AP) $(FLASH_GW) $(FLASH_VPN) \
	webpages-gw.bin webpages-ap.bin webpages-vpn.bin
endif
endif
endif

NEWADDEDCFLAGS =
endif

ifeq ($(CONFIG_RTL8196B),y)
	CFLAGS += -DCONFIG_RTL8196B
	NEWADDEDCFLAGS += -DCONFIG_RTL8196B
endif

ifeq ($(VLAN_CONFIG_SUPPORTED),1)
CFLAGS += -DVLAN_CONFIG_SUPPORTED
NEWADDEDCFLAGS += -DVLAN_CONFIG_SUPPORTED 
ifeq ($(RTK_VLAN_ROUTETYPE),1)
CFLAGS += -DCONFIG_RTK_VLAN_ROUTETYPE
NEWADDEDCFLAGS += -DCONFIG_RTK_VLAN_ROUTETYPE
endif
endif

ifeq ($(CONFIG_RTL_WAPI_SUPPORT),y)
	CFLAGS += -DCONFIG_RTL_WAPI_SUPPORT
	NEWADDEDCFLAGS += -DCONFIG_RTL_WAPI_SUPPORT
ifeq ($(CONFIG_RTL_WAPI_LOCAL_AS_SUPPORT),y)
CFLAGS += -DCONFIG_RTL_WAPI_LOCAL_AS_SUPPORT
NEWADDEDCFLAGS += -DCONFIG_RTL_WAPI_LOCAL_AS_SUPPORT
else
EXCLUDE_ASP += ! -name wlwapiCertManagement.asp ! -name  wlwapiDistribute.asp
endif
else
EXCLUDE_ASP += ! -name wlwapiCertManagement.asp ! -name  wlwapiDistribute.asp ! -name wlwapiRekey.asp ! -name  wlwapiinstallcert.asp
endif

# =========================================================
# added by Rock
# =========================================================
# rock: add voip support
-include ../../../linux-2.4.18/.config
ifneq ($(CONFIG_RTK_VOIP),n)
ifneq ($(CONFIG_RTK_VOIP),)
VOIP_SUPPORT=1
endif
endif


ifeq ($(CONFIG_RTL_COMAPI_CFGFILE),y)
	CFLAGS += -DCONFIG_IWPRIV_INTF
	CFLAGS += -DCONFIG_RTL_COMAPI_CFGFILE
	NEWADDEDCFLAGS += -DCONFIG_RTL_COMAPI_CFGFILE
endif

ifdef CONFIG_RTL_HW_SETTING_OFFSET
	CFLAGS += -DCONFIG_RTL_HW_SETTING_OFFSET=0x$(CONFIG_RTL_HW_SETTING_OFFSET)	
	NEWADDEDCFLAGS += -DCONFIG_RTL_HW_SETTING_OFFSET=0x$(CONFIG_RTL_HW_SETTING_OFFSET)
endif

ifdef CONFIG_RTL_DEFAULT_SETTING_OFFSET
	CFLAGS += -DCONFIG_RTL_DEFAULT_SETTING_OFFSET=0x$(CONFIG_RTL_DEFAULT_SETTING_OFFSET)	
	NEWADDEDCFLAGS += -DCONFIG_RTL_DEFAULT_SETTING_OFFSET=0x$(CONFIG_RTL_DEFAULT_SETTING_OFFSET)	
endif

ifdef CONFIG_RTL_CURRENT_SETTING_OFFSET
	CFLAGS += -DCONFIG_RTL_CURRENT_SETTING_OFFSET=0x$(CONFIG_RTL_CURRENT_SETTING_OFFSET)	
	NEWADDEDCFLAGS += -DCONFIG_RTL_CURRENT_SETTING_OFFSET=0x$(CONFIG_RTL_CURRENT_SETTING_OFFSET)	
endif

ifdef CONFIG_RTL_WEB_PAGES_OFFSET
	CFLAGS += -DCONFIG_RTL_WEB_PAGES_OFFSET=0x$(CONFIG_RTL_WEB_PAGES_OFFSET)	
	NEWADDEDCFLAGS += -DCONFIG_RTL_WEB_PAGES_OFFSET=0x$(CONFIG_RTL_WEB_PAGES_OFFSET)	
endif

ifdef CONFIG_RTL_LINUX_IMAGE_OFFSET  
	CFLAGS += -DCONFIG_RTL_CODE_IMAGE_OFFSET=0x$(CONFIG_RTL_LINUX_IMAGE_OFFSET)	
	NEWADDEDCFLAGS += -DCONFIG_RTL_CODE_IMAGE_OFFSET=0x$(CONFIG_RTL_LINUX_IMAGE_OFFSET)	
endif

ifdef CONFIG_RTL_ROOT_IMAGE_OFFSET 
	CFLAGS += -DCONFIG_RTL_ROOT_IMAGE_OFFSET=0x$(CONFIG_RTL_ROOT_IMAGE_OFFSET)	
	NEWADDEDCFLAGS += -DCONFIG_RTL_ROOT_IMAGE_OFFSET=0x$(CONFIG_RTL_ROOT_IMAGE_OFFSET)	
endif

ifdef VOIP_SUPPORT
PWD := $(shell pwd)
VOIP_APP = $(PWD)/../../rtk_voip
VOIP_AP_FILES = voip_flash_ap.o voip_flash_mib_ap.o voip_flash_tool_ap.o voip_flash_server_ap.o
VOIP_GW_FILES = voip_flash_gw.o voip_flash_mib_gw.o voip_flash_tool_gw.o voip_flash_server_gw.o
VOIP_VPN_FILES = voip_flash_vpn.o voip_flash_mib_vpn.o voip_flash_tool_vpn.o voip_flash_server_vpn.o
REALTEK_AP_FILES += $(VOIP_APP)/web/libweb_voip.a $(VOIP_AP_FILES)
REALTEK_GW_FILES += $(VOIP_APP)/web/libweb_voip.a $(VOIP_GW_FILES)
REALTEK_VPN_FILES += $(VOIP_APP)/web/libweb_voip.a $(VOIP_VPN_FILES)
VOIP_FLAGS = -DVOIP_SUPPORT -I$(VOIP_APP)/includes -I$(VOIP_APP)/web -I$(PWD) -I$(PWD)/..
ifdef CONFIG_RTK_VOIP_PACKAGE_867X
VOIP_FLAGS += -DATA867x
endif
ifdef CONFIG_RTK_VOIP_IP_PHONE
VOIP_FLAGS += -I$(VOIP_APP)/ui/include
endif
CFLAGS += $(VOIP_FLAGS)
endif


#=============================================================
#added by Jack for Tr-069
#=============================================================
ifdef CONFIG_CWMP_TR069
REALTEK_GW_FILES += fmtr069_gw.o
REALTEK_VPN_FILES += fmtr069_vpn.o
REALTEK_AP_FILES += fmtr069_ap.o
CFLAGS += -DCONFIG_CWMP_TR069
endif

ifdef CONFIG_RTK_VOIP_IP_PHONE
LDFLAGS += -lpthread
else
ifdef CONFIG_CWMP_TR069
LDFLAGS += -lpthread
endif
endif

compile:
ifdef VOIP_SUPPORT
	rm -f $(NAME_GW) $(NAME_AP) $(NAME_VPN)
	make -C $(VOIP_APP)/web -f rtl8186.mak WEB_FLAGS='$(DEBUG) $(CFLAGS) $(IFLAGS)' WEB_DIR=$(PWD)/..
endif
	@make compile2

compile2: $(TARGET)

ifeq ($(APMIB_SHARED),1)
CFLAGS += -DCONFIG_APMIB_SHARED
ifeq ($(AP),1)
APMIB_OBJ := apmib_ap.o mibtbl_ap.o
else
ifeq ($(GW),1)
APMIB_OBJ := apmib_gw.o mibtbl_gw.o
else
ifeq ($(VPN),1)
APMIB_OBJ := apmib_vpn.o mibtbl_vpn.o
endif
endif
endif

APMIB_LIB = apmib.so

$(APMIB_LIB): $(APMIB_OBJ)
	$(CC) -s -shared -o $@ $(APMIB_OBJ)
endif

#
#	Build archive of objects
#
$(ARCH): $(DEPEND_FILES)
	$(AR) $(ARFLAGS) $(ARCH) $?

#
#	Primary link
#
ifeq ($(APMIB_SHARED),1)
$(NAME_VPN): Makefile $(ARCH) $(REALTEK_VPN_FILES) main_vpn.o $(MATRIXSSL) $(APMIB_LIB)
	$(CC) -o $(NAME_VPN) $(DEBUG) \
		$(CFLAGS) $(IFLAGS) \
		main_vpn.o $(ARCH) $(REALTEK_VPN_FILES) $(LDFLAGS) $(APMIB_LIB)
	mips-uclibc-strip $@

$(NAME_GW): Makefile $(ARCH) $(REALTEK_GW_FILES) main_gw.o $(MATRIXSSL) $(APMIB_LIB)
	$(CC) -o $(NAME_GW) $(DEBUG) \
		$(CFLAGS) $(IFLAGS) \
		main_gw.o $(ARCH) $(REALTEK_GW_FILES) $(LDFLAGS) $(APMIB_LIB)
	mips-uclibc-strip $@

$(NAME_AP): Makefile $(ARCH) $(REALTEK_AP_FILES) main_ap.o $(APMIB_LIB)
	$(CC) -o $(NAME_AP) $(DEBUG) \
		$(CFLAGS) $(IFLAGS) \
		main_ap.o $(ARCH) $(REALTEK_AP_FILES) $(LDFLAGS) $(APMIB_LIB)
	mips-uclibc-strip $@

$(FLASH_VPN): Makefile flash_vpn.o $(VOIP_VPN_FILES) $(APMIB_LIB)
	$(CC) -o $@ $(DEBUG) \
		$(CFLAGS) $(IFLAGS) \
		flash_vpn.o $(VOIP_VPN_FILES) $(LDFLAGS) $(APMIB_LIB)
	mips-uclibc-strip $@

ifeq ($(CONFIG_RTL_COMAPI_CFGFILE),y)
$(FLASH_GW): Makefile flash_gw.o comapi.o $(VOIP_GW_FILES) $(APMIB_LIB)
	$(CC) -o $@ $(DEBUG) \
		$(CFLAGS) $(IFLAGS) \
		flash_gw.o comapi.o $(VOIP_GW_FILES) $(LDFLAGS) $(APMIB_LIB)
	mips-uclibc-strip $@
else
$(FLASH_GW): Makefile flash_gw.o $(VOIP_GW_FILES) $(APMIB_LIB)
        $(CC) -o $@ $(DEBUG) \
                $(CFLAGS) $(IFLAGS) \
                flash_gw.o $(VOIP_GW_FILES) $(LDFLAGS) $(APMIB_LIB)
        mips-uclibc-strip $@
endif

$(FLASH_AP): Makefile flash_ap.o $(VOIP_AP_FILES) $(APMIB_LIB)
	$(CC) -o $@ $(DEBUG) \
		$(CFLAGS) $(IFLAGS) \
		flash_ap.o $(VOIP_AP_FILES) $(LDFLAGS) $(APMIB_LIB)
	mips-uclibc-strip $@
else
$(NAME_VPN): Makefile $(ARCH) $(REALTEK_VPN_FILES) main_vpn.o $(MATRIXSSL)
	$(CC) -o $(NAME_VPN) $(DEBUG) \
		$(CFLAGS) $(IFLAGS) \
		main_vpn.o $(ARCH) $(REALTEK_VPN_FILES) $(LDFLAGS)
	mips-uclibc-strip $@

$(NAME_GW): Makefile $(ARCH) $(REALTEK_GW_FILES) main_gw.o $(MATRIXSSL)
	$(CC) -o $(NAME_GW) $(DEBUG) \
		$(CFLAGS) $(IFLAGS) \
		main_gw.o $(ARCH) $(REALTEK_GW_FILES) $(LDFLAGS)
	mips-uclibc-strip $@

$(NAME_AP): Makefile $(ARCH) $(REALTEK_AP_FILES) main_ap.o
	$(CC) -o $(NAME_AP) $(DEBUG) \
		$(CFLAGS) $(IFLAGS) \
		main_ap.o $(ARCH) $(REALTEK_AP_FILES) $(LDFLAGS)
	mips-uclibc-strip $@

$(FLASH_VPN): Makefile flash_vpn.o apmib_vpn_flash.o mibtbl_vpn.o $(VOIP_VPN_FILES)
	$(CC) -o $@ $(DEBUG) \
		$(CFLAGS) $(IFLAGS) \
		flash_vpn.o apmib_vpn_flash.o mibtbl_vpn.o $(VOIP_VPN_FILES) $(LDFLAGS)
	mips-uclibc-strip $@

ifeq ($(CONFIG_RTL_COMAPI_CFGFILE),y)
$(FLASH_GW): Makefile flash_gw.o comapi.o apmib_gw_flash.o mibtbl_gw.o $(VOIP_GW_FILES)
	$(CC) -o $@ $(DEBUG) \
		$(CFLAGS) $(IFLAGS) \
		flash_gw.o comapi.o apmib_gw_flash.o mibtbl_gw.o $(VOIP_GW_FILES) $(LDFLAGS)
	mips-uclibc-strip $@
else
$(FLASH_GW): Makefile flash_gw.o apmib_gw_flash.o mibtbl_gw.o $(VOIP_GW_FILES)
	$(CC) -o $@ $(DEBUG) \
		$(CFLAGS) $(IFLAGS) \
		flash_gw.o apmib_gw_flash.o mibtbl_gw.o $(VOIP_GW_FILES) $(LDFLAGS)
	mips-uclibc-strip $@
endif

$(SYSCONFIG_GW): Makefile sysconf_gw.o apmib_gw_flash.o mibtbl_gw.o $(REALTEK_SYS_GW_FILES)
	$(CC) -o $@ $(DEBUG) \
		$(CFLAGS) $(IFLAGS) \
		sysconf_gw.o apmib_gw_flash.o mibtbl_gw.o $(REALTEK_SYS_GW_FILES) $(LDFLAGS)
	mips-uclibc-strip $@

$(SYSCONFIG_AP): Makefile sysconf_ap.o apmib_ap_flash.o mibtbl_ap.o $(REALTEK_SYS_AP_FILES)
	$(CC) -o $@ $(DEBUG) \
		$(CFLAGS) $(IFLAGS) \
		sysconf_ap.o apmib_ap_flash.o mibtbl_ap.o $(REALTEK_SYS_AP_FILES) $(LDFLAGS)
		
$(INTERNET_PPP): Makefile ppp_inet.o $(REALTEK_INTERNET_PPP_FILES)
	$(CC) -o $@ $(DEBUG) \
		$(CFLAGS) $(IFLAGS) \
		ppp_inet.o $(REALTEK_INTERNET_PPP_FILES) $(LDFLAGS)
	mips-uclibc-strip $@
	
$(INTERNET_NTP): Makefile ntp_inet.o $(REALTEK_INTERNET_NTP_FILES)
	$(CC) -o $@ $(DEBUG) \
		$(CFLAGS) $(IFLAGS) \
		ntp_inet.o $(REALTEK_INTERNET_NTP_FILES) $(LDFLAGS)
	mips-uclibc-strip $@
		
$(INTERNET_DDNS): Makefile ddns_inet.o $(REALTEK_INTERNET_DDNS_FILES)
	$(CC) -o $@ $(DEBUG) \
		$(CFLAGS) $(IFLAGS) \
		ddns_inet.o $(REALTEK_INTERNET_DDNS_FILES) $(LDFLAGS)
	mips-uclibc-strip $@	
		
$(FLASH_AP): Makefile flash_ap.o apmib_ap_flash.o mibtbl_ap.o $(VOIP_AP_FILES)
	$(CC) -o $@ $(DEBUG) \
		$(CFLAGS) $(IFLAGS) \
		flash_ap.o apmib_ap_flash.o mibtbl_ap.o $(VOIP_AP_FILES) $(LDFLAGS)
	mips-uclibc-strip $@
endif

webpages-vpn.bin: tools
	find ../web-vpn $(EXCLUDE_ASP) -name "*.*" > web_files
	./compweb vpn web_files webpages-vpn.bin
		
webpages-gw.bin: tools
	find ../web-gw $(EXCLUDE_ASP) -name "*.*" > web_files
	./compweb gw web_files webpages-gw.bin

webpages-ap.bin: tools
	find ../web-ap $(EXCLUDE_ASP) -name "*.*" > web_files
	./compweb ap web_files webpages-ap.bin

webpages-cl.bin: tools
	find ../web-cl $(EXCLUDE_ASP) -name "*.*" > web_files
	./compweb cl web_files webpages-cl.bin

$(MATRIXSSL):
	make -C ../matrixssl/src

clean:
	rm -f $(NAME_GW) $(NAME_AP) $(NAME_VPN) $(FLASH_AP) $(FLASH_GW) $(FLASH_VPN) $(ARCH) *.o ../*.o *.bin web_files *.so
	rm -f $(SYSCONFIG_GW) $(SYSCONFIG_AP) $(INTERNET_PPP) $(INTERNET_NTP) $(INTERNET_DDNS)
	make -C ../matrixssl/src clean
	make -f Makefile-tools clean
	make -f Makefile-x86 clean
# =========================================================
# VoIP added by Rock
# =========================================================
ifdef VOIP_SUPPORT
	make -C $(VOIP_APP)/web -f rtl8186.mak clean WEB_DIR=$(PWD)/..
endif

#
#	Dependencies
#
../asp.o:  ../webs.h ../wsIntrn.h ../ej.h ../ejIntrn.h ../uemf.h

../balloc.o: ../balloc.c ../uemf.h

../base64.o:  ../base64.c ../webs.h ../wsIntrn.h  ../ej.h ../ejIntrn.h ../uemf.h

../cgi.o:  ../webs.h ../wsIntrn.h ../uemf.h

../default.o:  ../default.c ../webs.h ../wsIntrn.h ../ej.h ../ejIntrn.h ../uemf.h

../ejlex.o:  ../ejlex.c ../ej.h ../ejIntrn.h ../uemf.h

../ejparse.o:  ../ejparse.c ../ej.h ../ejIntrn.h ../uemf.h

../emfdb.o:  ../emfdb.h ../wsIntrn.h ../uemf.h

../form.o:  ../form.c ../webs.h ../wsIntrn.h ../ej.h ../ejIntrn.h ../uemf.h

../h.o:  ../h.c ../uemf.h

../handler.o:  ../handler.c ../webs.h ../wsIntrn.h ../ej.h ../ejIntrn.h ../uemf.h

../md5c.o:  ../md5.h ../wsIntrn.h ../uemf.h

../mime.o:  ../mime.c ../webs.h ../wsIntrn.h ../ej.h ../ejIntrn.h ../uemf.h

../misc.o:  ../misc.c ../uemf.h

../page.o:  ../page.c ../webs.h ../wsIntrn.h ../ej.h ../ejIntrn.h ../uemf.h

../ringq.o:  ../ringq.c ../uemf.h

../rom.o:  ../rom.c ../webs.h ../wsIntrn.h ../ej.h ../ejIntrn.h ../uemf.h

../security.o:  ../security.c ../webs.h ../wsIntrn.h ../ej.h ../ejIntrn.h ../uemf.h

../sock.o:  ../sock.c ../uemf.h

../sockGen.o:  ../sockGen.c ../uemf.h

../sym.o:  ../sym.c ../uemf.h

../uemf.o:  ../uemf.c ../uemf.h

../um.o:  ../webs.h ../wsIntrn.h ../um.h ../uemf.h

../umui.o:  ../webs.h ../wsIntrn.h ../um.h ../uemf.h

../url.o:  ../url.c ../webs.h ../wsIntrn.h ../ej.h ../ejIntrn.h ../uemf.h

../value.o:  ../value.c ../uemf.h

../webrom.o:  ../webrom.c ../webs.h ../wsIntrn.h ../uemf.h

../webs.o:  ../webs.c ../webs.h ../wsIntrn.h ../ej.h ../ejIntrn.h ../uemf.h

../websda.o:  ../webs.h ../wsIntrn.h ../websda.h ../uemf.h

../websuemf.o:  ../websuemf.c ../webs.h ../wsIntrn.h ../ej.h ../ejIntrn.h ../uemf.h

../websSSL.o:  ../websSSL.c ../websSSL.h ../wsIntrn.h ../ej.h ../ejIntrn.h ../uemf.h

## new added modules by realtek ---------------
main_gw.o:  main.c ../wsIntrn.h ../webs.h ../ej.h ../ejIntrn.h ../uemf.h apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(IFLAGS) $<

ifeq ($(APMIB_SHARED),1)
apmib_gw.o: apmib.c apmib.h apform.h utility.h
	$(CC) -c -o $@ -fpic $(DEBUG) $(CFLAGS) $(GATEWAY) $(IFLAGS) $<
else
apmib_gw.o: apmib.c apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(IFLAGS) $<
endif

fmtcpip_gw.o: fmtcpip.c apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(IFLAGS) $<

fmddns_gw.o: fmddns.c apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(IFLAGS) $<

utility_gw.o: utility.c apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(IFLAGS) $<

ifeq ($(APMIB_SHARED),1)
mibtbl_gw.o: mibtbl.c apmib.h apform.h utility.h
	$(CC) -c -o $@ -fpic $(DEBUG) $(CFLAGS) $(GATEWAY) $(IFLAGS) $<
else
mibtbl_gw.o: mibtbl.c apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(IFLAGS) $<
endif

fmfwall_gw.o: fmfwall.c apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(IFLAGS) $<

fmget_gw.o: fmget.c apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(IFLAGS) $<

fmwlan_gw.o: fmwlan.c apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(IFLAGS) $<

fmmgmt_gw.o: fmmgmt.c apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(IFLAGS) $<

version_gw.o: version.c
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(IFLAGS) $<

flash_gw.o: flash.c apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(IFLAGS) $<

ifeq ($(CONFIG_RTL_COMAPI_CFGFILE),y)
comapi.o: comapi.c apmib.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(IFLAGS) $<
endif

sysconf_gw.o: $(SYSCONFIG_DIR)/sysconf.c apmib.h apform.h $(SYSCONFIG_DIR)/sys_utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(IFLAGS) $<
	
set_init_gw.o:$(SYSCONFIG_DIR)/set_init.c apmib.h apform.h $(SYSCONFIG_DIR)/syswan.h $(SYSCONFIG_DIR)/sys_utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(IFLAGS) $<
	
set_firewall_gw.o: $(SYSCONFIG_DIR)/set_firewall.c apmib.h apform.h $(SYSCONFIG_DIR)/sys_utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(IFLAGS) $<
	
set_bridge_gw.o: $(SYSCONFIG_DIR)/set_bridge.c apmib.h apform.h $(SYSCONFIG_DIR)/sys_utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(IFLAGS) $<
		
set_wlanapp_gw.o:	$(SYSCONFIG_DIR)/set_wlanapp.c apmib.h apform.h $(SYSCONFIG_DIR)/sys_utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(IFLAGS) $<

set_wan_gw.o:$(SYSCONFIG_DIR)/set_wan.c apmib.h apform.h $(SYSCONFIG_DIR)/syswan.h $(SYSCONFIG_DIR)/sys_utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(IFLAGS) $<

set_domain_query_gw.o:$(SYSCONFIG_DIR)/set_domain_query.c apmib.h apform.h $(SYSCONFIG_DIR)/syswan.h $(SYSCONFIG_DIR)/sys_utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(IFLAGS) $<
	
sys_utility_gw.o: $(SYSCONFIG_DIR)/sys_utility.c $(SYSCONFIG_DIR)/sys_utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(IFLAGS) $<
	
	
ppp_inet.o: $(SYSCONFIG_DIR)/ppp_inet.c $(SYSCONFIG_DIR)/syswan.h $(SYSCONFIG_DIR)/sys_utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(IFLAGS) $<		
	
ntp_inet.o: $(SYSCONFIG_DIR)/ntp_inet.c $(SYSCONFIG_DIR)/syswan.h $(SYSCONFIG_DIR)/sys_utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(IFLAGS) $<		
		
ddns_inet.o: $(SYSCONFIG_DIR)/ddns_inet.c $(SYSCONFIG_DIR)/syswan.h $(SYSCONFIG_DIR)/sys_utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(IFLAGS) $<		
			
convert_gw.o: convert.c apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(IFLAGS) $<

fmroute_gw.o: fmroute.c apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(VPN) $(IFLAGS) $<

fmpocketap_gw.o: fmpocketap.c apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(VPN) $(IFLAGS) $<
	
fmdomain_query_gw.o: fmdomain_query.c apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(VPN) $(IFLAGS) $<
################################################################################
main_vpn.o:  main.c ../wsIntrn.h ../webs.h ../ej.h ../ejIntrn.h ../uemf.h apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(VPN) $(IFLAGS) $<

ifeq ($(APMIB_SHARED),1)
apmib_vpn.o: apmib.c apmib.h apform.h utility.h
	$(CC) -c -o $@ -fpic $(DEBUG) $(CFLAGS) $(GATEWAY) $(VPN) $(IFLAGS) $<
else
apmib_vpn.o: apmib.c apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(VPN) $(IFLAGS) $<
endif

fmtcpip_vpn.o: fmtcpip.c apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(VPN) $(IFLAGS) $<

fmddns_vpn.o: fmddns.c apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(VPN) $(IFLAGS) $<

utility_vpn.o: utility.c apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(VPN) $(IFLAGS) $<

ifeq ($(APMIB_SHARED),1)
mibtbl_vpn.o: mibtbl.c apmib.h apform.h utility.h
	$(CC) -c -o $@ -fpic $(DEBUG) $(CFLAGS) $(GATEWAY) $(VPN) $(IFLAGS) $<
else
mibtbl_vpn.o: mibtbl.c apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(VPN) $(IFLAGS) $<
endif

fmfwall_vpn.o: fmfwall.c apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(VPN) $(IFLAGS) $<

fmvpn.o: fmvpn.c apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(VPN) $(IFLAGS) $<

fmget_vpn.o: fmget.c apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(VPN) $(IFLAGS) $<

fmwlan_vpn.o: fmwlan.c apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(VPN) $(IFLAGS) $<

fmmgmt_vpn.o: fmmgmt.c apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(VPN) $(IFLAGS) $<

version_vpn.o: version.c apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(VPN) $(IFLAGS) $<

flash_vpn.o: flash.c apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(VPN) $(IFLAGS) $<

convert_vpn.o: convert.c apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(VPN) $(IFLAGS) $<

fmroute_vpn.o: fmroute.c apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(VPN) $(IFLAGS) $<

################################################################################
main_ap.o:  main.c ../wsIntrn.h ../webs.h ../ej.h ../ejIntrn.h ../uemf.h apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(IFLAGS) $<

ifeq ($(APMIB_SHARED),1)
apmib_ap.o: apmib.c apmib.h apform.h utility.h
	$(CC) -c -o $@ -fpic $(DEBUG) $(CFLAGS) $(IFLAGS) $<
else
apmib_ap.o: apmib.c apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(IFLAGS) $<
endif

fmtcpip_ap.o: fmtcpip.c apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(IFLAGS) $<

utility_ap.o: utility.c apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(IFLAGS) $<

ifeq ($(APMIB_SHARED),1)
mibtbl_ap.o: mibtbl.c apmib.h apform.h utility.h
	$(CC) -c -o $@ -fpic $(DEBUG) $(CFLAGS) $(IFLAGS) $<
else
mibtbl_ap.o: mibtbl.c apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(IFLAGS) $<
endif

fmget_ap.o: fmget.c apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(IFLAGS) $<

fmwlan_ap.o: fmwlan.c apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(IFLAGS) $<

fmmgmt_ap.o: fmmgmt.c apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(IFLAGS) $<

version_ap.o: version.c
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(IFLAGS) $<

flash_ap.o: flash.c apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(IFLAGS) $<

sysconf_ap.o: $(SYSCONFIG_DIR)/sysconf.c apmib.h apform.h $(SYSCONFIG_DIR)/sys_utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(IFLAGS) $<
	
set_init_ap.o:$(SYSCONFIG_DIR)/set_init.c apmib.h apform.h $(SYSCONFIG_DIR)/syswan.h $(SYSCONFIG_DIR)/sys_utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(IFLAGS) $<
	
set_bridge_ap.o: $(SYSCONFIG_DIR)/set_bridge.c apmib.h apform.h $(SYSCONFIG_DIR)/sys_utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(IFLAGS) $<
		
set_wlanapp_ap.o:	$(SYSCONFIG_DIR)/set_wlanapp.c apmib.h apform.h $(SYSCONFIG_DIR)/sys_utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(IFLAGS) $<
	
set_domain_query_ap.o:$(SYSCONFIG_DIR)/set_domain_query.c apmib.h apform.h $(SYSCONFIG_DIR)/syswan.h $(SYSCONFIG_DIR)/sys_utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(IFLAGS) $<
		
sys_utility_ap.o: $(SYSCONFIG_DIR)/sys_utility.c $(SYSCONFIG_DIR)/sys_utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(IFLAGS) $<
	
convert_ap.o: convert.c apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(IFLAGS) $<

fmdomain_query_ap.o: fmdomain_query.c apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(IFLAGS) $<
	
ifeq ($(APMIB_SHARED),0)
apmib_vpn_flash.o: apmib.c apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(GATEWAY) $(WLANSUPP) $(NEWADDEDCFLAGS)$(ROUTE) $(VPN) $(VOIP_FLAGS) $<

apmib_gw_flash.o: apmib.c apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(GATEWAY) $(WLANSUPP) $(NEWADDEDCFLAGS) $(ROUTE) $(VOIP_FLAGS) $<

apmib_ap_flash.o: apmib.c apmib.h apform.h utility.h
	$(CC) -c -o $@ $(DEBUG) $(WLANSUPP) $(NEWADDEDCFLAGS) $(VOIP_FLAGS) $<
endif
## --------------------------------------------
x86:
	make -f Makefile-x86
tools:
	make -f Makefile-tools
	find ../web-gw/ -name ".svn" | xargs rm -fr


#
#	Transition rules (add -o to put object in right directory)
#
.c.o:
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(IFLAGS) $<

# =========================================================
# added by Rock
# =========================================================
voip_flash_ap.o: $(VOIP_APP)/flash/voip_flash.c $(VOIP_APP)/includes/voip_flash.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(IFLAGS) $<

voip_flash_gw.o: $(VOIP_APP)/flash/voip_flash.c $(VOIP_APP)/includes/voip_flash.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(IFLAGS) $<

voip_flash_vpn.o: $(VOIP_APP)/flash/voip_flash.c $(VOIP_APP)/includes/voip_flash.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(VPN) $(IFLAGS) $<

voip_flash_mib_ap.o: $(VOIP_APP)/flash/voip_flash_mib.c $(VOIP_APP)/includes/voip_flash_mib.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(IFLAGS) $<

voip_flash_mib_gw.o: $(VOIP_APP)/flash/voip_flash_mib.c $(VOIP_APP)/includes/voip_flash_mib.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(IFLAGS) $<

voip_flash_mib_vpn.o: $(VOIP_APP)/flash/voip_flash_mib.c $(VOIP_APP)/includes/voip_flash_mib.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(VPN) $(IFLAGS) $<

voip_flash_tool_ap.o: $(VOIP_APP)/flash/voip_flash_tool.c $(VOIP_APP)/includes/voip_flash_tool.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(IFLAGS) $<

voip_flash_tool_gw.o: $(VOIP_APP)/flash/voip_flash_tool.c $(VOIP_APP)/includes/voip_flash_tool.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(IFLAGS) $<

voip_flash_tool_vpn.o: $(VOIP_APP)/flash/voip_flash_tool.c $(VOIP_APP)/includes/voip_flash_tool.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(VPN) $(IFLAGS) $<

voip_flash_server_ap.o: $(VOIP_APP)/flash/voip_flash_server.c $(VOIP_APP)/includes/voip_flash.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(IFLAGS) $<

voip_flash_server_gw.o: $(VOIP_APP)/flash/voip_flash_server.c $(VOIP_APP)/includes/voip_flash.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(IFLAGS) $<

voip_flash_server_vpn.o: $(VOIP_APP)/flash/voip_flash_server.c $(VOIP_APP)/includes/voip_flash.h
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(VPN) $(IFLAGS) $<

#===========================================================================
#added by Jack for TR-069
#===========================================================================
fmtr069_gw.o: fmtr069.c
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(VPN) $(IFLAGS) $<
fmtr069_vpn.o: fmtr069.c
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(VPN) $(IFLAGS) $<
fmtr069_ap.o: fmtr069.c
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(GATEWAY) $(VPN) $(IFLAGS) $<

