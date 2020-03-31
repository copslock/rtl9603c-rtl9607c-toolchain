<html>
<! Copyright (c) Realtek Semiconductor Corp., 2003. All Rights Reserved. ->
<head>
<meta http-equiv="Content-Type" content="text/html">
<title>VPN Setup </title>
<style>
.on {display:on}
.off {display:none}
</style>
<% language=javascript %>
<script type="text/javascript" src="common.js"> </script>
<SCRIPT>
var ike_setup=0, ikeConnectStatus=0;

function setikeConnected()
{
   ikeConnectStatus = 1;
}

function resetClicked()
{
   document.vpn.reset;
}
function checkIpSubnetAddr(field, msg)
{
  if (field.value=="") {
	alert("IP address cannot be empty! It should be filled with 4 digit numbers as xxx.xxx.xxx.xxx.");
	field.value = field.defaultValue;
	field.focus();
	return false;
  }
   if ( validateKey(field.value) == 0) {
      alert(msg + ' value. It should be the decimal number (0-9).');
      field.value = field.defaultValue;
      field.focus();
      return false;
   }
   if ( !checkDigitRange(field.value,1,0,255) ) {
      alert(msg+' range in 1st digit. It should be 0-255.');
      field.value = field.defaultValue;
      field.focus();
      return false;
   }
   if ( !checkDigitRange(field.value,2,0,255) ) {
      alert(msg + ' range in 2nd digit. It should be 0-255.');
      field.value = field.defaultValue;
      field.focus();
      return false;
   }
   if ( !checkDigitRange(field.value,3,0,255) ) {
      alert(msg + ' range in 3rd digit. It should be 0-255.');
      field.value = field.defaultValue;
      field.focus();
      return false;
   }
   if ( !checkDigitRange(field.value,4,0,255) ) {
      alert(msg + ' range in 4th digit. It should be 0-255.');
      field.value = field.defaultValue;
      field.focus();
      return false;
   }
   return true;
}

function checkVpnSubnet(ip, mask)
{
  
  ip_d = getDigit(ip.value, 1);
  mask_d = getDigit(mask.value, 1);
  ip_d = ip_d & mask_d ;
  strIp = ip_d + '.' ;

  ip_d = getDigit(ip.value, 2);
  mask_d = getDigit(mask.value, 2);
  ip_d = ip_d & mask_d ;
  strIp += ip_d + '.' ;
  

  ip_d = getDigit(ip.value, 3);
  mask_d = getDigit(mask.value, 3);
  ip_d = ip_d & mask_d ;
  strIp += ip_d + '.' ;
  

  ip_d = getDigit(ip.value, 4);
  mask_d = getDigit(mask.value, 4);
  ip_d = ip_d & mask_d ;
  strIp += ip_d ;
  ip.value = strIp ;  
 
  return true ;
}
function checkSpace(field, msg)
{
  str = field.value ;
  
  for (i=0; i<str.length; i++) {
  	if ( str.charAt(i) == ' ' ) {
		alert(msg + '. Can not have space in string');
      		field.value = field.defaultValue;
      		field.focus();
      		return false ;	
	}
  }  
  return true ;
}

function validateHex(str)
{
   for (var i=0; i<str.length; i++) {
    if ( (str.charAt(i) >= '0' && str.charAt(i) <= '9') ||
    	(str.charAt(i) >= 'a' && str.charAt(i) <= 'f') || 
    	(str.charAt(i) >= 'A' && str.charAt(i) <= 'F')
    		 )
			continue;
	return 0;
  }
  return 1;
}

function vpnAdvancedClick(url) {
	openWindow(url, 'VPNIkeAdvanced', 600, 570 );
	ike_setup =1 ;
}

function deleteClick()
{
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

function keyModeClick()
{
	//document.vpn.submit();
  field = document.vpn.ipsecKeyMode ;
  if(!document.getElementById){
  	alert('Error! Your browser must have CSS support !');
  	return;
  }
  if(document.vpn.ipsecKeyMode.checked){	//Ike mode
  	show_div(true,"ikeMode_div");
  	show_div(true,"ikeAdvanced_div");
  	show_div(true,"conType_div");
  	show_div(false,"manualMode_div");
	vpnConnTypeChange(document.vpn.vpnConnectType);
  }
  else { // Manual Mode 
  	show_div(false,"ikeMode_div");
  	show_div(false,"ikeAdvanced_div");
  	show_div(false,"conType_div");  	
  	show_div(true,"manualMode_div");  
  }
}

function remoteTypeClick(field)
{
	if(field.selectedIndex == 0){ // single address
		disableTextField(document.vpn.ipsecRemoteIpMask);
		enableTextField(document.vpn.ipsecRemoteGateway);
		enableTextField(document.vpn.ipsecRemoteIp);
	}	
	else if (field.selectedIndex == 1){ // subnet address
		enableTextField(document.vpn.ipsecRemoteGateway);
		enableTextField(document.vpn.ipsecRemoteIpMask);
		enableTextField(document.vpn.ipsecRemoteIp);

	}
	else if (field.selectedIndex == 2){ // any address
		disableTextField(document.vpn.ipsecRemoteGateway);
		disableTextField(document.vpn.ipsecRemoteIpMask);
		disableTextField(document.vpn.ipsecRemoteIp);
	}
	
	else{	// NAT-T address	
		disableTextField(document.vpn.ipsecRemoteGateway);
		enableTextField(document.vpn.ipsecRemoteIpMask);
		enableTextField(document.vpn.ipsecRemoteIp);
	}
 	vpnConnTypeChange(document.vpn.vpnConnectType);
}

function localTypeClick(field)
{

	if(field.selectedIndex == 0){ // single address
		disableTextField(document.vpn.ipsecLocalIpMask);
	}	
	else if (field.selectedIndex == 2){ // any address
		disableTextField(document.vpn.ipsecLocalIpMask);
	}
	else{		
		enableTextField(document.vpn.ipsecLocalIpMask);
	}	
}

function vpnConnTypeChange(field)
{
	enableTextField(document.vpn.vpnConnectType);
	if(field.selectedIndex == 0 || !document.vpn.tunnelEnabled.checked ||
		document.vpn.ipsecRemoteType.selectedIndex >=2){ //initiator
		if(document.vpn.ipsecRemoteType.selectedIndex >=2){
			disableTextField(document.vpn.vpnConnectType);
			field.selectedIndex  = 1;
		}

			disableButton(document.vpn.vpnConnect);
			disableButton(document.vpn.vpnDisconnect);
	}
	else{
  		if (ikeConnectStatus==0) { //disconnected
			enableButton(document.vpn.vpnConnect);
			disableButton(document.vpn.vpnDisconnect);
		} else
		{
			disableButton(document.vpn.vpnConnect);
			enableButton(document.vpn.vpnDisconnect);
		}
	}
}
function updateState()
{
 vpnConnTypeChange(document.vpn.vpnConnectType);
 if(document.vpn.tunnelEnabled.checked)
	enableButton(document.vpn.ikeAdvanced);
 else
	disableButton(document.vpn.ikeAdvanced);

}
function refreshClick()
{
	if(ike_setup == 0)
		 window.location.reload()
	else{ // form IKE advanced to update something
	//	espAhClick();		
		ike_setup =0; // back form IKE advanced setup
	}
}
function checkEncrKey(field, len, msg, algo)
{
	if(validateHex(field.value) == false){
		alert("Invalid " + msg + " ! You should set a " + len+ " hex string for " + algo +".");
		field.value = field.defaultValue;
		field.focus();
		return false;
        }

	// check key length
        var key_len =  field.value.length ;
	if( key_len != len){
		alert("Invalid " + msg + " ! You should set a " + len+ " hex string for " + algo +".");
		field.value = field.defaultValue;
		field.focus();
		return false;       
       }
	return true ;
}
function saveChanges()
{
   if (document.vpn.ipsecConnName.value=="") {
          alert('Connection Name cannot be empty!');
	  document.vpn.ipsecConnName.value = document.vpn.ipsecConnName.defaultValue;
	  document.vpn.ipsecConnName.focus();
	  return false;
   }
   if(checkSpace(document.vpn.ipsecConnName,'Invalid Connection Name') == false)
   	return false;
   if ( document.vpn.ipsecConnName.value.charAt(0) >= '0' && document.vpn.ipsecConnName.value.charAt(0) <= '9'){
   	alert(" First character of Connection Name can't be 0-9 number");
	document.vpn.ipsecConnName.value = document.vpn.ipsecConnName.defaultValue;
	document.vpn.ipsecConnName.focus();
	return false;
   }
   if ( checkIpSubnetAddr(document.vpn.ipsecLocalIp, 'Invalid Local IP Address') == false )
	     return false;
	     
   if (document.vpn.ipsecAuthType.selectedIndex == 1 && 
   		document.vpn.ipsecRemoteIdType.selectedIndex == 0) {
          alert('Cannot choose IP as Remote ID type in RSA authentication!');
          document.vpn.ipsecAuthType.focus();
          return false;
   }	      
	     
   if (document.vpn.ipsecLocalType.selectedIndex == 1 ){ //subnet address , then check mask
   	if (checkIPMask(document.vpn.ipsecLocalIpMask) == false)
	        return false ;
	
	if (document.vpn.ipsecLocalIpMask.value == "255.255.255.255"){
				document.vpn.ipsecLocalType.selectedIndex = 0 ;
				localTypeClick(document.vpn.ipsecLocalType);
	}
   }   
   if( document.vpn.ipsecRemoteType.selectedIndex != 2) {  // not any address
	   if ( checkIpSubnetAddr(document.vpn.ipsecRemoteIp, 'Invalid Remote IP Address') == false )
		     return false;
	   if (document.vpn.ipsecRemoteType.selectedIndex == 1 ||
		document.vpn.ipsecRemoteType.selectedIndex == 3  ){ //subnet address , then check mask
   		if (checkIPMask(document.vpn.ipsecRemoteIpMask) == false)
	        	return false ;
		if (document.vpn.ipsecRemoteIpMask.value == "255.255.255.255" && document.vpn.ipsecRemoteType.selectedIndex != 3){
				document.vpn.ipsecRemoteType.selectedIndex = 0 ;
				remoteTypeClick(document.vpn.ipsecRemoteType);
		}
	   }
	   if(document.vpn.ipsecRemoteType.selectedIndex != 3){ //natt
	    if ( validateKey( document.vpn.ipsecRemoteGateway.value ) == 0 ) {
		alert("Invalid Remote gateway value. It should be the decimal number (0-9).");
		document.vpn.ipsecRemoteGateway.value = document.vpn.ipsecRemoteGateway.defaultValue;      
		document.vpn.ipsecRemoteGateway.focus();
		
		return false;
	    }	     

	   if ( checkIpAddr(document.vpn.ipsecRemoteGateway, 'Invalid Remote gateway Address') == false )
	      return false;	
	  } // natt
   } 
   // if this is a subnet address , corrent the IP to subnet address according subnet mask  
   if (document.vpn.ipsecLocalType.selectedIndex == 1)
      		checkVpnSubnet(document.vpn.ipsecLocalIp,document.vpn.ipsecLocalIpMask)


   // if this is a subnet address , corrent the IP to subnet address according subnet mask  
   if (document.vpn.ipsecRemoteType.selectedIndex == 1 || document.vpn.ipsecRemoteType.selectedIndex == 3)
      		checkVpnSubnet(document.vpn.ipsecRemoteIp,document.vpn.ipsecRemoteIpMask)
   if (document.vpn.ipsecLocalIdType.selectedIndex!=0 && document.vpn.ipsecLocalId.value=="") {
          alert('Local Id cannot be empty!');
          document.vpn.ipsecLocalId.value = document.vpn.ipsecLocalId.defaultValue;
          document.vpn.ipsecLocalId.focus();
          return false;
   }	
   if (document.vpn.ipsecLocalIdType.selectedIndex == 2 && document.vpn.ipsecLocalId.value.indexOf("@") ==-1){
	  alert('error email format!');
          document.vpn.ipsecLocalId.value = document.vpn.ipsecLocalId.defaultValue;
          document.vpn.ipsecLocalId.focus();
          return false;
   }
   if (document.vpn.ipsecRemoteIdType.selectedIndex!=0 && document.vpn.ipsecRemoteId.value=="") {
          alert('Remote Id cannot be empty!');
          document.vpn.ipsecRemoteId.value = document.vpn.ipsecRemoteId.defaultValue;
          document.vpn.ipsecRemoteId.focus();
          return false;
   }	
   if (document.vpn.ipsecRemoteIdType.selectedIndex == 2 && document.vpn.ipsecRemoteId.value.indexOf("@") ==-1){
	  alert('error email format!');
          document.vpn.ipsecRemoteId.value = document.vpn.ipsecRemoteId.defaultValue;
          document.vpn.ipsecRemoteId.focus();
          return false;

   }
   if ( document.vpn.ipsecKeyMode.checked ){	// IKE mode
  
  	   if(document.vpn.ipsecAuthType.selectedIndex == 0){ //PSK
	   if (document.vpn.ikePsKey.value=="") {
		  alert('PreShared Key cannot be empty!');
		  document.vpn.ikePsKey.value = document.vpn.ikePsKey.defaultValue;
		  document.vpn.ikePsKey.focus();
		  return false;
	   }
	   }
	   else{
		  if (document.vpn.rtRsaKey.value =="" || document.vpn.rtRsaKey.value.length > 400) {
			  document.vpn.rtRsaKey.value = document.vpn.rtRsaKey.defaultValue;
			  document.vpn.rtRsaKey.focus();
			  alert('RSA Key cannot be empty or exceed 400!');
			  return false;
		   }
	   }
	   //check the case,  when remote site is any address, connType must be responder
   	   if (document.vpn.ipsecRemoteType.selectedIndex == 2){ // Any address
	   	if(document.vpn.vpnConnectType.selectedIndex == 0){ //initiator
			alert('When remote site is any addres,  local site must be IKE responder');
			document.vpn.vpnConnectType.focus();
			return false;
		}
	   }
	 
   }  //end IKE
   else{ //Manual Mode
	//Check Remote Site Any address cannot bind with Manual mode
	if(document.vpn.ipsecRemoteType.selectedIndex == 2){
		alert("Can't set any address for remote site with manual key");
		document.vpn.ipsecRemoteType.focus();
		return false;
	}
	//Check ESP Encr -NULL cannot with Manual Mode
	if(document.vpn.ipsecEspEncr.selectedIndex == 2){
		alert("Can't set NULL for with manual key");
		document.vpn.ipsecEspEncr.focus();
		return false;
	}

   	if(validateHex(document.vpn.ipsecSpi.value) == false){
   	   			 alert("Invalid  SPI ! You Should set a value between 100-fff ");
				document.vpn.ipsecSpi.value = document.vpn.ipsecSpi.defaultValue;
				document.vpn.ipsecSpi.focus();
				return false;       	   		
   	}
       spi = parseInt(document.vpn.ipsecSpi.value, 16);
        if((spi > 0xfff) || (spi < 0x100) || document.vpn.ipsecSpi.value == ""){
                alert("Invalid  SPI ! You Should set a value between 100-fff ");
				document.vpn.ipsecSpi.value = document.vpn.ipsecSpi.defaultValue;
				document.vpn.ipsecSpi.focus();
				return false;                
        }  
	
	// Check Esp Encryption Key Length
	index = document.vpn.ipsecEspEncr.selectedIndex ;
	if(index ==0){ // 3DES
		if(!checkEncrKey(document.vpn.ipsecEncrKey, 48, "ESP Encription Key","3DES"))
			return false;
	}else if(index ==1){ // AES
		if(!checkEncrKey(document.vpn.ipsecEncrKey, 32 , "ESP Encription Key","AES128"))
			return false;
	}
	index = document.vpn.ipsecEspAuth.selectedIndex ;
	// Check Esp Authenticaiton Key Length
	if(index ==0){ // MD5
	       if(!checkEncrKey(document.vpn.ipsecAuthKey, 32, "ESP Authentication Key" ,"MD5"))
			return false;
	}
	else if(index ==1){ // SHA-1
	       if(!checkEncrKey(document.vpn.ipsecAuthKey, 40, "ESP Authentication Key" ,"SHA1"))
			return false;
	}
   } // end Manual mode
   //return false;
   return true;
}
function remoteIdTypeClick(field)
{
	if(field.selectedIndex == 0)
		disableTextField(document.vpn.ipsecRemoteId);
	else
		enableTextField(document.vpn.ipsecRemoteId);

}

function localIdTypeClick(field)
{
	if(field.selectedIndex == 0)
		disableTextField(document.vpn.ipsecLocalId);
	else
		enableTextField(document.vpn.ipsecLocalId);

}
function authTypeClick(field)
{
	if(field.selectedIndex  == 0){
		disableTextField(document.vpn.rtRsaKey);
		enableTextField(document.vpn.ikePsKey);
	}
	else{
		disableTextField(document.vpn.ikePsKey);
		enableTextField(document.vpn.rtRsaKey);
	}
}
</SCRIPT>
</head>

<blockquote>
<!--
<h2><font color="#0000FF">VPN Settings</font></h2>
-->
<body>
<!--
<table border=0 width="550" cellspacing=0 cellpadding=0>
  <tr><font size=2>
    This page is used to enable/disable VPN tunnel and edit VPN tunnel parameter
  </tr>
  <tr><hr size=1 noshade align=top></tr>
</table>
-->
<form action=/goform/formVpnSetup method=POST name="vpn">

 <input type="hidden" value="ipsec" name="vpnMode">
  <font color="#0000FF" size=4><b>VPN Setup</b><font>
  <hr size=1 align=left width=500>

  <table border="0" width=572>
  	<tr> 
	<td width="566" colspan="2" ><font size=2><b>
   	<input type="checkbox" name="tunnelEnabled" value="ON"
	<% if (getIndex("tunnelEnabled")) write("checked"); %>
	ONCLICK=updateState()>&nbsp;&nbsp;Enable Tunnel &nbsp;<% getInfo("vpnTblIdx"); %> </b><br>
    	</td>
	</tr>
    <tr>
      <td width="168"><font size=2><b>Connection Name:</b></td>
      <td width="394"><font size=2>
      <input type="text" name="ipsecConnName" size="18" maxlength="30" value="<% getInfo("ipsecConnName"); %>"></td>
    </tr>

    <tr>
      <td width="168"><font size=2><b>Auth Type:</b></td>
      <td width="394"><font size=2>
	<select size="1" name="ipsecAuthType"  onChange="authTypeClick(this)">
		<option  value="0"  <% if (getIndex("ipsecAuthType")==0) write("selected"); %>>PSK </option>
		<option  value="1"  <% if (getIndex("ipsecAuthType")==1) write("selected"); %>>RSA </option>
	</select> 
      </td>
    </tr>

    <tr>
      <td width="168"><font size=2><b>Local Site:</b></td>
      	<td width="394"><font size=2>
			<select size="1" name="ipsecLocalType"  onChange="localTypeClick(this)">
				<option  <% if (getIndex("ipsecLocalType")==0) write("selected"); %> value="0">Single Address</option>
				<option <% if (getIndex("ipsecLocalType")==1) write("selected"); %> value="1">Subnet Address</option>
	    	</select>
      </td>
    </tr>     
    
    <tr>
    	<td width="168"><font size=2>&nbsp;&nbsp;&nbsp; Local IP Address/Network</td>
      <td width="394"><font size=2>
      <input type="text" name="ipsecLocalIp" size="18" maxlength="30" value="<% getInfo("ipsecLocalIp"); %>"></td>
    </tr>     

    <tr>
      <td width="168"><font size=2>&nbsp;&nbsp;&nbsp; Local Subnet Mask</td>
      <td width="394"><font size=2>
      <input type="text" name="ipsecLocalIpMask" size="18" maxlength="30" value="<% getInfo("ipsecLocalIpMask"); %>"></td>
    </tr>    
    
    <tr>
      <td width="168"><font size=2><b>Remote Site:</b></td>
      	<td width="394"><font size=2>
			<select size="1" name="ipsecRemoteType"  onChange="remoteTypeClick(this)">
				<option  <% if (getIndex("ipsecRemoteType")==0) write("selected"); %> value="0">Single Address</option>
				<option  <% if (getIndex("ipsecRemoteType")==1) write("selected"); %> value="1">Subnet Address</option>
				<option  <% if (getIndex("ipsecRemoteType")==2) write("selected"); %> value="2">Any Address</option>
				<option  <% if (getIndex("ipsecRemoteType")==3) write("selected"); %> value="3">NAT-T Any Address</option>

	    	</select>   	  		
      </td>
    </tr>     

     <tr>
      <td width="168"><font size=2>&nbsp;&nbsp;&nbsp; Remote Secure Gateway</td>
      <td width="394"><font size=2>
      <input type="text" name="ipsecRemoteGateway" size="18" maxlength="30" value="<% getInfo("ipsecRemoteGateway"); %>"></td>
    </tr> 
    <tr>
      <td width="168"><font size=2>&nbsp;&nbsp;&nbsp; Remote IP Address/Network</td>
      <td width="394"><font size=2>
      <input type="text" name="ipsecRemoteIp" size="18" maxlength="30" value="<% getInfo("ipsecRemoteIp"); %>"></td>
    </tr>     
    <tr>
      <td width="168"><font size=2>&nbsp;&nbsp;&nbsp; Remote Subnet Mask</td>
      <td width="394"><font size=2>
      <input type="text" name="ipsecRemoteIpMask" size="18" maxlength="30" value="<% getInfo("ipsecRemoteIpMask"); %>"></td>
    </tr>
    <tr>
      <td width="168"><font size=2><b>Local/Peer ID:</b></td>
      <td width="394"><font size=2> </td>
    </tr>   
    <tr>
      <td width="168"><font size=2>&nbsp;&nbsp;&nbsp; Local ID Type</td>
      <td width="394"><font size=2>
	<select size="1" name="ipsecLocalIdType"  onChange="localIdTypeClick(this)">
		<option <% if (getIndex("ipsecLocalIdType")==0) write("selected"); %>  value="0">IP</option>
		<option <% if (getIndex("ipsecLocalIdType")==1) write("selected"); %>  value="1">DNS</option>
		<option <% if (getIndex("ipsecLocalIdType")==2) write("selected"); %>  value="2">Email</option>
	</select> 
      </td>
    </tr> 
    <tr>
      <td width="168"><font size=2>&nbsp;&nbsp;&nbsp; Local ID</td>
      <td width="394"><font size=2>
      <input type="text" name="ipsecLocalId" size="18" maxlength="30" value="<% getInfo("ipsecLocalId"); %>"></td>
    </tr> 
     <tr>
      <td width="168"><font size=2>&nbsp;&nbsp;&nbsp; Remote ID Type</td>
      <td width="394"><font size=2>
       <select size="1" name="ipsecRemoteIdType"  onChange="remoteIdTypeClick(this)">
		<option <% if (getIndex("ipsecRemoteIdType")==0) write("selected"); %>  value="0">IP</option>
		<option <% if (getIndex("ipsecRemoteIdType")==1) write("selected"); %>  value="1">DNS</option>
		<option <% if (getIndex("ipsecRemoteIdType")==2) write("selected"); %>  value="2">Email</option>
	</select> 
      </td>
    </tr> 
    <tr>
      <td width="168"><font size=2>&nbsp;&nbsp;&nbsp; Remote ID</td>
      <td width="394"><font size=2>
      <input type="text" name="ipsecRemoteId" size="18" maxlength="30" value="<% getInfo("ipsecRemoteId"); %>"></td>
    </tr>
   <tr>
      <td width="168"><font size=2><b>Key Management:</b></td>
      <td width="394"><font size=2>

	  <input type="checkbox" name="ipsecKeyMode" value="ON"   <% if (getIndex("ipsecKeyMode")==0) write("checked"); %> onClick="return keyModeClick()">IKE &nbsp;&nbsp;
<!--
	  <input type="radio" name="ipsecKeyMode" value="1"   <% if (getIndex("ipsecKeyMode")==1) write("checked"); %> onClick="return keyModeClick()" >Manual &nbsp;&nbsp;
-->
	  <span id = "ikeAdvanced_div" class = "off" >
  	  <input type="button" value="Advanced" name="ikeAdvanced" onClick="vpnAdvancedClick('/vpnadvanced.asp')">
  	  </span>

	</td>
    </tr>
    </table>

	<span id = "conType_div" class = "off" >
	<table border="0" width=572>
     			<tr>
			<td width="168"><font size=2>&nbsp;&nbsp;&nbsp;&nbsp;Connection Type</font></td>
     			<td width="394"><font size=2><select size="1" name="vpnConnectType" onChange="vpnConnTypeChange(this)">
    	   	  	<option  <% if (getIndex("vpnConnectionType")==0) write("selected"); %> value="0">Initiator</option>
    		  	<option  <% if (getIndex("vpnConnectionType")==1) write("selected"); %> value="1">Responder</option>
        		</select>&nbsp;&nbsp;
			<input type="submit" value="Connect" name="vpnConnect" >&nbsp;&nbsp;
			<input type="submit" value="Disconnect" name="vpnDisconnect" >
			</td> </tr>
	</table>		
	</span>
           
	<table border="0" width=572>	
  	<tr>
		<td width="168"><font size=2>&nbsp;&nbsp;&nbsp; ESP </font></td>

       		<td width="394" > <font size=2>
		<select size="1" name="ipsecEspEncr" >
			<option  <% if (getIndex("ipsecEspEncr")==0) write("selected"); %> value="0">3DES</option>
			<option  <% if (getIndex("ipsecEspEncr")==1) write("selected"); %> value="1">AES128</option>
			<option <% if (getIndex("ipsecEspEncr")==2) write("selected"); %> value="2">NULL</option>
		</select>&nbsp;&nbsp;(Encryption Algorithm)</font></td>
	</tr>
 
	<tr>
		<td width="168"><font size=2></font></td>
       		<td width="394"><font size=2>
		  <select size="1" name="ipsecEspAuth">
			<option  <% if (getIndex("ipsecEspAuth")==0) write("selected"); %> value="0">MD5</option>
			<option  <% if (getIndex("ipsecEspAuth")==1) write("selected"); %> value="1">SHA1</option>
		  </select>&nbsp;&nbsp;(Authentication Algorithm) </font>
		</td>
	</tr>
       	
    	<tr>
	
   	</table>
	
	<span id = "ikeMode_div" class = "off" >
 	<table border="0" width=572>	    	
    		<tr>
    		<td width="168"><font size=2>&nbsp;&nbsp;&nbsp; PreShared Key</td>
    		<td width="394"><font size=2><input type="text" name="ikePsKey" size="18" maxlength="30" value="<% getInfo("ikePsKey"); %>"></td>
    		</tr>
		<tr>
    		<td width="168"><font size=2>&nbsp;&nbsp;&nbsp; Remote RSA Key</td>
    		<td width="394"><font size=2><input type="text" name="rtRsaKey" size="18" maxlength="400" value="<% getInfo("rtRsaKey"); %>"></td>
		</tr>
		<tr>
    		<td width="168"><font size=2 >&nbsp;&nbsp;&nbsp; Status</td>
    		<td width="394"><font color="red" size=2>
		<% if (getIndex("ikeConnectStatus")==1) write("<script>setikeConnected();</script>"); %>
		<script>
			if(ikeConnectStatus ==0)
				document.write("Disconnected");
			else
				document.write("Connected");
		</script>
		</td>
    		</tr>
    	</table>	
	</span>
	
	<span id = "manualMode_div" class = "off" >  	
    	<table border="0" width=572>
    		<tr>
    		<td width="168"><font size=2>&nbsp;&nbsp;&nbsp;&nbsp; SPI </td>
    		<td width="394"><font size=2><input type="text" name="ipsecSpi" size="8" maxlength="15" value="<% getInfo("ipsecSpi"); %>"> (100-fff)</td>
    		</tr> 

		<tr><td width="168"><font size=2>&nbsp;&nbsp;&nbsp; Encryption Key</td>
		<td width="394"><font size=2>
	   	<input type="text" name="ipsecEncrKey" size="35" maxlength="48" value="<% getInfo("ipsecEncrKey"); %>"> </td>
		</tr>
    		<tr>
		<td width="168"><font size=2>&nbsp;&nbsp;&nbsp; Authentication Key</td>
		<td width="394"><font size=2><input type="text" name="ipsecAuthKey" size="35" maxlength="40" value="<% getInfo("ipsecAuthKey"); %>"> </td>
    		</tr>
	</table>
	</span>	

	
	<script>
	// change select enable or disable
	keyModeClick();
		//espAhClick();
	localTypeClick(document.vpn.ipsecLocalType);
	remoteTypeClick(document.vpn.ipsecRemoteType);
	localIdTypeClick(document.vpn.ipsecLocalIdType);
	remoteIdTypeClick(document.vpn.ipsecRemoteIdType);
	authTypeClick(document.vpn.ipsecAuthType);
	disableCheckBox(document.vpn.ipsecKeyMode);
	</script>
    		    		
   
  <!-- for IKE advanced setup -->
  <input type="hidden" value="<% getInfo("ikeLifeTime"); %>" name="ikeLifeTime">
  <input type="hidden" value="<% getInfo("ikeEncr"); %>" name="ikeEncr">
  <input type="hidden" value="<% getInfo("ikeAuth"); %>" name="ikeAuth">
  <input type="hidden" value="<% getInfo("ikeKeyGroup"); %>" name="ikeKeyGroup">
  <input type="hidden" value="<% getInfo("ipsecLifeTime"); %>" name="ipsecLifeTime">
  <input type="hidden" value="<% getInfo("ipsecPfs"); %>" name="ipsecPfs">

  <input type="hidden" value="/vpnsetup.asp" name="submit-url">
  <input type="hidden" value="/vpnsetup.asp" name="top-url">
  <input type="hidden" value="<% getInfo("vpnTblIdx"); %>" name="vpnTblIdx">
  <br>
  <input type="submit" value="Apply Changes" name="save" onClick="return saveChanges()">&nbsp;&nbsp;
  <input type="reset" value="Reset" name="reset" onClick="resetClicked()">&nbsp;
  <input type="button" value="Refresh" name="refresh" onClick="refreshClick()">&nbsp;&nbsp; 
  <input type="button" value="Back" name="vpnBack" onClick="window.location='/vpn.asp'">&nbsp;&nbsp;&nbsp;&nbsp;
</form>
</blockquote>
</body>
</html>
