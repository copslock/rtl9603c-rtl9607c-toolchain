<html>
<! Copyright (c) Realtek Semiconductor Corp., 2003. All Rights Reserved. ->
<head>
<title>Power Consumption</title>
<meta http-equiv="Content-Type" content="text/html">
<SCRIPT language=JavaScript src="xml_data.js"></SCRIPT>

<script language="JavaScript" type="text/javascript">
<!-- 
	var dataReadyvar = false;
	var xsltReadyvar = false;
	var xmlData;
	
	
function xsltReady(xmlDoc)
{
	xsltReadyvar = true;
	refreshTable();
}
	
function dataReady(xmlDoc)
{
	xmlData = xmlDoc.getDocument();
	dataReadyvar = true;
	refreshTable();
}

function refreshTable()
{
	if (!(dataReadyvar && xsltReadyvar)) {
		return;
	}
	var my_current_doc = document.getElementById("interfaces");

	while(my_current_doc.firstChild != null) 
	{
		my_current_doc.removeChild(my_current_doc.firstChild);
	}

	get_result = xsltProcessor.transform(xmlData, window.document, my_current_doc);
	
	var totalPwrCon = document.getElementById("totalPwrCon").innerHTML;
	var wlanTx = document.getElementById("wlanTx").innerHTML;	
	var wlanRx = document.getElementById("wlanRx").innerHTML;	
	
	var tmp,tmp2;
	
	tmp = (totalPwrCon%10000)/1000;
	tmp = tmp+"";
	tmp2 = tmp.split(".");
	var pwrCon_1 = tmp2[0];	
	
	tmp = (totalPwrCon%1000)/100;
	tmp = tmp+"";
	tmp2 = tmp.split(".");
	var pwrCon_2 = tmp2[0];	
	
	tmp = (totalPwrCon%100)/10;
	tmp = tmp+"";
	tmp2 = tmp.split(".");
	var pwrCon_3 = tmp2[0];	
			
	document.pwrConGif_1.src = "./graphics/num-"+pwrCon_1+".gif";
	document.pwrConGif_2.src = "./graphics/num-"+pwrCon_2+".gif";
	document.pwrConGif_3.src = "./graphics/num-"+pwrCon_3+".gif";
	
	tmp = (wlanTx%100000000)/10000000;
	tmp = tmp+"";
	tmp2 = tmp.split(".");
	var wlanTx_1 = tmp2[0];	
	
	tmp = (wlanTx%10000000)/1000000;
	tmp = tmp+"";
	tmp2 = tmp.split(".");
	var wlanTx_2 = tmp2[0];	
	
	tmp = (wlanTx%1000000)/100000;
	tmp = tmp+"";
	tmp2 = tmp.split(".");
	var wlanTx_3 = tmp2[0];
	
	tmp = (wlanTx%100000)/10000;
	tmp = tmp+"";
	tmp2 = tmp.split(".");
	var wlanTx_4 = tmp2[0];
	
	document.wlanTxGif_1.src = "./graphics/num-"+wlanTx_1+".gif";
	document.wlanTxGif_2.src = "./graphics/num-"+wlanTx_2+".gif";
	document.wlanTxGif_3.src = "./graphics/num-"+wlanTx_3+".gif";
	document.wlanTxGif_4.src = "./graphics/num-"+wlanTx_4+".gif";
	
	tmp = (wlanRx%100000000)/10000000;
	tmp = tmp+"";
	tmp2 = tmp.split(".");
	var wlanRx_1 = tmp2[0];	
	
	tmp = (wlanRx%10000000)/1000000;
	tmp = tmp+"";
	tmp2 = tmp.split(".");
	var wlanRx_2 = tmp2[0];	
	
	tmp = (wlanRx%1000000)/100000;
	tmp = tmp+"";
	tmp2 = tmp.split(".");
	var wlanRx_3 = tmp2[0];
	
	tmp = (wlanRx%100000)/10000;
	tmp = tmp+"";
	tmp2 = tmp.split(".");
	var wlanRx_4 = tmp2[0];
	
	document.wlanRxGif_1.src = "./graphics/num-"+wlanRx_1+".gif";
	document.wlanRxGif_2.src = "./graphics/num-"+wlanRx_2+".gif";
	document.wlanRxGif_3.src = "./graphics/num-"+wlanRx_3+".gif";
	document.wlanRxGif_4.src = "./graphics/num-"+wlanRx_4+".gif";
	
	window.setTimeout("xmlDataFetcher.retrieveData();", 1000);	
}

function xsltProcessorTimeout(xmlDataInstance)
{		
			window.setTimeout("xsltProcessor.retrieveData()", 20000);
}

function xmlDataFetcherTimeout(xmlDataInstance)
{		
			window.setTimeout("xmlDataFetcher.retrieveData()", 20000);
}

function page_load() 
{
	xsltProcessor = new xsltProcessingObject(xsltReady, xsltProcessorTimeout, 6000, "./powerConsumption_proc.sxsl");
	xmlDataFetcher = new xmlDataObject(dataReady, xmlDataFetcherTimeout, 6000, "./powerConsumption_data.asp");
	
	xsltProcessor.retrieveData();
	xmlDataFetcher.retrieveData();		
}
// End Script -->
</script>
</head>
<body onload="page_load()">
<blockquote>
<h2><font color="#0000FF">Power Consumption</font></h2>

<table border=0 width="500" cellpadding=0>
  <tr><font size=2>
 This page shows the power consumption of AP/Router.
  </tr>
  <tr><hr size=1 noshade align=top></tr>
</table>
<form id="formStats" method=POST name="formStats">
	<div id="interfaces" style="display:none"></div>
	<div id="wan_offline">
		
		<table border="0">
			<tr>
				<td rowspan=3>
					<!-- http://tzoyiing.pixnet.net/blog/post/22374838 -->
					<table style="border: 5px ridge rgb(0, 255, 0)">
						<tr>					
							<td><img src="./graphics/num-0.gif" name="pwrConGif_1" height=60 width=50 ></td>
							<td><img src="./graphics/dot.gif" name="dot" height=60 width=30 ></td>
							<td><img src="./graphics/num-0.gif" name="pwrConGif_2" height=60 width=50 ></td>
							<td><img src="./graphics/num-0.gif" name="pwrConGif_3" height=60 width=50 ></td>				
						</tr>
					</table>
				</td>
				<td>
					<tr>
						<td>&nbsp;</td>
					</tr>
					<tr>
						<td><font size="5"><B><i>Watts</i></B></font></td>
					</tr>
				</td>
			</tr>
		</table>
		<br>
		
		<table border="0">
			<td>
				<table style="border: 3px ridge rgb(0, 255, 0)">
					<tr>
						<td align=center><B>Wireless Tx&nbsp;</B></td>			
					<tr>
					<tr>
						<td>
							<!-- http://tzoyiing.pixnet.net/blog/post/22374838 -->
							<table style="border: 2px ridge rgb(200, 255, 255)">
								<tr>					
									<td><img src="./graphics/num-0.gif" name="wlanTxGif_1" height=30 width=20 ></td>							
									<td><img src="./graphics/num-0.gif" name="wlanTxGif_2" height=30 width=20 ></td>
									<td><img src="./graphics/dot.gif" name="dot" height=30 width=20 ></td>
									<td><img src="./graphics/num-0.gif" name="wlanTxGif_3" height=30 width=20 ></td>
									<td><img src="./graphics/num-0.gif" name="wlanTxGif_4" height=30 width=20 ></td>
								</tr>
							</table>
						</td>				
					</tr>
				</table>
			</td>
			<td>&nbsp;</td>
			<td>
				<table style="border: 3px ridge rgb(0, 255, 0)">
					<tr>
						<td align=center><B>Wireless Rx&nbsp;</B></td>
					<tr>
					<tr>
						<td>
							<!-- http://tzoyiing.pixnet.net/blog/post/22374838 -->
							<table style="border: 2px ridge rgb(200, 255, 255)">
								<tr>					
									<td><img src="./graphics/num-0.gif" name="wlanRxGif_1" height=30 width=20 ></td>							
									<td><img src="./graphics/num-0.gif" name="wlanRxGif_2" height=30 width=20 ></td>
									<td><img src="./graphics/dot.gif" name="dot" height=30 width=20 ></td>
									<td><img src="./graphics/num-0.gif" name="wlanRxGif_3" height=30 width=20 ></td>
									<td><img src="./graphics/num-0.gif" name="wlanRxGif_4" height=30 width=20 ></td>
								</tr>
							</table>
						</td>							
					</tr>
				</table>
			<td>
		</table>
		
		<table width=300 border="0">
			<td align=right><font color="#0000AA" size=2><i>Uint is Mbps</i></font></td>
		</table>
		
	</div>
</form>
</blockquote>
</body>

</html>
