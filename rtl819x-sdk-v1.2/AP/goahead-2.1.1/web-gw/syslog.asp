<html>
<! Copyright (c) Realtek Semiconductor Corp., 2003~2005. All Rights Reserved. ->
<head>
<meta http-equiv="Content-Type" content="text/html">
<title>System Command</title>
<script type="text/javascript" src="util_gw.js"> </script>
<script>
var isMeshDefined = <% write(getIndex("isMeshDefined")); %> 
function saveClick(){
	
  	if ( document.formSysLog.rtLogEnabled.checked && checkIpAddr(document.formSysLog.logServer, 'Invalid Server address! ') == false )
              return false;
	return true;
}
function updateState(){
	if(document.formSysLog.logEnabled.checked){
		enableTextField(document.formSysLog.rtLogEnabled);
		enableTextField(document.formSysLog.syslogEnabled);
		enableTextField(document.formSysLog.wlanlogEnabled);
		enableTextField(document.formSysLog.doslogEnabled);
if( isMeshDefined ==1)
		enableTextField(document.formSysLog.meshlogEnabled);
		updateStateRemote();
		updateStateSys();
	}
	else {
		disableTextField(document.formSysLog.logServer);
		disableTextField(document.formSysLog.rtLogEnabled);
		disableTextField(document.formSysLog.syslogEnabled);
		disableTextField(document.formSysLog.wlanlogEnabled);
		disableTextField(document.formSysLog.doslogEnabled);
if( isMeshDefined ==1)
		disableTextField(document.formSysLog.meshlogEnabled);
	}
}

function updateStateSys(){
	if(document.formSysLog.syslogEnabled.checked){
		disableTextField(document.formSysLog.wlanlogEnabled);
if( isMeshDefined ==1)
		disableTextField(document.formSysLog.meshlogEnabled);
		disableTextField(document.formSysLog.doslogEnabled);
	}
	else {
		enableTextField(document.formSysLog.wlanlogEnabled);
if( isMeshDefined ==1)
		enableTextField(document.formSysLog.meshlogEnabled);
		enableTextField(document.formSysLog.doslogEnabled);
	}
}

function updateStateRemote(){
	if(document.formSysLog.rtLogEnabled.checked)
		enableTextField(document.formSysLog.logServer);
	else
		disableTextField(document.formSysLog.logServer);

}
function scrollElementToEnd (element) {
   if (typeof element.scrollTop != 'undefined' &&
       typeof element.scrollHeight != 'undefined') {
     element.scrollTop = element.scrollHeight;
   }
}
</script>
</head>
<blockquote>
<h2><font color="#0000FF">System Log</font></h2>

<body>
  <table border=0 width="550" cellspacing=0 cellpadding=0>
  <tr><font size=2>
	This page can be used to set remote log server and show the system log.
  </tr>
  <tr><hr size=1 noshade align=top></tr>
  <form action=/goform/formSysLog method=POST name="formSysLog">

  <tr><td width="200" ><font size=2><b><input type="checkbox" name="logEnabled"  <%log=getIndex("logEnabled"); log=log&1; if (log==1) write("checked"); %> value="ON" onClick="updateState()">&nbsp;&nbsp;Enable Log</b></font>
  </td></tr><br>

  <table border="0" width=600>
  <tr><td width="25%">&nbsp;&nbsp;&nbsp;&nbsp;<input type="checkbox" name="syslogEnabled" <%log=getIndex("logEnabled"); log=log>>1; log=log&1; if (log == 1) write("checked");%> value="ON" onClick="updateStateSys()"> <font size=2><b> system all </b></font><br></td> 
      <td width="25%"><input type="checkbox" name="wlanlogEnabled" <%log=getIndex("logEnabled"); log=log>>2; log=log&1; if (log == 1) write("checked"); %> value="ON" ><font size=2><b>wireless</b></font><br></td> 
      <td width="25%"><input type="checkbox" name="doslogEnabled"  <%log=getIndex("logEnabled"); log=log>>3; log=log&1; if (log == 1) write("checked"); %> value="ON" ><font size=2><b>DoS</b></font><br></td>
	<%
	if (getIndex("isMeshDefined") == 1) 
	{
		write("<td width=\"25%\"><input type=\"checkbox\" name=\"meshlogEnabled\" ");
		log =getIndex("logEnabled"); log=log>>4; log=log&1;
		if (log == 1) write("checked"); 
		write(" value=\"ON\" ><font size=2><b>11s</b></font><br></td>"); 
	}
	%>
  </tr>
   </table>
  <table border="0" width=600>
  <tr><td width="40%">&nbsp;&nbsp;&nbsp;&nbsp;<input type="checkbox" name="rtLogEnabled"  <% if (getIndex("rtLogEnabled")) write("checked"); %> value="ON" onClick="updateStateRemote()"> <font size=2><b>Enable Remote Log</b></font><br></td>
   	<td width="60%"><b><font size=2>Log Server IP Address:</b>&nbsp;&nbsp;
	<input type="text" name="logServer" value="<% getInfo("rtLogServer"); %>" size="13" maxlength="16"></td>
  </tr>
  </table>
   <tr><td height=10> </td> </tr>
   <tr><td colspan=2><p><input type="submit" value="Apply Changes" name="Apply" onClick="return saveClick()" >&nbsp;&nbsp; </td></tr>

  <p>
  <input type="hidden" value="/syslog.asp" name="submit-url">
  <textarea rows="15" name="msg" cols="80" wrap="virtual"><% sysLogList(); %></textarea>
  <script>
   scrollElementToEnd(this.formSysLog.msg);
  </script>
  <p>
  <input type="button" value="Refresh" name="refresh" onClick="javascript: window.location.reload()">
  &nbsp;&nbsp; <input type="submit" value=" Clear " name="clear">
</form>
</table>
</body>
</blockquote>
<script>
	updateState();	
</script>
</html>


