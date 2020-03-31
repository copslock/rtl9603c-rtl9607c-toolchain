<html>
<! Copyright (c) Realtek Semiconductor Corp., 2003. All Rights Reserved. ->
<head>
<meta http-equiv="Content-Type" content="text/html">
<title>VPN Advanced Setting</title>
<% language=javascript %>
<script type="text/javascript" src="common.js"></script>
<script>
function saveChanges(ok)
{

	openerfm =  window.opener.document.vpn;
	curfm = document.ike ;
	if(ok){
			
		//check keylife
		  d2 = parseInt(curfm.ikeLifeTime.value, 10);
		  if ( curfm.ikeLifeTime.value == "" ||
			validateKey(curfm.ikeLifeTime.value) == 0  ||
			(d2 > 28800 || d2 < 3600) ) {
			alert("Invalid IKE Life Time! You should set a value between 3600-28800 (1h-8h).");
			curfm.ikeLifeTime.value = openerfm.ikeLifeTime.defaultValue;
			curfm.ikeLifeTime.focus();
			return false;
		  }
		  d2 = parseInt(curfm.ipsecLifeTime.value, 10);
		  if ( curfm.ipsecLifeTime.value == "" 
			|| validateKey(curfm.ipsecLifeTime.value) == 0 ||
			(d2 > 86400 || d2 < 60) ){
			alert("Invalid Ipsec Key Life! You should set a value between 60-86400 (1m-24h).");
			curfm.ipsecLifeTime.value = openerfm.ipsecLifeTime.defaultValue;
			curfm.ipsecLifeTime.focus();
			return false;
		  }      
		   
		if(window.opener != null){
			openerfm.ikeLifeTime.value = curfm.ikeLifeTime.value ;
			openerfm.ikeEncr.value =  curfm.ikeEncr.value;
			openerfm.ikeAuth.value = curfm.ikeAuth.selectedIndex;
			openerfm.ikeKeyGroup.value = curfm.ikeKeyGroup.selectedIndex;
			openerfm.ipsecLifeTime.value = curfm.ipsecLifeTime.value;
			openerfm.ipsecEspEncr.selectedIndex = curfm.ipsecEncr.selectedIndex;
			openerfm.ipsecEspAuth.selectedIndex = curfm.ipsecAuth.selectedIndex;

			openerfm.ipsecPfs.value = curfm.ipsecPfs.selectedIndex;
			//window.opener.location.reload();
			openerfm.refresh.click();
		}
		alert('Remember to press "Apply Change" button to save change');
	}
	window.close();	
}
function loadSetting()
{
	openerfm =  window.opener.document.vpn;
	curfm = document.ike ;
	if(window.opener != null){
		curfm.ikeLifeTime.value =  openerfm.ikeLifeTime.value   ;
		curfm.ikeEncr.selectedIndex =  openerfm.ikeEncr.value ;
		curfm.ikeAuth.selectedIndex =  openerfm.ikeAuth.value   ;
		curfm.ikeKeyGroup.selectedIndex =  openerfm.ikeKeyGroup.value   ;
		curfm.ipsecLifeTime.value =  openerfm.ipsecLifeTime.value   ;
		curfm.ipsecEncr.selectedIndex = openerfm.ipsecEspEncr.selectedIndex;
		curfm.ipsecAuth.selectedIndex = openerfm.ipsecEspAuth.selectedIndex;
 		 curfm.ipsecPfs.selectedIndex = openerfm.ipsecPfs.value   ;
	}
}
</script>
</head>

<body>
<blockquote>
<h2><font color="#0000FF">Adavnced VPN Setting for IKE </font></h2>


<table border=0 width="480"  cellspacing=0 cellpadding=0>
  <tr><font size=2>
 This This page is used to provide advanced setting for IKE mode   
 </tr>
  <tr><hr size=1 noshade align=top></tr>
</table>
<form action="" method=POST name="ike">
  <table border="0" width=480>
    <tr>
      <td width="180"><font size="2"><b>Tunnel 1<b></font></td>
      <td width="250">&nbsp;</td>
    </tr>
    <tr>
      <td width="180"><font size="2"><b>Phase 1:</b></font></td>
      <td width="250">&nbsp;</td>
    </tr>

    <tr>
    	<td width="180"><font size=2>&nbsp;&nbsp;&nbsp; Negotiation Mode</td>
      <td width="250"><font size=2>Main mode</font></td>
    </tr>     

    <tr>
    	<td width="180"><font size=2>&nbsp;&nbsp;&nbsp; Encryption Algorithm</td>
      <td width="250"><font size=2>
		<select size="1" name="ikeEncr" >
			<option selected value="0">3DES</option>
			<option value="1">AES128</option>		
		</select>
		</font></td>
    </tr>     

    <tr>
    	<td width="180"><font size=2>&nbsp;&nbsp;&nbsp; Authenticaiton Algorithm</td>
      <td width="250"><font size=2>
		  <select size="1" name="ikeAuth">
			<option selected value="0">MD5</optionL>
			<option value="1">SHA1</option>
		  </select>
      </font></td>
    </tr>     

    <tr>
    	<td width="180"><font size=2>&nbsp;&nbsp;&nbsp; Key Group </td>
      <td width="250"><font size=2>
		<select size="1" name="ikeKeyGroup"> 
			<option selected value="0">DH1(modp768)</option>
			<option value="1">DH2(modp1024)</option>
			<option value="2">DH5(modp1536)</option>
		</select>
		</font></td>
    </tr>     

    <tr>
    	<td width="180"><font size=2>&nbsp;&nbsp;&nbsp; Key Life Time</td>
      <td width="250"><font size=2><input type="text" name="ikeLifeTime" size="18" maxlength="30" value=""></font></td>
    </tr>     

    <tr>
      <td width="180"><font size="2"><b>Phase 2:</b></font></td>
      <td width="250">&nbsp;</td>
    </tr>
    <tr>
    	<td width="180"><font size=2>&nbsp;&nbsp;&nbsp; Active Protocol</td>
      <td width="250"><font size=2> ESP </font></td>
    </tr>      

    <tr>
    	<td width="180"><font size=2>&nbsp;&nbsp;&nbsp; Encryption Algorithm</td>
      <td width="250"><font size=2>
		<select size="1" name="ipsecEncr" >
			<option selected value="0">3DES</option>
			<option value="1">AES128</option><option value="2">NULL</option>
		</select>
		</font></td>
    </tr>     

    <tr>
    	<td width="180"><font size=2>&nbsp;&nbsp;&nbsp; Authenticaiton Algorithm</td>
      <td width="250"><font size=2>
		  <select size="1" name="ipsecAuth">
			<option selected value="0">MD5</optionL>
			<option value="1">SHA1</option>
		  </select>
      </font></td>
    </tr>      

    <tr>
    	<td width="180"><font size=2>&nbsp;&nbsp;&nbsp; Key Life Time</td>
      <td width="250"><font size=2><input type="text" name="ipsecLifeTime" size="18" maxlength="30" value=""></font></td>
    </tr>     

    <tr>
    	<td width="180"><font size=2>&nbsp;&nbsp;&nbsp; Ecapsulation </td>
      <td width="250"><font size=2>Tunnel mode</font></td>
    </tr>     

    <tr>
    	<td width="180"><font size=2>&nbsp;&nbsp;&nbsp; Perfect Forward Secrecy (PFS)</td>
      <td width="250"><font size=2>
		  <select size="1" name="ipsecPfs">
			<option selected value="0">NONE</optionL>
			<option value="1">ON</option>
		  </select>
      </font></td>
    </tr>      

</table>    
<script>
	loadSetting();
  </script>
<input type="hidden" value="/vpntbl.asp" name="submit-url">
  <p><input type="button" value="  Ok  " name="Ok" onClick="return saveChanges(1)">&nbsp;&nbsp;
	<input type="button" value="Cancel" name="Cancel" onClick="return saveChanges(0)">&nbsp;&nbsp;
</form>
</blockquote>
</font>
</body>

</html>

