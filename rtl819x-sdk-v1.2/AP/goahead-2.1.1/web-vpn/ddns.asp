<html>
<! Copyright (c) Realtek Semiconductor Corp., 2003. All Rights Reserved. ->
<head>
<meta http-equiv="Content-Type" content="text/html">
<title>Time Zone Setting</title>
</head>
<script>

function disableButton (button,val) {
  if (document.all || document.getElementById)
    button.disabled = val;
  else if (button) {
    button.oldOnClick = button.onclick;
    button.onclick = null;
    button.oldValue = button.value;
    button.value = 'DISABLED';
  }
}

function disableDdnsButton(val)
{
	disableButton(document.ddns.ddnsType, val);
	disableButton(document.ddns.ddnsDomainName, val);
	disableButton(document.ddns.ddnsUser, val);
	disableButton(document.ddns.ddnsPassword, val);

}

function updateState()
{
	if(document.ddns.ddnsEnabled.checked)
		disableDdnsButton(false);
	else
		disableDdnsButton(true);
}

function ddns_saveChanges()
{
	form = document.ddns ;
	if(form.ddnsEnabled.checked){
		if(form.ddnsDomainName.value == ""){
			alert("Domain Name can't be empty");
			form.ddnsDomainName.value = form.ddnsDomainName.defaultValue;
			form.ddnsDomainName.focus();
			return false ;
		}
		if(form.ddnsUser.value == ""){
			alert("User Name/Email can't be empty");
			form.ddnsUser.value = form.ddnsUser.defaultValue;
			form.ddnsUser.focus();
			return false ;
		}
		if(form.ddnsPassword.value == ""){
			alert("Password/Key can't be empty");
			form.ddnsPassword.value = form.ddnsPassword.defaultValue;
			form.ddnsPassword.focus();
			return false ;
		}
	}
	return true;
}
</script>
<body >
<blockquote>
<h2><font color="#0000FF">Dynamic DNS&nbsp; Setting</font></h2>


<table border=0 width="500" cellspacing=0 cellpadding=0>
  <tr><font size=2>
   Dynamic DNS is a service, that provides you with a valid, unchanging, internet domain name (an URL) to go with that (possibly everchanging) IP-address. 
  </tr>
  <tr><hr size=1 noshade align=top></tr>
</table>
<form action=/goform/formDdns method=POST name="ddns">
<table border="0" width=600>

	<tr><td colspan="2"><font size=2><b>
		<input type="checkbox" name="ddnsEnabled" 
		value="ON" 
		<% if (getIndex("ddnsEnabled")) write("checked"); %>
		ONCLICK=updateState()>&nbsp;&nbsp;Enable DDNS </b><br>
	    </td>
	</tr>
	<tr ><td height=10> </td> </tr>
	<tr>
	<td width ="20%">
	<font size=2> <b> Service Provider : </b> </font>
	</td>
	<td width ="85%">
		<select name="ddnsType">
			<option value=0 <% if (getIndex("ddnsType") == 0) write("selected"); %>>DynDNS </option>
			<option value=1 <% if (getIndex("ddnsType") == 1) write("selected"); %>>TZO </option>			
		</select>
	</td>
	</tr>
	<tr><td width="20%"><font size=2> <b>Domain Name : </b> </font></td>
	    <td width="85%">
 			<font size=2><input type="text" name="ddnsDomainName" size="20" maxlength="50"			       value=<% getInfo("ddnsDomainName"); %>></font>
	    </td>
	</tr>
	<tr>
	<td width ="20%">
	<font size=2> <b> User Name/Email: </b> </font>
	</td>
	<td width ="85%">
		<font size=2><input type="text" name="ddnsUser" size="20" maxlength="50" 
		value="<% getInfo("ddnsUser"); %>"	></font>
	</td>
	</tr>
	
	<tr>
	<td width ="20%">
	<font size=2> <b> Password/Key: </b> </font>
	</td>
	<td width ="85%">
		<font size=2><input type="password" name="ddnsPassword" size="20" maxlength="30"
		value="<% getInfo("ddnsPassword"); %>"></font>
	</td>
	</tr>
	<tr>
		<td  height =5>
		</td>
	</tr>
	<tr>
		<td colspan="2" ><font size=2><i>Note:
		<br>For TZO, you can have a 30 days free trial 
			<a href="http://www.tzo.com/MainPageWebClient/clientsignup.html">here </a> </href>
			or manage your TZO account in 
            <a href="https://controlpanel.tzo.com/cgi-bin/tzopanel.exe"> 
            control panel</a></herf>
             <br>For DynDNS, you can create your DynDNS account  
			<a href=http://www.dyndns.org/account/">here </a> </href></a></herf> </i>
            </font>            
        </td>
        
	</tr>	
</table>
  <p>
  <input type="hidden" value="/ddns.asp" name="submit-url">
  <input type="submit" value="Apply Change" name="apply" onClick="return ddns_saveChanges()">
&nbsp;&nbsp;
  <input type="reset" value="Reset" name="set" >
<script>
		updateState();
  </script>
</form>
</blockquote>
</font>
</body>

</html>
