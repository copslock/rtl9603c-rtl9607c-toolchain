<html>
<! Copyright (c) Realtek Semiconductor Corp., 2003. All Rights Reserved. ->
<head>
<meta http-equiv="Content-Type" content="text/html">
<title>Wireless Site Survey</title>
<script type="text/javascript" src="util_ap.js"> </script>
<script>
var connectEnabled=0, autoconf=0;

function enableConnect(selId)
{ 	
	if(parent.document.getElementById("select"))
		parent.document.getElementById("select").value = "sel"+selId;	
						
	if(parent.document.getElementById("next"))
		enableTextField(parent.document.getElementById("next"));						
	if(parent.document.getElementById("survey_selected0"))
		parent.document.getElementById("survey_selected0").value = 1;
		
	if(parent.document.getElementById("pocket_ssid"))
  	parent.document.getElementById("pocket_ssid").value = document.getElementById("selSSID_"+selId).value;
  if(parent.document.getElementById("pocketAP_ssid"))
  	parent.document.getElementById("pocketAP_ssid").value = document.getElementById("selSSID_"+selId).value;
  if(parent.document.getElementById("survey_ssid0"))
  	parent.document.getElementById("survey_ssid0").value = document.getElementById("selSSID_"+selId).value;
  if(parent.document.getElementById("repeaterSSID0"))
  	parent.document.getElementById("repeaterSSID0").value = document.getElementById("selSSID_"+selId).value;
  
  
  if(parent.document.getElementById("pocket_channel"))
		parent.document.getElementById("pocket_channel").value = document.getElementById("selChannel_"+selId).value;
  if(parent.document.getElementById("survey_channel0"))
		parent.document.getElementById("survey_channel0").value = document.getElementById("selChannel_"+selId).value;
  	
  	
  if(parent.document.getElementById("pocket_encrypt"))
  	parent.document.getElementById("pocket_encrypt").value = document.getElementById("selEncrypt_"+selId).value;    	  	
  if(parent.document.getElementById("survey_encrypt0"))  
  	parent.document.getElementById("survey_encrypt0").value = document.getElementById("selEncrypt_"+selId).value;  
  
  
  if(parent.document.getElementById("ciphersuite0"))
  {
	  if(document.getElementById("wpa_tkip_aes_"+selId).value == "aes/tkip")
	  	parent.document.getElementById("ciphersuite0").value = "aes";
	  else if(document.getElementById("wpa_tkip_aes_"+selId).value == "tkip")
	  	parent.document.getElementById("ciphersuite0").value = "tkip";
	  else if(document.getElementById("wpa_tkip_aes_"+selId).value == "aes")
	  	parent.document.getElementById("ciphersuite0").value = "aes";
  }
  if(parent.document.getElementById("survey_ciphersuite0"))
  {
	  if(document.getElementById("wpa_tkip_aes_"+selId).value == "aes/tkip")
	  	parent.document.getElementById("survey_ciphersuite0").value = "aes";
	  else if(document.getElementById("wpa_tkip_aes_"+selId).value == "tkip")
	  	parent.document.getElementById("survey_ciphersuite0").value = "tkip";
	  else if(document.getElementById("wpa_tkip_aes_"+selId).value == "aes")
	  	parent.document.getElementById("survey_ciphersuite0").value = "aes";
  }
  
  if(parent.document.getElementById("wpa2ciphersuite0"))
  {
	  if(document.getElementById("wpa2_tkip_aes_"+selId).value == "aes/tkip")
	  	parent.document.getElementById("wpa2ciphersuite0").value = "aes";
	  else if(document.getElementById("wpa2_tkip_aes_"+selId).value == "tkip")
	  	parent.document.getElementById("wpa2ciphersuite0").value = "tkip";
	  else if(document.getElementById("wpa2_tkip_aes_"+selId).value == "aes")
	  	parent.document.getElementById("wpa2ciphersuite0").value = "aes";
	}
	if(parent.document.getElementById("survey_wpa2ciphersuite0"))
  {
	  if(document.getElementById("wpa2_tkip_aes_"+selId).value == "aes/tkip")
	  	parent.document.getElementById("survey_wpa2ciphersuite0").value = "aes";
	  else if(document.getElementById("wpa2_tkip_aes_"+selId).value == "tkip")
	  	parent.document.getElementById("survey_wpa2ciphersuite0").value = "tkip";
	  else if(document.getElementById("wpa2_tkip_aes_"+selId).value == "aes")
	  	parent.document.getElementById("survey_wpa2ciphersuite0").value = "aes";
	}
  	
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
  
  <% wlSiteSurveyTbl("all"); %>
  </table>
  <br>
  
 
</form>


</body>
</html>
