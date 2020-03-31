<html>
<head>
	<script type="text/javascript" src="mtmcode.js"></script>
	<script language=JavaScript>
	<!--
		var count = <% getInfo("countDownTime"); %>*1;
		
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
		}
		
		function do_count_down()
		{
			get_by_id("show_sec").innerHTML = count;
			
			if(count == 0) 
			{
				change_state('normal');
				
				parent.frames[3].location.href='http://<% getInfo("ip-rom"); %>/<% getInfo("lastUrl"); %>?t='+new Date().getTime(); 
				
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
		<h4><% getInfo("okMsg"); %><br>		
			Do not turn off or reboot the Device during this time.	
		</h4>
		<P align=left>
			<h4>Please wait <B><SPAN id=show_sec></SPAN></B>&nbsp;seconds ...</h4>
		</P>
	</blockquote>
</body>
</html>
