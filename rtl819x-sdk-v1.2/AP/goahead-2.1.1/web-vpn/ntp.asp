<html>
<! Copyright (c) Realtek Semiconductor Corp., 2004. All Rights Reserved. ->
<head>
<meta http-equiv="Content-Type" content="text/html">
<title>Time Zone Setting</title>
<script type="text/javascript" src="common.js"> </script>
<script>
function checkEmpty(field){
	if(field.value.length == 0){
		alert(field.name + " field can't be empty\n");
		field.value = field.defaultValue;
		field.focus();
		return false;
	}
	else
		return true;
}
function checkNumber(field){
    str =field.value ;
    for (var i=0; i<str.length; i++) {
    	if ( (str.charAt(i) >= '0' && str.charAt(i) <= '9'))
                        continue;
	field.value = field.defaultValue;
        alert("Invalid " +field.name + " Number. It should be in  number (0-9).");
        return false;
    }	
	return true;
}
function saveChanges(form){
	if((checkEmpty(form.year)& checkEmpty(form.month) & checkEmpty(form.hour)
	 & checkEmpty(form.day) &checkEmpty(form.minute) & checkEmpty(form.second))== false)
	 	return false;

	if((checkNumber(form.year)& checkNumber(form.month) & checkNumber(form.hour)
	 & checkNumber(form.day) &checkNumber(form.minute) & checkNumber(form.second))== false)
	 	return false;
	if(form.month.value == '0'){
		form.month.value = form.month.defaultValue;
        	alert("Invalid month Number. It should be in  number (1-9).");
		return false;
	}
	if (form.enabled.checked) {
		if(form.ntpServerIp2.value != ""){
			if ( checkIpAddr(form.ntpServerIp2, 'Invalid IP address') == false )
		    	return false;
		}
		else
			form.ntpServerIp2.value = "0.0.0.0" ;	
	}	
	return true;
}
function updateState(form)
{
	if(form.enabled.checked){
		enableTextField(form.ntpServerIp1);
		if(form.ntpServerIp2 != null)
			enableTextField(form.ntpServerIp2);
	}
	else{
		disableTextField(form.ntpServerIp1);
		if(form.ntpServerIp2 != null)
			disableTextField(form.ntpServerIp2);
	}
}
</script>
</head>
<body>
<blockquote>
<h2><font color="#0000FF">Time Zone Setting</font></h2>
<table border=0 width="500" cellspacing=0 cellpadding=0>
  <tr><font size=2>
  You can maintain the system time by synchronizing with a public time server over the Internet.
  </tr>
  <tr><hr size=1 noshade align=top></tr>
</table>
<form action=/goform/formNtp method=POST name="time">
<table border="0" width=520>
	<tr>
	<td width ="25%">
	<font size=2> <b> Current Time : </b> </font>
	</td>
	<td width ="75%">
                <font size =2> <b>
                Yr <input type="text" name="year" value="<% getInfo("year"); %>" size="4" maxlength="4">
                Mon <input type="text" name="month" value="<% getInfo("month"); %>" size="2" maxlength="2">
                Day <input type="text" name="day" value="<% getInfo("day"); %>" size="2" maxlength="2">
                Hr <input type="text" name="hour" value="<% getInfo("hour"); %>" size="2" maxlength="2">
                Mn <input type="text" name="minute" value="<% getInfo("minute"); %>" size="2" maxlength="2">
                Sec <input type="text" name="second" value="<% getInfo("second"); %>" size="2" maxlength="2">
                </b> </font>
        </td>
	</tr>
	
	<tr><td width="25%"><font size=2> <b>Time Zone Select : </b> </font></td>
	    <td width="75%">
            <select name="timeZone">
            	<script language="javascript">
            	var i;
            	for(i=0;i<ntp_zone_array.length;i++){
			if (i == ntp_zone_index)
				document.write('<option value="',ntp_zone_array[i].value,'" selected>',ntp_zone_array[i].name,'</option>');
			else
				document.write('<option value="',ntp_zone_array[i].value,'">',ntp_zone_array[i].name,'</option>');
            	}
		</script>
            </select>
	    </td>
	</tr>	
	<tr ><td height=10> </td> </tr>

	<tr><td colspan="2"><font size=2><b>
		<input type="checkbox" name="enabled" 
		value="ON" 
		<% if (getIndex("ntpEnabled")) write("checked"); %>
		ONCLICK=updateState(document.time)>&nbsp;&nbsp;Enable NTP client update </b><br>
	    </td>
	</tr>
	<tr>
	<td width ="25%">
	<font size=2> <b> NTP server : </b> </font>
	</td>
	<td width ="75%">
		<input type="radio" value="0" name="ntpServerId"
		<% if (getIndex("ntpServerId")==0) write("checked"); %>
		></input>
		<select name="ntpServerIp1">
			<option value="192.5.41.41">192.5.41.41    - North America</option>
			<option value="192.5.41.209">192.5.41.209   - North America</option>
			<option value="208.184.49.9">208.184.49.9   - North America</option>
			<option value="131.188.3.220">131.188.3.220  - Europe</option>
			<option value="130.149.17.8">130.149.17.8   - Europe</option>
			<option value="203.60.1.2"> 203.60.1.2  - Australia</option>
			<option value="203.117.180.36"> 203.117.180.36 - Asia Pacific</option>
			</select>
		</td>
	</tr>
	<td width ="25%"> <font size=2><b> </b></font>
	</td>
	<td width ="75%">
	<input type="radio" value="1" name="ntpServerId" 
	<% if (getIndex("ntpServerId")==1) write("checked"); %>
	></input>
	<input type="text" name="ntpServerIp2" size="15" maxlength="15" value=<% getInfo("ntpServerIp2"); %>> <font size=2> (Manual IP Setting) </font>
		</td>
	</tr>
</table>
  <input type="hidden" value="/ntp.asp" name="submit-url">
  <p><input type="submit" value="Apply Change" name="save" onClick="return saveChanges(document.time)">
&nbsp;&nbsp;
  <input type="reset" value="Reset" name="set" >
&nbsp;&nbsp;
  <input type="button" value="Refresh" name="refresh" onClick="javascript: window.location.reload()">
</form>
<script>
		setTimeZone(document.time.timeZone, "<% getInfo("ntpTimeZone"); %>");
		setNtpServer(document.time.ntpServerIp1, "<% getInfo("ntpServerIp1"); %>");	
		updateState(document.time);
</script>
</blockquote>
</font>
</body>

</html>
