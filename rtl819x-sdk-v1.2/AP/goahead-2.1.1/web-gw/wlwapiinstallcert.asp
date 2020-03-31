<html>
<! Copyright (c) Realtek Semiconductor Corp., 2003. All Rights Reserved. ->
<head>
<meta http-equiv="Content-Type" content="text/html">
<title>WAPI Certification Installation</title>
<style>
.on {display:on}
.off {display:none}
</style>
<script type="text/javascript" src="util_gw.js"></script>
<script>
var MWJ_progBar = 0;
var time=0;
var delay_time=1000;
var loop_num=0;

function progress()
{
  if (loop_num == 3) {
	alert("Update firmware failed!");
	return false;
  }
  if (time < 1) 
	time = time + 0.033;
  else {
	time = 0;
	loop_num++;
  }
  setTimeout('progress()',delay_time);  
  myProgBar.setBar(time); 
}


function sendClicked(F)
{
  if(document.uploadCert.asu_binary.value == "" && document.uploadCert==F){
      	document.uploadCert.asu_binary.focus();
  	alert('File name can not be empty !');
  	return false;
  }

  if(document.uploadUserCert.user_binary.value == "" && document.uploadUserCert==F){
      	document.uploadUserCert.user_binary.focus();
  	alert('File name can not be empty !');
  	return false;
  }
	
  F.submit();
  show_div(true, "progress_div");   
  progress();
}

</script>

</head>
<BODY>
<blockquote>
<h2><font color="#0000FF">Certification Installation</font></h2>

<form method="post" action="/goform/formUploadWapiCert" enctype="multipart/form-data" name="uploadCert">
<table border="0" cellspacing="4" width="500">
 <tr><font size=2>
 This page allows you to install ASU and user certification for our AP. Please note, certification type 
 X.509 is supported at present.
 </tr>
  <tr><hr size=1 noshade align=top></tr>
  <tr>
  <td width="20%"><font size=2><b>Certification Type:</b>&nbsp;&nbsp;&nbsp;&nbsp;</td>
  <td width="80%"><font size=2> <input name="cert_type" type=radio value=0 checked>X.509</td>
  </tr>
  <tr>
      <td width="20%"><font size=2><b>ASU Certification:</b>&nbsp;&nbsp;&nbsp;&nbsp;</td>
      <td width="80%"><font size=2><input type="file" name="asu_binary" size=20></td>
  </tr>
<!--  <tr>
      <td width="20%"><font size=2><b>User Certification:</b>&nbsp;&nbsp;&nbsp;&nbsp;</td>
      <td width="80%"><font size=2><input type="file" name="user_binary" size=20></td>
  </tr>
-->
  </table> 
    <p> <input onclick=sendClicked(this.form) type=button value="Upload" name="send">&nbsp;&nbsp;    
	<input type="reset" value="Reset" name="reset">
<!--
	<input type="hidden" value="0x10000" name="writeAddrWebPages">
	<input type="hidden" value="0x20000" name="writeAddrCode">
-->
	<input type="hidden" value="/wlwapiinstallcert.asp" name="submit-url">
        <input type="hidden" value="asu" name="uploadcerttype">
    </p>
 </form>

<form method="post" action="/goform/formUploadWapiCert" enctype="multipart/form-data" name="uploadUserCert">
<table border="0" cellspacing="4" width="500">
 <tr><font size=2>
 </tr>
  <tr><hr size=1 noshade align=top></tr>
<!--
  <tr>
  <td width="20%"><font size=2><b>Certification Type:</b>&nbsp;&nbsp;&nbsp;&nbsp;</td>
  <td width="80%"><font size=2> <input name="cert_type" type=radio value=0 checked>X.509</td>
  </tr>
  <tr>
      <td width="20%"><font size=2><b>ASU Certification:</b>&nbsp;&nbsp;&nbsp;&nbsp;</td>
      <td width="80%"><font size=2><input type="file" name="asu_binary" size=20></td>
  </tr>
-->
  <tr>
      <td width="20%"><font size=2><b>User Certification:</b>&nbsp;&nbsp;&nbsp;&nbsp;</td>
      <td width="80%"><font size=2><input type="file" name="user_binary" size=20></td>
  </tr>
  </table>
    <p> <input onclick=sendClicked(this.form) type=button value="Upload" name="send">&nbsp;&nbsp;
        <input type="reset" value="Reset" name="reset">
<!--
        <input type="hidden" value="0x10000" name="writeAddrWebPages">
        <input type="hidden" value="0x20000" name="writeAddrCode">
-->
        <input type="hidden" value="/wlwapiinstallcert.asp" name="submit-url">
        <input type="hidden" value="user" name="uploadcerttype">
 
    </p>
 </form>
 
 <script type="text/javascript" language="javascript1.2">
		var myProgBar = new progressBar(
			1,         //border thickness
			'#000000', //border colour
			'#ffffff', //background colour
			'#043db2', //bar colour
			300,       //width of bar (excluding border)
			15,        //height of bar (excluding border)
			1          //direction of progress: 1 = right, 2 = down, 3 = left, 4 = up
		);
</script>
 
 
 </blockquote>
</body>
</html>
