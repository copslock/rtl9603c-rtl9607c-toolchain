<html>
<! Copyright (c) Realtek Semiconductor Corp., 2003. All Rights Reserved. ->
<head>
<meta http-equiv="Content-Type" content="text/html">
<title>Wireless Access Control</title>
<script type="text/javascript" src="common.js"> </script>
<script>
function addClick()
{
  var str = document.formWlAcAdd.mac.value;

  if (document.formWlAcAdd.wlanAcEnabled.selectedIndex == 0)
	return true;

  if ( str.length == 0)
  	return true;

  if ( str.length < 12) {
	alert("Input MAC address is not complete. It should be 12 digits in hex.");
	document.formWlAcAdd.mac.focus();
	return false;
  }

  for (var i=0; i<str.length; i++) {
    if ( (str.charAt(i) >= '0' && str.charAt(i) <= '9') ||
			(str.charAt(i) >= 'a' && str.charAt(i) <= 'f') ||
			(str.charAt(i) >= 'A' && str.charAt(i) <= 'F') )
			continue;

	alert("Invalid MAC address. It should be in hex number (0-9 or a-f).");
	document.formWlAcAdd.mac.focus();
	return false;
  }
  return true;
}


function deleteClick()
{
  acl_num = <% write(getIndex("wlanAcNum")); %> ;
  delNum = 0 ;
  for(i=1 ; i <= acl_num ; i++){
  	if(document.formWlAcDel.elements["select"+i].checked)
  		delNum ++ ;
  }
  if(document.formWlAcAdd.wlanAcEnabled.selectedIndex==1 && delNum==acl_num){
		if ( !confirm('Delete the all entries will cause all client cannot connect to AP.  Sure?') )
			return false;
   }
  else if ( !confirm('Do you really want to delete the selected entry?') ) {
	return false;
  }
  else
	return true;
}

function deleteAllClick()
{
   if(document.formWlAcAdd.wlanAcEnabled.selectedIndex==1){
		if ( !confirm('Delete the all entries will cause all client cannot connect to AP.  Sure?') )
			return false;
   }else if ( !confirm('Do you really want to delete the all entries?') ) {
	return false;
  }
  else
	return true;
}
function disableDelButton()
{
	disableButton(document.formWlAcDel.deleteSelFilterMac);
	disableButton(document.formWlAcDel.deleteAllFilterMac);
}

function enableAc()
{
  enableTextField(document.formWlAcAdd.mac);
  enableTextField(document.formWlAcAdd.comment);
}

function disableAc()
{
  disableTextField(document.formWlAcAdd.mac);
  disableTextField(document.formWlAcAdd.comment);
}

function updateState()
{
  wlanDisabled = <% write(getIndex("wlanDisabled")); %> ;
  wlanMode = <% write(getIndex("wlanMode")); %>;
  
  if(wlanDisabled || wlanMode == 1 || wlanMode ==2){
	disableDelButton();
	disableButton(document.formWlAcDel.reset);
	disableButton(document.formWlAcAdd.reset);
	disableButton(document.formWlAcAdd.addFilterMac);
  	disableTextField(document.formWlAcAdd.wlanAcEnabled);
  	disableAc();
  } 
  else{
  if (document.formWlAcAdd.wlanAcEnabled.selectedIndex)
 	enableAc();
  else
  	disableAc();
  }

}

</script>
</head>
<body>
<blockquote>
<h2><font color="#0000FF">Wireless Access Control <% if (getIndex("wlan_num") > 1) write("-wlan"+(getIndex("wlan_idx")+1)); %></font></h2>

<table border=0 width="500" cellspacing=4 cellpadding=0>
<tr><font size=2>
 If you choose 'Allowed Listed', only those clients whose wireless MAC
 addresses are in the access control list will be able to connect to your
 Access Point. When 'Deny Listed' is selected, these wireless clients on 
 the list will not be able to connect the Access Point.
</font></tr>

<form action=/goform/formWlAc method=POST name="formWlAcAdd">
<tr><hr size=1 noshade align=top><br></tr>

<!--
<tr><font size=2><b>
   <input type="checkbox" name="wlanAcEnabled" value="ON" <% if (getIndex("wlanAcEnabled")) write("checked");
   %> onclick="updateState()">&nbsp;&nbsp;Enable Wireless Access Control</b>
</tr>
-->
<tr> <font size=2><b>
   	Wireless Access Control Mode: &nbsp;&nbsp;&nbsp;&nbsp;
	<select size="1" name="wlanAcEnabled" onclick="updateState()">
          <option value="0" >Disable</option>
          <option value="1" selected >Allow Listed</option>
          <option value="2" >Deny Listed</option>
        </select></font></b>
	<script>
	document.formWlAcAdd.wlanAcEnabled.selectedIndex = <% write(getIndex("wlanAcEnabled")); %> ;
	</script>
</tr>
<tr><td>

     <p><font size=2><b>MAC Address: </b> <input type="text" name="mac" size="15" maxlength="12">&nbsp;&nbsp;
   	<b><font size=2>Comment: </b> <input type="text" name="comment" size="16" maxlength="20"></font>
     </p>
     <p><input type="submit" value="Apply Changes" name="addFilterMac" onClick="return addClick()">&nbsp;&nbsp;
        <input type="reset" value="Reset" name="reset">&nbsp;&nbsp;&nbsp;
        <input type="hidden" value="/wlactrl.asp" name="submit-url">
     </p>
  </form>
</table>
<br>
<form action=/goform/formWlAc method=POST name="formWlAcDel">
  <table border="0" width=440>
  <tr><font size=2><b>Current Access Control List:</b></font></tr>
  <% wlAcList(); %>
  </table>
  <br>
  <input type="submit" value="Delete Selected" name="deleteSelFilterMac" onClick="return deleteClick()">&nbsp;&nbsp;
  <input type="submit" value="Delete All" name="deleteAllFilterMac" onClick="return deleteAllClick()">&nbsp;&nbsp;&nbsp;
  <input type="reset" value="Reset" name="reset">
  <input type="hidden" value="/wlactrl.asp" name="submit-url">
 <script>
   	<% entryNum = getIndex("wlanAcNum");
   	  if ( entryNum == 0 ) {
      	  	write( "disableDelButton();" );
       	  } %>
	updateState();
 </script>
</form>

</blockquote>
</body>
</html>
