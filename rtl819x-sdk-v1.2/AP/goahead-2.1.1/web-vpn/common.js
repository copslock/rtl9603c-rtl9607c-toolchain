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

var wps_wep_key1_old;
var wps_wep_key2_old;
var wps_wep_key3_old;
var wps_wep_key4_old;

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
 	band =form.elements["band"+wlan_id].selectedIndex+1;
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
				basicRate|=0xff0;
		}			
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
/*   tcpipwan.asp */
function wanShowDiv(pptp_bool, dns_bool, dnsMode_bool, pppoe_bool, static_bool)
{
 	show_div(pptp_bool,"pptp_div");
  	show_div(dns_bool,"dns_div");
  	show_div(dnsMode_bool,"dnsMode_div");
  	show_div(pppoe_bool,"pppoe_div");
  	show_div(static_bool,"static_div"); 
  	if (pptp_bool==0 && pppoe_bool==0 && static_bool==0 && dns_bool)
  	  	show_div(1,"dhcp_div");  	
  	else
  		show_div(0,"dhcp_div");  
}
function saveChanges_wan(form)
{
  var wanType = form.wanType.selectedIndex ;
  if ( wanType == 0 ){ //static IP
	  if ( checkIpAddr(form.wan_ip, 'Invalid IP address') == false )
	    return false;
  	  if (checkIPMask(form.wan_mask) == false)
  		return false ;

	  if (form.wan_gateway.value!="" && form.wan_gateway.value!="0.0.0.0") {

	    if ( checkIpAddr(form.wan_gateway, 'Invalid default gateway address') == false )
	      return false;
	    if ( !checkSubnet(form.wan_ip.value,form.wan_mask.value,form.wan_gateway.value)) {
	      alert('Invalid gateway address!\nIt should be located in the same subnet of current IP address.');
	      form.wan_gateway.value = form.wan_gateway.defaultValue;
	      form.wan_gateway.focus();
	      return false;
	    }
	  }
	  else
	      form.wan_gateway.value = '0.0.0.0';  

	  if (form.fixedIpMtuSize != null){
	     d2 = getDigit(form.fixedIpMtuSize.value, 1);
	     if ( validateKey(form.fixedIpMtuSize.value) == 0 ||
			(d2 > 1500 || d2 < 1400) ) {
			alert("Invalid MTU size! You should set a value between 1400-1500.");
			form.fixedIpMtuSize.value = form.fixedIpMtuSize.defaultValue;
			form.fixedIpMtuSize.focus();
			return false;
	     }
	  }    
  }
  else if ( wanType == 1){ //dhcp wanType
  	  if (form.dhcpMtuSize != null){
	     d2 = getDigit(form.dhcpMtuSize.value, 1);
	     if ( validateKey(form.dhcpMtuSize.value) == 0 ||
			(d2 > 1492 || d2 < 1400) ) {
			alert("Invalid MTU size! You should set a value between 1400-1492.");
			form.dhcpMtuSize.value = form.dhcpMtuSize.defaultValue;
			form.dhcpMtuSize.focus();
			return false;
	     }
	  } 
  }
  else if ( wanType == 2){ //pppoe wanType
	   if (form.pppUserName.value=="") {
		  alert('PPP user name cannot be empty!');
		  form.pppUserName.value = form.pppUserName.defaultValue;
		  form.pppUserName.focus();
		  return false;
	   }
	   if (form.pppPassword.value=="") {
		  alert('PPP password cannot be empty!');
		  form.pppPassword.value = form.pppPassword.defaultValue;
		  form.pppPassword.focus();
		  return false;
	   }
	   if ( form.pppConnectType != null){
	     if ( form.pppConnectType.selectedIndex != 0 ) {
		d1 = getDigit(form.pppIdleTime.value, 1);
		if ( validateKey(form.pppIdleTime.value) == 0 ||
			(d1 > 1000 || d1 < 1) ) {
			alert("Invalid idle time value! You should set a value between 1-1000.");
			form.pppIdleTime.focus();
			return false;
		}
	     }
	   } // if (pppConnectType !=null)
           
           if ( form.pppMtuSize != null){
	     d2 = getDigit(form.pppMtuSize.value, 1);
	     if ( validateKey(form.pppMtuSize.value) == 0 ||
			(d2 > 1492 || d2 < 1360) ) {
			alert("Invalid MTU size! You should set a value between 1360-1492.");
			form.pppMtuSize.value = form.pppMtuSize.defaultValue;
			form.pppMtuSize.focus();
			return false;
	     }
	   }  // if (pppMtuSize !=null)
  }
  else if ( wanType == 3){ //pptp wanType
	  if ( checkIpAddr(form.pptpIpAddr, 'Invalid IP address') == false )
	    return false;
	  if (checkIPMask(form.pptpSubnetMask) == false)
  			return false ;

	  if ( checkIpAddr(form.pptpServerIpAddr, 'Invalid server IP address') == false )
	      return false;

	  if ( !checkSubnet(form.pptpIpAddr.value,form.pptpSubnetMask.value,form.pptpServerIpAddr.value)) {
	      alert('Invalid server IP address!\nIt should be located in the same subnet of local IP address.');
	      form.pptpServerIpAddr.value = form.pptpServerIpAddr.defaultValue;
	      form.pptpServerIpAddr.focus();
	      return false;
	  }

	  if (form.pptpUserName.value=="") {
		  alert('User name cannot be empty!');
		  form.pptpUserName.value = form.pptpUserName.defaultValue;
		  form.pptpUserName.focus();
		  return false;
	  }
	  if (form.pptpPassword.value=="") {
		  alert('Password cannot be empty!');
		  form.pptpPassword.value = form.pptpPassword.defaultValue;
		  form.pptpPassword.focus();
		  return false;
	  }
	  if ( form.pptpMtuSize != null){
	  	d2 = getDigit(form.pptpMtuSize.value, 1);
	   	if ( validateKey(form.pptpMtuSize.value) == 0 ||
			(d2 > 1460 || d2 < 1400) ) {
			alert("Invalid MTU size! You should set a value between 1400-1460.");
			form.pptpMtuSize.value = form.pptpMtuSize.defaultValue;
			form.pptpMtuSize.focus();
			return false;
	   	}
	  } // if(pptpMtuSize != null)
   } // pppoe if wanType

   if( wanType != 0 ) { // not static IP
	   group = form.dnsMode;
	   for (var r = 0; r < group.length; r++)
		  if (group[r].checked)
		    break;

	   if (r == 1) {
	      if (form.dns1.value=="")
		form.dns1.value="0.0.0.0";
	      if (form.dns1.value!="0.0.0.0") {
		  if ( checkIpAddr(form.dns1, 'Invalid DNS1 address') == false )
		     return false;
	      }		
	      if (form.dns2 != null){
	      	if (form.dns2.value=="")
			form.dns2.value="0.0.0.0";
	      	if (form.dns2.value!="0.0.0.0") {
		 if ( checkIpAddr(form.dns2, 'Invalid DNS2 address') == false )
		     return false;
	      	}			
	      }//dns2 != null
	      if (form.dns3 != null){	
	      	if (form.dns3.value=="")
			form.dns3.value="0.0.0.0";
	      	if (form.dns3.value!="0.0.0.0") {
		  if ( checkIpAddr(form.dns3, 'Invalid DNS3 address') == false )
		     return false;
	      	}			
	      }// dns3 != null
	   }
   }
   else{
	  if (form.dns1.value=="")
	    form.dns1.value="0.0.0.0";
	  if (form.dns1.value!="0.0.0.0") {
	     if ( checkIpAddr(form.dns1, 'Invalid DNS1 address') == false )
	       return false;
	  }
	  	    
	  if (form.dns2 != null){  
	  	if (form.dns2.value=="")
	    		form.dns2.value="0.0.0.0";
	  	if (form.dns2.value!="0.0.0.0") {
	    		if ( checkIpAddr(form.dns2, 'Invalid DNS2 address') == false )
	      			return false;
	  	}	    		
	  }
	  if (form.dns3 != null){
	  	if (form.dns3.value=="")
	    		form.dns3.value="0.0.0.0";
	  	if (form.dns3.value!="0.0.0.0") {
	    		if ( checkIpAddr(form.dns3, 'Invalid DNS3 address') == false )
	      			return false;
	  	}	    		
	  } 
   }
   if (form.wan_macAddr != null){
   	if (form.wan_macAddr.value == "")
		form.wan_macAddr.value = "000000000000";
	var str = form.wan_macAddr.value;
   	if ( str.length < 12) {
		alert("Input MAC address is not complete. It should be 12 digits in hex.");
		form.wan_macAddr.value = form.wan_macAddr.defaultValue;
		form.wan_macAddr.focus();
		return false;
  	}
   	for (var i=0; i<str.length; i++) {
     		if ( (str.charAt(i) >= '0' && str.charAt(i) <= '9') ||
			(str.charAt(i) >= 'a' && str.charAt(i) <= 'f') ||
			(str.charAt(i) >= 'A' && str.charAt(i) <= 'F') )
			continue;
		alert("Invalid MAC address. It should be in hex number (0-9 or a-f).");
		form.wan_macAddr.value = form.wan_macAddr.defaultValue;
		form.wan_macAddr.focus();
		return false;
   	}  	
   }

   return true;
}
/*==============================================================================*/
/*   wlbasic.asp */
function enableWLAN(form, wlan_id)
{
  if (form.elements["mode"+wlan_id].selectedIndex!=1) {
  	disableTextField(form.elements["type"+wlan_id]);
  	if(form.elements["showMac"+wlan_id]!= null) {
  		if (form.elements["mode"+wlan_id].selectedIndex!=2)
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
  	if (form.elements["mode"+wlan_id].selectedIndex!=2)
  		enableTextField(form.elements["ssid"+wlan_id]);
  	else
  		disableTextField(form.elements["ssid"+wlan_id]);
  	enableTextField(form.elements["mode"+wlan_id]); 
  }  
  enableTextField(form.elements["band"+wlan_id]);

  if(form.elements["mode"+wlan_id].selectedIndex == 1 && opmode != 2) // client mode but not wisp
  	enableCheckBox(form.elements["wlanMacClone"+wlan_id]);
  else
  	disableCheckBox(form.elements["wlanMacClone"+wlan_id]);
}
function disableWLAN(form, wlan_id)
{
  disableTextField(form.elements["mode"+wlan_id]);
  disableTextField(form.elements["band"+wlan_id]);
  disableTextField(form.elements["type"+wlan_id]);
  disableTextField(form.elements["ssid"+wlan_id]);
  disableTextField(form.elements["chan"+wlan_id]);
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


function showChannel2G(form, wlan_id)
{
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

	defChanIdx=0;
	form.elements["chan"+wlan_id].length=0;

	idx=0;
	form.elements["chan"+wlan_id].options[0] = new Option("Auto", 0, false, false);
	if (0 == defaultChan[wlan_id]) {
		form.elements["chan"+wlan_id].selectedIndex = 0;
		defChanIdx = 0;
	}
	idx++;	
	for (chan=start; chan<=end; chan++, idx++) {
		form.elements["chan"+wlan_id].options[idx] = new Option(chan, chan, false, false);
		if (chan == defaultChan[wlan_id]) {
			form.elements["chan"+wlan_id].selectedIndex = idx;
			defChanIdx = idx;
		}
	}
	form.elements["chan"+wlan_id].length=idx;
	startChanIdx[wlan_id] = idx;
	if (defChanIdx==0)
		form.elements["chan"+wlan_id].selectedIndex = 0;
}

function updateChan(form, wlan_id)
{
  if (form.elements["band"+wlan_id].selectedIndex > 3)
     currentBand = 3;
  else {
     if (form.elements["band"+wlan_id].selectedIndex == 3)
     	currentBand = 2;
     else
	currentBand = 1;
  }

  if (lastBand[wlan_id] != currentBand) {
  	lastBand[wlan_id] = currentBand;
	if (currentBand == 3) {
		showChannel2G(form, wlan_id);
		showChannel5G(form, wlan_id);
	}
    	if (currentBand == 2) {
		startChanIdx[wlan_id]=0;
		showChannel5G(form, wlan_id);
	}
  	if (currentBand == 1)
		showChannel2G(form, wlan_id);
  }
}
function showBandAP(form, wlan_id)
{
  var idx=0;
 form.elements["band"+wlan_id].options[idx++] = new Option("2.4 GHz (B)", "0", false, false);
 form.elements["band"+wlan_id].options[idx++] = new Option("2.4 GHz (G)", "1", false, false);
 form.elements["band"+wlan_id].options[idx++] = new Option("2.4 GHz (B+G)", "2", false, false);

  if (RFType[wlan_id] != 7) {
	form.elements["band"+wlan_id].options[idx++] = new Option("5 GHz (A)", "3", false, false);
  }

 form.elements["band"+wlan_id].selectedIndex = bandIdx[wlan_id];
 form.elements["band"+wlan_id].length = idx;
}


function showBandClient(form, wlan_id)
{
  var idx=0;
 form.elements["band"+wlan_id].options[idx++] = new Option("2.4 GHz (B)", "0", false, false);
 form.elements["band"+wlan_id].options[idx++] = new Option("2.4 GHz (G)", "1", false, false);
 form.elements["band"+wlan_id].options[idx++] = new Option("2.4 GHz (B+G)", "2", false, false);

  if (RFType[wlan_id] != 7) {
	form.elements["band"+wlan_id].options[idx++] = new Option("5 GHz (A)", "3", false, false);
	form.elements["band"+wlan_id].options[idx++] = new Option("2.4 GHz (B) + 5 GHz (A)", "4", false, false);
	form.elements["band"+wlan_id].options[idx++] = new Option("2.4 GHz (G) + 5 GHz (A)", "5", false, false);
	form.elements["band"+wlan_id].options[idx++] = new Option("2.4 GHz (B+G) + 5 GHz (A)", "6", false, false);
  }
 form.elements["band"+wlan_id].selectedIndex = bandIdx[wlan_id];
 form.elements["band"+wlan_id].length = idx;
}

function showBand(form, wlan_id)
{
  if (APMode[wlan_id] != 1)
	showBandAP(form, wlan_id);
  else
 	showBandClient(form, wlan_id);
}

function updateMode(form, wlan_id)
{
  if (form.elements["mode"+wlan_id].selectedIndex != 1) {
  	if (APMode[wlan_id] == 1) {
		if (bandIdxAP[wlan_id] < 0)
			bandIdx[wlan_id]=2;	// set B+G as default
		else
			bandIdx[wlan_id]=bandIdxAP[wlan_id];
	}  	
  }
  else {
  	if (APMode[wlan_id] != 1) {
		if (bandIdxClient[wlan_id] < 0) {
 			if (RFType[wlan_id] == 7)
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
  updateIputState(form, wlan_id);
}

function updateBand(form, wlan_id)
{
  if (APMode[wlan_id] != 1)
	bandIdxAP[wlan_id] =form.elements["band"+wlan_id].selectedIndex;
  else
	bandIdxClient[wlan_id] =form.elements["band"+wlan_id].selectedIndex;

  updateChan(form, wlan_id);
}

function updateRepeaterState(form, wlan_id)
{   
  if(!form.elements["wlanDisabled"+wlan_id].checked &&  	
    ((form.elements["mode"+wlan_id].selectedIndex!=1) ||
       ((form.elements["mode"+wlan_id].selectedIndex==1) &&
     	(form.elements["type"+wlan_id].selectedIndex==0))) 
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
}

function updateType(form, wlan_id)
{
  updateChan(form, wlan_id);
  updateIputState(form, wlan_id);
  updateRepeaterState(form, wlan_id);
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
//<<------------------------------------------ for WPS	
  
  return true;
  }
function saveChanges_wpa(form, wlan_id)
{
  method = form.elements["method"+wlan_id] ;
  wpaAuth= form.elements["wpaAuth"+wlan_id] ;

// for WPS --------------------------------------->>
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
//<<-------------------------------------------- for WPS
  
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
  if (form.elements["defaultTxKeyId"+wlan_id].selectedIndex==idx && str.length==0) {
	alert('The encryption key you selected as the \'Tx Default Key\' cannot be blank.');
	return 0;
  }
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

function setDefaultKeyValue(form, wlan_id)
{
  if (form.elements["length"+wlan_id].selectedIndex == 0) {
	if ( form.elements["format"+wlan_id].selectedIndex == 0) {
		form.elements["key1"+wlan_id].maxLength = 5;
		form.elements["key2"+wlan_id].maxLength = 5;
		form.elements["key3"+wlan_id].maxLength = 5;
		form.elements["key4"+wlan_id].maxLength = 5;
		form.elements["key1"+wlan_id].value = "*****";
		form.elements["key2"+wlan_id].value = "*****";
		form.elements["key3"+wlan_id].value = "*****";
		form.elements["key4"+wlan_id].value = "*****";
	}
	else {
		form.elements["key1"+wlan_id].maxLength = 10;
		form.elements["key2"+wlan_id].maxLength = 10;
		form.elements["key3"+wlan_id].maxLength = 10;
		form.elements["key4"+wlan_id].maxLength = 10;
		form.elements["key1"+wlan_id].value = "**********";
		form.elements["key2"+wlan_id].value = "**********";
		form.elements["key3"+wlan_id].value = "**********";
		form.elements["key4"+wlan_id].value = "**********";

	}
  }
  else {
  	if ( form.elements["format"+wlan_id].selectedIndex == 0) {
		form.elements["key1"+wlan_id].maxLength = 13;
		form.elements["key2"+wlan_id].maxLength = 13;
		form.elements["key3"+wlan_id].maxLength = 13;
		form.elements["key4"+wlan_id].maxLength = 13;
		form.elements["key1"+wlan_id].value = "*************";
		form.elements["key2"+wlan_id].value = "*************";
		form.elements["key3"+wlan_id].value = "*************";
		form.elements["key4"+wlan_id].value = "*************";

	}
	else {
		form.elements["key1"+wlan_id].maxLength = 26;
		form.elements["key2"+wlan_id].maxLength = 26;
		form.elements["key3"+wlan_id].maxLength = 26;
		form.elements["key4"+wlan_id].maxLength = 26;
		form.elements["key1"+wlan_id].value ="**************************";
		form.elements["key2"+wlan_id].value ="**************************";
		form.elements["key3"+wlan_id].value ="**************************";
		form.elements["key4"+wlan_id].value ="**************************";
	}
  }

// for WPS ---------------------------------------->>
  wps_wep_key1_old =  form.elements["key1"+wlan_id].value
  wps_wep_key2_old =  form.elements["key1"+wlan_id].value
  wps_wep_key3_old =  form.elements["key1"+wlan_id].value
  wps_wep_key4_old =  form.elements["key1"+wlan_id].value
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

// for WPS --------------------------------------------------->>
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
		wps_wep_key1_old != form.elements["key1"+wlan_id].value) {
		if (!confirm(wps_warn3)) {
			form.elements["key1"+wlan_id].value = wps_wep_key1_old;
			form.elements["key1"+wlan_id].focus();
			return false;
		} 
		else
			clear_registrar.value = 1;		
	}
	if (clear_registrar.value == 0 &&
		wps_wep_key2_old != form.elements["key2"+wlan_id].value) {
		if (!confirm(wps_warn3)) {
			form.elements["key2"+wlan_id].value = wps_wep_key2_old;
			form.elements["key2"+wlan_id].focus();
			return false;
		} 
		else
			clear_registrar.value = 1;		
	}
	if (clear_registrar.value == 0 &&
		wps_wep_key3_old != form.elements["key3"+wlan_id].value) {
		if (!confirm(wps_warn3)) {
			form.elements["key3"+wlan_id].value = wps_wep_key3_old;
			form.elements["key3"+wlan_id].focus();
			return false;
		} 
		else
			clear_registrar.value = 1;		
	}
	if (clear_registrar.value == 0 &&
		wps_wep_key4_old != form.elements["key4"+wlan_id].value) {
		if (!confirm(wps_warn3)) {
			form.elements["key4"+wlan_id].value = wps_wep_key4_old;
			form.elements["key4"+wlan_id].focus();
			return false;
		} 
		else
			clear_registrar.value = 1;		
	}	
  }
//<<---------------------------------------------------- for WPS

  return true;
}



function lengthClick(form, wlan_id)
{
  updateFormat(form, wlan_id);
}

///////////////////////////////////////////////////////////////////////
//ntp.asp and wizard-ntp.asp
var ntp_zone_index=4;

function ntp_entry(name, value) { 
	this.name = name ;
	this.value = value ;
} 

var ntp_zone_array=new Array(65);
ntp_zone_array[0]=new ntp_entry("(GMT-12:00)Eniwetok, Kwajalein","12 1");
ntp_zone_array[1]=new ntp_entry("(GMT-11:00)Midway Island, Samoa","11 1");
ntp_zone_array[2]=new ntp_entry("(GMT-10:00)Hawaii", "10 1");
ntp_zone_array[3]=new ntp_entry("(GMT-09:00)Alaska", "9 1");
ntp_zone_array[4]=new ntp_entry("(GMT-08:00)Pacific Time (US & Canada); Tijuana", "8 1");
ntp_zone_array[5]=new ntp_entry("(GMT-07:00)Arizona", "7 1");
ntp_zone_array[6]=new ntp_entry("(GMT-07:00)Mountain Time (US & Canada)", "7 2");
ntp_zone_array[7]=new ntp_entry("(GMT-06:00)Central Time (US & Canada)", "6 1");
ntp_zone_array[8]=new ntp_entry("(GMT-06:00)Mexico City, Tegucigalpa", "6 2");
ntp_zone_array[9]=new ntp_entry("(GMT-06:00)Saskatchewan", "6 3");
ntp_zone_array[10]=new ntp_entry("(GMT-05:00)Bogota, Lima, Quito", "5 1");
ntp_zone_array[11]=new ntp_entry("(GMT-05:00)Eastern Time (US & Canada)", "5 2");
ntp_zone_array[12]=new ntp_entry("(GMT-05:00)Indiana (East)", "5 3");
ntp_zone_array[13]=new ntp_entry("(GMT-04:00)Atlantic Time (Canada)", "4 1");
ntp_zone_array[14]=new ntp_entry("(GMT-04:00)Caracas, La Paz", "4 2");
ntp_zone_array[15]=new ntp_entry("(GMT-04:00)Santiago", "4 3");
ntp_zone_array[16]=new ntp_entry("(GMT-03:30)Newfoundland", "3 1");
ntp_zone_array[17]=new ntp_entry("(GMT-03:00)Brasilia", "3 2");
ntp_zone_array[18]=new ntp_entry("(GMT-03:00)Buenos Aires, Georgetown", "3 3");
ntp_zone_array[19]=new ntp_entry("(GMT-02:00)Mid-Atlantic", "2 1");
ntp_zone_array[20]=new ntp_entry("(GMT-01:00)Azores, Cape Verde Is.", "1 1");
ntp_zone_array[21]=new ntp_entry("(GMT)Casablanca, Monrovia", "0 1");
ntp_zone_array[22]=new ntp_entry("(GMT)Greenwich Mean Time: Dublin, Edinburgh, Lisbon, London", "0 2");
ntp_zone_array[23]=new ntp_entry("(GMT+01:00)Amsterdam, Berlin, Bern, Rome, Stockholm, Vienna", "-1 1");
ntp_zone_array[24]=new ntp_entry("(GMT+01:00)Belgrade, Bratislava, Budapest, Ljubljana, Prague", "-1 2");
ntp_zone_array[25]=new ntp_entry("(GMT+01:00)Barcelona, Madrid", "-1 3");
ntp_zone_array[26]=new ntp_entry("(GMT+01:00)Brussels, Copenhagen, Madrid, Paris, Vilnius", "-1 4");
ntp_zone_array[27]=new ntp_entry("(GMT+01:00)Paris", "-1 5");
ntp_zone_array[28]=new ntp_entry("(GMT+01:00)Sarajevo, Skopje, Sofija, Warsaw, Zagreb", "-1 6");
ntp_zone_array[29]=new ntp_entry("(GMT+02:00)Athens, Istanbul, Minsk", "-2 1");
ntp_zone_array[30]=new ntp_entry("(GMT+02:00)Bucharest", "-2 2");
ntp_zone_array[31]=new ntp_entry("(GMT+02:00)Cairo", "-2 3");
ntp_zone_array[32]=new ntp_entry("(GMT+02:00)Harare, Pretoria", "-2 4");
ntp_zone_array[33]=new ntp_entry("(GMT+02:00)Helsinki, Riga, Tallinn", "-2 5");
ntp_zone_array[34]=new ntp_entry("(GMT+02:00)Jerusalem", "-2 6");
ntp_zone_array[35]=new ntp_entry("(GMT+03:00)Baghdad, Kuwait, Riyadh", "-3 1");
ntp_zone_array[36]=new ntp_entry("(GMT+03:00)Moscow, St. Petersburg, Volgograd", "-3 2");
ntp_zone_array[37]=new ntp_entry("(GMT+03:00)Mairobi", "-3 3");
ntp_zone_array[38]=new ntp_entry("(GMT+03:30)Tehran", "-3 4");
ntp_zone_array[39]=new ntp_entry("(GMT+04:00)Abu Dhabi, Muscat", "-4 1");
ntp_zone_array[40]=new ntp_entry("(GMT+04:00)Baku, Tbilisi", "-4 2");
ntp_zone_array[41]=new ntp_entry("(GMT+04:30)Kabul", "-4 3");
ntp_zone_array[42]=new ntp_entry("(GMT+05:00)Ekaterinburg", "-5 1");
ntp_zone_array[43]=new ntp_entry("(GMT+05:00)Islamabad, Karachi, Tashkent", "-5 2");
ntp_zone_array[44]=new ntp_entry("(GMT+05:30)Bombay, Calcutta, Madras, New Delhi", "-5 3");
ntp_zone_array[45]=new ntp_entry("(GMT+06:00)Astana, Almaty, Dhaka", "-6 1");
ntp_zone_array[46]=new ntp_entry("(GMT+06:00)Colombo", "-6 2");
ntp_zone_array[47]=new ntp_entry("(GMT+07:00)Bangkok, Hanoi, Jakarta", "-7 1");
ntp_zone_array[48]=new ntp_entry("(GMT+08:00)Beijing, Chongqing, Hong Kong, Urumqi", "-8 1");
ntp_zone_array[49]=new ntp_entry("(GMT+08:00)Perth", "-8 2");
ntp_zone_array[50]=new ntp_entry("(GMT+08:00)Singapore", "-8 3");
ntp_zone_array[51]=new ntp_entry("(GMT+08:00)Taipei", "-8 4");
ntp_zone_array[52]=new ntp_entry("(GMT+09:00)Osaka, Sapporo, Tokyo", "-9 1");
ntp_zone_array[53]=new ntp_entry("(GMT+09:00)Seoul", "-9 2");
ntp_zone_array[54]=new ntp_entry("(GMT+09:00)Yakutsk", "-9 3");
ntp_zone_array[55]=new ntp_entry("(GMT+09:30)Adelaide", "-9 4");
ntp_zone_array[56]=new ntp_entry("(GMT+09:30)Darwin", "-9 5");
ntp_zone_array[57]=new ntp_entry("(GMT+10:00)Brisbane", "-10 1");
ntp_zone_array[58]=new ntp_entry("(GMT+10:00)Canberra, Melbourne, Sydney", "-10 2");
ntp_zone_array[59]=new ntp_entry("(GMT+10:00)Guam, Port Moresby", "-10 3");
ntp_zone_array[60]=new ntp_entry("(GMT+10:00)Hobart", "-10 4");
ntp_zone_array[61]=new ntp_entry("(GMT+10:00)Vladivostok", "-10 5");
ntp_zone_array[62]=new ntp_entry("(GMT+11:00)Magadan, Solomon Is., New Caledonia", "-11 1");
ntp_zone_array[63]=new ntp_entry("(GMT+12:00)Auckland, Wllington", "-12 1");
ntp_zone_array[64]=new ntp_entry("(GMT+12:00)Fiji, Kamchatka, Marshall Is.", "-12 2");

function setTimeZone(field, value){
    field.selectedIndex = 4 ;
    for(i=0 ;i < field.options.length ; i++){
    	if(field.options[i].value == value){
		field.options[i].selected = true;
		break;
}
}

}

function setNtpServer(field, ntpServer){
    field.selectedIndex = 0 ;
    for(i=0 ;i < field.options.length ; i++){
    	if(field.options[i].value == ntpServer){
		field.options[i].selected = true;
		break;
	}
    }
}
function updateState_ntp(form)
{
	if(form.enabled.checked){
		enableTextField(form.timeZone);
		enableTextField(form.ntpServerIp1);
		if(form.ntpServerIp2 != null)
			enableTextField(form.ntpServerIp2);
	}
	else{
		disableTextField(form.timeZone);
		disableTextField(form.ntpServerIp1);
		if(form.ntpServerIp2 != null)
			disableTextField(form.ntpServerIp2);
	}
}

function saveChanges_ntp(form)
{
	if(form.ntpServerIp2.value != ""){
		if ( checkIpAddr(form.ntpServerIp2, 'Invalid IP address') == false )
		    return false;
	}
	else
		form.ntpServerIp2.value = "0.0.0.0" ;
	return true;
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
