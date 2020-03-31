<html>
<! Copyright (c) Realtek Semiconductor Corp., 2003. All Rights Reserved. ->
<head>
<meta http-equiv="Content-Type" content="text/html">
<title>VPN Setup </title>
<% language=javascript %>
<script type="text/javascript" src="common.js"> </script>
<script>
var vpnEnabled=0 ;
function  setVpnEnabled() {
	vpnEnabled = 1;
} 

function showRsaKey(url) {

	openWindow(url, 'VPNLog', 700, 300);
}
function vpnEnabledClick(){
	if(document.formVpn.enabled.checked){
		enableButton(document.formVpn.editVpn);
		enableButton(document.formVpn.deleteVpn);
		enableButton(document.formVpn.refresh);
		enableButton(document.formVpn.natt_enabled);
		enableButton(document.formVpn.rsa_key);
		//enableButton(document.formVpn.vpnLog);
	}
	else{
		disableButton(document.formVpn.editVpn);
		disableButton(document.formVpn.deleteVpn);
		disableButton(document.formVpn.refresh);
		disableButton(document.formVpn.natt_enabled);
		disableButton(document.formVpn.rsa_key);
		//disableButton(document.formVpn.vpnLog);
	}
}

function deleteClick()
{
	if ( !confirm('Do you really want to delete the selected entry?') ) {
	  	return false;
	}
	else
		return true;
}
</script>
</head>

<blockquote>
<h2><font color="#0000FF">VPN Setup</font></h2>

<body>
<table border=0 width="550" cellspacing=0 cellpadding=0>
  <tr><font size=2>
    This page is used to enable/disable VPN function and select a VPN connection to edit/delete.
  </tr>
  <tr><hr size=1 noshade align=top></tr>

<form action=/goform/formVpnConn method=POST name="formVpn">

  	<tr>
	<td width="200" ><font size=2><b><input type="checkbox" name="enabled" value="ON"  <% if (getIndex("ipsecVpnEnabled")) write("checked"); %> onclick="vpnEnabledClick()">&nbsp;&nbsp;Enable IPSEC VPN</b></font></td>
	<td width="200"  ><font size=2><b><input type="checkbox" name="natt_enabled" value="ON"  <% if (getIndex("ipsecNattEnabled")) write("checked"); %> >&nbsp;&nbsp;Enable NAT Traversal</b></font></td>
	<td width="200"  ><font size=2><b><input type="submit" name="rsa_key" value="Generate RSA Key"></b></font></td>

	</tr>

	<tr>
		<td width="150" colspan=2><font size=2>
	<input type="submit" value="Apply Changes" name="enableVpn" >&nbsp;&nbsp;</font></td>
  	<td width="200"  ><font size=2><b><input type="button" name="showKey" value="Show RSA Public Key" onClick="showRsaKey('/rsakey.asp')">
	</tr>
</table>
	

  <br>
  <table border="0" width=500>
  <tr><font size=2><b>Current VPN Connection Table: &nbsp; &nbsp; &nbsp; WAN IP:<% getInfo("wan-ip"); %></b></font></tr>
  
  </table>	

<table border='1' width="580">
<tr bgcolor="#808080" align=center>
<td width="19"><font size=2><center><p><b></b></p></td>
<td width="19"><font size=2><center><p><b>#</b></p></td>
<td width="60"><font size="2"><b>Name</b></font></td>
<!--<td width="19"><font size="2"><b>Active</b></font></td> -->
<td width="19"><font size="2"><b>Active</b></font></td>
<td width="88"><font size=2><b>Local Address</b></font></td>
<td width="100"><font size=2><b>Remote Address</b></font></td>
<td width="100"><font size=2><b>Remote Gateway</b></font></td>
<td width="70"><font size=2><b>Status</b></font></td>
</tr>
<% vpnConnList(); %>
</table>
  <br>
  <input type="submit" value=" Edit " name="editVpn" >&nbsp;&nbsp;
  <input type="submit" value="Delete" name="deleteVpn" onClick="return deleteClick()">&nbsp;&nbsp; 
  <input type="button" value="Refresh" name="refresh" onClick="javascript: window.location.reload()">&nbsp;&nbsp; 
<!--
  <input type="button" value="Show Log" name="vpnLog" onClick="vpnLogClick('/vpnlog.asp')" >&nbsp;&nbsp;&nbsp;&nbsp;
-->
  <input type="hidden" value="/vpn.asp" name="submit-url">
 <script>
	vpnEnabledClick();
	<% if ( getIndex("ipsecVpnEnabled") ) write(" setVpnEnabled();"); %>
 </script>  
</form>

</body>

</html>
