<html>
<! Copyright (c) Realtek Semiconductor Corp., 2003. All Rights Reserved. ->
<head>
<meta http-equiv="Content-Type" content="text/html">
<title>Wireless Security Setup</title>
<script type="text/javascript" src="common.js"> </script>
<style>
.on {display:on}
.off {display:none}
</style>
<script>
var defPskLen=new Array()
var defPskFormat=new Array();
var autoconf=new Array();
var wlan_idx= <% write(getIndex("wlan_idx")); %> ;
autoconf[wlan_idx]= 0;

// for WPS --------------------------------------------->>
var wps_encrypt_old=<% write(getIndex("encrypt"));%>;						
var wps_disabled=<% write(getIndex("wscDisable"));%>;
var wps_wpaCipher_old=<% write(getIndex("wpaCipher"));%>;
var wps_wpa2Cipher_old=<% write(getIndex("wpa2Cipher"));%>;
var wps_psk_old='<% getInfo("pskValue");%>';
var wps_psk_unmask_old='<% getInfo("pskValueUnmask");%>';
var wps_config_by_registrar=<% write(getIndex("wps_by_reg"));%>;
var wps_mode_old=<% write(getIndex("wlanMode"));%>;
var wps_type_old=<% write(getIndex("networkType"));%>;
var wps_wizard=0;
// <<---------------------------------------------- for WPS

function check_radius_state()
{
  form = document.formEncrypt ;
  method = form.elements["method"+wlan_idx];
  use1x = form.elements["use1x"+wlan_idx];
  wpaAuth = form.elements["wpaAuth"+wlan_idx];
  if ((method.selectedIndex<2 && use1x.checked) ||
	(method.selectedIndex>=2 && wpaAuth[0].checked)) {
	enableTextField(form.elements["radiusPort"+wlan_idx]);
  	enableTextField(form.elements["radiusIP"+wlan_idx]);
  	enableTextField(form.elements["radiusPass"+wlan_idx]);
  }
  else {
  	disableTextField(form.elements["radiusPort"+wlan_idx]);
  	disableTextField(form.elements["radiusIP"+wlan_idx]);
  	disableTextField(form.elements["radiusPass"+wlan_idx]);
  }
}

function check_wepbutton_state()
{
  form = document.formEncrypt ;
  method = form.elements["method"+wlan_idx];
  use1x = form.elements["use1x"+wlan_idx];
  if (method.selectedIndex==1 && !use1x.checked)
	enableButton(form.elements["wepKey"+wlan_idx]);
  else
	disableButton(form.elements["wepKey"+wlan_idx]);
}

function check_nonWpaSupp_state()
{
  check_radius_state();
  check_wepbutton_state();
  check_wepKeyLen_state();
}

function check_wepKeyLen_state()
{
  form = document.formEncrypt ;
  method = form.elements["method"+wlan_idx];
  use1x = form.elements["use1x"+wlan_idx];
  if ((method.selectedIndex==1 && use1x.checked) )
  	enableRadioGroup(form.elements["wepKeyLen"+wlan_idx]);
  else
  	disableRadioGroup(form.elements["wepKeyLen"+wlan_idx]);
}

function disable_wpa()
{
  form = document.formEncrypt ;
  enableCheckBox(form.elements["use1x"+wlan_idx]);
  disableRadioGroup(form.elements["wpaAuth"+wlan_idx]);
   // for wpa-aes
  disableRadioGroup(form.elements["ciphersuite"+wlan_idx]); 
  disableRadioGroup(form.elements["wpa2ciphersuite"+wlan_idx]);

  disableTextField(form.elements["pskFormat"+wlan_idx]);
  disableTextField(form.elements["pskValue"+wlan_idx]);

  check_nonWpaSupp_state();
}

function enable_wpa()
{
  
  form = document.formEncrypt ;
  wpaAuth = form.elements["wpaAuth"+wlan_idx]; 
  disableCheckBox(form.elements["use1x"+wlan_idx]);
  enableRadioGroup(wpaAuth);
  
  // for wpa-aes
  method = form.elements["method"+wlan_idx];  
  if (method.selectedIndex>=2 && method.selectedIndex <=4){  	
  	if (method.selectedIndex==2) {
		enableRadioGroup(form.elements["ciphersuite"+wlan_idx]);  
		disableRadioGroup(form.elements["wpa2ciphersuite"+wlan_idx]);		
	}
  	else if (method.selectedIndex==3) {
		disableRadioGroup(form.elements["ciphersuite"+wlan_idx]);  
		enableRadioGroup(form.elements["wpa2ciphersuite"+wlan_idx]);		
	}	
 	else {
		enableRadioGroup(form.elements["ciphersuite"+wlan_idx]);  
		enableRadioGroup(form.elements["wpa2ciphersuite"+wlan_idx]);		
	}		
  }
  else {
  	disableRadioGroup(form.elements["ciphersuite"+wlan_idx]);    
	disableRadioGroup(form.elements["wpa2ciphersuite"+wlan_idx]);  	
  }
  	
  if (wpaAuth[1].checked) {
	enableTextField(form.elements["pskFormat"+wlan_idx]);
	enableTextField(form.elements["pskValue"+wlan_idx]);
  }
  else {
	disableTextField(form.elements["pskFormat"+wlan_idx]);
	disableTextField(form.elements["pskValue"+wlan_idx]);
  }
  check_nonWpaSupp_state();
}

function disable_all()
{
  form = document.formEncrypt ;
  disableTextField(form.elements["method"+wlan_idx]);
  disableCheckBox(form.elements["use1x"+wlan_idx]);
  disableRadioGroup(form.elements["wpaAuth"+wlan_idx]);
//  disableCheckBox(document.formEncrypt.tkip);
//  disableCheckBox(document.formEncrypt.aes);

// for wpa-aes
  disableRadioGroup(form.elements["ciphersuite"+wlan_idx]);
  disableRadioGroup(form.elements["wpa2ciphersuite"+wlan_idx]);
  
  disableTextField(form.elements["pskFormat"+wlan_idx]);
  disableTextField(form.elements["pskValue"+wlan_idx]);
  disableTextField(form.elements["radiusPort"+wlan_idx]);
  disableTextField(form.elements["radiusIP"+wlan_idx]);
  disableTextField(form.elements["radiusPass"+wlan_idx]);
  disableButton(form.elements["wepKey"+wlan_idx]);
  disableRadioGroup(form.elements["wepKeyLen"+wlan_idx]);
  disableCheckBox(form.elements["preAuth"+wlan_idx]);  
  disableButton(document.formEncrypt.save);
  disableButton(document.formEncrypt.reset);  
}

function checkState(wlan_id)
{

  form = document.formEncrypt ;
  if (autoconf[wlan_id]) {
  	disable_all();
  }
  else {
  	if (form.elements["method"+wlan_id].selectedIndex>=2)
  		enable_wpa();
  	else
  		disable_wpa();
  	//wpa2 
  	//if(form.method.selectedIndex==2)
	//	form.ciphersuite.value = "tkip" ;  	
  	//else if(form.method.selectedIndex==3)
	//	form.ciphersuite.value = "aes" ;  	
  	//else if (form.method.selectedIndex==4) // mixed
	//	form.ciphersuite.value = "mixed" ;  	

  	if(form.elements["method"+wlan_id].selectedIndex >= 3 &&
	   wpaAuth[0].checked)
  		enableCheckBox(form.elements["preAuth"+wlan_id]);
  	else
  		disableCheckBox(form.elements["preAuth"+wlan_id]);
  }
}
function wepKeyClick(url)
{
  form = document.formEncrypt ;
  if (form.elements["method"+wlan_idx].selectedIndex==1)
	openWindow(url, 'SetWEPKey', 620, 420 );
}
</script>

</head>

<body>
<blockquote>
<h2><font color="#0000FF">Wireless Security Setup <% if (getIndex("wlan_num") > 1) write("-wlan"+(getIndex("wlan_idx")+1)); %></font></h2>

<form action=/goform/formWlEncrypt method=POST name="formEncrypt">
<!-- for WPS -->
<INPUT type=hidden name=wps_clear_configure_by_reg<% write(getIndex("wlan_idx")); %> value=0>

<table border=0 width="540" cellspacing=4 cellpadding=0>
    <tr><font size=2>
    This page allows you setup the wireless security. Turn on WEP or WPA by using
    Encryption Keys could prevent any unauthorized access to your wireless network.
    </tr>
    <tr><hr size=1 noshade align=top></tr>
    <tr>
      <td width="35%"><font size="2"><b>Encryption:&nbsp;</b>
        <select size="1" name="method<% write(getIndex("wlan_idx")); %>" onChange="checkState(wlan_idx)">
          <option <% choice = getIndex("encrypt"); if (choice == 0) write("selected"); %> value="0">None</option>
          <option <% choice = getIndex("encrypt"); if (choice == 1) write("selected"); %> value="1">WEP</option>
<!-- for wpa-aes          
          <option <% choice = getIndex("encrypt"); if (choice == 2) write("selected"); %> value="2">WPA (TKIP)</option>
          <option <% choice = getIndex("encrypt"); if (choice == 4) write("selected"); %> value="4">WPA2(AES)</option>
-->
          <option <% choice = getIndex("encrypt"); if (choice == 2) write("selected"); %> value="2">WPA</option>          
          <option <% choice = getIndex("encrypt"); if (choice == 4) write("selected"); %> value="4">WPA2</option>
<script>
	val = <% write(getIndex("wlanMode")); %>; 
	if(val == 0 || val == 3) // AP mode
	  document.write('<option <% choice = getIndex("encrypt"); if (choice == 6) write("selected"); %> value="6">WPA2 Mixed</option>');
</script>	  

        </select></font></td>
        </select></font></td>
      <td width="65%"><font size=2><input type="button" value="Set WEP Key" name="wepKey<% write(getIndex("wlan_idx")); %>" onClick="wepKeyClick('wlwep.asp')">
    </tr>
    <tr>
      <td width="35%" onClick="checkState(wlan_idx)"><font size="2"><b><input type="checkbox" name="use1x<% write(getIndex("wlan_idx")); %>" value="ON"
        <% if (getIndex("enable1X")&1) write("checked");%> >Use 802.1x Authentication</b></font></td>
      <td width="65%"><font size="2">
	  <input type="radio" name="wepKeyLen<% write(getIndex("wlan_idx")); %>" value="wep64" <% if ( getIndex("wep") != 2) write("checked"); %>>WEP 64bits&nbsp;
	  <input type="radio" name="wepKeyLen<% write(getIndex("wlan_idx")); %>" value="wep128"<% if ( getIndex("wep") == 2) write("checked"); %>>WEP 128bits
      </font></td>
    </tr>
    <tr>
      <td width="35%"><font size="2"><b>WPA Authentication Mode:</b></font></td>
	<td width="65%"><font size="2">
	  <input type="radio" name="wpaAuth<% write(getIndex("wlan_idx")); %>" value="eap" onClick="checkState(wlan_idx)" <% if (getIndex("wpaAuth") & 1) write("checked"); %>>Enterprise (RADIUS)&nbsp;
	  <input type="radio" name="wpaAuth<% write(getIndex("wlan_idx")); %>" value="psk" onClick="checkState(wlan_idx)" <% if (getIndex("wpaAuth") & 2) write("checked"); %>>Personal (Pre-Shared Key)
	</font></td>
    </tr>

<!-- for wpa-aes -->
    <tr>
      <td width="35%"><font size="2"><b>WPA Cipher Suite:</b></font> </td>
       <td width="65%"><font size="2">
	  <input type="checkbox" name="ciphersuite<% write(getIndex("wlan_idx")); %>" value="tkip" <% if ( getIndex("wpaCipher") & 1) write("checked"); %>>TKIP&nbsp;
	  <input type="checkbox" name="ciphersuite<% write(getIndex("wlan_idx")); %>" value="aes"  <% if ( getIndex("wpaCipher") & 2) write("checked"); %>>AES
       </font></td>
    </tr>
    <tr>
	  <td width="35%"><font size="2"><b>WPA2 Cipher Suite:</b></font> </td>
	  <td width="65%"><font size="2">
	  <input type="checkbox" name="wpa2ciphersuite<% write(getIndex("wlan_idx")); %>" value="tkip" <% if ( getIndex("wpa2Cipher") & 1) write("checked"); %>>TKIP&nbsp;
	  <input type="checkbox" name="wpa2ciphersuite<% write(getIndex("wlan_idx")); %>" value="aes"  <% if ( getIndex("wpa2Cipher") & 2) write("checked"); %>>AES
       </font></td>
    </tr>
<!-- for wpa-aes -->

    <tr>
      <td width="35%"><font size="2"><b>Pre-Shared Key Format:</b></font> </td>
      <td width="65%"><font size="2"><select size="1" name="pskFormat<% write(getIndex("wlan_idx")); %>">
          <option value="0" <% if (getIndex("pskFormat")==0) write("selected");%>>Passphrase</option>
          <option value="1" <% if (getIndex("pskFormat")) write("selected");%>>Hex (64 characters)</option>
        </select></font></td>
    </tr>
    <tr>
      <td width="35%"><font size="2"><b>Pre-Shared Key:</b></font> </td>
      <td width="65%"><font size="2"><input type="text" name="pskValue<% write(getIndex("wlan_idx")); %>" size="32" maxlength="64" value=<% getInfo("pskValue");%>></font></td>
    </tr>
    <tr>
    	<td width="35%"><font size="2"><b><input type="checkbox" name="preAuth<% write(getIndex("wlan_idx")); %>" value="ON"
	<% if (getIndex("preAuth")&1) write("checked");%>>
        Enable Pre-Authentication </b></font></td>
    </tr>
    <tr></tr><tr></tr><tr></tr>
    <tr>
      <td width="35%"><font size="2"><b>Authentication RADIUS Server:</b></font></td>
      <td width="65%"><font size="2">
        Port <input type="text" name="radiusPort<% write(getIndex("wlan_idx")); %>" size="4" value=<% getInfo("rsPort"); %>>&nbsp;&nbsp;
	IP address <input type="text" name="radiusIP<% write(getIndex("wlan_idx")); %>" size="10" value=<% getInfo("rsIp"); %>>&nbsp;&nbsp;
        Password <input type="password" name="radiusPass<% write(getIndex("wlan_idx")); %>" size="8" maxlength="64" value=<% getInfo("rsPassword"); %>></font></td>
    </tr>
    <tr><td colspan="2" width="100%" height="55"><font size=2><em>
    <script>
    	<% autoconf = getIndex("autoCfgEnabled");
      	   keyinstall = getIndex("autoCfgKeyInstall");
      	   mode = getIndex("wlanMode")
 	   if (mode == 2)
    	   	write("autoconf[wlan_idx]=1;\n");      	       		      	   
    	   if (autoconf) {
    	   	if (keyinstall) {
 	    		if (mode == getIndex("autoCfgWlanMode"))
    	   			write("autoconf[wlan_idx]=1;\n");  	   		
    	   	}    	   	
    	   }    
    	%>         
    	if (!autoconf[wlan_idx])        
    		document.write("Note: When encryption WEP is selected, you must set WEP key value.");
    	else
    		document.write("Note: If you want to change the setting, you must go to EasyConfig page to disable EasyConfig first.");
    </script>       		
    </em></font></td>
    </tr>
    <tr>
<!-- for wpa-aes    
      <input type="hidden" value="" name="ciphersuite<% write(getIndex("wlan_idx")); %>">
-->      
      <input type="hidden" value="/wlwpa.asp" name="submit-url">
      <td width="100%" colspan="2"><input type="submit" value="Apply Changes" name="save" onClick="return saveChanges_wpa(document.formEncrypt, wlan_idx)">&nbsp;
        <input type="reset" value="Reset" name="reset"></td>
    </tr>
    <script>  
    	checkState(wlan_idx);
	form = document.formEncrypt ;
        defPskLen[wlan_idx] = form.elements["pskValue"+wlan_idx].value.length;
	defPskFormat[wlan_idx] = form.elements["pskFormat"+wlan_idx].selectedIndex;
    </script>
  </table>
</form>
</blockquote>
</body>

</html>
