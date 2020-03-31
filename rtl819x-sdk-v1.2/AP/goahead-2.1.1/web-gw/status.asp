<html>
<! Copyright (c) Realtek Semiconductor Corp., 2003. All Rights Reserved. ->
<head>
<meta http-equiv="Content-Type" content="text/html">
<title>Access Point Status</title>

<script>
var wlanmode, wlanclientnum;
</script>

</head>
<body>
<blockquote>

<h2><b><font color="#0000FF">Access Point Status</font></b></h2>

<table border=0 width="400" cellspacing=0 cellpadding=0>
<tr><td><font size=2>
 This page shows the current status and some basic settings of the device.
</font></td></tr>

<% getInfo("status_warning"); %>

<tr><td><hr size=1 noshade align=top><br></td></tr>
</table>


<table width=400 border=0">
  <tr>
    <td width=100% colspan="2" bgcolor="#008000"><font color="#FFFFFF" size=2><b>System</b></font></td>
  </tr>
  <tr bgcolor="#DDDDDD">
    <td width=40%><font size=2><b>Uptime</b></td>
    <td width=60%><font size=2><% getInfo("uptime"); %></td>
  </tr>
  <tr bgcolor="#EEEEEE">
    <td width=40%><font size=2><b>Firmware Version</b></td>
    <td width=60%><font size=2><% getInfo("fwVersion"); %></td>
  </tr>
  <tr bgcolor="#DDDDDD">
    <td width=40%><font size=2><b>Build Time</b></td>
    <td width=60%><font size=2><% getInfo("buildTime"); %></td>
  </tr>
  <script>
 	var wlan_num = <% write(getIndex("wlan_num")); %>;
 	var isNewMeshUI =  <% write(getIndex("isNewMeshUI")); %> ;
  	var wlanMode =new Array();
  	var networkType =new Array();
  	var band=new Array();
  	var ssid_drv=new Array();
  	var channel_drv=new Array();
  	var wep=new Array();
  	var wdsEncrypt=new Array();
  	var meshEncrypt=new Array();
  	var bssid_drv=new Array();
  	var clientnum=new Array();
  	var state_drv=new Array();
  	var rp_enabled=new Array();
	var rp_mode=new Array();
  	var rp_encrypt=new Array();
  	var rp_clientnum=new Array();
  	var rp_ssid=new Array();
  	var rp_bssid=new Array();
  	var rp_state=new Array();
	var wlanDisabled=new Array();
	
	var mssid_num=<%write(getIndex("wlan_mssid_num"));%>;
	var mssid_disable=new Array();
	var mssid_bssid_drv=new Array();
  	var mssid_clientnum=new Array();
	var mssid_band=new Array();
  	var mssid_ssid_drv=new Array();
  	var mssid_wep=new Array();
  	
 	<%
	   for (i=0; i<getIndex("wlan_num"); i=i+1)
	     {
		wlan_name= "wlan"+i+"-status";
		getInfo(wlan_name);
		write("wlanMode["+i+"] ="+getIndex("wlanMode")+";\n");
		write("networkType["+i+"] ="+getIndex("networkType")+";\n");
		write("band["+i+"] ="+getIndex("band")+";\n");
		write("ssid_drv["+i+"] ='");getInfo("ssid_drv");write("';\n");
		write("channel_drv["+i+"] ='");getInfo("channel_drv");write("';\n");
		write("wep["+i+"] ='");getInfo("wep");write("';\n");
		write("wdsEncrypt["+i+"] ='");getInfo("wdsEncrypt");write("';\n");
 		write("meshEncrypt["+i+"] ='");
		if (getIndex("isMeshDefined") == 1) 
			getInfo("meshEncrypt");	
		write("';\n"); 
		write("bssid_drv["+i+"] ='");getInfo("bssid_drv");write("';\n");
		write("clientnum["+i+"] ='");getInfo("clientnum");write("';\n");
		write("state_drv["+i+"] ='");getInfo("state_drv");write("';\n");
		write("wlanDisabled["+i+"] ="+getIndex("wlanDisabled")+";\n");

		write("rp_enabled["+i+"] ="+getIndex("isRepeaterEnabled")+";\n");
		write("rp_mode["+i+"] ="+getIndex("repeaterMode")+";\n");
		write("rp_encrypt["+i+"] ='");getVirtualInfo("wep", 5);write("';\n");		
		write("rp_ssid["+i+"] ='");getInfo("repeaterSSID_drv");write("';\n");
		write("rp_bssid["+i+"] ='");getInfo("repeaterBSSID");write("';\n");
		write("rp_state["+i+"] ='");getInfo("repeaterState");write("';\n");
		write("rp_clientnum["+i+"] ='");getInfo("repeaterClientnum");write("';\n");

		for (k=0; k<getIndex("wlan_mssid_num"); k=k+1)
		{
			write("mssid_ssid_drv["+k+"] ='");getVirtualInfo("ssid_drv", k+1);write("';\n");			
			write("mssid_band["+k+"] ="+getVirtualIndex("band", k+1)+";\n");
			write("mssid_disable["+k+"] ="+getVirtualIndex("wlanDisabled", k+1)+";\n");			
			write("mssid_bssid_drv["+k+"] ='");getVirtualInfo("bssid_drv", k+1);write("';\n");			
			write("mssid_clientnum["+k+"] ='");getVirtualInfo("clientnum", k+1);write("';\n");
			write("mssid_wep["+k+"] ='");getVirtualInfo("wep", k+1);write("';\n");
		}	
	     }
	%>
	if(ssid_drv[0]=="")
	     		mssid_num=0;
    for(i=0; i < wlan_num; i++){
	document.write('\
	<tr bgcolor="#EEEEEE">\
	<td width=100% colspan="2" bgcolor="#008000"><font color="#FFFFFF" size=2><b>Wireless '+<% if (getIndex("wlan_num") > 1)  write("(i+1)+"); %>' \
	Configuration</b></font></td>\
	</tr>\
	<tr bgcolor="#EEEEEE">\
	<td width=40%><font size=2><b>Mode</b></td>\
	<td width=60%><font size=2>' );
	/* mode */
	if(wlanMode[i] == 0)
		document.write("AP");
	if(wlanMode[i] == 1){
		if (networkType[0] == 0)
      	  		document.write( "Infrastructure Client");
		else
			document.write( "Ad-hoc Client");
	  }
  	if ( wlanMode[i] == 2 )
      	  	document.write( "WDS");
  	if ( wlanMode[i] == 3 )
      	  	document.write( "AP+WDS");
/*#ifdef CONFIG_NEW_MESH_UI*/
if( isNewMeshUI ==1 )
{
	if ( wlanMode[i] == 4 )
      	  	document.write( "AP+MESH");  	
   	if ( wlanMode[i] == 5 )
     	  	document.write( "MESH");
}
else
{
   	if ( wlanMode[i] == 4 )
      	  	document.write( "AP+MPP");  	
   	if ( wlanMode[i] == 5 )
     	  	document.write( "MPP");
   	if ( wlanMode[i] == 6 )
      	  	document.write( "MAP");	
   	if ( wlanMode[i] == 7 )
      	  	document.write( "MP");  
} 	
	/* band */
	document.write('</td>\
		<tr bgcolor="#DDDDDD">\
	    	<td width=40%><font size=2><b>Band</b></td>\
	    	<td width=60%><font size=2>');
	if (band[i] == 1)
   		document.write( "2.4 GHz (B)");
      	if (band[i] == 2)
   		document.write( "2.4 GHz (G)");
      	if (band[i] == 8)
   		document.write( "2.4 GHz (N)");   		
   	if (band[i] == 3)
   		document.write( "2.4 GHz (B+G)");
      	if (band[i] == 4)
   		document.write( "5 GHz (A)");
   	if (band[i] == 10)
   		document.write( "2.4 GHz (G+N)");
   	if (band[i] == 11)
   		document.write( "2.4 GHz (B+G+N)");		
	
	document.write('</tr>\
	<tr bgcolor="#EEEEEE">\
    	<td width=40%><font size=2><b>SSID</b></td>\
    	<td width=60%><font size=2>');
	if (wlanMode[i] != 2) {
		document.write('<pre><font size=2>');
		document.write(ssid_drv[i]);
	}
	document.write('</td>\
	</tr>\
	<tr bgcolor="#DDDDDD">\
	<td width=40%><font size=2><b>Channel Number</b></td>\
	<td width=60%><font size=2>'+channel_drv[i] +'</td>\
	</tr>\
	<tr bgcolor="#EEEEEE">\
	<td width=40%><font size=2><b>Encryption</b></td>\
	<td width=60%><font size=2>');
	if (wlanMode[i] == 0 || wlanMode[i] == 1)
    		document.write(wep[i]);
    	else if (wlanMode[i] == 2)
    		document.write(wdsEncrypt[i]);
    	else if (wlanMode[i] == 3)
    		document.write(wep[i] + '(AP),  ' + wdsEncrypt[i] + '(WDS)');
    	else if (wlanMode[i] == 4 || wlanMode[i] == 6)
    		document.write(wep[i] + '(AP),  ' + meshEncrypt[i] + '(Mesh)');    	
    	else if (wlanMode[i] == 5|| wlanMode[i] == 7)
    		document.write( meshEncrypt[i] + '(Mesh)');

	document.write('</td>\
  	</tr>\
  	<tr bgcolor="#DDDDDD">\
    	<td width=40%><font size=2><b>BSSID</b></td>\
    	<td width=60%><font size=2>'+bssid_drv[i]+'</td>\
  	</tr>');
	if (wlanMode[i]!=2) {	//2 means WDS mode
		document.write('<tr bgcolor="#EEEEEE">\n');
		if (wlanMode[i]==0 || wlanMode[i]==3 || wlanMode[i]==4) {
			document.write("<td width=40%%><font size=2><b>Associated Clients</b></td>\n");
			document.write("<td width=60%%><font size=2>"+clientnum[i]+"</td></tr>");
		}
		else {
			document.write("<td width=40%%><font size=2><b>State</b></td>\n");
			document.write('<td width=60%%><font size=2>'+state_drv[i]+'</td></tr>');
		}
        }

	/* mesh does not support virtual ap */
	if (!wlanDisabled[i] && (wlanMode[i]==0 || wlanMode[i]==3 )) {
		for (idx=0; idx<mssid_num; idx++) {
			if (!mssid_disable[idx]) {
				document.write('\
				<tr bgcolor="#EEEEEE">\
				<td width=100% colspan="2" bgcolor="#008000"><font color="#FFFFFF" size=2>\
				<b>Virtual AP'+(idx+1)+' Configuration</b></font></td>\
				</tr>\
				<tr bgcolor="#EEEEEE">\
	    			<td width=40%><font size=2><b>Band</b></td>\
			    	<td width=60%><font size=2>');
				if (mssid_band[idx] == 1)
			   		document.write( "2.4 GHz (B)");
			      	if (mssid_band[idx] == 2)
			   		document.write( "2.4 GHz (G)");
			      	if (mssid_band[idx] == 8)
			   		document.write( "2.4 GHz (N)");  	
			   	if (mssid_band[idx] == 3)
			   		document.write( "2.4 GHz (B+G)");
			      	if (mssid_band[idx] == 4)
			   		document.write( "5 GHz (A)");
			   	if (mssid_band[idx] == 10)
			   		document.write( "2.4 GHz (G+N)");
			   	if (mssid_band[idx] == 11)
			   		document.write( "2.4 GHz (B+G+N)");		
	
				document.write('</td></tr>\
				<tr bgcolor="#DDDDDD">\
			    	<td width=40%><font size=2><b>SSID</b></td>\
			    	<td width=60%><font size=2>');
				document.write('<pre><font size=2>'+mssid_ssid_drv[idx]+'</td>');

				document.write('</tr>\
				<tr bgcolor="#EEEEEE">\
				<td width=40%><font size=2><b>Encryption</b></td>\
				<td width=60%><font size=2>'+mssid_wep[idx]+'</td>');
			
				document.write('</tr>\
				<tr bgcolor="#DDDDDD">\
				<td width=40%><font size=2><b>BSSID</b></td>\
				<td width=60%><font size=2>'+mssid_bssid_drv[idx]+'</td>');
				
				document.write('</tr>\
				<tr bgcolor="#EEEEEE">\
				<td width=40%%><font size=2><b>Associated Clients</b></td>\
				<td width=60%%><font size=2>'+mssid_clientnum[idx]+'</td></tr>');
			}
		}
	}

//    document.write('</table>\
    if (rp_enabled[i])	// start of repeater
    {
    	document.write('\
    	<tr bgcolor="#EEEEEE">\
    	<td width=100% colspan="2" bgcolor="#008000"><font color="#FFFFFF" size=2><b>Wireless '+<% if (getIndex("wlan_num") > 1)  write("(i+1)+"); %>' \
    	Repeater Interface Configuration</b></font></td>\
    	</tr>\
    	<tr bgcolor="#EEEEEE">\
    	<td width=40%><font size=2><b>Mode</b></td>\
    	<td width=60%><font size=2>' );
    	/* mode */
    	if(rp_mode[i] == 0)
    		document.write("AP");
    	else
    		document.write( "Infrastructure Client");
    	document.write('</td>\
    	</tr>\
    	<tr bgcolor="#DDDDDD">\
    	<td width=40%><font size=2><b>SSID</b></td>\
    	<td width=60%><font size=2>'+rp_ssid[i] +'</td>\
    	</tr>\
    	<tr bgcolor="#EEEEEE">\
    	<td width=40%><font size=2><b>Encryption</b></td>\
    	<td width=60%><font size=2>'+rp_encrypt[i] +'</td>\
    	</tr>\
    	<tr bgcolor="#DDDDDD">\
    	<td width=40%><font size=2><b>BSSID</b></td>\
    	<td width=60%><font size=2>'+rp_bssid[i] +'</td>\
    	</tr>');
    	document.write('<tr bgcolor="#EEEEEE">\n');
    	if (rp_mode[i]==0 || rp_mode[i]==3) {
    		document.write("<td width=40%%><font size=2><b>Associated Clients</b></td>\n");
    		document.write("<td width=60%%><font size=2>"+rp_clientnum[i]+"</td></tr>");
    	}
    	else {
    		document.write("<td width=40%%><font size=2><b>State</b></td>\n");
    		document.write('<td width=60%%><font size=2>'+rp_state[i]+'</td></tr>');
    	}
	}	// end of repeater
   }//end of wlan_num for
  </script>
<% getInfo("pocketRouter_html_lan_hide_s"); %>  
  <tr>
    <td width=100% colspan="2" bgcolor="#008000"><font color="#FFFFFF" size=2><b>TCP/IP Configuration</b></font></td>
  </tr>
  <tr bgcolor="#EEEEEE">
    <td width=40%><font size=2><b>Attain IP Protocol</b></td>
    <td width=60%><font size=2><% getInfo("dhcp-current"); %></td>
  </tr>

  <tr bgcolor="#DDDDDD">
    <td width=40%><font size=2><b>IP Address</b></td>
    <td width=60%><font size=2><% getInfo("ip"); %></td>
  </tr>
  <tr bgcolor="#EEEEEE">
    <td width=40%><font size=2><b>Subnet Mask</b></td>
    <td width=60%><font size=2><% getInfo("mask"); %></td>
  </tr>
  <tr bgcolor="#DDDDDD">
    <td width=40%><font size=2><b>Default Gateway</b></td>
    <td width=60%><font size=2><% getInfo("gateway"); %></td>
  </tr>
  <tr bgcolor="#EEEEEE">
    <td width=40%><font size=2><b>DHCP Server</b></td>
    <td width=60%><font size=2>
      <%  choice = getIndex("dhcp-current");
      	  if ( choice == 0 ) write( "Disabled" );
    	  if ( choice == 2 ) write( "Enabled" );
    	  if ( choice == 15 ) write( "Auto" );
      %></td>
  </tr>

  <tr bgcolor="#DDDDDD">
    <td width=40%><font size=2><b>MAC Address</b></td>
    <td width=60%><font size=2><% getInfo("hwaddr"); %></td>
  </tr>
<% getInfo("pocketRouter_html_lan_hide_e"); %>

<% getInfo("pocketRouter_html_wan_hide_s"); %>
 <tr>
    <td width=100% colspan=2 bgcolor="#008000"><font color="#FFFFFF" size=2><b>WAN Configuration</b></font></td>
  </tr>
  <tr bgcolor="#EEEEEE">
    <td width=40%><font size=2><b>Attain IP Protocol</b></td>
    <td width=60%><font size=2>
	    <% getInfo("wanDhcp-current"); %></td>
  </tr>
  <tr bgcolor="#DDDDDD">
    <td width=40%><font size=2><b>IP Address</b></td>
    <td width=60%><font size=2><% getInfo("wan-ip"); %></td>
  </tr>
  <tr bgcolor="#EEEEEE">
    <td width=40%><font size=2><b>Subnet Mask</b></td>
    <td width=60%><font size=2><% getInfo("wan-mask"); %></td>
  </tr>
  <tr bgcolor="#DDDDDD">
    <td width=40%><font size=2><b>Default Gateway</b></td>
    <td width=60%><font size=2><% getInfo("wan-gateway"); %></td>
  </tr>
  <tr bgcolor="#EEEEEE">
    <td width=40%><font size=2><b>MAC Address</b></td>
    <td width=60%><font size=2><% getInfo("wan-hwaddr"); %></td>
  </tr>
<% getInfo("pocketRouter_html_wan_hide_e"); %>  
  <% getInfo("voip_status"); %>
</table>
<br>
<table border="0" align="center" width="70%">
  <tr><td><img border="0" src="/graphics/goahead_logo.gif"></td></tr>
</table>

</blockquote>

</body>

</html>
