<html>
<! Copyright (c) Realtek Semiconductor Corp., 2003. All Rights Reserved. ->
<head>
<meta http-equiv="Content-Type" content="text/html">
<title>Active Wireless Client Table</title>
<script type="text/javascript" src="util_ap.js"> </script>
<style>
.on {display:on}
.off {display:none}	
</style>
<script>
var vap_num;
var vap_id;
function write_comment(id, is_head)
{
	if (vap_num != id) {
		if (is_head) {	
			document.write("<!"); 
			document.write("--\n");
		}
		else
			document.write("-->\n");
	}
}

function init()
{
	
	var url_tmp = location.href;
	var url_tmp_1 = url_tmp.split("?");
	var id = url_tmp_1[1].split("=");

	vap_id = id[1]*1;
	get_by_id("submit-url").value = "/wlstatbl_vap.asp?id="+vap_id;

}
</script>
</head>
<body>
<form action=/goform/formWirelessTbl method=POST name="formWirelessTbl">
<input type="hidden" value="" id="submit-url" name="submit-url">
<blockquote>
<h2><font color="#0000FF">Active Wireless Client Table
	<script>
		init();
		//if (window.opener.aclist_index == 1) 
		if(vap_id == 1)
		{
			document.write(" - AP1");
			vap_num = 1;
		}
		//else if (window.opener.aclist_index == 2) 
		else if (vap_id == 2) 
		{
			document.write(" - AP2");
			vap_num = 2;
		}
		//else if (window.opener.aclist_index == 3) 
		else if (vap_id == 3) 
		{
			document.write(" - AP3");
			vap_num = 3;
		}
		//else if (window.opener.aclist_index == 4)
		else if (vap_id == 4) 
		{
			document.write(" - AP4");
			vap_num = 4;
		}
		else
		{
			alert("We can not process AP"+vap_id+". The windows will be closed.");
			window.opener=null;   
			window.open("","_self");   
			window.close();
		}
	</script>
	</font></h2>

<table border=0 width="600" cellspacing=0 cellpadding=0>
  <tr><font size=2>
 This table shows the MAC address, transmission, receiption packet counters and encrypted
 status for each associated wireless client.
  </tr>
  <tr><hr size=1 noshade align=top></tr>
</table>

<script> if (vap_num != 1) document.write("</table><span class = \"off\" ><table>"); </script>
<table border='1' width="600">
<tr bgcolor=#7f7f7f><td width="100"><font size=2><b>MAC Address</b></td>
<td width="60"><font size=2><b>Mode</b></td>
<td width="60"><font size=2><b>Tx Packet</b></td>
<td width="60"><font size=2><b>Rx Packet</b></td>
<td width="60"><font size=2><b>Tx Rate (Mbps)</b></td>
<td width="60"><font size=2><b>Power Saving</b></td>
<td width="60"><font size=2><b>Expired Time (s)</b></td></tr>
<% wirelessClientList(" ", 1); %>	
<script> if (vap_num != 1) document.write("</table></span><table border='1' width=\"600\">"); </script>

<script> if (vap_num != 2) document.write("</table><span class = \"off\" ><table>"); </script>
<tr bgcolor=#7f7f7f><td width="100"><font size=2><b>MAC Address</b></td>
<td width="60"><font size=2><b>Mode</b></td>
<td width="60"><font size=2><b>Tx Packet</b></td>
<td width="60"><font size=2><b>Rx Packet</b></td>
<td width="60"><font size=2><b>Tx Rate (Mbps)</b></td>
<td width="60"><font size=2><b>Power Saving</b></td>
<td width="60"><font size=2><b>Expired Time (s)</b></td></tr>
<% wirelessClientList(" ", 2); %>	
<script> if (vap_num != 2) document.write("</table></span><table border='1' width=\"600\">"); </script>

<script> if (vap_num != 3) document.write("</table><span class = \"off\" ><table>"); </script>
<tr bgcolor=#7f7f7f><td width="100"><font size=2><b>MAC Address</b></td>
<td width="60"><font size=2><b>Mode</b></td>
<td width="60"><font size=2><b>Tx Packet</b></td>
<td width="60"><font size=2><b>Rx Packet</b></td>
<td width="60"><font size=2><b>Tx Rate (Mbps)</b></td>
<td width="60"><font size=2><b>Power Saving</b></td>
<td width="60"><font size=2><b>Expired Time (s)</b></td></tr>
<% wirelessClientList(" ", 3); %>	
<script> if (vap_num != 3) document.write("</table></span><table border='1' width=\"600\">"); </script>

<script> if (vap_num != 4) document.write("</table><span class = \"off\" ><table>"); </script>
<tr bgcolor=#7f7f7f><td width="100"><font size=2><b>MAC Address</b></td>
<td width="60"><font size=2><b>Mode</b></td>
<td width="60"><font size=2><b>Tx Packet</b></td>
<td width="60"><font size=2><b>Rx Packet</b></td>
<td width="60"><font size=2><b>Tx Rate (Mbps)</b></td>
<td width="60"><font size=2><b>Power Saving</b></td>
<td width="60"><font size=2><b>Expired Time (s)</b></td></tr>
<% wirelessClientList(" ", 4); %>	
<script> if (vap_num != 4) document.write("</table></span><table border='1' width=\"600\">"); </script>

</table>
  <p><input type="submit" value="Refresh" name="refresh">&nbsp;&nbsp;
  <input type="button" value=" Close " name="close" onClick="javascript: window.close();"></p>
</form>
</blockquote>
</body>

</html>
