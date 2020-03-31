<html>
<! Copyright (c) Realtek Semiconductor Corp., 2003. All Rights Reserved. ->
<head>
<meta http-equiv="Content-Type" content="text/html">
<title>WAN Interface Setup </title>
<style>
.on {display:on}
.off {display:none}
</style>
<% language=javascript %>
<script type="text/javascript" src="common.js"> </script>
<SCRIPT>
var initialDnsMode, pppConnectStatus=0;

function setPPPConnected()
{
   pppConnectStatus = 1;
}
function resetClicked()
{
   if(document.tcpip.wanType.selectedIndex != 0){
	   if(document.tcpip.dnsMode[0].checked)
	      disableDNSinput();
	   else
	      enableDNSinput();
   }

   document.tcpip.reset;
}


function pppConnectClick(connect)
{
  if (document.tcpip.pppConnectType.selectedIndex == 2 && pppConnectStatus==connect) {
      if (document.tcpip.pppUserName.value=="") {
          alert('PPP user name cannot be empty!');
	  document.tcpip.pppUserName.value = document.tcpip.pppUserName.defaultValue;
	  document.tcpip.pppUserName.focus();
	  return false;
      }
      if (document.tcpip.pppPassword.value=="") {
          alert('PPP password cannot be empty!');
	  document.tcpip.pppPassword.value = document.tcpip.pppPassword.defaultValue;
	  document.tcpip.pppPassword.focus();
	  return false;
      }
      return true;
  }
  return false;
}

function pppTypeSelection()
{
  if ( document.tcpip.pppConnectType.selectedIndex == 2) {
  	if (pppConnectStatus==0) {
  		enableButton(document.tcpip.pppConnect);
		disableButton(document.tcpip.pppDisconnect);
	}
	else {
 		disableButton(document.tcpip.pppConnect);
		enableButton(document.tcpip.pppDisconnect);
	}
	disableTextField(document.tcpip.pppIdleTime);
  }
  else {
	disableButton(document.tcpip.pppConnect);
	disableButton(document.tcpip.pppDisconnect);
	if (document.tcpip.pppConnectType.selectedIndex == 1)
		enableTextField(document.tcpip.pppIdleTime);
	else
		disableTextField(document.tcpip.pppIdleTime);
  }
}
function wanTypeSelection(field)
{
  //document.tcpip.submit();
  if(!document.getElementById){
  	alert('Error! Your browser must have CSS support !');
  	return;
  }
  if(field.selectedIndex == 0){	//static ip
  	wanShowDiv(0 ,1, 0, 0, 1); //pptp, dns, dnsMode, pppoe, static (div)
	enableDNSinput();	
	document.tcpip.dnsMode[1].checked = true ; // dns change to manual mode
  }
  else if(field.selectedIndex == 1){ //Dhcp
  	wanShowDiv(0 ,1, 1, 0, 0);   
	if(document.tcpip.dnsMode[0].checked)
		disableDNSinput();	
  }else if(field.selectedIndex == 2){ //pppoe
  	wanShowDiv(0 ,1, 1, 1, 0); 
	if(document.tcpip.dnsMode[0].checked)
		disableDNSinput();	
  
  }else if(field.selectedIndex == 3){ //pptp
  	wanShowDiv(1, 1, 1, 0, 0); 
	if(document.tcpip.dnsMode[0].checked)
		disableDNSinput();	
  }
  
}


</SCRIPT>
</head>

<body>
<blockquote>
<h2><font color="#0000FF">WAN Interface Setup</font></h2>
<form action=/goform/formWanTcpipSetup method=POST name="tcpip">

<table border=0 width="550" cellspacing=0 cellpadding=0>
  <tr><font size=2>
    This page is used to configure the parameters for Internet network which
    connects to the WAN port of your Access Point. Here you may change the access
    method to static IP, DHCP, PPPoE or PPTP by click the item value of WAN Access
    type.
  </tr>
  <tr><hr size=1 noshade align=top></tr>
  <tr>
</table>
 <input type="hidden" value="pptp" name="ipMode">

  <table border="0" width=480>
    <tr>
       <td width="30%" height=40><font size=2><b>WAN Access Type:</b></td>
       <td width="70%"><font size=2><select size="1" name="wanType" onChange="wanTypeSelection(this)">
        <option  <% if (getIndex("wanDhcp")==0) write("selected"); %> value="fixedIp">Static IP</option>
	<option <% if (getIndex("wanDhcp")==1) write("selected"); %> value="autoIp">DHCP Client</option>
 	<option <% if (getIndex("wanDhcp")==3) write("selected"); %> value="ppp">PPPoE</option>
	<option <% if (getIndex("wanDhcp")==4) write("selected"); %> value="pptp">PPTP</option>
	</select></td>
    <tr>
  </table>
  <span id = "static_div" class = "off" >  
  <table border="0" width=480>
    <tr>
       <td width="30%"><font size=2><b>IP Address:</b></td>
       <td width="70%"><font size=2>
        <input type="text" name="wan_ip" size="18" maxlength="15" value="<% getInfo("wan-ip-rom"); %>">
      </td>
    </tr>
    <tr>
      <td width="30%"><font size=2><b>Subnet Mask:</b></td>
      <td width="70%"><font size=2><input type="text" name="wan_mask" size="18" maxlength="15" value="<% getInfo("wan-mask-rom"); %>"></td>
    </tr>
    <tr>
      <td width="30%"><font size=2><b>Default Gateway:</b></td>
      <td width="70%"><font size=2><input type="text" name="wan_gateway" size="18" maxlength="15" value="<% getInfo("wan-gateway-rom"); %>"></td>
    </tr>
    <tr>
      <td width="30%"><font size=2><b>MTU Size:</b></td>
      <td width="70%"><font size=2><input type="text" name="fixedIpMtuSize" size="10" maxlength="10" value="<% getInfo("fixedIpMtuSize"); %>">&nbsp;(1400-1500 bytes)</td>
    </tr>
  </table>  
  </span>
  
  <span id = "dhcp_div" class = "off" >  
  <table border="0" width=480>
    <tr>
      <td width="30%"><font size=2><b>Host Name:</b></td>
      <td width="70%"><font size=2><input type="text" name="hostName" size="18" maxlength="30" value="<% getInfo("hostName"); %>"></td>
    </tr>  
    <tr>
      <td width="30%"><font size=2><b>MTU Size:</b></td>
      <td width="70%"><font size=2><input type="text" name="dhcpMtuSize" size="10" maxlength="10" value="<% getInfo("dhcpMtuSize"); %>">&nbsp;(1400-1492 bytes)</td>
    </tr>
  </table>  
  </span>

  <span id = "pppoe_div" class = "off" >  
  <table border="0" width=480>
     <tr>
      <td width="30%"><font size=2><b>User Name:</b></td>
      <td width="70%"><font size=2><input type="text" name="pppUserName" size="18" maxlength="128" value="<% getInfo("pppUserName"); %>"></td>
    </tr>
    <tr>
      <td width="30%"><font size=2><b>Password:</b></td>
      <td width="70%"><font size=2><input type="password" name="pppPassword" size="18" maxlength="128" value="<% getInfo("pppPassword"); %>"></td>
    </tr>
    <tr>
      <td width="30%"><font size=2><b>Service Name:</b></td>
      <td width="70%"><font size=2><input type="text" name="pppServiceName" size="18" maxlength="40" value="<% getInfo("pppServiceName"); %>"></td>
    </tr>
    <tr>
      <td width="30%"><font size=2><b>Connection Type:</b></td>
      <td width="70%"><font size=2><select size="1" name="pppConnectType" onChange="pppTypeSelection()">
        <% var type = getIndex("pppConnectType");
	     	if ( type == 0 ) {
	      	  	write( "<option selected value=\"0\">Continuous</option>" );
    	   	  	write( "<option value=\"1\">Connect on Demand</option>" );
    		  	write( "<option value=\"2\">Manual</option>" );
	     	}
	     	if ( type == 1 ) {
	      	  	write( "<option value=\"0\">Continuous</option>" );
    	   	  	write( "<option selected value=\"1\">Connect on Demand</option>" );
    		  	write( "<option value=\"2\">Manual</option>" );
	     	}
	     	if ( type == 2 ) {
	      	  	write( "<option value=\"0\">Continuous</option>" );
    	   	  	write( "<option value=\"1\">Connect on Demand</option>" );
    		  	write( "<option selected value=\"2\">Manual</option>" );
	     	}  %>
        </select>&nbsp;&nbsp;
	<input type="submit" value="Connect" name="pppConnect" onClick="return pppConnectClick(0)">&nbsp;&nbsp;
	<input type="submit" value="Disconnect" name="pppDisconnect" onClick="return pppConnectClick(1)">
	<% if ( getIndex("pppConnectStatus") ) write("\n<script> setPPPConnected(); </script>\n"); %>
	</td>
    </tr>
    <tr>
      <td width="30%"><font size=2><b>Idle Time:</b></td>
      <td width="70%"><font size=2><input type="text" name="pppIdleTime" size="10" maxlength="10" value="<% getInfo("wan-ppp-idle"); %>">&nbsp;(1-1000 minutes)</td>
    </tr>
    <tr>
      <td width="30%"><font size=2><b>MTU Size:</b></td>
      <td width="70%"><font size=2><input type="text" name="pppMtuSize" size="10" maxlength="10" value="<% getInfo("pppMtuSize"); %>">&nbsp;(1360-1492 bytes)</td>
    </tr>
  </table>
  </span>
      
  <span id = "pptp_div" class = "off" >  
  <table border="0" width=480>
      <td width="30%"><font size=2><b>IP Address:</b></td>
      <td width="70%"><font size=2><input type="text" name="pptpIpAddr" size="18" maxlength="30" value="<% getInfo("pptpIp"); %>"></td>
    </tr>
    <tr>
      <td width="30%"><font size=2><b>Subnet Mask:</b></td>
      <td width="70%"><font size=2><input type="text" name="pptpSubnetMask" size="18" maxlength="30" value="<% getInfo("pptpSubnet"); %>"></td>
    </tr>
    <tr>
      <td width="30%"><font size=2><b>Server IP Address:</b></td>
      <td width="70%"><font size=2><input type="text" name="pptpServerIpAddr" size="18" maxlength="30" value="<% getInfo("pptpServerIp"); %>"></td>
    </tr>
    <tr>
      <td width="30%"><font size=2><b>User Name:</b></td>
      <td width="70%"><font size=2><input type="text" name="pptpUserName" size="18" maxlength="128" value="<% getInfo("pptpUserName"); %>"></td>
    </tr>
    <tr>
      <td width="30%"><font size=2><b>Password:</b></td>
      <td width="70%"><font size=2><input type="password" name="pptpPassword" size="18" maxlength="128" value="<% getInfo("pptpPassword"); %>"></td>
    </tr>
     <tr>
      <td width="30%"><font size=2><b>MTU Size:</b></td>
      <td width="70%"><font size=2><input type="text" name="pptpMtuSize" size="10" maxlength="10" value="<% getInfo("pptpMtuSize"); %>">&nbsp;(1400-1460 bytes)</td>
    </tr>    
	<tr>
      <td width="100%" colspan="2"><font size=2>
        <b><input type="checkbox" name="pptpSecurity" <% if (getIndex("pptpSecurity")) write("checked"); %> value="ON">&nbsp;&nbsp;Request MPPE Encryption</b></td>
    </tr>
    <tr></tr><tr></tr><tr></tr>    
    </table>
    </span>
    
    <span id = "dnsMode_div" class = "off" >
    <table border="0" width=480>
    <!--
    <tr height="35" valign="bottom">
    -->
    <tr>
      <td width="100%" colspan="2"><font size=2>
        <b><input type="radio" value="dnsAuto" name="dnsMode" <% if (getIndex("wanDNS")==0) write("checked"); %> onClick="autoDNSclicked()">Attain DNS Automatically</b>
      </td>
    </tr>
    <tr>
      <td width="100%" colspan="2"><font size=2>
        <b><input type="radio" value="dnsManual" name="dnsMode" <% if (getIndex("wanDNS")==1) write("checked"); %> onClick="manualDNSclicked()">Set DNS Manually</b>
      </td>
    </tr>
    <tr>
    </table>
    </span>
    
    <span id = "dns_div" class = "off" >
    <table border="0" width=480>
       <td width="30%"><font size=2><b>&nbsp;&nbsp;&nbsp;DNS 1:</b></td>
       <td width="70%"><font size=2><input type="text" name="dns1" size="18" maxlength="15" value=<% getInfo("wan-dns1"); %>></td>
    </tr>
    <tr>
       <td width="30%"><font size=2><b>&nbsp;&nbsp;&nbsp;DNS 2:</b></td>
       <td width="70%"><font size=2><input type="text" name="dns2" size="18" maxlength="15" value=<% getInfo("wan-dns2"); %>></td>
    </tr>
    <tr>
       <td width="30%"><font size=2><b>&nbsp;&nbsp;&nbsp;DNS 3:</b></td>
       <td width="70%"><font size=2><input type="text" name="dns3" size="18" maxlength="15" value=<% getInfo("wan-dns3"); %>></td>
    </tr>
    </table>
    </span>
    
    <table border="0" width=480>
    <tr> 
       <td width="30%"><font size=2><b>Clone MAC Address:</b></td>
       <td width="70%"><font size=2><input type="text" name="wan_macAddr" size="18" maxlength="12" value=<% getInfo("wanMac"); %>></td>
    </tr>
    <tr>
      <td width="100%" colspan="2"><font size=2>
        <b><input type="checkbox" name="upnpEnabled" value="ON" <% if (getIndex("upnpEnabled")) write("checked");
           %>>&nbsp;&nbsp;Enable uPNP</b></td>
    </tr>
    <tr>
      <td width="100%" colspan="2"><font size=2>
        <b><input type="checkbox" name="pingWanAccess" value="ON" <% if (getIndex("pingWanAccess")) write("checked");
           %>>&nbsp;&nbsp;Enable Ping Access on WAN</b></td>
    </tr>
    <tr>
      <td width="100%" colspan="2"><font size=2>
        <b><input type="checkbox" name="webWanAccess" value="ON" <% if (getIndex("webWanAccess")) write("checked");
           %>>&nbsp;&nbsp;Enable Web Server Access on WAN</b></td>
    </tr>
    <tr>
      <td width="100%" colspan="2"><font size=2>
        <b><input type="checkbox" name="WANPassThru1" <% if (getIndex("VPNPassThruIPsec")) write("checked"); %> value="ON">&nbsp;&nbsp;Enable IPsec pass through on VPN connection</b></td>
    </tr>
    <tr>
      <td width="100%" colspan="2"><font size=2>
        <b><input type="checkbox" name="WANPassThru2" <% if (getIndex("VPNPassThruPPTP")) write("checked"); %> value="ON">&nbsp;&nbsp;Enable PPTP pass through on VPN connection</b></td>
    </tr>
    <td width="100%" colspan="2"><font size=2>
        <b><input type="checkbox" name="WANPassThru3" <% if (getIndex("VPNPassThruL2TP")) write("checked"); %> value="ON">&nbsp;&nbsp;Enable L2TP pass through on VPN connection</b></td>
    </tr>
   </table>

   <SCRIPT>
/*
   if(document.tcpip.wanType.selectedIndex != 0){ // static ip
	   group = document.tcpip.dnsMode;
	   for (var r = 0; r < group.length; r++)
		  if (group[r].checked)
		    break;
	   initialDnsMode = r;
	   if (initialDnsMode == 0)
	      disableDNSinput();
   }
*/
   wanTypeSelection(document.tcpip.wanType);
   pppTypeSelection();
  </SCRIPT>

  <BR>
  <input type="hidden" value="/tcpipwan.asp" name="submit-url">
  <p><input type="submit" value="Apply Changes" name="save" onClick="return saveChanges_wan(document.tcpip)">&nbsp;&nbsp;
  <input type="reset" value="Reset" name="reset" onClick="resetClicked()">
</p>
</form>
</blockquote>
</body>
</html>
