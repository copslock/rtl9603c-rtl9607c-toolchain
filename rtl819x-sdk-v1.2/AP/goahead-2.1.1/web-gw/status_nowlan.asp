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
      %></td>
  </tr>

  <tr bgcolor="#DDDDDD">
    <td width=40%><font size=2><b>MAC Address</b></td>
    <td width=60%><font size=2><% getInfo("hwaddr"); %></td>
  </tr>

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
  <% getInfo("voip_status"); %>
</table>
<br>
<table border="0" align="center" width="70%">
  <tr><td><img border="0" src="/graphics/goahead_logo.gif"></td></tr>
</table>

</blockquote>

</body>

</html>
