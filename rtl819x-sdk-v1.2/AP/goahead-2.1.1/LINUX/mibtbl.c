/*
 *      MIB table declaration
 *
 *      Authors: David Hsu	<davidhsu@realtek.com.tw>
 *
 *      $Id: mibtbl.c,v 1.39.2.4 2011/01/17 08:26:53 keith_huang Exp $
 *
 */

/* Include files */
#include "apmib.h"
#include "mibtbl.h"

/* Global variable definitions */

/*
 * When using flash (set/get/all) command to access the MIB of below table,
 * it needs append a keyword "DEF_" in ahead of mib name when access
 * default setting MIB.
 */

mib_table_entry_T mib_table[]={
#ifdef HOME_GATEWAY
{MIB_WAN_MAC_ADDR, 		"WAN_MAC_ADDR",		BYTE6_T, 	_OFFSET(wanMacAddr), _SIZE(wanMacAddr)},
{MIB_WAN_IP_ADDR,		"WAN_IP_ADDR", 		IA_T,		_OFFSET(wanIpAddr), _SIZE(wanIpAddr)},
{MIB_WAN_SUBNET_MASK,		"WAN_SUBNET_MASK", 	IA_T, 		_OFFSET(wanSubnetMask), _SIZE(wanSubnetMask)},
{MIB_WAN_DEFAULT_GATEWAY,	"WAN_DEFAULT_GATEWAY",	IA_T,		_OFFSET(wanDefaultGateway), _SIZE(wanDefaultGateway)},
{MIB_WAN_DHCP,			"WAN_DHCP",		BYTE_T,		_OFFSET(wanDhcp), _SIZE(wanDhcp)},
{MIB_WAN_DNS_MODE,		"DNS_MODE",		BYTE_T,		_OFFSET(dnsMode), _SIZE(dnsMode)},
{MIB_PPP_USER,			"PPP_USER_NAME",	STRING_T,	_OFFSET(pppUserName), _SIZE(pppUserName)},
{MIB_PPP_PASSWORD,		"PPP_PASSWORD", 	STRING_T,	_OFFSET(pppPassword), _SIZE(pppPassword)},
{MIB_PPP_IDLE_TIME,		"PPP_IDLE_TIME",	WORD_T,		_OFFSET(pppIdleTime), _SIZE(pppIdleTime)},
{MIB_PPP_CONNECT_TYPE,		"PPP_CONNECT_TYPE",	BYTE_T,		_OFFSET(pppConnectType), _SIZE(pppConnectType)},
{MIB_PORTFW_ENABLED,		"PORTFW_ENABLED",	BYTE_T,		_OFFSET(portFwEnabled), _SIZE(portFwEnabled)},
{MIB_PORTFW_NUM,		"PORTFW_TBL_NUM",	BYTE_T,		_OFFSET(portFwNum), _SIZE(portFwNum)},
{MIB_PORTFW,			"PORTFW_TBL",		PORTFW_ARRAY_T,	_OFFSET(portFwArray), _SIZE(portFwArray)},
{MIB_IPFILTER_ENABLED,		"IPFILTER_ENABLED",	BYTE_T,		_OFFSET(ipFilterEnabled), _SIZE(ipFilterEnabled)},
{MIB_IPFILTER_NUM,		"IPFILTER_TBL_NUM",	BYTE_T,		_OFFSET(ipFilterNum), _SIZE(ipFilterNum)},
{MIB_IPFILTER,			"IPFILTER_TBL",		IPFILTER_ARRAY_T,_OFFSET(ipFilterArray), _SIZE(ipFilterArray)},
{MIB_PORTFILTER_ENABLED,	"PORTFILTER_ENABLED",	BYTE_T,		_OFFSET(portFilterEnabled), _SIZE(portFilterEnabled)},
{MIB_PORTFILTER_NUM,		"PORTFILTER_TBL_NUM",	BYTE_T,		_OFFSET(portFilterNum), _SIZE(portFilterNum)},
{MIB_PORTFILTER,		"PORTFILTER_TBL",	PORTFILTER_ARRAY_T,_OFFSET(portFilterArray), _SIZE(portFilterArray)},
{MIB_MACFILTER_ENABLED,		"MACFILTER_ENABLED",	BYTE_T,		_OFFSET(macFilterEnabled), _SIZE(macFilterEnabled)},
{MIB_MACFILTER_NUM,		"MACFILTER_TBL_NUM",	BYTE_T,		_OFFSET(macFilterNum), _SIZE(macFilterNum)},
{MIB_MACFILTER,			"MACFILTER_TBL",	MACFILTER_ARRAY_T,_OFFSET(macFilterArray), _SIZE(macFilterArray)},
{MIB_TRIGGERPORT_ENABLED,	"TRIGGERPORT_ENABLED",	BYTE_T,		_OFFSET(triggerPortEnabled), _SIZE(triggerPortEnabled)},
{MIB_TRIGGERPORT_NUM,		"TRIGGERPORT_TBL_NUM",	BYTE_T,		_OFFSET(triggerPortNum), _SIZE(triggerPortNum)},
{MIB_TRIGGERPORT,		"TRIGGERPORT_TBL",	TRIGGERPORT_ARRAY_T,_OFFSET(triggerPortArray), _SIZE(triggerPortArray)},
{MIB_DMZ_ENABLED,		"DMZ_ENABLED", 		BYTE_T,		_OFFSET(dmzEnabled), _SIZE(dmzEnabled)},
{MIB_DMZ_HOST,			"DMZ_HOST", 		IA_T,		_OFFSET(dmzHost), _SIZE(dmzHost)},
{MIB_UPNP_ENABLED,		"UPNP_ENABLED", 	BYTE_T,		_OFFSET(upnpEnabled), _SIZE(upnpEnabled)},
{MIB_PPP_MTU_SIZE,		"PPP_MTU_SIZE",		WORD_T,		_OFFSET(pppMtuSize), _SIZE(pppMtuSize)},
{MIB_PPTP_IP,			"PPTP_IP_ADDR", 	IA_T,		_OFFSET(pptpIpAddr), _SIZE(pptpIpAddr)},
{MIB_PPTP_SUBNET_MASK,		"PPTP_SUBNET_MASK", 	IA_T, 		_OFFSET(pptpSubnetMask), _SIZE(pptpSubnetMask)},
{MIB_PPTP_SERVER_IP,		"PPTP_SERVER_IP_ADDR",	IA_T,		_OFFSET(pptpServerIpAddr), _SIZE(pptpServerIpAddr)},
{MIB_PPTP_USER,			"PPTP_USER_NAME",	STRING_T,	_OFFSET(pptpUserName), _SIZE(pptpUserName)},
{MIB_PPTP_PASSWORD,		"PPTP_PASSWORD", 	STRING_T,	_OFFSET(pptpPassword), _SIZE(pptpPassword)},
{MIB_PPTP_MTU_SIZE,		"PPTP_MTU_SIZE",	WORD_T,		_OFFSET(pptpMtuSize), _SIZE(pptpMtuSize)},
{MIB_PPTP_SECURITY_ENABLED, "PPTP_SECURITY_ENABLED", BYTE_T, _OFFSET(pptpSecurityEnabled), _SIZE(pptpSecurityEnabled)},
{MIB_PPTP_MPPC_ENABLED, "PPTP_MPPC_ENABLED", BYTE_T, _OFFSET(pptpMppcEnabled), _SIZE(pptpMppcEnabled)},
{MIB_PPTP_CONNECTION_TYPE, 	"PPTP_CONNECTION_TYPE", BYTE_T, _OFFSET(pptpConnectType), _SIZE(pptpConnectType)},
{MIB_PPTP_IDLE_TIME, 		"PPTP_IDLE_TIME", 	WORD_T, _OFFSET(pptpIdleTime), _SIZE(pptpIdleTime)},

/* # keith: add l2tp support. 20080515 */
{MIB_L2TP_IP,			"L2TP_IP_ADDR", 		IA_T,		_OFFSET(l2tpIpAddr), _SIZE(l2tpIpAddr)},
{MIB_L2TP_SUBNET_MASK,		"L2TP_SUBNET_MASK", 	IA_T, 		_OFFSET(l2tpSubnetMask), _SIZE(l2tpSubnetMask)},
{MIB_L2TP_GATEWAY,	"L2TP_GATEWAY",	IA_T,		_OFFSET(l2tpGateway), _SIZE(l2tpGateway)},
{MIB_L2TP_SERVER_IP,		"L2TP_SERVER_IP_ADDR",	IA_T,		_OFFSET(l2tpServerIpAddr), _SIZE(l2tpServerIpAddr)},
{MIB_L2TP_USER,			"L2TP_USER_NAME",	STRING_T,	_OFFSET(l2tpUserName), _SIZE(l2tpUserName)},
{MIB_L2TP_PASSWORD,		"L2TP_PASSWORD", 	STRING_T,	_OFFSET(l2tpPassword), _SIZE(l2tpPassword)},
{MIB_L2TP_MTU_SIZE,		"L2TP_MTU_SIZE",		WORD_T,		_OFFSET(l2tpMtuSize), _SIZE(l2tpMtuSize)},
{MIB_L2TP_CONNECTION_TYPE, 	"L2TP_CONNECTION_TYPE", BYTE_T, _OFFSET(l2tpConnectType), _SIZE(l2tpConnectType)},
{MIB_L2TP_IDLE_TIME, 		"L2TP_IDLE_TIME", 	WORD_T, _OFFSET(l2tpIdleTime), _SIZE(l2tpIdleTime)},
{MIB_L2TP_WAN_IP_DYNAMIC,		"L2TP_WAN_IP_DYNAMIC",	BYTE_T,		_OFFSET(L2tpwanIPMode), _SIZE(L2tpwanIPMode)},

{MIB_FIXED_IP_MTU_SIZE,	"FIXED_IP_MTU_SIZE", WORD_T,	_OFFSET(fixedIpMtuSize), _SIZE(fixedIpMtuSize)},
{MIB_DHCP_MTU_SIZE,		"DHCP_MTU_SIZE", WORD_T,	_OFFSET(dhcpMtuSize), _SIZE(dhcpMtuSize)},
#endif
{MIB_HOST_NAME,			"HOST_NAME", 		STRING_T,	_OFFSET(hostName), _SIZE(hostName)},
#ifdef HOME_GATEWAY
{MIB_NTP_ENABLED, 		"NTP_ENABLED",	BYTE_T, 	_OFFSET(ntpEnabled), _SIZE(ntpEnabled)},
{MIB_NTP_SERVER_ID, 		"NTP_SERVER_ID",	BYTE_T, 	_OFFSET(ntpServerId), _SIZE(ntpServerId)},
{MIB_NTP_TIMEZONE, 		"NTP_TIMEZONE",	STRING_T, 	_OFFSET(ntpTimeZone), _SIZE(ntpTimeZone)},
{MIB_NTP_SERVER_IP1, 		"NTP_SERVER_IP1",	IA_T, 	_OFFSET(ntpServerIp1), _SIZE(ntpServerIp1)},
{MIB_NTP_SERVER_IP2, 		"NTP_SERVER_IP2",	IA_T, 	_OFFSET(ntpServerIp2), _SIZE(ntpServerIp2)},
{MIB_DAYLIGHT_SAVE, 		"DAYLIGHT_SAVE",	BYTE_T, 	_OFFSET(daylightsaveEnabled), _SIZE(daylightsaveEnabled)},
#ifdef VPN_SUPPORT
{MIB_IPSEC_NATT_ENABLED,	"IPSEC_NATT_ENABLED",	BYTE_T,		_OFFSET(ipsecNattEnabled), _SIZE(ipsecNattEnabled)},
{MIB_IPSECTUNNEL_ENABLED,	"IPSECTUNNEL_ENABLED",	BYTE_T,		_OFFSET(ipsecTunnelEnabled), _SIZE(ipsecTunnelEnabled)},
{MIB_IPSECTUNNEL_NUM,		"IPSECTUNNEL_TBL_NUM",	BYTE_T,		_OFFSET(ipsecTunnelNum ), _SIZE(ipsecTunnelNum )},
{MIB_IPSECTUNNEL,		"IPSECTUNNEL_TBL",	IPSECTUNNEL_ARRAY_T,_OFFSET(ipsecTunnelArray), _SIZE(ipsecTunnelArray)},
{MIB_IPSEC_RSA_FILE,	"IPSEC_RSA_FILE",	BYTE_ARRAY_T,		_OFFSET(ipsecRsaKeyFile), _SIZE(ipsecRsaKeyFile)},
#endif
#endif // HOME_GATEWAY
{MIB_WAN_DNS1,			"DNS1",			IA_T,		_OFFSET(dns1), _SIZE(dns1)},
{MIB_WAN_DNS2,			"DNS2", 		IA_T, 		_OFFSET(dns2), _SIZE(dns2)},
{MIB_WAN_DNS3,			"DNS3", 		IA_T,		_OFFSET(dns3), _SIZE(dns3)},
{MIB_DHCP_CLIENT_START,		"DHCP_CLIENT_START", 	IA_T, 		_OFFSET(dhcpClientStart), _SIZE(dhcpClientStart)},
{MIB_DHCP_CLIENT_END,		"DHCP_CLIENT_END", 	IA_T,		_OFFSET(dhcpClientEnd), _SIZE(dhcpClientEnd)},
{MIB_ELAN_MAC_ADDR, 		"ELAN_MAC_ADDR",	BYTE6_T, 	_OFFSET(elanMacAddr), _SIZE(elanMacAddr)},
{MIB_DEVICE_NAME,		"DEVICE_NAME",		STRING_T,	_OFFSET(deviceName), _SIZE(deviceName)},
{MIB_IP_ADDR,			"IP_ADDR", 		IA_T,		_OFFSET(ipAddr), _SIZE(ipAddr)},
{MIB_SUBNET_MASK,		"SUBNET_MASK", 		IA_T,		_OFFSET(subnetMask), _SIZE(subnetMask)},
{MIB_DEFAULT_GATEWAY,		"DEFAULT_GATEWAY",	IA_T,		_OFFSET(defaultGateway), _SIZE(defaultGateway)},
{MIB_DHCP,			"DHCP", 		BYTE_T,		_OFFSET(dhcp), _SIZE(dhcp)},
{MIB_STP_ENABLED,		"STP_ENABLED",		BYTE_T,		_OFFSET(stpEnabled), _SIZE(stpEnabled)},
{MIB_DOMAIN_NAME,		"DOMAIN_NAME",		STRING_T,	_OFFSET(domainName), _SIZE(domainName)},
{MIB_SUPER_NAME,		"SUPER_NAME", 		STRING_T,	_OFFSET(superName), _SIZE(superName)},
{MIB_DHCP_LEASE_TIME,	"DHCP_LEASE_TIME",DWORD_T, 	_OFFSET(dhcpLeaseTime), _SIZE(dhcpLeaseTime)},
{MIB_SUPER_PASSWORD,		"SUPER_PASSWORD",	STRING_T,	_OFFSET(superPassword), _SIZE(superPassword)},
{MIB_USER_NAME,			"USER_NAME",		STRING_T,	_OFFSET(userName), _SIZE(userName)},
{MIB_USER_PASSWORD,		"USER_PASSWORD",	STRING_T,	_OFFSET(userPassword), _SIZE(userPassword)},
{MIB_LOG_ENABLED,		"SCRLOG_ENABLED",	BYTE_T,		_OFFSET(scrlogEnabled), _SIZE(scrlogEnabled)},
{MIB_AUTO_DISCOVERY_ENABLED,	"AUTODISCOVERY_ENABLED",BYTE_T,		_OFFSET(autoDiscoveryEnabled), _SIZE(autoDiscoveryEnabled)},
#ifdef HOME_GATEWAY
{MIB_DDNS_ENABLED,		"DDNS_ENABLED",		BYTE_T,		_OFFSET(ddnsEnabled), _SIZE(ddnsEnabled)},
{MIB_DDNS_TYPE,			"DDNS_TYPE",		BYTE_T,		_OFFSET(ddnsType), _SIZE(ddnsType)},
{MIB_DDNS_DOMAIN_NAME,		"DDNS_DOMAIN_NAME",	STRING_T,	_OFFSET(ddnsDomainName), _SIZE(ddnsDomainName)},
{MIB_DDNS_USER,			"DDNS_USER",		STRING_T,	_OFFSET(ddnsUser), _SIZE(ddnsUser)},
{MIB_DDNS_PASSWORD,		"DDNS_PASSWORD",	STRING_T,	_OFFSET(ddnsPassword), _SIZE(ddnsPassword)},
#endif
{MIB_OP_MODE,			"OP_MODE",		BYTE_T,		_OFFSET(opMode), _SIZE(opMode)},
{MIB_WISP_WAN_ID,		"WISP_WAN_ID",		BYTE_T,		_OFFSET(wispWanId), _SIZE(wispWanId)},
#ifdef TLS_CLIENT
{MIB_CERTROOT_NUM,		"CERTROOT_TBL_NUM",	BYTE_T,		_OFFSET(certRootNum), _SIZE(certRootNum)},
{MIB_CERTROOT,			"CERTROOT_TBL",		CERTROOT_ARRAY_T,_OFFSET(certRootArray), _SIZE(certRootArray)},
{MIB_CERTUSER_NUM,		"CERTUSER_TBL_NUM",	BYTE_T,		_OFFSET(certUserNum), _SIZE(certUserNum)},
{MIB_CERTUSER,			"CERTUSER_TBL",		CERTUSER_ARRAY_T,_OFFSET(certUserArray), _SIZE(certUserArray)},
{MIB_ROOT_IDX,			"ROOT_IDX",		BYTE_T,		_OFFSET(rootIdx), _SIZE(rootIdx)},
{MIB_USER_IDX,			"USER_IDX",		BYTE_T,		_OFFSET(userIdx), _SIZE(userIdx)},
#endif
#ifdef   HOME_GATEWAY
#ifdef ROUTE_SUPPORT
{MIB_STATICROUTE_ENABLED,	"STATICROUTE_ENABLED",	BYTE_T,		_OFFSET(staticRouteEnabled), _SIZE(staticRouteEnabled)},
{MIB_STATICROUTE_NUM,		"STATICROUTE_TBL_NUM",	BYTE_T,		_OFFSET(staticRouteNum), _SIZE(staticRouteNum)},
{MIB_STATICROUTE,		"STATICROUTE_TBL",	STATICROUTE_ARRAY_T,_OFFSET(staticRouteArray), _SIZE(staticRouteArray)},
{MIB_RIP_ENABLED,		"RIP_ENABLED",	BYTE_T, _OFFSET(ripEnabled), _SIZE(ripEnabled)},
{MIB_RIP_LAN_TX,		"RIP_LAN_TX",	BYTE_T, _OFFSET(ripLanTx), _SIZE(ripLanTx)},
{MIB_RIP_LAN_RX,		"RIP_LAN_RX",	BYTE_T, _OFFSET(ripLanRx), _SIZE(ripLanRx)},
{MIB_RIP_WAN_TX,		"RIP_WAN_TX",	BYTE_T, _OFFSET(ripWanTx), _SIZE(ripWanTx)},
{MIB_RIP_WAN_RX,		"RIP_WAN_RX",	BYTE_T, _OFFSET(ripWanRx), _SIZE(ripWanRx)},
#endif
{WEB_WAN_ACCESS_ENABLED,	"WEB_WAN_ACCESS_ENABLED",BYTE_T, _OFFSET(wanAccessEnabled), _SIZE(wanAccessEnabled)},
{PING_WAN_ACCESS_ENABLED,	"PING_WAN_ACCESS_ENABLED",BYTE_T, _OFFSET(pingAccessEnabled), _SIZE(pingAccessEnabled)},
#endif
{MIB_REMOTELOG_ENABLED,		"REMOTELOG_ENABLED",	BYTE_T, _OFFSET(rtLogEnabled), _SIZE(rtLogEnabled)},
{MIB_REMOTELOG_SERVER,		"REMOTELOG_SERVER",	IA_T, _OFFSET(rtLogServer), _SIZE(rtLogServer)},

#ifdef UNIVERSAL_REPEATER
{MIB_REPEATER_ENABLED1,		"REPEATER_ENABLED1",	BYTE_T, 	_OFFSET(repeaterEnabled1), _SIZE(repeaterEnabled1)},
{MIB_REPEATER_SSID1,		"REPEATER_SSID1", 	STRING_T,	_OFFSET(repeaterSSID1), _SIZE(repeaterSSID1)},
{MIB_REPEATER_ENABLED2,		"REPEATER_ENABLED2",	BYTE_T, 	_OFFSET(repeaterEnabled2), _SIZE(repeaterEnabled2)},
{MIB_REPEATER_SSID2,		"REPEATER_SSID2", 	STRING_T,	_OFFSET(repeaterSSID2), _SIZE(repeaterSSID2)},
#endif

{MIB_WIFI_SPECIFIC,		"WIFI_SPECIFIC",	BYTE_T, _OFFSET(wifiSpecific), _SIZE(wifiSpecific)},

{MIB_DHCPRSVDIP_ENABLED,	"DHCPRSVDIP_ENABLED",	BYTE_T,		_OFFSET(dhcpRsvdIpEnabled), _SIZE(dhcpRsvdIpEnabled)},
{MIB_DHCPRSVDIP_NUM,		"DHCPRSVDIP_TBL_NUM",	BYTE_T,		_OFFSET(dhcpRsvdIpNum), _SIZE(dhcpRsvdIpNum)},
{MIB_DHCPRSVDIP,			"DHCPRSVDIP_TBL",	DHCPRSVDIP_ARRY_T,_OFFSET(dhcpRsvdIpArray), _SIZE(dhcpRsvdIpArray)},

#ifdef HOME_GATEWAY
{MIB_PPP_SERVICE,		"PPP_SERVICE_NAME",	STRING_T,	_OFFSET(pppServiceName), _SIZE(pppServiceName)},
#ifdef DOS_SUPPORT
{MIB_DOS_ENABLED, 			"DOS_ENABLED",			DWORD_T, 	_OFFSET(dosEnabled), _SIZE(dosEnabled)},
{MIB_DOS_SYSSYN_FLOOD, 	"DOS_SYSSYN_FLOOD",		WORD_T, 	_OFFSET(syssynFlood), _SIZE(syssynFlood)},
{MIB_DOS_SYSFIN_FLOOD, 		"DOS_SYSFIN_FLOOD",		WORD_T, 	_OFFSET(sysfinFlood), _SIZE(sysfinFlood)},
{MIB_DOS_SYSUDP_FLOOD, 	"DOS_SYSUDP_FLOOD",	WORD_T, 	_OFFSET(sysudpFlood), _SIZE(sysudpFlood)},
{MIB_DOS_SYSICMP_FLOOD, 	"DOS_SYSICMP_FLOOD",	WORD_T, 	_OFFSET(sysicmpFlood), _SIZE(sysicmpFlood)},
{MIB_DOS_PIPSYN_FLOOD, 		"DOS_PIPSYN_FLOOD",		WORD_T, 	_OFFSET(pipsynFlood), _SIZE(pipsynFlood)},
{MIB_DOS_PIPFIN_FLOOD, 		"DOS_PIPFIN_FLOOD",		WORD_T, 	_OFFSET(pipfinFlood), _SIZE(pipfinFlood)},
{MIB_DOS_PIPUDP_FLOOD, 		"DOS_PIPUDP_FLOOD",		WORD_T, 	_OFFSET(pipudpFlood), _SIZE(pipudpFlood)},
{MIB_DOS_PIPICMP_FLOOD, 	"DOS_PIPICMP_FLOOD",	WORD_T, 	_OFFSET(pipicmpFlood), _SIZE(pipicmpFlood)},
{MIB_DOS_BLOCK_TIME, 		"DOS_BLOCK_TIME",		WORD_T, 	_OFFSET(blockTime), _SIZE(blockTime)},
#endif
{MIB_URLFILTER_ENABLED,		"URLFILTER_ENABLED",		BYTE_T,		_OFFSET(urlFilterEnabled), _SIZE(urlFilterEnabled)},
{MIB_URLFILTER_NUM,			"URLFILTER_TBL_NUM",		BYTE_T,		_OFFSET(urlFilterNum), _SIZE(urlFilterNum)},
{MIB_URLFILTER,				"URLFILTER_TBL",			URLFILTER_ARRAY_T,_OFFSET(urlFilterArray), _SIZE(urlFilterArray)},
{VPN_PASSTHRU_IPSEC_ENABLED,	"VPN_PASSTHRU_IPSEC_ENABLED",BYTE_T, 	_OFFSET(vpnPassthruIPsecEnabled), _SIZE(vpnPassthruIPsecEnabled)},
{VPN_PASSTHRU_PPTP_ENABLED,	"VPN_PASSTHRU_PPTP_ENABLED",BYTE_T, 	_OFFSET(vpnPassthruPPTPEnabled), _SIZE(vpnPassthruPPTPEnabled)},
{VPN_PASSTHRU_L2TP_ENABLED,	"VPN_PASSTHRU_L2TP_ENABLED",BYTE_T, 	_OFFSET(vpnPassthruL2TPEnabled), _SIZE(vpnPassthruL2TPEnabled)},
{MIB_IGMP_PROXY_DISABLED,	"IGMP_PROXY_DISABLED",BYTE_T, 	_OFFSET(igmpproxyDisabled), _SIZE(igmpproxyDisabled)},

#if defined(GW_QOS_ENGINE) || defined(QOS_BY_BANDWIDTH)
{MIB_QOS_ENABLED,		"QOS_ENABLED",	       BYTE_T,		       _OFFSET(qosEnabled), _SIZE(qosEnabled)},
{MIB_QOS_AUTO_UPLINK_SPEED,   "QOS_AUTO_UPLINK_SPEED",	BYTE_T,		_OFFSET(qosAutoUplinkSpeed), _SIZE(qosAutoUplinkSpeed)},
{MIB_QOS_MANUAL_UPLINK_SPEED,  "QOS_MANUAL_UPLINK_SPEED",	DWORD_T,		_OFFSET(qosManualUplinkSpeed), _SIZE(qosManualUplinkSpeed)},
{MIB_QOS_AUTO_DOWNLINK_SPEED,  "QOS_AUTO_DOWNLINK_SPEED",	BYTE_T,		_OFFSET(qosAutoDownLinkSpeed), _SIZE(qosAutoDownLinkSpeed)},
{MIB_QOS_MANUAL_DOWNLINK_SPEED,  "QOS_MANUAL_DOWNLINK_SPEED",	DWORD_T,		_OFFSET(qosManualDownLinkSpeed), _SIZE(qosManualDownLinkSpeed)},

{MIB_QOS_RULE_NUM,		       "QOS_RULE_TBL_NUM",	       BYTE_T,		       _OFFSET(qosRuleNum), _SIZE(qosRuleNum)},
{MIB_QOS_RULE,			       "QOS_RULE_TBL",		       QOS_ARRAY_T,	_OFFSET(qosRuleArray), _SIZE(qosRuleArray)},
#endif
#endif // HOME_GATEWAY

{MIB_START_MP_DAEMON,	 "START_MP",		       BYTE_T,	_OFFSET(startMp), _SIZE(startMp)},

/*+++++added by Jack for Tr-069 configuration+++++*/
#ifdef CONFIG_CWMP_TR069
{CWMP_ID,	"CWMP_ID",	 BYTE_T, _OFFSET(cwmp_onoff), _SIZE(cwmp_onoff)},
{CWMP_PROVISIONINGCODE,	"CWMP_PROVISIONINGCODE",	 STRING_T, _OFFSET(cwmp_ProvisioningCode), _SIZE(cwmp_ProvisioningCode)},
{CWMP_ACS_URL, "CWMP_ACS_URL",	STRING_T,	_OFFSET(cwmp_ACSURL),	_SIZE(cwmp_ACSURL)},
{CWMP_ACS_USERNAME, "CWMP_ACS_USERNAME",	STRING_T,		_OFFSET(cwmp_ACSUserName),	_SIZE(cwmp_ACSUserName)},
{CWMP_ACS_PASSWORD, "CWMP_ACS_PASSWORD",	STRING_T,		_OFFSET(cwmp_ACSPassword),	_SIZE(cwmp_ACSPassword)},
{CWMP_INFORM_ENABLE, "CWMP_INFORM_ENABLE",	BYTE_T,		_OFFSET(cwmp_InformEnable),	_SIZE(cwmp_InformEnable)},
{CWMP_INFORM_INTERVAL, "CWMP_INFORM_INTERVAL",	WORD_T,		_OFFSET(cwmp_InformInterval),	_SIZE(cwmp_InformInterval)},
{CWMP_INFORM_TIME,"CWMP_INFORM_TIME",	WORD_T,		_OFFSET(cwmp_InformTime),	_SIZE(cwmp_InformTime)},
{CWMP_CONREQ_USERNAME, "CWMP_CONREQ_USERNAME",	STRING_T,		_OFFSET(cwmp_ConnReqUserName),	_SIZE(cwmp_ConnReqUserName)},
{CWMP_CONREQ_PASSWORD, "CWMP_CONREQ_PASSWORD",	STRING_T,		_OFFSET(cwmp_ConnReqPassword),	_SIZE(cwmp_ConnReqPassword)},
{CWMP_ACS_UPGRADESMANAGED, "CWMP_ACS_UPGRADESMANAGED",	BYTE_T,		_OFFSET(cwmp_UpgradesManaged),	_SIZE(cwmp_UpgradesManaged)},
//{CWMP_LAN_CONFIGPASSWD, "CWMP_LAN_CONFIGPASSWD",	STRING_T,		_OFFSET(cwmp_LANConfPassword),	_SIZE(cwmp_LANConfPassword)},
//{CWMP_SERIALNUMBER, "CWMP_SERIALNUMBER",	STRING_T,		_OFFSET(cwmp_SerialNumber),	_SIZE(cwmp_SerialNumber)},
//{CWMP_DHCP_SERVERCONF,	"CWMP_DHCP_SERVERCONF",	BYTE_T,	_OFFSET(cwmp_DHCP_ServerConf), _SIZE(cwmp_DHCP_ServerConf)},
//{CWMP_LAN_IPIFENABLE,	"CWMP_LAN_IPIFENABLE",	BYTE_T,	_OFFSET(cwmp_LAN_IPIFEnable), _SIZE(cwmp_LAN_IPIFEnable)},
//{CWMP_LAN_ETHIFENABLE,	"CWMP_LAN_ETHIFENABLE",	BYTE_T,	_OFFSET(cwmp_LAN_EthIFEnable), _SIZE(cwmp_LAN_EthIFEnable)},
//{CWMP_WLAN_BASICENCRY,	"CWMP_WLAN_BASICENCRY",	BYTE_T,	_OFFSET(cwmp_WLAN_BasicEncry), _SIZE(cwmp_WLAN_BasicEncry)},
//{CWMP_WLAN_WPAENCRY,	"CWMP_WLAN_WPAENCRY",	BYTE_T,	_OFFSET(cwmp_WLAN_WPAEncry), _SIZE(cwmp_WLAN_WPAEncry)},
{CWMP_DL_COMMANDKEY,	"CWMP_DL_COMMANDKEY",	BYTE_T,	_OFFSET(cwmp_DL_CommandKey), _SIZE(cwmp_DL_CommandKey)},
{CWMP_DL_STARTTIME,	"CWMP_DL_STARTTIME",	WORD_T,	_OFFSET(cwmp_DL_StartTime), _SIZE(cwmp_DL_StartTime)},
{CWMP_DL_COMPLETETIME,	"CWMP_DL_COMPLETETIME",	WORD_T,	_OFFSET(cwmp_DL_CompleteTime), _SIZE(cwmp_DL_CompleteTime)},
{CWMP_DL_FAULTCODE,	"CWMP_DL_FAULTCODE",	WORD_T,	_OFFSET(cwmp_DL_FaultCode), _SIZE(cwmp_DL_FaultCode)},
{CWMP_INFORM_EVENTCODE,	"CWMP_INFORM_EVENTCODE",	WORD_T,	_OFFSET(cwmp_Inform_EventCode), _SIZE(cwmp_Inform_EventCode)},
{CWMP_RB_COMMANDKEY,	"CWMP_RB_COMMANDKEY",	STRING_T,	_OFFSET(cwmp_RB_CommandKey), _SIZE(cwmp_RB_CommandKey)},
//{CWMP_ACS_PARAMETERKEY,	"CWMP_ACS_PARAMETERKEY",	BYTE_T,	_OFFSET(cwmp_ACS_ParameterKey), _SIZE(cwmp_ACS_ParameterKey)},
{CWMP_CERT_PASSWORD,	"CWMP_CERT_PASSWORD",	STRING_T,	_OFFSET(cwmp_CERT_Password), _SIZE(cwmp_CERT_Password)},
{CWMP_FLAG,	"CWMP_FLAG",	BYTE_T,	_OFFSET(cwmp_Flag), _SIZE(cwmp_Flag)},
{CWMP_SI_COMMANDKEY, "CWMP_SI_COMMANDKEY", STRING_T, _OFFSET(cwmp_SI_CommandKey), _SIZE(cwmp_SI_CommandKey)},
#ifdef _PRMT_USERINTERFACE_
{UIF_PW_REQUIRED,			"UIF_PW_REQUIRED",		BYTE_T,			_OFFSET(UIF_PW_Required), 		_SIZE(UIF_PW_Required)},
{UIF_PW_USER_SEL,			"UIF_PW_USER_SEL",		BYTE_T,			_OFFSET(UIF_PW_User_Sel), 		_SIZE(UIF_PW_User_Sel)},
{UIF_UPGRADE,			"UIF_UPGRADE",			BYTE_T,			_OFFSET(UIF_Upgrade), 			_SIZE(UIF_Upgrade)},
{UIF_WARRANTYDATE,			"UIF_WARRANTYDATE",		DWORD_T,		_OFFSET(UIF_WarrantyDate), 		_SIZE(UIF_WarrantyDate)},
{UIF_AUTOUPDATESERVER, "UIF_AUTOUPDATESERVER",		STRING_T,		_OFFSET(UIF_AutoUpdateServer), 		_SIZE(UIF_AutoUpdateServer)},
{UIF_USERUPDATESERVER, "UIF_USERUPDATESERVER",		STRING_T,		_OFFSET(UIF_UserUpdateServer), 		_SIZE(UIF_UserUpdateServer)},
#endif //#ifdef _PRMT_USERINTERFACE_
{CWMP_ACS_KICKURL, "CWMP_ACS_KICKURL", STRING_T, _OFFSET(cwmp_ACS_KickURL), _SIZE(cwmp_ACS_KickURL)},
{CWMP_ACS_DOWNLOADURL, "CWMP_ACS_DOWNLOADURL",		STRING_T,		_OFFSET(cwmp_ACS_DownloadURL), 		_SIZE(cwmp_ACS_DownloadURL)},
{CWMP_CONREQ_PORT,			"CWMP_CONREQ_PORT",		DWORD_T,		_OFFSET(cwmp_ConnReqPort), 		_SIZE(cwmp_ConnReqPort)},
{CWMP_CONREQ_PATH,			"CWMP_CONREQ_PATH",		STRING_T,		_OFFSET(cwmp_ConnReqPath), 		_SIZE(cwmp_ConnReqPath)},
#ifdef _PRMT_TR143_
{TR143_UDPECHO_ENABLE, "TR143_UDPECHO_ENABLE",		BYTE_T,			_OFFSET(tr143_udpecho_enable), 		_SIZE(tr143_udpecho_enable)},
{TR143_UDPECHO_ITFTYPE, "TR143_UDPECHO_ITFTYPE",	BYTE_T,			_OFFSET(tr143_udpecho_itftype),		_SIZE(tr143_udpecho_itftype)},
{TR143_UDPECHO_SRCIP, "TR143_UDPECHO_SRCIP",		IA_T,			_OFFSET(tr143_udpecho_srcip), 		_SIZE(tr143_udpecho_srcip)},
{TR143_UDPECHO_PORT, "TR143_UDPECHO_PORT",		WORD_T,			_OFFSET(tr143_udpecho_port), 		_SIZE(tr143_udpecho_port)},
{TR143_UDPECHO_PLUS, "TR143_UDPECHO_PLUS",		BYTE_T,			_OFFSET(tr143_udpecho_plus), 		_SIZE(tr143_udpecho_plus)},
#endif //_PRMT_TR143_
#ifdef VOIP_SUPPORT /* Keith add for tr069 --start */
{MIB_VOIP_CFG,	"MIB_VOIP_CFG", BYTE_ARRAY_T, _OFFSET(voipCfgParam), _SIZE(voipCfgParam)},
#endif
#endif /*CONFIG_CWMP_TR069*/

// SNMP, Forrest added, 2007.10.25.     
#ifdef CONFIG_SNMP
{MIB_SNMP_ENABLED,			"SNMP_ENABLED",			BYTE_T,		_OFFSET(snmpEnabled),		_SIZE(snmpEnabled)},
{MIB_SNMP_NAME,				"SNMP_NAME",			STRING_T,	_OFFSET(snmpName),			_SIZE(snmpName)},
{MIB_SNMP_LOCATION,			"SNMP_LOCATION",		STRING_T,	_OFFSET(snmpLocation),		_SIZE(snmpLocation)},
{MIB_SNMP_CONTACT,			"SNMP_CONTACT",			STRING_T,	_OFFSET(snmpContact),		_SIZE(snmpContact)},
{MIB_SNMP_RWCOMMUNITY,		"SNMP_RWCOMMUNITY",		STRING_T,	_OFFSET(snmpRWCommunity),	_SIZE(snmpRWCommunity)},
{MIB_SNMP_ROCOMMUNITY,		"SNMP_ROCOMMUNITY",		STRING_T,	_OFFSET(snmpROCommunity),	_SIZE(snmpROCommunity)},
{MIB_SNMP_TRAP_RECEIVER1,	"SNMP_TRAP_RECEIVER1",	IA_T,		_OFFSET(snmpTrapReceiver1),	_SIZE(snmpTrapReceiver1)},
{MIB_SNMP_TRAP_RECEIVER2,	"SNMP_TRAP_RECEIVER2",	IA_T,		_OFFSET(snmpTrapReceiver2),	_SIZE(snmpTrapReceiver2)},
{MIB_SNMP_TRAP_RECEIVER3,	"SNMP_TRAP_RECEIVER3",	IA_T,		_OFFSET(snmpTrapReceiver3),	_SIZE(snmpTrapReceiver3)},
#endif

{MIB_TIME_YEAR, 		"SYSTIME_YEAR",		WORD_T, _OFFSET(system_time_year), _SIZE(system_time_year)},
{MIB_TIME_MONTH, 	"SYSTIME_MON",		BYTE_T, 	_OFFSET(system_time_month), _SIZE(system_time_month)},
{MIB_TIME_DAY, 		"SYSTIME_DAY",		BYTE_T, 	_OFFSET(system_time_day), _SIZE(system_time_day)},
{MIB_TIME_HOUR,		"SYSTIME_HOUR", 	BYTE_T,	_OFFSET(system_time_hour), _SIZE(system_time_hour)},
{MIB_TIME_MIN,		"SYSTIME_MIN", 	BYTE_T,	_OFFSET(system_time_min), _SIZE(system_time_min)},
{MIB_TIME_SEC,		"SYSTIME_SEC", 	BYTE_T,	_OFFSET(system_time_sec), _SIZE(system_time_sec)},

#ifdef CONFIG_RTK_MESH 
//new feature:Mesh enable/disable
{MIB_WLAN_MESH_ENABLE,          "MESH_ENABLE",			BYTE_T,		_OFFSET(meshEnabled),               _SIZE(meshEnabled)},
{MIB_MESH_ROOT_ENABLE,          "MESH_ROOT_ENABLE",			BYTE_T,		_OFFSET(meshRootEnabled),               _SIZE(meshRootEnabled)},
{MIB_MESH_ID,					"MESH_ID",					STRING_T,	_OFFSET(meshID),                                _SIZE(meshID)},
{MIB_MESH_MAX_NEIGHTBOR,        "MESH_MAX_NEIGHTBOR",		WORD_T,		_OFFSET(meshMaxNumOfNeighbors), _SIZE(meshMaxNumOfNeighbors)},
{MIB_MESH_ENCRYPT,				"MESH_ENCRYPT",				BYTE_T,		_OFFSET(meshEncrypt), _SIZE(meshEncrypt)},
{MIB_MESH_WPA_PSK_FORMAT,		"MESH_PSK_FORMAT",			BYTE_T,		_OFFSET(meshWpaPSKFormat), _SIZE(meshWpaPSKFormat)},
{MIB_MESH_WPA_PSK,				"MESH_WPA_PSK",				STRING_T,	_OFFSET(meshWpaPSK), _SIZE(meshWpaPSK)},
{MIB_MESH_WPA_AUTH,				"MESH_WPA_AUTH",			BYTE_T,		_OFFSET(meshWpaAuth), _SIZE(meshWpaAuth)},
{MIB_MESH_WPA2_CIPHER_SUITE,	"MESH_WPA2_CIPHER_SUITE",	BYTE_T,		_OFFSET(meshWpa2Cipher), _SIZE(meshWpa2Cipher)},

#ifdef	_MESH_ACL_ENABLE_ 
{MIB_MESH_ACL_ENABLED,			"MESH_ACL_ENABLED", 		BYTE_T, 			_OFFSET(meshAclEnabled),	_SIZE(meshAclEnabled)},
{MIB_MESH_ACL_NUM,				"MESH_ACL_NUM", 			BYTE_T, 			_OFFSET(meshAclNum),		_SIZE(meshAclNum)},
{MIB_MESH_ACL_ADDR, 			"MESH_ACL_ADDR",			MESH_ACL_ARRAY_T,	_OFFSET(meshAclAddrArray),	_SIZE(meshAclAddrArray)},
#endif

#ifdef 	_11s_TEST_MODE_	
{MIB_MESH_TEST_PARAM1,			"MESH_TEST_PARAM1",			WORD_T,		_OFFSET(meshTestParam1),                   _SIZE(meshTestParam1)},
{MIB_MESH_TEST_PARAM2,			"MESH_TEST_PARAM2",			WORD_T,		_OFFSET(meshTestParam2),                   _SIZE(meshTestParam2)},
{MIB_MESH_TEST_PARAM3,			"MESH_TEST_PARAM3",			WORD_T,		_OFFSET(meshTestParam3),                   _SIZE(meshTestParam3)},
{MIB_MESH_TEST_PARAM4,			"MESH_TEST_PARAM4",			WORD_T,		_OFFSET(meshTestParam4),                   _SIZE(meshTestParam4)},
{MIB_MESH_TEST_PARAM5,			"MESH_TEST_PARAM5",			WORD_T,		_OFFSET(meshTestParam5),                   _SIZE(meshTestParam5)},
{MIB_MESH_TEST_PARAM6,			"MESH_TEST_PARAM6",			WORD_T,		_OFFSET(meshTestParam6),                   _SIZE(meshTestParam6)},
{MIB_MESH_TEST_PARAM7,			"MESH_TEST_PARAM7",			WORD_T,		_OFFSET(meshTestParam7),                   _SIZE(meshTestParam7)},
{MIB_MESH_TEST_PARAM8,			"MESH_TEST_PARAM8",			WORD_T,		_OFFSET(meshTestParam8),                   _SIZE(meshTestParam8)},
{MIB_MESH_TEST_PARAM9,			"MESH_TEST_PARAM9",			WORD_T,		_OFFSET(meshTestParam9),                   _SIZE(meshTestParam9)},
{MIB_MESH_TEST_PARAMA,			"MESH_TEST_PARAMA",			WORD_T,		_OFFSET(meshTestParama),                   _SIZE(meshTestParama)},
{MIB_MESH_TEST_PARAMB,			"MESH_TEST_PARAMB",			WORD_T,		_OFFSET(meshTestParamb),                   _SIZE(meshTestParamb)},
{MIB_MESH_TEST_PARAMC,			"MESH_TEST_PARAMC",			WORD_T,		_OFFSET(meshTestParamc),                   _SIZE(meshTestParamc)},
{MIB_MESH_TEST_PARAMD,			"MESH_TEST_PARAMD",			WORD_T,		_OFFSET(meshTestParamd),                   _SIZE(meshTestParamd)},
{MIB_MESH_TEST_PARAME,			"MESH_TEST_PARAME",			WORD_T,		_OFFSET(meshTestParame),                   _SIZE(meshTestParame)},
{MIB_MESH_TEST_PARAMF,			"MESH_TEST_PARAMF",			WORD_T,		_OFFSET(meshTestParamf),                   _SIZE(meshTestParamf)},
{MIB_MESH_TEST_PARAMSTR1,		"MESH_TEST_PARAMSTR1",		STRING_T,	_OFFSET(meshTestParamStr1),                _SIZE(meshTestParamStr1)},

#endif

#endif // CONFIG_RTK_MESH

#ifdef SNMP_SUPPORT
{MIB_SNMP_RO_COMMUNITY,			"SNMP_RO_COMMUNITY",		STRING_T,	_OFFSET(snmpROcommunity), _SIZE(snmpROcommunity)},
{MIB_SNMP_RW_COMMUNITY,			"SNMP_RW_COMMUNITY",		STRING_T,	_OFFSET(snmpRWcommunity), _SIZE(snmpRWcommunity)},
#endif
/*-----end-----*/

{MIB_SCHEDULE_ENABLED,		"SCHEDULE_ENABLED",	BYTE_T,		_OFFSET(scheduleRuleEnabled), _SIZE(scheduleRuleEnabled)},
{MIB_SCHEDULE_NUM,		"SCHEDULE_TBL_NUM",	BYTE_T,		_OFFSET(scheduleRuleNum), _SIZE(scheduleRuleNum)},
{MIB_SCHEDULE,			"SCHEDULE_TBL",	SCHEDULE_ARRAY_T,	_OFFSET(scheduleRuleArray), _SIZE(scheduleRuleArray)},
#ifdef HOME_GATEWAY
#if defined(VLAN_CONFIG_SUPPORTED) 
{MIB_VLANCONFIG_ENABLED,	"VLANCONFIG_ENABLED",	BYTE_T,		_OFFSET(VlanConfigEnabled), _SIZE(VlanConfigEnabled)},
{MIB_VLANCONFIG_NUM,		"VLANCONFIG_TBL_NUM",	BYTE_T,		_OFFSET(VlanConfigNum), _SIZE(VlanConfigNum)},
{MIB_VLANCONFIG,			"VLANCONFIG_TBL",	VLANCONFIG_ARRAY_T,_OFFSET(VlanConfigArray), _SIZE(VlanConfigArray)},
#endif
#ifdef ROUTE_SUPPORT
{MIB_NAT_ENABLED,          "NAT_ENABLED",			BYTE_T,		_OFFSET(natEnabled),               _SIZE(natEnabled)},
#endif
#endif
{MIB_WLAN_11N_ONOFF_TKIP,	"WLAN_11N_ONOFF_TKIP",	BYTE_T,		_OFFSET(wlan11nOnOffTKIP), _SIZE(wlan11nOnOffTKIP)},
{MIB_VPN_PASSTHRU_IPV6,	"VPN_PASSTHRU_IPV6",BYTE_T, 	_OFFSET(vpnPassthruIPv6), _SIZE(vpnPassthruIPv6)},
{0}
};


/*
 * When using flash (set/get/all) command to access the MIB of below table,
 * it needs append a keyword "WLANx_" in ahead of mib name.
 * When access default setting, it needs appened a keyword "DEF_" in front of
 * "WLANx_" keyword.
 */

mib_table_entry_T mib_wlan_table[]={
{MIB_WLAN_MAC_ADDR, 		"WLAN_MAC_ADDR",	BYTE6_T, 	_OFFSET_WLAN(wlanMacAddr), _SIZE_WLAN(wlanMacAddr)},
{MIB_WLAN_SSID,			"SSID", 		STRING_T,	_OFFSET_WLAN(ssid), _SIZE_WLAN(ssid)},
{MIB_WLAN_CHAN_NUM,		"CHANNEL", 		BYTE_T, 	_OFFSET_WLAN(channel), _SIZE_WLAN(channel)},
{MIB_WLAN_WEP,			"WEP", 			BYTE_T,		_OFFSET_WLAN(wep), _SIZE_WLAN(wep)},
//{MIB_WLAN_WEP64_KEY,		"WEP64_KEY", 		BYTE5_T,	_OFFSET_WLAN(wep64Key), _SIZE_WLAN(wep64Key)},
//{MIB_WLAN_WEP128_KEY,		"WEP128_KEY", 		BYTE13_T,	_OFFSET_WLAN(wep128Key), _SIZE_WLAN(wep128Key)},
{MIB_WLAN_WEP64_KEY1,		"WEP64_KEY1", 		BYTE5_T,	_OFFSET_WLAN(wep64Key1), _SIZE_WLAN(wep64Key1)},
{MIB_WLAN_WEP64_KEY2,		"WEP64_KEY2", 		BYTE5_T,	_OFFSET_WLAN(wep64Key2), _SIZE_WLAN(wep64Key2)},
{MIB_WLAN_WEP64_KEY3,		"WEP64_KEY3", 		BYTE5_T,	_OFFSET_WLAN(wep64Key3), _SIZE_WLAN(wep64Key3)},
{MIB_WLAN_WEP64_KEY4,		"WEP64_KEY4", 		BYTE5_T,	_OFFSET_WLAN(wep64Key4), _SIZE_WLAN(wep64Key4)},
{MIB_WLAN_WEP128_KEY1,		"WEP128_KEY1", 		BYTE13_T,	_OFFSET_WLAN(wep128Key1), _SIZE_WLAN(wep128Key1)},
{MIB_WLAN_WEP128_KEY2,		"WEP128_KEY2", 		BYTE13_T,	_OFFSET_WLAN(wep128Key2), _SIZE_WLAN(wep128Key2)},
{MIB_WLAN_WEP128_KEY3,		"WEP128_KEY3", 		BYTE13_T,	_OFFSET_WLAN(wep128Key3), _SIZE_WLAN(wep128Key3)},
{MIB_WLAN_WEP128_KEY4,		"WEP128_KEY4", 		BYTE13_T,	_OFFSET_WLAN(wep128Key4), _SIZE_WLAN(wep128Key4)},
{MIB_WLAN_WEP_DEFAULT_KEY,	"WEP_DEFAULT_KEY", 	BYTE_T,		_OFFSET_WLAN(wepDefaultKey), _SIZE_WLAN(wepDefaultKey)},
{MIB_WLAN_WEP_KEY_TYPE,		"WEP_KEY_TYPE", 	BYTE_T,		_OFFSET_WLAN(wepKeyType), _SIZE_WLAN(wepKeyType)},
{MIB_WLAN_FRAG_THRESHOLD,	"FRAG_THRESHOLD", 	WORD_T,		_OFFSET_WLAN(fragThreshold), _SIZE_WLAN(fragThreshold)},
{MIB_WLAN_SUPPORTED_RATE,	"SUPPORTED_RATES", 	WORD_T,		_OFFSET_WLAN(supportedRates), _SIZE_WLAN(supportedRates)},
{MIB_WLAN_BEACON_INTERVAL,	"BEACON_INTERVAL", 	WORD_T,		_OFFSET_WLAN(beaconInterval), _SIZE_WLAN(beaconInterval)},
{MIB_WLAN_PREAMBLE_TYPE,	"PREAMBLE_TYPE", 	BYTE_T,		_OFFSET_WLAN(preambleType), _SIZE_WLAN(preambleType)},
{MIB_WLAN_BASIC_RATE,		"BASIC_RATES", 		WORD_T, 	_OFFSET_WLAN(basicRates), _SIZE_WLAN(basicRates)},
{MIB_WLAN_RTS_THRESHOLD,	"RTS_THRESHOLD", 	WORD_T, 	_OFFSET_WLAN(rtsThreshold), _SIZE_WLAN(rtsThreshold)},
{MIB_WLAN_AUTH_TYPE,		"AUTH_TYPE", 		BYTE_T,		_OFFSET_WLAN(authType), _SIZE_WLAN(authType)},
{MIB_WLAN_HIDDEN_SSID,		"HIDDEN_SSID", 		BYTE_T,		_OFFSET_WLAN(hiddenSSID), _SIZE_WLAN(hiddenSSID)},
{MIB_WLAN_DISABLED,		"WLAN_DISABLED", 	BYTE_T,		_OFFSET_WLAN(wlanDisabled), _SIZE_WLAN(wlanDisabled)},
{MIB_WLAN_INACTIVITY_TIME,	"INACTIVITY_TIME",	DWORD_T,	_OFFSET_WLAN(inactivityTime), _SIZE_WLAN(inactivityTime)},
{MIB_WLAN_RATE_ADAPTIVE_ENABLED,"RATE_ADAPTIVE_ENABLED",BYTE_T,		_OFFSET_WLAN(rateAdaptiveEnabled), _SIZE_WLAN(rateAdaptiveEnabled)},
{MIB_WLAN_DTIM_PERIOD,		"DTIM_PERIOD", 		BYTE_T,		_OFFSET_WLAN(dtimPeriod), _SIZE_WLAN(dtimPeriod)},
{MIB_WLAN_MODE,			"MODE", 		BYTE_T,		_OFFSET_WLAN(wlanMode), _SIZE_WLAN(wlanMode)},
{MIB_WLAN_NETWORK_TYPE,		"NETWORK_TYPE",		BYTE_T,		_OFFSET_WLAN(networkType), _SIZE_WLAN(networkType)},
{MIB_WLAN_IAPP_DISABLED,	"IAPP_DISABLED",	BYTE_T,		_OFFSET_WLAN(iappDisabled), _SIZE_WLAN(iappDisabled)},
{MIB_WLAN_PROTECTION_DISABLED,	"PROTECTION_DISABLED",	BYTE_T,		_OFFSET_WLAN(protectionDisabled), _SIZE_WLAN(protectionDisabled)},
{MIB_WLAN_DEFAULT_SSID,		"DEFAULT_SSID", 	STRING_T,	_OFFSET_WLAN(defaultSsid), _SIZE_WLAN(defaultSsid)},
#if !defined(CONFIG_RTL8196C_CLIENT_ONLY)
{MIB_WLAN_WDS_ENABLED,		"WDS_ENABLED",		BYTE_T,		_OFFSET_WLAN(wdsEnabled), _SIZE_WLAN(wdsEnabled)},
{MIB_WLAN_WDS_NUM,		"WDS_NUM",		BYTE_T,		_OFFSET_WLAN(wdsNum), _SIZE_WLAN(wdsNum)},
{MIB_WLAN_WDS,			"WDS",			WDS_ARRAY_T,	_OFFSET_WLAN(wdsArray), _SIZE_WLAN(wdsArray)},
{MIB_WLAN_WDS_ENCRYPT,		"WDS_ENCRYPT", 		BYTE_T,		_OFFSET_WLAN(wdsEncrypt), _SIZE_WLAN(wdsEncrypt)},
{MIB_WLAN_WDS_WEP_FORMAT,	"WDS_WEP_FORMAT", 	BYTE_T,		_OFFSET_WLAN(wdsWepKeyFormat), _SIZE_WLAN(wdsWepKeyFormat)},
{MIB_WLAN_WDS_WEP_KEY,		"WDS_WEP_KEY",		STRING_T,	_OFFSET_WLAN(wdsWepKey), _SIZE_WLAN(wdsWepKey)},
{MIB_WLAN_WDS_PSK_FORMAT,	"WDS_PSK_FORMAT", 	BYTE_T,		_OFFSET_WLAN(wdsPskFormat), _SIZE_WLAN(wdsPskFormat)},
{MIB_WLAN_WDS_PSK,		"WDS_PSK",		STRING_T,	_OFFSET_WLAN(wdsPsk), _SIZE_WLAN(wdsPsk)},
#endif
{MIB_WLAN_ENCRYPT,		"ENCRYPT",		BYTE_T,		_OFFSET_WLAN(encrypt), _SIZE_WLAN(encrypt)},
{MIB_WLAN_ENABLE_SUPP_NONWPA,	"ENABLE_SUPP_NONWPA", 	BYTE_T,		_OFFSET_WLAN(enableSuppNonWpa), _SIZE_WLAN(enableSuppNonWpa)},
{MIB_WLAN_SUPP_NONWPA,		"SUPP_NONWPA", 		BYTE_T,		_OFFSET_WLAN(suppNonWpa), _SIZE_WLAN(suppNonWpa)},
{MIB_WLAN_WPA_AUTH,		"WPA_AUTH", 		BYTE_T,		_OFFSET_WLAN(wpaAuth), _SIZE_WLAN(wpaAuth)},
{MIB_WLAN_WPA_CIPHER_SUITE,	"WPA_CIPHER_SUITE",	BYTE_T,		_OFFSET_WLAN(wpaCipher), _SIZE_WLAN(wpaCipher)},
{MIB_WLAN_WPA_PSK,		"WPA_PSK",		STRING_T,	_OFFSET_WLAN(wpaPSK), _SIZE_WLAN(wpaPSK)},
{MIB_WLAN_WPA_GROUP_REKEY_TIME,	"WPA_GROUP_REKEY_TIME",	DWORD_T,	_OFFSET_WLAN(wpaGroupRekeyTime), _SIZE_WLAN(wpaGroupRekeyTime)},
{MIB_WLAN_ENABLE_MAC_AUTH,	"MAC_AUTH_ENABLED",     BYTE_T,         _OFFSET_WLAN(macAuthEnabled), _SIZE_WLAN(macAuthEnabled)},
{MIB_WLAN_RS_IP,		"RS_IP",		IA_T,		_OFFSET_WLAN(rsIpAddr), _SIZE_WLAN(rsIpAddr)},
{MIB_WLAN_RS_PORT,		"RS_PORT",		WORD_T,		_OFFSET_WLAN(rsPort), _SIZE_WLAN(rsPort)},
{MIB_WLAN_RS_PASSWORD,		"RS_PASSWORD",		STRING_T,	_OFFSET_WLAN(rsPassword), _SIZE_WLAN(rsPassword)},
{MIB_WLAN_RS_RETRY,		"RS_MAXRETRY",		BYTE_T,		_OFFSET_WLAN(rsMaxRetry), _SIZE_WLAN(rsMaxRetry)},
{MIB_WLAN_RS_INTERVAL_TIME,	"RS_INTERVAL_TIME",	WORD_T,		_OFFSET_WLAN(rsIntervalTime), _SIZE_WLAN(rsIntervalTime)},
{MIB_WLAN_ACCOUNT_RS_ENABLED,	"ACCOUNT_RS_ENABLED", 	BYTE_T,		_OFFSET_WLAN(accountRsEnabled), _SIZE_WLAN(accountRsEnabled)},
{MIB_WLAN_ACCOUNT_RS_IP,	"ACCOUNT_RS_IP",	IA_T,		_OFFSET_WLAN(accountRsIpAddr), _SIZE_WLAN(accountRsIpAddr)},
{MIB_WLAN_ACCOUNT_RS_PORT,	"ACCOUNT_RS_PORT",	WORD_T,		_OFFSET_WLAN(accountRsPort), _SIZE_WLAN(accountRsPort)},
{MIB_WLAN_ACCOUNT_RS_PASSWORD,	"ACCOUNT_RS_PASSWORD",	STRING_T,	_OFFSET_WLAN(accountRsPassword), _SIZE_WLAN(accountRsPassword)},
{MIB_WLAN_ACCOUNT_UPDATE_ENABLED,"ACCOUNT_RS_UPDATE_ENABLED",BYTE_T,	_OFFSET_WLAN(accountRsUpdateEnabled), _SIZE_WLAN(accountRsUpdateEnabled)},
{MIB_WLAN_ACCOUNT_UPDATE_DELAY,"ACCOUNT_RS_UPDATE_DELAY",WORD_T,	_OFFSET_WLAN(accountRsUpdateDelay), _SIZE_WLAN(accountRsUpdateDelay)},
{MIB_WLAN_ACCOUNT_RS_RETRY,	"ACCOUNT_RS_MAXRETRY",	BYTE_T,		_OFFSET_WLAN(accountRsMaxRetry), _SIZE_WLAN(accountRsMaxRetry)},
{MIB_WLAN_ACCOUNT_RS_INTERVAL_TIME, "ACCOUNT_RS_INTERVAL_TIME",	WORD_T,	_OFFSET_WLAN(accountRsIntervalTime), _SIZE_WLAN(accountRsIntervalTime)},
{MIB_WLAN_ENABLE_1X,		"ENABLE_1X", 		BYTE_T,		_OFFSET_WLAN(enable1X), _SIZE_WLAN(enable1X)},
{MIB_WLAN_WPA_PSK_FORMAT,	"PSK_FORMAT", 		BYTE_T,		_OFFSET_WLAN(wpaPSKFormat), _SIZE_WLAN(wpaPSKFormat)},
{MIB_WLAN_WPA2_PRE_AUTH,	"WPA2_PRE_AUTH",	BYTE_T,         _OFFSET_WLAN(wpa2PreAuth), _SIZE_WLAN(wpa2PreAuth)},
{MIB_WLAN_WPA2_CIPHER_SUITE,	"WPA2_CIPHER_SUITE",	BYTE_T,		_OFFSET_WLAN(wpa2Cipher), _SIZE_WLAN(wpa2Cipher)},
#if !defined(CONFIG_RTL8196C_CLIENT_ONLY)
{MIB_WLAN_AC_NUM, 		"MACAC_NUM",		BYTE_T,		_OFFSET_WLAN(acNum), _SIZE_WLAN(acNum)},
{MIB_WLAN_AC_ENABLED,		"MACAC_ENABLED",	BYTE_T,		_OFFSET_WLAN(acEnabled), _SIZE_WLAN(acEnabled)},
{MIB_WLAN_AC_ADDR,		"MACAC_ADDR",		WLAC_ARRAY_T,  _OFFSET_WLAN(acAddrArray), _SIZE_WLAN(acAddrArray)},
#endif
{MIB_WLAN_BLOCK_RELAY,		"BLOCK_RELAY",		BYTE_T,		_OFFSET_WLAN(blockRelay), _SIZE_WLAN(blockRelay)},
{MIB_WLAN_NAT25_MAC_CLONE,	"MACCLONE_ENABLED",	BYTE_T,		_OFFSET_WLAN(maccloneEnabled), _SIZE_WLAN(maccloneEnabled)},
{MIB_WLAN_BAND,			"BAND",			BYTE_T,		_OFFSET_WLAN(wlanBand), _SIZE_WLAN(wlanBand)},
{MIB_WLAN_FIX_RATE,		"FIX_RATE",		DWORD_T,		_OFFSET_WLAN(fixedTxRate), _SIZE_WLAN(fixedTxRate)},
{MIB_TURBO_MODE,		"TURBO_MODE",	BYTE_T, _OFFSET_WLAN(turboMode), _SIZE_WLAN(turboMode)},
{MIB_WLAN_RF_POWER,		"RFPOWER_SCALE", BYTE_T, _OFFSET_WLAN(RFPowerScale), _SIZE_WLAN(RFPowerScale)},
// for WMM
{MIB_WLAN_WMM_ENABLED,	"WMM_ENABLED", BYTE_T, _OFFSET_WLAN(wmmEnabled), _SIZE_WLAN(wmmEnabled)},

#ifdef WLAN_EASY_CONFIG
{MIB_WLAN_EASYCFG_ENABLED,	"EASYCFG_ENABLED",	BYTE_T,		_OFFSET_WLAN(acfEnabled), _SIZE_WLAN(acfEnabled)},
{MIB_WLAN_EASYCFG_MODE,		"EASYCFG_MODE",		BYTE_T,		_OFFSET_WLAN(acfMode), _SIZE_WLAN(acfMode)},
{MIB_WLAN_EASYCFG_SSID,		"EASYCFG_SSID",		STRING_T,	_OFFSET_WLAN(acfSSID), _SIZE_WLAN(acfSSID)},
{MIB_WLAN_EASYCFG_KEY,		"EASYCFG_KEY",		STRING_T,	_OFFSET_WLAN(acfKey), _SIZE_WLAN(acfKey)},
{MIB_WLAN_EASYCFG_DIGEST,	"EASYCFG_DIGEST",	STRING_T,	_OFFSET_WLAN(acfDigest), _SIZE_WLAN(acfDigest)},
{MIB_WLAN_EASYCFG_ALG_REQ,	"EASYCFG_ALG_REQ",	BYTE_T,		_OFFSET_WLAN(acfAlgReq), _SIZE_WLAN(acfAlgReq)},
{MIB_WLAN_EASYCFG_ALG_SUPP,	"EASYCFG_ALG_SUPP",	BYTE_T,		_OFFSET_WLAN(acfAlgSupp), _SIZE_WLAN(acfAlgSupp)},
{MIB_WLAN_EASYCFG_ROLE,		"EASYCFG_ROLE",		BYTE_T,		_OFFSET_WLAN(acfRole), _SIZE_WLAN(acfRole)},
{MIB_WLAN_EASYCFG_SCAN_SSID,	"EASYCFG_SCAN_SSID",	STRING_T,	_OFFSET_WLAN(acfScanSSID), _SIZE_WLAN(acfScanSSID)},
{MIB_WLAN_EASYCFG_WLAN_MODE,	"EASYCFG_WLAN_MODE",	BYTE_T,		_OFFSET_WLAN(acfWlanMode), _SIZE_WLAN(acfWlanMode)},
#endif

#ifdef WIFI_SIMPLE_CONFIG
{MIB_WSC_DISABLE,		"WSC_DISABLE",		BYTE_T,		_OFFSET_WLAN(wscDisable), _SIZE_WLAN(wscDisable)},
{MIB_WSC_METHOD,		"WSC_METHOD",		BYTE_T,		_OFFSET_WLAN(wscMethod), _SIZE_WLAN(wscMethod)},
{MIB_WSC_CONFIGURED,	"WSC_CONFIGURED",	BYTE_T,		_OFFSET_WLAN(wscConfigured), _SIZE_WLAN(wscConfigured)},
{MIB_WSC_AUTH,			"WSC_AUTH",			BYTE_T,		_OFFSET_WLAN(wscAuth), _SIZE_WLAN(wscAuth)},
{MIB_WSC_ENC,			"WSC_ENC",			BYTE_T,		_OFFSET_WLAN(wscEnc), _SIZE_WLAN(wscEnc)},
{MIB_WSC_MANUAL_ENABLED, "WSC_MANUAL_ENABLED",	BYTE_T,	_OFFSET_WLAN(wscManualEnabled), _SIZE_WLAN(wscManualEnabled)},
{MIB_WSC_UPNP_ENABLED, "WSC_UPNP_ENABLED",	BYTE_T,	_OFFSET_WLAN(wscUpnpEnabled), _SIZE_WLAN(wscUpnpEnabled)},
{MIB_WSC_REGISTRAR_ENABLED, "WSC_REGISTRAR_ENABLED",	BYTE_T,	_OFFSET_WLAN(wscRegistrarEnabled), _SIZE_WLAN(wscRegistrarEnabled)},
{MIB_WSC_PSK,			"WSC_PSK",			STRING_T,		_OFFSET_WLAN(wscPsk), _SIZE_WLAN(wscPsk)},
{MIB_WSC_SSID,			"WSC_SSID",			STRING_T,		_OFFSET_WLAN(wscSsid), _SIZE_WLAN(wscSsid)},
{MIB_WSC_CONFIG_BY_EXT_REG,		"WSC_CONFIGBYEXTREG",	BYTE_T,		_OFFSET_WLAN(wscConfigByExtReg), _SIZE_WLAN(wscConfigByExtReg)},
#endif

{MIB_WLAN_ACCESS,	"ACCESS",		BYTE_T,	_OFFSET_WLAN(access), _SIZE_WLAN(access)},
{MIB_WLAN_PRIORITY,	"PRIORITY",	BYTE_T,	_OFFSET_WLAN(priority), _SIZE_WLAN(priority)},

//for 11N
{MIB_WLAN_CHANNEL_BONDING,	"CHANNEL_BONDING", BYTE_T, _OFFSET_WLAN(channelbonding), _SIZE_WLAN(channelbonding)},
{MIB_WLAN_CONTROL_SIDEBAND,	"CONTROL_SIDEBAND", BYTE_T, _OFFSET_WLAN(controlsideband), _SIZE_WLAN(controlsideband)},
{MIB_WLAN_AGGREGATION,	"AGGREGATION", BYTE_T, _OFFSET_WLAN(aggregation), _SIZE_WLAN(aggregation)},
{MIB_WLAN_SHORT_GI,	"SHORT_GI", BYTE_T, _OFFSET_WLAN(shortgiEnabled), _SIZE_WLAN(shortgiEnabled)},
#if CONFIG_RTL_WAPI_SUPPORT
{MIB_WLAN_WAPI_PSK,	"WAPI_PSK", STRING_T, _OFFSET_WLAN(wapiPsk),_SIZE_WLAN(wapiPsk)},
{MIB_WLAN_WAPI_PSKLEN,	"WAPI_PSKLEN", BYTE_T	, _OFFSET_WLAN(wapiPskLen),_SIZE_WLAN(wapiPskLen)},
{MIB_WLAN_WAPI_PSK_FORMAT,	"WAPI_PSKFORMAT", BYTE_T, _OFFSET_WLAN(wapiPskFormat), _SIZE_WLAN(wapiPskFormat)},
{MIB_WLAN_WAPI_AUTH,	"WAPI_AUTH", BYTE_T, _OFFSET_WLAN(wapiAuth), _SIZE_WLAN(wapiAuth)},
{MIB_WLAN_WAPI_ASIPADDR,	"WAPI_ASIPADDR", IA_T, _OFFSET_WLAN(wapiAsIpAddr), _SIZE_WLAN(wapiAsIpAddr)},
{MIB_WLAN_WAPI_SEARCH_CERTINFO,	"WAPI_SEARCHINFO", STRING_T, _OFFSET_WLAN(wapiSearchCertInfo), _SIZE_WLAN(wapiSearchCertInfo)},
{MIB_WLAN_WAPI_SEARCH_CERTINDEX,	"WAPI_SEARCHINDEX", BYTE_T, _OFFSET_WLAN(wapiSearchIndex), _SIZE_WLAN(wapiSearchIndex)},
{MIB_WLAN_WAPI_MCAST_REKEYTYPE,	"WAPI_MCASTREKEY", BYTE_T, _OFFSET_WLAN(wapiMcastkey), _SIZE_WLAN(wapiMcastkey)},
{MIB_WLAN_WAPI_MCAST_TIME,	"WAPI_MCAST_TIME", DWORD_T, _OFFSET_WLAN(wapiMcastRekeyTime), _SIZE_WLAN(wapiMcastRekeyTime)},
{MIB_WLAN_WAPI_MCAST_PACKETS,	"WAPI_MCAST_PACKETS", DWORD_T, _OFFSET_WLAN(wapiMcastRekeyPackets), _SIZE_WLAN(wapiMcastRekeyPackets)},
{MIB_WLAN_WAPI_UCAST_REKETTYPE,	"WAPI_UCASTREKEY", BYTE_T, _OFFSET_WLAN(wapiUcastkey), _SIZE_WLAN(wapiUcastkey)},
{MIB_WLAN_WAPI_UCAST_TIME,	"WAPI_UCAST_TIME", DWORD_T, _OFFSET_WLAN(wapiUcastRekeyTime), _SIZE_WLAN(wapiUcastRekeyTime)},
{MIB_WLAN_WAPI_UCAST_PACKETS,	"WAPI_UCAST_PACKETS", DWORD_T, _OFFSET_WLAN(wapiUcastRekeyPackets), _SIZE_WLAN(wapiUcastRekeyPackets)},
{MIB_WLAN_WAPI_CA_INIT,	"WAPI_CA_INIT", BYTE_T, _OFFSET_WLAN(wapiCAInit), _SIZE_WLAN(wapiCAInit)},
#endif
{MIB_WLAN_11N_STBC,	 "STBC_ENABLED", BYTE_T, _OFFSET_WLAN(STBCEnabled), _SIZE_WLAN(STBCEnabled)},
{MIB_WLAN_11N_COEXIST,	 "COEXIST_ENABLED", BYTE_T, _OFFSET_WLAN(CoexistEnabled), _SIZE_WLAN(CoexistEnabled)},
{0}
};


/*
 * When using flash (set/get/all) command to access the MIB of below table,
 * it needs append a keyword "HW_" in ahead of mib name.
 */

mib_table_entry_T hwmib_table[]={
{MIB_HW_BOARD_VER, 		"BOARD_ID",		BYTE_T, 	_OFFSET_HW(boardVer), _SIZE_HW(boardVer)},
{MIB_HW_NIC0_ADDR, 		"NIC0_ADDR",		BYTE6_T, 	_OFFSET_HW(nic0Addr), _SIZE_HW(nic0Addr)},
{MIB_HW_NIC1_ADDR, 		"NIC1_ADDR",		BYTE6_T, 	_OFFSET_HW(nic1Addr), _SIZE_HW(nic1Addr)},
{0}
};


/*
 * When using flash (set/get/all) command to access the MIB of below table,
 * it needs append a keyword "HW_WLANx_" in ahead of mib name.
 */

mib_table_entry_T hwmib_wlan_table[]={
{MIB_HW_WLAN_ADDR, 		"WLAN_ADDR",		BYTE6_T, 	_OFFSET_HW_WLAN(macAddr), _SIZE_HW_WLAN(macAddr)},
{MIB_HW_WLAN_ADDR1, 		"WLAN_ADDR1",		BYTE6_T, 	_OFFSET_HW_WLAN(macAddr1), _SIZE_HW_WLAN(macAddr1)},
{MIB_HW_WLAN_ADDR2, 		"WLAN_ADDR2",		BYTE6_T, 	_OFFSET_HW_WLAN(macAddr2), _SIZE_HW_WLAN(macAddr2)},
{MIB_HW_WLAN_ADDR3, 		"WLAN_ADDR3",		BYTE6_T, 	_OFFSET_HW_WLAN(macAddr3), _SIZE_HW_WLAN(macAddr3)},
{MIB_HW_WLAN_ADDR4, 		"WLAN_ADDR4",		BYTE6_T, 	_OFFSET_HW_WLAN(macAddr4), _SIZE_HW_WLAN(macAddr4)},
#if defined(CONFIG_RTL8196B)
#if defined(CONFIG_RTL8198) || defined(CONFIG_RTL8196C)
{MIB_HW_WLAN_ADDR5, 		"WLAN_ADDR5",		BYTE6_T, 	_OFFSET_HW_WLAN(macAddr5), _SIZE_HW_WLAN(macAddr5)},
{MIB_HW_WLAN_ADDR6, 		"WLAN_ADDR6",		BYTE6_T, 	_OFFSET_HW_WLAN(macAddr6), _SIZE_HW_WLAN(macAddr6)},
{MIB_HW_WLAN_ADDR7, 		"WLAN_ADDR7",		BYTE6_T, 	_OFFSET_HW_WLAN(macAddr7), _SIZE_HW_WLAN(macAddr7)},
{MIB_HW_REG_DOMAIN, 		"REG_DOMAIN",		BYTE_T, 	_OFFSET_HW_WLAN(regDomain), _SIZE_HW_WLAN(regDomain)},
{MIB_HW_RF_TYPE, 		"RF_TYPE",		BYTE_T, 	_OFFSET_HW_WLAN(rfType), _SIZE_HW_WLAN(rfType)},
{MIB_HW_TX_POWER_CCK_A, 		"TX_POWER_CCK_A",		BYTE_ARRAY_T, 	_OFFSET_HW_WLAN(pwrlevelCCK_A), _SIZE_HW_WLAN(pwrlevelCCK_A)},
{MIB_HW_TX_POWER_CCK_B, 		"TX_POWER_CCK_B",		BYTE_ARRAY_T, 	_OFFSET_HW_WLAN(pwrlevelCCK_B), _SIZE_HW_WLAN(pwrlevelCCK_B)},
{MIB_HW_TX_POWER_HT40_1S_A, 		"TX_POWER_HT40_1S_A",	BYTE_ARRAY_T, 	_OFFSET_HW_WLAN(pwrlevelHT40_1S_A), _SIZE_HW_WLAN(pwrlevelHT40_1S_A)},
{MIB_HW_TX_POWER_HT40_1S_B, 		"TX_POWER_HT40_1S_B",	BYTE_ARRAY_T, 	_OFFSET_HW_WLAN(pwrlevelHT40_1S_B), _SIZE_HW_WLAN(pwrlevelHT40_1S_B)},
{MIB_HW_TX_POWER_HT40_2S, 		"TX_POWER_DIFF_HT40_2S",	BYTE_ARRAY_T, 	_OFFSET_HW_WLAN(pwrdiffHT40_2S), _SIZE_HW_WLAN(pwrdiffHT40_2S)},
{MIB_HW_TX_POWER_HT20, 		"TX_POWER_DIFF_HT20",	BYTE_ARRAY_T, 	_OFFSET_HW_WLAN(pwrdiffHT20), _SIZE_HW_WLAN(pwrdiffHT20)},
{MIB_HW_TX_POWER_DIFF_OFDM, 		"TX_POWER_DIFF_OFDM",	BYTE_ARRAY_T, 	_OFFSET_HW_WLAN(pwrdiffOFDM), _SIZE_HW_WLAN(pwrdiffOFDM)},
{MIB_HW_11N_XCAP, 		"11N_XCAP",		BYTE_T, 	_OFFSET_HW_WLAN(xCap), _SIZE_HW_WLAN(xCap)},
{MIB_HW_11N_TSSI1, 		"11N_TSSI1",		BYTE_T, 	_OFFSET_HW_WLAN(TSSI1), _SIZE_HW_WLAN(TSSI1)},
{MIB_HW_11N_TSSI2, 		"11N_TSSI2",		BYTE_T, 	_OFFSET_HW_WLAN(TSSI2), _SIZE_HW_WLAN(TSSI2)},
{MIB_HW_11N_THER, 		"11N_THER",		BYTE_T, 	_OFFSET_HW_WLAN(Ther), _SIZE_HW_WLAN(Ther)},
{MIB_HW_11N_TRSWITCH, 		"11N_TRSWITCH",		BYTE_T, 	_OFFSET_HW_WLAN(trswitch), _SIZE_HW_WLAN(trswitch)},
{MIB_HW_11N_RESERVED2, 		"11N_RESERVED2",		BYTE_T, 	_OFFSET_HW_WLAN(Reserved2), _SIZE_HW_WLAN(Reserved2)},
{MIB_HW_11N_RESERVED3, 		"11N_RESERVED3",		BYTE_T, 	_OFFSET_HW_WLAN(Reserved3), _SIZE_HW_WLAN(Reserved3)},
{MIB_HW_11N_RESERVED4, 		"11N_RESERVED4",		BYTE_T, 	_OFFSET_HW_WLAN(Reserved4), _SIZE_HW_WLAN(Reserved4)},
{MIB_HW_11N_RESERVED5, 		"11N_RESERVED5",		BYTE_T, 	_OFFSET_HW_WLAN(Reserved5), _SIZE_HW_WLAN(Reserved5)},
{MIB_HW_11N_RESERVED6, 		"11N_RESERVED6",		BYTE_T, 	_OFFSET_HW_WLAN(Reserved6), _SIZE_HW_WLAN(Reserved6)},
{MIB_HW_11N_RESERVED7, 		"11N_RESERVED7",		BYTE_T, 	_OFFSET_HW_WLAN(Reserved7), _SIZE_HW_WLAN(Reserved7)},
{MIB_HW_11N_RESERVED8, 		"11N_RESERVED8",		BYTE_T, 	_OFFSET_HW_WLAN(Reserved8), _SIZE_HW_WLAN(Reserved8)},
{MIB_HW_11N_RESERVED9, 		"11N_RESERVED9",		BYTE_T, 	_OFFSET_HW_WLAN(Reserved9), _SIZE_HW_WLAN(Reserved9)},
{MIB_HW_11N_RESERVED10, 		"11N_RESERVED10",		BYTE_T, 	_OFFSET_HW_WLAN(Reserved10), _SIZE_HW_WLAN(Reserved10)},
{MIB_HW_LED_TYPE, 		"LED_TYPE",		BYTE_T, 	_OFFSET_HW_WLAN(ledType), _SIZE_HW_WLAN(ledType)},
{MIB_HW_TX_POWER_5G_HT40_1S_A, 		"TX_POWER_5G_HT40_1S_A",	BYTE_ARRAY_T, 	_OFFSET_HW_WLAN(pwrlevel5GHT40_1S_A), _SIZE_HW_WLAN(pwrlevel5GHT40_1S_A)},
{MIB_HW_TX_POWER_5G_HT40_1S_B, 		"TX_POWER_5G_HT40_1S_B",	BYTE_ARRAY_T, 	_OFFSET_HW_WLAN(pwrlevel5GHT40_1S_B), _SIZE_HW_WLAN(pwrlevel5GHT40_1S_B)},
{MIB_HW_TX_POWER_5G_HT40_2S, 		"TX_POWER_DIFF_5G_HT40_2S",	BYTE_ARRAY_T, 	_OFFSET_HW_WLAN(pwrdiff5GHT40_2S), _SIZE_HW_WLAN(pwrdiff5GHT40_2S)},
{MIB_HW_TX_POWER_5G_HT20, 		"TX_POWER_DIFF_5G_HT20",	BYTE_ARRAY_T, 	_OFFSET_HW_WLAN(pwrdiff5GHT20), _SIZE_HW_WLAN(pwrdiff5GHT20)},
{MIB_HW_TX_POWER_5G_DIFF_OFDM, 		"TX_POWER_DIFF_5G_OFDM",	BYTE_ARRAY_T, 	_OFFSET_HW_WLAN(pwrdiff5GOFDM), _SIZE_HW_WLAN(pwrdiff5GOFDM)},

#else

{MIB_HW_WLAN_ADDR5, 		"WLAN_ADDR5",		BYTE6_T, 	_OFFSET_HW_WLAN(macAddr5), _SIZE_HW_WLAN(macAddr5)},
{MIB_HW_WLAN_ADDR6, 		"WLAN_ADDR6",		BYTE6_T, 	_OFFSET_HW_WLAN(macAddr6), _SIZE_HW_WLAN(macAddr6)},
{MIB_HW_WLAN_ADDR7, 		"WLAN_ADDR7",		BYTE6_T, 	_OFFSET_HW_WLAN(macAddr7), _SIZE_HW_WLAN(macAddr7)},
{MIB_HW_REG_DOMAIN, 		"REG_DOMAIN",		BYTE_T, 	_OFFSET_HW_WLAN(regDomain), _SIZE_HW_WLAN(regDomain)},
{MIB_HW_RF_TYPE, 		"RF_TYPE",		BYTE_T, 	_OFFSET_HW_WLAN(rfType), _SIZE_HW_WLAN(rfType)},
{MIB_HW_TX_POWER_CCK, 		"TX_POWER_CCK",		BYTE_ARRAY_T, 	_OFFSET_HW_WLAN(txPowerCCK), _SIZE_HW_WLAN(txPowerCCK)},
{MIB_HW_TX_POWER_OFDM_1S, 		"TX_POWER_OFDM_1S",	BYTE_ARRAY_T, 	_OFFSET_HW_WLAN(txPowerOFDM_HT_OFDM_1S), _SIZE_HW_WLAN(txPowerOFDM_HT_OFDM_1S)},
{MIB_HW_TX_POWER_OFDM_2S, 		"TX_POWER_OFDM_2S",	BYTE_ARRAY_T, 	_OFFSET_HW_WLAN(txPowerOFDM_HT_OFDM_2S), _SIZE_HW_WLAN(txPowerOFDM_HT_OFDM_2S)},
{MIB_HW_11N_XCAP, 		"11N_XCAP",		BYTE_T, 	_OFFSET_HW_WLAN(xCap), _SIZE_HW_WLAN(xCap)},
{MIB_HW_11N_LOFDMPWDA, 		"11N_LOFDMPWDA",		BYTE_T, 	_OFFSET_HW_WLAN(LOFDMPwDiffA), _SIZE_HW_WLAN(LOFDMPwDiffA)},
{MIB_HW_11N_LOFDMPWDB, 		"11N_LOFDMPWDB",		BYTE_T, 	_OFFSET_HW_WLAN(LOFDMPwDiffB), _SIZE_HW_WLAN(LOFDMPwDiffB)},
{MIB_HW_11N_TSSI1, 		"11N_TSSI1",		BYTE_T, 	_OFFSET_HW_WLAN(TSSI1), _SIZE_HW_WLAN(TSSI1)},
{MIB_HW_11N_TSSI2, 		"11N_TSSI2",		BYTE_T, 	_OFFSET_HW_WLAN(TSSI2), _SIZE_HW_WLAN(TSSI2)},
{MIB_HW_11N_THER, 		"11N_THER",		BYTE_T, 	_OFFSET_HW_WLAN(Ther), _SIZE_HW_WLAN(Ther)},
{MIB_HW_11N_TRSWITCH, 		"11N_TRSWITCH",		BYTE_T, 	_OFFSET_HW_WLAN(trswitch), _SIZE_HW_WLAN(trswitch)},
{MIB_HW_11N_RESERVED2, 		"11N_RESERVED2",		BYTE_T, 	_OFFSET_HW_WLAN(Reserved2), _SIZE_HW_WLAN(Reserved2)},
{MIB_HW_11N_RESERVED3, 		"11N_RESERVED3",		BYTE_T, 	_OFFSET_HW_WLAN(Reserved3), _SIZE_HW_WLAN(Reserved3)},
{MIB_HW_11N_RESERVED4, 		"11N_RESERVED4",		BYTE_T, 	_OFFSET_HW_WLAN(Reserved4), _SIZE_HW_WLAN(Reserved4)},
{MIB_HW_11N_RESERVED5, 		"11N_RESERVED5",		BYTE_T, 	_OFFSET_HW_WLAN(Reserved5), _SIZE_HW_WLAN(Reserved5)},
{MIB_HW_11N_RESERVED6, 		"11N_RESERVED6",		BYTE_T, 	_OFFSET_HW_WLAN(Reserved6), _SIZE_HW_WLAN(Reserved6)},
{MIB_HW_11N_RESERVED7, 		"11N_RESERVED7",		BYTE_T, 	_OFFSET_HW_WLAN(Reserved7), _SIZE_HW_WLAN(Reserved7)},
{MIB_HW_11N_RESERVED8, 		"11N_RESERVED8",		BYTE_T, 	_OFFSET_HW_WLAN(Reserved8), _SIZE_HW_WLAN(Reserved8)},
{MIB_HW_LED_TYPE, 		"LED_TYPE",		BYTE_T, 	_OFFSET_HW_WLAN(ledType), _SIZE_HW_WLAN(ledType)},

#endif
#else
//!CONFIG_RTL8196B => rtl8651c+rtl8190
{MIB_HW_REG_DOMAIN, 		"REG_DOMAIN",		BYTE_T, 	_OFFSET_HW_WLAN(regDomain), _SIZE_HW_WLAN(regDomain)},
{MIB_HW_RF_TYPE, 		"RF_TYPE",		BYTE_T, 	_OFFSET_HW_WLAN(rfType), _SIZE_HW_WLAN(rfType)},
{MIB_HW_TX_POWER_CCK, 		"TX_POWER_CCK",		BYTE_ARRAY_T, 	_OFFSET_HW_WLAN(txPowerCCK), _SIZE_HW_WLAN(txPowerCCK)},
{MIB_HW_TX_POWER_OFDM, 		"TX_POWER_OFDM",	BYTE_ARRAY_T, 	_OFFSET_HW_WLAN(txPowerOFDM), _SIZE_HW_WLAN(txPowerOFDM)},
{MIB_HW_ANT_DIVERSITY, 		"ANT_DIVERSITY",	BYTE_T, 	_OFFSET_HW_WLAN(antDiversity), _SIZE_HW_WLAN(antDiversity)},
{MIB_HW_TX_ANT, 		"TX_ANT",		BYTE_T, 	_OFFSET_HW_WLAN(txAnt), _SIZE_HW_WLAN(txAnt)},
{MIB_HW_INIT_GAIN, 		"INIT_GAIN",		BYTE_T, 	_OFFSET_HW_WLAN(initGain), _SIZE_HW_WLAN(initGain)},
{MIB_HW_CCA_MODE, 		"CCA_MODE",		BYTE_T, 	_OFFSET_HW_WLAN(ccaMode), _SIZE_HW_WLAN(ccaMode)},
{MIB_HW_LED_TYPE, 		"LED_TYPE",		BYTE_T, 	_OFFSET_HW_WLAN(ledType), _SIZE_HW_WLAN(ledType)},
{MIB_HW_11N_XCAP, 		"11N_XCAP",		BYTE_T, 	_OFFSET_HW_WLAN(xCap), _SIZE_HW_WLAN(xCap)},
{MIB_HW_11N_LOFDMPWD, 		"11N_LOFDMPWD",		BYTE_T, 	_OFFSET_HW_WLAN(LOFDMPwDiff), _SIZE_HW_WLAN(LOFDMPwDiff)},
//{MIB_HW_11N_ANTPWD_B, 		"11N_ANTPWD_B",		BYTE_T, 	_OFFSET_HW_WLAN(AntPwDiff_B), _SIZE_HW_WLAN(AntPwDiff_B)},
{MIB_HW_11N_ANTPWD_C, 		"11N_ANTPWD_C",		BYTE_T, 	_OFFSET_HW_WLAN(AntPwDiff_C), _SIZE_HW_WLAN(AntPwDiff_C)},
//{MIB_HW_11N_ANTPWD_D, 		"11N_ANTPWD_D",		BYTE_T, 	_OFFSET_HW_WLAN(AntPwDiff_D), _SIZE_HW_WLAN(AntPwDiff_D)},
{MIB_HW_11N_THER_RFIC, 		"11N_THER_RFIC",		BYTE_T, 	_OFFSET_HW_WLAN(TherRFIC), _SIZE_HW_WLAN(TherRFIC)},
#endif

{MIB_WSC_PIN,			"WSC_PIN",			STRING_T,	_OFFSET_HW_WLAN(wscPin), _OFFSET_HW_WLAN(wscPin)},
{0}
};
