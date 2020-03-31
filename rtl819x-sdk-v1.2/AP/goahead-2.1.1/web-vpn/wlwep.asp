<html>
<! Copyright (c) Realtek Semiconductor Corp., 2003. All Rights Reserved. ->
<head>
<meta http-equiv="Content-Type" content="text/html">
<title>WEP Key Setup</title>
<script type="text/javascript" src="common.js"> </script>
<script>
var wlan_idx= <% write(getIndex("wlan_idx")); %> ;

// for WPS --------------------------------------------->>
var wps_encrypt_old=<% write(getIndex("encrypt"));%>;						
var wps_disabled='<% write(getIndex("wscDisable"));%>';
var wps_config_by_registrar='<% write(getIndex("wps_by_reg"));%>';
var wps_wep_keylen_old='<% write(getIndex("wep"));%>';
//<<---------------------------------------------- for WPS

function updateFormat(form, wlan_idx)
{
  format= form.elements["format"+wlan_idx];
  if (form.elements["length"+wlan_idx].selectedIndex == 0) {
	format.options[0].text = 'ASCII (5 characters)';
	format.options[1].text = 'Hex (10 characters)';
  }
  else {
	format.options[0].text = 'ASCII (13 characters)';
	format.options[1].text = 'Hex (26 characters)';
  }
  <% type = getIndex("keyType");
     write("format.options[" + type + "].selected = \'true\';");
  %>
  
   setDefaultKeyValue(form, wlan_idx);
}
</script>
</head>

<body>
<blockquote>
<h2><font color="#0000FF">Wireless WEP Key Setup <% if (getIndex("wlan_num") > 1) write("-wlan"+(getIndex("wlan_idx")+1)); %></font></h2>

<form action=/goform/formWep method=POST name="formWep">
<!-- for WPS -->
<INPUT type=hidden name=wps_clear_configure_by_reg<% write(getIndex("wlan_idx")); %> value=0>

<table border=0 width="500" cellspacing=4>
  <tr><font size=2>
    This page allows you setup the WEP key value. You could choose use 64-bit or
    128-bit as the encryption key, and select ASCII or Hex as the format of input value.
   </tr>
  <tr><hr size=1 noshade align=top></tr>
      <input type="hidden" name="wepEnabled<% write(getIndex("wlan_idx")); %>" value="ON" checked>
  <tr>
      <td width="15%"><font size=2><b>Key Length:</b></td>
      <td width="40%"><font size=2><select size="1" name="length<% write(getIndex("wlan_idx")); %>" ONCHANGE="lengthClick(document.formWep, wlan_idx)">
      			<option value=1 <% if ( getIndex("wep") != 2) write("selected"); %>>64-bit</option>
			<option value=2 <% if ( getIndex("wep") == 2) write("selected"); %>>128-bit</option>
      </select></td>
  </tr>

  <tr>
      <td width="15%"><font size=2><b>Key Format:</b></td>
      <td width="40%"><font size=2><select size="1" name="format<% write(getIndex("wlan_idx")); %>" ONCHANGE="setDefaultKeyValue(document.formWep, wlan_idx)">
     	<option value=1>ASCII</option>
	<option value=2>Hex</option>
       </select></td>
  </tr>

  <tr>
      <td width="15%"><font size=2><b>Default Tx Key:</b></td>
      <td width="40%"><select size="1" name="defaultTxKeyId<% write(getIndex("wlan_idx")); %>">
      <option <% choice = getIndex("defaultKeyId"); if (choice == 1) write("selected"); %> value="1">Key 1</option>
      <option <% choice = getIndex("defaultKeyId"); if (choice == 2) write("selected"); %> value="2">Key 2</option>
      <option <% choice = getIndex("defaultKeyId"); if (choice == 3) write("selected"); %> value="3">Key 3</option>
      <option <% choice = getIndex("defaultKeyId"); if (choice == 4) write("selected"); %> value="4">Key 4</option>
      </select></td>
  </tr>
  <tr>
     <td width="15%"><font size=2><b>Encryption Key 1:</b></td>
     <td width="40%"><font size=2>
     	<input type="text" name="key1<% write(getIndex("wlan_idx")); %>" size="26" maxlength="26">
     </td>
  </tr>
  <tr>
     <td width="15%"><font size=2><b>Encryption Key 2:</b></td>
     <td width="40%"><font size=2>
     	<input type="text" name="key2<% write(getIndex("wlan_idx")); %>" size="26" maxlength="26">
     </td>
  </tr>
  <tr>
     <td width="15%"><font size=2><b>Encryption Key 3:</b></td>
     <td width="40%"><font size=2>
     	<input type="text" name="key3<% write(getIndex("wlan_idx")); %>" size="26" maxlength="26">
     </td>
  </tr>
  <tr>
     <td width="15%"><font size=2><b>Encryption Key 4:</b></td>
     <td width="40%"><font size=2>
     	<input type="text" name="key4<% write(getIndex("wlan_idx")); %>" size="26" maxlength="26">
     </td>
  </tr>

  <tr>
     <td colspan=2 width="100%"><br>
     <input type="hidden" value="/wlwep.asp" name="submit-url">
     <input type="submit" value="Apply Changes" name="save" onClick="return saveChanges_wep(document.formWep, wlan_idx)">&nbsp;&nbsp;
     <input type="button" value="Close" name="close" OnClick=window.close()>&nbsp;&nbsp;
     <input type="reset" value="Reset" name="reset">
     </td>
  </tr>

<script>
   updateFormat(document.formWep, wlan_idx);
</script>


</table>
</form>

</blockquote>
</body>
</html>
