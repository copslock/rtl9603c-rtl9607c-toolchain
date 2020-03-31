<html>
<! Copyright (c) Realtek Semiconductor Corp., 2003. All Rights Reserved. ->
<head>
<meta http-equiv="Content-Type" content="text/html">
<title>WLAN Basic Settings</title>
<script type="text/javascript" src="util_gw.js"> </script>
<style>
.on {display:on}
.off {display:none}
</style>
<SCRIPT>
var wlan_channel=new Array();
var wlan_txrate=new Array();
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
var networkType=new Array();
var wlan_idx= <% write(getIndex("wlan_idx")); %> ;
var opmode=<% write(getIndex("opMode")); %> ;
var WiFiTest=<% write(getIndex("WiFiTest")); %> ;
lastBand[wlan_idx] = 0;
bandIdxAP[wlan_idx] = -1;
bandIdxClient[wlan_idx] = -1;
startChanIdx[wlan_idx] = 0;
disableSSID[wlan_idx] = 0;
networkType[wlan_idx] = <% write(getIndex("networkType"));%>;
var enc_method=new Array();
enc_method[wlan_idx] = <%write(getIndex("encrypt"));%>;

// for WPS ---------------------------------------->>
var wps_disabled=<% write(getIndex("wscDisable"));%>;
var wps_ssid_old='<% getInfo("ssid"); %>';
var wps_mode_old=<% write(getIndex("wlanMode")); %>;
var wps_config_by_registrar=<% write(getIndex("wps_by_reg")); %>;
var wps_encrypt_old=<% write(getIndex("encrypt"));%>;
var wps_enable1x=<% write(getIndex("enable1X"));%>;
var wps_wpa_auth=<% write(getIndex("wpaAuth"));%>;
// <<-------------------------------------- for WPS

function showMacClick(form, url)
{
var mode_idx = form.elements["mode"+wlan_idx].selectedIndex;
var mode_value = form.elements["mode"+wlan_idx].options[mode_idx].value; 
  if (!form.elements["wlanDisabled"+wlan_idx].checked 
  // && (mode_value==0 || mode_value==3))
  && (mode_value==0 || mode_value==3|| mode_value==4|| mode_value==6))
	openWindow(url, 'showWirelessClient', 800, 500 );
}

function showMultipleAP(form, url)
{	
var mode_idx = form.elements["mode"+wlan_idx].selectedIndex;
var mode_value = form.elements["mode"+wlan_idx].options[mode_idx].value; 
  if (!form.elements["wlanDisabled"+wlan_idx].checked 
  // 802.11s Mesh NOTICE: If Mesh work with Multiple AP, Add mode_value == 4 (AP+MPP) and mode_value == 6 (MAP)
  && (mode_value==0 || mode_value==3 || mode_value==4 || mode_value==6 ))
	//openWindow(url, 'MultipleAP', 800, 380 );
	document.location.href = url;
}

function updateState(form, wlan_id)
{
  updateIputState(form, wlan_id);
  updateRepeaterState(form, wlan_id);
}

function updateRepeaterState(form, wlan_id)
{   
  if(!form.elements["wlanDisabled"+wlan_id].checked &&  	
     ((form.elements["mode"+wlan_id].selectedIndex!=1) ||
       ((form.elements["mode"+wlan_id].selectedIndex==1) &&
     	(form.elements["type"+wlan_id].selectedIndex==0))) &&
     (form.elements["mode"+wlan_id].selectedIndex!=2) 
     ){     	
	enableCheckBox(form.elements["repeaterEnabled"+wlan_id]);
	if (form.elements["repeaterEnabled"+wlan_id].checked)
 		enableTextField(form.elements["repeaterSSID"+wlan_id]);
  	else
  		disableTextField(form.elements["repeaterSSID"+wlan_id]);
  }
  else {
	disableCheckBox(form.elements["repeaterEnabled"+wlan_id]);
	disableTextField(form.elements["repeaterSSID"+wlan_id]);
  }
// mesh check
if(form.elements["mode"+wlan_id].selectedIndex>3)
{
  disableCheckBox(form.elements["repeaterEnabled"+wlan_id]);
  disableTextField(form.elements["repeaterSSID"+wlan_id]);
}
}

function updateMode_basic(form, wlan_id)
{
	var mode_selected=0;
	var Type_selected=0;
	updateRepeaterState(form, wlan_id);
	updateMode(form, wlan_id);
	Type_selected = document.wlanSetup.elements["type"+wlan_id].selectedIndex;
  	mode_selected=document.wlanSetup.elements["mode"+wlan_id].selectedIndex;
  	var chan_number_idx=form.elements["chan"+wlan_id].selectedIndex;
	var chan_number= form.elements["chan"+wlan_id].options[chan_number_idx].value;	
  	//if client and infrastructure mode
  	if(mode_selected ==1){
		if(Type_selected == 0){
			disableTextField(document.wlanSetup.elements["controlsideband"+wlan_id]);
			disableTextField(document.wlanSetup.elements["channelbound"+wlan_id]);
		}else{
			enableTextField(document.wlanSetup.elements["channelbound"+wlan_id]);
			index_channelbound=document.wlanSetup.elements["channelbound"+wlan_id].selectedIndex;
		if(index_channelbound ==0)
			disableTextField(document.wlanSetup.elements["controlsideband"+wlan_id]);	
		else{
			if(chan_number != 0)
				enableTextField(document.wlanSetup.elements["controlsideband"+wlan_id]);
			else
				disableTextField(document.wlanSetup.elements["controlsideband"+wlan_id]);
		}
		}
	}else{
		enableTextField(document.wlanSetup.elements["channelbound"+wlan_id]);
			index_channelbound=document.wlanSetup.elements["channelbound"+wlan_id].selectedIndex;
		if(index_channelbound ==0)
			disableTextField(document.wlanSetup.elements["controlsideband"+wlan_id]);	
		else{
			if(chan_number != 0)
				enableTextField(document.wlanSetup.elements["controlsideband"+wlan_id]);
	 		else
				disableTextField(document.wlanSetup.elements["controlsideband"+wlan_id]);
		}
}
	
	if( mode_selected == 5 )	//6 should be MESH mode
		disableTextField(document.wlanSetup.elements["ssid"+<% write(getIndex("wlan_idx")); %>]);

}

function updateType_basic(form, wlan_id)
{
	updateRepeaterState(form, wlan_id);
	updateType(form, wlan_id);
	
}
function checkTurboState()
{
}

function update_controlsideband(form, wlan_id)
{
	var index=document.wlanSetup.elements["channelbound"+<% write(getIndex("wlan_idx")); %>].selectedIndex;
	
	if(index ==0)
		disableTextField(document.wlanSetup.elements["controlsideband"+<% write(getIndex("wlan_idx")); %>]);	
	else
		enableTextField(document.wlanSetup.elements["controlsideband"+<% write(getIndex("wlan_idx")); %>]);
	updateChan_channebound(form, wlan_id);	
	var chan_number_idx=form.elements["chan"+wlan_idx].selectedIndex;	
	if(chan_number_idx==0)
		disableTextField(document.wlanSetup.elements["controlsideband"+<% write(getIndex("wlan_idx")); %>]);	

}

 
function Set_onChangeBand(form, wlan_id, band, index){
	    var band;
	    var auto;
 	    var txrate;
	var value;
	
	var checkid_wmm1 = document.getElementById("wlan_wmm");
	var checkid_wmm2 = document.wlanSetup.wlanwmm<% write(getIndex("wlan_idx")); %>;
	 var checkid_bound=document.getElementById("channel_bounding");
	 var checkid_sideband = document.getElementById("control_sideband");
	 var wmm_value = <% write(getIndex("wmmEnabled")); %>;
	 var mode_selected=0;
	var Type_selected=0;
	var index_channelbound=0;
	if(wmm_value==0)
		document.wlanSetup.elements["wlanwmm"+<% write(getIndex("wlan_idx")); %>].selectedIndex=0;
	else
		document.wlanSetup.elements["wlanwmm"+<% write(getIndex("wlan_idx")); %>].selectedIndex=1;	
	value =band.options[index].value;
	if(value ==9 || value ==10 || value ==7){
		checkid_bound.style.display = "";
		checkid_sideband.style.display = "";
		document.wlanSetup.elements["wlanwmm"+<% write(getIndex("wlan_idx")); %>].selectedIndex = 1;
	 	//checkid_wmm1.disabled = true;
	 	checkid_wmm2.disabled = true;
	}else{
		checkid_bound.style.display = "none";
		checkid_sideband.style.display = "none";
		//checkid_wmm1.disabled = false;
		checkid_wmm2.disabled = false;
	}
	
	
	var txrate_idx=form.elements["txRate"+wlan_idx].selectedIndex;
	var wlan_txrate_value= form.elements["txRate"+wlan_idx].options[txrate_idx].value;
	wlan_txrate[wlan_idx] = wlan_txrate_value;
	document.wlanSetup.elements["txRate"+<% write(getIndex("wlan_idx")); %>].length=0;
	showtxrate_updated(document.wlanSetup, value, wlan_idx, <% write(getIndex("rf_used")); %>);	
	
	updateChan_channebound(form, wlan_id);
	Type_selected = document.wlanSetup.elements["type"+wlan_id].selectedIndex;
  	mode_selected=document.wlanSetup.elements["mode"+wlan_id].selectedIndex;
  	//if client and infrastructure mode
  	if(mode_selected ==1){
		if(Type_selected == 0){
			disableTextField(document.wlanSetup.elements["controlsideband"+wlan_id]);
			disableTextField(document.wlanSetup.elements["channelbound"+wlan_id]);
		}else{
			enableTextField(document.wlanSetup.elements["channelbound"+wlan_id]);
			index_channelbound=document.wlanSetup.elements["channelbound"+wlan_id].selectedIndex;
		if(index_channelbound ==0)
			disableTextField(document.wlanSetup.elements["controlsideband"+wlan_id]);	
		else
			enableTextField(document.wlanSetup.elements["controlsideband"+wlan_id]);
		}
	}else{
		enableTextField(document.wlanSetup.elements["channelbound"+wlan_id]);
			index_channelbound=document.wlanSetup.elements["channelbound"+wlan_id].selectedIndex;
		if(index_channelbound ==0)
			disableTextField(document.wlanSetup.elements["controlsideband"+wlan_id]);	
		else
			enableTextField(document.wlanSetup.elements["controlsideband"+wlan_id]);
	}	
/*	
	if( value ==9 || value ==10){
		if(enc_method[wlan_idx]==1){
			alert(encrypt_basic);
		}else if(enc_method[wlan_idx]==2){
			var wpaCipher = <% write(getIndex("wpaCipher"));%>;
			if(wpaCipher ==1)
				alert(encrypt_basic);
		}else if(enc_method[wlan_idx]==4){
			var wpa2Cipher = <% write(getIndex("wpa2Cipher"));%>;
				if(wpa2Cipher ==1)
					alert(encrypt_basic);
		}else if(enc_method[wlan_idx]==6){
			var wpaCipher = <% write(getIndex("wpaCipher"));%>;
			var wpa2Cipher = <% write(getIndex("wpa2Cipher"));%>;
			if(wpa2Cipher ==1 || wpaCipher ==1)
				alert(encrypt_basic);
		}
	}	
*/	
	var chan_number_idx=form.elements["chan"+wlan_id].selectedIndex;
	var chan_number= form.elements["chan"+wlan_id].options[chan_number_idx].value;	
	if(chan_number == 0)
		disableTextField(document.wlanSetup.elements["controlsideband"+wlan_id]);	
	else{
		if(document.wlanSetup.elements["channelbound"+<% write(getIndex("wlan_idx")); %>].selectedIndex == "0")
 			disableTextField(document.wlanSetup.elements["controlsideband"+<% write(getIndex("wlan_idx")); %>]);	
 		else
			enableTextField(document.wlanSetup.elements["controlsideband"+<% write(getIndex("wlan_idx")); %>]);		
	}
}
function updateChan_selectedIndex(form, wlan_id)
{
	var chan_number_idx=form.elements["chan"+wlan_id].selectedIndex;
	var chan_number= form.elements["chan"+wlan_id].options[chan_number_idx].value;
	
	wlan_channel[wlan_id] = chan_number;
	if(chan_number == 0)
		disableTextField(document.wlanSetup.elements["controlsideband"+wlan_id]);	
	else{
		if(document.wlanSetup.elements["channelbound"+<% write(getIndex("wlan_idx")); %>].selectedIndex == "0")
 			disableTextField(document.wlanSetup.elements["controlsideband"+<% write(getIndex("wlan_idx")); %>]);	
 		else
			enableTextField(document.wlanSetup.elements["controlsideband"+<% write(getIndex("wlan_idx")); %>]);		
	}
}
function LoadSetting()
 {
 	var form = document.wlanSetup;
 	var wlanband="<% if (getIndex("band") < 7) write("0"); %>"; 
 	var checkid_bound;
 	var checkid_sideband;
 	var checkid_wmm1 = document.getElementById("wlan_wmm");
 	var checkid_wmm2 = document.wlanSetup.wlanwmm<% write(getIndex("wlan_idx")); %>;
 	var mode_index = document.wlanSetup.elements["mode"+<% write(getIndex("wlan_idx")); %>].selectedIndex;
 	var nettype_index = document.wlanSetup.elements["type"+<% write(getIndex("wlan_idx")); %>].selectedIndex;
 	checkid_bound=document.getElementById("channel_bounding");
 	checkid_sideband=document.getElementById("control_sideband");
 	var wmm_value = <% write(getIndex("wmmEnabled")); %>;
	if(wmm_value==0)
		document.wlanSetup.elements["wlanwmm"+<% write(getIndex("wlan_idx")); %>].selectedIndex=0;
	else
		document.wlanSetup.elements["wlanwmm"+<% write(getIndex("wlan_idx")); %>].selectedIndex=1;	
 	if(wlanband == "0"){
 		checkid_bound.style.display = "none";
 		checkid_sideband.style.display = "none";
 		//checkid_wmm1.disabled = false;
	 	checkid_wmm2.disabled = false;
 	}else{
 		checkid_bound.style.display = "";
 		checkid_sideband.style.display = "";
 		document.wlanSetup.elements["wlanwmm"+<% write(getIndex("wlan_idx")); %>].selectedIndex = 1;
	 	//checkid_wmm1.disabled = true;
	 	checkid_wmm2.disabled = true;
 	}
 
 	var init_bound = "<% if (getIndex("ChannelBonding")==0) write("0"); %>";
 	var init_sideband="<% if (getIndex("ControlSideBand")==0) write("0"); %>";
 
 	if(init_bound=="0")
 		document.wlanSetup.elements["channelbound"+<% write(getIndex("wlan_idx")); %>].selectedIndex=0;
 	else
 		document.wlanSetup.elements["channelbound"+<% write(getIndex("wlan_idx")); %>].selectedIndex=1;
 		
 	if(init_sideband=="0")
 		document.wlanSetup.elements["controlsideband"+<% write(getIndex("wlan_idx")); %>].selectedIndex=0;
 	else
 		document.wlanSetup.elements["controlsideband"+<% write(getIndex("wlan_idx")); %>].selectedIndex=1;	
 	if(init_bound == "0")
 		disableTextField(document.wlanSetup.elements["controlsideband"+<% write(getIndex("wlan_idx")); %>]);	
 	else
		enableTextField(document.wlanSetup.elements["controlsideband"+<% write(getIndex("wlan_idx")); %>]);		
		
		//if client and infrastructure mode
	if(mode_index==1){
	 	if(nettype_index ==0){
	 		disableTextField(document.wlanSetup.elements["controlsideband"+<% write(getIndex("wlan_idx")); %>]);
			disableTextField(document.wlanSetup.elements["channelbound"+<% write(getIndex("wlan_idx")); %>]);
	 	}
	}

	if( mode_index == 5 )
		disableTextField(document.wlanSetup.elements["ssid"+<% write(getIndex("wlan_idx")); %>]);
		
	var hiddenSSID_value = <% write(getIndex("hiddenSSID")); %>;
	if(hiddenSSID_value==0)
		document.wlanSetup.elements["hiddenSSID"+<% write(getIndex("wlan_idx")); %>].selectedIndex=1;
	else
		document.wlanSetup.elements["hiddenSSID"+<% write(getIndex("wlan_idx")); %>].selectedIndex=0;
	updateChan_channebound(document.wlanSetup, wlan_idx);	
/*		
	if( wlanband != "0"){
		if(enc_method[wlan_idx]==1){
			alert(encrypt_basic);
		}else if(enc_method[wlan_idx]==2){
			var wpaCipher = <% write(getIndex("wpaCipher"));%>;
			if(wpaCipher ==1)
				alert(encrypt_basic);
		}else if(enc_method[wlan_idx]==4){
			var wpa2Cipher = <% write(getIndex("wpa2Cipher"));%>;
				if(wpa2Cipher ==1)
					alert(encrypt_basic);
		}else if(enc_method[wlan_idx]==6){
			var wpaCipher = <% write(getIndex("wpaCipher"));%>;
			var wpa2Cipher = <% write(getIndex("wpa2Cipher"));%>;
			if(wpa2Cipher ==1 || wpaCipher ==1)
				alert(encrypt_basic);
		}
	}
*/		
	var chan_number_idx=form.elements["chan"+wlan_idx].selectedIndex;
	var chan_number= form.elements["chan"+wlan_idx].options[chan_number_idx].value;
	
	wlan_channel[wlan_idx] = chan_number;
	
	var txrate_idx=form.elements["txRate"+wlan_idx].selectedIndex;
	var wlan_txrate_value= form.elements["txRate"+wlan_idx].options[txrate_idx].value;
	wlan_txrate[wlan_idx] = wlan_txrate_value;
	if(chan_number == 0)
		disableTextField(document.wlanSetup.elements["controlsideband"+<% write(getIndex("wlan_idx")); %>]);	

	var isRepeaterDisplay =  <% write(getIndex("isRepeaterDisplay")); %> ;
	if(isRepeaterDisplay == 0)
		disableCheckBox(document.wlanSetup.elements["repeaterEnabled"+wlan_idx]);			
		
	var isPocketRouter="<% getInfo("isPocketRouter"); %>"*1;
	var pocketRouter_Mode="<% getInfo("pocketRouter_Mode"); %>"*1;
	if(pocketRouter_Mode == 1) //1:bridge and client mode
		disableTextField(document.wlanSetup.elements["mode"+wlan_idx]);
		
 }

 	function resetForm()
 	{
 		//history.go(0);
		//location.reload(); 
		location=location; 
		//location.assign(location); 
		//document.execCommand('Refresh'); 
		//window.navigate(location); 
		//location.replace(location); 
		//document.URL=location.href; 
	}


</SCRIPT>
</head>

<body onload="LoadSetting();">
<blockquote>
<h2><font color="#0000FF">Wireless Basic Settings <% if (getIndex("wlan_num")>1) write("-wlan"+(getIndex("wlan_idx")+1)); 
%></font></h2>

<form action=/goform/formWlanSetup method=POST name="wlanSetup">
<!-- for WPS -->
<INPUT type=hidden name=wps_clear_configure_by_reg<% write(getIndex("wlan_idx")); %> value=0>
<table border=0 width="500" cellspacing=4>
  <tr><font size=2>
 This page is used to configure the parameters for wireless LAN clients which
 may connect to your Access Point. Here you may change wireless encryption settings
 as well as wireless network parameters.
  </tr>
  <tr><hr size=1 noshade align=top></tr>
  <tr>
      <td width="100%" colspan=2><font size=2><b>
   	<input type="checkbox" name="wlanDisabled<% write(getIndex("wlan_idx")); %>" value="ON" <% if (getIndex("wlanDisabled")) write("checked");
   	%> ONCLICK="updateState(document.wlanSetup, wlan_idx)">&nbsp;&nbsp;Disable Wireless LAN Interface</b>
      </td>
  </tr>

  <tr>
      <td width="26%"><font size=2><b>Band:</b></td>
      <td width="74%"><font size=2><select size="1" name="band<% write(getIndex("wlan_idx")); %>" onChange="updateBand(document.wlanSetup, wlan_idx);Set_onChangeBand(document.wlanSetup, wlan_idx, document.wlanSetup.band<% write(getIndex("wlan_idx")); %>, document.wlanSetup.band<% write(getIndex("wlan_idx")); %>.selectedIndex);">

     <SCRIPT>
 	<%  val = getIndex("RFType");
	    write ("RFType[wlan_idx]=" + val + ";\n");
	    val = getIndex("wlanMode");
	    write ("\tAPMode[wlan_idx]=" + val + ";\n");
   	    val = getIndex("band");
	    if (val > 0) val = val-1;
    	    write ("\tbandIdx[wlan_idx]=" + val + ";\n");
  	%>
//	if (APMode[wlan_idx]!=1)
		bandIdxAP[wlan_idx]=bandIdx[wlan_idx];
//	else
		bandIdxClient[wlan_idx]=bandIdx[wlan_idx];
	showBand(document.wlanSetup, wlan_idx);
     </SCRIPT>

	 </select>
      </td>
  </tr>
  <tr>
      <td width="26%"><font size=2><b>Mode:</b></td>
      <td width="74%"><font size=2><select size="1" name="mode<% write(getIndex("wlan_idx")); %>"	onChange="updateMode_basic(document.wlanSetup, wlan_idx)">
	<%  getModeCombobox(); %>   
	 </select>

	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<input type="button" value="Multiple AP" name="multipleAP<% write(getIndex("wlan_idx")); %>" onClick="showMultipleAP(document.wlanSetup, '/wlmultipleap.asp')">
	  
      </td>
  </tr>
  <tr>
      <td width="26%"><font size=2><b>Network Type:</b></td>
      <td width="74%"><font size=2><select size="1" name="type<% write(getIndex("wlan_idx")); %>"	onChange="updateType_basic(document.wlanSetup, wlan_idx)">
	<%  val = getIndex("networkType");
   	  if ( val == 0 ) {
      	  	write( "<option selected value=\"0\">Infrastructure </option>" );
   	  	write( "<option value=\"1\">Ad hoc</option>" );
      	  }

	  if ( val == 1 ) {
     	  	write( "<option value=\"0\">Infrastructure </option>" );
   	  	write( "<option selected value=\"1\">Ad hoc</option>" );
      	  }

      	  %>
	 </select>
      </td>
  </tr>
  <tr>
      <td width="26%"><font size=2><b>SSID:
      </b></td>
      <td width="74%"><font size=2><input type="text" name="ssid<% write(getIndex("wlan_idx")); %>" size="33" maxlength="32" value="<% getInfo("ssid"); %>">
      </td>
  </tr>
      <tr id="channel_bounding" style="display:none">
      <td width="26%"><font size=2><b>Channel Width:</b></td>
      <td width="74%"><font size=2><select size="1" name="channelbound<% write(getIndex("wlan_idx")); %>" onChange="update_controlsideband(document.wlanSetup, wlan_idx)">
     <option value="0">20MHz</option>
	<option value="1">40MHz</option>
	 </select>
      </td>
  </tr>
  <tr id="control_sideband" style="display:none">
      <td width="26%"><font size=2><b>Control Sideband:</b></td>
      <td width="74%"><font size=2><select size="1" name="controlsideband<% write(getIndex("wlan_idx")); %>" onChange="updateChan_channebound(document.wlanSetup, wlan_idx);">
     <option value="0">Upper</option>
	<option value="1">Lower</option>
	 </select>
      </td>
  </tr>
  <tr>
      <td width="26%"><font size=2><b>Channel Number:</b></td>
      <td width="74%"><font size=2><select size="1" name="chan<% write(getIndex("wlan_idx")); %>" onChange="updateChan_selectedIndex(document.wlanSetup, wlan_idx);"> </select></td>
    <SCRIPT>
 	<%  regDomain = getIndex("regDomain");
	    defaultChan = getIndex("channel");
	    write ("regDomain[wlan_idx]=" + regDomain + ";\n");
  	    write ("\tdefaultChan[wlan_idx]=" + defaultChan + ";\n");
	%>
	wlan_channel[wlan_idx] = defaultChan[wlan_idx];
	updateChan(document.wlanSetup, wlan_idx);
    </SCRIPT>
  </tr>
<tr>
      <td width="26%"><font size=2><b>Broadcast SSID:</b></td>
      <td width="74%"><font size=2><select size="1" name="hiddenSSID<% write(getIndex("wlan_idx")); %>"	onChange="">
      	  	<option value="1">Disabled</option>
   	  	<option value="0">Enabled</option>
	 </select>
      </td>
  </tr>
  
  <tr id="wlan_wmm" style="display:">
      <td width="26%"><font size=2><b>WMM:</b></td>
      <td width="74%"><font size=2><select size="1" name="wlanwmm<% write(getIndex("wlan_idx")); %>"	onChange="">
      	  	<option value="0">Disabled</option>
   	  	<option value="1">Enabled</option>
	 </select>
      </td>
  </tr>
  
  
  <tr>
      <td width="26%"><font size=2><b>Data Rate:</b></td>
      <td width="74%"><font size=2><select size="1" name="txRate<% write(getIndex("wlan_idx")); %>" onChange="checkTurboState()"> 
	 <SCRIPT>
	    <%
	    band = getIndex("band");
	    auto=getIndex("rateAdaptiveEnabled")
 	    txrate = getIndex("fixTxRate");
     	    rf_num = getIndex("rf_used");
	    write ("band=" + band + "\n");
 	    write ("txrate=" + txrate + "\n");
 	    write ("auto=" + auto + "\n");
 	    write ("rf_num=" + rf_num + "\n");		
	%>
	var rate_mask = [15,1,1,1,1,2,2,2,2,2,2,2,2,4,4,4,4,4,4,4,4,8,8,8,8,8,8,8,8];
	var rate_name=["Auto","1M","2M","5.5M","11M","6M","9M","12M","18M","24M","36M","48M","54M", "MCS0", "MCS1",
		"MCS2", "MCS3", "MCS4", "MCS5", "MCS6", "MCS7", "MCS8", "MCS9", "MCS10", "MCS11", "MCS12", "MCS13", "MCS14", "MCS15"];
	var mask=0;
	var defidx=0;
	var idx, i, rate;
	
	if (auto)
		txrate=0;
	if (band & 1)
		mask |= 1;
	if ((band&2) || (band&4))
		mask |= 2;
	if (band & 8) {
		if (rf_num == 2)
			mask |= 12;	
		else
			mask |= 4;
	}	
	for (idx=0, i=0; i<=28; i++) {
		if (rate_mask[i] & mask) {
			if (i == 0)
				rate = 0;
			else
				rate = (1 << (i-1));
			if (txrate == rate)
				defidx = idx;
			document.write('<option value="' + i + '">' + rate_name[i] + '\n');
			idx++;
		}
	}
	document.wlanSetup.elements["txRate"+ <% write(getIndex("wlan_idx")); %>].selectedIndex=defidx;
	</SCRIPT>
	</select>
	</td>
  </tr>
  <tr>
      <td width="26%"><font size=2><b>Associated Clients:</b></td>
      <td width="74%"><font size=2><input type="button" value="Show Active Clients" name="showMac<% write(getIndex("wlan_idx")); %>" onClick="showMacClick(document.wlanSetup, '/wlstatbl.asp')">
      </td>
  </tr>
  <tr>
      <td width="100%" colspan=2><font size=2><b>
   	<input type="checkbox" name="wlanMacClone<% write(getIndex("wlan_idx")); %>" value="ON" <% if (getIndex("wlanMacClone")) write("checked");
   	%> >&nbsp;&nbsp; Enable Mac Clone (Single Ethernet Client)</b>
      </td>
  </tr>
  <tr>
      <td width="100%" colspan=2><font size=2><b>
   	<input type="checkbox" name="repeaterEnabled<% write(getIndex("wlan_idx")); %>" value="ON" <% if (getIndex("repeaterEnabled")) write("checked");
   	%> ONCLICK="updateRepeaterState(document.wlanSetup, wlan_idx)">&nbsp;&nbsp; Enable Universal Repeater Mode (Acting as AP and client simultaneouly)</b>
      </td>
  </tr>
  <tr>
      <td width="100%" colspan=2><font size=2><b>SSID of Extended Interface:&nbsp;&nbsp;
      <input type="text" name="repeaterSSID<% write(getIndex("wlan_idx")); %>" size="33" maxlength="32" value="<% getInfo("repeaterSSID"); %>">
      </td>
  </tr> 
  <script>
  if (!disableSSID[wlan_idx]) {
	document.write("</table>\n");
	document.write("<span id = \"hide_div\" class = \"off\">\n");
	document.write("<table border=\"0\" width=500>\n");
  }
  </script>

  </tr>
  <tr><td colspan="2" width="100%" height="55"><font size=2>
  <em>Note: If you want to change the setting for Mode and SSID, you must go to EasyConfig page to disable EasyConfig first.</em></font></td>
  </tr>
  <script>
  if (!disableSSID[wlan_idx]) {
	document.write("</table>\n");
	document.write("</span>\n");
	document.write("<table border=\"0\" width=500>\n");
  }
  </script>

  </table>
  </span>
  <table border="0" width=500>

  </table>
  <br>
  <table border=0 width=500 cellspacing=0 cellpadding=0>
  <tr>
     <input type="hidden" value="/wlbasic.asp" name="wlan-url">
     <input type="submit" value="Apply Changes" name="save" onClick="return saveChanges_basic(document.wlanSetup, wlan_idx)">&nbsp;&nbsp;
     <input type="button" value="Reset" name="reset" onclick="resetForm();">

     <input type="hidden" name="basicrates<% write(getIndex("wlan_idx")); %>" value=0>
     <input type="hidden" name="operrates<% write(getIndex("wlan_idx")); %>" value=0>
     <input type="hidden" id="action" name="Action" value="">
  </tr>
  <script>
   <%
    band = getIndex("band");
    write ("usedBand[wlan_idx]=" + band + ";\n");
   %>
   updateState(document.wlanSetup, wlan_idx);
	var mssid_num=<%write(getIndex("wlan_mssid_num"));%>;
	if(mssid_num == 0)
		   disableButton(document.wlanSetup.elements["multipleAP"+<% write(getIndex("wlan_idx")); %>]);
  </script>
</form>
</table>

</blockquote>
</body>

</html>
