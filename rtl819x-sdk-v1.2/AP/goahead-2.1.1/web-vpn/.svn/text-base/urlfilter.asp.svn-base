<html>
<! Copyright (c) Realtek Semiconductor Corp., 2003. All Rights Reserved. ->
<head>
<meta http-equiv="Content-Type" content="text/html">
<title>URL Filtering</title>
<script type="text/javascript" src="common.js"> </script>
<script>
function addClick()
{
  if (!document.formFilterAdd.enabled.checked)
  	return true;

  if (document.formFilterAdd.url.value=="")
	return true;

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
	disableButton(document.formFilterDel.deleteSelFilterUrl);
	disableButton(document.formFilterDel.deleteAllFilterUrl);
}

function updateState()
{
  if (document.formFilterAdd.enabled.checked)
 	enableTextField(document.formFilterAdd.url);  
  else
 	disableTextField(document.formFilterAdd.url); 
}

</script>
</head>

<body>
<blockquote>
<h2><font color="#0000FF">URL Filtering</font></h2>

<table border=0 width="500" cellspacing=4 cellpadding=0>
<tr><td><font size=2>
 URL filter is used to deny LAN users from accessing the internet. Block those URLs which contain keywords listed below.
</font></td></tr>

<tr><td><hr size=1 noshade align=top></td></tr>

<form action=/goform/formFilter method=POST name="formFilterAdd">
<tr><td><font size=2><b>
   	<input type="checkbox" name="enabled" value="ON" <% if (getIndex("urlFilterEnabled")) write("checked");
   	%> ONCLICK=updateState()>&nbsp;&nbsp;Enable URL Filtering</b><br>
    </td>
</tr>

<tr><td>
     <font size=2><b>URL Address: </b> <input type="text" name="url" size="30" maxlength="30">&nbsp;&nbsp;</font>
     <p><input type="submit" value="Apply Changes" name="addFilterUrl" onClick="return addClick()">&nbsp;&nbsp;
        <input type="reset" value="Reset" name="reset">
        <input type="hidden" value="/urlfilter.asp" name="submit-url">
     </p>
     </td>
<tr>
  <script> updateState(); </script>
</form>
</table>

<br>
<form action=/goform/formFilter method=POST name="formFilterDel">
  <table border="0" width=500>
  <tr><font size=2><b>Current Filter Table:</b></font></tr>
  <% urlFilterList(); %>
  </table>
  <br>
  <input type="submit" value="Delete Selected" name="deleteSelFilterUrl" onClick="return deleteClick()">&nbsp;&nbsp;
  <input type="submit" value="Delete All" name="deleteAllFilterUrl" onClick="return deleteAllClick()">&nbsp;&nbsp;&nbsp;
  <input type="reset" value="Reset" name="reset">
  <input type="hidden" value="/urlfilter.asp" name="submit-url">
 <script>
   	<% entryNum = getIndex("urlFilterNum");
   	  if ( entryNum == 0 ) {
      	  	write( "disableDelButton();" );
       	  } %>
 </script>
</form>

</blockquote>
</body>
</html>
