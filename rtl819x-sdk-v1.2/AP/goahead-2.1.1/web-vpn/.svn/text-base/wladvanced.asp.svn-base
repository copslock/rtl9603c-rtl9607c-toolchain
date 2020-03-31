<html>
<! Copyright (c) Realtek Semiconductor Corp., 2003. All Rights Reserved. ->
<head>
<meta http-equiv="Content-Type" content="text/html">
<title>Wireless Advanced Setting</title>
<script type="text/javascript" src="common.js"> </script>
<% language=javascript %>

<SCRIPT>
function validateNum(str)
{
  for (var i=0; i<str.length; i++) {
   	if ( !(str.charAt(i) >='0' && str.charAt(i) <= '9')) {
		alert("Invalid value. It should be in decimal number (0-9).");
		return false;
  	}
  }
  return true;
}
function saveChanges()
{
  if ( validateNum(document.advanceSetup.fragThreshold.value) == 0 ) {
  	document.advanceSetup.fragThreshold.focus();
	return false;
  }
  num = parseInt(document.advanceSetup.fragThreshold.value);
  if (document.advanceSetup.fragThreshold.value == "" || num < 256 || num > 2346) {
  	alert('Invalid value of Fragment Threshold. Input value should be between 256-2346 in decimal.');
  	document.advanceSetup.fragThreshold.focus();
	return false;
  }

  if ( validateNum(document.advanceSetup.rtsThreshold.value) == 0 ) {
  	document.advanceSetup.rtsThreshold.focus();
	return false;
  }
  num = parseInt(document.advanceSetup.rtsThreshold.value);
  if (document.advanceSetup.rtsThreshold.value=="" || num > 2347) {
  	alert('Invalid value of RTS Threshold. Input value should be between 0-2347 in decimal.');
  	document.advanceSetup.rtsThreshold.focus();
	return false;
  }

  if ( validateNum(document.advanceSetup.beaconInterval.value) == 0 ) {
  	document.advanceSetup.beaconInterval.focus();
	return false;
  }
  num = parseInt(document.advanceSetup.beaconInterval.value);
  if (document.advanceSetup.beaconInterval.value=="" || num < 20 || num > 1024) {
  	alert('Invalid value of Beacon Interval. Input value should be between 20-1024 in decimal.');
  	document.advanceSetup.beaconInterval.focus();
	return false;

  }
  return true;
}

function checkTurboState()
{
	if (document.advanceSetup.txRate.selectedIndex == 0) 
		enableRadioGroup(document.advanceSetup.turbo);
	else		
		disableRadioGroup(document.advanceSetup.turbo);
}

</SCRIPT>
<blockquote>
<body>
<h2><font color="#0000FF">Wireless Advanced Settings <% if (getIndex("wlan_num") > 1) write("-wlan"+(getIndex("wlan_idx")+1)); %></font></h2>

<form action=/goform/formAdvanceSetup method=POST name="advanceSetup">
<table border=0 width="500" cellspacing=4 cellpadding=0>
  <tr><font size=2>
 These settings are only for more technically advanced users who have a sufficient
 knowledge about wireless LAN. These settings should not be changed unless you know
 what effect the changes will have on your Access Point.
  </tr>
  <tr><hr size=1 noshade align=top></tr>
    <tr>
      <td width="30%"><font size=2><b>Authentication Type:</b></td>
      <td width="70%"><font size=2>
   	  <input type="radio" name="authType" value="open" <% if (getIndex("authType")==0) write("checked"); %>>Open System&nbsp;&nbsp;
   	  <input type="radio" name="authType" value="shared" <% if (getIndex("authType")==1) write("checked"); %>>Shared Key&nbsp;&nbsp;
   	  <input type="radio" name="authType" value="both" <% if (getIndex("authType")==2) write("checked"); %>>Auto</td>
    </tr>
    <tr>
      <td width="30%"><font size=2><b>Fragment Threshold:</b></td>
      <td width="70%"><font size=2><input type="text" name="fragThreshold" size="10" maxlength="4" value=<% getInfo("fragThreshold"); %>>(256-2346)</td>
    </tr>
    <tr>
      <td width="30%"><font size=2><b>RTS Threshold:</b></td>
      <td width="70%"><font size=2><input type="text" name="rtsThreshold" size="10" maxlength="4" value=<% getInfo("rtsThreshold"); %>>(0-2347)</td>
    </tr>
    <tr>
      <td width="30%"><font size=2><b>Beacon Interval:</b></td>
      <td width="70%"><font size=2><input type="text" name="beaconInterval" size="10" maxlength="4" value=<% getInfo("beaconInterval"); %>> (20-1024 ms)</td>
    </tr>

    <tr>
      <td width="30%"><font size=2><b>Data Rate:</b></td>
      <td width="70%"><select size="1" name="txRate" onChange="checkTurboState()">
	<SCRIPT>
	<%
	    band = getIndex("band");
	    auto=getIndex("rateAdaptiveEnabled")
 	    txrate = getIndex("fixTxRate");
	    write ("band=" + band + "\n");
 	    write ("txrate=" + txrate + "\n");
 	    write ("auto=" + auto + "\n");
	%>

	rate_mask = [3,1,1,1,1,2,2,2,2,2,2,2,2];
	rate_name =["Auto","1M","2M","5.5M","11M","6M","9M","12M","18M","24M","36M","48M","54M"];
	mask=0;
	if (auto)
		txrate=0;
	if (band & 1)
		mask |= 1;
	if ((band&2) || (band&4))
		mask |= 2;
	defidx=0;
	for (idx=0, i=0; i<=12; i++) {
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
	document.advanceSetup.txRate.selectedIndex=defidx;

	</SCRIPT>
	</select>
     </td>
   </tr>
  
  <!-- for WiFi test, start --
    <tr>
      <td width="30%"><font size=2><b>Tx Operation Rate:</b></td>
      <td width="70%"><font size=2>
        <input type="checkbox" name="operRate1M" value="1M"<% if (getIndex("operRate") & 1) write("checked"); %>>1M&nbsp;&nbsp;&nbsp;
        <input type="checkbox" name="operRate2M" value="2M"<% if (getIndex("operRate") & 2) write("checked"); %>>2M&nbsp;&nbsp;
	<input type="checkbox" name="operRate5M" value="5M"<% if (getIndex("operRate") & 4) write("checked"); %>>5.5M&nbsp;&nbsp;
        <input type="checkbox" name="operRate11M" value="11M"<% if (getIndex("operRate") & 8) write("checked") ;%>>11M&nbsp;&nbsp;
	<input type="checkbox" name="operRate6M" value="6M"<% if (getIndex("operRate") & 16) write("checked"); %>>6M&nbsp;&nbsp;
        <input type="checkbox" name="operRate9M" value="9M"<% if (getIndex("operRate") & 32) write("checked") ;%>>9M&nbsp;&nbsp;
       <br>
        <input type="checkbox" name="operRate12M" value="12M"<% if (getIndex("operRate") & 64) write("checked"); %>>12M&nbsp;&nbsp;
        <input type="checkbox" name="operRate18M" value="18M"<% if (getIndex("operRate") & 128) write("checked"); %>>18M&nbsp;&nbsp;
	<input type="checkbox" name="operRate24M" value="24M"<% if (getIndex("operRate") & 256) write("checked"); %>>24M&nbsp;&nbsp;
        <input type="checkbox" name="operRate36M" value="36M"<% if (getIndex("operRate") & 512) write("checked") ;%>>36M&nbsp;&nbsp;
	<input type="checkbox" name="operRate48M" value="48M"<% if (getIndex("operRate") & 1024) write("checked"); %>>28M&nbsp;&nbsp;
        <input type="checkbox" name="operRate54M" value="54M"<% if (getIndex("operRate") & 2048) write("checked") ;%>>54M&nbsp;&nbsp; 
       </td>
    </tr>
    <tr>
      <td width="30%"><font size=2><b>Tx Basic Rate:</b></td>
      <td width="70%"><font size=2>
        <input type="checkbox" name="basicRate1M" value="1M"<% if (getIndex("basicRate") & 1) write("checked"); %>>1M&nbsp;&nbsp;&nbsp;
        <input type="checkbox" name="basicRate2M" value="2M"<% if (getIndex("basicRate") & 2) write("checked"); %>>2M&nbsp;&nbsp;
	<input type="checkbox" name="basicRate5M" value="5M"<% if (getIndex("basicRate") & 4) write("checked"); %>>5.5M&nbsp;&nbsp;
        <input type="checkbox" name="basicRate11M" value="11M"<% if (getIndex("basicRate") & 8) write("checked"); %>>11M&nbsp;&nbsp;       
      	<input type="checkbox" name="basicRate6M" value="6M"<% if (getIndex("basicRate") & 16) write("checked"); %>>6M&nbsp;&nbsp;
        <input type="checkbox" name="basicRate9M" value="9M"<% if (getIndex("basicRate") & 32) write("checked") ;%>>9M&nbsp;&nbsp;
       <br>
        <input type="checkbox" name="basicRate12M" value="12M"<% if (getIndex("basicRate") & 64) write("checked"); %>>12M&nbsp;&nbsp;
        <input type="checkbox" name="basicRate18M" value="18M"<% if (getIndex("basicRate") & 128) write("checked"); %>>18M&nbsp;&nbsp;
	<input type="checkbox" name="basicRate24M" value="24M"<% if (getIndex("basicRate") & 256) write("checked"); %>>24M&nbsp;&nbsp;
        <input type="checkbox" name="basicRate36M" value="36M"<% if (getIndex("basicRate") & 512) write("checked") ;%>>36M&nbsp;&nbsp;
	<input type="checkbox" name="basicRate48M" value="48M"<% if (getIndex("basicRate") & 1024) write("checked"); %>>28M&nbsp;&nbsp;
        <input type="checkbox" name="basicRate54M" value="54M"<% if (getIndex("basicRate") & 2048) write("checked") ;%>>54M&nbsp;&nbsp; 
       </td>        
    </tr>
    <tr>
      <td width="30%"><font size=2><b>DTIM Period:</b></td>
      <td width="70%"><font size=2><input type="text" name="dtimPeriod" size="5" maxlength="3" value=<% getInfo("dtimPeriod"); %>>(1-255)</td>
    </tr>

-- for WiFi test, end -->

    <tr>
      <td width="30%"><font size=2><b>Preamble Type:</b></td>
      <td width="70%"><font size=2>
      <input type="radio" value="long" name="preamble"<% if (getIndex("preamble")==0) write("checked"); %>>Long Preamble&nbsp;&nbsp;
      <input type="radio" name="preamble" value="short"<% if (getIndex("preamble")==1) write("checked"); %>>Short Preamble</td>
    </tr>
    <tr>
      <td width="30%"><font size=2><b>Broadcast SSID:</b></td>
      <td width="70%"><font size=2>
      <input type="radio" name="hiddenSSID" value="no"<% if (getIndex("hiddenSSID")==0) write("checked"); %>>Enabled&nbsp;&nbsp;
      <input type="radio" name="hiddenSSID" value="yes"<% if (getIndex("hiddenSSID")==1) write("checked"); %>>Disabled</td>
    </tr>
    <tr>
      <td width="30%"><font size=2><b>IAPP:</b></td>
      <td width="70%"><font size=2>
      <input type="radio" name="iapp" value="yes"<% if (getIndex("iappDisabled")==0) write("checked"); %>>Enabled&nbsp;&nbsp;
      <input type="radio" name="iapp" value="no"<% if (getIndex("iappDisabled")==1) write("checked"); %>>Disabled</td>
    </tr>
    <tr>
      <td width="30%"><font size=2><b>802.11g Protection:</b></td>
      <td width="70%"><font size=2>
      <input type="radio" name="11g_protection" value="yes"<% if (getIndex("protectionDisabled")==0) write("checked"); %>>Enabled&nbsp;&nbsp;
      <input type="radio" name="11g_protection" value="no"<% if (getIndex("protectionDisabled")==1) write("checked"); %>>Disabled</td>
    </tr> 
<!-- for WMM ------------------------>  
    <tr>
      <td width="30%"><font size=2><b>WMM:</b></td>
      <td width="70%"><font size=2>
      <input type="radio" name="wmm" value="on"<% if (getIndex("wmmEnabled")==1) write("checked"); %>>Enabled&nbsp;&nbsp;
      <input type="radio" name="wmm" value="off"<% if (getIndex("wmmEnabled")==0) write("checked"); %>>Disabled</td>
    </tr> 
   
    <tr>
      <td width="30%"><font size=2><b>RF Output Power:</b></td>
      <td width="70%"><font size=2>
      <input type="radio" name="RFPower" value="0"<% if (getIndex("RFPower")==0) write("checked"); %>>100%&nbsp;&nbsp;
      <input type="radio" name="RFPower" value="1"<% if (getIndex("RFPower")==1) write("checked"); %>>50%&nbsp;&nbsp;
      <input type="radio" name="RFPower" value="2"<% if (getIndex("RFPower")==2) write("checked"); %>>25%&nbsp;&nbsp;
	  <input type="radio" name="RFPower" value="3"<% if (getIndex("RFPower")==3) write("checked"); %>>10%&nbsp;&nbsp;
	  <input type="radio" name="RFPower" value="4"<% if (getIndex("RFPower")==4) write("checked"); %>>5%</td>
    </tr>      
    <tr>
      <td width="30%"><font size=2><b>Turbo Mode:</b></td>
      <td width="70%"><font size=2>
      <input type="radio" name="turbo" value="auto"<% if (getIndex("turboMode")==0) write("checked"); %>>Auto&nbsp;&nbsp;
      <input type="radio" name="turbo" value="always"<% if (getIndex("turboMode")==1) write("checked"); %>>Always&nbsp;&nbsp;
      <input type="radio" name="turbo" value="off"<% if (getIndex("turboMode")==2) write("checked"); %>>Off</td>
    </tr>
    <tr>
      <td width="30%"></td>
      <td width="70%"><font size=2>Note: "Always" may have compatibility issue. "Auto" will only work with Realtek product.</td>
    </tr>    
  <script>
  	checkTurboState();
 </script>

  </table>
  <p>
  <input type="submit" value="Apply Changes" name="save" onClick="return saveChanges()">&nbsp;&nbsp;
  <input type="reset" value="  Reset  " name="reset">&nbsp;&nbsp;&nbsp;
  <input type="hidden" value="/wladvanced.asp" name="submit-url">
  </p>
</form>
</blockquote>
</body>

</html>

