<html>
<! Copyright (c) Realtek Semiconductor Corp., 2003. All Rights Reserved. ->
<head>
<meta http-equiv="Content-Type" content="text/html">
<title>Static DHCP Setup</title>
<script type="text/javascript" src="util_ap.js"> </script>
<script>
function addClick()
{
  var str = document.formStaticDHCPAdd.mac_addr.value;
   if ( checkIpAddr(document.formStaticDHCPAdd.ip_addr, 'Invalid IP address value! ') == false )
      	    return false;
   if ( str.length < 12) {
	alert("Input MAC address is not complete. It should be 12 digits in hex.");
	document.formStaticDHCPAdd.mac_addr.focus();
	return false;
  }
  for (var i=0; i<str.length; i++) {
    if ( (str.charAt(i) >= '0' && str.charAt(i) <= '9') ||
			(str.charAt(i) >= 'a' && str.charAt(i) <= 'f') ||
			(str.charAt(i) >= 'A' && str.charAt(i) <= 'F') )
			continue;

	alert("Invalid MAC address. It should be in hex number (0-9 or a-f).");
	document.formStaticDHCPAdd.mac_addr.focus();
	return false;
  }   	    
  
  
  return true;
}


function deleteClick()
{
  acl_num = <% write(getIndex("wlanAcNum")); %> ;
  delNum = 0 ;
  for(i=1 ; i <= acl_num ; i++){
  	if(document.formStaticDHCP.elements["select"+i].checked)
  		delNum ++ ;
  }
  if ( !confirm('Do you really want to delete the selected entry?') ) {
	return false;
  }
  else
	return true;
}

function deleteAllClick()
{
   if ( !confirm('Do you really want to delete the all entries?') ) {
	return false;
  }
  else
	return true;
}
function disableDelButton()
{
	disableButton(document.formStaticDHCP.deleteSelRsvIP);
	disableButton(document.formStaticDHCP.deleteAllRsvIP);
}

function enableAc()
{
  enableTextField(document.formStaticDHCPAdd.mac_addr);
  enableTextField(document.formStaticDHCPAdd.hostname);
}

function disableAc()
{
  disableTextField(document.formStaticDHCPAdd.mac_addr);
  disableTextField(document.formStaticDHCPAdd.hostname);
}

function updateState()
{
  

}

</script>
</head>
<body>
<blockquote>
<h2><font color="#0000FF">Static DHCP Setup</font></h2>

<table border=0 width="700" cellspacing=4 cellpadding=0>
<tr><font size=2>
<!--
This page allows you setup the Static DHCP. When you specify a reserved
IP address will always receive the same IP address each time the client accesses the server.
Reserved IP address should be assigned to server that require permanent IP settings.
-->
This page allows you reserve IP addresses, and assign the same IP address to the network device 
with the specified MAC address any time it requests an IP address. This is almost the same as when 
a device has a static IP address except that the device must still request an IP address from 
the DHCP server. 
<!--DHCP Reservations are helpful for server computers on the local network that are 
hosting applications such as Web and FTP. Servers on your network should either use a static IP address or use this option. 
-->
</font></tr>

<form action=/goform/formStaticDHCP method=POST name="formStaticDHCPAdd">
<tr><hr size=1 noshade align=top><br></tr>

<table width="100%" border="0" cellpadding="1" cellspacing="0" bgcolor="#FFFFFF">
  <tr>
 <td align="left" width="13%" class=""><font size=2><b>IP Address:</td>
 <td width="87%">
 <input type="text" id="ip_addr" name="ip_addr" size="16" maxlength="15" value="">
 </b>
 </td>
 </tr>
  <tr>
 <td align="left" width="13%" class=""><font size=2><b>MAC Address:</td>
 <td width="87%">
 <input type="text" id="mac_addr" name="mac_addr" size="15" maxlength="12" value=""> 
 </b>
 </td>
 </tr>
 <tr>
 <td align="left" width="13%" class=""><font size=2><b>Comment:</b></td>
 <td width="87%">
 <input type="text" id="hostname" name="hostname" size="20" maxlength="19" value="">
 </font>
 </td>
 </tr>
 </table>
<!--
<tr><td>
     <p><font size=2><b>IP Address: </b><input type="text" name="ip" size="15" maxlength="15">&nbsp;&nbsp;</font>
     <font size=2><b>MAC Address: </b><input type="text" name="mac" size="15" maxlength="12">&nbsp;&nbsp;</font>
   	<b><font size=2>Comment: </b> <input type="text" name="comment" size="16" maxlength="20"></font>
     </p>
     </td>
     </tr>
-->     
     <p><input type="submit" value="Apply Changes" name="addRsvIP" onClick="return addClick()">&nbsp;&nbsp;
        <input type="reset" value="Reset" name="reset">&nbsp;&nbsp;&nbsp;
        <input type="hidden" value="/tcpip_staticdhcp.asp" name="submit-url">
     </p>
  </form>
</table>
<br>
<form action=/goform/formStaticDHCP method=POST name="formStaticDHCP">
  <table border="0" width=640>
  <tr><font size=2><b>Static DHCP List:</b></font></tr>
  <% dhcpRsvdIp_List();%>
  </table>
  <br>
  <input type="submit" value="Delete Selected" name="deleteSelRsvIP" onClick="return deleteClick()">&nbsp;&nbsp;
  <input type="submit" value="Delete All" name="deleteAllRsvIP" onClick="return deleteAllClick()">&nbsp;&nbsp;&nbsp;
  <input type="reset" value="Reset" name="reset">
  <input type="hidden" value="/tcpip_staticdhcp.asp" name="submit-url">
  <!--
 <script>
   	<% entryNum = getIndex("wlanAcNum");
   	  if ( entryNum == 0 ) {
      	  	write( "disableDelButton();" );
       	  } %>
	updateState();
 </script>
 -->
</form>

</blockquote>
</body>
</html>
