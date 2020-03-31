<html>
<! Copyright (c) Realtek Semiconductor Corp., 2003. All Rights Reserved. ->
<head>
<meta http-equiv="Content-Type" content="text/html">
<title>Statisitcs</title>
</head>
<body>
<blockquote>
<h2><font color="#0000FF">Statistics</font></h2>

<table border=0 width="500" cellpadding=0>
  <tr><font size=2>
 This page shows the packet counters for transmission and reception regarding to wireless
 and Ethernet networks.
  </tr>
  <tr><hr size=1 noshade align=top></tr>
</table>
<form action=/goform/formStats method=POST name="formStats">
<table border="1" width="360">
  <script>
	
	 
		function drawLanStatistics()
		{
			if( "<% getInfo("vlanOnOff"); %>"*1 == 0)
			{
				var lanTx="<% getInfo("lanTxPacketNum"); %>";
				var lanRx="<% getInfo("lanRxPacketNum"); %>";
				
				document.write("<tr><td width='30%' rowspan='2'><font size=2><b>Ethernet LAN</b></td>");
	    	document.write("<td width='30%'><font size=2><i>Sent Packets</i></td>");
	    	document.write("<td width='20%'><font size=2>"+lanTx+"</td></tr>");
	  		document.write("<tr><td width='30%'><font size=2><i>Received Packets</i></td>");
	    	document.write("<td width='20%'><font size=2>"+lanRx+"</td></tr>");
    	}
    	else
    	{
    		var lan1Tx="<% getInfo("lanTxPacketNum"); %>";
				var lan1Rx="<% getInfo("lanRxPacketNum"); %>";
				var lan2Tx="<% getInfo("lan2TxPacketNum"); %>";
				var lan2Rx="<% getInfo("lan2RxPacketNum"); %>";
				var lan3Tx="<% getInfo("lan3TxPacketNum"); %>";
				var lan3Rx="<% getInfo("lan3RxPacketNum"); %>";
				var lan4Tx="<% getInfo("lan4TxPacketNum"); %>";
				var lan4Rx="<% getInfo("lan4RxPacketNum"); %>";
				
	    	document.write("<tr><td width='30%' rowspan='2'><font size=2><b>Ethernet LAN1</b></td>");
	    	document.write("<td width='30%'><font size=2><i>Sent Packets</i></td>");
	    	document.write("<td width='20%'><font size=2>"+lan1Tx+"</td></tr>");
	  		document.write("<tr><td width='30%'><font size=2><i>Received Packets</i></td>");
	    	document.write("<td width='20%'><font size=2>"+lan1Rx+"</td></tr>");
	    	
	    	document.write("<tr><td width='30%' rowspan='2'><font size=2><b>Ethernet LAN2</b></td>");
	    	document.write("<td width='30%'><font size=2><i>Sent Packets</i></td>");
	    	document.write("<td width='20%'><font size=2>"+lan2Tx+"</td></tr>");
	  		document.write("<tr><td width='30%'><font size=2><i>Received Packets</i></td>");
	    	document.write("<td width='20%'><font size=2>"+lan2Rx+"</td></tr>");
	    	document.write("<tr><td width='30%' rowspan='2'><font size=2><b>Ethernet LAN3</b></td>");
	    	document.write("<td width='30%'><font size=2><i>Sent Packets</i></td>");
	    	document.write("<td width='20%'><font size=2>"+lan3Tx+"</td></tr>");
	  		document.write("<tr><td width='30%'><font size=2><i>Received Packets</i></td>");
	    	document.write("<td width='20%'><font size=2>"+lan3Rx+"</td></tr>");
	    	
	    	document.write("<tr><td width='30%' rowspan='2'><font size=2><b>Ethernet LAN4</b></td>");
	    	document.write("<td width='30%'><font size=2><i>Sent Packets</i></td>");
	    	document.write("<td width='20%'><font size=2>"+lan4Tx+"</td></tr>");
	  		document.write("<tr><td width='30%'><font size=2><i>Received Packets</i></td>");
	    	document.write("<td width='20%'><font size=2>"+lan4Rx+"</td></tr>");
	    }
			
		}
  </script>
  <!--
  <tr>
   <td width="30%" rowspan="2"><font size=2><b>Ethernet LAN</b></td>
    <td width="30%"><font size=2><i>Sent Packets</i></td>
    <td width="20%"><font size=2><% getInfo("lanTxPacketNum"); %></td>
  </tr>
  <tr>
    <td width="30%"><font size=2><i>Received Packets</i></td>
    <td width="20%"><font size=2><% getInfo("lanRxPacketNum"); %></td>
  </tr>
  -->
  <SCRIPT >drawLanStatistics();</SCRIPT>
  
  <tr>
    <td width="30%" rowspan="2"><font size=2><b>Ethernet WAN</b></td>
    <td width="30%"><font size=2><i>Sent Packets</i></td>
    <td width="20%"><font size=2><% getInfo("wanTxPacketNum"); %></td>
  </tr>
  <tr>
    <td width="30%"><font size=2><i>Received Packets</i></td>
    <td width="20%"><font size=2><% getInfo("wanRxPacketNum"); %></td>
  </tr>

</table>
  <br>
  <input type="hidden" value="/stats.asp" name="submit-url">
  <input type="submit" value="Refresh" name="refresh">
</form>
</blockquote>
</body>

</html>
