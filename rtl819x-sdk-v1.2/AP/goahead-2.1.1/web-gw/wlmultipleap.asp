<html>
<! Copyright (c) Realtek Semiconductor Corp., 2003. All Rights Reserved. ->
<head>
<meta http-equiv="Content-Type" content="text/html">
<title>Multiple AP</title>
<style>
.on {display:on}
.off {display:none}
</style>
<script type="text/javascript" src="util_gw.js"> </script>
<style>
.MainTd {
	font-family: "Arial", "Helvetica", "sans-serif";
	font-size: 8pt;
	vertical-align: middle;
	background-position: center;
}
</style>
<script language="JavaScript" type="text/javascript">
var wlan_idx= <% write(getIndex("wlan_idx")); %> ;
var WiFiTest=<% write(getIndex("WiFiTest")); %> ;
var val;
var bandIdx=new Array();
var virtual_bandIdx=new Array();
var virtual_wmm_enable=new Array();
var virtual_wlan_enable=new Array();
var virtual_access=new Array();
var aclist_index;
var mssid_num=<%write(getIndex("wlan_mssid_num"));%>;

<%     
	val = getIndex("band");
	if (val > 0) 
		val = val-1;
    	write ("bandIdx[wlan_idx]=" + val + ";\n");

	for (k=1; k<=getIndex("wlan_mssid_num"); k=k+1)
	{
		val = getVirtualIndex("band", k);
		if (val > 0) 
			val = val-1;
		write ("virtual_bandIdx["+k+"]=" + val + ";\n");

		val = getVirtualIndex("wmmEnabled", k);
		write ("virtual_wmm_enable["+k+"]=" + val + ";\n");
		
		val = getVirtualIndex("wlanDisabled", k);
		if (val) val = 0; 
		else val = 1;
		write ("virtual_wlan_enable["+k+"]=" + val + ";\n");				

		val = getVirtualIndex("wlanAccess", k);
		write ("virtual_access["+k+"]=" + val + ";\n");
	}
%>
	
var rate_mask = [15,1,1,1,1,2,2,2,2,2,2,2,2,4,4,4,4,4,4,4,4,8,8,8,8,8,8,8,8];
var rate_name =["Auto","1M","2M","5.5M","11M","6M","9M","12M","18M","24M","36M","48M","54M", "MCS0", "MCS1"
				, "MCS2", "MCS3", "MCS4", "MCS5", "MCS6", "MCS7", "MCS8", "MCS9", "MCS10", "MCS11", "MCS12", "MCS13", "MCS14", "MCS15"];

function DisplayTxRate(v_index, band, auto, txrate, rf_num)
{
	var mask, defidx, i, rate;
	
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
	document.MultipleAP.elements["TxRate"+v_index].selectedIndex=defidx;
}

function open_client_table(id)
{
	aclist_index = id;
	openWindow('/wlstatbl_vap.asp?id='+id, 'showWirelessClient', 700, 400);
}

function click_reset()
{
	for (i=1; i<=4; i++) {
		document.MultipleAP["wl_disable" + i].checked = (virtual_wlan_enable[i] ? true :  false);		
		UpdateVirtualStatus(document.MultipleAP, i);		
	}
}

function saveChanges(form, wlan_id)
{
	var idx;
 	for (idx=1; idx<=4; idx++) {
		if (form.elements["wl_disable"+idx].checked) {
			ssid = form.elements["wl_ssid"+idx];		
			if (ssid.value == "") {
				alert('SSID cannot be empty!');
				ssid.value = ssid.defaultValue;
				ssid.focus();
				return false;
			}	   	
		}
	}
}

function showtxrate_updated_virtual(form, wlan_id, original_wmm_enable)
{
  	var idx=0;
  	var i;
  	var txrate_str;
	var band, current_value, txrate_idx;
	var rf_num= <% write(getIndex("rf_used")); %> ;	

	txrate_idx = form.elements["TxRate"+wlan_id].selectedIndex;
	current_value = form.elements["TxRate"+wlan_id].options[txrate_idx].value;
	i = form.elements["wl_band"+wlan_id].selectedIndex;
	band = form.elements["wl_band"+wlan_id].options[i].value;
	form.elements["TxRate"+wlan_id].length=0;
	
 	form.elements["TxRate"+wlan_id].options[idx++] = new Option("Auto", "0", false, false);
 	if(band == 0 || band ==2 || band ==10){
 		form.elements["TxRate"+wlan_id].options[idx++] = new Option("1M", "1", false, false);
 		form.elements["TxRate"+wlan_id].options[idx++] = new Option("2M", "2", false, false);
 		form.elements["TxRate"+wlan_id].options[idx++] = new Option("5.5M", "3", false, false);
 		form.elements["TxRate"+wlan_id].options[idx++] = new Option("11M", "4", false, false);
	}
 	if(band ==9 || band ==10 || band ==1 || band ==2){
 		form.elements["TxRate"+wlan_id].options[idx++] = new Option("6M", "5", false, false);
	 	form.elements["TxRate"+wlan_id].options[idx++] = new Option("9M", "6", false, false);
 		form.elements["TxRate"+wlan_id].options[idx++] = new Option("12M", "7", false, false);
 		form.elements["TxRate"+wlan_id].options[idx++] = new Option("18M", "8", false, false);
 		form.elements["TxRate"+wlan_id].options[idx++] = new Option("24M", "9", false, false);
 		form.elements["TxRate"+wlan_id].options[idx++] = new Option("36M", "10", false, false);
 		form.elements["TxRate"+wlan_id].options[idx++] = new Option("48M", "11", false, false);
 		form.elements["TxRate"+wlan_id].options[idx++] = new Option("54M", "12", false, false);
	}
 	if(band ==7 || band ==9 || band ==10){
 		form.elements["TxRate"+wlan_id].options[idx++] = new Option("MCS0", "13", false, false);
 		form.elements["TxRate"+wlan_id].options[idx++] = new Option("MCS1", "14", false, false);
 		form.elements["TxRate"+wlan_id].options[idx++] = new Option("MCS2", "15", false, false);
 		form.elements["TxRate"+wlan_id].options[idx++] = new Option("MCS3", "16", false, false);
 		form.elements["TxRate"+wlan_id].options[idx++] = new Option("MCS4", "17", false, false);
 		form.elements["TxRate"+wlan_id].options[idx++] = new Option("MCS5", "18", false, false);
 		form.elements["TxRate"+wlan_id].options[idx++] = new Option("MCS6", "19", false, false);
 		form.elements["TxRate"+wlan_id].options[idx++] = new Option("MCS7", "20", false, false);
 		if(rf_num == 2)
 		{
 		form.elements["TxRate"+wlan_id].options[idx++] = new Option("MCS8", "21", false, false);
 		form.elements["TxRate"+wlan_id].options[idx++] = new Option("MCS9", "22", false, false);
 		form.elements["TxRate"+wlan_id].options[idx++] = new Option("MCS10", "23", false, false);
 		form.elements["TxRate"+wlan_id].options[idx++] = new Option("MCS11", "24", false, false);
 		form.elements["TxRate"+wlan_id].options[idx++] = new Option("MCS12", "25", false, false);
 		form.elements["TxRate"+wlan_id].options[idx++] = new Option("MCS13", "26", false, false);
 		form.elements["TxRate"+wlan_id].options[idx++] = new Option("MCS14", "27", false, false);
 		form.elements["TxRate"+wlan_id].options[idx++] = new Option("MCS15", "28", false, false);
	 	}

		form.elements["wl_wmm_capable"+wlan_id].selectedIndex = 1;
		disableTextField(form.elements["wl_wmm_capable"+wlan_id]);
	}
	else {
		if (original_wmm_enable)
			form.elements["wl_wmm_capable"+wlan_id].selectedIndex = 1;
		else
			form.elements["wl_wmm_capable"+wlan_id].selectedIndex = 0;

		if (form.elements["wl_band"+wlan_id].selectedIndex == 2 || 
			form.elements["wl_band"+wlan_id].selectedIndex >= 4) {
			form.elements["wl_wmm_capable"+wlan_id].selectedIndex = 1;		
			disableTextField(form.elements["wl_wmm_capable"+wlan_id]);
		}
		else 
			enableTextField(form.elements["wl_wmm_capable"+wlan_id]);	
	}
 	form.elements["TxRate"+wlan_id].length = idx;
 
 	for (i=0; i<idx; i++) {
 		txrate_str = form.elements["TxRate"+wlan_id].options[i].value;
 		if(current_value == txrate_str)
 			form.elements["TxRate"+wlan_id].selectedIndex = i;
		else
			form.elements["TxRate"+wlan_id].selectedIndex = 0;
 	}
}

function showBand_MultipleAP(form, wlan_id, band_root, index_id)
{
  var idx=0;
  var band_value=virtual_bandIdx[index_id];

  form.elements["wl_band"+index_id].options[idx++] = new Option("2.4 GHz (B)", "0", false, false);
  form.elements["wl_band"+index_id].options[idx++] = new Option("2.4 GHz (G)", "1", false, false);
  form.elements["wl_band"+index_id].options[idx++] = new Option("2.4 GHz (N)", "7", false, false); 
  form.elements["wl_band"+index_id].options[idx++] = new Option("2.4 GHz (B+G)", "2", false, false);
  form.elements["wl_band"+index_id].options[idx++] = new Option("2.4 GHz (G+N)", "9", false, false);
  form.elements["wl_band"+index_id].options[idx++] = new Option("2.4 GHz (B+G+N)", "10", false, false);

  if (band_value==0) // B
	form.elements["wl_band"+index_id].selectedIndex = 0;
  else if (band_value==1)  // G
	form.elements["wl_band"+index_id].selectedIndex = 1; 
  else if (band_value==7)  // N
	form.elements["wl_band"+index_id].selectedIndex = 2;
  else if (band_value==2)  // B+G
	form.elements["wl_band"+index_id].selectedIndex = 3;
  else if (band_value==9)  // G+N
	form.elements["wl_band"+index_id].selectedIndex = 4;
  else // B+G+N
	form.elements["wl_band"+index_id].selectedIndex = 5;
  
  form.elements["wl_band"+index_id].length = idx;
}

function enableVirtualWLAN(form, virtual_id)
{
	enableTextField(form.elements["wl_band"+virtual_id]);
	enableTextField(form.elements["wl_ssid"+virtual_id]);
	enableTextField(form.elements["TxRate"+virtual_id]);
	enableTextField(form.elements["wl_hide_ssid"+virtual_id]);

	if (form.elements["wl_band"+virtual_id].selectedIndex == 2 || 
			form.elements["wl_band"+virtual_id].selectedIndex >= 4) {
		form.elements["wl_wmm_capable"+virtual_id].selectedIndex = 1;		
		disableTextField(form.elements["wl_wmm_capable"+virtual_id]);
	}
	else 
		enableTextField(form.elements["wl_wmm_capable"+virtual_id]);
	enableTextField(form.elements["wl_access"+virtual_id]);		
	enableTextField(form.elements["aclient"+virtual_id]);
}

function disableVirtualWLAN(form, virtual_id)
{
	disableTextField(form.elements["wl_band"+virtual_id]);
	disableTextField(form.elements["wl_ssid"+virtual_id]);
	disableTextField(form.elements["TxRate"+virtual_id]);
	disableTextField(form.elements["wl_hide_ssid"+virtual_id]);
	disableTextField(form.elements["wl_wmm_capable"+virtual_id]);
	disableTextField(form.elements["wl_access"+virtual_id]);	
	disableTextField(form.elements["aclient"+virtual_id]);
}

function UpdateVirtualStatus(form, virtual_id)
{
	if (!form.elements["wl_disable"+virtual_id].checked)
		disableVirtualWLAN(form, virtual_id);
	else
		enableVirtualWLAN(form, virtual_id);
}

</script>
</head>

<body>
<blockquote>
<h2><font color="#0000FF">Multiple APs</font></h2>


<table border=0 width="700" cellspacing=0 cellpadding=0>
<tr><font size=2>
 This page shows and updates the wireless setting for multiple APs.
</tr>
<tr><hr size=1 noshade align=top></tr>
</table>
<form action=/goform/formWlanMultipleAP method=POST name="MultipleAP">
<table border='1' width="700"> 
<tr bgcolor=#7f7f7f>
   <td align="center"><font size=2><b>No.</b></td>
   <td align="center"><font size=2><b>Enable</b></td>
   <td align="center"><font size=2><b>Band</b></td>
   <td align="center"><font size=2><b>SSID</b></td>
   <td align="center"><font size=2><b>Data Rate</b></td>
   <td align="center"><font size=2><b>Broadcast SSID</b></td>
   <td align="center"><font size=2><b>WMM</b></td>
   <td align="center"><font size=2><b>Access</b></td>   
   <td align="center"><font size=2><b>Active Client List</b></td>
</tr>

<tr>
	<td height="25" bgcolor="#FFFFFF" align="center" class="MainTd">AP1</td>
	<td height="25" align="center" bgcolor="#FFFFFF">
		<input type="checkbox" name="wl_disable1" value="ON" <% if (!getVirtualIndex("wlanDisabled", 1)) write("checked"); %> 
			onClick="UpdateVirtualStatus(document.MultipleAP, 1)" >
	</td>
	<td height="25" align="center" bgcolor="#FFFFFF" class="MainTd">
		<select name=wl_band1 onChange="showtxrate_updated_virtual(document.MultipleAP, 1, virtual_wmm_enable[1])">
			<script>
				showBand_MultipleAP(document.MultipleAP, wlan_idx, bandIdx[wlan_idx], 1);
     			</script>
		</select>
	</td>
	<td height="25" align="center" bgcolor="#FFFFFF" class="MainTd">
		<input type="text" name="wl_ssid1" size="10" maxlength="32" value="<% getVirtualInfo("ssid", 1); %>">
	</td>
	<td height="25" align="center" bgcolor="#FFFFFF" class="MainTd">
		<select name="TxRate1">
			<script>
    				<%
	    				band = getVirtualIndex("band", 1);
	    				auto=getVirtualIndex("rateAdaptiveEnabled", 1);
 	    				txrate = getVirtualIndex("fixTxRate", 1);
 	    				rf_num = getIndex("rf_used");						
	    				write ("band=" + band + "\n");
 	    				write ("txrate=" + txrate + "\n");
 	    				write ("auto=" + auto + "\n");
 	    				write ("rf_num=" + rf_num + "\n");						
				%>
				DisplayTxRate(1, band, auto, txrate, rf_num);
			</script>
		</select>
	</td>
	<td height="25" align="center" bgcolor="#FFFFFF" class="MainTd">
		<select name=wl_hide_ssid1>
			<option value="1">Disabled</option>
			<option value="0">Enabled</option>
			<script>
				<%
					hiddenSSID = getVirtualIndex("hiddenSSID", 1);
					write ("hiddenSSID=" + hiddenSSID + "\n");
				%>
				if (hiddenSSID)
					document.MultipleAP.elements["wl_hide_ssid1"].selectedIndex=0;
				else
					document.MultipleAP.elements["wl_hide_ssid1"].selectedIndex=1;
			</script>
		</select>
	</td>
	
	<td height="25" align="center" bgcolor="#FFFFFF" class="MainTd">
		<select name=wl_wmm_capable1>
			<option value="0">Disabled</option>
			<option value="1">Enabled</option>
			<script>
				if (virtual_wmm_enable[1])
					document.MultipleAP.elements["wl_wmm_capable1"].selectedIndex=1;
				else
					document.MultipleAP.elements["wl_wmm_capable1"].selectedIndex=0;
			</script>
		</select>
	</td>

	<td height="25" align="center" bgcolor="#FFFFFF" class="MainTd">
		<select name=wl_access1>
			<option value="0">LAN+WAN</option>
			<option value="1">WAN</option>
			<script>
				if (virtual_access[1])
					document.MultipleAP.elements["wl_access1"].selectedIndex=1;
				else
					document.MultipleAP.elements["wl_access1"].selectedIndex=0;
			</script>
		</select>
	</td>
	
	<td height="25" align="center" bgcolor="#FFFFFF" class="MainTd">
		<input type="hidden" value="1" name="vap">	
		<input type="button" value="Show" name="aclient1" onClick="open_client_table(1);">
      </td>
</tr>
<script>
  if (mssid_num == 1) {
	document.write("</table>\n");
	document.write("<span id = \"hide_div\" class = \"off\">\n");
	document.write("<table border=\"1\" width=700>\n");
  }
</script>
<tr>
	<td height="25" bgcolor="#FFFFFF" align="center" class="MainTd">AP2</td>
	<td height="25" align="center" bgcolor="#FFFFFF">
		<input type="checkbox" name="wl_disable2" value="ON" <% if (!getVirtualIndex("wlanDisabled", 2)) write("checked"); %> 
			onClick="UpdateVirtualStatus(document.MultipleAP, 2)" >
	</td>
	<td height="25" align="center" bgcolor="#FFFFFF" class="MainTd">
		<select name=wl_band2 onChange="showtxrate_updated_virtual(document.MultipleAP, 2, virtual_wmm_enable[2])">
			<script>
				showBand_MultipleAP(document.MultipleAP, wlan_idx, bandIdx[wlan_idx], 2);
     			</script>
		</select>
	</td>
	<td height="25" align="center" bgcolor="#FFFFFF" class="MainTd">
		<input type="text" name="wl_ssid2" size="10" maxlength="32" value="<% getVirtualInfo("ssid", 2); %>">
	</td>
	<td height="25" align="center" bgcolor="#FFFFFF" class="MainTd">
		<select name="TxRate2">
			<script>
    				<%
	    				band = getVirtualIndex("band", 2);
	    				auto=getVirtualIndex("rateAdaptiveEnabled", 2);
 	    				txrate = getVirtualIndex("fixTxRate", 2);
 	    				rf_num = getIndex("rf_used");								
	    				write ("band=" + band + "\n");
 	    				write ("txrate=" + txrate + "\n");
 	    				write ("auto=" + auto + "\n");
 	    				write ("rf_num=" + rf_num + "\n");						
				%>
				DisplayTxRate(2, band, auto, txrate, rf_num);
			</script>
		</select>
	</td>
	<td height="25" align="center" bgcolor="#FFFFFF" class="MainTd">
		<select name=wl_hide_ssid2>
			<option value="1">Disabled</option>
			<option value="0">Enabled</option>
			<script>
				<%
					hiddenSSID = getVirtualIndex("hiddenSSID", 2);
					write ("hiddenSSID=" + hiddenSSID + "\n");
				%>
				if (hiddenSSID)
					document.MultipleAP.elements["wl_hide_ssid2"].selectedIndex=0;
				else
					document.MultipleAP.elements["wl_hide_ssid2"].selectedIndex=1;
			</script>
		</select>
	</td>
	<td height="25" align="center" bgcolor="#FFFFFF" class="MainTd">
		<select name=wl_wmm_capable2>
			<option value="0">Disabled</option>
			<option value="1">Enabled</option>
			<script>
				if (virtual_wmm_enable[2])
					document.MultipleAP.elements["wl_wmm_capable2"].selectedIndex=1;
				else
					document.MultipleAP.elements["wl_wmm_capable2"].selectedIndex=0;
			</script>
		</select>
	</td>
	<td height="25" align="center" bgcolor="#FFFFFF" class="MainTd">
		<select name=wl_access2>
			<option value="0">LAN+WAN</option>
			<option value="1">WAN</option>
			<script>
				if (virtual_access[2])
					document.MultipleAP.elements["wl_access2"].selectedIndex=1;
				else
					document.MultipleAP.elements["wl_access2"].selectedIndex=0;
			</script>
		</select>
	</td>	
	<td height="25" align="center" bgcolor="#FFFFFF" class="MainTd">
		<input type="button" value="Show" name="aclient2" onClick="open_client_table(2);">
      </td>
</tr>
<tr>
	<td height="25" bgcolor="#FFFFFF" align="center" class="MainTd">AP3</td>
	<td height="25" align="center" bgcolor="#FFFFFF">
		<input type="checkbox" name="wl_disable3" value="ON" <% if (!getVirtualIndex("wlanDisabled", 3)) write("checked"); %> 
			onClick="UpdateVirtualStatus(document.MultipleAP, 3)" >
	</td>
	<td height="25" align="center" bgcolor="#FFFFFF" class="MainTd">
		<select name=wl_band3 onChange="showtxrate_updated_virtual(document.MultipleAP, 3, virtual_wmm_enable[3])">
			<script>
				showBand_MultipleAP(document.MultipleAP, wlan_idx, bandIdx[wlan_idx], 3);
     			</script>
		</select>
	</td>
	<td height="25" align="center" bgcolor="#FFFFFF" class="MainTd">
		<input type="text" name="wl_ssid3" size="10" maxlength="32" value="<% getVirtualInfo("ssid", 3); %>">
	</td>
	<td height="25" align="center" bgcolor="#FFFFFF" class="MainTd">
		<select name="TxRate3">
			<script>
    				<%
	    				band = getVirtualIndex("band", 3);
	    				auto=getVirtualIndex("rateAdaptiveEnabled", 3);
 	    				txrate = getVirtualIndex("fixTxRate", 3);
 	    				rf_num = getIndex("rf_used");						
	    				write ("band=" + band + "\n");
 	    				write ("txrate=" + txrate + "\n");
 	    				write ("auto=" + auto + "\n");
 	    				write ("rf_num=" + rf_num + "\n");							
				%>
				DisplayTxRate(3, band, auto, txrate, rf_num);
			</script>
		</select>
	</td>
	<td height="25" align="center" bgcolor="#FFFFFF" class="MainTd">
		<select name=wl_hide_ssid3>
			<option value="1">Disabled</option>
			<option value="0">Enabled</option>
			<script>
				<%
					hiddenSSID = getVirtualIndex("hiddenSSID", 3);
					write ("hiddenSSID=" + hiddenSSID + "\n");
				%>
				if (hiddenSSID)
					document.MultipleAP.elements["wl_hide_ssid3"].selectedIndex=0;
				else
					document.MultipleAP.elements["wl_hide_ssid3"].selectedIndex=1;
			</script>
		</select>
	</td>
	<td height="25" align="center" bgcolor="#FFFFFF" class="MainTd">
		<select name=wl_wmm_capable3>
			<option value="0">Disabled</option>
			<option value="1">Enabled</option>
			<script>
				if (virtual_wmm_enable[3])
					document.MultipleAP.elements["wl_wmm_capable3"].selectedIndex=1;
				else
					document.MultipleAP.elements["wl_wmm_capable3"].selectedIndex=0;
			</script>
		</select>
	</td>
	<td height="25" align="center" bgcolor="#FFFFFF" class="MainTd">
		<select name=wl_access3>
			<option value="0">LAN+WAN</option>
			<option value="1">WAN</option>
			<script>
				if (virtual_access[3])
					document.MultipleAP.elements["wl_access3"].selectedIndex=1;
				else
					document.MultipleAP.elements["wl_access3"].selectedIndex=0;
			</script>
		</select>
	</td>	
	<td height="25" align="center" bgcolor="#FFFFFF" class="MainTd">
		<input type="button" value="Show" name="aclient3" onClick="open_client_table(3);">
      </td>
</tr>

<tr>
	<td height="25" bgcolor="#FFFFFF" align="center" class="MainTd">AP4</td>
	<td height="25" align="center" bgcolor="#FFFFFF">
		<input type="checkbox" name="wl_disable4" value="ON" <% if (!getVirtualIndex("wlanDisabled", 4)) write("checked"); %> 
			onClick="UpdateVirtualStatus(document.MultipleAP, 4)" >
	</td>
	<td height="25" align="center" bgcolor="#FFFFFF" class="MainTd">
		<select name=wl_band4 onChange="showtxrate_updated_virtual(document.MultipleAP, 4, virtual_wmm_enable[4])">
			<script>
				showBand_MultipleAP(document.MultipleAP, wlan_idx, bandIdx[wlan_idx], 4);
     			</script>
		</select>
	</td>
	<td height="25" align="center" bgcolor="#FFFFFF" class="MainTd">
		<input type="text" name="wl_ssid4" size="10" maxlength="32" value="<% getVirtualInfo("ssid", 4); %>">
	</td>
	<td height="25" align="center" bgcolor="#FFFFFF" class="MainTd">
		<select name="TxRate4">
			<script>
    				<%
	    				band = getVirtualIndex("band", 4);
	    				auto=getVirtualIndex("rateAdaptiveEnabled", 4);
 	    				txrate = getVirtualIndex("fixTxRate", 4);
 	    				rf_num = getIndex("rf_used");						
	    				write ("band=" + band + "\n");
 	    				write ("txrate=" + txrate + "\n");
 	    				write ("auto=" + auto + "\n");
 	    				write ("rf_num=" + rf_num + "\n");						
				%>
				DisplayTxRate(4, band, auto, txrate, rf_num);
			</script>
		</select>
	</td>
	<td height="25" align="center" bgcolor="#FFFFFF" class="MainTd">
		<select name=wl_hide_ssid4>
			<option value="1">Disabled</option>
			<option value="0">Enabled</option>
			<script>
				<%
					hiddenSSID = getVirtualIndex("hiddenSSID", 4);
					write ("hiddenSSID=" + hiddenSSID + "\n");
				%>
				if (hiddenSSID)
					document.MultipleAP.elements["wl_hide_ssid4"].selectedIndex=0;
				else
					document.MultipleAP.elements["wl_hide_ssid4"].selectedIndex=1;
			</script>
		</select>
	</td>
	<td height="25" align="center" bgcolor="#FFFFFF" class="MainTd">
		<select name=wl_wmm_capable4>
			<option value="0">Disabled</option>
			<option value="1">Enabled</option>
			<script>
				if (virtual_wmm_enable[4])
					document.MultipleAP.elements["wl_wmm_capable4"].selectedIndex=1;
				else
					document.MultipleAP.elements["wl_wmm_capable4"].selectedIndex=0;
			</script>
		</select>
	</td>
	<td height="25" align="center" bgcolor="#FFFFFF" class="MainTd">
		<select name=wl_access4>
			<option value="0">LAN+WAN</option>
			<option value="1">WAN</option>
			<script>
				if (virtual_access[4])
					document.MultipleAP.elements["wl_access4"].selectedIndex=1;
				else
					document.MultipleAP.elements["wl_access4"].selectedIndex=0;
			</script>
		</select>
	</td>	
	<td height="25" align="center" bgcolor="#FFFFFF" class="MainTd">
		<input type="button" value="Show" name="aclient4" onClick="open_client_table(4);">
      </td>
</tr>

<script>
	for (i=1; i<=mssid_num; i++)
		UpdateVirtualStatus(document.MultipleAP, i);

	if (mssid_num == 1) {
		document.write("</table>\n");
		document.write("</span>\n");
		document.write("<table border=\"1\" width=700>\n");
	}	
</script>

</table>									
<p></p>
<input type="hidden" value="/wlmultipleap.asp" name="submit-url">
  <input type="submit" value="Apply Changes" name="save" onClick="return saveChanges(document.MultipleAP, wlan_idx)">&nbsp;&nbsp;
  <input type="button" value="Reset" name="reset1" onClick="click_reset();">&nbsp;&nbsp;  
  <!-- <input type="button" value=" Close " name="close" onClick="javascript: window.close();"> -->
</form>
</blockquote>
</body>

</html>
