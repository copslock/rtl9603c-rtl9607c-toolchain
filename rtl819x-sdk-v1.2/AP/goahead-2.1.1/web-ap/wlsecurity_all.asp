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
var defPskLen=new Array()
var defPskFormat=new Array();
var autoconf=new Array();
var bandIdx=new Array();
var wlan_idx=<% write(getIndex("wlan_idx")); %>;

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

var SSIDnum;
var mode;
var enable_1x;
var wlan_auth;
var wpa_auth;
var wepmode;
var wep_key_fmt;
var wpa_cipher;
var wpa2_cipher;
//var wpa2_preAuth;
var psk_fmt;
var ap_mode;

function show_8021x_settings()
{
	var wlan_encmode = get_by_id("method");
	var enable_1x = get_by_id("use1x");
	var form1 = document.formEncrypt ;
	var dF=document.forms[0];
	if (enable_1x.checked) {		
		if (wlan_encmode.selectedIndex == 1)
			get_by_id("show_1x_wep").style.display = "";
		else 
			get_by_id("show_1x_wep").style.display = "none";
		get_by_id("setting_wep").style.display = "none";		
		get_by_id("show_8021x_eap").style.display = "";
		dF.auth_type[2].checked = true;
		dF.auth_type[0].disabled = true;
		dF.auth_type[1].disabled = true;
		dF.auth_type[2].disabled = true;
	}
	else {	
		if (wlan_encmode.selectedIndex == 1)
			get_by_id("setting_wep").style.display = "";	
		else	
			get_by_id("setting_wep").style.display = "none";	

		get_by_id("show_1x_wep").style.display = "none";
get_by_id("show_8021x_eap").style.display = "none";

		if(ap_mode!=1){//AP
			if (wlan_encmode.selectedIndex == 2 || wlan_encmode.selectedIndex == 3 || wlan_encmode.selectedIndex == 4){
				if(dF.wpaAuth<% write(getIndex("wlan_idx")); %>[1].checked==true)
					get_by_id("show_8021x_eap").style.display = "none";
				else
					get_by_id("show_8021x_eap").style.display = "";
			}
		}else{//Router
			if (wlan_encmode.selectedIndex == 2 || wlan_encmode.selectedIndex == 3 || wlan_encmode.selectedIndex == 4 ){
				if(dF.wpaAuth<% write(getIndex("wlan_idx")); %>[1].checked==true)
					get_by_id("show_8021x_eap").style.display = "none";
				else
					get_by_id("show_8021x_eap").style.display = "";
			}
		}					
		//get_by_id("show_8021x_eap").style.display = "none";
		//dF.auth_type[2].checked = true;
		dF.auth_type[0].disabled = false;
		dF.auth_type[1].disabled = false;
		dF.auth_type[2].disabled = false;
	}		
}

function show_wpa_settings()
{
	var dF=document.forms[0];
	var wep_type = get_by_id("method");
	var allow_tkip=0;
	if(wep_type.selectedIndex==2 || wep_type.selectedIndex==3)
		allow_tkip=0;
	else
		allow_tkip=1;
	get_by_id("show_wpa_psk1").style.display = "none";
	get_by_id("show_wpa_psk2").style.display = "none";	
	get_by_id("show_8021x_eap").style.display = "none";
//	get_by_id("show_pre_auth").style.display = "none";
	
	if (dF.wpaAuth<% write(getIndex("wlan_idx")); %>[1].checked)
	{
		get_by_id("show_wpa_psk1").style.display = "";
		get_by_id("show_wpa_psk2").style.display = "";		
	}
	else{
		if (ap_mode != 1)
		get_by_id("show_8021x_eap").style.display = "";
//		if (wep_type.selectedIndex > 2) 
//			get_by_id("show_pre_auth").style.display = "";		
	}
	
	if("<% getInfo('wlan_onoff_tkip'); %>" * 1 == 0)
		{
			wlanBand = "<% getInfo('wlanband'); %>" *1;
			wlanMode = "<% write(getIndex("wlanMode"));%>"*1;
			if(wlanMode!=1 && (wlanBand == 8 || wlanBand == 10 || wlanBand == 11) && allow_tkip==0)
			{
				dF.ciphersuite<% write(getIndex("wlan_idx")); %>[0].checked=false;
				dF.ciphersuite<% write(getIndex("wlan_idx")); %>[0].disabled=true;
				
				dF.wpa2ciphersuite<% write(getIndex("wlan_idx")); %>[0].checked=false;
				dF.wpa2ciphersuite<% write(getIndex("wlan_idx")); %>[0].disabled=true;
				
			}
			else
			{
				dF.ciphersuite<% write(getIndex("wlan_idx")); %>[0].disabled=false;
				dF.wpa2ciphersuite<% write(getIndex("wlan_idx")); %>[0].disabled=false;
			}
		}
	
}

function show_authentication()
{	
	var wep_type = get_by_id("method");
	var enable_1x = get_by_id("use1x");	
	var form1 = document.formEncrypt ;

	get_by_id("show_wep_auth").style.display = "none";	
	get_by_id("setting_wep").style.display = "none";
	get_by_id("setting_wpa").style.display = "none";
//	get_by_id("show_pre_auth").style.display = "none";
	get_by_id("show_wpa_cipher").style.display = "none";
	get_by_id("show_wpa2_cipher").style.display = "none";
	get_by_id("enable_8021x").style.display = "none";
	get_by_id("show_8021x_eap").style.display = "none";
	get_by_id("show_1x_wep").style.display = "none";
	
	if (wep_type.selectedIndex == 1){
		get_by_id("show_wep_auth").style.display = "";		
		if (ap_mode == 1) 
			get_by_id("setting_wep").style.display = "";		
		else {
			get_by_id("enable_8021x").style.display = "";
			if(enable_1x.checked){		
				get_by_id("show_8021x_eap").style.display = "";
				get_by_id("show_1x_wep").style.display = "";
				get_by_id("setting_wep").style.display = "none";
			}else{		
				get_by_id("setting_wep").style.display = "";
			}
		}
	
	}else if (wep_type.selectedIndex > 1 && wep_type.selectedIndex < 5){
		get_by_id("setting_wpa").style.display = "";
		if (wep_type.selectedIndex == 2)	
			get_by_id("show_wpa_cipher").style.display = "";
		if(wep_type.selectedIndex ==3)
			get_by_id("show_wpa2_cipher").style.display = "";
		if(wep_type.selectedIndex ==4){
			get_by_id("show_wpa_cipher").style.display = "";
			get_by_id("show_wpa2_cipher").style.display = "";
		}		
		show_wpa_settings();
	}
	
	if (wep_type.selectedIndex == 0) {
		if (ap_mode != 1) {
			get_by_id("enable_8021x").style.display = "";
			if(enable_1x.checked){		
				get_by_id("show_8021x_eap").style.display = "";
			}
			else {
				get_by_id("show_8021x_eap").style.display = "none";			
			}
		}
	}	
}

function updateWepFormat(form, wlan_id)
{
	if (form.elements["length" + wlan_id].selectedIndex == 0) {
		form.elements["format" + wlan_id].options[0].text = 'ASCII (5 characters)';
		form.elements["format" + wlan_id].options[1].text = 'Hex (10 characters)';
	}
	else {
		form.elements["format" + wlan_id].options[0].text = 'ASCII (13 characters)';
		form.elements["format" + wlan_id].options[1].text = 'Hex (26 characters)';
	}
	form.elements["format" + wlan_id].selectedIndex =  wep_key_fmt;

	setDefaultKeyValue(form, wlan_id);
}

function Load_Setting()
{
	var dF=document.forms[0];
	idx = parent.document.forms[0].SSID_Setting.selectedIndex;	
	SSIDnum = parent.document.forms[0].SSID_Setting[idx].value;	
	
	if (SSIDnum == 0) {	
	<%	write ("\t\tmode=\"" + getVirtualIndex("encrypt", 0) + "\";\n");
		write ("\t\tenable_1x=\"" + getVirtualIndex("enable1X", 0) + "\";\n");
		write ("\t\twlan_auth=\"" + getVirtualIndex("authType", 0) + "\";\n");
		write ("\t\twpa_auth=\"" + getVirtualIndex("wpaAuth", 0) + "\";\n");
		write ("\t\twepmode=\"" + getVirtualIndex("wep", 0) + "\";\n");
		write ("\t\twep_key_fmt=\"" + getVirtualIndex("keyType", 0) + "\";\n");
		write ("\t\twpa_cipher=\"" + getVirtualIndex("wpaCipher", 0) + "\";\n");
		write ("\t\twpa2_cipher=\"" + getVirtualIndex("wpa2Cipher", 0) + "\";\n");
		write ("\t\tpsk_fmt=\"" + getVirtualIndex("pskFormat", 0) + "\";\n");
	%>	
		dF.pskValue<% write(getIndex("wlan_idx")); %>.value="<%getVirtualInfo("pskValue", 0);%>";	
		dF.radiusIP<% write(getIndex("wlan_idx")); %>.value="<%getVirtualInfo("rsIp", 0);%>";
		dF.radiusPort<% write(getIndex("wlan_idx")); %>.value="<%getVirtualInfo("rsPort", 0);%>";
		dF.radiusPass<% write(getIndex("wlan_idx")); %>.value="<%getVirtualInfo("rsPassword", 0);%>";
		var val=<% write(getVirtualIndex("band", 0)); %>;
		if (val > 0) val = val-1;
			bandIdx[wlan_idx] = val;  
	}

	if (SSIDnum == 1) {	
	<%	write ("\t\tmode=\"" + getVirtualIndex("encrypt", 1) + "\";\n");
		write ("\t\tenable_1x=\"" + getVirtualIndex("enable1X", 1) + "\";\n");
		write ("\t\twlan_auth=\"" + getVirtualIndex("authType", 1) + "\";\n");
		write ("\t\twpa_auth=\"" + getVirtualIndex("wpaAuth", 1) + "\";\n");
		write ("\t\twepmode=\"" + getVirtualIndex("wep", 1) + "\";\n");
		write ("\t\twep_key_fmt=\"" + getVirtualIndex("keyType", 1) + "\";\n");
		write ("\t\twpa_cipher=\"" + getVirtualIndex("wpaCipher", 1) + "\";\n");
		write ("\t\twpa2_cipher=\"" + getVirtualIndex("wpa2Cipher", 1) + "\";\n");
		write ("\t\tpsk_fmt=\"" + getVirtualIndex("pskFormat", 1) + "\";\n");
	%>	
		dF.pskValue<% write(getIndex("wlan_idx")); %>.value="<%getVirtualInfo("pskValue", 1);%>";	
		dF.radiusIP<% write(getIndex("wlan_idx")); %>.value="<%getVirtualInfo("rsIp", 1);%>";
		dF.radiusPort<% write(getIndex("wlan_idx")); %>.value="<%getVirtualInfo("rsPort", 1);%>";
		dF.radiusPass<% write(getIndex("wlan_idx")); %>.value="<%getVirtualInfo("rsPassword", 1);%>";
		var val=<% write(getVirtualIndex("band", 1)); %>;
		if (val > 0) val = val-1;
			bandIdx[wlan_idx] = val;  
	}

	if (SSIDnum == 2) {	
	<%	write ("\t\tmode=\"" + getVirtualIndex("encrypt", 2) + "\";\n");
		write ("\t\tenable_1x=\"" + getVirtualIndex("enable1X", 2) + "\";\n");
		write ("\t\twlan_auth=\"" + getVirtualIndex("authType", 2) + "\";\n");
		write ("\t\twpa_auth=\"" + getVirtualIndex("wpaAuth", 2) + "\";\n");
		write ("\t\twepmode=\"" + getVirtualIndex("wep", 2) + "\";\n");
		write ("\t\twep_key_fmt=\"" + getVirtualIndex("keyType", 2) + "\";\n");
		write ("\t\twpa_cipher=\"" + getVirtualIndex("wpaCipher", 2) + "\";\n");
		write ("\t\twpa2_cipher=\"" + getVirtualIndex("wpa2Cipher", 2) + "\";\n");
		write ("\t\tpsk_fmt=\"" + getVirtualIndex("pskFormat", 2) + "\";\n");
	%>	
		dF.pskValue<% write(getIndex("wlan_idx")); %>.value="<%getVirtualInfo("pskValue", 2);%>";	
		dF.radiusIP<% write(getIndex("wlan_idx")); %>.value="<%getVirtualInfo("rsIp", 2);%>";
		dF.radiusPort<% write(getIndex("wlan_idx")); %>.value="<%getVirtualInfo("rsPort", 2);%>";
		dF.radiusPass<% write(getIndex("wlan_idx")); %>.value="<%getVirtualInfo("rsPassword", 2);%>";
		var val=<% write(getVirtualIndex("band", 2)); %>;
		if (val > 0) val = val-1;
			bandIdx[wlan_idx] = val;  
	}

	if (SSIDnum == 3) {	
	<%	write ("\t\tmode=\"" + getVirtualIndex("encrypt", 3) + "\";\n");
		write ("\t\tenable_1x=\"" + getVirtualIndex("enable1X", 3) + "\";\n");
		write ("\t\twlan_auth=\"" + getVirtualIndex("authType", 3) + "\";\n");
		write ("\t\twpa_auth=\"" + getVirtualIndex("wpaAuth", 3) + "\";\n");
		write ("\t\twepmode=\"" + getVirtualIndex("wep", 3) + "\";\n");
		write ("\t\twep_key_fmt=\"" + getVirtualIndex("keyType", 3) + "\";\n");
		write ("\t\twpa_cipher=\"" + getVirtualIndex("wpaCipher", 3) + "\";\n");
		write ("\t\twpa2_cipher=\"" + getVirtualIndex("wpa2Cipher", 3) + "\";\n");
		write ("\t\tpsk_fmt=\"" + getVirtualIndex("pskFormat", 3) + "\";\n");
	%>	
		dF.pskValue<% write(getIndex("wlan_idx")); %>.value="<%getVirtualInfo("pskValue", 3);%>";	
		dF.radiusIP<% write(getIndex("wlan_idx")); %>.value="<%getVirtualInfo("rsIp", 3);%>";
		dF.radiusPort<% write(getIndex("wlan_idx")); %>.value="<%getVirtualInfo("rsPort", 3);%>";
		dF.radiusPass<% write(getIndex("wlan_idx")); %>.value="<%getVirtualInfo("rsPassword", 3);%>";
		var val=<% write(getVirtualIndex("band", 3)); %>;
		if (val > 0) val = val-1;
			bandIdx[wlan_idx] = val;  
	}

	if (SSIDnum == 4) {	
	<%	write ("\t\tmode=\"" + getVirtualIndex("encrypt", 4) + "\";\n");
		write ("\t\tenable_1x=\"" + getVirtualIndex("enable1X", 4) + "\";\n");
		write ("\t\twlan_auth=\"" + getVirtualIndex("authType", 4) + "\";\n");
		write ("\t\twpa_auth=\"" + getVirtualIndex("wpaAuth", 4) + "\";\n");
		write ("\t\twepmode=\"" + getVirtualIndex("wep", 4) + "\";\n");
		write ("\t\twep_key_fmt=\"" + getVirtualIndex("keyType", 4) + "\";\n");
		write ("\t\twpa_cipher=\"" + getVirtualIndex("wpaCipher", 4) + "\";\n");
		write ("\t\twpa2_cipher=\"" + getVirtualIndex("wpa2Cipher", 4) + "\";\n");
		write ("\t\tpsk_fmt=\"" + getVirtualIndex("pskFormat", 4) + "\";\n");
	%>	
		dF.pskValue<% write(getIndex("wlan_idx")); %>.value="<%getVirtualInfo("pskValue", 4);%>";	
		dF.radiusIP<% write(getIndex("wlan_idx")); %>.value="<%getVirtualInfo("rsIp", 4);%>";
		dF.radiusPort<% write(getIndex("wlan_idx")); %>.value="<%getVirtualInfo("rsPort", 4);%>";
		dF.radiusPass<% write(getIndex("wlan_idx")); %>.value="<%getVirtualInfo("rsPassword", 4);%>";
		var val=<% write(getVirtualIndex("band", 4)); %>;
		if (val > 0) val = val-1;
			bandIdx[wlan_idx] = val;  
	}

	if (SSIDnum == 5) {	
	<%	write ("\t\tmode=\"" + getVirtualIndex("encrypt", 5) + "\";\n");
		write ("\t\tenable_1x=\"" + getVirtualIndex("enable1X", 5) + "\";\n");
		write ("\t\twlan_auth=\"" + getVirtualIndex("authType", 5) + "\";\n");
		write ("\t\twpa_auth=\"" + getVirtualIndex("wpaAuth", 5) + "\";\n");
		write ("\t\twepmode=\"" + getVirtualIndex("wep", 5) + "\";\n");
		write ("\t\twep_key_fmt=\"" + getVirtualIndex("keyType", 5) + "\";\n");
		write ("\t\twpa_cipher=\"" + getVirtualIndex("wpaCipher", 5) + "\";\n");
		write ("\t\twpa2_cipher=\"" + getVirtualIndex("wpa2Cipher", 5) + "\";\n");
		write ("\t\tpsk_fmt=\"" + getVirtualIndex("pskFormat", 5) + "\";\n");
	%>	
		dF.pskValue<% write(getIndex("wlan_idx")); %>.value="<%getVirtualInfo("pskValue", 5);%>";	
		dF.radiusIP<% write(getIndex("wlan_idx")); %>.value="<%getVirtualInfo("rsIp", 5);%>";
		dF.radiusPort<% write(getIndex("wlan_idx")); %>.value="<%getVirtualInfo("rsPort", 5);%>";
		dF.radiusPass<% write(getIndex("wlan_idx")); %>.value="<%getVirtualInfo("rsPassword", 5);%>";
		var val=<% write(getVirtualIndex("band", 5)); %>;
		if (val > 0) val = val-1;
			bandIdx[wlan_idx] = val;  
	}

	if(mode =='0')
		dF.method<% write(getIndex("wlan_idx")); %>.selectedIndex=0;
	else if(mode =='1')
		dF.method<% write(getIndex("wlan_idx")); %>.selectedIndex=1;
	else if(mode == '2')
		dF.method<% write(getIndex("wlan_idx")); %>.selectedIndex=2;
	else if (mode =='4')
		dF.method<% write(getIndex("wlan_idx")); %>.selectedIndex=3;
	else if (mode =='6' && ap_mode != 1)	
		dF.method<% write(getIndex("wlan_idx")); %>.selectedIndex=4;	
	else 
		dF.method<% write(getIndex("wlan_idx")); %>.selectedIndex=0;

	if (enable_1x != '0')
		dF.use1x<% write(getIndex("wlan_idx")); %>.checked=true;	
	else
		dF.use1x<% write(getIndex("wlan_idx")); %>.checked=false;
	show_authentication();	

	dF.auth_type[wlan_auth].checked = true;

	if (wepmode != 2)
		dF.length<% write(getIndex("wlan_idx")); %>.selectedIndex=0;
	else		
		dF.length<% write(getIndex("wlan_idx")); %>.selectedIndex=1;
	
	if (wep_key_fmt == "0")
		dF.format<% write(getIndex("wlan_idx")); %>.selectedIndex=0;
	else
		dF.format<% write(getIndex("wlan_idx")); %>.selectedIndex=1;
		
	updateWepFormat(document.formEncrypt, <% write(getIndex("wlan_idx")); %>);		
	if (wpa_auth & 1)
		dF.wpaAuth<% write(getIndex("wlan_idx")); %>[0].checked=true;
	else
		dF.wpaAuth<% write(getIndex("wlan_idx")); %>[1].checked=true;
	if (psk_fmt ==0)
		dF.pskFormat<% write(getIndex("wlan_idx")); %>.selectedIndex=0;
	else		
		dF.pskFormat<% write(getIndex("wlan_idx")); %>.selectedIndex=1;
		
	if (wpa_cipher & 1)
		dF.ciphersuite<% write(getIndex("wlan_idx")); %>[0].checked=true;
	if (wpa_cipher & 2)	
		dF.ciphersuite<% write(getIndex("wlan_idx")); %>[1].checked=true;
	if (wpa2_cipher & 1)
		dF.wpa2ciphersuite<% write(getIndex("wlan_idx")); %>[0].checked=true;
	if (wpa2_cipher & 2)	
			dF.wpa2ciphersuite<% write(getIndex("wlan_idx")); %>[1].checked=true;
//	if (wpa2_preAuth !=0)	
//		dF.preAuth<% write(getIndex("wlan_idx")); %>.checked=true;
	
	if (wepmode!=2)
		dF.wepKeyLen<% write(getIndex("wlan_idx")); %>[0].checked=1;
	else
		dF.wepKeyLen<% write(getIndex("wlan_idx")); %>[1].checked=1;

	if (mode != '0' && mode != '1')
		show_wpa_settings();	
	defPskLen[wlan_idx] = dF.elements["pskValue"+wlan_idx].value.length;
	defPskFormat[wlan_idx] = dF.elements["pskFormat"+wlan_idx].selectedIndex;						
}

function check_wepkey(form, wlan_id)
{
  var keyLen;
  if (form.elements["length"+wlan_id].selectedIndex == 0) {
  	if ( form.elements["format"+wlan_id].selectedIndex == 0)
		keyLen = 5;
	else
		keyLen = 10;
  }
  else {
  	if ( form.elements["format"+wlan_id].selectedIndex == 0)
		keyLen = 13;
	else
		keyLen = 26;
  }

  if (validateKey_wep(form, 0,form.elements["key"+wlan_id].value, keyLen, wlan_id)==0) {
	var tF= document.forms[0];  	
	tF.key<% write(getIndex("wlan_idx")); %>.focus();
	return false;
  }
  return true;
}

function save_confirm(wlan_id)
{
	ret = saveChanges_wpa(document.formEncrypt, wlan_id);
	if (ret == false) 
		parent.ValidateForm(parent.document.forms[0], 1);		
	return ret;
}


function ValidateForm(passForm)
{
	var tF= document.forms[0];

	var enc_mode = tF.method<% write(getIndex("wlan_idx")); %>.value;

	if (tF.use1x<% write(getIndex("wlan_idx")); %>.checked)
		tF.use1x<% write(getIndex("wlan_idx")); %>.value = "ON";
	else
		tF.use1x<% write(getIndex("wlan_idx")); %>.value = "OFF";	

	if (enc_mode == 1 && !tF.use1x<% write(getIndex("wlan_idx")); %>.checked) {
		if (check_wepkey(tF, <% write(getIndex("wlan_idx")); %>) != true)
			return false;
	}
	else {
		if (save_confirm(<% write(getIndex("wlan_idx")); %>) == false)
			return false;
	}
	
	var auth;
	if (tF.auth_type[0].checked)
		auth = "open";
	else if (tF.auth_type[1].checked)
		auth = "shared";
	else
		auth = "auto";
	passForm.authType.value =auth;
	
	passForm.method<% write(getIndex("wlan_idx")); %>.value =enc_mode;
	if (tF.wepKeyLen<% write(getIndex("wlan_idx")); %>[0].checked)	
		passForm.wepKeyLen<% write(getIndex("wlan_idx")); %>.value= tF.wepKeyLen<% write(getIndex("wlan_idx")); %>[0].value;
	else		
		passForm.wepKeyLen<% write(getIndex("wlan_idx")); %>.value= tF.wepKeyLen<% write(getIndex("wlan_idx")); %>[1].value;
	passForm.radiusIP<% write(getIndex("wlan_idx")); %>.value = tF.radiusIP<% write(getIndex("wlan_idx")); %>.value;
	passForm.radiusPort<% write(getIndex("wlan_idx")); %>.value = tF.radiusPort<% write(getIndex("wlan_idx")); %>.value;
	passForm.radiusPass<% write(getIndex("wlan_idx")); %>.value = tF.radiusPass<% write(getIndex("wlan_idx")); %>.value;
	if (ap_mode != 1 && tF.use1x<% write(getIndex("wlan_idx")); %>.checked)		
		passForm.use1x<% write(getIndex("wlan_idx")); %>.value = "ON";
	else
		passForm.use1x<% write(getIndex("wlan_idx")); %>.value = "OFF";	
	passForm.length<% write(getIndex("wlan_idx")); %>.value = tF.length<% write(getIndex("wlan_idx")); %>.value;
	passForm.format<% write(getIndex("wlan_idx")); %>.value = tF.format<% write(getIndex("wlan_idx")); %>.value;
	passForm.key<% write(getIndex("wlan_idx")); %>.value = tF.key<% write(getIndex("wlan_idx")); %>.value;
	passForm.wpaAuth<% write(getIndex("wlan_idx")); %>[0].checked = tF.wpaAuth<% write(getIndex("wlan_idx")); %>[0].checked;
	passForm.wpaAuth<% write(getIndex("wlan_idx")); %>[1].checked = tF.wpaAuth<% write(getIndex("wlan_idx")); %>[1].checked;	
//	if (tF.preAuth<% write(getIndex("wlan_idx")); %>.checked)
//		passForm.preAuth<% write(getIndex("wlan_idx")); %>.value = "ON";
//	else
//		passForm.preAuth<% write(getIndex("wlan_idx")); %>.value = "OFF";		

	if (tF.wpaAuth<% write(getIndex("wlan_idx")); %>[1].checked)
	{
		wlanMode = "<% write(getIndex("wlanMode"));%>"*1;
		
		if(enc_mode == 2) //WPA only
		{
			if(tF.ciphersuite<% write(getIndex("wlan_idx")); %>[0].checked == false &&
			 	 tF.ciphersuite<% write(getIndex("wlan_idx")); %>[1].checked == false )
			{
				alert("WPA Cipher Suite Can't be empty.");
				return false;
			}
			
			if(wlanMode == 1 && tF.ciphersuite0[0].checked == true && tF.ciphersuite0[1].checked == true)
			{
				alert("In the Client mode, you can't select TKIP and AES in the same time.");
				return false;				
			}
		}
		else if(enc_mode == 4) //WPA2 only
		{
			if(tF.wpa2ciphersuite<% write(getIndex("wlan_idx")); %>[0].checked == false &&
			 	 tF.wpa2ciphersuite<% write(getIndex("wlan_idx")); %>[1].checked == false )
			{
				alert("WPA2 Cipher Suite Can't be empty.");
				return false;
			}

if(wlanMode == 1 && tF.wpa2ciphersuite0[0].checked == true && tF.wpa2ciphersuite0[1].checked == true)
			{
				alert("In the Client mode, you can't select TKIP and AES in the same time.");
				return false;				
			}
		}
		else if(enc_mode == 5)//mix mode
		{
			if(tF.ciphersuite<% write(getIndex("wlan_idx")); %>[0].checked == false &&
			 	 tF.ciphersuite<% write(getIndex("wlan_idx")); %>[1].checked == false )
			{
				alert("WPA Cipher Suite Can't be empty.");
				return false;
			}
			
			if(tF.wpa2ciphersuite<% write(getIndex("wlan_idx")); %>[0].checked == false &&
			 	 tF.wpa2ciphersuite<% write(getIndex("wlan_idx")); %>[1].checked == false )
			{
				alert("WPA2 Cipher Suite Can't be empty.");
				return false;
			}
		}
	}
	passForm.ciphersuite<% write(getIndex("wlan_idx")); %>[0].checked = tF.ciphersuite<% write(getIndex("wlan_idx")); %>[0].checked;
	passForm.ciphersuite<% write(getIndex("wlan_idx")); %>[1].checked = tF.ciphersuite<% write(getIndex("wlan_idx")); %>[1].checked;
	passForm.wpa2ciphersuite<% write(getIndex("wlan_idx")); %>[0].checked = tF.wpa2ciphersuite<% write(getIndex("wlan_idx")); %>[0].checked;
	passForm.wpa2ciphersuite<% write(getIndex("wlan_idx")); %>[1].checked = tF.wpa2ciphersuite<% write(getIndex("wlan_idx")); %>[1].checked;			
	passForm.pskFormat<% write(getIndex("wlan_idx")); %>.value= tF.pskFormat<% write(getIndex("wlan_idx")); %>.value;
	passForm.pskValue<% write(getIndex("wlan_idx")); %>.value = tF.pskValue<% write(getIndex("wlan_idx")); %>.value;

	var ssidIndex = parent.document.forms[0].SSID_Setting.selectedIndex; 
	passForm.wlan_ssid_id.value=  ssidIndex;
	passForm.wlan_ssid.value=  parent.document.forms[0].SSID_Setting.options[ssidIndex].value;
	return true;
}
</script>
</head>

<body onload="Load_Setting();">
<blockquote>

<form method=POST name="formEncrypt">
<table width="100%" border="0" cellpadding="0" cellspacing="4">	
	<tr>
		<td colspan="2" width="100%">
			<table width="100%" border="0" cellpadding="0" cellspacing="4"> 
				<tr>
					 <td width="30%" class="bgblue"><font size="2"><b>Encryption:</b></font></td>					 
					 <td width="70%" class="bggrey"><font size="2">
					 <select size="1"  id="method" name="method<% write(getIndex("wlan_idx")); %>" onChange="show_authentication()">	 
						<option value="0"> Disable </option>
			 			<option value="1"> WEP </option>
						<option value="2"> WPA </option>
						<option value="4"> WPA2 </option> 
						<script>		
							var dF=document.forms[0];
							var idx = parent.document.forms[0].SSID_Setting.selectedIndex;						
							SSIDnum = parent.document.forms[0].SSID_Setting[idx].value;
							if (SSIDnum == 0) {
								<%	write ("\t\tap_mode=\"" + getVirtualIndex("wlanMode", 0) + "\";\n"); %>
							}	
							else if (SSIDnum == 1) {
								<%	write ("\t\tap_mode=\"" + getVirtualIndex("wlanMode", 1) + "\";\n"); %>
							}	
							else if (SSIDnum == 2) {
								<%	write ("\t\tap_mode=\"" + getVirtualIndex("wlanMode", 2) + "\";\n"); %>
							}	
							else if (SSIDnum == 3) {
								<%	write ("\t\tap_mode=\"" + getVirtualIndex("wlanMode", 3) + "\";\n"); %>
							}	
							else if (SSIDnum == 4) {
								<%	write ("\t\tap_mode=\"" + getVirtualIndex("wlanMode", 4) + "\";\n"); %>
							}	
							else if (SSIDnum == 5) {
								<%	write ("\t\tap_mode=\"" + getVirtualIndex("wlanMode", 5) + "\";\n"); %>
							}	
							if (ap_mode != 1)
							  document.write('<option value=\"6\"> WPA-Mixed </option>');
						</script>						
					</select> </font>
				</td> </tr>
			</table>
	</td></tr>			

	<tr id="enable_8021x" style="display:none">
		<td colspan="2" width="100%">			
			<table width="100%" border="0" cellpadding="0" cellspacing="4">
			 	<tr>
					 <td width="30%" class="bgblue"><font size="2"><b>802.1x Authentication:</b></font></td>
					 <td width="70%" class="bggrey"><font size="2">
						<input type="checkbox" id="use1x" name="use1x<% write(getIndex("wlan_idx")); %>" value="ON" onClick="show_8021x_settings()">
							</font>
				</td></tr>
			</table>
	</td></tr>			

	<tr id="show_wep_auth" style="display:none">
		<td colspan="2" width="100%">			
			<table width="100%" border="0" cellpadding="0" cellspacing="4">
			 	<tr>	
					<td width="30%"bgcolor="#FFFFFF" class="bgblue"><font size="2"><b>Authentication:</b></font></td>
					<td width="70%" class="bggrey"><font size="2">
						<input name="auth_type" type=radio value=0>Open System
						<input name="auth_type" type=radio value=1>Shared Key
						<input name="auth_type" type=radio value=2>Auto
							</font>
				</td></tr>
			</table>
	</td></tr>			

	<tr id="setting_wep" style="display:none">	
		<td colspan="2" width="100%">			
			<table width="100%" border="0" cellpadding="0" cellspacing="4">
			      <input type="hidden" name="wepEnabled<% write(getIndex("wlan_idx")); %>" value="ON" checked>			
			 	<tr bgcolor="#FFFFFF">
					<td width="30%" bgcolor="#FFFFFF" class="bgblue"><font size="2"><b>Key Length:</b></font></td>
					<td width="70%" class="bggrey"><font size="2">
					<select size="1" name="length<% write(getIndex("wlan_idx")); %>" id="length" onChange="updateWepFormat(document.formEncrypt, <% write(getIndex("wlan_idx")); %>)">	
						 <option value=1> 64-bit</option>
						 <option value=2>128-bit</option>
					</select></font>
				</td></tr>
				<tr bgcolor="#FFFFFF">
					<td width="30%" bgcolor="#FFFFFF" class="bgblue"><font size="2"><b>Key Format:</b></font></td>
					<td width="70%" class="bggrey">
					<select id="format" name="format<% write(getIndex("wlan_idx")); %>" onChange="setDefaultKeyValue(document.formEncrypt, <% write(getIndex("wlan_idx")); %>)">
					     	<option value="1">ASCII</option>
						<option value="2">Hex</option>					
					</select>
				</td></tr>
				<tr bgcolor="#FFFFFF">
					<td width="30%" bgcolor="#FFFFFF" class="bgblue"><font size="2"><b>Encryption Key:</b></font></td>
					<td width="70%" class="bggrey">
						<input type="text" id="key" name="key<% write(getIndex("wlan_idx")); %>" maxlength="26" size="26" value="">
				</td></tr> 
			</table>				
	</td></tr>     

	<tr id="setting_wpa" style="display:none">
		<td colspan="2">	
			<table width="100%" border="0" cellpadding="0" cellspacing="4">			
				<tr>
					<td width="30%" class="bgblue"><font size="2"><b>Authentication Mode:</b></font></td>
					<td width="70%" class="bggrey"><font size="2">
						<input name="wpaAuth<% write(getIndex("wlan_idx")); %>" type="radio" value="eap" onClick="show_wpa_settings()">Enterprise (RADIUS)
						<input name="wpaAuth<% write(getIndex("wlan_idx")); %>" type="radio" value="psk" onClick="show_wpa_settings()">Personal (Pre-Shared Key)
							</font>
				</td></tr>  

				<tr id="show_wpa_cipher" style="display:none">
					<td width="30%" class="bgblue"><font size="2"><b>WPA Cipher Suite:</b></font></td>
					<td width="70%" class="bggrey"><font size="2">
						<input type="checkbox" name="ciphersuite<% write(getIndex("wlan_idx")); %>" value="tkip">TKIP&nbsp;
						<input type="checkbox" name="ciphersuite<% write(getIndex("wlan_idx")); %>" value="aes">AES
							</font>
				</td></tr> 
					
				<tr id="show_wpa2_cipher" style="display:none">
					<td width="30%"class="bgblue"><font size="2"><b>WPA2 Cipher Suite:</b></font></td>
					<td width="70%" class="bggrey"><font size="2">
						<input type="checkbox" name="wpa2ciphersuite<% write(getIndex("wlan_idx")); %>" value="tkip">TKIP&nbsp;
						<input type="checkbox" name="wpa2ciphersuite<% write(getIndex("wlan_idx")); %>" value="aes">AES
							</font>
				</td></tr>
<!--					
				<tr id="show_pre_auth" style="display:none">
					<td width="30%" class="bgblue"><font size="2"><b>Enable Pre-Authentication:</b></font></td>
					<td width="70%" class="bggrey"><font size="2">
						<input type="checkbox" name="preAuth<% write(getIndex("wlan_idx")); %>" value="ON">
							</font>
				</td></tr>
-->				
  
				<tr id="show_wpa_psk1" style="display:none">				
					<td width="30%" bgcolor="#FFFFFF" class="bgblue"><font size="2"><b>Pre-Shared&nbsp;Key&nbsp;Format:</b></font></td>
					<td width="70%" class="bggrey">
					<select id="psk_fmt" name="pskFormat<% write(getIndex("wlan_idx")); %>" onChange="">
						<option value="0">Passphrase</option>
						<option value="1">HEX (64 characters)</option>
						</select>
				</td></tr>
				<tr id="show_wpa_psk2" style="display:none">
					<td width="30%" bgcolor="#FFFFFF" class="bgblue"><font size="2"><b>Pre-Shared&nbsp;Key:</b></font></td>
					<td width="70%" class="bggrey"><input type="password" name="pskValue<% write(getIndex("wlan_idx")); %>" id="wpapsk" size="32" maxlength="64" value="">
				</td></tr>
			</table>		
	</td></tr>	 

	<tr id="show_1x_wep" style="display:none">
		<td colspan="2">	
			<table width="100%" border="0" cellpadding="0" cellspacing="4">			
				<tr>	
					<td width="30%" class="bgblue"><font size="2"><b>Key Length:</b></font></td>
					<td width="70%" class="bggrey"><font size="2">
						<input name="wepKeyLen<% write(getIndex("wlan_idx")); %>" type="radio" value="wep64">64 Bits
						<input name="wepKeyLen<% write(getIndex("wlan_idx")); %>" type="radio" value="wep128">128 Bits
							</font>
				</td></tr>
		 	</table>
	</td></tr> 

	<tr id="show_8021x_eap" style="display:none">
		<td colspan="2">			
			<table width="100%" border="0" cellpadding="0" cellspacing="4"> 
				<tr>
					 <td width="30%" bgcolor="#FFFFFF" class="bgblue"><font size="2"><b>RADIUS&nbsp;Server&nbsp;IP&nbsp;Address:</b></font></td>
					 <td width="70%" bgcolor="#FFFFFF" class="bggrey"><input id="radius_ip" name="radiusIP<% write(getIndex("wlan_idx")); %>" size="16" maxlength="15" value="0.0.0.0"></td>
				 </tr>
				<tr>
					<td width="30%" bgcolor="#FFFFFF" class="bgblue"><font size="2"><b>RADIUS&nbsp;Server&nbsp;Port:</b></font></td>
					<td width="70%" class="bggrey"><input type="text" id="radius_port" name="radiusPort<% write(getIndex("wlan_idx")); %>" size="5" maxlength="5" value="1812"></td>
				 </tr>
				<tr>
					<td width="30%" class="bgblue"><font size="2"><b>RADIUS&nbsp;Server&nbsp;Password:</b></font></td>
					<td width="70%" class="bggrey"><input type="password" id="radius_pass" name="radiusPass<% write(getIndex("wlan_idx")); %>" size="32" maxlength="64" value=""></td>
				</tr>
			</table>								
	</td></tr>
			
	<script> updateWepFormat(document.formEncrypt, <% write(getIndex("wlan_idx")); %>);</script>
</table>
</form>
</blockquote>
</body>
</html>
