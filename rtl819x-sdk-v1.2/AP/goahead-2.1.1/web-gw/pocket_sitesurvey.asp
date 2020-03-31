<html>
<! Copyright (c) Realtek Semiconductor Corp., 2003. All Rights Reserved. ->
<head>
<meta http-equiv="Content-Type" content="text/html">
<title>Wireless Site Survey</title>
<script type="text/javascript" src="util_gw.js"> </script>
<script>
var connectEnabled=0, autoconf=0;

function enableConnect(selId)
{ 	
  parent.document.getElementById("pocket_ssid").value = document.getElementById("selSSID_"+selId).value;
  parent.document.getElementById("pocketAP_ssid").value = document.getElementById("selSSID_"+selId).value;
  parent.document.getElementById("pocket_encrypt").value = document.getElementById("selEncrypt_"+selId).value;  
  
  if(document.getElementById("wpa_tkip_aes_"+selId).value == "aes/tkip")
  	parent.document.wizardPocket.elements["ciphersuite0"].value = "aes";
  else if(document.getElementById("wpa_tkip_aes_"+selId).value == "tkip")
  	parent.document.wizardPocket.elements["ciphersuite0"].value = "tkip";
  else if(document.getElementById("wpa_tkip_aes_"+selId).value == "aes")
  	parent.document.wizardPocket.elements["ciphersuite0"].value = "aes";
  	
  if(document.getElementById("wpa2_tkip_aes_"+selId).value == "aes/tkip")
  	parent.document.wizardPocket.elements["wpa2ciphersuite0"].value = "aes";
  else if(document.getElementById("wpa2_tkip_aes_"+selId).value == "tkip")
  	parent.document.wizardPocket.elements["wpa2ciphersuite0"].value = "tkip";
  else if(document.getElementById("wpa2_tkip_aes_"+selId).value == "aes")
  	parent.document.wizardPocket.elements["wpa2ciphersuite0"].value = "aes";
  	
}

function siteSurvey()  
{  
	//alert("SSIDSiteSurvey");
	document.formWlSiteSurvey.submit();

}
</script>
</head>
<body>


<form action=/goform/formWlSiteSurvey method=POST name="formWlSiteSurvey">
	<input type="hidden" value="Site Survey" name="refresh">&nbsp;&nbsp;  
  <input type="hidden" value="/pocket_sitesurvey.asp" name="submit-url">
  <table border="1" width="500">
  
  <% wlSiteSurveyTbl(); %>
  </table>
  <br>
  
 
</form>


</body>
</html>
