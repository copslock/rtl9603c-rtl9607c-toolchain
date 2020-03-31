<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<title>[insert your title here]</title>

<script type="text/javascript">
// Framebuster script to relocate browser when MSIE bookmarks this
// page instead of the parent frameset.  Set variable relocateURL to
// the index document of your website (relative URLs are ok):
var relocateURL = "/";

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

menu.addItem("Setup Wizard", "wizard.asp", "", "Setup Wizard");
menu.addItem("Operation Mode", "opmode.asp", "", "Operation Mode");
function get_form(page, wlan_id){
	return 'goform/formWlanRedirect?redirect-url='+page+'&wlan_id='+wlan_id ;
}
menu.addItem("Wireless");
wlan = new MTMenu();
/******** wlan0 interface menu *********/
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
	wlan0.addItem("Security", get_form("wlwpa.asp",i), "", "Setup wireless security");
	wlan0.addItem("Access Control", get_form("wlactrl.asp",i), "", "Setup access control list for wireless clients");
	wlan0.addItem("WDS settings", get_form("wlwds.asp",i), "", "Setup wireless distribution system");
	wlan0.addItem("Site Survey", get_form("wlsurvey.asp",i), "",  "Setup access control list for wireless clients");	
// for WPS
//	wlan0.addItem("EasyConfig", get_form("wlautocfg.asp",i), "", "Setup wireless EasyConfig");
	wlan0.addItem("WPS", get_form("wlwps.asp",i), "", "Wi-Fi Protected Setup");
	if(wlan_num != 1)
	wlan.makeLastSubmenu(wlan0);
}
	menu.makeLastSubmenu(wlan);

menu.addItem("TCP/IP Settings");
tcpip = new MTMenu();
tcpip.addItem("LAN Interface", "tcpiplan.asp", "", "Setup LAN Interface");
tcpip.addItem("WAN Interface", "tcpipwan.asp", "", "Setup WAN Interface");
menu.makeLastSubmenu(tcpip);

menu.addItem("Firewall");
firewall = new MTMenu();
firewall.addItem("Port Filtering", "portfilter.asp", "", "Setup port filtering");
firewall.addItem("IP Filtering", "ipfilter.asp", "", "Setup IP filering");
firewall.addItem("MAC Filtering", "macfilter.asp", "", "Setup MAC filering");
firewall.addItem("Port Forwarding", "portfw.asp", "", "Setup port-forwarding");
firewall.addItem("URL Filtering", "urlfilter.asp", "", "Setup URL filering");
//firewall.addItem("Trigger Port", "trigport.asp", "", "Setup trigger port");
firewall.addItem("DMZ", "dmz.asp", "", "Setup DMZ");
menu.makeLastSubmenu(firewall);
menu.addItem("VPN Setting", "vpn.asp", "", "Setup VPN");
//menu.addItem("Route Setup", "route.asp", "", "Route Setup");

<% getInfo("voip_tree_menu"); %>

menu.addItem("Management");
manage = new MTMenu();
manage.addItem("Status", "status.asp", "", "Display current status");
manage.addItem("Statistics", "stats.asp", "", "Display packet statistics");
manage.addItem("DDNS", "ddns.asp", "", "Setup Dynamic DNS");
manage.addItem("Time Zone Setting", "ntp.asp", "", "Setup Time Zone");
manage.addItem("Denial-of-Service", "dos.asp", "", "Setup DoS");
manage.addItem("Log", "syslog.asp", "", "System Log");
manage.addItem("Upgrade Firmware", "upload.asp", "", "Update firmware image");
manage.addItem("Save/Reload Settings", "saveconf.asp", "", "Save/reload current settings");
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
