<html>
<! Copyright (c) Realtek Semiconductor Corp., 2003. All Rights Reserved. ->
<head>
<meta http-equiv="Content-Type" content="text/html">
<title>VLAN Settings</title>
<script type="text/javascript" src="util_gw.js"> </script>
<script>
	
	var MAXNUM_VLAN = "<% getInfo("maxWebVlanNum"); %>"*1;
	var wlanDisabled = "<% if (getIndex("wlanDisabled")) write("1"); else write("0");%>"*1;
	var wlanDisabled_va1 = "<% if (getVirtualIndex("wlanDisabled", 1)) write("1");  else write("0"); %>"*1;
	var wlanDisabled_va2 = "<% if (getVirtualIndex("wlanDisabled", 2)) write("1");  else write("0"); %>"*1;
	var wlanDisabled_va3 = "<% if (getVirtualIndex("wlanDisabled", 3)) write("1");  else write("0"); %>"*1;
	var wlanDisabled_va4 = "<% if (getVirtualIndex("wlanDisabled", 4)) write("1");  else write("0"); %>"*1;
	var wlanMode = "<% getInfo("wlanMode"); %>"*1; //0:AP, 1:Client, 2:WDS, 3:AP+WDS
	var mssid_num=<%write(getIndex("wlan_mssid_num"));%>*1;
	var isSupportWLAN="<% getInfo("isVLANIncludeWLAN"); %>"*1;
	var isPocketRouter="<% getInfo("isPocketRouter"); %>"*1;
	
	function get_by_id(id)
	{
		with(document)
		{
			return getElementById(id);
		}
	}

	function init()
	{
		
		for(i=1;i<=MAXNUM_VLAN;i++)
		{
			if(get_by_id("vlan_enable_"+i) == null)
				continue;
			vlan_tag_select(i, get_by_id("vlan_tag_"+i).value);
			vlan_priority_select(i, get_by_id("vlan_priority_"+i).value);
			vlan_cfg_select(i, get_by_id("vlan_cfg_"+i).value);
			vlan_enable_select(i, get_by_id("vlan_enable_"+i).value);
			
			if(includeVLAN_Forward == 1)
			vlan_forward_select(i, get_by_id("vlan_forward_"+i).value);
		}
		
		vlan_onoff_select(get_by_id("vlan_onoff").value);
		
	}
	
	function vlan_table_disabled(index, value)
	{
		if(index == 5 && wlanDisabled == 1) // Wireless Primary
		{
			value = 0;
			get_by_id("enable_"+index).disabled = true;
		}
		else if(index >= 6 && index <=9) // Wireless Virtual AP1~4
		{
			if(wlanDisabled == 1 || eval("wlanDisabled_va"+(index-5)) == 1 || ((wlanMode != 0) && (wlanMode != 3)) )
			{
				get_by_id("enable_"+index).disabled = true;
			value = 0;
		}
		}
		
		if(value == 1)
		{
			get_by_id("tag_"+index).disabled = false;
			get_by_id("vlan_id_"+index).disabled = false;
			get_by_id("priority_"+index).disabled = false;
			get_by_id("cfg_"+index).disabled = false;
			
			if(includeVLAN_Forward == 1)
			get_by_id("vlan_forward_sel"+index).disabled = false;
		}
		else
		{
			get_by_id("tag_"+index).disabled = true;
			get_by_id("vlan_id_"+index).disabled = true;
			get_by_id("priority_"+index).disabled = true;
			get_by_id("cfg_"+index).disabled = true;
			if(includeVLAN_Forward == 1)
			get_by_id("vlan_forward_sel"+index).disabled = true;
		}
		
		if(get_by_id("wanlan_"+index).value == "WAN")
		{
			if(includeVLAN_Forward == 1) {
			get_by_id("vlan_forward_sel"+index).disabled = true;			
				get_by_id("vlan_tag_"+index).value = 0;
				get_by_id("tag_"+index).disabled = true;
			}
		}
	}
	
	function vlan_onoff_select(value)
	{
		if(value == true || value == 1)
		{
			get_by_id("vlan_onoff").value = 1;
			get_by_id("onoff").checked = true;
		}
		else
		{
			get_by_id("vlan_onoff").value = 0;
			get_by_id("onoff").checked = false;
		}
		
		vlan_onoff_disabled(get_by_id("vlan_onoff").value);
		
	}
	
	function vlan_onoff_disabled(value)
	{
		for(i=1;i<=MAXNUM_VLAN;i++)
		{
			if(get_by_id("vlan_enable_"+i) == null)
				continue;
				
			var disable = 0;
			if(value == 1)
			{
				get_by_id("enable_"+i).disabled = false;
			}
			else
			{
				get_by_id("enable_"+i).disabled = true;
			}

			if(value == 1 && get_by_id("vlan_enable_"+i).value == 1)
			{
				disable = 1;
			}
			else
			{
				disable = 0;
			}
			vlan_table_disabled(i, (disable && get_by_id("vlan_enable_"+i).value));
		}
	}
	
	function vlan_enable_select(index, value)
	{
		if(value == true || value == 1)
		{
			get_by_id("vlan_enable_"+index).value = 1;
			get_by_id("enable_"+index).checked = true;
		}
		else
		{
			get_by_id("vlan_enable_"+index).value = 0;
			get_by_id("enable_"+index).checked = false;
		}
		
		vlan_table_disabled(index, get_by_id("vlan_enable_"+index).value);
	}
	
	function vlan_tag_select(index, value)
	{
		
		
	}

	function vlan_tag_select(index, value)
	{
		if(includeVLAN_Forward == 1)
		{
			get_by_id("vlan_tag_"+index).value = value;
			get_by_id("tag_"+index).value = value;
		}
		else
		{
			if(value == true || value == 1)
			{
				get_by_id("vlan_tag_"+index).value = 1;
				get_by_id("tag_"+index).checked = true;
			}
			else
			{
				get_by_id("vlan_tag_"+index).value = 0;
				get_by_id("tag_"+index).checked = false;
			}
		}
	}
	
	function vlan_priority_select(index, value)
	{
		get_by_id("vlan_priority_"+index).value = value;
		get_by_id("priority_"+index).value = value;
	}
	
	function vlan_forward_select(index, value)
	{
		get_by_id("vlan_forward_"+index).value = value;
		get_by_id("vlan_forward_sel"+index).value = value;
		if(get_by_id("wanlan_"+index).value == "WAN")
		{
			get_by_id("vlan_forward_sel"+index).disabled = true;			
			get_by_id("vlan_tag_"+index).value = 0;
			get_by_id("tag_"+index).disabled = true;
		}
	}
	
	function vlan_vlan_id(index,value)
	{
		get_by_id("vlan_id_"+i).value = value;
	}
	
	function vlan_cfg_select(index, value)
	{
		if(value == true || value == 1)
		{
			get_by_id("vlan_cfg_"+index).value = 1;
			get_by_id("cfg_"+index).checked = true;
		}
		else
		{
			get_by_id("vlan_cfg_"+index).value = 0;
			get_by_id("cfg_"+index).checked = false;
		}
	}
	
	function page_submit()
	{
		for (var i = 1; i <= MAXNUM_VLAN; i++)
		{
			if(get_by_id("vlan_enable_"+i) == null)
				continue;
				
			if(get_by_id("vlan_enable_"+i).value != 1)
			{
				continue;
			}
			
			if(get_by_id("vlan_id_"+i).value > 4090 || get_by_id("vlan_id_"+i).value < 1)
			{
				alert("Invalid Vlan ID.");
				get_by_id("vlan_id_"+i).select();
				get_by_id("vlan_id_"+i).focus();
				return false;
			}
			
		}
		
		mf = document.forms.mainform;
		mf.submit();
	}
	
	var token= new Array();		
	var DataArray = new Array();
token[0]='<% vlanList("webVlanList_0");%>';
		token[1]='<% vlanList("webVlanList_1");%>';
		token[2]='<% vlanList("webVlanList_2");%>';
		token[3]='<% vlanList("webVlanList_3");%>';
		token[4]='<% vlanList("webVlanList_4");%>';
		token[5]='<% vlanList("webVlanList_5");%>';
		token[6]='<% vlanList("webVlanList_6");%>';
		token[7]='<% vlanList("webVlanList_7");%>';
		token[8]='<% vlanList("webVlanList_8");%>';
		token[9]='<% vlanList("webVlanList_9");%>';
		token[10]='<% vlanList("webVlanList_10");%>';

		var includeVLAN_Forward=0;
		DataArray = token[1].split("|"); /* web domain/url */
		if(DataArray.length == 10)
				includeVLAN_Forward = 1;

	function webVlanList(num)
	{
	
		for (var i = 1; i <= num; i++)
		{
			/* enabled/netIface/tagged/untagged/priority/cfi/groupId/vlanId/wanlan */
			DataArray = token[i].split("|"); /* web domain/url */
			
			if(isSupportWLAN==0 && i>=5 && i<=9)
				continue;
			if(mssid_num ==1 && i>=7 && i<=9)
				continue;
				
			if(isPocketRouter == 1 && i>=1 && i<=4)
				continue;
				
			document.write("<tr><td align = center bgcolor='#C0C0C0'><input type='hidden' id='vlan_enable_"+i+"' name='vlan_enable_"+i+"' value='"+DataArray[0]+"'>");
			document.write("<input type='checkbox' id='enable_"+i+"' onclick='vlan_enable_select("+i+",this.checked);'></td>");
			document.write("<td bgcolor='#C0C0C0'><input type='hidden' id='vlan_iface_"+i+"' name='vlan_iface_"+i+"' value='"+DataArray[1]+"'><font size='2'>");
			
			if(i>=6 && i<=9)
				document.write("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;");
			
			document.write("&nbsp;"+DataArray[1]+"</font></td>");
			
			document.write("<td align = center bgcolor='#C0C0C0'><input id='wanlan_"+i+"' name='wanlan_"+i+"' type='hidden' value='"+DataArray[8]+"'><font size='2'>"+DataArray[8]+"</font></td>");
			
			if(includeVLAN_Forward == 1)
			{
			document.write("<td align = center bgcolor='#C0C0C0'><input type='hidden' id='vlan_forward_"+i+"' name='vlan_forward_"+i+"' value='"+DataArray[9]+"'>");
			document.write("<select id='vlan_forward_sel"+i+"' size='1' onchange='vlan_forward_select("+i+",this.value);'>");
      	document.write("<option value='0'>Disabled</option><option value='1'>Bridge</option><option value='2'>NAT</option>");      	
      document.write("</select></td>");
    	}
if(includeVLAN_Forward == 1)
{
	document.write("<td align = center bgcolor='#C0C0C0'><input type='hidden' id='vlan_tag_"+i+"' name='vlan_tag_"+i+"' value='"+DataArray[2]+"'>");
	document.write("<select id='tag_"+i+"' size='1' onchange='vlan_tag_select("+i+",this.value);'>");
	//document.write("<option value='0'>UNTAG</option><option value='1'>WAN</option><option value='2'>LAN</option><option value='3'>BOTH</option>");
	document.write("<option value='0'>UNTAG</option><option value='1'>WAN</option><option value='3'>BOTH</option>");
	document.write("</select></td>");
}
else
{
			document.write("<td align = center bgcolor='#C0C0C0'><input type='hidden' id='vlan_tag_"+i+"' name='vlan_tag_"+i+"' value='"+DataArray[2]+"'>");
      document.write("<input id='tag_"+i+"' type='checkbox' onclick='vlan_tag_select("+i+",this.checked);'></td>");
}
      document.write("<td align = center bgcolor='#C0C0C0'><input id='vlan_id_"+i+"' name='vlan_id_"+i+"' size='4' maxlength='4' type='text' value='"+DataArray[7]+"'></td>");
      document.write("<td align = center bgcolor='#C0C0C0'><input type='hidden' id='vlan_priority_"+i+"' name='vlan_priority_"+i+"' value='"+DataArray[4]+"'>");
      		document.write("<select id='priority_"+i+"' size='1' onchange='vlan_priority_select("+i+",this.value);'>");
          	document.write("<option value='0'>0</option><option value='1'>1</option><option value='2'>2</option><option value='3'>3</option>");
          	document.write("<option value='4'>4</option><option value='5'>5</option><option value='6'>6</option><option value='7'>7</option>");
      document.write("</select></td>");
      document.write("<td align = center bgcolor='#C0C0C0'><input type='hidden' id='vlan_cfg_"+i+"' name='vlan_cfg_"+i+"' value='"+DataArray[5]+"'>");
      document.write("<input id='cfg_"+i+"' type='checkbox' onclick='vlan_cfg_select("+i+",this.checked);'></td></tr>");
  	}
		
	}
	
	function page_reset()
	{
		
		for(i=1;i<=MAXNUM_VLAN;i++)
		{
			/* enabled/netIface/tagged/untagged/priority/cfi/groupId/vlanId/wanlan */
			DataArray = token[i].split("|");
			
			if(get_by_id("vlan_enable_"+i) == null)
				continue;
			vlan_tag_select(i, DataArray[2]);
			vlan_priority_select(i, DataArray[4]);
			vlan_cfg_select(i, DataArray[5]);
			vlan_vlan_id(i, DataArray[7]);
			vlan_enable_select(i, DataArray[0]);
			if(includeVLAN_Forward == 1)
			vlan_forward_select(i, DataArray[9]);
		}
		
		vlan_onoff_select("<% getInfo("vlanOnOff"); %>"*1);

	}
	
	
	
</script>
</head>
  
  <body onload="init();">
  <blockquote>
  <form action=/goform/formVlan method=POST name="mainform">
  	<input type="hidden" value="/vlan.asp" name="submit-url">
  <h2><font color="#0000FF">VLAN Settings</font></h2>
	<table border=0 width="720" cellspacing=4 cellpadding=0>
	<tr><td><font size=2>
	 	Entries in below table are used to config vlan settings. 
		VLANs are created to provide the segmentation services traditionally provided by routers. 
		VLANs address issues such as scalability, security, and network management.
	</font></td></tr>

    <tr><td><hr size="1" align="top" noshade="noshade"></td></tr>
    
    
  	
  	<tr><td><font size=2><b>
    <input type='hidden' id='vlan_onoff' name='vlan_onoff' value='<% getInfo("vlanOnOff"); %>'>
		   	<input id="onoff" type="checkbox" onclick='vlan_onoff_select(this.checked);'>&nbsp;&nbsp;Enable VLAN</b><br>
		    </td>
    </tr>
  </table>
    
    
    <table border="0" width=720>
  
  <tr>
  	<td height="30" align=center width="10%" bgcolor="#808080"><font size="2"><b>&nbsp;Enable&nbsp;</b></font></td>
  	<td align=center width="30%" bgcolor="#808080"><font size="2"><b>Ethernet/Wireless</b></font></td>
  	<td align=center width="15%" bgcolor="#808080"><font size="2"><b>&nbsp;WAN/LAN&nbsp;</b></font></td>
  	<script>
  		if(includeVLAN_Forward == 1)
  			document.write("<td align=center width='' bgcolor='#808080'><font size='2'><b>&nbsp;Forwarding<br>Rule</b></font></td>");
  	</script>
		<td align=center width="" bgcolor="#808080"><font size="2"><b>&nbsp;Tag&nbsp;</b></font></td>
		<td align=center width="" bgcolor="#808080"><font size="2"><b>&nbsp;VID</b></font><font size="1">(1~4090)</font>&nbsp;</td>
		<td align=center width="" bgcolor="#808080"><font size="2"><b>&nbsp;Priority&nbsp;</b></font></td>
		<td align=center width="" bgcolor="#808080"><font size="2"><b>&nbsp;CFI&nbsp;</b></font></td>
	</tr>
	
    <!--
      <tr>
				<td align = center bgcolor="#C0C0C0">
      		<input type='hidden' id='vlan_enable_1' name='vlan_enable_1' value='1'>
      		<input type='checkbox' id='enable_1' onclick='vlan_enable_select(1,this.checked);'>
      	</td>
      	<td align=right bgcolor="#C0C0C0">Wireless Primary Interface</td>
      	
      	<td bgcolor="#C0C0C0">
      		<input type='hidden' id='vlan_tag_1' name='vlan_tag_1' value='1'>
      		<input id='tag_1' type="checkbox" onclick='vlan_tag_select(1,this.checked);'>
      	</td>
      	<td bgcolor="#C0C0C0"><input id='vlan_id_1' name='vlan_id_1' size='4' maxlength='4' type='text' value='20'></td>
      	<td bgcolor="#C0C0C0">
      		<input type='hidden' id='vlan_priority_1' name='vlan_priority_1' value="6">
      		<select id="priority_1" size="1" onchange='vlan_priority_select(1,this.value);'>
          	<option value="0">0</option>
          	<option value="1">1</option>
          	<option value="2">2</option>
          	<option value="3">3</option>
          	<option value="4">4</option>
          	<option value="5">5</option>
          	<option value="6">6</option>
          	<option value="7">7</option>
      		</select>
      	</td>
      	<td bgcolor="#C0C0C0">
      		<input type='hidden' id='vlan_cfg_1' name='vlan_cfg_1' value='1'>
      		<input id='cfg_1' type='checkbox' onclick='vlan_cfg_select(1,this.checked);'>
      	</td>
      </tr>
    -->
		<SCRIPT >webVlanList(MAXNUM_VLAN);</SCRIPT>
    
		<br>
		<tr><td>&nbsp;</td></tr>
			
	
  </table>
  <tr><td>
    <input type=button name=apply value='Apply Changes' onclick="page_submit();">&nbsp;&nbsp;
    <input type="button" value="Reset" onclick="page_reset();">
  </td></tr>
	</form>
 	<br>
  <br>
  <br>
    
	</blockquote>
  </body></html>