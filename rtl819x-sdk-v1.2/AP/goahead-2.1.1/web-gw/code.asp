<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<title>[insert your title here]</title>

<script type="text/javascript">
// Framebuster script to relocate browser when MSIE bookmarks this
// page instead of the parent frameset.  Set variable relocateURL to
// the index document of your website (relative URLs are ok):
var relocateURL = "/";

var state="normal";
var pocketRouter_Mode="<% getInfo("pocketRouter_Mode"); %>"*1;
var POCKETROUTER_GATEWAY = 3;
var POCKETROUTER_BRIDGE_AP = 2;
var POCKETROUTER_BRIDGE_CLIENT = 1;

if(parent.frames.length == 0) {
  if(document.images) {
    location.replace(relocateURL);
  } else {
    location = relocateURL;
  }
}
</script>

<script type="text/javascript" src="mtmcode.js">
</script>

<script type="text/javascript">
// Morten's JavaScript Tree Menu
// version 2.3.2-macfriendly, dated 2002-06-10
// http://www.treemenu.com/

// Copyright (c) 2001-2002, Morten Wang & contributors
// All rights reserved.

// This software is released under the BSD License which should accompany
// it in the file "COPYING".  If you do not have this file you can access
// the license through the WWW at http://www.treemenu.com/license.txt

// Nearly all user-configurable options are set to their default values.
// Have a look at the section "Setting options" in the installation guide
// for description of each option and their possible values.

MTMDefaultTarget = "view";

/******************************************************************************
* User-configurable list of icons.                                            *
******************************************************************************/

var MTMIconList = null;
MTMIconList = new IconList();
MTMIconList.addIcon(new MTMIcon("menu_link_external.gif", "http://", "pre"));
MTMIconList.addIcon(new MTMIcon("menu_link_pdf.gif", ".pdf", "post"));

/******************************************************************************
* User-configurable menu.                                                     *
******************************************************************************/

// Main menu.
var menu = null;
var wlan_num =  <% write(getIndex("wlan_num")); %> ;
menu = new MTMenu();

var isWDSDefined = <% write(getIndex("isWDSDefined")); %> ;
var isMeshDefined =  <% write(getIndex("isMeshDefined")); %> ;


menu.addItem("Setup Wizard", "wizard.asp", "", "Setup Wizard");

menu.addItem("Operation Mode", "opmode.asp", "", "Operation Mode");

function get_form(page, wlan_id){
	return 'goform/formWlanRedirect?redirect-url='+page+'&wlan_id='+wlan_id ;
}

<% write(getIndex("isWlanMenuStart")); %>
menu.addItem("Wireless");
wlan = new MTMenu();
for(i=0; i < wlan_num ; i++){
	wlan_name= "wlan" +(i+1) ;
	if(wlan_num == 1)
		wlan0 = wlan ;
	else{
		wlan.addItem(wlan_name);
	 	wlan0= new MTMenu();
	}
	wlan0.addItem("Basic Settings", get_form("wlbasic.asp",i), "", "Setup wireless basic configuration");
	wlan0.addItem("Advanced Settings",get_form("wladvanced.asp",i),"", "Setup wireless advanced configuration");
//	wlan0.addItem("Security", get_form("wlwpa.asp",i), "", "Setup wireless security");
	wlan0.addItem("Security", get_form("wlsecurity.asp",i), "", "Setup wireless security");
	
	if(pocketRouter_Mode != POCKETROUTER_BRIDGE_CLIENT && pocketRouter_Mode != POCKETROUTER_BRIDGE_AP)
		wlan0.addItem("Access Control", get_form("wlactrl.asp",i), "", "Setup access control list for wireless clients");
	
	if( isWDSDefined ==1 && pocketRouter_Mode != POCKETROUTER_BRIDGE_CLIENT)
	wlan0.addItem("WDS settings", get_form("wlwds.asp",i), "", "Setup wireless distribution system");
	
	if( isMeshDefined ==1 )
	wlan0.addItem("Mesh settings", get_form("wlmesh.asp",i), "", "Setup wireless mesh setting");
	
	wlan0.addItem("Site Survey", get_form("wlsurvey.asp",i), "",  "Setup access control list for wireless clients");	
// for WPS
//	wlan0.addItem("EasyConfig", get_form("wlautocfg.asp",i), "", "Setup wireless EasyConfig");
	wlan0.addItem("WPS", get_form("wlwps.asp",i), "", "Wi-Fi Protected Setup");
	wlan0.addItem("Schedule", get_form("wlan_schedule.asp",i), "", "Wireles LAN Schedule");
	if(wlan_num != 1)
	wlan.makeLastSubmenu(wlan0);
}
	menu.makeLastSubmenu(wlan);
<% write(getIndex("isWlanMenuEnd")); %>

menu.addItem("TCP/IP Settings");
tcpip = new MTMenu();
tcpip.addItem("LAN Interface", "tcpiplan.asp", "", "Setup LAN Interface");

if(pocketRouter_Mode != POCKETROUTER_BRIDGE_CLIENT && pocketRouter_Mode != POCKETROUTER_BRIDGE_AP)
	tcpip.addItem("WAN Interface", "tcpipwan.asp", "", "Setup WAN Interface");
	
menu.makeLastSubmenu(tcpip);
<% getInfo("wapiMenu"); %>

if(pocketRouter_Mode != POCKETROUTER_BRIDGE_CLIENT && pocketRouter_Mode != POCKETROUTER_BRIDGE_AP)
{
	menu.addItem("Firewall");
	firewall = new MTMenu();
	firewall.addItem("Port Filtering", "portfilter.asp", "", "Setup port filtering");
	firewall.addItem("IP Filtering", "ipfilter.asp", "", "Setup IP filering");
	firewall.addItem("MAC Filtering", "macfilter.asp", "", "Setup MAC filering");
	firewall.addItem("Port Forwarding", "portfw.asp", "", "Setup port-forwarding");
	firewall.addItem("URL Filtering", "urlfilter.asp", "", "Setup URL filering");
	//firewall.addItem("Trigger Port", "trigport.asp", "", "Setup trigger port");
	firewall.addItem("DMZ", "dmz.asp", "", "Setup DMZ");
	<% getInfo("vlan_menu_onoff"); %>
	menu.makeLastSubmenu(firewall);
}
//menu.addItem("Route Setup", "route.asp", "", "Route Setup");

//menu.addItem("QoS", "qos.asp", "", "Setup QoS");

if(pocketRouter_Mode != POCKETROUTER_BRIDGE_CLIENT && pocketRouter_Mode != POCKETROUTER_BRIDGE_AP)
	<% getInfo("qos_root_menu"); %>

if(pocketRouter_Mode != POCKETROUTER_BRIDGE_CLIENT && pocketRouter_Mode != POCKETROUTER_BRIDGE_AP)
	<% getInfo("route_menu_onoff"); %>
	
<% getInfo("voip_tree_menu"); %>

menu.addItem("Management");
manage = new MTMenu();
<% getInfo("status_menu"); %>
<% getInfo("statistic_menu"); %>
<% getInfo("powerConsumption_menu"); %>

if(pocketRouter_Mode != POCKETROUTER_BRIDGE_CLIENT && pocketRouter_Mode != POCKETROUTER_BRIDGE_AP)
	manage.addItem("DDNS", "ddns.asp", "", "Setup Dynamic DNS");
	
if(pocketRouter_Mode != POCKETROUTER_BRIDGE_CLIENT && pocketRouter_Mode != POCKETROUTER_BRIDGE_AP)	
	manage.addItem("Time Zone Setting", "ntp.asp", "", "Setup Time Zone");
	
if(pocketRouter_Mode != POCKETROUTER_BRIDGE_CLIENT && pocketRouter_Mode != POCKETROUTER_BRIDGE_AP)	
	manage.addItem("Denial-of-Service", "dos.asp", "", "Setup DoS");
	
manage.addItem("Log", "syslog.asp", "", "System Log");
manage.addItem("Upgrade Firmware", "upload.asp", "", "Update firmware image");
manage.addItem("Save/Reload Settings", "saveconf.asp", "", "Save/reload current settings");
<% getInfo("cwmp_tr069_menu"); %>
manage.addItem("Password", "password.asp", "", "Setup access password");
menu.makeLastSubmenu(manage);
userName= '<% getInfo("userName"); %>' ;
if(userName != "")
        menu.addItem("Logout", "logout.asp", "", "Logout");

</script>
</head>
<body onload="MTMStartMenu(true)" bgcolor="#000033" text="#ffffcc" link="yellow" vlink="lime" alink="red">
</body>
</html>
