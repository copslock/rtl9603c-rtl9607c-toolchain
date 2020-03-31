<html>
<! Copyright (c) Realtek Semiconductor Corp., 2003. All Rights Reserved. ->
<head>
<meta http-equiv="Content-Type" content="text/html">
<title>IP Filtering</title>
<script type="text/javascript" src="common.js"> </script>
<script>
function addClick()
{
  if (!document.formFilterAdd.enabled.checked)
  	return true;

  if (document.formFilterAdd.ip.value=="" && document.formFilterAdd.comment.value=="" )
	return true;

  if ( checkIpAddr(document.formFilterAdd.ip, 'Invalid IP address') == false )
	    return false;
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
	disableButton(document.formFilterDel.deleteSelFilterIp);
	disableButton(document.formFilterDel.deleteAllFilterIp);
}

function updateState()
{
  if (document.formFilterAdd.enabled.checked) {
 	enableTextField(document.formFilterAdd.ip);
	enableTextField(document.formFilterAdd.protocol);
	enableTextField(document.formFilterAdd.comment);
  }
  else {
 	disableTextField(document.formFilterAdd.ip);
  	disableTextField(document.formFilterAdd.protocol);
	disableTextField(document.formFilterAdd.comment);
  }
}


</script>
</head>

<body>
<blockquote>
<h2><font color="#0000FF">IP Filtering</font></h2>

<table border=0 width="500" cellspacing=4 cellpadding=0>
<tr><td><font size=2>
 Entries in this table are used to restrict certain types of data packets from your local
 network to Internet through the Gateway.  Use of such filters can be helpful in securing
 or restricting your local network.
</font></td></tr>

<tr><td><hr size=1 noshade align=top></td></tr>

<form action=/goform/formFilter method=POST name="formFilterAdd">
<tr><td><font size=2><b>
   	<input type="checkbox" name="enabled" value="ON" <% if (getIndex("ipFilterEnabled")) write("checked");
   	%> ONCLICK=updateState()>&nbsp;&nbsp;Enable IP Filtering</b><br>
    </td>
</tr>

<tr><td>
     <p><font size=2><b>Loal IP Address: </b> <input type="text" name="ip" size="10" maxlength="15">&nbsp;&nbsp;
  	<b><font size=2>Protocol:</b> <select name="protocol">
    		<option select value="0">Both</option>
    		<option value="1">TCP</option>
    		<option value="2">UDP</option>
    		</select></font>&nbsp;&nbsp
  	<b><font size=2>Comment: </b> <input type="text" name="comment" size="12" maxlength="20"></font>
     </p>
     <p><input type="submit" value="Apply Changes" name="addFilterIp" onClick="return addClick()">&nbsp;&nbsp;
        <input type="reset" value="Reset" name="reset">
        <input type="hidden" value="/ipfilter.asp" name="submit-url">
     </p>
   </td></tr>
  <script> updateState(); </script>
</form>
</table>

<br>
<form action=/goform/formFilter method=POST name="formFilterDel">
  <table border="0" width=500>
  <tr><font size=2><b>Current Filter Table:</b></font></tr>
  <% ipFilterList(); %>
  </table>
  <br>
  <input type="submit" value="Delete Selected" name="deleteSelFilterIp" onClick="return deleteClick()">&nbsp;&nbsp;
  <input type="submit" value="Delete All" name="deleteAllFilterIp" onClick="return deleteAllClick()">&nbsp;&nbsp;&nbsp;
  <input type="reset" value="Reset" name="reset">
  <input type="hidden" value="/ipfilter.asp" name="submit-url">
 <script>
   	<% entryNum = getIndex("ipFilterNum");
   	  if ( entryNum == 0 ) {
      	  	write( "disableDelButton();" );
       	  } %>
 </script>
</form>

</blockquote>
</body>
</html>
