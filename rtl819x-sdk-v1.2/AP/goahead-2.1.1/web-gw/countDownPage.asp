<html>
<head>
	<script type="text/javascript" src="mtmcode.js"></script>
	<script type="text/javascript" src="util_gw.js"></script>
	<style>
	.on {display:on}
	.off {display:none}
	</style>
	<script language=JavaScript>
	<!--
		var count = <% getInfo("countDownTime"); %>*1;
		var pocketRouter_Mode="<% getInfo("pocketRouter_Mode_countdown"); %>"*1;
		var waitcount = <% getInfo("countDownTime_wait"); %>*1;
		var seconds_word=" seconds ";
		
		if(pocketRouter_Mode == 2)
			count+=20;
			
		if(waitcount==1 && pocketRouter_Mode==1){
			count=90;
		}else if(waitcount==2 && pocketRouter_Mode==1){
			count=120;
		}
			
		function get_by_id(id)
		{
			with(document)
			{
				return getElementById(id);
			}
		}
		
		function change_state(istate)
		{
			if(parent.frames[1])
			{
			parent.frames[1].state = istate;			
			MTMDisplayMenu();
			}
			
			if(pocketRouter_Mode == 1){
				get_by_id("show_msg").innerHTML = "Please wait awhile and re-open browser to configure Pocket Router. ";
			}
		}
		
		function closeBrowser()
		{
			parent.window.close();			
		}
		
		function do_count_down()
		{
			show_div(false, "close_button");
			
			if(pocketRouter_Mode == 1)
			{
				get_by_id("show_sec").innerHTML = count;
			
				if(count == 0) 
				{
					change_state('normal');
					closeBrowser();
					return false;
				}
				
				if (count > 0) 
				{
					
					count--;
					setTimeout('do_count_down()',1000);
				}
				
				
				return;
			}
						
			get_by_id("show_sec").innerHTML = count;
			
			if(count == 0) 
			{
				change_state('normal');
				
				parent.frames[3].location.href='http://<% getInfo("ip-lan"); %>/<% getInfo("lastUrl"); %>?t='+new Date().getTime();
				
				return false;
			}
			
			if (count > 0) 
			{
				
				count--;
				setTimeout('do_count_down()',1000);
			}
		}
	//-->
	</script>
</head>
<body onload="change_state('normal');do_count_down();">
	<blockquote>
		<h4><h4>Change setting successfully!<BR><br>
			<SPAN id=show_msg>Do not turn off or reboot the Device during this time.</span>
		</h4>
		<P align=left>
			<h4><SPAN id=please_wait>Please wait </span><B><SPAN id=show_sec></SPAN></B>&nbsp;<SPAN id=show_seconds>seconds ...</SPAN></h4>
		</P>
		<span id = "close_button" class = "off" >
			<input type="button" value="  OK  " name="cancel" onClick='closeBrowser();'>
		</span>
	</blockquote>
</body>
</html>
