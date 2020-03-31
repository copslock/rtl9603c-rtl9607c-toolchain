<html>
<! Copyright (c) Realtek Semiconductor Corp., 2004. All Rights Reserved. ->
<head>
<meta http-equiv="Content-Type" content="text/html">
<title>System Time Setting</title>
<script type="text/javascript" src="util_gw.js"> </script>
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
	var Month_num;
	var Day_num;
	var Hour_num;
	var Min_num;
	var Sec_num;
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
	Month_num =parseInt(form.month.value, 10);
	Day_num =parseInt(form.day.value, 10);
	Hour_num =parseInt(form.hour.value, 10);
	Min_num =parseInt(form.minute.value, 10);
	Sec_num =parseInt(form.second.value, 10);
	if((Month_num<=0) || (Month_num > 12) || (Day_num <= 0) || (Day_num > 31) || (Hour_num < 0)  || (Hour_num > 23) || (Min_num < 0) || (Min_num > 59) || (Sec_num < 0) || (Sec_num > 59)){
			alert("Invalid Time value!");
		return false;
	}
	
	return true;
}

/** Copy time from the host computer.*/
function copy_computer_time()
{
	var date = new Date();
	document.time.year.value = date.getFullYear();
	document.time.month.value = date.getMonth()+1;
	document.time.day.value = date.getDate();
	document.time.hour.value = date.getHours();
	document.time.minute.value = date.getMinutes();
	document.time.second.value = date.getSeconds();
}
</script>
</head>
<body>
<blockquote>
<h2><font color="#0000FF">System Time Setting</font></h2>
<table border=0 width="500" cellspacing=0 cellpadding=0>
  <tr><font size=2>
  This page allows you setup the current system time.
  </tr>
  <tr><hr size=1 noshade align=top></tr>
</table>
<form action=/goform/formSetTime method=POST name="time">
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

	<tr>
		<td width ="25%">&nbsp;</td>
		<td width ="75%"><input type="button" class="button_submit" id="copy_computer_time_button" value="Copy Computer Time" onclick="copy_computer_time();"/></td>
	</tr>
	
</table>
  <input type="hidden" value="/time.asp" name="submit-url">
  <p><input type="submit" value="Apply Change" name="save" onClick="return saveChanges(document.time)">
&nbsp;&nbsp;
  <input type="button" value="Refresh" name="refresh" onClick="javascript: window.location.reload()">
</form>
</blockquote>
</font>
</body>

</html>
