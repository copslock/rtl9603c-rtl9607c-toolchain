/*==============================================================================*/
/*   wlbasic.asp and wizard-wlan1.asp  tcpiplan.asp*/

// for WPS ---------------------------------------------------->>
var wps_warn1='The SSID had been configured by WPS. Any change of the setting ' +
				'may cause stations to be disconnected. ' + 
				'Are you sure you want to continue with the new setting?';
var wps_warn2='AP Mode had been configured by WPS. Any change of the setting ' +
				'may cause stations to be disconnected. ' + 
				'Are you sure you want to continue with the new setting?';
var wps_warn3='The security setting had been configured by WPS. Any change of the setting ' +
				'may cause stations to be disconnected. ' + 
				'Are you sure you want to continue with the new setting?';
var wps_warn4='The WPA Enterprise Authentication cannot be supported by WPS. ' +
				'Use this configuration will cause WPS be disabled. ' + 
				'Are you sure you want to continue with the new setting?';
var wps_warn5='The 802.1x Authentication cannot be supported by WPS. ' +
				'Use this configuration will cause WPS be disabled. ' + 
				'Are you sure you want to continue with the new setting?';
var wps_warn6='WDS mode cannot be supported by WPS. ' +
				'Use this configuration will cause WPS be disabled. ' + 
				'Are you sure you want to continue with the new setting?';
var wps_warn7='Adhoc Client mode cannot be supported by WPS. ' +
				'Use this configuration will cause WPS be disabled. ' + 
				'Are you sure you want to continue with the new setting?';
var encrypt_11n = 'Invalid Encryption Mode! WPA or WPA2, Cipher suite AES should be used for 802.11n band.';
var encrypt_basic = 'The Encryption Mode is not suitable for 802.11n band, please modify wlan encrypt setting, or it will not work properly.';
var encrypt_confirm='Are you sure you want to continue with this encrypt mode for 11n band? It may not get good performance while the user is using wlan network!';
//var wps_wep_key1_old;
//var wps_wep_key2_old;
//var wps_wep_key3_old;
//var wps_wep_key4_old;
var wps_wep_key_old;

function check_wps_enc(enc, radius, auth)
{
	if (enc == 0 || enc == 1) {
		if (radius != 0)
			return 2;
	}		
	else {
		if (auth & 1)
			return 2;
	}
	return 0;
}

function check_wps_wlanmode(mo, type)
{
	if (mo == 2) {
		return 1;
	}
	if (mo == 1 && type != 0) {
		return 1;
	}
	return 0;
}
//<<----------------------------------------------- for WPS

function skip () { this.blur(); }
function disableTextField (field) {
  if (document.all || document.getElementById)
    field.disabled = true;
  else {
    field.oldOnFocus = field.onfocus;
    field.onfocus = skip;
  }
}

function enableTextField (field) {
  if (document.all || document.getElementById)
    field.disabled = false;
  else {
    field.onfocus = field.oldOnFocus;
  }
}

function verifyBrowser() {
	var ms = navigator.appVersion.indexOf("MSIE");
	ie4 = (ms>0) && (parseInt(navigator.appVersion.substring(ms+5, ms+6)) >= 4);
	var ns = navigator.appName.indexOf("Netscape");
	ns= (ns>=0) && (parseInt(navigator.appVersion.substring(0,1))>=4);
	if (ie4)
		return "ie4";
	else
		if(ns)
			return "ns";
		else
			return false;
}

function saveChanges_basic(form, wlan_id)
{
  mode =form.elements["mode"+wlan_id] ;
  ssid =form.elements["ssid"+wlan_id] ;
  if ((mode.selectedIndex==0 || mode.selectedIndex==3)&& ssid.value=="") {
	alert('SSID cannot be empty!');
	ssid.value = ssid.defaultValue;
	ssid.focus();
	return false;
   }

   if (!form.elements["wlanDisabled"+wlan_id].checked) {
	var idx_value= form.elements["band"+wlan_id].selectedIndex;
	var band_value= form.elements["band"+wlan_id].options[idx_value].value;
	var band = parseInt(band_value, 10) + 1;

	basicRate=0;
	operRate=0;
	if (band & 1) {
		basicRate|=0xf;
		operRate|=0xf;
	}
	if ( (band & 2) || (band & 4) ) {
		operRate|=0xff0;
		if (!(band & 1)) {
			if (WiFiTest)
				basicRate=0x15f;
			else
				basicRate=0x1f0;
		}			
	}
	if (band & 8) {
		if (!(band & 3))
			operRate|=0xfff;	
		if (band & 1)
			basicRate=0xf;
		else if (band & 2)			
			basicRate=0x1f0;
		else
			basicRate=0xf;
	}

	operRate|=basicRate;
	if (band && band != usedBand[wlan_id]) {
		form.elements["basicrates"+wlan_id].value = basicRate;
		form.elements["operrates"+wlan_id].value = operRate;
	}
	else {
		form.elements["basicrates"+wlan_id].value = 0;
		form.elements["operrates"+wlan_id].value = 0;
	}
   }

// for WPS -------------------------------------->>
/*
   if (check_wps_enc(wps_encrypt_old, wps_enable1x, wps_wpa_auth) > 0)
   		return true;

   type=form.elements["type"+wlan_id];
   if (wps_disabled == 0 && mode.selectedIndex == 2) {
		if (!confirm(wps_warn6)) {
			mode.selectedIndex = wps_mode_old;
			mode.focus();
			return false;
		} 
		else
			return true;
   }
   if (wps_disabled == 0 && mode.selectedIndex== 1 && type.selectedIndex == 1) {
		if (!confirm(wps_warn7)) {
			type.selectedIndex =type.defaultValue;
			type.focus();
			return false;
		} 
		else
			return true;
   }
   if (wps_disabled == 0 &&
	   	(wps_mode_old == 0 || wps_mode_old == 3) && 
	   	wps_config_by_registrar !=0) {
	 if (wps_ssid_old != ssid.value) {
		if (!confirm(wps_warn1)) {
			ssid.value = ssid.defaultValue;
			ssid.focus();
			return false;
		} 
		else
			form.elements["wps_clear_configure_by_reg"+wlan_id].value = 1;
	 }
  	 if (mode.selectedIndex != 0 && mode.selectedIndex != 3) {
		if (!confirm(wps_warn2)) {
			mode.selectedIndex = wps_mode_old;
			mode.focus();
			return false;
		} 
		else
			form.elements["wps_clear_configure_by_reg"+wlan_id].value = 1;
	 }	   	 
   }   
*/   
//<<--------------------------------------- for WPS   
   return true;
}
/*==============================================================================*/
function show_div(show,id) {

	if(show)
		document.getElementById(id).className  = "on" ;
    	else	    
    		document.getElementById(id).className  = "off" ;
}
/*   wlbasic.asp */
function enableWLAN(form, wlan_id)
{
	var idx_value= form.elements["band"+wlan_id].selectedIndex;
	var band_value= form.elements["band"+wlan_id].options[idx_value].value;
	var chan_boundIdx = form.elements["channelbound"+wlan_id].selectedIndex;
	var mode_idx = form.elements["mode"+wlan_id].selectedIndex;  
	var mode_value =form.elements["mode"+wlan_id].options[mode_idx].value; 	
	if(form.elements["multipleAP"+wlan_id] != null) { // for multiple ap
		if (mode_value == 0 || mode_value == 3)
			enableButton(form.elements["multipleAP"+wlan_id]);
		else
			disableButton(form.elements["multipleAP"+wlan_id]);
	}
	
  if (mode_value !=1) {
  	disableTextField(form.elements["type"+wlan_id]); //network type
  	if(form.elements["showMac"+wlan_id]!= null) {
  		if (mode_value !=2)
  			enableButton(form.elements["showMac"+wlan_id]);
  		else
  			disableButton(form.elements["showMac"+wlan_id]);
  	}
  	enableTextField(form.elements["chan"+wlan_id]);
  }
  else {
    	if (disableSSID[wlan_id])
  		disableTextField(form.elements["type"+wlan_id]);
  	else
   		enableTextField(form.elements["type"+wlan_id]);   	   	
    	
   	if(form.elements["showMac"+wlan_id] != null)
		disableButton(form.elements["showMac"+wlan_id]);
	if (form.elements["type"+wlan_id].selectedIndex==0) {
		disableTextField(form.elements["chan"+wlan_id]);
	}
	else {
		enableTextField(form.elements["chan"+wlan_id]);
	}

  }
  if (disableSSID[wlan_id]){
	disableTextField(form.elements["ssid"+wlan_id]);
 	disableTextField(form.elements["mode"+wlan_id]);  	
  }
  else {
  	if (mode_value !=2)
  		enableTextField(form.elements["ssid"+wlan_id]);
  	else
  		disableTextField(form.elements["ssid"+wlan_id]);
  	enableTextField(form.elements["mode"+wlan_id]); 
  }  
  enableTextField(form.elements["band"+wlan_id]);

  if(form.elements["mode"+wlan_id].selectedIndex == 1) // client mode
  	enableCheckBox(form.elements["wlanMacClone"+wlan_id]);
  else
  	disableCheckBox(form.elements["wlanMacClone"+wlan_id]);
   if(band_value == 9 || band_value ==10 || band_value ==7){
	  	enableTextField(form.elements["channelbound"+wlan_id]);
	  	if(chan_boundIdx == 1)
	  		enableTextField(form.elements["controlsideband"+wlan_id]);
	  	else
	  		 disableTextField(form.elements["controlsideband"+wlan_id]);
	 }	
  if(form ==document.wlanSetup){  	
	enableTextField(form.elements["txRate"+wlan_id]);	
	enableTextField(form.elements["hiddenSSID"+wlan_id]);	
  }	
}
function disableWLAN(form, wlan_id)
{
  disableTextField(form.elements["mode"+wlan_id]);
  disableTextField(form.elements["band"+wlan_id]);
  disableTextField(form.elements["type"+wlan_id]); 
  disableTextField(form.elements["ssid"+wlan_id]);
  disableTextField(form.elements["chan"+wlan_id]);
  
   disableTextField(form.elements["channelbound"+wlan_id]);
  disableTextField(form.elements["controlsideband"+wlan_id]);
  if(form == document.wlanSetup){  
  disableTextField(form.elements["hiddenSSID"+wlan_id]);
  disableTextField(form.elements["txRate"+wlan_id]);
   if(form.elements["multipleAP"+wlan_id]!= null)
  	disableButton(form.elements["multipleAP"+wlan_id]);
}  
  disableCheckBox(form.elements["wlanMacClone"+wlan_id]);

  if(form.elements["showMac"+wlan_id]!= null)
  	disableButton(form.elements["showMac"+wlan_id]);
}
function updateIputState(form, wlan_id)
{
  if (form.elements["wlanDisabled"+wlan_id].checked)
 	disableWLAN(form, wlan_id);
  else
  	enableWLAN(form, wlan_id);
}

function disableButton (button) {
  //if (verifyBrowser() == "ns")
  //	return;
  if (document.all || document.getElementById)
    button.disabled = true;
  else if (button) {
    button.oldOnClick = button.onclick;
    button.onclick = null;
    button.oldValue = button.value;
    button.value = 'DISABLED';
  }
}

function enableButton (button) {
  //if (verifyBrowser() == "ns")
  //	return;
  if (document.all || document.getElementById)
    button.disabled = false;
  else if (button) {
    button.onclick = button.oldOnClick;
    button.value = button.oldValue;
  }
}

function showChannel5G(form, wlan_id)
{
	form.elements["chan"+wlan_id].length=startChanIdx[wlan_id];
	if (startChanIdx[wlan_id] == 0)
		defChanIdx=0;
	else
		defChanIdx=1;

	if (startChanIdx[wlan_id]==0) {
		form.elements["chan"+wlan_id].options[0] = new Option("Auto", 0, false, false);
		if (0 == defaultChan[wlan_id]) {
			form.elements["chan"+wlan_id].selectedIndex = 0;
			defChanIdx = 0;
		}
		startChanIdx[wlan_id]++;		
	}
	if (regDomain[wlan_idx]==6) {	// MKK
		for (idx=startChanIdx[wlan_id], chan=34; chan<=64; idx++, chan+=2) {
			if ((chan==50) || (chan==54) || (chan==58) || (chan==62)) {
				idx--;
				continue;
			}
			form.elements["chan"+wlan_id].options[idx] = new Option(chan, chan, false, false);
			if (chan == defaultChan[wlan_id]) {
				form.elements["chan"+wlan_id].selectedIndex = idx;
				defChanIdx=idx;
			}
		}
	}
	else {
		for (idx=startChanIdx[wlan_id], chan=36; chan<=64; idx++, chan+=4) {
			form.elements["chan"+wlan_id].options[idx] = new Option(chan, chan, false, false);
			if (chan == defaultChan[wlan_id]) {
				form.elements["chan"+wlan_id].selectedIndex = idx;
				defChanIdx=idx;
			}
		}
		if (RFType[wlan_id] == 9) {
			// FCC, IC
			if ((regDomain[wlan_idx] == 1) || (regDomain[wlan_idx] == 2)) {
				for (chan=149; chan<=161; idx++, chan+=4) {
					form.elements["chan"+wlan_id].options[idx] = new Option(chan, chan, false, false);
					if (chan == defaultChan[wlan_id]) {
						form.elements["chan"+wlan_id].selectedIndex = idx;
						defChanIdx=idx;
					}
				}
			}
			// ETSI, SPAIN, FRANCE, ISRAEL
			if (((regDomain[wlan_idx] >= 3) && (regDomain[wlan_idx] <= 5)) || (regDomain[wlan_idx] == 7)) {
				for (chan=100; chan<=140; idx++, chan+=4) {
					form.elements["chan"+wlan_id].options[idx] = new Option(chan, chan, false, false);
					if (chan == defaultChan[wlan_id]) {
						form.elements["chan"+wlan_id].selectedIndex = idx;
						defChanIdx=idx;
					}
				}
			}
			// MKK3
			if (regDomain[wlan_idx] == 10) {
				for (chan=183; chan<=190; idx++, chan+=1) {
					form.elements["chan"+wlan_id].options[idx] = new Option(chan, chan, false, false);
					if (chan == defaultChan[wlan_id]) {
						form.elements["chan"+wlan_id].selectedIndex = idx;
						defChanIdx=idx;
					}
				}
				for (chan=192; chan<=196; idx++, chan+=2) {
					form.elements["chan"+wlan_id].options[idx] = new Option(chan, chan, false, false);
					if (chan == defaultChan[wlan_id]) {
						form.elements["chan"+wlan_id].selectedIndex = idx;
						defChanIdx=idx;
					}
				}
				for (chan=207; chan<=212; idx++, chan+=1) {
					form.elements["chan"+wlan_id].options[idx] = new Option(chan, chan, false, false);
					if (chan == defaultChan[wlan_id]) {
						form.elements["chan"+wlan_id].selectedIndex = idx;
						defChanIdx=idx;
					}
				}
				for (chan=214; chan<=216; idx++, chan+=2) {
					form.elements["chan"+wlan_id].options[idx] = new Option(chan, chan, false, false);
					if (chan == defaultChan[wlan_id]) {
						form.elements["chan"+wlan_id].selectedIndex = idx;
						defChanIdx=idx;
					}
				}
			}
		}
	}
	form.elements["chan"+wlan_id].length = idx;
	if (defChanIdx==0)
		form.elements["chan"+wlan_id].selectedIndex = 0;
}


function showChannel2G(form, wlan_id, bound_40, band)
{
	var rf_start = 1;
	var rf_end = 14;
	if (regDomain[wlan_id]==1 || regDomain[wlan_id]==2) {
		start = 1;
		end = 11;
	}
	if (regDomain[wlan_id]==3) {
		start = 1;
		end = 13;
	}
	if (regDomain[wlan_id]==4) {
		start = 10;
		end = 11;
	}
	if (regDomain[wlan_id]==5) {
		start = 10;
		end = 13;
	}
	if (regDomain[wlan_id]==6) {
		start = 1;
		end = 14;
	}
	
	if(band == 9 || band == 10 || band == 7){
		if(bound_40 ==1){
			var sideBand_idex = form.elements["controlsideband"+wlan_id].selectedIndex;
			var sideBand=form.elements["controlsideband"+wlan_id].options[sideBand_idex].value;
			if(regDomain[wlan_id]==4){
				if(sideBand ==0){  //upper
					start = 11;
					end = 11;
				}else if(sideBand ==1){ //lower
					start = 10;
					end = 10;
				}
			}else if(regDomain[wlan_id]==5){
				if(sideBand ==0){  //upper
					start = 13;
					end = 13;
				}else if(sideBand ==1){ //lower
					start = 10;
					end = 10;
				}
			}else{
				if(sideBand ==0){  //upper
					start = 5;
					if (regDomain[wlan_id]==1 || regDomain[wlan_id]==2)
						end = 11;
					else  				
						end = 13;			
					
				}else if(sideBand ==1){ //lower
					end = 7;
					start = 1;
				}
			}
		}
	}
	defChanIdx=0;
	form.elements["chan"+wlan_id].length=0;
	idx=0;
	form.elements["chan"+wlan_id].options[0] = new Option("Auto", 0, false, false);
	if(wlan_channel[wlan_id] ==0){
		form.elements["chan"+wlan_id].selectedIndex = 0;
		defChanIdx = 0;
	}
	//if (0 == defaultChan[wlan_id]) {
	//	form.elements["chan"+wlan_id].selectedIndex = 0;
	//	defChanIdx = 0;
	//}
	idx++;	
	for (chan=start; chan<=end; chan++, idx++) {
		form.elements["chan"+wlan_id].options[idx] = new Option(chan, chan, false, false);
		
		if(chan == wlan_channel[wlan_id]){
			form.elements["chan"+wlan_id].selectedIndex = idx;
			defChanIdx = idx;
		}
		//if (chan == defaultChan[wlan_id]) {
		//	form.elements["chan"+wlan_id].selectedIndex = idx;
		//	defChanIdx = idx;
		//}
	}
	form.elements["chan"+wlan_id].length=idx;
	startChanIdx[wlan_id] = idx;
	//if (defChanIdx==0)
	//	form.elements["chan"+wlan_id].selectedIndex = 0;
}
function updateChan_channebound(form, wlan_id)
{
	var idx_value= form.elements["band"+wlan_id].selectedIndex;
	var band_value= form.elements["band"+wlan_id].options[idx_value].value;
	var bound = form.elements["channelbound"+wlan_id].selectedIndex;
	var adjust_chan;
	
	if(band_value ==3){
		currentBand = 2;
	}
	else if(band_value ==0 || band_value ==1 || band_value ==2 || band_value == 9 || band_value ==10 || band_value ==7 ){
		currentBand = 1;
	}else if(band_value == 4 || band_value==5 || band_value==6){
		currentBand = 3;
	}
if(band_value==9 || band_value==10 || band_value ==7){	
if(bound ==0)
	adjust_chan=0;
if(bound ==1)
	adjust_chan=1;	
}else
	adjust_chan=0;	
  //if (lastBand[wlan_id] != currentBand) {
  //	lastBand[wlan_id] = currentBand;
	if (currentBand == 3) {
		showChannel2G(form, wlan_id, adjust_chan, band_value);
		showChannel5G(form, wlan_id);
	}
    	if (currentBand == 2) {
		startChanIdx[wlan_id]=0;
		showChannel5G(form, wlan_id);
	}
  	if (currentBand == 1)
		showChannel2G(form, wlan_id, adjust_chan, band_value);
  //}
  	if(band_value==9 || band_value==10 || band_value==7){
	  	if(form.elements["chan"+wlan_id].selectedIndex ==0){
	  		disableTextField(form.elements["controlsideband"+wlan_id]);	
		}
	}
}



function updateChan(form, wlan_id)
{
	var idx_value= form.elements["band"+wlan_id].selectedIndex;
	var band_value= form.elements["band"+wlan_id].options[idx_value].value;
	if(band_value ==3){
		currentBand = 2;
	}
	else if(band_value ==0 || band_value ==1 || band_value ==2 || band_value == 9 || band_value ==10 ||band_value ==7){
		currentBand = 1;
	}else if(band_value == 4 || band_value==5 || band_value==6){
		currentBand = 3;
	}
//  if (form.elements["band"+wlan_id].selectedIndex > 3)
//     currentBand = 3;
//  else {
//     if (form.elements["band"+wlan_id].selectedIndex == 3)
//     	currentBand = 2;
//     else
//	currentBand = 1;
//  }

  if (lastBand[wlan_id] != currentBand) {
  	lastBand[wlan_id] = currentBand;
	if (currentBand == 3) {
		showChannel2G(form, wlan_id, 0, band_value);
		showChannel5G(form, wlan_id);
	}
    	if (currentBand == 2) {
		startChanIdx[wlan_id]=0;
		showChannel5G(form, wlan_id);
	}
  	if (currentBand == 1)
		showChannel2G(form, wlan_id, 0, band_value);
  }
  	if(band_value==9 || band_value==10 || band_value==7){
	  	if(form.elements["chan"+wlan_id].selectedIndex ==0){
	  		disableTextField(form.elements["controlsideband"+wlan_id]);	
		}
	}
}

function showBand_MultipleAP(form, wlan_id, band_root, index_id)
{
  var idx=0;
  var band_value=bandIdx[wlan_id];
  if(band_root ==0){
	form.elements["wl_band_ssid"+index_id].options[idx++] = new Option("2.4 GHz (B)", "0", false, false);
}else if(band_root ==1){
	form.elements["wl_band_ssid"+index_id].options[idx++] = new Option("2.4 GHz (G)", "1", false, false);
}else if(band_root ==2){
	form.elements["wl_band_ssid"+index_id].options[idx++] = new Option("2.4 GHz (B)", "0", false, false);
	form.elements["wl_band_ssid"+index_id].options[idx++] = new Option("2.4 GHz (G)", "1", false, false);	
 	form.elements["wl_band_ssid"+index_id].options[idx++] = new Option("2.4 GHz (B+G)", "2", false, false);
}else if(band_root ==9){
	form.elements["wl_band_ssid"+index_id].options[idx++] = new Option("2.4 GHz (G)", "1", false, false);	
	form.elements["wl_band_ssid"+index_id].options[idx++] = new Option("2.4 GHz (G+N)", "9", false, false);
}else if(band_root ==10){
	form.elements["wl_band_ssid"+index_id].options[idx++] = new Option("2.4 GHz (B)", "0", false, false);
	form.elements["wl_band_ssid"+index_id].options[idx++] = new Option("2.4 GHz (G)", "1", false, false);	
 	form.elements["wl_band_ssid"+index_id].options[idx++] = new Option("2.4 GHz (B+G)", "2", false, false);
 	form.elements["wl_band_ssid"+index_id].options[idx++] = new Option("2.4 GHz (G+N)", "9", false, false);
 	form.elements["wl_band_ssid"+index_id].options[idx++] = new Option("2.4 GHz (B+G+N)", "10", false, false);
}
/*
  if (RFType[wlan_id] != 10) {
	form.elements["band"+wlan_id].options[idx++] = new Option("5 GHz (A)", "3", false, false);
  }
*/
/*
if(band_value==9 && RFType[wlan_id] != 10)
	form.elements["wl_band_ssid"+index_id].selectedIndex = 4;
else if( band_value==9 && RFType[wlan_id] == 10)
	form.elements["wl_band_ssid"+index_id].selectedIndex = 3;
else if(band_value==10 && RFType[wlan_id] != 10)
	form.elements["wl_band_ssid"+index_id].selectedIndex = 5;
else if(band_value==10 && RFType[wlan_id] == 10)	
	form.elements["wl_band_ssid"+index_id].selectedIndex = 4;
else
	form.elements["wl_band_ssid"+index_id].selectedIndex = band_value;
*/	 
form.elements["wl_band_ssid"+index_id].selectedIndex = 0;
 form.elements["wl_band_ssid"+index_id].length = idx;
}


function showBandAP(form, wlan_id)
{
  var idx=0;
  var band_value=bandIdx[wlan_id];
  
 form.elements["band"+wlan_id].options[idx++] = new Option("2.4 GHz (B)", "0", false, false);
 form.elements["band"+wlan_id].options[idx++] = new Option("2.4 GHz (G)", "1", false, false);
 form.elements["band"+wlan_id].options[idx++] = new Option("2.4 GHz (N)", "7", false, false); 
 form.elements["band"+wlan_id].options[idx++] = new Option("2.4 GHz (B+G)", "2", false, false);
 form.elements["band"+wlan_id].options[idx++] = new Option("2.4 GHz (G+N)", "9", false, false);
 form.elements["band"+wlan_id].options[idx++] = new Option("2.4 GHz (B+G+N)", "10", false, false);

 if (band_value==0) // B
	form.elements["band"+wlan_id].selectedIndex = 0;
 else if (band_value==1)  // G
	form.elements["band"+wlan_id].selectedIndex = 1; 
 else if (band_value==7)  // N
	form.elements["band"+wlan_id].selectedIndex = 2;
 else if (band_value==2)  // B+G
	form.elements["band"+wlan_id].selectedIndex = 3;
 else if (band_value==9)  // G+N
	form.elements["band"+wlan_id].selectedIndex = 4;
 else // B+G+N
	form.elements["band"+wlan_id].selectedIndex = 5;
	 
 form.elements["band"+wlan_id].length = idx;

}
        
     
function showBandClient(form, wlan_id)
{
  var idx=0;
   var band_value=bandIdx[wlan_id];
 form.elements["band"+wlan_id].options[idx++] = new Option("2.4 GHz (B)", "0", false, false);
 form.elements["band"+wlan_id].options[idx++] = new Option("2.4 GHz (G)", "1", false, false);
 form.elements["band"+wlan_id].options[idx++] = new Option("2.4 GHz (N)", "7", false, false); 
 form.elements["band"+wlan_id].options[idx++] = new Option("2.4 GHz (B+G)", "2", false, false);
 form.elements["band"+wlan_id].options[idx++] = new Option("2.4 GHz (G+N)", "9", false, false);
 form.elements["band"+wlan_id].options[idx++] = new Option("2.4 GHz (B+G+N)", "10", false, false);

 if (band_value==0) // B
	form.elements["band"+wlan_id].selectedIndex = 0;
 else if (band_value==1)  // G
	form.elements["band"+wlan_id].selectedIndex = 1; 
 else if (band_value==7)  // N
	form.elements["band"+wlan_id].selectedIndex = 2;
 else if (band_value==2)  // B+G
	form.elements["band"+wlan_id].selectedIndex = 3;
 else if (band_value==9)  // G+N
	form.elements["band"+wlan_id].selectedIndex = 4;
 else // B+G+N
	form.elements["band"+wlan_id].selectedIndex = 5;
 form.elements["band"+wlan_id].length = idx;
}

function showBand(form, wlan_id)
{
  if (APMode[wlan_id] != 1)
	showBandAP(form, wlan_id);
  else
 	showBandClient(form, wlan_id);
 	
}

function get_by_id(id){
	with(document){
	return getElementById(id);
	}
}
function get_by_name(name){
	with(document){
	return getElementsByName(name);
	}
}
function updateMode(form, wlan_id)
{
	var chan_boundid;
	var controlsidebandid;
	var wlan_wmm1;
	var wlan_wmm2;
	var networktype;
	var mode_idx =form.elements["mode"+wlan_id].selectedIndex;
	var mode_value = form.elements["mode"+wlan_id].options[mode_idx].value; 
	var idx_value= form.elements["band"+wlan_id].selectedIndex;
	var band= form.elements["band"+wlan_id].options[idx_value].value;
	if (form.elements["mode"+wlan_id].selectedIndex != 1) {
  		if (APMode[wlan_id] == 1) {
			if (bandIdxAP[wlan_id] < 0){
				bandIdx[wlan_id]=2;	// set B+G as default
			}else{
	bandIdx[wlan_id]=bandIdxAP[wlan_id];
			}
		}  
	}else {
	  	if (APMode[wlan_id] != 1) {
			if (bandIdxClient[wlan_id] < 0) {
	 			if (RFType[wlan_id] == 10)
					bandIdx[wlan_id]=2;	// set B+G as default
				else
					bandIdx[wlan_id]=6;	// set A+B+G as default
			}
			else
				bandIdx[wlan_id]=bandIdxClient[wlan_id];
		}	
	}
  APMode[wlan_id] =form.elements["mode"+wlan_id].selectedIndex;
  showBand(form, wlan_id);
  	if(form == document.wlanSetup){
  		wlan_wmm1 = form.elements["wlanwmm"+wlan_id];
  		wlan_wmm2 =  get_by_id("wlan_wmm");
	}

	networktype = form.elements["type"+wlan_id];
	if(mode_value !=1){ 
		networktype.disabled = true;
	}else {
		networktype.selectedIndex = networkType[wlan_id];
		networktype.disabled = false;		
	}
  
 	chan_boundid = get_by_id("channel_bounding");
  	controlsidebandid = get_by_id("control_sideband");  
  	
	if(bandIdx[wlan_id] == 9 || bandIdx[wlan_id] == 10 || bandIdx[wlan_id] == 7){
	  		chan_boundid.style.display = "";
	 		controlsidebandid.style.display = "";
		 if(form == document.wlanSetup){
			wlan_wmm1.disabled = true;
		 		//wlan_wmm2.disabled = true;
		}
		}else{
	 		chan_boundid.style.display = "none";
	 		controlsidebandid.style.display = "none";
	 	 if(form == document.wlanSetup){
	 		wlan_wmm1.disabled = false;
	 		//wlan_wmm2.disabled = false;
	 	}
	 }
  updateIputState(form, wlan_id);
  if(form==document.wizard){
  		var chan_number_idx=form.elements["chan"+wlan_id].selectedIndex;
		var chan_number= form.elements["chan"+wlan_id].options[chan_number_idx].value;	
		if(chan_number == 0)
			disableTextField(form.elements["controlsideband"+wlan_id]);	
		else{
			if(form.elements["channelbound"+wlan_id].selectedIndex == "0")
	 			disableTextField(form.elements["controlsideband"+wlan_id]);	
	 		else
				enableTextField(form.elements["controlsideband"+wlan_id]);										
		}				
		
		if(APMode[wlan_id] == 0 || APMode[wlan_id] == 3)// 0:AP; 3:AP+WDS
		{
			if(document.getElementById("wlanRptEnableDiv"))
				document.getElementById("wlanRptEnableDiv").style.display = "";
			if(document.getElementById("wlanRptSsidDiv"))
				document.getElementById("wlanRptSsidDiv").style.display = "";
		}
		else
		{
			if(document.getElementById("wlanRptEnableDiv"))
				document.getElementById("wlanRptEnableDiv").style.display = "none";
			if(document.getElementById("wlanRptSsidDiv"))
				document.getElementById("wlanRptSsidDiv").style.display = "none";
		}	
	}
}

function updateBand(form, wlan_id)
{
  var band_index= form.elements["band"+wlan_id].selectedIndex;
	var band_value= form.elements["band"+wlan_id].options[band_index].value;
  if (APMode[wlan_id] != 1){
	bandIdxAP[wlan_id] = band_value;
  }else{
	bandIdxClient[wlan_id] =band_value;
  }	

  updateChan(form, wlan_id);
  
}

function updateRepeaterState(form, wlan_id)
{   
  if(!form.elements["wlanDisabled"+wlan_id].checked &&  	
    ((form.elements["mode"+wlan_id].selectedIndex!=1) ||
       ((form.elements["mode"+wlan_id].selectedIndex==1) &&
     	(form.elements["type"+wlan_id].selectedIndex==0))) 
     ){     	
     	  if(form == document.wlanSetup){	
	enableCheckBox(form.elements["repeaterEnabled"+wlan_id]);
	if (form.elements["repeaterEnabled"+wlan_id].checked)
 		enableTextField(form.elements["repeaterSSID"+wlan_id]);
  	else
  		disableTextField(form.elements["repeaterSSID"+wlan_id]);
  }
  }
  else {
  		 if(form == document.wlanSetup){	
			disableCheckBox(form.elements["repeaterEnabled"+wlan_id]);
			disableTextField(form.elements["repeaterSSID"+wlan_id]);
		}
  }
}

function updateType(form, wlan_id)
{
	var mode_selected=0;
	var Type_selected=0;
	var index_channelbound=0;
  updateChan(form, wlan_id);
  updateIputState(form, wlan_id);
  updateRepeaterState(form, wlan_id);
  Type_selected = form.elements["type"+wlan_id].selectedIndex;
  mode_selected=form.elements["mode"+wlan_id].selectedIndex;
  //if client and infrastructure mode
  	if(mode_selected ==1){
		if(Type_selected == 0){
			disableTextField(form.elements["controlsideband"+wlan_id]);
			disableTextField(form.elements["channelbound"+wlan_id]);
		}else{
			enableTextField(form.elements["channelbound"+wlan_id]);
			index_channelbound=form.elements["channelbound"+wlan_id].selectedIndex;
		if(index_channelbound ==0)
			disableTextField(form.elements["controlsideband"+wlan_id]);	
		else
			enableTextField(form.elements["controlsideband"+wlan_id]);
		}
	}
	
		var chan_number_idx=form.elements["chan"+wlan_id].selectedIndex;
		var chan_number= form.elements["chan"+wlan_id].options[chan_number_idx].value;	
		if(chan_number == 0)
			disableTextField(form.elements["controlsideband"+wlan_id]);	
		else{
			if(form.elements["channelbound"+wlan_id].selectedIndex == "0")
	 			disableTextField(form.elements["controlsideband"+wlan_id]);	
	 		else
				enableTextField(form.elements["controlsideband"+wlan_id]);		
		}
	
}

/*==============================================================================*/
/*   wlwpa.asp */
function disableRadioGroup (radioArrOrButton)
{
  if (radioArrOrButton.type && radioArrOrButton.type == "radio") {
 	var radioButton = radioArrOrButton;
 	var radioArray = radioButton.form[radioButton.name];
  }
  else
 	var radioArray = radioArrOrButton;
 	radioArray.disabled = true;
 	for (var b = 0; b < radioArray.length; b++) {
 	if (radioArray[b].checked) {
 		radioArray.checkedElement = radioArray[b];
 		break;
	}
  }
  for (var b = 0; b < radioArray.length; b++) {
 	radioArray[b].disabled = true;
 	radioArray[b].checkedElement = radioArray.checkedElement;
  }
}

function enableRadioGroup (radioArrOrButton)
{
  if (radioArrOrButton.type && radioArrOrButton.type == "radio") {
 	var radioButton = radioArrOrButton;
 	var radioArray = radioButton.form[radioButton.name];
  }
  else
 	var radioArray = radioArrOrButton;

  radioArray.disabled = false;
  radioArray.checkedElement = null;
  for (var b = 0; b < radioArray.length; b++) {
 	radioArray[b].disabled = false;
 	radioArray[b].checkedElement = null;
  }
}

function preserve () { this.checked = this.storeChecked; }
function disableCheckBox (checkBox) {
  if (!checkBox.disabled) {
    checkBox.disabled = true;
    if (!document.all && !document.getElementById) {
      checkBox.storeChecked = checkBox.checked;
      checkBox.oldOnClick = checkBox.onclick;
      checkBox.onclick = preserve;
    }
  }
}

function enableCheckBox (checkBox)
{
  if (checkBox.disabled) {
    checkBox.disabled = false;
    if (!document.all && !document.getElementById)
      checkBox.onclick = checkBox.oldOnClick;
  }
}
function openWindow(url, windowName, wide, high) {
	if (document.all)
		var xMax = screen.width, yMax = screen.height;
	else if (document.layers)
		var xMax = window.outerWidth, yMax = window.outerHeight;
	else
	   var xMax = 640, yMax=500;
	var xOffset = (xMax - wide)/2;
	var yOffset = (yMax - high)/3;

	var settings = 'width='+wide+',height='+high+',screenX='+xOffset+',screenY='+yOffset+',top='+yOffset+',left='+xOffset+', resizable=yes, toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes';
	window.open( url, windowName, settings );
}
function validateKey(str)
{
   for (var i=0; i<str.length; i++) {
    if ( (str.charAt(i) >= '0' && str.charAt(i) <= '9') ||
    		(str.charAt(i) == '.' ) )
			continue;
	return 0;
  }
  return 1;
}

function getDigit(str, num)
{
  i=1;
  if ( num != 1 ) {
  	while (i!=num && str.length!=0) {
		if ( str.charAt(0) == '.' ) {
			i++;
		}
		str = str.substring(1);
  	}
  	if ( i!=num )
  		return -1;
  }
  for (i=0; i<str.length; i++) {
  	if ( str.charAt(i) == '.' ) {
		str = str.substring(0, i);
		break;
	}
  }
  if ( str.length == 0)
  	return -1;
  d = parseInt(str, 10);
  return d;
}

function checkDigitRange(str, num, min, max)
{
  d = getDigit(str,num);
  if ( d > max || d < min )
      	return false;
  return true;
}


function check_wpa_psk(form, wlan_id)
{
	var str = form.elements["pskValue"+wlan_id].value;
	if (form.elements["pskFormat"+wlan_id].selectedIndex==1) {
		if (str.length != 64) {
			alert('Pre-Shared Key value should be 64 characters.');
			form.elements["pskValue"+wlan_id].focus();
			return false;
		}
		takedef = 0;
		if (defPskFormat[wlan_id] == 1 && defPskLen[wlan_id] == 64) {
			for (var i=0; i<64; i++) {
    				if ( str.charAt(i) != '*')
					break;
			}
			if (i == 64 )
				takedef = 1;
  		}
		if (takedef == 0) {
			for (var i=0; i<str.length; i++) {
    				if ( (str.charAt(i) >= '0' && str.charAt(i) <= '9') ||
					(str.charAt(i) >= 'a' && str.charAt(i) <= 'f') ||
					(str.charAt(i) >= 'A' && str.charAt(i) <= 'F') )
					continue;
				alert("Invalid Pre-Shared Key value. It should be in hex number (0-9 or a-f).");
				form.elements["pskValue"+wlan_id].focus();
				return false;
  			}
		}
	}
	else {
		if (str.length < 8) {
			alert('Pre-Shared Key value should be set at least 8 characters.');
			form.elements["pskValue"+wlan_id].focus();
			return false;
		}
		if (str.length > 63) {
			alert('Pre-Shared Key value should be less than 64 characters.');
			form.elements["pskValue"+wlan_id].focus();
			return false;
		}
	}

// for WPS  ---------------------------------------->>
/*
   method = form.elements["method"+wlan_id];
   clear_registrar = form.elements["wps_clear_configure_by_reg"+wlan_id];
   if (wps_disabled == 0 && wps_config_by_registrar !=0) {
  	if (clear_registrar.value==0 &&
  		(wps_encrypt_old == 2 || wps_encrypt_old == 6)) {		
		if (((wps_wpaCipher_old & 1) && form.elements["ciphersuite"+wlan_id][0].checked==0) ||
			(!(wps_wpaCipher_old & 1) && form.elements["ciphersuite"+wlan_id][0].checked) ||
			((wps_wpaCipher_old & 2) && form.elements["ciphersuite"+wlan_id][1].checked==0) ||
			(!(wps_wpaCipher_old & 2) && form.elements["ciphersuite"+wlan_id][1].checked)) {			
			if (!confirm(wps_warn3)) {
				return false;
			} 
			else
				clear_registrar.value = 1;
		}
	}
  	if (clear_registrar.value==0 && 
  		(wps_encrypt_old == 4 || wps_encrypt_old == 6)) {		
		if (((wps_wpa2Cipher_old & 1) && form.elements["wpa2ciphersuite"+wlan_id][0].checked==0) ||
			(!(wps_wpa2Cipher_old & 1) && form.elements["wpa2ciphersuite"+wlan_id][0].checked) ||
			((wps_wpa2Cipher_old & 2) && form.elements["wpa2ciphersuite"+wlan_id][1].checked==0) ||
			(!(wps_wpa2Cipher_old & 2) && form.elements["wpa2ciphersuite"+wlan_id][1].checked)) {
			if (!confirm(wps_warn3)) {
				return false;
			} 
			else
				clear_registrar.value = 1;
		}
	}	 
	if (clear_registrar.value==0 &&
			wps_psk_old != form.elements["pskValue"+wlan_id].value &&
			wps_psk_unmask_old != form.elements["pskValue"+wlan_id].value) {
		if (!confirm(wps_warn3)) {
			method.selectedIndex = method.defaultValue;
			method.focus();
			return false;
		} 
		else
			clear_registrar.value = 1;		
	}  	
  }   	
*/  
//<<------------------------------------------ for WPS	
  
  return true;
  }

function saveChanges_wpa(form, wlan_id)
{
  method = form.elements["method"+wlan_id] ;
  wpaAuth= form.elements["wpaAuth"+wlan_id] ;
// for WPS --------------------------------------->>
/*
  if (check_wps_wlanmode(wps_mode_old, wps_type_old) == 0) {
	  use1x = form.elements["use1x"+wlan_id];
	  clear_registrar = form.elements["wps_clear_configure_by_reg"+wlan_id];
	  if (wps_disabled == 0) {
			if (wps_config_by_registrar !=0) {
				if (wps_encrypt_old != method.value) {
					if (!confirm(wps_warn3)) {
						method.value = method.defaultValue;
						method.focus();
						return false;
					} 
					else
						clear_registrar.value = 1;
				}
			}
			if ((method.selectedIndex == 0 || method.selectedIndex == 1) &&
									use1x.checked) {
				if (!confirm(wps_warn5)) {
					use1x.checked = 0;
					use1x.focus();
					return false;
				} 
			}
			if (method.selectedIndex >=2 && 
				(wps_wizard == 0 && wpaAuth[0].checked)) {
				if (!confirm(wps_warn4)) {
					wpaAuth[0].checked = 0;
					wpaAuth[1].checked = 1				
					wpaAuth[1].focus();
					return false;
				}
			}	
	  }
  }
*/  
//<<-------------------------------------------- for WPS
/*
  //if band include 11n then the security use AES only
  if(bandIdx[wlan_id]==9 || bandIdx[wlan_id] ==10){
   		if(method.value==1){
   			alert(encrypt_11n);
   			return false;
   		}else if(method.value ==2){
   			var wpa_cipher=form.elements["ciphersuite"+wlan_id];
   			if(wpa_cipher[0].checked){
   				alert(encrypt_11n);
   				return false;
   			}
   		}else if(method.value ==4){
   			var wpa2_cipher=form.elements["wpa2ciphersuite"+wlan_id];
   			if(wpa2_cipher[0].checked ){
   				alert(encrypt_11n);
   				return false;
   			}
   		}else if(method.value ==6){
   			var wpa_cipher=form.elements["ciphersuite"+wlan_id];
   			if(wpa_cipher[0].checked){
   				alert(encrypt_11n);
   				return false;
   			}
   			var wpa2_cipher=form.elements["wpa2ciphersuite"+wlan_id];
   			if(wpa2_cipher[0].checked){
   				alert(encrypt_11n);
   				return false;
   			}
   		}
   	}
*/   	
  if (method.selectedIndex>=2 && (wpaAuth.value == "psk" || wpaAuth[1].checked))
		return check_wpa_psk(form, wlan_id);	
 
    if (form.elements["use1x"+wlan_id].value != "OFF" && form.elements["radiusPort"+wlan_id].disabled == false ) {
	if (form.elements["radiusPort"+wlan_id].value=="") {
		alert("RADIUS Server port number cannot be empty! It should be a decimal number between 1-65535.");
		form.elements["radiusPort"+wlan_id].focus();
		return false;
  	}
	if (validateKey(form.elements["radiusPort"+wlan_id].value)==0) {
		alert("Invalid port number of RADIUS Server! It should be a decimal number between 1-65535.");
		form.elements["radiusPort"+wlan_id].focus();
		return false;
	}
        port = parseInt(form.elements["radiusPort"+wlan_id].value, 10);

 	if (port > 65535 || port < 1) {
		alert("Invalid port number of RADIUS Server! It should be a decimal number between 1-65535.");
		form.elements["radiusPort"+wlan_id].focus();
		return false;
  	}

	if ( checkIpAddr(form.elements["radiusIP"+wlan_id], 'Invalid RADIUS Server IP address') == false )
	    return false;
   } 
   	
   

   return true;
}
/*==============================================================================*/
/*   tcpiplan.asp  */
function checkMask(str, num)
{
  d = getDigit(str,num);
  if( !(d==0 || d==128 || d==192 || d==224 || d==240 || d==248 || d==252 || d==254 || d==255 ))
  	return false;
  return true;
}


function checkSubnet(ip, mask, client)
{
  ip_d = getDigit(ip, 1);
  mask_d = getDigit(mask, 1);
  client_d = getDigit(client, 1);
  if ( (ip_d & mask_d) != (client_d & mask_d ) )
	return false;

  ip_d = getDigit(ip, 2);
  mask_d = getDigit(mask, 2);
  client_d = getDigit(client, 2);
  if ( (ip_d & mask_d) != (client_d & mask_d ) )
	return false;

  ip_d = getDigit(ip, 3);
  mask_d = getDigit(mask, 3);
  client_d = getDigit(client, 3);
  if ( (ip_d & mask_d) != (client_d & mask_d ) )
	return false;

  ip_d = getDigit(ip, 4);
  mask_d = getDigit(mask, 4);
  client_d = getDigit(client, 4);
  if ( (ip_d & mask_d) != (client_d & mask_d ) )
	return false;

  return true;
}
function checkIPMask(field)
{

  if (field.value=="") {
      	alert("Subnet mask cannot be empty! It should be filled with 4 digit numbers as xxx.xxx.xxx.xxx.");
	field.value = field.defaultValue;
	field.focus();
	return false;
  }
  if ( validateKey( field.value ) == 0 ) {
      	alert("Invalid subnet mask value. It should be the decimal number (0-9).");
      	field.value = field.defaultValue;
	field.focus();
	return false;
  }
  if ( !checkMask(field.value,1) ) {
      	alert('Invalid subnet mask in 1st digit.\nIt should be the number of 0, 128, 192, 224, 240, 248, 252 or 254');
	field.value = field.defaultValue;
	field.focus();
	return false;
  }

  if ( !checkMask(field.value,2) ) {
      	alert('Invalid subnet mask in 2nd digit.\nIt should be the number of 0, 128, 192, 224, 240, 248, 252 or 254');
	field.value = field.defaultValue;
	field.focus();
	return false;
  }
  if ( !checkMask(field.value,3) ) {
      	alert('Invalid subnet mask in 3rd digit.\nIt should be the number of 0, 128, 192, 224, 240, 248, 252 or 254');
	field.value = field.defaultValue;
	field.focus();
	return false;
  }
  if ( !checkMask(field.value,4) ) {
      	alert('Invalid subnet mask in 4th digit.\nIt should be the number of 0, 128, 192, 224, 240, 248, 252 or 254');
	field.value = field.defaultValue;
	field.focus();
	return false;
  }
}  
function checkIpAddr(field, msg)
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
   if ( !checkDigitRange(field.value,4,1,254) ) {
      alert(msg + ' range in 4th digit. It should be 1-254.');
      field.value = field.defaultValue;
      field.focus();
      return false;
   }
   return true;
}


/////////////////////////////////////////////////////////////////////////////
/*wlwep.asp*/
function validateKey_wep(form, idx, str, len, wlan_id)
{
 if (idx >= 0) {
 	//Brad disable
 // if (form.elements["defaultTxKeyId"+wlan_id].selectedIndex==idx && str.length==0) {
//	alert('The encryption key you selected as the \'Tx Default Key\' cannot be blank.');
//	return 0;
//  }
  if (str.length ==0)
  	return 1;

  if ( str.length != len) {
  	idx++;
	alert('Invalid length of Key ' + idx + ' value. It should be ' + len + ' characters.');
	return 0;
  }
  }
  else {
	if ( str.length != len) {
		alert('Invalid length of WEP Key value. It should be ' + len + ' characters.');
		return 0;
  	}
  }
  if ( str == "*****" ||
       str == "**********" ||
       str == "*************" ||
       str == "**************************" )
       return 1;

  if (form.elements["format"+wlan_id].selectedIndex==0)
       return 1;

  for (var i=0; i<str.length; i++) {
    if ( (str.charAt(i) >= '0' && str.charAt(i) <= '9') ||
			(str.charAt(i) >= 'a' && str.charAt(i) <= 'f') ||
			(str.charAt(i) >= 'A' && str.charAt(i) <= 'F') )
			continue;

	alert("Invalid key value. It should be in hex number (0-9 or a-f).");
	return 0;
  }

  return 1;
}

function setDefaultWPAKeyValue(form, wlan_id, formatStr, keyStr)
{
	if ( form.elements[formatStr+wlan_id].selectedIndex == 0) {
		form.elements[keyStr+wlan_id].maxLength = 63;
		form.elements[keyStr+wlan_id].value = "***************************************************************";
	}
	else {
		form.elements[keyStr+wlan_id].maxLength = 64;
		form.elements[keyStr+wlan_id].value = "****************************************************************";

	}
}

function setDefaultWEPKeyValue(form, wlan_id, lengthStr, formatStr, keyStr)
{
  if (form.elements[lengthStr+wlan_id].selectedIndex == 0) {
	if ( form.elements[formatStr+wlan_id].selectedIndex == 0) {
		form.elements[keyStr+wlan_id].maxLength = 5;
		form.elements[keyStr+wlan_id].value = "*****";
	}
	else {
		form.elements[keyStr+wlan_id].maxLength = 10;
		form.elements[keyStr+wlan_id].value = "**********";

	}
  }
  else {
  	if ( form.elements[formatStr+wlan_id].selectedIndex == 0) {
		form.elements[keyStr+wlan_id].maxLength = 13;
		form.elements[keyStr+wlan_id].value = "*************";

	}
	else {
		form.elements[keyStr+wlan_id].maxLength = 26;
		form.elements[keyStr+wlan_id].value ="**************************";
	}
  }

// for WPS ---------------------------------------->>
//  wps_wep_key_old =  form.elements["key"+wlan_id].value;
//<<----------------------------------------- for WPS
  
}
function saveChanges_wepkey(form, wlan_id)
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
	form.elements["key"+wlan_id].focus();
	return false;
  }

// for WPS --------------------------------------------------->>
/*
  clear_registrar = form.elements["wps_clear_configure_by_reg"+wlan_id];
  if (wps_disabled == 0 && wps_config_by_registrar !=0) {
	if (wps_encrypt_old != 1) {
		if (!confirm(wps_warn3))
			return false;
		else
			clear_registrar.value = 1;
	}
	if (clear_registrar.value == 0 &&
		wps_wep_keylen_old != form.elements["length"+wlan_id].value) {
		if (!confirm(wps_warn3)) {
			form.elements["length"+wlan_id].value = wps_wep_keylen_old;			
			form.elements["length"+wlan_id].focus();
			setDefaultWEPKeyValue(form, wlan_id);
			return false;
		} 
		else
			clear_registrar.value = 1;		
	}
	if (clear_registrar.value == 0 &&
		wps_wep_key_old != form.elements["key"+wlan_id].value) {
		if (!confirm(wps_warn3)) {
			form.elements["key"+wlan_id].value = wps_wep_key_old;
			form.elements["key"+wlan_id].focus();
			return false;
		} 
		else
			clear_registrar.value = 1;		
	}
	
  }
*/  
//<<---------------------------------------------------- for WPS

  return true;
}

function setDefaultKeyValue(form, wlan_id)
{
  if (form.elements["length"+wlan_id].selectedIndex == 0) {
	if ( form.elements["format"+wlan_id].selectedIndex == 0) {
		form.elements["key"+wlan_id].maxLength = 5;
		form.elements["key"+wlan_id].value = "*****";
		
/*		
		form.elements["key1"+wlan_id].maxLength = 5;
		form.elements["key2"+wlan_id].maxLength = 5;
		form.elements["key3"+wlan_id].maxLength = 5;
		form.elements["key4"+wlan_id].maxLength = 5;
		form.elements["key1"+wlan_id].value = "*****";
		form.elements["key2"+wlan_id].value = "*****";
		form.elements["key3"+wlan_id].value = "*****";
		form.elements["key4"+wlan_id].value = "*****";
*/		
	}
	else {
		form.elements["key"+wlan_id].maxLength = 10;
		form.elements["key"+wlan_id].value = "**********";
		
/*		
		form.elements["key1"+wlan_id].maxLength = 10;
		form.elements["key2"+wlan_id].maxLength = 10;
		form.elements["key3"+wlan_id].maxLength = 10;
		form.elements["key4"+wlan_id].maxLength = 10;
		form.elements["key1"+wlan_id].value = "**********";
		form.elements["key2"+wlan_id].value = "**********";
		form.elements["key3"+wlan_id].value = "**********";
		form.elements["key4"+wlan_id].value = "**********";
*/
	}
  }
  else {
  	if ( form.elements["format"+wlan_id].selectedIndex == 0) {
		form.elements["key"+wlan_id].maxLength = 13;		
		form.elements["key"+wlan_id].value = "*************";		
/*		
		form.elements["key1"+wlan_id].maxLength = 13;
		form.elements["key2"+wlan_id].maxLength = 13;
		form.elements["key3"+wlan_id].maxLength = 13;
		form.elements["key4"+wlan_id].maxLength = 13;
		form.elements["key1"+wlan_id].value = "*************";
		form.elements["key2"+wlan_id].value = "*************";
		form.elements["key3"+wlan_id].value = "*************";
		form.elements["key4"+wlan_id].value = "*************";
*/		

	}
	else {
		form.elements["key"+wlan_id].maxLength = 26;
		form.elements["key"+wlan_id].value ="**************************";		
/*		
		form.elements["key1"+wlan_id].maxLength = 26;
		form.elements["key2"+wlan_id].maxLength = 26;
		form.elements["key3"+wlan_id].maxLength = 26;
		form.elements["key4"+wlan_id].maxLength = 26;
		form.elements["key1"+wlan_id].value ="**************************";
		form.elements["key2"+wlan_id].value ="**************************";
		form.elements["key3"+wlan_id].value ="**************************";
		form.elements["key4"+wlan_id].value ="**************************";
*/		
	}
  }

// for WPS ---------------------------------------->>
/*
  wps_wep_key1_old =  form.elements["key"+wlan_id].value
  wps_wep_key2_old =  form.elements["key"+wlan_id].value
  wps_wep_key3_old =  form.elements["key"+wlan_id].value
  wps_wep_key4_old =  form.elements["key"+wlan_id].value
*/  
//<<----------------------------------------- for WPS
  
}


function saveChanges_wep(form, wlan_id)
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
	form.elements["key"+wlan_id].focus();
	return false;
  }

  
/*
  if (validateKey_wep(form, 0,form.elements["key1"+wlan_id].value, keyLen, wlan_id)==0) {
	form.elements["key1"+wlan_id].focus();
	return false;
  }
  if (validateKey_wep(form, 1,form.elements["key2"+wlan_id].value, keyLen, wlan_id)==0) {
	form.elements["key2"+wlan_id].focus();
	return false;
  }
  if (validateKey_wep(form, 2,form.elements["key3"+wlan_id].value, keyLen, wlan_id)==0) {
	form.elements["key3"+wlan_id].focus();
	return false;
  }
  if (validateKey_wep(form, 3,form.elements["key4"+wlan_id].value, keyLen, wlan_id)==0) {
	form.elements["key4"+wlan_id].focus();
	return false;
  }
*/

// for WPS --------------------------------------------------->>
/*
  clear_registrar = form.elements["wps_clear_configure_by_reg"+wlan_id];
  if (wps_disabled == 0 && wps_config_by_registrar !=0) {
	if (wps_encrypt_old != 1) {
		if (!confirm(wps_warn3))
			return false;
		else
			clear_registrar.value = 1;
	}
	if (clear_registrar.value == 0 &&
		wps_wep_keylen_old != form.elements["length"+wlan_id].value) {
		if (!confirm(wps_warn3)) {
			form.elements["length"+wlan_id].value = wps_wep_keylen_old;			
			form.elements["length"+wlan_id].focus();
			setDefaultKeyValue(form, wlan_id);
			return false;
		} 
		else
			clear_registrar.value = 1;		
	}
	if (clear_registrar.value == 0 &&
		wps_wep_key1_old != form.elements["key"+wlan_id].value) {
		if (!confirm(wps_warn3)) {
			form.elements["key"+wlan_id].value = wps_wep_key1_old;
			form.elements["key"+wlan_id].focus();
			return false;
		} 
		else
			clear_registrar.value = 1;		
	}	
  }
  */
//<<---------------------------------------------------- for WPS

  return true;
}



function lengthClick(form, wlan_id)
{
  updateFormat(form, wlan_id);
}

function getRefToDivNest(divID, oDoc) 
{
  if( !oDoc ) { oDoc = document; }
  if( document.layers ) {
	if( oDoc.layers[divID] ) { return oDoc.layers[divID]; } else {
	for( var x = 0, y; !y && x < oDoc.layers.length; x++ ) {
		y = getRefToDivNest(divID,oDoc.layers[x].document); }
	return y; } }
  if( document.getElementById ) { return document.getElementById(divID); }
  if( document.all ) { return document.all[divID]; }
  return document[divID];
}

function progressBar( oBt, oBc, oBg, oBa, oWi, oHi, oDr ) 
{
  MWJ_progBar++; this.id = 'MWJ_progBar' + MWJ_progBar; this.dir = oDr; this.width = oWi; this.height = oHi; this.amt = 0;
  //write the bar as a layer in an ilayer in two tables giving the border
  document.write( '<span id = "progress_div" class = "off" > <table border="0" cellspacing="0" cellpadding="'+oBt+'">'+
	'<tr><td>Please wait...</td></tr><tr><td bgcolor="'+oBc+'">'+
		'<table border="0" cellspacing="0" cellpadding="0"><tr><td height="'+oHi+'" width="'+oWi+'" bgcolor="'+oBg+'">' );
  if( document.layers ) {
	document.write( '<ilayer height="'+oHi+'" width="'+oWi+'"><layer bgcolor="'+oBa+'" name="MWJ_progBar'+MWJ_progBar+'"></layer></ilayer>' );
  } else {
	document.write( '<div style="position:relative;top:0px;left:0px;height:'+oHi+'px;width:'+oWi+';">'+
			'<div style="position:absolute;top:0px;left:0px;height:0px;width:0;font-size:1px;background-color:'+oBa+';" id="MWJ_progBar'+MWJ_progBar+'"></div></div>' );
  }
  document.write( '</td></tr></table></td></tr></table></span>\n' );
  this.setBar = resetBar; //doing this inline causes unexpected bugs in early NS4
  this.setCol = setColour;
}

function resetBar( a, b ) 
{
  //work out the required size and use various methods to enforce it
  this.amt = ( typeof( b ) == 'undefined' ) ? a : b ? ( this.amt + a ) : ( this.amt - a );
  if( isNaN( this.amt ) ) { this.amt = 0; } if( this.amt > 1 ) { this.amt = 1; } if( this.amt < 0 ) { this.amt = 0; }
  var theWidth = Math.round( this.width * ( ( this.dir % 2 ) ? this.amt : 1 ) );
  var theHeight = Math.round( this.height * ( ( this.dir % 2 ) ? 1 : this.amt ) );
  var theDiv = getRefToDivNest( this.id ); if( !theDiv ) { window.status = 'Progress: ' + Math.round( 100 * this.amt ) + '%'; return; }
  if( theDiv.style ) { theDiv = theDiv.style; theDiv.clip = 'rect(0px '+theWidth+'px '+theHeight+'px 0px)'; }
  var oPix = document.childNodes ? 'px' : 0;
  theDiv.width = theWidth + oPix; theDiv.pixelWidth = theWidth; theDiv.height = theHeight + oPix; theDiv.pixelHeight = theHeight;
  if( theDiv.resizeTo ) { theDiv.resizeTo( theWidth, theHeight ); }
  theDiv.left = ( ( this.dir != 3 ) ? 0 : this.width - theWidth ) + oPix; theDiv.top = ( ( this.dir != 4 ) ? 0 : this.height - theHeight ) + oPix;
}

function setColour( a ) 
{
  //change all the different colour styles
  var theDiv = getRefToDivNest( this.id ); if( theDiv.style ) { theDiv = theDiv.style; }
  theDiv.bgColor = a; theDiv.backgroundColor = a; theDiv.background = a;
}

function showtxrate_updated(form, band, wlan_id, rf_num)
{
  var idx=0;
  var i;
  var txrate_str;
 form.elements["txRate"+wlan_id].options[idx++] = new Option("Auto", "0", false, false);
 if(band == 0 || band ==2 || band ==10){
 form.elements["txRate"+wlan_id].options[idx++] = new Option("1M", "1", false, false);
 form.elements["txRate"+wlan_id].options[idx++] = new Option("2M", "2", false, false);
 form.elements["txRate"+wlan_id].options[idx++] = new Option("5.5M", "3", false, false);
 form.elements["txRate"+wlan_id].options[idx++] = new Option("11M", "4", false, false);
}
 if(band ==9 || band ==10 || band ==1 || band ==2){
 form.elements["txRate"+wlan_id].options[idx++] = new Option("6M", "5", false, false);
 form.elements["txRate"+wlan_id].options[idx++] = new Option("9M", "6", false, false);
 form.elements["txRate"+wlan_id].options[idx++] = new Option("12M", "7", false, false);
 form.elements["txRate"+wlan_id].options[idx++] = new Option("18M", "8", false, false);
 form.elements["txRate"+wlan_id].options[idx++] = new Option("24M", "9", false, false);
 form.elements["txRate"+wlan_id].options[idx++] = new Option("36M", "10", false, false);
 form.elements["txRate"+wlan_id].options[idx++] = new Option("48M", "11", false, false);
 form.elements["txRate"+wlan_id].options[idx++] = new Option("54M", "12", false, false);
}
 if(band ==9 || band ==10 || band==7){
 form.elements["txRate"+wlan_id].options[idx++] = new Option("MCS0", "13", false, false);
 form.elements["txRate"+wlan_id].options[idx++] = new Option("MCS1", "14", false, false);
 form.elements["txRate"+wlan_id].options[idx++] = new Option("MCS2", "15", false, false);
 form.elements["txRate"+wlan_id].options[idx++] = new Option("MCS3", "16", false, false);
 form.elements["txRate"+wlan_id].options[idx++] = new Option("MCS4", "17", false, false);
 form.elements["txRate"+wlan_id].options[idx++] = new Option("MCS5", "18", false, false);
 form.elements["txRate"+wlan_id].options[idx++] = new Option("MCS6", "19", false, false); 
 form.elements["txRate"+wlan_id].options[idx++] = new Option("MCS7", "20", false, false);
 if (rf_num >=2) {
	 form.elements["txRate"+wlan_id].options[idx++] = new Option("MCS8", "21", false, false);
	 form.elements["txRate"+wlan_id].options[idx++] = new Option("MCS9", "22", false, false);
	 form.elements["txRate"+wlan_id].options[idx++] = new Option("MCS10", "23", false, false);
	 form.elements["txRate"+wlan_id].options[idx++] = new Option("MCS11", "24", false, false);
	 form.elements["txRate"+wlan_id].options[idx++] = new Option("MCS12", "25", false, false);
	 form.elements["txRate"+wlan_id].options[idx++] = new Option("MCS13", "26", false, false);
	 form.elements["txRate"+wlan_id].options[idx++] = new Option("MCS14", "27", false, false);
	 form.elements["txRate"+wlan_id].options[idx++] = new Option("MCS15", "28", false, false);
 }
}
 form.elements["txRate"+wlan_id].length = idx;
for (i=0; i<idx; i++) {
 	txrate_str = form.elements["txRate"+wlan_idx].options[i].value;
 if(wlan_txrate[wlan_id]  == txrate_str)
 	form.elements["txRate"+wlan_id].selectedIndex = i;
 }
}

