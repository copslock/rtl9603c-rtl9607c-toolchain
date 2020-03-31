<html>
<! Copyright (c) Realtek Semiconductor Corp., 2003. All Rights Reserved. ->
<head>
<meta http-equiv="Content-Type" content="text/html">
<title>Wireless Site Survey</title>
<script type="text/javascript" src="util_gw.js"> </script>
<script>
var connectEnabled=0, autoconf=0;

function enableConnect()
{ 
  if (autoconf == 0) {
  enableButton(document.formWlSiteSurvey.connect);
  connectEnabled=1;
}
}

function connectClick()
{
  if (connectEnabled==1)
	return true;
  else
  	return false;
}

</script>
</head>
<body>
<blockquote>
<h2><font color="#0000FF">Wireless Site Survey <% if (getIndex("wlan_num") > 1) write("-wlan"+(getIndex("wlan_idx")+1)); %></font></h2>

<table border=0 width="500" cellspacing=0 cellpadding=0>
<tr><font size=2>
 This page provides tool to scan the wireless network. If any Access Point or
 IBSS is found, you could choose to connect it manually when client mode is enabled.
</font></tr>
<tr><hr size=1 noshade align=top></tr>
</table>
<form action=/goform/formWlSiteSurvey method=POST name="formWlSiteSurvey">
  <table border="1" width="500">
  <tr><h4><font>List of APs</font></tr>
  <% wlSiteSurveyTbl(); %>
  </table>
  <br>
  <input type="submit" value="Refresh" name="refresh">&nbsp;&nbsp;
  <input type="submit" value="Connect" name="connect" onClick="return connectClick()">
  <input type="hidden" value="/wlsurvey.asp" name="submit-url">
 <script>
 	var wlanState="<%getScheduleInfo("wlan_state");%>";
   	<% 	
   	
   		if (getIndex("wlanDisabled"))
     	  	write( "disableButton(document.formWlSiteSurvey.refresh);" );
 	 %>
 	 
 	 if (wlanState=="Disabled")
     	 	disableButton(document.formWlSiteSurvey.refresh);
 	 
	disableButton(document.formWlSiteSurvey.connect);
 </script>
</form>

</blockquote>
</body>
</html>
