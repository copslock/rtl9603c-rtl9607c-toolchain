<html>
<! Copyright (c) Realtek Semiconductor Corp., 2003. All Rights Reserved. ->
<head>
<meta http-equiv="Content-Type" content="text/html">
<title>LAN Interface Setup </title>
<% language=javascript %>
<script type="text/javascript" src="common.js"> </script>
<SCRIPT>
var initialDhcp;
function dhcpChange(index)
{
  if ( index != 2 ) {
	  disableTextField(document.tcpip.dhcpRangeStart);
   	  disableTextField(document.tcpip.dhcpRangeEnd);
   	  enableTextField(document.tcpip.lan_gateway);
	  disableButton( document.tcpip.dhcpClientTbl );
   	  disableTextField(document.tcpip.domainName);	  
  }
  else {
   	  enableTextField(document.tcpip.dhcpRangeStart);
   	  enableTextField(document.tcpip.dhcpRangeEnd);
   	  disableTextField(document.tcpip.lan_gateway);
	  enableButton( document.tcpip.dhcpClientTbl );
   	  enableTextField(document.tcpip.domainName);	  
  }
  if ( index == 1 ) {
 	  disableTextField(document.tcpip.lan_ip);
	  disableTextField(document.tcpip.lan_mask);
	  disableTextField(document.tcpip.lan_gateway);
  }
  else {
 	  enableTextField(document.tcpip.lan_ip);
	  enableTextField(document.tcpip.lan_mask);
   	  enableTextField(document.tcpip.lan_gateway);
  }
}

function resetClick()
{
   dhcpChange( initialDhcp );
   document.tcpip.reset;
}


function checkClientRange(start,end)
{
  start_d = getDigit(start,4);
  start_d += getDigit(start,3)*256;
  start_d += getDigit(start,2)*256;
  start_d += getDigit(start,1)*256;

  end_d = getDigit(end,4);
  end_d += getDigit(end,3)*256;
  end_d += getDigit(end,2)*256;
  end_d += getDigit(end,1)*256;

  if ( start_d < end_d )
	return true;

  return false;
}


function saveChanges()
{

  if ( checkIpAddr(document.tcpip.lan_ip, 'Invalid IP address value! ') == false )
	    return false;

  if (checkIPMask(document.tcpip.lan_mask) == false)
  	return false ;

  if ( document.tcpip.dhcp.selectedIndex == 1) {
  	if ( checkIpAddr(document.tcpip.dhcpRangeStart, 'Invalid DHCP client start address! ') == false )
	    return false;

	if ( !checkSubnet(document.tcpip.lan_ip.value,document.tcpip.lan_mask.value,document.tcpip.dhcpRangeStart.value)) {
		alert('Invalid DHCP client start address!\nIt should be located in the same subnet of current IP address.');
		document.tcpip.dhcpRangeStart.value = document.tcpip.dhcpRangeStart.defaultValue;
		document.tcpip.dhcpRangeStart.focus();
		return false;
	}
  	if ( checkIpAddr(document.tcpip.dhcpRangeEnd, 'Invalid DHCP client End address! ') == false )
	    return false;	

	if ( !checkSubnet(document.tcpip.lan_ip.value,document.tcpip.lan_mask.value,document.tcpip.dhcpRangeEnd.value)) {
		alert('Invalid DHCP client end address!\nIt should be located in the same subnet of current IP address.');
		document.tcpip.dhcpRangeEnd.value = document.tcpip.dhcpRangeEnd.defaultValue;
		document.tcpip.dhcpRangeEnd.focus();
		return false;
	}
        if ( !checkClientRange(document.tcpip.dhcpRangeStart.value,document.tcpip.dhcpRangeEnd.value) ) {
		alert('Invalid DHCP client address range!\nEnding address should be greater than starting address.');
		document.tcpip.dhcpRangeStart.focus();
		return false;
        }
   }
   if ( document.tcpip.dhcp.selectedIndex != 1) {
   	if ( document.tcpip.lan_gateway.value=="")
		document.tcpip.lan_gateway.value = '0.0.0.0';
	
	if (document.tcpip.lan_gateway.value!="0.0.0.0") {
		if ( checkIpAddr(document.tcpip.lan_gateway, 'Invalid DHCP client End address! ') == false )
	    		return false;
	    	if ( !checkSubnet(document.tcpip.lan_ip.value,document.tcpip.lan_mask.value,document.tcpip.lan_gateway.value)) {
			alert('Invalid gateway address!\nIt should be located in the same subnet of current IP address.');
			document.tcpip.lan_gateway.value = document.tcpip.lan_gateway.defaultValue;
			document.tcpip.lan_gateway.focus();
			return false;
      		}
	}
  }

  var str = document.tcpip.lan_macAddr.value;
  if ( str.length < 12) {
	alert("Input MAC address is not complete. It should be 12 digits in hex.");
	document.tcpip.lan_macAddr.focus();
	return false;
  }

  for (var i=0; i<str.length; i++) {
    if ( (str.charAt(i) >= '0' && str.charAt(i) <= '9') ||
			(str.charAt(i) >= 'a' && str.charAt(i) <= 'f') ||
			(str.charAt(i) >= 'A' && str.charAt(i) <= 'F') )
			continue;
	alert("Invalid MAC address. It should be in hex number (0-9 or a-f).");
	document.tcpip.lan_macAddr.focus();
	return false;
  }
  return true;
}


function dhcpTblClick(url) {
	if ( document.tcpip.dhcp.selectedIndex == 2) {
		openWindow(url, 'DHCPTbl',600, 400 );
	}
}

</SCRIPT>
</head>

<body>
<blockquote>
<h2><font color="#0000FF">LAN Interface Setup</font></h2>

<form action=/goform/formTcpipSetup method=POST name="tcpip">
<table border=0 width="500" cellspacing=4 cellpadding=0>
  <tr><font size=2>
    This page is used to configure the parameters for local area network which
    connects to the LAN port of your Access Point. Here you may change the setting
    for IP addresss, subnet mask, DHCP, etc..
  </tr>
  <tr><hr size=1 noshade align=top></tr>

  <tr>
      <td width="30%"><font size=2><b>IP Address:</b></td>
      <td width="70%"><input type="text" name="lan_ip" size="15" maxlength="15" value=<% getInfo("ip-rom"); %>></td>
  </tr>

  <tr>
      <td width="30%"><font size=2><b>Subnet Mask:</b></td>
      <td width="70%"><input type="text" name="lan_mask" size="15" maxlength="15" value="<% getInfo("mask-rom"); %>"></td>
  </tr>

  <tr>
      <td width="30%"><font size=2><b>Default Gateway:</b></td>
      <td width="70%"><input type="text" name="lan_gateway" size="15" maxlength="15" value="<% getInfo("gateway-rom"); %>"></td>
  </tr>

  <tr>
      <td width="30%"><font size=2><b>DHCP:</b></td>
      <td width="70%"><select size="1" name="dhcp"
      onChange="dhcpChange(document.tcpip.dhcp.selectedIndex)">
       <%  choice = getIndex("dhcp");
      	  if ( choice == 0 ) {
      	  	write( "<option selected value=\"0\">Disabled</option>" );
		write( "<option value=\"1\">Client</option>" );
      	  	write( "<option value=\"2\">Server</option>" );
      	  }
     	  if ( choice == 1 ) {
      	  	write( "<option value=\"0\">Disabled</option>" );
		write( "<option selected value=\"1\">Client</option>" );
      	  	write( "<option value=\"2\">Server</option>" );
      	  }

       	  if ( choice == 2 ) {
      	  	write( "<option value=\"0\">Disabled</option>" );
		write( "<option value=\"1\">Client</option>" );
      	  	write( "<option selected value=\"2\">Server</option>" );
      	  }
       %>
	   </select>
     </td>
  </tr>

  <tr>
      <td width="30%"><font size=2><b>DHCP Client Range:</b></td>
      <td width="70%"><input type="text" name="dhcpRangeStart" size="12" maxlength="15" value="<% getInfo("dhcpRangeStart"); %>">
      <font face="Arial" size="5">-</font> <input type="text" name="dhcpRangeEnd" size="12" maxlength="15" value="<% getInfo("dhcpRangeEnd"); %>">&nbsp;
        <input type="button" value="Show Client" name="dhcpClientTbl" onClick="dhcpTblClick('/dhcptbl.asp')" >
      </td>
  </tr>

  <tr>
      <td width="30%"><font size=2><b>Domain Name:</b></td>
      <td width="70%"><input type="text" name="domainName" size="15" maxlength="30" value="<% getInfo("domainName"); %>"></td>
  </tr>

  <tr>
      <td width="30%"><font size=2><b>802.1d Spanning Tree:</b></td>
      <td width="70%"><select size="1" name="stp">
       <%  choice = getIndex("stp");
   	  if ( choice == 0 ) {
      	  	write( "<option selected value=\"0\">Disabled</option>" );
   	  		write( "<option value=\"1\">Enabled</option>" );
      	  }
      	  else {
      	  	write( "<option value=\"0\">Disabled</option>" );
   	  		write( "<option selected value=\"1\">Enabled</option>" );
      	  } %>
	    </select>
      </td>
  </tr>

  <tr>
      <td width="30%"><font size=2><b>Clone MAC Address:</b></td>
      <td width="70%"><input type="text" name="lan_macAddr" size="15" maxlength="12" value="<% getInfo("bridgeMac"); %>"></td>
  </tr>

  <SCRIPT>
  	initialDhcp = document.tcpip.dhcp.selectedIndex;
	dhcpChange(initialDhcp);
  </SCRIPT>
  </table>
  <br>
      <input type="submit" value="Apply Changes" name="save" onClick="return saveChanges()">&nbsp;&nbsp;
      <input type="reset" value="Reset" name="reset" onClick="resetClick()">
      <input type="hidden" value="/tcpiplan.asp" name="submit-url">
 </form>
</blockquote>
</body>

</html>
