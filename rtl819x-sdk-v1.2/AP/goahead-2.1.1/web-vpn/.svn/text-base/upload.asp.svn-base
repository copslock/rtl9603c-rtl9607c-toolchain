<html>
<! Copyright (c) Realtek Semiconductor Corp., 2003. All Rights Reserved. ->
<head>
<meta http-equiv="Content-Type" content="text/html">
<title>Firmware Update</title>
<style>
.on {display:on}
.off {display:none}
</style>
<script type="text/javascript" src="common.js"></script>
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
  if(document.upload.binary.value == ""){
      	document.upload.binary.focus();
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
<h2><font color="#0000FF">Upgrade Firmware</font></h2>

<form method="post" action="goform/formUpload" enctype="multipart/form-data" name="upload">
<table border="0" cellspacing="4" width="500">
 <tr><font size=2>
 This page allows you upgrade the Access Point firmware to new version. Please note,
 do not power off the device during the upload because it may crash the system.
 </tr>
  <tr><hr size=1 noshade align=top></tr>

<!--
  <tr>
      <td width="20%"><font size=2><b>Start Address:</b></td>
      <td width="80%"><font size=2><input type="text" name="readAddr" size="10" maxlength="8" value=20000>(hex)</td>
  </tr>
  <tr>
      <td width="20%"><font size=2><b>Size:</b></td>
      <td width="80%"><font size=2><input type="text" name="size" size="10" maxlength="8" value=F0000>(hex)</td>
  </tr>
  <tr>
      <td width="20%"><font size=2><b>Save File:</b></td>
      <td width="80%"><font size=2>
      <p><input type="submit" value="Save..." name="save"></p></td>
  </tr>
-->

  <tr>
      <td width="20%"><font size=2><b>Select File:</b>&nbsp;&nbsp;&nbsp;&nbsp;</td>
      <td width="80%"><font size=2><input type="file" name="binary" size=20></td>
  </tr>
  </table> 
    <p> <input onclick=sendClicked(this.form) type=button value="Upload" name="send">&nbsp;&nbsp;    
	<input type="reset" value="Reset" name="reset">
<!--
	<input type="hidden" value="0x10000" name="writeAddrWebPages">
	<input type="hidden" value="0x20000" name="writeAddrCode">
-->
	<input type="hidden" value="/upload.asp" name="submit-url">

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
