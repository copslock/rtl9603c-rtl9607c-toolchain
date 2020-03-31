<html>
<! Copyright (c) Realtek Semiconductor Corp., 2003. All Rights Reserved. ->
<head>
<meta http-equiv="Content-Type" content="text/html">
<title>DMZ Host</title>
<script type="text/javascript" src="util_gw.js"> </script>
<script>
function saveClick()
{
  if (!document.formDMZ.enabled.checked)
  	return true;

  if (document.formDMZ.ip.value=="")
	return true;

  if ( checkIpAddr(document.formDMZ.ip, 'Invalid IP address') == false )
	    return false;

  return true;
}

function updateState()
{
  if (document.formDMZ.enabled.checked) {
 	enableTextField(document.formDMZ.ip);
  }
  else {
 	disableTextField(document.formDMZ.ip);
  }
}


</script>
</head>

<body>
<blockquote>
<h2><font color="#0000FF">DMZ</font></h2>

<table border=0 width="500" cellspacing=4 cellpadding=0>
<tr><td><font size=2>

A Demilitarized Zone is used to provide Internet services without sacrificing
unauthorized access to its local private network. Typically, the DMZ host contains
devices accessible to Internet traffic, such as Web (HTTP ) servers, FTP servers,
SMTP (e-mail) servers and DNS servers.

</font></td></tr>

<tr><td><hr size=1 noshade align=top></td></tr>

<form action=/goform/formDMZ method=POST name="formDMZ">
<tr><td><font size=2><b>
   	<input type="checkbox" name="enabled" value="ON" <% if (getIndex("dmzEnabled")) write("checked");
   	%> ONCLICK=updateState()>&nbsp;&nbsp;Enable DMZ</b>
    </td>
</tr>
<tr><td>
     <font size=2><b>DMZ Host IP Address: </b><input type="text" name="ip" size="15" maxlength="15" value=<% getInfo("dmzHost"); %> ></font>
</td></tr>
<tr><td>
   <br>
     <input type="submit" value="Apply Changes" name="save" onClick="return saveClick()">&nbsp;&nbsp;
        <input type="reset" value="Reset" name="reset">
        <input type="hidden" value="/dmz.asp" name="submit-url">
</td></tr>
     <script> updateState(); </script>
</form>
</table>
</blockquote>
</body>
</html>
