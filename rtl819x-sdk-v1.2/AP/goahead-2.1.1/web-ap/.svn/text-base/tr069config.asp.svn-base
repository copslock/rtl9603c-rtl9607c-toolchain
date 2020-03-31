<html>
<! Copyright (c) Realtek Semiconductor Corp., 2003. All Rights Reserved. ->
<head>
<meta http-equiv="Content-Type" content="text/html">
<title>TR-069 Configuration</title>
<script type="text/javascript" src="share.js">
</script>
<% language=javascript %>
<SCRIPT>
function resetClick()
{
   document.tr069.reset;
}

function periodicSel() {
	if ( document.tr069.enable[0].checked ) {
		disableTextField(document.tr069.interval);
	} else {
		enableTextField(document.tr069.interval);
	}
}  

function saveChanges()
{
  if (document.tr069.url.value=="") {
	alert("ACS URL cannot be empty!");
	document.tr069.url.value = document.tr069.url.defaultValue;
	document.tr069.url.focus();
	return false;		
  }

  if (document.tr069.enable[1].checked) {
	if ( document.tr069.interval.value=="") {
		alert("Please input periodic interval time.");
		document.tr069.interval.focus();
		return false;
	}
	if ( validateKey( document.tr069.interval.value ) == 0 ) {
		alert('Interval should be the decimal number (0-9).');
		document.tr069.interval.focus();
		return false;
	}
  }

  return true;
}

</SCRIPT>
</head>

<body>
<blockquote>
<h2><font color="#0000FF">TR-069 Configuration</font></h2>

<form action=/goform/formTR069Config method=POST name="tr069">
<table border=0 width="500" cellspacing=4 cellpadding=0>
  <tr><font size=2>
    This page is used to configure the TR-069 CPE.
    Here you may change the setting for the ACS's parameters.
  </tr>
  <tr><hr size=1 noshade align=top></tr>
  <tr>
      <td width="30%"><font size=2><b>ACS:</b></td>
      <td width="70%"><b></b></td>
  </tr>
  <tr>
      <td width="30%"><font size=2><b>ACS URL:</b></td>
      <td width="70%"><input type="text" name="url" size="32" maxlength="256" value="<% getInfo("acs_url"); %>"></td>
  </tr>
  <tr>
      <td width="30%"><font size=2><b>User Name:</b></td>
      <td width="70%"><input type="text" name="username" size="32" maxlength="256" value="<% getInfo("acs_username"); %>"></td>
  </tr>
  <tr>
      <td width="30%"><font size=2><b>Password:</b></td>
      <td width="70%"><input type="text" name="password" size="32" maxlength="256" value="<% getInfo("acs_password"); %>"></td>
  </tr>
  <tr>
      <td width="30%"><font size=2><b>Periodic Inform Enable:</b></td>
      <td width="70%"><font size=2>
      <input type="radio" name=enable value=0 <% getInfo("tr069-inform-0"); %> onClick="return periodicSel()">Disabled&nbsp;&nbsp;
      <input type="radio" name=enable value=1 <% getInfo("tr069-inform-1"); %> onClick="return periodicSel()">Enabled</td>
  </tr>
  <tr>
      <td width="30%"><font size=2><b>Periodic Inform Interval:</b></td>
      <td width="70%"><input type="text" name="interval" size="32" maxlength="10" value="<% getInfo("inform_interval"); %>" <% getInfo("tr069_interval");     %> ></td>
  </tr>
</table>

<table border=0 width="500" cellspacing=4 cellpadding=0>
  <tr><hr size=1 noshade align=top></tr>
  <tr>
      <td width="30%"><font size=2><b>Connection Request:</b></td>
      <td width="70%"><b></b></td>
  </tr>
  <tr>
      <td width="30%"><font size=2><b>User Name:</b></td>
      <td width="70%"><input type="text" name="conreqname" size="32" maxlength="256" value="<% getInfo("conreq_name"); %>"></td>
  </tr>
  <tr>
      <td width="30%"><font size=2><b>Password:</b></td>
      <td width="70%"><input type="text" name="conreqpw" size="32" maxlength="256" value="<% getInfo("conreq_pw"); %>"></td>
  </tr>
</table>
<table border=0 width="500" cellspacing=4 cellpadding=0>
  <tr><hr size=1 noshade align=top></tr>
  <tr>
      <td width="30%"><font size=2><b>Debug:</b></td>
      <td width="70%"><b></b></td>
  </tr>
  
  <tr>
      <td width="30%"><font size=2><b>Show Message:</b></td>
      <td width="70%"><font size=2>
      <input type="radio" name=dbgmsg value=0 <% getInfo("tr069-dbgmsg-0"); %> >Disabled&nbsp;&nbsp;
      <input type="radio" name=dbgmsg value=1 <% getInfo("tr069-dbgmsg-1"); %> >Enabled
      </td>
  </tr>
  <tr>
      <td width="30%"><font size=2><b>CPE Sends GetRPC:</b></td>
      <td width="70%"><font size=2>
      <input type="radio" name=sendgetrpc value=0 <% getInfo("tr069-sendgetrpc-0"); %> >Disabled&nbsp;&nbsp;
      <input type="radio" name=sendgetrpc value=1 <% getInfo("tr069-sendgetrpc-1"); %> >Enabled
      </td>
  </tr>
  
  <tr>
      <td width="30%"><font size=2><b>Skip MReboot:</b></td>
      <td width="70%"><font size=2>
      <input type="radio" name=skipmreboot value=0 <% getInfo("tr069-skipmreboot-0"); %> >Disabled&nbsp;&nbsp;
      <input type="radio" name=skipmreboot value=1 <% getInfo("tr069-skipmreboot-1"); %> >Enabled
      </td>
  </tr>
  <tr>
      <td width="30%"><font size=2><b>Auto-Execution:</b></td>
      <td width="70%"><font size=2>
      <input type="radio" name=autoexec value=0 <% getInfo("tr069-autoexec-0"); %> >Disabled&nbsp;&nbsp;
      <input type="radio" name=autoexec value=1 <% getInfo("tr069-autoexec-1"); %> >Enabled
      </td>
  </tr>
  <tr>
      <td width="30%"><font size=2><b>Delay:</b></td>
      <td width="70%"><font size=2>
      <input type="radio" name=delay value=0 <% getInfo("tr069-delay-0"); %> >Disabled&nbsp;&nbsp;
      <input type="radio" name=delay value=1 <% getInfo("tr069-delay-1"); %> >Enabled
      </td>
  </tr>

</table>
<br>
      <input type="submit" value="Apply Changes" name=submitbut_tr069 onClick="return saveChanges()">&nbsp;&nbsp;
      <input type="reset" value="Undo" name="reset" onClick="resetClick()">
      <input type="hidden" value="/tr069config.asp" name="submit-url">
</form>

<% TR069ConPageShow("ShowMNGCertTable"); %>

</blockquote>
</body>

</html>
