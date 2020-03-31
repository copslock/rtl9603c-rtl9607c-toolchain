<HTML>
<! Copyright (c) Realtek Semiconductor Corp., 2003. All Rights Reserved. ->
<HEAD>
<meta http-equiv="Content-Type" content="text/html">
<META http-equiv=Pragma content=no-cache>
<script type="text/javascript" src="util_ap.js"> </script>
<SCRIPT language="JavaScript">
<!--
var mf = document.schedule_form;
var weekdays_old;
function time_retriever(time, id)
{
	var min = (time % 3600) / 60;
	var hr = (time - min*60) / 3600;
	var pm = (hr >= 12 && hr != 24) ? 1 : 0;
	switch(id) {
	case 1:
		if (hr == 0 && pm == 0) {
			return(12);	/*   300 => 12:05AM */
		}
		hr -= (hr > 12) ? 12 : 0;
		return (hr);
	case 2:
		return(min);
	case 3:
		return(pm);
	}
}
function sched_time_to_min(hr, min)
{
	return (hr*60+ min );			
}

function CheckTime()
{
	var mf = document.schedule_form;
	if(mf.enabled_sch.checked==true){
		if(mf.everyday.checked == false && parseInt(mf.weekdays.value, 10) == 0){
			alert("Please set the Days!")
				return false;
		}
	}else {
		return true;
	}
	var time1_idx =mf.fTime1.selectedIndex;
	var time2_idx =mf.fTime2.selectedIndex;
	var time3_idx =mf.tTime1.selectedIndex;
	var time4_idx =mf.tTime2.selectedIndex;
	
	if (!mf.all_day.checked) {
		mf.start_time.value = sched_time_to_min(time1_idx, time2_idx*5);
		mf.end_time.value = sched_time_to_min(time3_idx, time4_idx*5);
		if( parseInt(mf.start_time.value, 10) >= parseInt(mf.end_time.value, 10)){
			alert("The end time should bigger than start time");
			return false;
		}
	}
}
/** Convert from stored config data to UI checkbox value. */
function is_day_set(val, saved_val)
{	
	if ((saved_val.value == 0x7f) && (saved_val.value != val)) {
			return false;
	} else {
			return(((saved_val & val) == val) ? true : false);
	}
}
function all_week_radio_selector(val, clear_flag)
{
	var mf = document.schedule_form;
	var saved_val;
	if (val) {
		mf.all_week.checked = true;
		mf.weekdays.value = 127;
	} else {
		
		if (clear_flag) {
			mf.weekdays.value = 0;
		}
	}
/* Enable/disable weekdays checkbox. */
	mf.sunday.disabled = val;
	mf.monday.disabled = val;
	mf.tuesday.disabled = val;
	mf.wednesday.disabled = val;
	mf.thursday.disabled = val;
	mf.friday.disabled = val;
	mf.saturday.disabled = val;
/* Set weekdays checkbox. */
		saved_val=weekdays_old;
	mf.sunday.checked = is_day_set(0x01, saved_val);
	mf.monday.checked = is_day_set(0x02, saved_val);
	mf.tuesday.checked = is_day_set(0x04, saved_val);
	mf.wednesday.checked = is_day_set(0x08, saved_val);
	mf.thursday.checked = is_day_set(0x10, saved_val);
	mf.friday.checked = is_day_set(0x20, saved_val);
	mf.saturday.checked = is_day_set(0x40, saved_val);			
}
/** Selector function to update sched_weekdays when checkbox changes. */
function sched_weekdays_selector(checked, val)
{
	var mf = document.schedule_form;
	if (checked) {
		
		mf.weekdays.value |= val;	// set the bit
	} else {
		mf.weekdays.value &= ~val;	// clear the bit
	}
	if (mf.weekdays.value == 127) {
		all_week_radio_selector(true, false);
	}
}
function init()		
{
	var f=document.schedule_form;
	var wlanSchedule="<%getScheduleInfo("getentry_1");%>";
	var t1=wlanSchedule.indexOf('-');
	var t2=wlanSchedule.indexOf('-',t1+1);
	var t3=wlanSchedule.indexOf('-',t2+1);
	var t4=wlanSchedule.indexOf('-',t3+1);
	var t5=wlanSchedule.indexOf('-',t4+1);
	var t6=wlanSchedule.indexOf('-',t5+1);
	
	var schedule_name=wlanSchedule.substring(0,t1);
	var schedule_enabled=wlanSchedule.substring(t1+1,t2);
	var schedule_everyday=wlanSchedule.substring(t2+1,t3);
	var schedule_weekday=wlanSchedule.substring(t3+1,t4);
	var schedule_allday_24=wlanSchedule.substring(t4+1,t5);
	var schedule_startTime=wlanSchedule.substring(t5+1,t6);
	var schedule_endTime=wlanSchedule.substring(t6+1);
	var starthours=parseInt(schedule_startTime, 10)/60;
	var startmin=parseInt(schedule_startTime, 10)%60;
	var endhours=parseInt(schedule_endTime, 10)/60;
	var endmin=parseInt(schedule_endTime, 10)%60;
	f.fTime1.selectedIndex=starthours;
	f.fTime2.selectedIndex=(startmin/5);
	f.tTime1.selectedIndex=endhours;
	f.tTime2.selectedIndex=(endmin/5);
	
	if(schedule_enabled=='1')
		f.enabled_sch.checked = true;
	else
		f.enabled_sch.checked = false;
	
	
	if(	schedule_everyday == '1')
		f.everyday.checked=true;
	else
		f.everyday.checked=false;
	
	if(schedule_allday_24 =='1'){	
		f.all_day.checked=true;
		f.rTime.checked=false;
	}else{
		f.all_day.checked=false;
		f.rTime.checked=true;
	}
	
	f.weekdays.value= parseInt(schedule_weekday, 10);
	weekdays_old = f.weekdays.value;
	if(f.everyday.checked==true)	
		all_week_radio_selector(true, true);
	else
		all_week_radio_selector(false, false);
		updateConfig(0, f.enabled_sch.checked);	
		
	
}
function updateConfig(from,isChecked)
{
	var f=document.schedule_form;

	f.enabled_sch.checked = isChecked;
	if(f.enabled_sch.checked){
		if(f.everyday.checked == false){
		f.sunday.disabled=false;
		f.monday.disabled=false;
		f.tuesday.disabled=false;
		f.wednesday.disabled=false;
		f.thursday.disabled=false;
		f.friday.disabled=false;
		f.saturday.disabled=false;
		}else{
			f.sunday.disabled=true;
			f.monday.disabled=true;
			f.tuesday.disabled=true;
			f.wednesday.disabled=true;
			f.thursday.disabled=true;
			f.friday.disabled=true;
			f.saturday.disabled=true;
		}
		
		if(f.rTime.checked == true){
		f.fTime1.disabled=false;
		f.fTime2.disabled=false;
		f.tTime1.disabled=false;
		f.tTime2.disabled=false;
		}else{
			f.fTime1.disabled=true;
			f.fTime2.disabled=true;
			f.tTime1.disabled=true;
			f.tTime2.disabled=true;
		}
		f.rTime.disabled=false;
		f.everyday.disabled=false;
		f.all_day.disabled=false;
	}else{
	
		f.sunday.disabled=true;
		f.monday.disabled=true;
		f.tuesday.disabled=true;
		f.wednesday.disabled=true;
		f.thursday.disabled=true;
		f.friday.disabled=true;
		f.saturday.disabled=true;
		f.rTime.disabled=true;
		f.fTime1.disabled=true;
		f.fTime2.disabled=true;
		f.tTime1.disabled=true;
		f.tTime2.disabled=true;
		f.everyday.disabled=true;
		f.all_day.disabled=true;
		f.enabled.value=0;
	}
	if(from==1){
		if(f.enabled_sch.checked){
			var wlanSchedule="<%getScheduleInfo("getentry_1");%>";
			var t1=wlanSchedule.indexOf('-');
			var t2=wlanSchedule.indexOf('-',t1+1);
			var t3=wlanSchedule.indexOf('-',t2+1);
			var t4=wlanSchedule.indexOf('-',t3+1);
			var t5=wlanSchedule.indexOf('-',t4+1);
			var t6=wlanSchedule.indexOf('-',t5+1);
	
			var schedule_name=wlanSchedule.substring(0,t1);
			var schedule_enabled=wlanSchedule.substring(t1+1,t2);
			var schedule_everyday=wlanSchedule.substring(t2+1,t3);
			var schedule_weekday=wlanSchedule.substring(t3+1,t4);
			var schedule_allday_24=wlanSchedule.substring(t4+1,t5);
			var schedule_startTime=wlanSchedule.substring(t5+1,t6);
			var schedule_endTime=wlanSchedule.substring(t6+1);
			var starthours=parseInt(schedule_startTime, 10)/60;
			var startmin=parseInt(schedule_startTime, 10)%60;
			var endhours=parseInt(schedule_endTime, 10)/60;
			var endmin=parseInt(schedule_endTime, 10)%60;
			f.fTime1.selectedIndex=starthours;
			f.fTime2.selectedIndex=(startmin/5);
			f.tTime1.selectedIndex=endhours;
			f.tTime2.selectedIndex=(endhours/5);
	
			if(	schedule_everyday == '1')
				f.everyday.checked=true;
			else
				f.everyday.checked=false;
	
			if(schedule_allday_24 =='1'){	
				f.all_day.checked=true;
				f.rTime.checked=false;
			}else{
				f.all_day.checked=false;
				f.rTime.checked=true;
			}
			setTime();
			f.weekdays.value= parseInt(schedule_weekday, 10);
			if(f.everyday.checked==true)	
				all_week_radio_selector(true, true);
			else
				all_week_radio_selector(false, false);
		}
	}
}			
	function setDay()	
	{
		var f=document.schedule_form;
		if(f.everyday.checked)
		{
			f.sunday.checked=true;
			f.sunday.disabled=true;
			f.monday.checked=true;
			f.monday.disabled=true;
			f.tuesday.checked=true;
			f.tuesday.disabled=true;
			f.wednesday.checked=true;
			f.wednesday.disabled=true;
			f.thursday.checked=true;
			f.thursday.disabled=true;
			f.friday.checked=true;
			f.friday.disabled=true;
			f.saturday.checked=true;
			f.saturday.disabled=true;
		}
		else
		{
			f.sunday.disabled=false;
			f.monday.disabled=false;
			f.tuesday.disabled=false;
			f.wednesday.disabled=false;
			f.thursday.disabled=false;
			f.friday.disabled=false;
			f.saturday.disabled=false;
		}
	}	

	function allTime()	
	{
		var f=document.schedule_form;
		if(f.all_day.checked)
		{
			f.rTime.checked=false;
			f.fTime1.disabled=true;
			f.fTime2.disabled=true;
			f.tTime1.disabled=true;
			f.tTime2.disabled=true;
		}
		else
		{
			f.rTime.checked=true;
			f.fTime1.disabled=false;
			f.fTime2.disabled=false;
			f.tTime1.disabled=false;
			f.tTime2.disabled=false;
		}
	}

	function setTime()	
	{
		var f=document.schedule_form;
		if(f.rTime.checked)
		{
			f.all_day.checked=false;
			f.fTime1.disabled=false;
			f.fTime2.disabled=false;
			f.tTime1.disabled=false;
			f.tTime2.disabled=false;
		}
		else
		{
			f.all_day.checked=true;
			f.fTime1.disabled=true;
			f.fTime2.disabled=true;
			f.tTime1.disabled=true;
			f.tTime2.disabled=true;
		}
	}
	function submitApply()
	{
		if(CheckTime()==false)
			return false;
	}
	
	var ftTime1Str;
	function dispalyftTime1Option(s)
	{
		var ftTime1StrTmp;
		for(var i = 0; i < 24; i++){
			if(i<10){
					ftTime1StrTmp +="<OPTION >"+0+i;
			}
			else{
					ftTime1StrTmp +="<OPTION >"+i;
			}		
		}
		ftTime1Str=ftTime1StrTmp;
	}
	var ftTime2Str;
	function dispalyftTime2Option(s)
	{
		var ftTime2StrTmp;
		var i;
		for(var j = 0;j < 12;j++) 
		{
			i=j*5;
			if(i<10){
					ftTime2StrTmp +="<OPTION >"+0+i;
			}else{
					ftTime2StrTmp +="<OPTION >"+i;
			}		
		}
		ftTime2Str=ftTime2StrTmp;
	}
	
//-->
</SCRIPT>
</HEAD>
<BODY onload=init();>
	<blockquote>
<BR><h2><font color="#0000FF">Wireless Schedule</font></h2> 

<FORM id="mainform" name=schedule_form action=/goform/formSchedule method=post> 
<INPUT type=hidden value="/wlan_schedule.asp" name=webpage> 
<input type="hidden" name="enabled" id="enabled" />
<input type="hidden" name="weekdays" id="weekdays" />
<input type="hidden" name="start_time" id="start_time" />
<input type="hidden" name="end_time" id="end_time" />

<TD valign="top" width="100%" height="100%">
<TABLE cellSpacing=1 cellPadding=0 width=500 align=left border=0>
	<tr><font size=2>This page allows you setup the wireless schedule rule. Please do not forget to configure 
		system time before enable this feature.</font></tr>
 <tr><hr width="500" size=1 noshade align=left></tr>
<TBODY>
<TD></TD> 
<TR>
<TD>
<TABLE cellSpacing=1 cellPadding=0 width=500 align=left border=0>
<TBODY>
<TR>
<TD>
<TABLE cellSpacing=1 cellPadding=0 width=500 align=left border=0> 
<TBODY>
	
<tr align=left><td colspan=5><font size=2><b>
   	<input type="checkbox" name="enabled_sch" value="true"  onclick="updateConfig(1, this.checked)"/>Enable Wireless Schedule</b><br>
    </td>
</tr>
<TR><TD colspan=5>
<TABLE  cellSpacing=1 cellPadding=0 width=500 align=left border=0 bordercolor=#000000> 
<TBODY>
<TR bordercolor=#ffffff>
<TD colspan=5>
<DIV>
<BR>&nbsp;<font size=2><b>Days :</b></font><BR></DIV></TD></TR>
<TR bordercolor=#ffffff>
<TD colspan=5>	<DIV>
<INPUT type="checkbox" id="all_week" name="everyday" onclick="all_week_radio_selector(this.checked, true);"><font size=2> Everyday</font>
&nbsp;&nbsp;
<INPUT type="checkbox" name="sunday" onclick="sched_weekdays_selector(this.checked, 0x01);"/><font size=2>Sun&nbsp;</font>
<INPUT type="checkbox" name="monday" onclick="sched_weekdays_selector(this.checked, 0x02);"/><font size=2> Mon&nbsp;</font>
<INPUT type="checkbox" name="tuesday" onclick="sched_weekdays_selector(this.checked, 0x04);"/> <font size=2>Tue&nbsp;</font>
<INPUT type="checkbox" name="wednesday" onclick="sched_weekdays_selector(this.checked, 0x08);"/><font size=2> Wed&nbsp;</font>
<INPUT type="checkbox" name="thursday" onclick="sched_weekdays_selector(this.checked, 0x10);"/> <font size=2>Thu&nbsp;</font>
<INPUT type="checkbox" name="friday" onclick="sched_weekdays_selector(this.checked, 0x20);"/> <font size=2>Fri&nbsp;</font>
<INPUT type="checkbox" name="saturday"onclick="sched_weekdays_selector(this.checked, 0x40);"/><font size=2> Sat&nbsp;</font>
</DIV></TD></TR>
<TR bordercolor=#ffffff>
<TD colspan=5><DIV>
<BR>&nbsp;<font size=2><b>Time :</b></font><BR></DIV></TD></TR>
<TR bordercolor=#ffffff>
<TD colspan=5>	<DIV>
<INPUT type="radio" name="all_day" onclick="allTime()" /><font size=2> 24 Hours</font>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
<INPUT type="radio" name="rTime" onclick="setTime()"/><font size=2> From</font>&nbsp;&nbsp;
<SELECT name="fTime1" style="width:43px"/>
<SCRIPT>
dispalyftTime1Option(1);
document.write(ftTime1Str);
</SCRIPT>	
</SELECT>
&nbsp;:&nbsp;
<SELECT name="fTime2" style="width:43px"/>
<SCRIPT>
dispalyftTime2Option(1);
document.write(ftTime2Str);
</SCRIPT>	
</SELECT>
&nbsp;&nbsp;&nbsp;&nbsp;
<font size=2>To</font>&nbsp;&nbsp;
<SELECT name="tTime1" style="width:43px"/>
<SCRIPT>
dispalyftTime1Option(2);
document.write(ftTime1Str);
</SCRIPT>
</SELECT>
&nbsp;:&nbsp;
<SELECT name="tTime2" style="width:43px"/>
<SCRIPT>
dispalyftTime2Option(2);
document.write(ftTime2Str);
</SCRIPT>
</SELECT>
</DIV>
</TD>
</TR>

</TD>
</TR></TBODY></TABLE></TD></TR></TBODY></TABLE></TD></TR>

<TR><TD >&nbsp;</TD></TR>
<TR><TD>&nbsp;</TD></TR><TD>
<DIV align=left>
<input type="submit" value="Apply Changes" name="save" onClick="return submitApply();">&nbsp;&nbsp;
<input type="button" value="Reset" name="reset" onClick="init();">
</DIV>
</TD></TR>
</TBODY></TABLE></TD></TR></TBODY></TABLE></TD>
<script>
   	var wlanDisabled = <% write(getIndex("wlanDisabled")); %> ;
   	 if ( wlanDisabled == 1)
   	 			disableTextField(document.schedule_form.enabled_sch);
 </script>
</FORM></blockquote></BODY></HTML>
