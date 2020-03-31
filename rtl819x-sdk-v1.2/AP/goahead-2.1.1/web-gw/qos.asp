<HTML lang=en-US xml:lang="en-US" 
xmlns="http://www.w3.org/1999/xhtml">
<HEAD><TITLE>QoS Engine</TITLE>
<META http-equiv=content-type content="text/html; charset=utf-8"><LINK 
rev=stylesheet href="style.css" type=text/css 
rel=stylesheet>
<!-- InstanceBeginEditable name="Local Styles" -->
<STYLE type=text/css></STYLE>
<!-- InstanceEndEditable -->
<SCRIPT src="util_qos.js" type=text/javascript></SCRIPT>

<!-- InstanceBeginEditable name="Scripts" -->
<SCRIPT type=text/javascript>
	//<![CDATA[
		/*
		 * Handle for document.form["mainform"].
		 */
		var mf;

		/*
		 * Number of rules.
		 */
		var nbrules = parseInt("10", 10);

		/*
		 * Selectors.
		 */
		function qos_enabled_selector(checked)
		{
			mf.qos_enabled.value = checked;
			mf.qos_enabled_select.checked = checked;

			var disabled = !checked;

			mf.qos_auto_trans_rate.disabled = disabled;
			mf.qos_auto_trans_rate_select.disabled = disabled;
			mf.qos_max_trans_rate.disabled = disabled;
			mf.qos_max_trans_rate_select.disabled = disabled;

			if  ((mf.qos_enabled.value == "true") && (mf.qos_auto_trans_rate.value == "true")) {
				mf.qos_max_trans_rate.disabled = true;
				mf.qos_max_trans_rate_select.disabled = true;
			}
			
			for (var i = 0; i < nbrules; i++) {
				mf["qos_rules_enabled_select_" + i].disabled = disabled;
				mf["qos_rules_entry_name_" + i].disabled = disabled;
				mf["qos_rules_priority_" + i].disabled = disabled;
				mf["qos_rules_protocol_" + i].disabled = disabled;
				mf["qos_rules_protocol_select_" + i].disabled = disabled;
				mf["qos_rules_local_ip_start_" + i].disabled = disabled;
				mf["qos_rules_local_ip_end_" + i].disabled = disabled;
				mf["qos_rules_local_port_start_" + i].disabled = disabled;
				mf["qos_rules_local_port_end_" + i].disabled = disabled;
				mf["qos_rules_remote_ip_start_" + i].disabled = disabled;
				mf["qos_rules_remote_ip_end_" + i].disabled = disabled;
				mf["qos_rules_remote_port_start_" + i].disabled = disabled;
				mf["qos_rules_remote_port_end_" + i].disabled = disabled;
			}
		}

		function qos_auto_trans_rate_selector(checked)
		{
			mf.qos_auto_trans_rate.value = checked;
			mf.qos_auto_trans_rate_select.checked = checked;
			mf.qos_max_trans_rate.disabled = checked;
			mf.qos_max_trans_rate_select.disabled = checked;
		}

		function qos_auto_trans_rate_selector2(checked)
		{
			mf.qos_auto_trans_rate.value = checked;
			mf.qos_auto_trans_rate_select.checked = checked;
		}

		function qos_max_trans_rate_selector(value)
		{
			mf.qos_max_trans_rate.value = value;
			// Always go back to "Select"
			mf.qos_max_trans_rate_select.value = 0;			
		}

		function table_form_enable(index, value)
		{
			mf["qos_rules_enabled_" + index].value = value;
			mf["qos_rules_used_" + index].value = (value) ? 1 : 0;
			mf["qos_rules_enabled_select_" + index].checked = value;
		}

		function table_form_protocol(index, value)
		{
		
			if (value != -1) {
				mf["qos_rules_protocol_" + index].value = value;
			}
			
			if (value == "256" || value == "257" || value == "1" || value == "6" || value == "17") {
				mf["qos_rules_protocol_select_" + index].value = value;
				mf["qos_rules_protocol_" + index].disabled = true;
			} else {
				mf["qos_rules_protocol_select_" + index].value = -1;
				mf["qos_rules_protocol_" + index].disabled = false;
			}
			mf["qos_rules_protocol_menu_" + index].value = value;

			if (mf.qos_enabled.value == "false") {
				return;
			}

			var disabled = value != "257" && value != "6"  && value != "17";
			mf["qos_rules_local_port_start_" + index].disabled = disabled;
			mf["qos_rules_local_port_end_" + index].disabled = disabled;
			mf["qos_rules_remote_port_start_" + index].disabled = disabled;
			mf["qos_rules_remote_port_end_" + index].disabled = disabled;

			if (value == -1 || value == "256" || value == "1" ) {
				mf["qos_rules_local_port_start_" + index].value = 0;
				mf["qos_rules_local_port_end_" + index].value = 65535;
				mf["qos_rules_remote_port_start_" + index].value = 0;
				mf["qos_rules_remote_port_end_" + index].value = 65535;
			}
		}

		function table_form_set()
		{
			for (var i = 0; i < nbrules; i++) {
				table_form_protocol(i, mf["qos_rules_protocol_" + i].value);

				if (mf["qos_rules_used_" + i].value == "1") {
					table_form_enable(i, mf["qos_rules_enabled_" + i].value == "true");
				}
			}
		}

		function page_load()
		{
			mf = document.forms.mainform;

			qos_enabled_selector(mf.qos_enabled.value == "true");
			/* if  (mf.qos_enabled.value == "true") */
				qos_auto_trans_rate_selector2(mf.qos_auto_trans_rate.value == "true");
			
			qos_max_trans_rate_selector(mf.qos_max_trans_rate.value);

			table_form_set();

			set_form_default_values("mainform");		

		}

		/*
		 * Validate and submit the form.
		 */
		function page_submit()
		{

			if (!is_form_modified("mainform")) {
				if (!confirm("Nothing has changed, save anyway?")) {
					return false;
				}
			}

			var s2;
			var speed = parseInt(mf.qos_max_trans_rate.value);
			if  ((mf.qos_max_trans_rate.disabled == false)) {
				if (isNaN(speed) || (speed < 128) || (speed > 102400)) {
					s2 = "Max transmission rate should be between 128kbps and 100Mbps, inclusive.";
					alert(s2);					
					return;
				}
			}

			for (var i = 0; i < nbrules; i++) {
				/*
				 * Ignore rules that are not enabled.
				 */
				var enabled = mf["qos_rules_enabled_" + i].value == "true";
				if (!enabled) {
					continue;
				}

				/*
				 * We have to check this here so that we can use the name in the error messages below.
				 */
				if (mf["qos_rules_entry_name_" + i].value == "") {
					alert("A name is required for rule number " + (i + 1) + ".");
					return;
				}

				/*
				 * Since ipReflection does not validate malformed IP addresses, check them on the client
				 * side for now.
				 */
				var s  = "Rule \'" + mf["qos_rules_entry_name_" + i].value + "\': ";				 
				if (!is_ipv4_valid(mf["qos_rules_local_ip_start_" + i].value)) {
					s += "Invalid local start IP Address.";
					alert(s);
					return;
				}
				if (!is_ipv4_valid(mf["qos_rules_local_ip_end_" + i].value)) {
					s += "Invalid local end IP Address.";
					alert(s);
					return;
				}
				if (!is_ipv4_valid(mf["qos_rules_remote_ip_start_" + i].value)) {
					s += "Invalid remote start IP Address.";
					alert(s);
					return;
				}
				if (!is_ipv4_valid(mf["qos_rules_remote_ip_end_" + i].value)) {
					s += "Invalid remote end IP Address.";
					alert(s);					
					return;
				}

				if (!is_number(mf["qos_rules_protocol_" + i].value)) {
					s += "Protocol must be a number.";
					alert(s);
					return;
				}
				/*
				 * Since ipReflection does not validate "empty" string conversions, check them on the client
				 * side for now.
				 */
				if (mf["qos_rules_protocol_" + i].value == "") {
					s += "Protocol must be specified.";
					alert(s);					
					return;
				}

				var priority = parseInt(mf["qos_rules_priority_" + i].value);
				if (isNaN(priority) || (priority < 1) || (priority > 255)) {
					s += "Priority must be a number between 1 and 255 inclusive.";
					alert(s);					
					return;
				}

				var protocol = mf["qos_rules_protocol_" + i].value;
				if (isNaN(protocol) || (protocol < 0) || (protocol > 257)) {
					s += "Protocol must be a number between 0 and 257 inclusive.";
					alert(s);					
					return;
				}

				/*
				 * Only check ports for TCP, UDP or both.
				 */
				if ((mf["qos_rules_protocol_menu_" + i].value == -1) || 
				   ((protocol != 6) && (protocol != 17) && (protocol != 257))) {
					mf["qos_rules_local_port_start_" + i].disabled = false;
					mf["qos_rules_local_port_end_" + i].disabled = false;
					mf["qos_rules_remote_port_start_" + i].disabled = false;
					mf["qos_rules_remote_port_end_" + i].disabled = false;

					mf["qos_rules_local_port_start_" + i].value = 0;
					mf["qos_rules_local_port_end_" + i].value = 65535;
					mf["qos_rules_remote_port_start_" + i].value = 0;
					mf["qos_rules_remote_port_end_" + i].value = 65535;
				
					continue;
				}

				/*
				 * We need to catch ports that are bigger than 65536
				 * since the code on the server will ignore the error
				 * of converting too large a number.
				 */
				if (!is_port_valid(mf["qos_rules_local_port_start_" + i].value)) {
					s += "Local start port should be between 0 and 65535 inclusive.";
					alert(s);
					return;
				}
				if (!is_port_valid(mf["qos_rules_local_port_end_" + i].value)) {
					s += "Local end port should be between 0 and 65535 inclusive.";
					alert(s);
					return;
				}
				if (!is_port_valid(mf["qos_rules_remote_port_start_" + i].value)) {
					s += "Remote start port should be between 0 and 65535 inclusive.";
					alert(s);
					return;
				}
				if (!is_port_valid(mf["qos_rules_remote_port_end_" + i].value)) {
					s += "Remote end port should be between 0 and 65535 inclusive.";
					alert(s);					
					return;
				}	
           			 if ( bigger_than(mf["qos_rules_local_port_start_" + i].value, mf["qos_rules_local_port_end_" + i].value)) {
					s += "Local start port must be less than or equal to local end port.";
					alert(s);
					return;
            			}
           			 if ( bigger_than(mf["qos_rules_remote_port_start_" + i].value, mf["qos_rules_remote_port_end_" + i].value)) {
					s += "Remote start port must be less than or equal to remote end port.";
					alert(s);
					return;
            			}

			}

			for (var i = 0; i < nbrules; i++) {
				mf["qos_rules_protocol_" + i].disabled = false;
			}
			mf.submit();
		}

		/*
		 * Cancel and reset changes to the page.
		 */
		function page_cancel()
		{
			if (is_form_modified("mainform") && confirm ("Do you want to abandon all changes you made to this page?")) {
				reset_form("mainform");
				page_load();
			}
		}

		function dw(message)
		{
			document.write(message);	
		}
	//]]>
	</SCRIPT>
	
<!-- InstanceEndEditable -->
<META content="MSHTML 6.00.2800.1593" name=GENERATOR></HEAD>
<BODY onload=page_load();> 

<DIV id=outside>
          <TD id=maincontent_container>
            
            <DIV id=maincontent style="DISPLAY: block"><!-- InstanceBeginEditable name="Main Content" -->
            <FORM id=mainform action=/goform/formQoS method=post>
            
            <DIV class=section>
            
            <DIV class=section_head>
            <H2>QoS</H2>
            <P>Use this section to configure Realtek's QoS. The QoS settings 
            improve your online gaming experience by ensuring that your game 
            traffic is prioritized over other network traffic, such as FTP or 
            Web. </P><INPUT class=button_submit onclick=page_submit() type=button value="Save Settings"> 
            <INPUT class=button_submit onclick=page_cancel() type=button value="Don't Save Settings"> 
            <INPUT type="hidden" value="/qos.asp" name="submit-url">
            </DIV><!-- section_head -->
            
            
            <DIV class=box>
            <H3>QoS Setup</H3>
            <FIELDSET>

            <P><INPUT id=qos_enabled type=hidden value="<% getInfo("qosEnabled"); %>"
            name=config.qos_enabled> <LABEL class=duple 
            for=qos_enabled_select>Enable QoS :</LABEL> <INPUT 
            id=qos_enabled_select onclick=qos_enabled_selector(this.checked); type=checkbox > </P>

            <DIV id=traffic_shaping_box>
            <P><INPUT id=qos_auto_trans_rate type=hidden value="<% getInfo("qosAutoUplinkSpeed"); %>"
            name=config.qos_auto_trans_rate> <LABEL class=duple 
            for=qos_auto_trans_rate_select>Automatic Uplink Speed :</LABEL> <INPUT 
            id=qos_auto_trans_rate_select onclick=qos_auto_trans_rate_selector(this.checked); type=checkbox > </P>
            
            <P><LABEL class=duple for=qos_max_trans_rate>Manual Uplink Speed 
            :</LABEL> <INPUT id=qos_max_trans_rate maxLength=6 size=6 value="<% getInfo("qosManualUplinkSpeed"); %>"
            name=config.qos_max_trans_rate> kbps 
            <SPAN>&nbsp;&lt;&lt;&nbsp;</SPAN> <SELECT 
            onchange=qos_max_trans_rate_selector(this.value); 
            name=qos_max_trans_rate_select> 
            <OPTION value=0 selected>Select Transmission Rate</OPTION> 
            <OPTION value=512>512 kbps</OPTION> 
            <OPTION value=1024>1024 kbps</OPTION> 
            <OPTION value=2048>2048 kbps</OPTION>
            <OPTION value=4096>4096 kbps</OPTION>
            <OPTION value=6144>6144 kbps</OPTION>
            <OPTION value=8192>8192 kbps</OPTION>
            </SELECT> </P>
            
            </DIV>
            </FIELDSET> </DIV><!-- box -->


            <DIV class=box>
            <H3>10 -- QoS Rules </H3>
            <TABLE class=formlisting cellSpacing=1 cellPadding=0 summary="" border=0>
              <TBODY>


		<SCRIPT>
		var strHtml;
		count=0;
		var token= new Array(10);
		token[0]='<% qosList("0");%>';
		token[1]='<% qosList("1");%>';
		token[2]='<% qosList("2");%>';
		token[3]='<% qosList("3");%>';
		token[4]='<% qosList("4");%>';
		token[5]='<% qosList("5");%>';
		token[6]='<% qosList("6");%>';
		token[7]='<% qosList("7");%>';
		token[8]='<% qosList("8");%>';
		token[9]='<% qosList("9");%>';

		while(count<10)
		{
			var temp=token[count];
			if(temp=='') temp='0-1-6-0.0.0.0-255.255.255.255-0-65535-0.0.0.0-255.255.255.255-0-65535-'; 
				
			t1=temp.indexOf('-');  		/*    temp.substring(0,t1) = entry.enabled*/
			t2=temp.indexOf('-',t1+1);	/*    temp.substring(t1+1,t2) = entry.priority */	/* t1+1: starting point of entry.priority  */
			t3=temp.indexOf('-',t2+1);	/*    temp.substring(t2+1,t3) = entry.protocol */
			t4=temp.indexOf('-',t3+1);	/*    temp.substring(t3+1,t4) = entry.local_ip_start */
			t5=temp.indexOf('-',t4+1);	/*    temp.substring(t4+1,t5) = entry.local_ip_end */
			t6=temp.indexOf('-',t5+1);	/*    temp.substring(t5+1,t6) = entry.local_port_start */
			t7=temp.indexOf('-',t6+1);	/*    temp.substring(t6+1,t7) = entry.local_port_end */
			t8=temp.indexOf('-',t7+1);	/*    temp.substring(t7+1,t8) = entry.remote_ip_start */
			t9=temp.indexOf('-',t8+1);	/*    temp.substring(t8+1,t9) = entry.remote_ip_end */
			t10=temp.indexOf('-',t9+1);	/*    temp.substring(t9+1,t10) = entry.remote_port_start */
			t11=temp.indexOf('-',t10+1);	/*    temp.substring(t10+1,t11) = entry.remote_port_end */
			var t12=temp.substring(0,t1);	/*   temp.substring(t11+1) = entry.entry_name */
			var t13='false';
									
			if(t12=='1') t13='true';
                  strHtml= 
                         '<TR><TD rowSpan=3><INPUT id=qos_rules_used_' +count+ ' type=hidden value=' + t12
                      + ' name=config.qos_rules[' +count+ '].used> <INPUT id=qos_rules_enabled_' +count
                      + ' type=hidden value="' +t13+ '" name=config.qos_rules[' +count+ '].enabled> '
                      + '<INPUT id=qos_rules_enabled_select_' +count
                      + ' onclick=\'table_form_enable("' +count+ '", this.checked);\' type=checkbox></TD> '
                                            
                      + '<TD>Name<BR><INPUT id=qos_rules_entry_name_' +count+ ' maxLength=15 '
                      + 'name=config.qos_rules[' +count+ '].entry_name value="'+ temp.substring(t11+1) + '"> </TD> '
                      + '<TD>Priority (1 is highest)<BR><INPUT id=qos_rules_priority_' +count+ ' maxLength=3 '
                      + 'value='+ temp.substring(t1+1,t2) + ' name=config.qos_rules[' +count+ '].priority> (1..255) </TD> '
                      + '<TD>Protocol<BR><INPUT id=qos_rules_protocol_' +count+ ' disabled '
                      + 'maxLength=3 size=5 value='+ temp.substring(t2+1,t3) + ' name=config.qos_rules[' +count+ '].protocol> '
                      + '<SPAN>&nbsp;&lt;&lt;&nbsp;</SPAN> <SELECT '
                      + 'id=qos_rules_protocol_select_' +count
                      + ' onchange=\'table_form_protocol("' +count+ '", this.value);\' value="6"> '
                      + '<OPTION value=256 selected>Any</OPTION> '
                      + '<OPTION value=6>TCP</OPTION> <OPTION value=17>UDP</OPTION> '
                      + '<OPTION value=257>Both</OPTION> <OPTION value=1>ICMP</OPTION> '
                      + '<OPTION value=-1>Other</OPTION></SELECT>  '

                      + '<INPUT id=qos_rules_protocol_menu_' +count+ ' type=hidden value=0' 
                      + ' name=config.qos_rules[' +count+ '].protocol_menu></TD></TR> '
                      
                      + '<TR><TD colSpan=2>Local IP Range<BR><INPUT '
                      + 'id=qos_rules_local_ip_start_' +count+ ' maxLength=15 size=15 '
                      + 'value='+ temp.substring(t3+1,t4) + ' name=config.qos_rules[' +count+ '].local_ip_start> to '
                      + '<INPUT id=qos_rules_local_ip_end_' +count+ ' maxLength=15 size=15 '
                      + 'value='+ temp.substring(t4+1,t5) + ' name=config.qos_rules[' +count+ '].local_ip_end> </TD> '
                      
                      + '<TD colSpan=2>Local Port Range<BR><INPUT '
                      + 'id=qos_rules_local_port_start_' +count+ ' maxLength=5 size=5 value='+ temp.substring(t5+1,t6)
                      + ' name=config.qos_rules[' +count+ '].local_port_start> to <INPUT '
                      + 'id=qos_rules_local_port_end_' +count+ ' maxLength=5 size=5 value='+ temp.substring(t6+1,t7)
                      + ' name=config.qos_rules[' +count+ '].local_port_end> </TD></TR> '
              
                      + '<TR><TD colSpan=2>Remote IP Range<BR><INPUT '
                      + 'id=qos_rules_remote_ip_start_' +count+ ' maxLength=15 size=15 '
                      + 'value='+ temp.substring(t7+1,t8) + ' name=config.qos_rules[' +count+ '].remote_ip_start> to <INPUT '
                      + 'id=qos_rules_remote_ip_end_' +count+ ' maxLength=15 size=15 '
                      + 'value='+ temp.substring(t8+1,t9) + ' name=config.qos_rules[' +count+ '].remote_ip_end></TD> '
                      
                      + '<TD colSpan=2>Remote Port Range<BR><INPUT '
                      + 'id=qos_rules_remote_port_start_' +count+ ' maxLength=5 size=5 value='+ temp.substring(t9+1,t10)
                      + ' name=config.qos_rules[' +count+ '].remote_port_start> to <INPUT '
                      + 'id=qos_rules_remote_port_end_' +count+ ' maxLength=5 size=5 value='+ temp.substring(t10+1,t11)
                      + ' name=config.qos_rules[' +count+ '].remote_port_end> </TD></TR>'

			dw(strHtml);		
		count++;
		}
		</SCRIPT>

            
            </TBODY></TABLE></DIV><!-- box --></DIV><!-- section -->
            </FORM><!-- InstanceEndEditable -->
                 <INPUT type="hidden" value="/qos.asp" name="submit-url">

            </DIV><!-- id=maincontent -->
            </TD>
            </DIV><!-- outside --><!-- InstanceEnd -->            


</BODY></HTML>

