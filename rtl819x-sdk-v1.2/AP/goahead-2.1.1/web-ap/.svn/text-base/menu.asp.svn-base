<html>
<! Copyright (c) Realtek Semiconductor Corp., 2003. All Rights Reserved. ->
<head>
<meta http-equiv="Content-Type" content="text/html">
<title>menu</title>
</head>

<body>
<table border="0" width="100%" font color="#ffffff">
<tr><td><a href="wizard.asp" target="view">Setup Wizard</a></td></tr>
<tr><td><a href="status.asp" target="view">Status</a></td></tr>
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
<tr><td><a href="tcpip.asp" target="view">TCP/IP Settings</a></td></tr>
<% getInfo("voip_menu"); %>
<% getInfo("tr069_nojs_menu"); %>
<tr><td><a href="syslog.asp" target="view">System Log</a></td></tr>
<tr><td><a href="stats.asp" target="view">Statistics</a></td></tr>
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
