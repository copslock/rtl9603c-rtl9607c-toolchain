<html>
<! Copyright (c) Realtek Semiconductor Corp., 2003. All Rights Reserved. ->
<head>
<meta http-equiv="Content-Type" content="text/html">
<title>Setup Wizard</title>
<script type="text/javascript" src="common.js"></script>
<style>
.on {display:on}
.off {display:none}
</style>
<script>
//for wlan1 page
var wlan_idx=<% write(getIndex("wlan_idx")); %> ;
var wlan_num=<% write(getIndex("wlan_num")); %> ;
var opmode=<% write(getIndex("opMode")); %> ;
var wispWanId = <% write(getIndex("wispWanId")); %> ;
var WiFiTest=<% write(getIndex("WiFiTest")); %> ;
var defPskLen=new Array()
var defPskFormat=new Array();
var autoconf=new Array();
var regDomain=new Array();
var defaultChan=new Array();
var lastBand=new Array();
var usedBand=new Array();
var RFType=new Array();
var APMode=new Array();
var bandIdx=new Array();
var bandIdxAP=new Array();
var bandIdxClient=new Array();
var startChanIdx=new Array();
var disableSSID=new Array();
var wlanDisabled=new Array();
var networkType=new Array();
var keyinstall=new Array();
var ssid=new Array();
var encrypt=new Array();
var wep=new Array();
var defaultKeyId=new Array();
var pskValue=new Array();
var macClone=new Array();
var keyType=new Array();
var autoCfgWlanMode=new Array();
/*
autoconf[wlan_idx]= 0;
lastBand[wlan_idx] = 0;
bandIdxAP[wlan_idx] = -1;
bandIdxClient[wlan_idx] = -1;
startChanIdx[wlan_idx] = 0;
disableSSID[wlan_idx] = 0;
*/

// for WPS ------------------------------------------------>>
var wps_disabled=<% write(getIndex("wscDisable"));%>;
var wps_ssid_old='<% getInfo("ssid"); %>';
var wps_mode_old=<% write(getIndex("wlanMode")); %>;
var wps_config_by_registrar=<% write(getIndex("wps_by_reg")); %>;
var wps_encrypt_old=<% write(getIndex("encrypt")); %>;
var wps_wpaCipher_old=<% write(getIndex("wpaCipher"));%>;
var wps_wpa2Cipher_old=<% write(getIndex("wpa2Cipher"));%>;
var wps_psk_old='<% getInfo("pskValue");%>';
var wps_psk_unmask_old='<% getInfo("pskValueUnmask");%>';
var wps_type_old=<% write(getIndex("networkType"));%>;
var wps_enable1x=<% write(getIndex("enable1X"));%>;
var wps_wpa_auth=<% write(getIndex("wpaAuth"));%>;
var wps_wizard=1;
//<<------------------------------------------------- for WPS

function saveChanges_lan(form)
{
  if ( checkIpAddr(form.lan_ip, 'Invalid IP address value! ') == false )
	    return false;
  if (checkIPMask(form.lan_mask) == false)
  	return false ;
   return true;
}

function disable_all(form, wlan_id)
{
  disableTextField(form.elements["method"+wlan_id]);
  disableTextField(form.elements["pskFormat"+wlan_id]);
  disableTextField(form.elements["pskValue"+wlan_id]);
}

function checkState(wlan_id)
{
  form = document.wizard;
  if (disableSSID[wlan_id]) {
  	disable_all(form,wlan_id);
  }
  else {
	if(form.elements["method"+wlan_id].selectedIndex==0){
  		show_div(false,("wpa_div"+wlan_id));	
  		show_div(false,("wep_div"+wlan_id));	
	document.getElementById(("wep_btn"+wlan_id)).style.top = "260px";
  }
  	else if(form.elements["method"+wlan_id].selectedIndex==1){
  		show_div(false,("wpa_div"+wlan_id));	
  	show_div(true,("wep_div"+wlan_id));	
	document.getElementById(("wep_btn"+wlan_id)).style.top = "350px";

  }
  	else if(form.elements["method"+wlan_id].selectedIndex>=2){
  		show_div(true,("wpa_div"+wlan_id));	
  	show_div(false,("wep_div"+wlan_id));	
	document.getElementById(("wep_btn"+wlan_id)).style.top = "260px";

  }
}
}

function updateFormat(form, wlan_id)
{
  if (form.elements["length"+wlan_id].selectedIndex == 0) {
	form.elements["format"+wlan_id].options[0].text = 'ASCII (5 characters)';
	form.elements["format"+wlan_id].options[1].text = 'Hex (10 characters)';
  }
  else {
	form.elements["format"+wlan_id].options[0].text = 'ASCII (13 characters)';
	form.elements["format"+wlan_id].options[1].text = 'Hex (26 characters)';
  }
  form.elements["format"+wlan_id].selectedIndex =  	keyType[wlan_id];
  
   setDefaultKeyValue(form, wlan_id);
}
function setDhcpRange()
{
	// cal the bigger ip range 
	form = document.wizard;
	//check ip
	val = getDigit(form.lan_ip.value,4) ;
	//check mask
	mask_end = (255 - getDigit(form.lan_mask.value,4)) + (getDigit(form.lan_mask.value,4)&val);
	mask_start = (getDigit(form.lan_mask.value,4)&val);
	// val will in range of mask start, end
	range1 = mask_end - val; // val+1 to mask_end
	range2 = val-mask_start;	    // 1 to val-1
	if((mask_end - mask_start) < 3){
		form.dhcp.value = 0 ; //disable
		return false ;
	}	
	dhcpStart = (range1 > range2)? (val+1):(mask_start+1) ;
	dhcpEnd = (range1 > range2)? (mask_end-1): (val-1);

	form.dhcpRangeStart.value = getDigit(form.lan_ip.value,1) + "." + getDigit(form.lan_ip.value,2)
	+"." + getDigit(form.lan_ip.value,3) + "."+ dhcpStart ;
	
	form.dhcpRangeEnd.value = getDigit(form.lan_ip.value,1) + "." + getDigit(form.lan_ip.value,2) +"." + getDigit(form.lan_ip.value,3) + "." +dhcpEnd ;


}
function wanTypeSelection(field)
{
  if(!document.getElementById){
  	alert('Error! Your browser must have CSS support !');
  	return;
  }
  if(field.selectedIndex == 0){	//static ip
  	wanShowDiv(0 ,1, 0, 0, 1); //pptp, dns, dnsMode, pppoe, static (div)
	document.getElementById("wan_btn").style.top = "320px";
  }
  else if(field.selectedIndex == 1){//Dhcp
  	wanShowDiv(0 ,0, 0, 0, 0); 
	document.getElementById("wan_btn").style.top = "260px";
  }
  else if(field.selectedIndex == 2){ //pppoe
  	wanShowDiv(0 ,0, 0, 1, 0); 
	document.getElementById("wan_btn").style.top = "260px";
  }
  else if(field.selectedIndex == 3){ //pptp
  	wanShowDiv(1 ,0, 0, 0, 0); 
	document.getElementById("wan_btn").style.top = "320px";
  }

}

function wizardHideDiv()
{

	show_div(false, "opmode_div");
	show_div(false, "top_div");
	show_div(false, "lan_div");
	show_div(false, "ntp_div");
	show_div(false, "wan_div");
	<% 
	   var i ;
	   for (i=0; i<getIndex("wlan_num"); i=i+1) 
	     { 
		write('show_div(false, "wlan1_div'+i+'");\n'); 
		write('show_div(false, "wlan2_div'+i+'");\n'); 
	     } 
	%>
  }
function saveClick_opmode(next){
	form =  document.wizard;
	wizardHideDiv();
	if(next)
		show_div(true, ("ntp_div"));
	else
		show_div(true, "top_div");
}
function saveClick_ntp(next){
	form =  document.wizard;
	wizardHideDiv();
	if(next)
		show_div(true, ("lan_div"));
	else
		show_div(true, "opmode_div");
}
function saveClick_lan(next){
	form =  document.wizard;
	setDhcpRange();
	if(saveChanges_lan(form) ==false)
		return false ;
	else{
		wizardHideDiv();
		if(next)
			show_div(true, ("wan_div"));
		else
			show_div(true, "ntp_div");
	}			
}
function saveClick_wan(next){
	form =  document.wizard;
	if(saveChanges_wan(form) ==false)
		return false ;
	else{
		wizardHideDiv();
		if(next)
			show_div(true, ("wlan1_div0"));
		else
			show_div(true, ("lan_div"));
	}
}
function saveClick_wlan1(next, wlan_id){
	form =  document.wizard;
	if(saveChanges_basic(form,wlan_id) ==false)
		return false ;
	else{
		wizardHideDiv();
		if(next)
			show_div(true, ("wlan2_div"+wlan_id));
		else{
			if(wlan_id == 0)
				show_div(true, "wan_div");
		else
				show_div(true, ("wlan2_div"+(wlan_id-1)));
}
	}
	
		
}
function cancelClick(){
	
	window.location="wizard.asp" ;
}
function saveClick_wlan2(next, wlan_id)
{

	form =  document.wizard;
	if(form.elements["method"+wlan_id].selectedIndex == 2){
		if(saveChanges_wpa(form, wlan_id) == false)
			return false ;
	}else if(form.elements["method"+wlan_id].selectedIndex == 1){
		if(saveChanges_wep(form, wlan_id) == false)
			return false ;
	}
	if(form.elements["method"+wlan_id].selectedIndex == 0)
		form.elements["wepEnabled"+wlan_id].value =  "OFF" ;
	else
		form.elements["wepEnabled"+wlan_id].value =  "ON" ;
	
	wizardHideDiv();
	if(next != 1) //prev
		show_div(true, ("wlan1_div"+wlan_id));
	else if (next == 1 && wlan_id < (wlan_num-1)) //next
	//not last next
		show_div(true, ("wlan1_div"+(wlan_id+1)));

	return true ;
}
function opModeClick(mode)
{
	form = document.wizard ;
	wlan_id = form.wispWanId.selectedIndex;
	if(mode == 2){ //WISP mode
		// infrastructure client mode
		 form.elements["mode"+wlan_id].selectedIndex = 1;
		 form.elements["type"+wlan_id].selectedIndex = 0;
	}
	else{    // reset default
		 form.elements["mode"+wlan_id].selectedIndex =  form.elements["mode"+wlan_id].defaultSelected;
		 form.elements["type"+wlan_id].selectedIndex =  form.elements["type"+wlan_id].defaultSelected;

	}
	updateMode(document.wizard, wlan_id);
	updateType(document.wizard, wlan_id);
	updateWISPWan();	

}
function updateWISPWan()
{
	if(wlan_num > 1){
		if(form.opMode[2].checked)
			enableTextField(form.wispWanId);
		else
			disableTextField(form.wispWanId);
	}
}
</script>
</head>
<body >
<blockquote>


<form action=/goform/formWizard method=POST name="wizard">
<!-- for WPS -->
<INPUT type=hidden name=wps_clear_configure_by_reg<% write(getIndex("wlan_idx")); %> value=0>

<span id = "top_div" class = "on" >
<table  border=0 width="500" cellspacing=4 cellpadding=0>
<tr><td><h2><b><font color="#0000FF">Setup Wizard</font></b></h2></td> </tr>
<tr><td><font size=2>
 The setup wizard will guide  you to configure access point for first time. Please 
 follow the setup wizard step by step.
</font></td></tr>
<tr><td><hr size=1 noshade align=top></td></tr>

<tr ><td colspan="2"> 
<font size=2> <b>
Welcome to Setup Wizard.<br> <br>The Wizard will guide you the through following steps. Begin by clicking on  Next.
</b></font>
</td></tr> 
<tr height=10> <td> </td></tr>
<tr ><td> <font size=2>
		<ol>
		<li>Setup Operation Mode</li> 
		<li>Choose your Time Zone </li> 
		<li>Setup LAN Interface</li>
		<li>Setup WAN Interface</li> 
		<li>Wireless LAN Setting</li>
		<li>Wireless Security Setting</li>
		</ol>
	</font></td> 
</tr>

</td></tr>
</table>
<div style="LEFT: 60px; WIDTH: 300px; POSITION: absolute; TOP: 300px; HEIGHT: 100px">  
<table border=0 width="500" cellspacing=4 cellpadding=0 >
<tr>
  <td colspan="2" align=right> 

	<input type="button" value="  Next>>" name="next" onClick='wizardHideDiv();show_div(true, "opmode_div");'>

  </td>
</tr>
</table>
</div>
</span>
<!-- opmode page -->
<span id = "opmode_div" class = "off" >
<tr><td><h2><b><font color="#0000FF">1. Operation Mode</font></b></h2></td> </tr>
<table border=0 width="500" cellspacing=0 cellpadding=0>
  <tr><td><font size=2>
  You can setup different modes to LAN and WLAN interface for NAT and bridging function. 
  </td>
  </tr>
  <tr><td><hr size=1 noshade align=top></td></tr>
</table>
<table border="0" width=500>
	<tr>
		<td width ="23%" valign="top">
		<input type="radio" value="0" name="opMode" onClick="opModeClick(0)" ></input>
		<font size=2> <b> Gateway: </b> </font>
		</td>
		<td width ="77%">
			<font size=2>In this mode, the device is supposed to connect to internet via ADSL/Cable Modem. The NAT is enabled and PCs in four LAN ports share the same IP to ISP through WAN port. The connection type can be setup in WAN page by using PPPOE, DHCP client, PPTP client or static IP.</font>
		</td>
	</tr>
	<td colspan="2" height="10"></tr>
	<tr>
		<td width ="23%" valign="top">
		<input type="radio" value="1" name="opMode" onClick="opModeClick(1)"></input>
		<font size=2> <b> Bridge: </b> </font>
		</td>
		<td width ="77%">
			<font size=2>In this mode, all ethernet ports and wireless interface are bridged together and NAT function is disabled. All the WAN related function and firewall are not supported.</font>
		</td>
	</tr>
	<td colspan="2" height="10"></tr>

	<tr>
		<td width ="23%" valign="top">
		<input type="radio" value="2" name="opMode" onClick="opModeClick(2)"></input>
		<font size=2> <b> Wireless ISP: </b> </font>
		</td>
		<td width ="77%">
			<font size=2>In this mode, all ethernet ports  are bridged together and the wireless client will connect to ISP access point. The NAT is enabled and PCs in ethernet ports share the same IP to ISP through wireless LAN. You must set the wireless to client mode first and connect to the ISP AP in Site-Survey page.  The connection type can be setup in WAN page by using PPPOE, DHCP client, PPTP client or static IP.</font>
		</td>
	</tr>
	
	<tr>
		<td width ="23%" valign="top"> </td>
		<td width ="77%">
			<script>
			if(wlan_num > 1){
				document.write('<font size=2><b>WAN Interface : </b>');
				document.write('<select size="1" name="wispWanId" onClick="opModeClick(2)">');
				for(i=0 ; i < wlan_num ; i++)
					document.write('<option value="'+i+'">wlan'+(i+1)+'</option>');
				document.write('</select>');
			}
			</script>
		</td>
	</tr>
</table>
<script>
	form = document.wizard;
	opmode = <% write(getIndex("opMode")); %> ;
	form.opMode[opmode].defaultChecked= true;
	form.opMode[opmode].checked= true;
	if(wlan_num > 1){
		form.wispWanId.defaultSelected= wispWanId;
		form.wispWanId.selectedIndex= wispWanId;
	}
	updateWISPWan();
</script>
<div style="LEFT: 60px; WIDTH: 300px; POSITION: absolute; TOP: 390px; HEIGHT: 100px">  
<table border=0 width="500" cellspacing=4 cellpadding=0>  
	<tr> <td colspan="2" align=right>
		  <input type="button" value="  Cancel  " name="cancel" onClick='cancelClick();'>
		  <input type="button" value="<<Back  " name="back" onClick='saveClick_opmode(0)' >
		  <input type="button" value="  Next>>" name="next" onClick='saveClick_opmode(1)'>
  	</td> </tr>
</table>
</div>
</span>
<!-- ntp page -->
<span id = "ntp_div" class = "off" >
<table  border=0 width="500" cellspacing=4 cellpadding=0>
  <tr><td colspan="2"><h2><font color="#0000FF">2. Time Zone Setting</font></h2></td></tr>
  <tr><td colspan="2"><font size=2>

  You can maintain the system time by synchronizing with a public time server over the Internet.
  </tr>
  </td>
  <tr><td colspan="2"><hr size=1 noshade align=top></td></tr>
</table>

<span id = "ntp_enable_div" class = "off" >  
<table  border="0" width=500 cellspacing=0 cellpadding=0>

	<tr><td colspan="2"><font size=2><b>
		<input type="checkbox" name="enabled" 
		value="ON" 
		ONCLICK=updateState_ntp(document.wizard)>&nbsp;&nbsp;Enable NTP client update </b><br>
	    </td>
	</tr>
</table>
</span>
<table  border="0" width=500 cellspacing=4 cellpadding=0>
	<tr ><td height=10> </td> </tr>

	<tr><td width="25%"><font size=2> <b>Time Zone Select : </b> </font></td>
	    <td width="75%">
            <select name="timeZone">
            	<script language="javascript">
            	var i;
            	for(i=0;i<ntp_zone_array.length;i++){
			if (i == ntp_zone_index)
				document.write('<option value="',ntp_zone_array[i].value,'" selected>',ntp_zone_array[i].name,'</option>');
			else
				document.write('<option value="',ntp_zone_array[i].value,'">',ntp_zone_array[i].name,'</option>');
            	}
		        </script>
            </select>
	    </td>
	</tr>
	<tr>
	<td width ="20%">
	<font size=2> <b> NTP server : </b> </font>
	</td>
	<td width ="85%">
		<select name="ntpServerIp1">
			<option value="192.5.41.41">192.5.41.41    - North America</option>
			<option value="192.5.41.209">192.5.41.209   - North America</option>
			<option value="208.184.49.9">208.184.49.9   - North America</option>
			<option value="131.188.3.220">131.188.3.220  - Europe</option>
			<option value="130.149.17.8">130.149.17.8   - Europe</option>
			<option value="203.60.1.2"> 203.60.1.2  - Australia</option>
			<option value="203.117.180.36"> 203.117.180.36 - Asia Pacific</option>
			</select>
		</td>
	</tr>
	<tr><td height="120"></td></tr>
</table>
<script>
			form = document.wizard ;
			form.enabled.checked = 
			<% if (getIndex("ntpEnabled")) write("true;");
			   else write("false;");
			%>
			if(form.enabled.checked == false)
				show_div(true, "ntp_enable_div");
			else
				show_div(false, "ntp_enable_div");
			setTimeZone(form.timeZone, "<% getInfo("ntpTimeZone"); %>");
			setNtpServer(form.ntpServerIp1, "<% getInfo("ntpServerIp1"); %>");
			updateState_ntp(form);

</script>
     <input type="hidden" value="0" name="ntpServerId">
<div style="LEFT: 60px; WIDTH: 300px; POSITION: absolute; TOP: 260px; HEIGHT: 100px">  
<table border=0 width="500" cellspacing=4 cellpadding=0>  
	<tr> <td colspan="2" align=right>
		  <input type="button" value="  Cancel  " name="cancel" onClick='cancelClick();'>
		  <input type="button" value="<<Back  " name="back" onClick='saveClick_ntp(0)' >
		  <input type="button" value="  Next>>" name="next" onClick='saveClick_ntp(1)'>
  	</td> </tr>
</table>
</div>
</span>
<!-- lan page -->
<span id = "lan_div" class = "off" >
<table  border=0 width="500" cellspacing=4 cellpadding=0>
  <tr><td colspan="2"><h2><font color="#0000FF">3. LAN Interface Setup</font></h2></td></tr>
  <tr><td colspan="2"><font size=2>
    This page is used to configure the parameters for local area network which
    connects to the LAN port of your Access Point. Here you may change the setting
    for IP addresss, subnet mask, DHCP, etc..
    </td>
  </tr>
  <tr><td  colspan="2"><hr size=1 noshade align=top></td></tr>

  <tr>
      <td width="30%"><font size=2><b>IP Address:</b></td>
      <td width="70%"><input type="text" name="lan_ip" size="15" maxlength="15" value=<% getInfo("ip-rom"); %>></td>
  </tr>

  <tr>
      <td width="30%"><font size=2><b>Subnet Mask:</b></td>
      <td width="70%"><input type="text" name="lan_mask" size="15" maxlength="15" value="<% getInfo("mask-rom"); %>"></td>
  </tr>
  <tr><td colspan="2" height="135"></td></tr>
  </table>
<div style="LEFT: 60px; WIDTH: 300px; POSITION: absolute; TOP: 260px; HEIGHT: 100px">  
   <table border=0 width="500" >  
	<tr> <td colspan="2" align=right>
		  <input type="button" value="  Cancel  " name="cancel" onClick='cancelClick();'>
		  <input type="button" value="<<Back  " name="back" onClick='return saveClick_lan(0)' >
		  <input type="button" value="  Next>>" name="next"  onClick='return saveClick_lan(1)'>
  	</td> </tr>
  </table>
</div>
</span>  
<!-- wan page -->
<span id = "wan_div" class = "off" >
<table  border=0 width="500" cellspacing=4 cellpadding=0>
  <tr><td colspan="2"><h2><font color="#0000FF">4. WAN Interface Setup</font></h2></td></tr>
  <tr><td colspan="2"><font size=2>
    This page is used to configure the parameters for Internet network which
    connects to the WAN port of your Access Point. Here you may change the access
    method to static IP, DHCP, PPPoE or PPTP by click the item value of WAN Access
    type.
    </td>
  </tr>
  <tr><td colspan="2"><hr size=1 noshade align=top></td></tr>
  <tr>
</table>
 <input type="hidden" value="pptp" name="ipMode">

  <table border="0" width=500>
    <tr>
       <td width="30%" ><font size=2><b>WAN Access Type:</b></td>
       <td width="70%"><font size=2><select size="1" name="wanType" onChange="wanTypeSelection(this)">
        <option  <% if (getIndex("wanDhcp")==0) write("selected"); %> value="fixedIp">Static IP</option>
	<option <% if (getIndex("wanDhcp")==1) write("selected"); %> value="autoIp">DHCP Client</option>
 	<option <% if (getIndex("wanDhcp")==3) write("selected"); %> value="ppp">PPPoE</option>
	<option <% if (getIndex("wanDhcp")==4) write("selected"); %> value="pptp">PPTP</option>
	</select></td>
    <tr>
  </table>
  <span id = "static_div" class = "off" >  
  <table border="0" width=500>
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
  </table>  
  </span>
  <span id = "dhcp_div" class = "off" >  
  </span>
  <span id = "pppoe_div" class = "off" >  
  <table border="0" width=500>
     <tr>
      <td width="30%"><font size=2><b>User Name:</b></td>
      <td width="70%"><font size=2><input type="text" name="pppUserName" size="18" maxlength="128" value="<% getInfo("pppUserName"); %>"></td>
    </tr>
    <tr>
      <td width="30%"><font size=2><b>Password:</b></td>
      <td width="70%"><font size=2><input type="password" name="pppPassword" size="18" maxlength="128" value="<% getInfo("pppPassword"); %>"></td>
    </tr>
      </table>
  </span>
      
  <span id = "pptp_div" class = "off" >  
  <table border="0" width=500>
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
        </table>
    </span>
    
    <span id = "dnsMode_div" class = "off" >
    <table border="0" width=500>

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
    <table border="0" width=500>
    <tr>
       <td width="30%"><font size=2><b>DNS :</b></td>
       <td width="70%"><font size=2><input type="text" name="dns1" size="18" maxlength="15" value=<% getInfo("wan-dns1"); %>></td>
    </tr>
    
    </table>
    </span>
<div id="wan_btn" style="LEFT: 60px; WIDTH: 300px; POSITION: absolute; TOP: 260px; HEIGHT: 100px">  
    <table border="0" width=500>
	<tr> <td colspan="2" align=right>
		  <input type="button" value="  Cancel  " name="cancel" onClick='cancelClick();'>
		  <input type="button" value="<<Back  " name="back" onClick='return saveClick_wan(0)' >
		  <input type="button" value="  Next>>" name="next"  onClick='return saveClick_wan(1)'>
  	</td> </tr>
    </table>
</div>
<script>
			wanTypeSelection(form.wanType);
</script>   
</span>
<!-- wlan1 page -->
<script>
  	
	<% 
	   var i ;
	   for (i=0; i<getIndex("wlan_num"); i=i+1) 
	     { 
			wlan_name= "wlan"+i+"-status";
			getInfo(wlan_name);
			write("wlanDisabled["+i+"] ="+getIndex("wlanDisabled")+";\n"); 
			write("RFType["+i+"] ="+getIndex("RFType")+";\n"); 
			write("APMode["+i+"] ="+getIndex("wlanMode")+";\n"); 
			val= getIndex("band");
			if (val > 0) val=val-1;
			write("bandIdx["+i+"] ="+val+";\n"); 
			write("networkType["+i+"] ="+getIndex("networkType")+";\n"); 
	    		write ("regDomain["+i+"]=" +getIndex("regDomain")+ ";\n");
  	    		write ("defaultChan["+i+"]=" +getIndex("channel")+ ";\n");
  	    		write ("autoconf["+i+"]=" +getIndex("autoCfgEnabled")+ ";\n");
  	    		write ("keyinstall["+i+"]=" +getIndex("autoCfgKeyInstall")+ ";\n");
  	    		write ("usedBand["+i+"]=" +getIndex("band")+ ";\n");
  	    		write ("ssid["+i+"]=\""); getInfo("ssid");write("\";\n");
  	    		write ("encrypt["+i+"]=" +getIndex("encrypt")+ ";\n");
  	    		write ("wep["+i+"]=" +getIndex("wep")+ ";\n");
  	    		write ("defaultKeyId["+i+"]=" +getIndex("defaultKeyId")+ ";\n");
  	    		write ("defPskFormat["+i+"]=" +getIndex("pskFormat")+ ";\n");
  	    		write ("macClone["+i+"]=" +getIndex("wlanMacClone")+ ";\n");
  	    		write ("pskValue["+i+"]=\""); getInfo("pskValue");write("\";\n");
  	    		write ("keyType["+i+"]=" +getIndex("keyType")+ ";\n");
  	    		write ("autoCfgWlanMode["+i+"]=" +getIndex("autoCfgWlanMode")+ ";\n");
	     } 
	%>
menu_num =5;
for(i=0; i< wlan_num; i++){
/**********************************************************/
document.write('\
<span id = "wlan1_div'+i+'" class = "off" > \
<table  border=0 width="500" cellspacing=4  cellpadding=0> \
  <tr><td  colspan="2"> <h2><font color="#0000FF">'+menu_num+'. Wireless ');
if(wlan_num !=1)
	document.write((i+1));
document.write(' Basic Settings</font></h2> </td></tr> \
  <tr><td  colspan="2"> <font size=2> \
 This page is used to configure the parameters for wireless LAN clients which \
 may connect to your Access Point.  \
  </td> \
  </tr> \
  <tr><td  colspan="2"><hr size=1 noshade align=top></td></tr> \
    </table> \
<span id = "wlan_enable_div'+i+'" class = "off" > \
<table  border=0 width="500" cellspacing=0  cellpadding=0> \
  <tr> \
      <td width="100%" colspan=2><font size=2><b> \
   	<input type="checkbox" name="wlanDisabled'+i+'" value="ON" \
	ONCLICK="updateIputState(document.wizard, '+i+')">&nbsp;&nbsp;Disable Wireless LAN Interface</b> \
      </td> \
  </tr> \
</table> \
</span> \
<table  border=0 width="500" cellspacing=4  cellpadding=0> \
  <tr> \
      <td width="26%"><font size=2><b>Band:</b></td> \
      <td width="74%"><font size=2><select size="1" name="band'+i+'" onChange="updateBand(document.wizard, '+i+')">');
/**********************************************************/
        if(wlanDisabled[i])
		document.wizard.elements["wlanDisabled"+i].checked = true;
 	
	if (APMode[i]!=1)
		bandIdxAP[i]=bandIdx[i];
	else
		bandIdxClient[i]=bandIdx[i];
	showBand(document.wizard, i);
/**********************************************************/
document.write(' \
	 </select> \
      </td> \
  </tr> ');
/**********************************************************/
/*
<!--
  </table>
  <span id = "hide_div" class = "off" >
  <table border="0" width=500>
-->
*/
/**********************************************************/
document.write(' \
  <tr> \
      <td width="26%"><font size=2><b>Mode:</b></td>\
      <td width="74%"><font size=2><select size="1" name="mode'+i+'"	onChange="updateMode(document.wizard, '+i+')">');
	 val = APMode[i] ;
   	 if ( val == 0 ) {
      	  	document.write( "<option selected value=\"0\">AP</option>" );
   	  	document.write( "<option value=\"1\">Client</option>" );
   	  	document.write( "<option value=\"2\">WDS</option>" );
   	  	document.write( "<option value=\"3\">AP+WDS</option>" );
      	 }
	 if ( val == 1 ) {
     	  	document.write( "<option value=\"0\">AP</option>" );
   	  	document.write( "<option selected value=\"1\">Client </option>" );
 	  	document.write( "<option value=\"2\">WDS</option>" );
   	  	document.write( "<option value=\"3\">AP+WDS</option>" );   	  	
   	  }	
	  if ( val == 2 ) {
     	  	document.write( "<option value=\"0\">AP</option>" );
   	  	document.write( "<option value=\"1\">Client </option>" );
 	  	document.write( "<option selected value=\"2\">WDS</option>" );
   	  	document.write( "<option value=\"3\">AP+WDS</option>" );   	  	
   	  }	   	  	
   	  if ( val == 3 ) {
     	  	document.write( "<option value=\"0\">AP</option>" );
   	  	document.write( "<option value=\"1\">Client </option>" );
 	  	document.write( "<option value=\"2\">WDS</option>" );
   	  	document.write( "<option selected value=\"3\">AP+WDS</option>" );     	  	
      	  }
/**********************************************************/

document.write(' \
	 </select> \
      </td> \
  </tr> \
  <tr> \
      <td width="26%"><font size=2><b>Network Type:</b></td>\
      <td width="74%"><font size=2><select size="1" name="type'+i+'"	onChange="updateType(document.wizard, '+i+')">');
	 val = networkType[i];
   	  if ( val == 0 ) {
      	  	document.write( "<option selected value=\"0\">Infrastructure </option>" );
   	  	document.write( "<option value=\"1\">Ad hoc</option>" );
      	  }

	  if ( val == 1 ) {
     	  	document.write( "<option value=\"0\">Infrastructure </option>" );
   	  	document.write( "<option selected value=\"1\">Ad hoc</option>" );
      	  } 
document.write('\
	 </select>\
      </td>\
  </tr>'); //end of document write
	form.elements["type"+i].defaultSelected = form.elements["type"+i].selectedIndex ;
	form.elements["mode"+i].defaultSelected = form.elements["mode"+i].selectedIndex ;
/**********************************************************/
/*
<!--
  </table>
  </span>
  <table border="0" width=500>
-->*/
/**********************************************************/
document.write('\
  <tr>\
      <td width="26%"><font size=2><b>SSID:');
/**********************************************************/
    if (wlanDisabled[i] == 0) {
	if (autoconf[i]) {
		if (keyinstall[i]) {
			if(autoCfgWlanMode[i] == APMode[i]){
			document.write("(EasyConfig)");
			disableSSID[i]=1;
		}
	}
	}
    } 
/**********************************************************/
document.write('\
      </b></td>\
      <td width="74%"><font size=2><input type="text" name="ssid'+i+'" size="33" maxlength="32" value='+ssid[i]+'>\
      </td>\
  </tr>\
  <tr>\
      <td width="26%"><font size=2><b>Channel Number:</b></td>\
      <td width="74%"><font size=2><select size="1" name="chan'+i+'"> </select></td>');
/**********************************************************/

	updateChan(document.wizard, i);

/**********************************************************/
document.write('\
  </tr>\
  <tr>\
      <td width="100%" colspan=2><font size=2><b>\
   	<input type="checkbox" name="wlanMacClone'+i+'" value="ON"\
   	>&nbsp;&nbsp; Enable Mac Clone (Single Ethernet Client)</b>\
      </td>\
  </tr>\
  </table>\
  <br>\
<div style="LEFT: 60px; WIDTH: 300px; POSITION: absolute; TOP: 290px; HEIGHT: 100px">  \
  <table border=0 width="500" > \
     <input type="hidden" value="/wlbasic.asp" name="wlan-url">\
	<tr> <td colspan="2" align=right>\
		  <input type="button" value="  Cancel  " name="cancel" onClick="cancelClick();">\
		  <input type="button" value="<<Back  " name="back" onClick="return saveClick_wlan1(0,'+i+')" >\
		  <input type="button" value="  Next>>" name="next"  onClick="return saveClick_wlan1(1,'+i+')">\
  	</td> </tr>\
	<input type="hidden" value="/wizard-wan.asp" name="back_url">\
	<input type="hidden" value="/wizard-wlan2.asp" name="next_url">\
     <input type="hidden" name="basicrates'+i+'" value=0>\
     <input type="hidden" name="operrates'+i+'" value=0>	\
   </table>\
</div> \
</span>');
/**********************************************************/
		form = document.wizard ;
		updateMode(form, i)
		updateIputState(form, i); 
		if(form.elements["wlanDisabled"+i].checked == false)
			show_div(false, ("wlan_enable_div"+i));
		else
			show_div(true, ("wlan_enable_div"+i));

/**********************************************************/
/*<!-- wlan2 page --> */
/**********************************************************/
document.write('\
<span id = "wlan2_div'+i+'" class = "off" >\
<table  border=0 width="500" cellspacing=4 cellpadding=0>\
    <tr><td  colspan="2"> <h2><font color="#0000FF">'+(menu_num+1)+'. Wireless ');
if(wlan_num !=1)
	document.write((i+1));
document.write('\
    Security Setup</font></h2> </td></tr>\
    <tr><td  colspan="2"><font size=2>\
    This page allows you setup the wireless security. Turn on WEP or WPA by using\
    Encryption Keys could prevent any unauthorized access to your wireless network.\
    </td>\
    </tr>\
  <tr><td  colspan="2"><hr size=1 noshade align=top></td></tr>\
</table>\
\
<table border=0 width="500" cellspacing=4 cellpadding=0>\
   <td width="35%"><font size="2"><b>Encryption:&nbsp;</b>\
        <select size="1" name="method'+i+'" onChange="checkState('+i+')">\
          <option  value="0">None</option>\
          <option value="1">WEP</option>\
          <option value="2">WPA (TKIP)</option>\
	  <option value="4">WPA2(AES)</option>\
	  <option value="6">WPA2 Mixed</option>\
        </select></font></td>\
</table>\
<span id = "wep_div'+i+'" class = "off" >  \
<table border=0 width="500" cellspacing=4  cellpadding=0>\
\
  <tr>\
      <td width="15%"><font size=2><b>Key Length:</b></td>\
      <td width="40%"><font size=2><select size="1" name="length'+i+'" ONCHANGE="lengthClick(document.wizard, '+i+')">\
      			<option value=1 >64-bit</option>\
			<option value=2 >128-bit</option>\
      </select></td>\
  </tr>\
\
  <tr>\
      <td width="15%"><font size=2><b>Key Format:</b></td>\
      <td width="40%"><font size=2><select size="1" name="format'+i+'" ONCHANGE="setDefaultKeyValue(document.wizard, '+i+')">\
     	<option value=1>ASCII</option>\
	<option value=2>Hex</option>\
       </select></td>\
  </tr>\
  <tr>\
      <td width="15%"><font size=2><b>Default Tx Key:</b></td>\
      <td width="40%"><select size="1" name="defaultTxKeyId'+i+'">\
      <option  value="1">Key 1</option>\
      <option  value="2">Key 2</option>\
      <option  value="3">Key 3</option>\
      <option  value="4">Key 4</option>\
      </select></td>\
  </tr>\
  <tr>\
     <td width="15%"><font size=2><b>Encryption Key 1:</b></td>\
     <td width="40%"><font size=2>\
     	<input type="text" name="key1'+i+'" size="26" maxlength="26">\
     </td>\
  </tr>\
  <tr>\
     <td width="15%"><font size=2><b>Encryption Key 2:</b></td>\
     <td width="40%"><font size=2>\
     	<input type="text" name="key2'+i+'" size="26" maxlength="26">\
     </td>\
  </tr>\
  <tr>\
     <td width="15%"><font size=2><b>Encryption Key 3:</b></td>\
     <td width="40%"><font size=2>\
     	<input type="text" name="key3'+i+'" size="26" maxlength="26">\
     </td>\
  </tr>\
  <tr>\
     <td width="15%"><font size=2><b>Encryption Key 4:</b></td>\
     <td width="40%"><font size=2>\
     	<input type="text" name="key4'+i+'" size="26" maxlength="26">\
     </td>\
  </tr>\
</table>\
</span>');
/**********************************************************/

document.write('\
<span id = "wpa_div'+i+'" class = "on" >  \
   <table border=0 width="500" cellspacing=4 cellpadding=0>');
/**********************************************************/
/*
<!--  
    <tr>
      <td width="35%"><font size="2"><b>WPA Unicast Cipher Suite:</b></font> </td>
      <td width="65%"><font size="2">
	  <input type="radio" name="ciphersuite" value="tkip" <% if ( getIndex("wpaCipher") != 2) write("checked"); %>>TKIP&nbsp;
	  <input type="radio" name="ciphersuite" value="aes"  <% if ( getIndex("wpaCipher") == 2) write("checked"); %>>AES
       </font></td>
    </tr>

-->  */
/**********************************************************/
document.write('\
    <tr>\
      <td width="35%"><font size="2"><b>Pre-Shared Key Format:</b></font> </td>\
      <td width="65%"><font size="2"><select size="1" name="pskFormat'+i+'">\
          <option value="0" >Passphrase</option>\
          <option value="1" >Hex (64 characters)</option>\
        </select></font></td>\
    </tr>\
    <tr>\
      <td width="35%"><font size="2"><b>Pre-Shared Key:</b></font> </td>\
      <td width="65%"><font size="2"><input type="text" name="pskValue'+i+'" size="32" maxlength="64" value='+pskValue[i]+'></font></td>\
    </tr>\
     <tr></tr><tr></tr><tr></tr>\
  </table>\
</span>\
<div id="wep_btn'+i+'" style="LEFT: 60px; WIDTH: 300px; POSITION: absolute; TOP: 260px; HEIGHT: 100px">  \
	<table border=0 width="500" >  \
	<tr> <td colspan="2" align=right>\
		  <input type="button" value="  Cancel  " name="cancel" onClick="cancelClick();">\
		  <input type="button" value="<<Back  " name="back" onClick="return saveClick_wlan2(0,'+i+')" > ');
if(i == (wlan_num-1))
	document.write('<input type="submit" value="Finished" name="next" ');
else
	document.write('<input type="button" value="  Next>>" name="next" ');
document.write(' onClick="return saveClick_wlan2(1,'+i+')">\
  	</td> </tr>\
	<input type="hidden" value="" name="ciphersuite'+i+'">\
</table> \
</div>\
</span>');
/**********************************************************/
	form = document.wizard ;
	// set  ecrypt
	if(encrypt[i]==0)
		form.elements["method"+i].selectedIndex = 0;
	else if(encrypt[i]==1)
		form.elements["method"+i].selectedIndex = 1;
	else if(encrypt[i]==2)
		form.elements["method"+i].selectedIndex = 2;
	else if(encrypt[i]==4)
		form.elements["method"+i].selectedIndex = 3;
	else if(encrypt[i]==6)
		form.elements["method"+i].selectedIndex = 4;
	//set wep   key length
	if(wep[i] ==2 )
		form.elements["length"+i].selectedIndex = 1;
	else
		form.elements["length"+i].selectedIndex = 0;
	//set wep default key	
	form.elements["defaultTxKeyId"+i].selectedIndex = defaultKeyId[i]-1;
	//set pskFormat
	if(defPskFormat[i]==0)
		form.elements["pskFormat"+i].selectedIndex= 0;
	else
		form.elements["pskFormat"+i].selectedIndex= 1;

	if(macClone[i])
		form.elements["wlanMacClone"+i].checked = true;

	updateFormat(form, i);

	checkState(i);
	defPskLen[i] = form.elements["pskValue"+i].value.length;
	//set default div
	show_div(true , "top_div");
	document.write('<input type="hidden" value="ON" name="wepEnabled'+i+'">\
	<input type="hidden" value="OFF" name="use1x'+i+'">\
	<input type="hidden" value="psk" name="wpaAuth'+i+'">\
	<input type="hidden" value="1812" name="radiusPort'+i+'">\
	<input type="hidden" value="" name="radiusIP'+i+'">\
	<input type="hidden" value="" name="radiusPass'+i+'">');

	menu_num +=2;
} // end of for loop

</script> 
</blockquote>
	<!-- lan page -->
	<input type="hidden" value="0.0.0.0" name="lan_gateway">
	<input type="hidden" value="2" name="dhcp">
	<input type="hidden" value="0.0.0.0" name="dhcpRangeStart">
	<input type="hidden" value="0.0.0.0" name="dhcpRangeEnd">
	<input type="hidden" value="0" name="stp">
	<input type="hidden" value="000000000000" name="lan_macAddr">
	<!-- wan page -->
	<input type="hidden"   value="0" name="pppConnectType">
	<input type="hidden" value="5" name="pppIdleTime">
	<input type="hidden" value="0.0.0.0" name="dsn2">
	<input type="hidden" value="0.0.0.0" name="dsn3">
	<input type="hidden" value="000000000000" name="wan_macAddr">
<!--	<input type="hidden" value="0" name="upnpEnabled">	-->
	<!-- wlan1 page -->
	<input type="hidden" value="" name="preAuth">
	<!-- wlan2 page -->
</form>
</body>

</html>
