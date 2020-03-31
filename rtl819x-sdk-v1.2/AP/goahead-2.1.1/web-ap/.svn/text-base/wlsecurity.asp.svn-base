<html>
<! Copyright (c) Realtek Semiconductor Corp., 2007. All Rights Reserved. ->
<head>
<meta http-equiv="Content-Type" content="text/html">
<title>Wireless Security Setup</title>
<script type="text/javascript" src="util_ap.js"> </script>
<style>
.on {display:on}
.off {display:none}	
.bggrey {
	BACKGROUND: #FFFFFF
}
</style>
<script>
SecurityTypePage = new Array("wlsecurity_all.asp");
var defPskLen=new Array();
var defPskFormat=new Array();
var autoconf=new Array();
var wlan_idx=<% write(getIndex("wlan_idx")); %>;
autoconf[wlan_idx]= 0;
var SSIDnum = 1;

function ValidateForm(passForm, reset)
{
	if (reset) {
		passForm.reset();				
		var auth = get_by_id("SSIDAuthMode");
		auth.src = SecurityTypePage[0];
	}
	else {
		if (self.SSIDAuthMode.ValidateForm(passForm) == true) {
			passForm.submit();
		}
	}
}

function SetSSIDSettingOption()
{	
	var mode = <% write(getIndex("wlanMode"));%>;
	var mssid_num=<%write(getIndex("wlan_mssid_num"));%>;
	if (mode != 1) {	
		var ssid="<% getInfo("ssid"); %>";
		var val="Root AP - " + ssid;	
		var disable=new Array()
		disable[0] = 0;		
		disable[1] = <% write(getVirtualIndex("wlanDisabled", 1)); %>;
		disable[2] = <% write(getVirtualIndex("wlanDisabled", 2)); %>;
		disable[3] = <% write(getVirtualIndex("wlanDisabled", 3)); %>;
		disable[4] = <% write(getVirtualIndex("wlanDisabled", 4)); %>;		
		
		for (i=0; i<=mssid_num; i++) {	
			if (!disable[i]) {
				document.write("<option value=\"" + i + "\"><font face=\"Arial\" style=\"font-size: 8pt\">");
			document.write(val);
			document.write("</font></option>");
			}
			if (i != 4 && !disable[i+1]) {
				id =  i+1;
				if (id == 1)	
					ssid =  " - <% getVirtualInfo("ssid", 1); %>";
				else if (id == 2)	
					ssid =  " - <% getVirtualInfo("ssid", 2); %>";
				else if (id == 3)	
					ssid =  " - <% getVirtualInfo("ssid", 3 ); %>";
				else
				ssid =  " - <% getVirtualInfo("ssid", 4 ); %>";	
				val = "AP" + id + ssid;
			}		
		}
	}
	else { // client mode
		var ssid="Root Client - " + "<% getInfo("ssid"); %>";
		document.write("<option value=0><font face=\"Arial\" style=\"font-size: 8pt\">");
		document.write(ssid);
		document.write("</font></option>");			
	}	
	var rp_enable = <% write(getIndex("repeaterEnabled")); %>;		
	var is_adhoc = <% write(getIndex("networkType")); %>;
	if (rp_enable && ((mode == 0 || mode == 3) || (mode == 1 && !is_adhoc)) ) {
		ssid =  " - <% getInfo("repeaterSSID"); %>";
		if (mode == 1)
			val = "Repeater AP" + ssid;
		else
			val = "Repeater Client" + ssid;
		document.write("<option value=5><font face=\"Arial\" style=\"font-size: 8pt\">");
		document.write(val);
		document.write("</font></option>");			
	}	
	var select=<% write(getIndex("mssid_idx")); %>;	
	document.formEncrypt.SSID_Setting.selectedIndex = select;	
}

function SSIDSelected(index)
{
	var auth = get_by_id("SSIDAuthMode");
	auth.src = SecurityTypePage[0];
}
</script>

</head>

<body onload="SSIDSelected();">
<blockquote>
<h2><font color="#0000FF">Wireless Security Setup<% if (getIndex("wlan_num") > 1) write("-wlan"+(getIndex("wlan_idx")+1)); %> </font></h2>

<form action=/goform/formWlEncrypt method=POST name="formEncrypt">
<table border=0 width="540" cellspacing=4 cellpadding=0>
    <tr><font size=2>
    This page allows you setup the wireless security. Turn on WEP or WPA by using
    Encryption Keys could prevent any unauthorized access to your wireless network.
    </tr>
    <tr><hr size=1 noshade align=top></tr>
<table width="540" height="25" border="0" cellpadding="0" cellspacing="0">
<tr> 
<td bgcolor="#FFFFFF" height="20" class="MainTD"><font size="2"><b>Select SSID:&nbsp;&nbsp;</b>
<select name="SSID_Setting" onChange="SSIDSelected( this.selectedIndex )">
<script>SetSSIDSettingOption()</script>
</select>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
	 <input type="button" value="Apply Changes" name="save" onClick="ValidateForm(document.formEncrypt, 0);">&nbsp;&nbsp;
	 <input type="button" value="Reset" name="reset1" onClick="ValidateForm(document.formEncrypt, 1);">
</td>
</tr>
</table>
<table width="540" height="25" border="0" cellpadding="0" cellspacing="0">
<tr> 
<td bgcolor="#FFFFFF" colspan="2" height="20"> 
<hr color="#B5B5E6" size="1">
</td>
</tr>
</table>
<table width="540" border="0" cellpadding="0" cellspacing="0">
<tr> 
<td bgcolor="#FFFFFF" rowspan="12" width="540">
<iframe width="540" height="400" id="SSIDAuthMode" name="SSIDAuthMode" frameborder="0" marginheight="0" scrolling="no"> 
</iframe> </td>
</tr>
</table>
</table>

<span id = "wpa_auth" class = "off" >
<table>
<input name="wpaAuth<% write(getIndex("wlan_idx")); %>" type="radio" value="eap" >PSK
<input name="wpaAuth<% write(getIndex("wlan_idx")); %>" type="radio" value="psk">Pre-Shared&nbsp;Key
<input type="checkbox" name="ciphersuite<% write(getIndex("wlan_idx")); %>" value="tkip">TKIP
<input type="checkbox" name="ciphersuite<% write(getIndex("wlan_idx")); %>" value="aes" >AES
<input type="checkbox" name="wpa2ciphersuite<% write(getIndex("wlan_idx")); %>" value="tkip">TKIP
<input type="checkbox" name="wpa2ciphersuite<% write(getIndex("wlan_idx")); %>" value="aes" >AES
</table>
</span>

<input type=hidden name=wps_clear_configure_by_reg<% write(getIndex("wlan_idx")); %> value=0>
<input type="hidden" id="wlan_ssid" name="wlan_ssid" value="">
<input type="hidden" id="wlan_ssid_id" name="wlan_ssid_id" value="">        
<input type="hidden" id="wlan_security_mode" name="method<% write(getIndex("wlan_idx")); %>" value="">
<input type="hidden" id="wlan_authtype" name="authType" value="">
<input type="hidden" id="wlan_wepkeylength" name="wepKeyLen<% write(getIndex("wlan_idx")); %>" value="">
<input type="hidden" id="wlan_wepkeyfmt" name="wlan_wepkeyfmt" value="">
<input type="hidden" id="wlan_wepdefaultkey" name="wlan_wepdefaultkey" value="">
<input type="hidden" id="wlan_wepkey" name="wlan_wepkey" value="">

<input type="hidden" id="wepEnabled" name="wepEnabled<% write(getIndex("wlan_idx")); %>" value="ON">
<input type="hidden" id="length" name="length<% write(getIndex("wlan_idx")); %>" value="">
<input type="hidden" id="format" name="format<% write(getIndex("wlan_idx")); %>" value="">
<input type="hidden" id="key" name="key<% write(getIndex("wlan_idx")); %>" value="">
<input type="hidden" id="wlan_wpa_psk_fmt" name="pskFormat<% write(getIndex("wlan_idx")); %>" value="">
<input type="hidden" id="wlan_wpa_psk" name="pskValue<% write(getIndex("wlan_idx")); %>" value="">
<input type="hidden" id="wlan_wpa_preAuth" name="preAuth<% write(getIndex("wlan_idx")); %>" value="">
<input type="hidden" id="wlan_radius_server_ip" name="radiusIP<% write(getIndex("wlan_idx")); %>" value="">	
<input type="hidden" id="wlan_radius_server_port" name="radiusPort<% write(getIndex("wlan_idx")); %>" value="">	
<input type="hidden" id="wlan_radius_server_psk" name="radiusPass<% write(getIndex("wlan_idx")); %>" value="">	
<input type="hidden" id="wlan_ieee8021x" name="use1x<% write(getIndex("wlan_idx")); %>" value="">
<input type="hidden" value="/wlsecurity.asp" name="submit-url">
</form>
</blockquote>
</body>
</html>
