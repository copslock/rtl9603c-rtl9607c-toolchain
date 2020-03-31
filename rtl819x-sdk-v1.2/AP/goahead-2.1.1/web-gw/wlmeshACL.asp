<html>
<! Copyright (c) Realtek Semiconductor Corp., 2003~2005. All Rights Reserved. ->
<head>
<meta http-equiv="Content-Type" content="text/html">
<title>Mesh Access Control List</title>
<script type="text/javascript" src="util_gw.js"> </script>
<script>
// _MESH_ACL_ENABLE_ Below for Access Control List
function addClick()
{
	var str = document.formMeshACLSetup.aclmac.value;

	if (document.formMeshACLSetup.meshAclEnabled[0].checked) // ACL mode = disable
		return true;

	if ( str.length == 0)
		return true;

	if ( str.length < 12) {
		alert("Input MAC address is not complete. It should be 12 digits in hex.");
		document.formMeshACLSetup.aclmac.focus();
		return false;
	}

	for (var i=0; i<str.length; i++) {
		if ( (str.charAt(i) >= '0' && str.charAt(i) <= '9') ||
			(str.charAt(i) >= 'a' && str.charAt(i) <= 'f') ||
			(str.charAt(i) >= 'A' && str.charAt(i) <= 'F') )
			continue;

		alert("Invalid MAC address. It should be in hex number (0-9 or a-f).");
		document.formMeshACLSetup.aclmac.focus();
		return false;
	}
	return true;
}


function deleteClick()
{
	var acl_num = <% write(getIndex("meshAclNum")); %> ;
	var delNum = 0 ;
	for(i=1 ; i <= acl_num ; i++){
		if(document.formMeshACLSetup.elements["select"+i].checked)
			delNum ++ ;
	}
	
	if(document.formMeshACLSetup.meshAclEnabled[1].checked && delNum==acl_num){ // ACL mode = Allow list
		if ( !confirm('Delete the all entries will cause all peer cannot connect to MP.  Sure?') )
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
	if(document.formMeshACLSetup.meshAclEnabled[1].checked){ // ACL mode = Allow list
		if ( !confirm('Delete the all entries will cause all peer cannot connect to MP.  Sure?') )
			return false;
	}else if ( !confirm('Do you really want to delete the all entries?') ) {
		return false;
	}
	else
		return true;
}
function disableDelButton()
{
	disableButton(document.formMeshACLSetup.deleteSelMeshAclMac);
	disableButton(document.formMeshACLSetup.deleteAllMeshAclMac);
}

function enableAc()
{
	enableTextField(document.formMeshACLSetup.aclmac);
	enableTextField(document.formMeshACLSetup.aclcomment);
}

function disableAc()
{
	disableTextField(document.formMeshACLSetup.aclmac);
	disableTextField(document.formMeshACLSetup.aclcomment);
}

function updateState()
{
	wlanDisabled = <% write(getIndex("wlanDisabled")); %> ;
	wlanMode = <% write(getIndex("wlanMode")); %>;
  
	if(wlanDisabled || wlanMode < 4){
		disableDelButton();
		disableButton(document.formMeshACLSetup.resetAclAdd);
		disableButton(document.formMeshACLSetup.resetAclDel);
		disableButton(document.formMeshACLSetup.addMeshAclMac);
		disableRadioGroup(document.formMeshACLSetup.meshAclEnabled);  
		disableAc();
	} 
	else
	{
		if (document.formMeshACLSetup.meshAclEnabled[0].checked) // ACL mode = disable
			disableAc();
		else
			enableAc();
	}

}

</script>
</head>
<blockquote>
<h2><font color="#0000FF">Access Control List for Mesh Network</font></h2>
<body>
	<form action=/goform/formMeshACLSetup method=POST name="formMeshACLSetup">
	<!-- _MESH_ACL_ENABLE_ Below for Access Control List -->
	<table border=0 width="540" cellspacing=4 cellpadding=0>
		<tr><font size=2>
		If you choose 'Allowed Listed', only those mesh nodes whose wireless MAC addresses are in the access control list will be able to connect to Mesh network. When 'Deny Listed' is selected, those mesh nodes in the list will not be able to create connections.
		</font></tr>
		<tr><hr size=1 noshade align=top></tr>
		<tr><br></tr>
		<tr><font size=2><b>
		   	Mode: &nbsp;&nbsp;&nbsp;&nbsp;
			<input type="radio" name="meshAclEnabled" value="0"<% if (getIndex("meshAclEnabled")==0) write("checked"); %> onclick="updateState()">Disable&nbsp;&nbsp;
			<input type="radio" name="meshAclEnabled" value="1"<% if (getIndex("meshAclEnabled")==1) write("checked"); %> onclick="updateState()">Allow&nbsp;&nbsp;
			<input type="radio" name="meshAclEnabled" value="2"<% if (getIndex("meshAclEnabled")==2) write("checked"); %> onclick="updateState()">Deny
			</font></b>
		</tr>

		<tr><td>
			<p><font size=2><b>MAC Address: </b> <input type="text" name="aclmac" size="15" maxlength="12">&nbsp;&nbsp;
				<b><font size=2>Comment: </b> <input type="text" name="aclcomment" size="16" maxlength="20"></font>
			</p>
		
		<p><input type="submit" value="Apply Changes" name="addMeshAclMac" onClick="return addClick()">&nbsp;&nbsp;
			<input type="reset" value="Reset" name="resetAclAdd">&nbsp;&nbsp;&nbsp;
		</p>
	</table>
	<br>

	<table border="0" width=440>
		<tr><font size=2><b>Current Access Control List:</b></font></tr>
		<% wlMeshAcList(); %>
	</table>
	<br>
	<input type="hidden" value="/wlmeshACL.asp" name="mesh-url">
	<input type="reset" value="Reset" name="resetAclDel">&nbsp;&nbsp;
	<input type="submit" value="Delete Selected" name="deleteSelMeshAclMac" onClick="return deleteClick()">&nbsp;&nbsp;
	<input type="submit" value="Delete All" name="deleteAllMeshAclMac" onClick="return deleteAllClick()">&nbsp;&nbsp;&nbsp;

	<script>
		<% entryNum = getIndex("meshAclNum");
		if ( entryNum == 0 ) {
			write( "disableDelButton();" );
		} %>
		updateState();
	</script>
	<!-- Above for Access Control List -->
	</form>
</body>
</html>