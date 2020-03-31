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
	var wlan_num = <% write(getIndex("wlan_num")); %>;
	var wlanMode = <% write(getIndex("wlanMode")); %>;
	var ssid_drv=new Array();
  	var tx_pkt_num =new Array();
  	var rx_pkt_num =new Array();
  	var rp_enabled=new Array();
	var rp_tx_pkt_num =new Array();
  	var rp_rx_pkt_num =new Array();  	
	var mssid_num=<%write(getIndex("wlan_mssid_num"));%>;
	var mssid_disable=new Array();
	var mssid_tx_pkt_num=new Array();
	var mssid_rx_pkt_num=new Array();
	var wlanDisabled=new Array();
	
	<% 
	write("mssid_disable[0] ="+getVirtualIndex("wlanDisabled", 1)+";\n");	
	write("mssid_disable[1] ="+getVirtualIndex("wlanDisabled", 2)+";\n");
	write("mssid_disable[2] ="+getVirtualIndex("wlanDisabled", 3)+";\n");
	write("mssid_disable[3] ="+getVirtualIndex("wlanDisabled", 4)+";\n");
	write("mssid_tx_pkt_num[0] =");getVirtualInfo("wlanTxPacketNum", 1);write(";\n"); 
	write("mssid_tx_pkt_num[1] =");getVirtualInfo("wlanTxPacketNum", 2);write(";\n"); 
	write("mssid_tx_pkt_num[2] =");getVirtualInfo("wlanTxPacketNum", 3);write(";\n"); 
	write("mssid_tx_pkt_num[3] =");getVirtualInfo("wlanTxPacketNum", 4);write(";\n"); 
	write("mssid_rx_pkt_num[0] =");getVirtualInfo("wlanRxPacketNum", 1);write(";\n");
	write("mssid_rx_pkt_num[1] =");getVirtualInfo("wlanRxPacketNum", 2);write(";\n");
	write("mssid_rx_pkt_num[2] =");getVirtualInfo("wlanRxPacketNum", 3);write(";\n");
	write("mssid_rx_pkt_num[3] =");getVirtualInfo("wlanRxPacketNum", 4);write(";\n");
	
	   var i ;
	   for (i=0; i<getIndex("wlan_num"); i=i+1) 
	     { 
			wlan_name= "wlan"+i+"-status";
			getInfo(wlan_name);
			write("ssid_drv["+i+"] ='");getInfo("ssid_drv");write("';\n");
			write("tx_pkt_num["+i+"] =");getInfo("wlanTxPacketNum");write(";\n"); 
			write("rx_pkt_num["+i+"] =");getInfo("wlanRxPacketNum");write(";\n");
			write("rp_enabled["+i+"] ="+getIndex("isRepeaterEnabled")+";\n"); 
			write("rp_tx_pkt_num["+i+"] =");getInfo("wlanRepeaterTxPacketNum");write(";\n");
			write("rp_rx_pkt_num["+i+"] =");getInfo("wlanRepeaterRxPacketNum");write(";\n");
			write("wlanDisabled["+i+"] ="+getIndex("wlanDisabled")+";\n");			
	     } 
	%>
	if(ssid_drv[0]=="")
	     		mssid_num=0;
  	for(i=0 ;i < wlan_num ; i++){
		if (!wlanDisabled[i]) { 
			 if(ssid_drv[0]!=""){ 	
			  document.write(' <tr>\
			    <td width="30%" rowspan="2"><font size=2><b>Wireless '+<% if (getIndex("wlan_num") > 1)  write("(i+1)+"); %>' LAN</b></td>\
			    <td width="30%"><font size=2><i>Sent Packets</i></td>\
			    <td width="20%"><font size=2>' + tx_pkt_num[i] +'</td>\
			  </tr>\
			  <tr>\
			    <td width="30%"><font size=2><i>Received Packets</i></td>\
			    <td width="20%"><font size=2>' + rx_pkt_num[i] + '</td>\
			  </tr>');
			}
			  if (rp_enabled[i])
			  {
			  document.write(' <tr>\
			    <td width="30%" rowspan="2"><font size=2><b>Wireless '+<% if (getIndex("wlan_num") > 1)  write("(i+1)+"); %>' Repeater LAN</b></td>\
			    <td width="30%"><font size=2><i>Sent Packets</i></td>\
			    <td width="20%"><font size=2>' + rp_tx_pkt_num[i] +'</td>\
			  </tr>\
			  <tr>\
			    <td width="30%"><font size=2><i>Received Packets</i></td>\
			    <td width="20%"><font size=2>' + rp_rx_pkt_num[i] + '</td>\
			  </tr>');	  	  
			  }	

			if (wlanMode==0 || wlanMode==3 ) {
			 for (idx=0; idx<mssid_num; idx++) {
				if (!mssid_disable[idx]) {
				  document.write(' <tr>\
				  	<td width="30%" rowspan="2"><font size=2><b>&nbsp;&nbsp;Virtual AP'+(idx+1)+'</b></td>\
					<td width="30%"><font size=2><i>Sent Packets</i></td>\
					<td width="20%"><font size=2>' + mssid_tx_pkt_num[idx] +'</td>\
					</tr>\
				  	<tr>\
					<td width="30%"><font size=2><i>Received Packets</i></td>\
					<td width="20%"><font size=2>' + mssid_rx_pkt_num[idx] + '</td>\
					</tr>');	  	  		
				}	  
			}
			}
		}
	 }
  </script>
    <tr>
   <td width="30%" rowspan="2"><font size=2><b>Ethernet LAN</b></td>
    <td width="30%"><font size=2><i>Sent Packets</i></td>
    <td width="20%"><font size=2><% getInfo("lanTxPacketNum"); %></td>
  </tr>
  <tr>
    <td width="30%"><font size=2><i>Received Packets</i></td>
    <td width="20%"><font size=2><% getInfo("lanRxPacketNum"); %></td>
  </tr>
 </table>
  <br>
  <input type="hidden" value="/stats.asp" name="submit-url">
  <input type="submit" value="Refresh" name="refresh">
</form>
</blockquote>
</body>

</html>
