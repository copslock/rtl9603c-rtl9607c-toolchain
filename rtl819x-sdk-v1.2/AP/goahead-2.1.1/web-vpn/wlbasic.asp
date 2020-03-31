<html>
<! Copyright (c) Realtek Semiconductor Corp., 2003. All Rights Reserved. ->
<head>
<meta http-equiv="Content-Type" content="text/html">
<title>WLAN Basic Settings</title>
<script type="text/javascript" src="common.js"> </script>
<style>
.on {display:on}
.off {display:none}
</style>
<SCRIPT>
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
var wlan_idx= <% write(getIndex("wlan_idx")); %> ;
var opmode=<% write(getIndex("opMode")); %> ;
var WiFiTest=<% write(getIndex("WiFiTest")); %> ;
lastBand[wlan_idx] = 0;
bandIdxAP[wlan_idx] = -1;
bandIdxClient[wlan_idx] = -1;
startChanIdx[wlan_idx] = 0;
disableSSID[wlan_idx] = 0;

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
  if (!form.elements["wlanDisabled"+wlan_idx].checked 
  && (form.elements["mode"+wlan_idx].selectedIndex==0 ||
  	form.elements["mode"+wlan_idx].selectedIndex==3))
	openWindow(url, 'showWirelessClient', 620, 420 );
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
}

function updateMode_basic(form, wlan_id)
{
	updateRepeaterState(form, wlan_id);
	updateMode(form, wlan_id);
}

function updateType_basic(form, wlan_id)
{
	updateRepeaterState(form, wlan_id);
	updateType(form, wlan_id);
}

</SCRIPT>
</head>

<body>
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
      <td width="74%"><font size=2><select size="1" name="band<% write(getIndex("wlan_idx")); %>" onChange="updateBand(document.wlanSetup, wlan_idx)">

     <SCRIPT>
 	<%  val = getIndex("RFType");
	    write ("RFType[wlan_idx]=" + val + ";\n");
	    val = getIndex("wlanMode");
	    write ("\tAPMode[wlan_idx]=" + val + ";\n");
   	    val = getIndex("band");
	    if (val > 0) val = val-1;
    	    write ("\tbandIdx[wlan_idx]=" + val + ";\n");
  	%>
	if (APMode[wlan_idx]!=1)
		bandIdxAP[wlan_idx]=bandIdx[wlan_idx];
	else
		bandIdxClient[wlan_idx]=bandIdx[wlan_idx];
	showBand(document.wlanSetup, wlan_idx);
     </SCRIPT>

	 </select>
      </td>
  </tr>
  <tr>
      <td width="26%"><font size=2><b>Mode:</b></td>
      <td width="74%"><font size=2><select size="1" name="mode<% write(getIndex("wlan_idx")); %>"	onChange="updateMode_basic(document.wlanSetup, wlan_idx)">
	<%  val = getIndex("wlanMode");
 	  if ( val == 0 ) {
      	  	write( "<option selected value=\"0\">AP</option>" );
   	  	write( "<option value=\"1\">Client</option>" );
   	  	write( "<option value=\"2\">WDS</option>" );
   	  	write( "<option value=\"3\">AP+WDS</option>" );
      	  }
	  if ( val == 1 ) {
     	  	write( "<option value=\"0\">AP</option>" );
   	  	write( "<option selected value=\"1\">Client </option>" );
 	  	write( "<option value=\"2\">WDS</option>" );
   	  	write( "<option value=\"3\">AP+WDS</option>" );
   	  }	
	  if ( val == 2 ) {
     	  	write( "<option value=\"0\">AP</option>" );
   	  	write( "<option value=\"1\">Client </option>" );
 	  	write( "<option selected value=\"2\">WDS</option>" );
   	  	write( "<option value=\"3\">AP+WDS</option>" );   	  	
   	  }
   	  if ( val == 3 ) {
     	  	write( "<option value=\"0\">AP</option>" );
   	  	write( "<option value=\"1\">Client </option>" );
 	  	write( "<option value=\"2\">WDS</option>" );
   	  	write( "<option selected value=\"3\">AP+WDS</option>" );
   	  } %>   	  	
	 </select>
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
      	  } %>
	 </select>
      </td>
  </tr>
 
  <tr>
      <td width="26%"><font size=2><b>SSID:
      <SCRIPT>
 	<%  autoconf = getIndex("autoCfgEnabled");
 	    keyinstall = getIndex("autoCfgKeyInstall");
 	    if (autoconf) {
 	    	if (keyinstall) {
	    		mode = getIndex("autoCfgWlanMode")
 	    		if (mode == getIndex("wlanMode")) {
 	    			write("document.write(\"(EasyConfig)\");");
 	    			write("disableSSID[wlan_idx]=1;");
 	    		}
 	    	}
 	    }
	%>
    </SCRIPT>

      </b></td>
      <td width="74%"><font size=2><input type="text" name="ssid<% write(getIndex("wlan_idx")); %>" size="33" maxlength="32" value="<% getInfo("ssid"); %>">
      </td>
  </tr>
  <tr>
      <td width="26%"><font size=2><b>Channel Number:</b></td>
      <td width="74%"><font size=2><select size="1" name="chan<% write(getIndex("wlan_idx")); %>"> </select></td>
    <SCRIPT>
 	<%  regDomain = getIndex("regDomain");
	    defaultChan = getIndex("channel");
	    write ("regDomain[wlan_idx]=" + regDomain + ";\n");
  	    write ("\tdefaultChan[wlan_idx]=" + defaultChan + ";\n");
	%>
	updateChan(document.wlanSetup, wlan_idx);
    </SCRIPT>
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
     <input type="reset" value="Reset" name="reset">

     <input type="hidden" name="basicrates<% write(getIndex("wlan_idx")); %>" value=0>
     <input type="hidden" name="operrates<% write(getIndex("wlan_idx")); %>" value=0>
  </tr>
  <script>
   <%
    band = getIndex("band");
    write ("usedBand[wlan_idx]=" + band + ";\n");
   %>
   updateState(document.wlanSetup, wlan_idx);
  </script>
</form>
</table>

</blockquote>
</body>

</html>
