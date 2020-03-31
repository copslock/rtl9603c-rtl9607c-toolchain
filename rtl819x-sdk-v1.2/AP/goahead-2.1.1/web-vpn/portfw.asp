<html>
<! Copyright (c) Realtek Semiconductor Corp., 2003. All Rights Reserved. ->
<head>
<meta http-equiv="Content-Type" content="text/html">
<title>Port Forwarding</title>
<script type="text/javascript" src="common.js"> </script>
<script>

function addClick()
{
  if (!document.formPortFwAdd.enabled.checked)
  	return true;
	
  if (document.formPortFwAdd.ip.value=="" && document.formPortFwAdd.fromPort.value=="" &&
	document.formPortFwAdd.toPort.value=="" && document.formPortFwAdd.comment.value=="" )
	return true;
  if ( checkIpAddr(document.formPortFwAdd.ip, 'Invalid IP address') == false )
	    return false;


  if (document.formPortFwAdd.fromPort.value=="") {
	alert("Port range cannot be empty! You should set a value between 1-65535.");
	document.formPortFwAdd.fromPort.focus();
	return false;
  }
  if ( validateKey( document.formPortFwAdd.fromPort.value ) == 0 ) {
	alert("Invalid port number! It should be the decimal number (0-9).");
	document.formPortFwAdd.fromPort.focus();
	return false;
  }
  d1 = getDigit(document.formPortFwAdd.fromPort.value, 1);
  if (d1 > 65535 || d1 < 1) {
	alert("Invalid port number! You should set a value between 1-65535.");
	document.formPortFwAdd.fromPort.focus();
	return false;
  }
  if (document.formPortFwAdd.toPort.value!="") {
  	if ( validateKey( document.formPortFwAdd.toPort.value ) == 0 ) {
		alert("Invalid port number! It should be the decimal number (0-9).");
		document.formPortFwAdd.toPort.focus();
		return false;
  	}
	d2 = getDigit(document.formPortFwAdd.toPort.value, 1);
 	if (d2 > 65535 || d2 < 1) {
		alert("Invalid port number! You should set a value between 1-65535.");
		document.formPortFwAdd.toPort.focus();
		return false;
  	}
	if (d1 > d2 ) {
		alert("Invalid port range! 1st port value should be less than 2nd value.");
		document.formPortFwAdd.fromPort.focus();
		return false;

	}
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
	disableButton(document.formPortFwDel.deleteSelPortFw);
	disableButton(document.formPortFwDel.deleteAllPortFw);
}

function updateState()
{
  if (document.formPortFwAdd.enabled.checked) {
 	enableTextField(document.formPortFwAdd.ip);
	enableTextField(document.formPortFwAdd.protocol);
	enableTextField(document.formPortFwAdd.fromPort);
	enableTextField(document.formPortFwAdd.toPort);
	enableTextField(document.formPortFwAdd.comment);
  }
  else {
 	disableTextField(document.formPortFwAdd.ip);
	disableTextField(document.formPortFwAdd.protocol);
	disableTextField(document.formPortFwAdd.fromPort);
	disableTextField(document.formPortFwAdd.toPort);
	disableTextField(document.formPortFwAdd.comment);
  }
}

</script>
</head>

<body>
<blockquote>
<h2><font color="#0000FF">Port Forwarding</font></h2>

<table border=0 width="550" cellspacing=4 cellpadding=0>
<tr><td><font size=2>
 Entries in this table allow you to automatically redirect common network services
 to a specific machine behind the NAT firewall.  These settings are only necessary
 if you wish to host some sort of server like a web server or mail server on the
 private local network behind your Gateway's NAT firewall.
</font></td></tr>

<tr><td><hr size=1 noshade align=top></td></tr>

<form action=/goform/formPortFw method=POST name="formPortFwAdd">

<tr><td><font size=2><b>
   	<input type="checkbox" name="enabled" value="ON" <% if (getIndex("portFwEnabled")) write("checked");
   	%> ONCLICK=updateState()>&nbsp;&nbsp;Enable Port Forwarding</b><br>
    </td>
</tr>

<tr><td>
  <p><font size=2><b>IP Address:</b> <input type="text" name="ip" size="10" maxlength="15">&nbsp;
  <b>Protocol:</b> <select name="protocol">
    <option select value="0">Both</option>
    <option value="1">TCP</option>
    <option value="2">UDP</option>
  </select>&nbsp;<b>Port Range:</b> <input type="text" name="fromPort" size="3"><b>-</b>
      <input type="text" name="toPort" size="3">
    <b>Comment:</b> <input type="text" name="comment" size="6" maxlength="20"></font>

  <p><input type="submit" value="Apply Changes" name="addPortFw" onClick="return addClick()">&nbsp;&nbsp;
     <input type="reset" value="Reset" name="reset"></p>
  <input type="hidden" value="/portfw.asp" name="submit-url">
</td></tr>
  <script> updateState(); </script>
</form>
</table>


<br>
<form action=/goform/formPortFw method=POST name="formPortFwDel">
<table border=0 width=500>
  <tr><font size=2><b>Current Port Forwarding Table:</b></font></tr>
  <% portFwList(); %>
</table>

 <br><input type="submit" value="Delete Selected" name="deleteSelPortFw" onClick="return deleteClick()">&nbsp;&nbsp;
     <input type="submit" value="Delete All" name="deleteAllPortFw" onClick="return deleteAllClick()">&nbsp;&nbsp;&nbsp;
     <input type="reset" value="Reset" name="reset">
 <script>
   	<% entryNum = getIndex("portFwNum");
   	  if ( entryNum == 0 ) {
      	  	write( "disableDelButton();" );
       	  } %>
 </script>
     <input type="hidden" value="/portfw.asp" name="submit-url">
</form>

</td></tr></table>

</blockquote>
</body>
</html>

