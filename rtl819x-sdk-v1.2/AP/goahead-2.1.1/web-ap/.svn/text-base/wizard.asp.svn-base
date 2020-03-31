<html>
<! Copyright (c) Realtek Semiconductor Corp., 2003. All Rights Reserved. ->
<head>
<meta http-equiv="Content-Type" content="text/html">
<title>Setup Wizard</title>
<script type="text/javascript" src="util_ap.js"></script>
<style>
.on {display:on}
.off {display:none}
</style>
<script>
//for wlan1 page
var wlan_channel=new Array();
var wlan_idx=<% write(getIndex("wlan_idx")); %> ;
var wlan_num=<% write(getIndex("wlan_num")); %> ;
var WiFiTest=<% write(getIndex("WiFiTest")); %> ;
var isMeshDefined = <% write(getIndex("isMeshDefined")); %> ;
var isNewMeshUI =  <% write(getIndex("isNewMeshUI")); %> ;
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
var rpt_ssid=new Array();

var rpt_mode=new Array();
var rpt_enable_1x=new Array();
var rpt_wlan_auth=new Array();
var rpt_wpa_auth=new Array();
var rpt_wepmode=new Array();
var rpt_wep_key_fmt=new Array();
var rpt_wpa_cipher=new Array();
var rpt_wpa2_cipher=new Array();
var rpt_psk_fmt=new Array();
var rpt_psk_key=new Array();

var init_bound = "<% if (getIndex("ChannelBonding")==0) write("0"); %>";
var init_sideband="<% if (getIndex("ControlSideBand")==0) write("0"); %>";

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
  //brad20070726
   //setDefaultKeyValue(form, wlan_id);
   
}

function wizardHideDiv()
{
	
	show_div(false, "top_div");
	show_div(false, "lan_div");
	<% 
	   var i ;
	   for (i=0; i<getIndex("wlan_num"); i=i+1) 
	     { 
		write('show_div(false, "wlan1_div'+i+'");\n'); 
		write('show_div(false, "wlan2_div'+i+'");\n'); 
		write('show_div(false, "survey_top_div'+i+'");\n');
		write('show_div(false, "survey_security_div'+i+'");\n');
	     } 
	%>
}

function saveClick_lan(next){
	form =  document.wizard;
	if(saveChanges_lan(form) ==false)
		return false ;
	else{
		wizardHideDiv();
		if(next)
			show_div(true, ("wlan1_div0"));
		else
			show_div(true, "top_div");
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
				show_div(true, "lan_div");
			else
				show_div(true, ("wlan2_div"+(wlan_id-1)));
		}
	}
	
	var isRptOn=0;
	if(form.elements["wlanRptEnabled"+wlan_id].checked == true)
		isRptOn = 1;


	if( (APMode[wlan_id] == 0 && isRptOn == 1) || APMode[wlan_id] == 1 ) // 0:AP 1:CLIENT
	{
		form.elements["wlan2_div_next"+wlan_id].value = "  Next>>";
	}
	else
		form.elements["wlan2_div_next"+wlan_id].value = "Finished";
	
		
}
function update_controlsideband(form, wlan_id)
{
	var index=document.wizard.elements["channelbound"+<% write(getIndex("wlan_idx")); %>].selectedIndex;
	
	if(index ==0)
		disableTextField(document.wizard.elements["controlsideband"+<% write(getIndex("wlan_idx")); %>]);	
	else
		enableTextField(document.wizard.elements["controlsideband"+<% write(getIndex("wlan_idx")); %>]);
	updateChan_channebound(form, wlan_id);	
	var chan_number_idx=form.elements["chan"+wlan_idx].selectedIndex;	
	if(chan_number==0)
		disableTextField(document.wizard.elements["controlsideband"+<% write(getIndex("wlan_idx")); %>]);	

}

function updateChan_selectedIndex(form, wlan_id)
{
	var chan_number_idx=form.elements["chan"+wlan_id].selectedIndex;
	var chan_number= form.elements["chan"+wlan_id].options[chan_number_idx].value;
	
	wlan_channel[wlan_id] = chan_number;
	if(chan_number == 0)
		disableTextField(document.wizard.elements["controlsideband"+wlan_id]);	
	else{
		if(document.wizard.elements["channelbound"+wlan_id].selectedIndex == "0")
 			disableTextField(document.wizard.elements["controlsideband"+<% write(getIndex("wlan_idx")); %>]);	
 		else
			enableTextField(document.wizard.elements["controlsideband"+<% write(getIndex("wlan_idx")); %>]);		
		}
} 
function Set_onChangeBand(form, wlan_id, band, index){
	    var band;
	    var auto;
 	    var txrate;
	var value;
	
	 var checkid_bound=document.getElementById("channel_bounding");
	 var checkid_sideband = document.getElementById("control_sideband");
	value =band.options[index].value;
	if(value ==9 || value ==10){
		checkid_bound.style.display = "";
		checkid_sideband.style.display = "";
	}else{
		checkid_bound.style.display = "none";
		checkid_sideband.style.display = "none";
	}
	
	
	var chan_number_idx=form.elements["chan"+wlan_idx].selectedIndex;
	var chan_number= form.elements["chan"+wlan_idx].options[chan_number_idx].value;
	wlan_channel[wlan_idx] = chan_number;
	updateChan_channebound(form, wlan_id);		
}


function cancelClick(){
	
	window.location="wizard.asp" ;
}

function initSurveyAuth(wlan_id)
{
	// set survey ecrypt
	if(rpt_mode[wlan_id] == 0)
		form.elements["survey_encrypt"+wlan_id].value = "no";
	else if(rpt_mode[wlan_id] == 1)
		form.elements["survey_encrypt"+wlan_id].value = "WEP";
	else if(rpt_mode[wlan_id] == 2)
		form.elements["survey_encrypt"+wlan_id].value = "WPA-PSK";
	else
		form.elements["survey_encrypt"+wlan_id].value = "WPA2-PSK";
		
	//set survey wep key length
	if(rpt_wepmode[wlan_id] ==2 ) //1:64; 2:128
		form.elements["survey_length"+wlan_id].selectedIndex = 1;
	else
		form.elements["survey_length"+wlan_id].selectedIndex = 0;
		
	if(rpt_wep_key_fmt[wlan_id] ==0 ) //0:ascii; 1:hex
		form.elements["survey_format"+wlan_id].selectedIndex = 0;
	else
		form.elements["survey_format"+wlan_id].selectedIndex = 1;
		
	setDefaultWEPKeyValue(document.wizard, wlan_id,"survey_length","survey_format","survey_key");
	
	
	//set survey cipher
	if(rpt_wpa_cipher[wlan_id] == 1) //1:TKIP
		form.elements["survey_ciphersuite"+wlan_id].value = "tkip";
	else
		form.elements["survey_ciphersuite"+wlan_id].value = "aes";		
	
	if(rpt_wpa2_cipher[wlan_id] == 1) //1:TKIP
		form.elements["survey_wpa2ciphersuite"+wlan_id].value = "tkip";
	else
		form.elements["survey_wpa2ciphersuite"+wlan_id].value = "aes";
		
	if(rpt_mode[wlan_id]==2) //WPA
	{
		form.elements["survey_cipher"+wlan_id].selectedIndex= (rpt_wpa_cipher[wlan_id]*1)-1;
	}
	else if(rpt_mode[wlan_id]==4) //WPA2
	{
		form.elements["survey_cipher"+wlan_id].selectedIndex= (rpt_wpa2_cipher[wlan_id].value*1)-1;
	}
		
	//set survey pskFormat
	if(rpt_psk_fmt[wlan_id]==0)
		form.elements["survey_pskFormat"+wlan_id].selectedIndex= 0;
	else
		form.elements["survey_pskFormat"+wlan_id].selectedIndex= 1;
		
	setDefaultWPAKeyValue(document.wizard, wlan_id,"survey_pskFormat", "survey_pskValue");
}
function saveClick_wlan2(next, wlan_id)
{

	form =  document.wizard;
	if(form.elements["method"+wlan_id].selectedIndex == 2){
		if(saveChanges_wpa(form, wlan_id) == false)
			return false ;
	}else if(form.elements["method"+wlan_id].selectedIndex == 1){
	//brad20070726
	/*
		if(saveChanges_wep(form, wlan_id) == false)
			return false ;
	*/
		if(saveChanges_wepkey(form, wlan_id) == false)
			return false;
	}
	if(form.elements["method"+wlan_id].selectedIndex == 0)
		form.elements["wepEnabled"+wlan_id].value =  "OFF" ;
	else
		form.elements["wepEnabled"+wlan_id].value =  "ON" ;
	
	wizardHideDiv();
	if(next != 1) //prev
		show_div(true, ("wlan1_div"+wlan_id));
	else if (next == 1) //next
	{
			var isRptOn=0;
		if(form.elements["wlanRptEnabled"+wlan_id].checked == true)
				isRptOn = 1;

		if( APMode[wlan_id] == 0 && isRptOn == 1)
			{
			initSurveyAuth(wlan_id);
			show_div(true, ("survey_top_div"+wlan_id));							
			form.elements["survey_ssid"+wlan_id].value = form.elements["repeaterSSID"+wlan_id].value;
			form.elements["isSurveyHandle"+wlan_id].value = 1;
			}
		else if(wlan_id < (wlan_num-1))
			{
				show_div(true, ("wlan1_div"+(wlan_id+1)));	
			}
		else
		{
			form.submit();			
		}
	}

	return true ;
}
function Load_Setting()
{
	var dF=document.forms[0];
	<%
		write ("\t\twpa_cipher=\"" + getVirtualIndex("wpaCipher", 0) + "\";\n");
		write ("\t\twpa2_cipher=\"" + getVirtualIndex("wpa2Cipher", 0) + "\";\n");
	%>	
	//var dF=document.forms[0];
	if (wpa_cipher == 0)
		wpa_cipher = 1;
	if (wpa2_cipher == 0)
		wpa2_cipher = 2;	
	if (wpa_cipher & 1)
		dF.ciphersuite<% write(getIndex("wlan_idx")); %>[0].value = "tkip";
	if (wpa_cipher & 2)	
		dF.ciphersuite<% write(getIndex("wlan_idx")); %>[1].value =  "aes";
	if (wpa2_cipher & 1)
		dF.wpa2ciphersuite<% write(getIndex("wlan_idx")); %>[0].value = "tkip";
	if (wpa2_cipher & 2)	
		dF.wpa2ciphersuite<% write(getIndex("wlan_idx")); %>[1].value = "aes";	
}
</script>
</head>
<body onload="Load_Setting();">

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
		<li>Setup LAN Interface</li>
		<li>Wireless LAN Setting</li>
		<li>Wireless Security Setting</li>
		</ol>
	</font></td> 
</tr>
<!--
</td></tr>
-->
</table>
<div style="LEFT: 60px; WIDTH: 300px; POSITION: absolute; TOP: 260px; HEIGHT: 100px">  
<table border=0 width="500" cellspacing=4 cellpadding=0 >
<tr>
  <td colspan="2" align=right> 

	<input type="button" value="  Next>>" name="next" onClick='wizardHideDiv();show_div(true, "lan_div");'>

  </td>
</tr>
</table>
</div>
</span>

<!-- lan page -->
<span id = "lan_div" class = "off" >
<table border=0 width="500" cellspacing=4 cellpadding=0>
  <tr><td colspan="2"><h2><font color="#0000FF">1. LAN Interface Setup</font></h2></td></tr>
  <tr><td  colspan="2"><font size=2>
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
			if (val > 0) 
				val=val-1;
			write("bandIdx["+i+"] ="+val+";\n"); 
			
			write("networkType["+i+"] ="+getIndex("networkType")+";\n"); 
			write ("regDomain["+i+"]=" +getIndex("regDomain")+ ";\n");
			write ("defaultChan["+i+"]=" +getIndex("channel")+ ";\n");
			write ("usedBand["+i+"]=" +getIndex("band")+ ";\n");
			write ("ssid["+i+"]=\""); getInfo("ssid");write("\";\n");
			write ("encrypt["+i+"]=" +getIndex("encrypt")+ ";\n");
			write ("wep["+i+"]=" +getIndex("wep")+ ";\n");
			write ("defaultKeyId["+i+"]=" +getIndex("defaultKeyId")+ ";\n");
			write ("defPskFormat["+i+"]=" +getIndex("pskFormat")+ ";\n");
			write ("macClone["+i+"]=" +getIndex("wlanMacClone")+ ";\n");
			write ("pskValue["+i+"]=\""); getInfo("pskValue");write("\";\n");
			write ("keyType["+i+"]=" +getIndex("keyType")+ ";\n");			
			
			write ("rpt_ssid["+i+"]='");getInfo("repeaterSSID");write("';\n");			
			
			write ("rpt_mode["+i+"]=\"" + getVirtualIndex("encrypt", 5) + "\";\n");
			write ("rpt_enable_1x["+i+"]=\"" + getVirtualIndex("enable1X", 5) + "\";\n");
			write ("rpt_wlan_auth["+i+"]=\"" + getVirtualIndex("authType", 5) + "\";\n");
			write ("rpt_wpa_auth["+i+"]=\"" + getVirtualIndex("wpaAuth", 5) + "\";\n");
			write ("rpt_wepmode["+i+"]=\"" + getVirtualIndex("wep", 5) + "\";\n");
			write ("rpt_wep_key_fmt["+i+"]=\"" + getVirtualIndex("keyType", 5) + "\";\n");
			write ("rpt_wpa_cipher["+i+"]=\"" + getVirtualIndex("wpaCipher", 5) + "\";\n");
			write ("rpt_wpa2_cipher["+i+"]=\"" + getVirtualIndex("wpa2Cipher", 5) + "\";\n");
			write ("rpt_psk_fmt["+i+"]=\"" + getVirtualIndex("pskFormat", 5) + "\";\n");
			write ("rpt_psk_key["+i+"]='");getVirtualInfo("pskValue", 5);write("';\n");			
		}
		
	%>
wlan_channel[wlan_idx] = defaultChan[wlan_idx];
menu_num =2;
for(i=0; i< wlan_num; i++){
/**********************************************************/
document.write(' \
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
      <td width="74%"><font size=2><select size="1" name="band'+i+'" onChange="updateBand(document.wizard, '+i+');Set_onChangeBand(document.wizard, '+i+', document.wizard.band'+i+', document.wizard.band'+i+'.selectedIndex);">');
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
      <td width="74%"><font size=2><select size="1" name="mode'+i+'"	onChange="updateMode(document.wizard, '+i+');updateChan_channebound(document.wizard, '+i+');">');
	 document.write('<%  getModeCombobox(); %>');  
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
  </tr>');
  //end of document write
  
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
//	if (autoconf[i]) {
//		if (keyinstall[i]) {
//			if(autoCfgWlanMode[i] == APMode[i]){
//				document.write("(EasyConfig)");
//				disableSSID[i]=1;
//			}
//		}
//	}
    } 
/**********************************************************/
   	 
	
		
/*************************************************************/
  
     
/**********************************************************/
document.write('\
      </b></td>\
      <td width="74%"><font size=2><input type="text" name="ssid'+i+'" size="33" maxlength="32" value="'+ssid[i]+'">\
      </td>\
  </tr>\
  <tr id="channel_bounding" style="display:none">\
   <td width="26%"><font size=2><b>Channel Width:</b></td>\
 <td width="74%"><font size=2><select size="1" name="channelbound'+i+'"	onChange="update_controlsideband(document.wizard, '+i+')">\
 	<option value=\"0\">20MHz</option>"\
	 "<option value=\"1\">40MHz</option>" \
	 </select>\
	 </td>\
	 </tr>\
	 <tr id="control_sideband" style="display:none">\
      <td width="26%"><font size=2><b>ControlSideband:</b></td>\
      <td width="74%"><font size=2><select size="1" name="controlsideband'+i+'" onChange="updateChan_channebound(document.wizard, '+i+');">\
     <option value="0">Upper</option>\
	<option value="1">Lower</option>\
	 </select>\
      </td>\
  </tr>\
  <tr>\
      <td width="26%"><font size=2><b>Channel Number:</b></td>\
      <td width="74%"><font size=2><select size="1" id="chan'+i+'" name="chan'+i+'" onchange ="updateChan_selectedIndex(document.wizard, '+i+');"> </select></td>');
/**********************************************************/
/**************************************************************/
	var form = document.wizard ;	
	band = 10;
	var checkid_bound; 
	var checkid_sideband;
	checkid_bound=document.getElementById("channel_bounding");
	checkid_sideband=document.getElementById("control_sideband");
	if (band < 9){
		checkid_bound.style.display = "none";
 		checkid_sideband.style.display = "none";
	}else{
		checkid_bound.style.display = "";
 		checkid_sideband.style.display = "";  
	if(init_bound=="0")
 		document.wizard.elements["channelbound"+i].selectedIndex=0;
 	else
 		document.wizard.elements["channelbound"+i].selectedIndex=1;
 		
 	if(init_sideband=="0")
 		document.wizard.elements["controlsideband"+i].selectedIndex=0;
 	else
 		document.wizard.elements["controlsideband"+i].selectedIndex=1;	
 	if(init_bound == "0")
 		disableTextField(document.wizard.elements["controlsideband"+i]);	
 	else
		enableTextField(document.wizard.elements["controlsideband"+i]);		
	}
	

	updateChan(document.wizard, i);
	updateChan_channebound(document.wizard, i);	
	var chan_number_idx=form.elements["chan"+i].selectedIndex;
	var chan_number= form.elements["chan"+i].options[chan_number_idx].value;
	
	function rptClick(wlan_idx)
	{
		if(form.elements["wlanRptEnabled"+wlan_idx].checked)
		{
			form.elements["is_rpt_enabled"+wlan_idx].value = 1;
			form.elements["repeaterSSID"+wlan_idx].disabled = false;
		}
		else
		{
			form.elements["is_rpt_enabled"+wlan_idx].value = 0;
			form.elements["repeaterSSID"+wlan_idx].disabled = true;
		}
		
	}
/**********************************************************/
document.write('\
  </tr>\
  <tr>\
      <td width="100%" colspan=2><font size=2><b>\
   	<input type="checkbox" name="wlanMacClone'+i+'" value="ON"\
   	>&nbsp;&nbsp; Enable Mac Clone (Single Ethernet Client)</b>\
      </td>\
  </tr>\
  <tr id="wlanRptEnableDiv" style="display:none">\
      <td width="100%" colspan=2><font size=2><b>\
   	<input type="checkbox" name="wlanRptEnabled'+i+'" value="" onclick=rptClick('+i+');\
   	>&nbsp;&nbsp; Enable Universal Repeater Mode</b>\
      </td>\
  </tr>\
  <tr id="wlanRptSsidDiv" style="display:none">\
      <td width="100%" colspan=2><font size=2><b>SSID of Extended Interface:&nbsp;&nbsp;\
      <input type="text" id="repeaterSSID'+i+'" name="repeaterSSID'+i+'" size="33" maxlength="32" value="">\
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
		
		if(chan_number== 0)
		disableTextField(document.wizard.elements["controlsideband"+i]);
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
</table>');
if(document.wizard.elements["band"+i].value==7 || document.wizard.elements["band"+i].value==9 || document.wizard.elements["band"+i].value==10){
document.write('\
<table border=0 width="500" cellspacing=4 cellpadding=0>\
   <td width="35%"><font size="2"><b>Encryption:&nbsp;</b>\
        <select size="1" name="method'+i+'" onChange="checkState('+i+')">\
          <option  value="0">None</option>\
          <option value="1">WEP</option>\
	  <option value="4">WPA2(AES)</option>\
	  <option value="6">WPA2 Mixed</option>\
        </select></font></td>\
</table>');
}else
	{
		document.write('\
<table border=0 width="500" cellspacing=4 cellpadding=0>\
   <td width="35%"><font size="2"><b>Encryption:&nbsp;</b>\
        <select size="1" name="method'+i+'" onChange="checkState('+i+')">\
          <option  value="0">None</option>\
          <option value="1">WEP</option>\
          <option value="2">WPA (TKIP)</option>\
	  <option value="4">WPA2(AES)</option>\
	  <option value="6">WPA2 Mixed</option>\
        </select></font></td>\
</table>');
		
	}
document.write('\
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
      <td width="40%"><font size=2><select size="1" name="format'+i+'" ONCHANGE="setDefaultWEPKeyValue(document.wizard, '+i+',\'length\',\'format\',\'key\')">\
     	<option value=1>ASCII</option>\
	<option value=2>Hex</option>\
       </select></td>\
  </tr>\
  <tr>\
     <td width="15%"><font size=2><b>Key Setting:</b></td>\
     <td width="40%"><font size=2>\
     	<input type="text" name="key'+i+'" size="26" maxlength="26">\
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
	document.write('<input type="button" value="Finished" name="wlan2_div_next'+i+'" ');
else
	document.write('<input type="button" value="  Next>>" name="wlan2_div_next'+i+'" ');
document.write(' onClick="return saveClick_wlan2(1,'+i+')">\
  	</td> </tr>\
	<input type="hidden" value="" name="ciphersuite'+i+'">\
</table> \
</div>\
</span>');

function survey_checkState(wlanIdx)
{
  if(get_by_id("survey_method"+wlanIdx).selectedIndex == 0){ //none
  		show_div(false,("survey_wpa_div"+wlanIdx));	
  		show_div(false,("survey_wep_div"+wlanIdx));	
  }
  else if(get_by_id("survey_method"+wlanIdx).selectedIndex == 1){ //wep
  		show_div(false,("survey_wpa_div"+wlanIdx));	
  		show_div(true,("survey_wep_div"+wlanIdx));	
  }
  else if(get_by_id("survey_method"+wlanIdx).selectedIndex >= 2){ //wpa~
  		show_div(true,("survey_wpa_div"+wlanIdx));	
  		show_div(false,("survey_wep_div"+wlanIdx));	
  		
  		if(get_by_id("survey_method"+wlanIdx).selectedIndex == 2) //WPA
  		{
  			if(form.elements["survey_ciphersuite"+wlanIdx].value == "tkip")
  				form.elements["survey_cipher"+wlanIdx].selectedIndex= 0;
  			else
  				form.elements["survey_cipher"+wlanIdx].selectedIndex= 1;
  		}
  		else if(get_by_id("survey_method"+wlanIdx).selectedIndex == 3) //WPA2
  		{  	
  			if(form.elements["survey_wpa2ciphersuite"+wlanIdx].value == "tkip")
  				form.elements["survey_cipher"+wlanIdx].selectedIndex= 0;
  			else
  				form.elements["survey_cipher"+wlanIdx].selectedIndex= 1;
  		}
  }

}

function saveClickSSID(next, wlan_id)
{

	form =  document.wizard;	
	
	if(next == 0) //prev
	{
		wizardHideDiv();
		show_div(true, ("wlan2_div"+wlan_id));
	}
	else if (next == 1) //next
	{
		if(document.getElementById("survey_ssid"+wlan_id).value == "")
		{
			alert("SSID is empty!!");
			return true;
		}
		wizardHideDiv();

		if(document.getElementById("survey_selected"+wlan_id).value == 1)
		{
			if(document.getElementById("repeaterSSID"+wlan_id).value != document.getElementById("survey_ssid"+wlan_id).value)
			{
				document.getElementById("survey_encrypt"+wlan_id).value = "no";			
			}
		}
		else
		{
			if(rpt_ssid[wlan_id] != document.getElementById("survey_ssid"+wlan_id).value)
			{
				document.getElementById("survey_encrypt"+wlan_id).value = "no";			
			}
		}		
		
		if(document.getElementById("survey_encrypt"+wlan_id).value == "no")
			get_by_id("survey_method"+wlan_id).selectedIndex = 0;
		else if(document.getElementById("survey_encrypt"+wlan_id).value == "WEP")
			get_by_id("survey_method"+wlan_id).selectedIndex = 1;
		else if(document.getElementById("survey_encrypt"+wlan_id).value == "WPA-PSK")				
			get_by_id("survey_method"+wlan_id).selectedIndex = 2;
		else
			get_by_id("survey_method"+wlan_id).selectedIndex = 3;
		
		if(get_by_id("survey_channel"+wlan_id).value != "")
		{
			if(get_by_id('channelbound'+wlan_id).selectedIndex == 1)//40MHz
			{
				if(get_by_id("survey_channel"+wlan_id).value >= 1 && get_by_id("survey_channel"+wlan_id).value <=4) //lower
				{
					get_by_id('controlsideband'+wlan_id).selectedIndex = 1; //lower
				}
				else if(get_by_id("survey_channel"+wlan_id).value >= 8 && get_by_id("survey_channel"+wlan_id).value <=11) //upper
				{
					get_by_id('controlsideband'+wlan_id).selectedIndex = 0; //upper
				}
			}
			else //20MHz
			{
				
			}
			get_by_id('chan'+wlan_id).value = get_by_id("survey_channel"+wlan_id).value*1;
		}
		
		show_div(true, ("survey_security_div"+wlan_id));
		survey_checkState(wlan_id);
	}

	return true ;
}

var getFFVersion=navigator.userAgent.substring(navigator.userAgent.indexOf("Firefox")).split("/")[1]
//extra height in px to add to iframe in FireFox 1.0+ browsers
var FFextraHeight=getFFVersion>=0.1? 16 : 0 
function dyniframesize() {
	var iframename ="SSIDSiteSurvey";
  var pTar = null;
  if (document.getElementById){
    pTar = document.getElementById(iframename);
  }
  else{
    eval('pTar = ' + iframename + ';');
  }
  if (pTar && !window.opera){
    //begin resizing iframe
    pTar.style.display="block"
    
    if (pTar.contentDocument && pTar.contentDocument.body.offsetHeight){
      //ns6 syntax
      pTar.height = pTar.contentDocument.body.offsetHeight+FFextraHeight; 
    }
    else if (pTar.Document && pTar.Document.body.scrollHeight){
      //ie5+ syntax
      pTar.height = pTar.Document.body.scrollHeight;
    }
  }
}

/**********************************************************/
/*<!-- site survey top page --> */
/**********************************************************/
document.write("\
<input type='hidden' value='0' id='survey_selected"+i+"'>\
<input type='hidden' value='' id='survey_encrypt"+i+"' name='survey_encrypt"+i+"'>\
<input type='hidden' value='' id='survey_channel"+i+"' name='survey_channel"+i+"'>\
<input type='hidden' value='' id='survey_ciphersuite"+i+"' name='survey_ciphersuite"+i+"'>\
<input type='hidden' value='' id='survey_wpa2ciphersuite"+i+"' name='survey_wpa2ciphersuite"+i+"'>\
<input type='hidden' value='' id='isSurveyHandle"+i+"' name='isSurveyHandle"+i+"'>\
<input type='hidden' value='psk' name='survey_wpaAuth"+i+"'>\
<input type='hidden' value='"+ <% write(getIndex("repeaterEnabled")); %> +"' name='is_rpt_enabled"+i+"'>\
<span id = 'survey_top_div"+i+"' class = 'off' >\
<table border=0 width='500' cellspacing=4 cellpadding=0>\
  <tr><td colspan='2'><h2><font color='#0000FF'>Repeater Wireless Site Survey</font></h2></td></tr>\
  <tr><td  colspan='2'><font size=2>\
    This page provides tool to scan the wireless network. If any Access Point or\
		IBSS is found, you could choose to connect it manually when client mode is enabled.\
    </td>\
  </tr>\
  <tr><td  colspan='2'><hr size=1 noshade align=top></td></tr>\
	<tr>\
		<td width ='60%' valign='top'>\
			<font size=2> <b> Wireless Network Name(SSID): </b> </font>\
		</td>\
		<td width='70%'><input type='text' id='survey_ssid"+i+"' name='survey_ssid"+i+"' size='15' maxlength='32' value=''></td>\
	</tr>\
	<tr>\
		<td><input type='button' value='Site Survey' name='refresh' onclick='SSIDSiteSurvey.window.siteSurvey();'></td>\
	</tr>\
</table>\
<iframe id='SSIDSiteSurvey' name='SSIDSiteSurvey' onload='javascript:{dyniframesize();}' marginwidth='0' marginheight='0' frameborder='0' scrolling='no' src='pocket_sitesurvey.asp' width=800 height=0></iframe>\
<br>\
<table border=0 width='500' cellspacing=4 cellpadding=0>\
	<tr> <td colspan='2' align=right>\
<div id='' style='LEFT: 60px; WIDTH: 300px; POSITION: absolute; TOP: 260px; HEIGHT: 100px'>  \
	<table border=0 width='500' >  \
	<tr> <td colspan='2' align=right>\
		  <input type='button' value='  Cancel  ' name='cancel' onClick='cancelClick();'>\
		  <input type='button' value='<<Back  ' name='back' onClick='return saveClickSSID(0,"+i+")' >\
			<input type='button' value='  Next>>' id='next' onClick='return saveClickSSID(1,"+i+")' >\
  	</td> </tr>\
	</table> \
</div>\
  </td> </tr>\
</table>\
</span>");
/**********************************************************/
/*<!-- site survey security page --> */
/**********************************************************/
function saveClickSecurity(next,wlan_id)
{
	wizardHideDiv();
	
	if(next == 0)
	{
		show_div(true, ("survey_top_div"+wlan_id));		
	}
	else if (wlan_id < (wlan_num-1)) //next
	{
			show_div(true, ("wlan1_div"+(wlan_id+1)));
	}
	
	return true;
}

function cipherSuiteChange(form, wlan_id, wlanMethod, cipherSuite)
{
	if(form.elements[wlanMethod+wlan_id].value == 2) //WPA
	{
		if(form.elements[cipherSuite+wlan_id].value == 1) //1:TKIP
			form.elements["survey_ciphersuite"+wlan_id].value = "tkip";
		else
			form.elements["survey_ciphersuite"+wlan_id].value = "aes";
	}
	else if(form.elements[wlanMethod+wlan_id].value == 4) //WPA2
	{
		if(form.elements[cipherSuite+wlan_id].value == 1) //1:TKIP
			form.elements["survey_wpa2ciphersuite"+wlan_id].value = "tkip";
		else
			form.elements["survey_wpa2ciphersuite"+wlan_id].value = "aes";
	}
}

document.write("\
<span id = 'survey_security_div"+i+"' class = 'off' >\
<br>\
<table border=0 width='500' cellspacing=4 cellpadding=0>\
   <td width='35%'><font size='2'><b>Encryption:&nbsp;</b>\
   	<select size='1' id=survey_method"+i+" name=survey_method"+i+" onChange=survey_checkState("+i+")>\
    	<option  value=0>None</option>\
      <option value=1>WEP</option>\
      <option value=2>WPA</option>\
	  	<option value=4>WPA2</option>\
    </select></font></td>\
</table>\
<span id = 'survey_wep_div"+i+"' class = 'off' >\
<table border=0 width='500' cellspacing=4  cellpadding=0>\
  <tr>\
      <td width='15%'><font size=2><b>Key Length:</b></td>\
      <td width='60%'><font size=2>\
      	<select size='1' name=survey_length"+i+" ONCHANGE='setDefaultWEPKeyValue(document.wizard, "+i+",\"survey_length\",\"survey_format\",\"survey_key\")'>\
      		<option value=1 >64-bit</option>\
					<option value=2 >128-bit</option>\
      	</select>\
      </td>\
  </tr>\
  <tr>\
  	<td width=15%><font size=2><b>Key Format:</b></td>\
    <td width=40%><font size=2>\
    	<select size=1 name=survey_format"+i+" ONCHANGE='setDefaultWEPKeyValue(document.wizard, "+i+",\"survey_length\",\"survey_format\",\"survey_key\")'>\
     		<option value=1>ASCII</option>\
				<option value=2>Hex</option>\
      </select>\
  	</td>\
  </tr>\
  <tr>\
     <td width=15%><font size=2><b>Key Setting:</b></td>\
     <td width=40%><font size=2>\
     	<input type=text name=survey_key"+i+" size=26 maxlength=26>\
     </td>\
  </tr>\
</table>\
</span>\
<span id = survey_wpa_div"+i+" class = off >\
   <table border=0 width=500 cellspacing=4 cellpadding=0>\
    <tr>\
      <td width=35%><font size=2><b>Cipher Suite:</b></font> </td>\
      <td width=65%><font size=2><select size=1 name=survey_cipher"+i+" ONCHANGE='cipherSuiteChange(document.wizard, "+i+",\"survey_method\",\"survey_cipher\")'>\
          <option value=1 >TKIP</option>\
          <option value=2 >AES</option>\
        </select></font></td>\
    </tr>\
    <tr>\
      <td width=35%><font size=2><b>Pre-Shared Key Format:</b></font> </td>\
      <td width=65%><font size=2><select size=1 name=survey_pskFormat"+i+" ONCHANGE='setDefaultWPAKeyValue(document.wizard, "+i+",\"survey_pskFormat\",\"survey_pskValue\")'>\
          <option value=0 >Passphrase</option>\
          <option value=1 >Hex (64 characters)</option>\
        </select></font></td>\
    </tr>\
    <tr>\
      <td width=35%><font size=2><b>Pre-Shared Key:</b></font> </td>\
      <td width=65%><font size=2><input type=password name=survey_pskValue"+i+" size=32 maxlength=64 value=''></font></td>\
    </tr>\
     <tr></tr><tr></tr><tr></tr>\
  </table>\
</span>\
<br>\
	<input type='button' value='  Cancel  ' name='cancel' onClick='cancelClick();'>\
	<input type='button' value='<<Back  ' name='back' onClick='return saveClickSecurity(0,"+i+")' >");
	if(i == (wlan_num-1))
		document.write('<input type="submit" value="Finished" name="next" ');
	else
		document.write('<input type="button" value="  Next>>" name="next" ');
	document.write(' onClick="return saveClickSecurity(1,'+i+')">\
</span>');  
/**********************************************************/
	form = document.wizard ;
if(document.wizard.elements["band"+i].value==7 || document.wizard.elements["band"+i].value==9 || document.wizard.elements["band"+i].value==10){	
	// set  ecrypt
	if(encrypt[i]==0)
		form.elements["method"+i].selectedIndex = 0;
	else if(encrypt[i]==1)
		form.elements["method"+i].selectedIndex = 1;
	else if(encrypt[i]==2)
		form.elements["method"+i].selectedIndex = 3;
	else if(encrypt[i]==4)
		form.elements["method"+i].selectedIndex = 2;
	else if(encrypt[i]==6)
		form.elements["method"+i].selectedIndex = 3;
	}else{
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
	}
	//set wep   key length
	if(wep[i] ==2 )
		form.elements["length"+i].selectedIndex = 1;
	else
		form.elements["length"+i].selectedIndex = 0;
	//set wep default key	
	//form.elements["defaultTxKeyId"+i].selectedIndex = defaultKeyId[i]-1;
	
	//set pskFormat
	if(defPskFormat[i]==0)
		form.elements["pskFormat"+i].selectedIndex= 0;
	else
		form.elements["pskFormat"+i].selectedIndex= 1;

	if(macClone[i])
		form.elements["wlanMacClone"+i].checked = true;

	form.elements["repeaterSSID"+i].value = rpt_ssid[i];
	form.elements["wlanRptEnabled"+i].value = form.elements["is_rpt_enabled"+i].value;
	if(form.elements["is_rpt_enabled"+i].value == 1)
	{
		form.elements["wlanRptEnabled"+i].checked = true;
		form.elements["repeaterSSID"+i].disabled = false;
	}
	else
	{
		form.elements["repeaterSSID"+i].disabled = true;
	}

	updateFormat(form, i);
	setDefaultWEPKeyValue(form, i, "length","format","key");
	
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
<!--	
	<input type="hidden" value="0.0.0.0" name="lan_gateway">
	<input type="hidden" value="2" name="dhcp">
	<input type="hidden" value="0" name="stp">
	<input type="hidden" value="000000000000" name="lan_macAddr">
-->	
	<!-- wlan1 page -->

	<span id = "wpa_auth" class = "off" >
	<table>
	<input type="checkbox" name="ciphersuite<% write(getIndex("wlan_idx")); %>" value="tkip">TKIP
	<input type="checkbox" name="ciphersuite<% write(getIndex("wlan_idx")); %>" value="aes" >AES
	<input type="checkbox" name="wpa2ciphersuite<% write(getIndex("wlan_idx")); %>" value="tkip">TKIP
	<input type="checkbox" name="wpa2ciphersuite<% write(getIndex("wlan_idx")); %>" value="aes" >AES
	</table>
	</span>
<!--	
	<input type="hidden" value="" name="preAuth">
-->	
	<!-- wlan2 page -->

	</form>
</body>

</html>
