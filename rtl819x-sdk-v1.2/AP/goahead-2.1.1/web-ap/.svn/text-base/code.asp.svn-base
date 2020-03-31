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
var isACLIncluded = <% write(getIndex("isACLIncluded")); %> ;
menu.addItem("Setup Wizard", "wizard.asp", "", "Setup Wizard");
menu.addItem("Status", "status.asp", "", "Display current status");
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
//	wlan0.addItem("Security", get_form("wlwpa.asp",i), "", "Setup wireless security");
	wlan0.addItem("Security", get_form("wlsecurity.asp",i), "", "Setup wireless security");
	if(isACLIncluded==1)
	wlan0.addItem("Access Control", get_form("wlactrl.asp",i), "", "Setup access control list for wireless clients");
	
	if( isWDSDefined ==1 )
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

menu.addItem("TCP/IP Settings", "tcpip.asp", "", "Setup TCPIP settings");
<% getInfo("voip_tree_menu"); %>
<% getInfo("cwmp_tr069_menu"); %>
menu.addItem("Log", "syslog.asp", "", "System Log");
menu.addItem("Statistics", "stats.asp", "", "Display packet statistics");
menu.addItem("Upgrade Firmware", "upload.asp", "", "Update firmware image");
menu.addItem("Save/Reload Settings", "saveconf.asp", "", "Save/reload current settings");
menu.addItem("Password", "password.asp", "", "Setup access password");
<% getScheduleInfo("system_time"); %>
userName= '<% getInfo("userName"); %>' ;
if(userName != "")
        menu.addItem("Logout", "logout.asp", "", "Logout");

</script>
</head>
<body onload="MTMStartMenu(true)" bgcolor="#000033" text="#ffffcc" link="yellow" vlink="lime" alink="red">
</body>
</html>
