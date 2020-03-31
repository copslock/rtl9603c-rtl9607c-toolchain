<html>
<! Copyright (c) Realtek Semiconductor Corp., 2003. All Rights Reserved. ->
<head>
<meta http-equiv="Content-Type" content="text/html">
<title>WDS Settings</title>
<script type="text/javascript" src="util_ap.js"> </script>
<script>
function addClick()
{
  var str = document.formWlWdsAdd.mac.value;

  if (!document.formWlWdsAdd.wlanWdsEnabled.checked)
	return true;

  if ( str.length == 0)
  	return true;

  if ( str.length < 12) {
	alert("Input MAC address is not complete. It should be 12 digits in hex.");
	document.formWlWdsAdd.mac.focus();
	return false;
  }

  for (var i=0; i<str.length; i++) {
    if ( (str.charAt(i) >= '0' && str.charAt(i) <= '9') ||
			(str.charAt(i) >= 'a' && str.charAt(i) <= 'f') ||
			(str.charAt(i) >= 'A' && str.charAt(i) <= 'F') )
			continue;

	alert("Invalid MAC address. It should be in hex number (0-9 or a-f).");
	document.formWlWdsAdd.mac.focus();
	return false;
  }
  return true;
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

function disableDelButton()
{
	disableButton(document.formWlWdsDel.deleteSelWdsMac);
	disableButton(document.formWlWdsDel.deleteAllWdsMac);
}

function enableWds()
{
  enableTextField(document.formWlWdsAdd.mac);
  enableTextField(document.formWlWdsAdd.comment);
  enableButton(document.formWlWdsAdd.showWep);
  enableButton(document.formWlWdsAdd.showAp);
}

function disableWds()
{
  disableTextField(document.formWlWdsAdd.mac);
  disableTextField(document.formWlWdsAdd.comment);
  
  	disableButton(document.formWlWdsAdd.showWep);
  	disableButton(document.formWlWdsAdd.showAp);
}

function updateState()
{
  wlanDisabled = <% write(getIndex("wlanDisabled")); %> ;
  wlanMode = <% write(getIndex("wlanMode")); %>;
  var wdstxrate = document.getElementById("wds_txrate");
  var wds_txrate = document.formWlWdsAdd.txRate;
  if(wlanDisabled || wlanMode == 0 || wlanMode ==1){
	disableWds();
	disableTextField(document.formWlWdsAdd.wlanWdsEnabled);
	disableButton(document.formWlWdsAdd.reset);
	disableButton(document.formWlWdsAdd.addWdsMac);
	disableButton(document.formWlWdsDel.reset);
	disableWds();
	disableDelButton();
	wds_txrate.disabled = true;
	//wdstxrate.disabled = true;

  }else{
	  if (document.formWlWdsAdd.wlanWdsEnabled.checked){
	 	enableWds();
	 	wds_txrate.disabled = false;
		//wdstxrate.disabled = false;
	  }else{
	  	disableWds();
	  	wds_txrate.disabled = true;
		//wdstxrate.disabled = true;
	  }
}
}

function showApClick(url)
{
  if (document.formWlWdsAdd.wlanWdsEnabled.checked)
	openWindow(url, 'showWdsStatistics',620,440 );
}

function showWepClick(url)
{
  if (document.formWlWdsAdd.wlanWdsEnabled.checked)
 	{
		//openWindow(url, 'showWdsWep',620,340 );
		document.location.href = url;
	}
}

</script>
</head>
<body>
<blockquote>
<h2><font color="#0000FF">WDS Settings <% if (getIndex("wlan_num") > 1) write("-wlan"+(getIndex("wlan_idx")+1)); %></font></h2>
<table border=0 width="500" cellspacing=4 cellpadding=0>
<tr><font size=2>
 Wireless Distribution System uses wireless media to communicate with other
 APs, like the Ethernet does. To do this, you must set these APs in the same
 channel and set MAC address of other APs which you want to communicate with
 in the table and then enable the WDS.
</font></tr>

<form action=/goform/formWlWds method=POST name="formWlWdsAdd">
<tr><hr size=1 noshade align=top><br></tr>

<tr><font size=2><b>
   <input type="checkbox" name="wlanWdsEnabled" value="ON" <% if (getIndex("wlanWdsEnabled")) write("checked");
   %> onclick="updateState()">&nbsp;&nbsp;Enable WDS</b></tr>
   <!--
<tr><font size=2><b>Add WDS AP:&nbsp;&nbsp;&nbsp;&nbsp</b></tr>  
-->
<table width="100%" border="0" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
 <tr>
 <td align="left" width="13%" class=""><font size=2><b>MAC Address:</td>
 <td width="87%">
 <input type="text" id="mac" name="mac" size="15" maxlength="12" value=""> 
 </b>
 </td>
 </tr>
 <tr id="wds_txrate">
 <td width="13%"><font size=2><b>Data Rate:</b></td>
 <td width="87%"><font size=2><select size="1" name="txRate" onChange=""> 
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
	rate_mask = [15,1,1,1,1,2,2,2,2,2,2,2,2,4,4,4,4,4,4,4,4,8,8,8,8,8,8,8,8];
	rate_name =["Auto","1M","2M","5.5M","11M","6M","9M","12M","18M","24M","36M","48M","54M", "MCS0", "MCS1"
				, "MCS2", "MCS3", "MCS4", "MCS5", "MCS6", "MCS7", "MCS8", "MCS9", "MCS10", "MCS11", "MCS12", "MCS13", "MCS14", "MCS15"];
	mask=0;
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
	defidx=0;
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
	document.formWlWdsAdd.elements["txRate"].selectedIndex=defidx;
 </SCRIPT>
 </select>
 </td>
 </tr>  
 <tr>
 <td align="left" width="13%" class=""><font size=2><b>Comment:</b></td>
 <td width="87%">
 <input type="text" id="comment" name="comment" size="16" maxlength="20" value="">
 </font>
 </td>
 </tr>
 </table>


<p><input type="submit" value="Apply Changes" name="addWdsMac" onClick="return addClick()">&nbsp;&nbsp;
   <input type="reset" value="Reset" name="reset">&nbsp;&nbsp;
   <input type="hidden" value="/wlwds.asp" name="submit-url">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;

   <font size=2><input type="button" value="Set Security" name="showWep" onClick="showWepClick('/wlwdsenp.asp')">&nbsp;&nbsp
   
   <font size=2><input type="button" value="Show Statistics" name="showAp" onClick="showApClick('/wlwdstbl.asp')">&nbsp;&nbsp
</p></tr>
</form>
</table>

<form action=/goform/formWlWds method=POST name="formWlWdsDel">
  <table border=0 width=440>
  <tr><font size=2><b>Current WDS AP List:</b></font></tr>
  <% wlWdsList(); %>
  </table>
  <br>
  <input type="submit" value="Delete Selected" name="deleteSelWdsMac" onClick="return deleteClick()">&nbsp;&nbsp;
  <input type="submit" value="Delete All" name="deleteAllWdsMac" onClick="return deleteAllClick()">&nbsp;&nbsp;&nbsp;
  <input type="reset" value="Reset" name="reset">
  <input type="hidden" value="/wlwds.asp" name="submit-url">
 <script>
   	<% entryNum = getIndex("wlanWdsNum");
   	  if ( entryNum == 0 ) {
      	  	write( "disableDelButton();" );
       	  } %>
	updateState();
 </script>
</form>

</blockquote>
</body>
</html>
