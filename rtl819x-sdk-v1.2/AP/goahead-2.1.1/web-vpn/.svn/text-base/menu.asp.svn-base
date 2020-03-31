<html>
<! Copyright (c) Realtek Semiconductor Corp., 2003. All Rights Reserved. ->
<head>
<meta http-equiv="Content-Type" content="text/html">
<title>menu</title>
</head>

<body>
<table border="0" width="100%" font color="#ffffff">
<tr><td><a href="wizard.asp" target="view">Setup Wizard</a></td></tr>
<tr><td><a href="opmode.asp" target="view">Operation Mode</a></td></tr>
<script>
function get_form(page, wlan_id){
	return 'goform/formWlanRedirect?redirect-url='+page+'&wlan_id='+wlan_id ;
}
wlan_num = <% write(getIndex("wlan_num")); %>; 
for(i=0;i < wlan_num ;i ++){
	document.write('<tr><td><b>Wireless '+i+'</b></td></tr>\
	<tr><td><a href="'+get_form("wlbasic.asp",i)+ '" target="view">Basic Settings</a></td></tr>\
	<tr><td><a href="'+get_form("wladvanced.asp",i)+'" target="view">Advanced Settings</a></td></tr>\
	<tr><td><a href="'+get_form("wlwpa.asp",i)+'" target="view">Security</a></td></tr>\
	<tr><td><a href="'+get_form("wlactrl.asp",i)+'" target="view">Access Control</a></td></tr>\
	<tr><td><a href="'+get_form("wlsurvey.asp",i)+'" target="view">Site Survey</a></td></tr>\
	<tr><td><a href="'+get_form("wlwds.asp",i)+'" target="view">WDS Setting</a></td></tr>\
//	<tr><td><a href="'+get_form("wlautocfg.asp",i)+'" target="view">EasyConfig</a></td></tr>');
	<tr><td><a href="'+get_form("wlwps.asp",i)+'" target="view">WPS</a></td></tr>');
}
</script>
<tr><td><b>TCP/IP Settings</b></td></tr>
<tr><td><a href="tcpiplan.asp" target="view">LAN Interface</a></td></tr>
<tr><td><a href="tcpipwan.asp" target="view">WAN Interface</a></td></tr>

<tr><td><b>Firewall</b></td></tr>
<tr><td><a href="portfilter.asp" target="view">Port Filtering</a></td></tr>
<tr><td><a href="ipfilter.asp" target="view">IP Filtering</a></td></tr>
<tr><td><a href="macfilter.asp" target="view">MAC Filtering</a></td></tr>
<tr><td><a href="portfw.asp" target="view">Port Forwarding</a></td></tr>
<tr><td><a href="urlfilter.asp" target="view">URL Filtering</a></td></tr>
<tr><td><a href="dmz.asp" target="view">DMZ</a></td></tr>
<tr><td><a href="vpn.asp" target="view">VPN Setting</a></td></tr>
<!-- 
<tr><td><a href="route.asp" target="view">Route Setup</a></td></tr>
-->

<% getInfo("voip_menu"); %>

<tr><td><b>Management</b></td></tr>
<tr><td><a href="status.asp" target="view">Status</a></td></tr>
<tr><td><a href="stats.asp" target="view">Statistics</a></td></tr>
<tr><td><a href="ddns.asp" target="view">DDNS</a></td></tr>
<tr><td><a href="ntp.asp" target="view">Time Zone Setting</a></td></tr>
<tr><td><a href="dos.asp" target="view">Denial-of-Service</a></td></tr>
<tr><td><a href="syslog.asp" target="view">System Log</a></td></tr>
<tr><td><a href="upload.asp" target="view">Upgrade Firmware</a></td></tr>
<tr><td><a href="saveconf.asp" target="view">Save/Load Setting</a></td></tr>
<tr><td><a href="password.asp" target="view">Password</a></td></tr>
<script>
userName= '<% getInfo("userName"); %>' ;
if(userName != "")
document.write('<tr><td><a href="logout.asp" target="view">Logout</a></td></tr>');
</script>
</table>

</body>
</html>
