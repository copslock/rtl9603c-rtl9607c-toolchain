<html>
<! Copyright (c) Realtek Semiconductor Corp., 2003~2005. All Rights Reserved. ->
<head>
<meta http-equiv="Content-Type" content="text/html">
<title>System Command</title>
<script type="text/javascript" src="util_gw.js"></script>
<script>
function showProxiedMAC(owner)
{
        openWindow("/goform/formMeshProxy?owner="+owner, 'formMeshProxy',620,340 );
}
</script>
</head>
<blockquote>
<h2><font color="#0000FF">Wireless Mesh Network Information</font></h2>

<body>

  <form action=/goform/formWlanRedirect method=POST name="formWlanRedirect">

<!-- <table border=0 width="540" cellspacing=4 cellpadding=0>    	
    	<tr><hr size=1 noshade align=top></tr>
    	<tr>
      <td width="20%"><font size=2><b>Root MAC:&nbsp;&nbsp;&nbsp;&nbsp;</b></td>
      <td width="40%"><input type="text" name="rootmac" size="15" maxlength="13" value=" <% wlMeshRootInfo();  %>" disabled="true"></td>
	</tr></table>
	<br><br> -->
	<table border="0" width=540>
	<tr><font size=2>
		These information is only for more technically advanced users who have a sufficient knowledge about wireless mesh network
        </font></tr>
	<tr><hr size=1 noshade align=top></tr>
	<tr><br></tr>
	<tr><font size=4><b>Neighbor Table</b></font></tr>
	<% wlMeshNeighborTable(); %>
	</table>
	<br>
	  <table border="0" width=540>
	  <tr><font size=4><b>Routing Table</b></font></tr>
	  <% wlMeshRoutingTable(); %>
	  </table>
	  <br>
  	  <!-- <table border="1" width=540>
	  <tr><font size=4><b>Portal Table</b></font></tr>
	  <% wlMeshPortalTable(); %>
	  </table>
	  <br><br>

	<table border="1" width=540>
	<tr><font size=4><b>Station List</b></font></tr>
	<tr bgcolor=#7f7f7f><td width="20%"><font size=2><b>MAC Address</b></td>
	<td width="10%"><font size=2><b>Mode</b></td>
	<td width="15%"><font size=2><b>Tx Packet</b></td>
	<td width="15%"><font size=2><b>Rx Packet</b></td>
	<td width="10%"><font size=2><b>Tx Rate (Mbps)</b></td>
	<td width="10%"><font size=2><b>Power Saving</b></td>
	<td width="15%"><font size=2><b>Expired Time (s)</b></td></tr>
	<% wirelessClientList(); %>
	</table>
	<br><br>

	  <table border="1" width=240>
	  <tr><font size=4><b>Proxy Table</b></font></tr>
	  <% wlMeshProxyTable(); %>
	  </table>
	<br><br> -->
	<input type="hidden" name="redirect-url" value="/wlmeshinfo.asp">
	<input type="submit" name="refresh" value="Refresh">
	<br><br>
  </form>  
</body>
</blockquote>
</html>

