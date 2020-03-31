<html>
<! Copyright (c) Realtek Semiconductor Corp., 2003~2005. All Rights Reserved. ->
<head>
<meta http-equiv="Content-Type" content="text/html">
<title>System Command</title>
<script type="text/javascript" src="util_gw.js"> </script>
<script>

var defPskLen=new Array();
var defPskFormat=new Array();
var wlan_idx= <% write(getIndex("wlan_idx")); %> ;
var isNewMeshUI =  <% write(getIndex("isNewMeshUI")); %> ;
var wlanDisabled = <% write(getIndex("wlanDisabled")); %> ;

function enable_allEncrypt()
{
  form = document.formMeshSetup ;
  enableTextField(form.elements["method"+wlan_idx]);
  enableTextField(form.elements["pskFormat"+wlan_idx]);
  enableTextField(form.elements["pskValue"+wlan_idx]);
  //enableButton(document.formEncrypt.save);
  //enableButton(document.formEncrypt.reset);  
}

function disable_allEncrypt()
{
  form = document.formMeshSetup ;
  disableTextField(form.elements["method"+wlan_idx]);
  disableTextField(form.elements["pskFormat"+wlan_idx]);
  disableTextField(form.elements["pskValue"+wlan_idx]);
  //disableButton(document.formEncrypt.save);
  //disableButton(document.formEncrypt.reset);  
}

function disable_wpa()
{
  form = document.formMeshSetup ;
  disableTextField(form.elements["pskFormat"+wlan_idx]);
  disableTextField(form.elements["pskValue"+wlan_idx]);
}

function enable_wpa()
{  
  form = document.formMeshSetup ;
  enableTextField(form.elements["pskFormat"+wlan_idx]);
  enableTextField(form.elements["pskValue"+wlan_idx]);
}

function checkState(wlan_id)
{
  form = document.formMeshSetup ;
  	if (form.elements["method"+wlan_id].selectedIndex==1)
  		enable_wpa();
  	else
  		disable_wpa();

}

function switch_Mesh(option)
{
	form = document.formMeshSetup ;
	var mesh_enable = <% write(getIndex("wlanMeshEnabled")); %>;

	if( option == 1 )	//switch ON
	{
		enableButton(document.formMeshSetup.meshID);
		if( mesh_enable == 0 )
                {
                        disableButton(document.formMeshSetup.showACL);
                        disableButton(document.formMeshSetup.showInfo);
                }
		else
		{
			enableButton(document.formMeshSetup.showACL);
                        enableButton(document.formMeshSetup.showInfo);
		}
		enableButton(document.formMeshSetup.reset);
  		enableTextField(form.elements["method"+wlan_idx]);
		checkState(wlan_idx);
	}
	else
	{
		disableButton(document.formMeshSetup.meshID);
		disableButton(document.formMeshSetup.showACL);
		disableButton(document.formMeshSetup.showInfo);
		disableButton(document.formMeshSetup.reset);
		disableTextField(form.elements["method"+wlan_idx]);
	}
}

function updateState2(){
	wlanMode = <% write(getIndex("wlanMode")); %>;
	
	if( wlanMode <4 || wlanDisabled )
	{		
		disableButton(document.formMeshSetup.save);
		disableButton(document.formMeshSetup.reset);	
		disableButton(document.formMeshSetup.meshID);
		disableButton(document.formMeshSetup.showACL);
		disableButton(document.formMeshSetup.showInfo);
		disableTextField(document.formMeshSetup.wlanMeshEnable);
		disable_allEncrypt();
		return;
	}
	else
	{
		enableTextField(document.formMeshSetup.wlanMeshEnable);
		switch_Mesh(document.formMeshSetup.wlanMeshEnable.checked);
	}
}

function saveChanges_mesh(form, wlan_id)
{  
	  method = form.elements["method"+wlan_id] ;
	  if (method.selectedIndex == 1 )
		return check_wpa_psk(form, wlan_id);
	     return true;
}

function showMeshACLClick(url)
{
	//openWindow(url, 'showMeshACL',620,340 );
	document.location.href = url;
}

function showMeshInfoClick(url)
{
	//openWindow(url, 'showMeshInfo',620,340 );
	document.location.href = url;
}

</script>
</head>
<body>
<blockquote>
<h2><font color="#0000FF">Wireless Mesh Network Setting</font></h2>
<table border=0 width="550" cellspacing=4 cellpadding=0>
<tr><font size=2>
	  Mesh network uses wireless media to communicate with other APs, like the Ethernet does.
	  To do this, you must set these APs in the same channel with the same Mesh ID.
	  The APs should be under AP+MESH/MESH mode.
</font></tr>

<form action=/goform/formMeshSetup method=POST name="formMeshSetup">
<tr><hr size=1 noshade align=top><br></tr>
<!-- new feature:Mesh enable/disable -->
<tr><font size=2><b>
<input type="checkbox" name="wlanMeshEnable" value="ON" <% if (getIndex("wlanMeshEnabled")) write("checked"); %> onClick="updateState2()">&nbsp;&nbsp;Enable Mesh</b></tr>
<table width="550" border="0" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF">
  <tr>
 	<td width="35%"><font size=2><b>Mesh ID:</b></td>
 	<td width="65%"><input type="text" name="meshID" size="33" maxlength="32" value="<% getInfo("meshID"); %>"></td>
	</tr>
  <tr>
  <td width="35%"><font size="2"><b>Encryption:&nbsp;</b></td>
  <td width="65%"><select size="1" name="method<% write(getIndex("wlan_idx")); %>" onChange="checkState(wlan_idx)" >
    <option <% choice = getIndex("meshEncrypt"); if (choice == 0) write("selected"); %> value="0">None</option>    
    <option <% choice = getIndex("meshEncrypt"); if (choice == 4) write("selected"); %> value="4">WPA2 (AES)</option>
    </select></font></td>
  </tr>
  <tr>
  <td width="35%"><font size="2"><b>Pre-Shared Key Format:</b></font> </td>
  <td width="65%"><font size="2"><select size="1" name="pskFormat<% write(getIndex("wlan_idx")); %>">
    <option value="0" <% if (getIndex("meshPskFormat")==0) write("selected");%>>Passphrase</option>
    <option value="1" <% if (getIndex("meshPskFormat")) write("selected");%>>Hex (64 characters)</option>
    </select></font></td>
  </tr>
  <tr>
    <td width="35%"><font size="2"><b>Pre-Shared Key:</b></font> </td>
    <td width="65%"><font size="2"><input type="password" name="pskValue<% write(getIndex("wlan_idx")); %>" size="40" maxlength="64" value=<% getInfo("meshPskValue");%>></font></td>
  </tr>
</table>
    
	<br>
	<input type="hidden" value="/wlmesh.asp" name="mesh-url">
	<input type="submit" value="Apply Changes" name="save" onClick="return saveChanges_mesh(document.formMeshSetup, wlan_idx)">&nbsp;&nbsp;
	<input type="reset" value="  Reset  " name="reset" OnClick="checkState(wlan_idx)" >&nbsp;&nbsp;&nbsp;&nbsp;
	<input type="button" value="Set Access Control" name="showACL" onClick="showMeshACLClick('/wlmeshACL.asp')">&nbsp;
	<input type="button" value="Show Advanced Information" name="showInfo" onClick="showMeshInfoClick('/wlmeshinfo.asp')">&nbsp;&nbsp;
</tr>
</form>
</table>  

  <script>
	updateState2();
	checkState(wlan_idx);
  </script>

</body>
</blockquote>
</html>



