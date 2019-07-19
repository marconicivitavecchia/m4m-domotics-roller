#include "common.h"
#define serverp (*serveru)

ESP8266WebServer *serveru; 
String  *paramsp;
String  *mqttJsonp;
String  *confJsonp;
String  *confcmdp;
int EEaddress = FIXEDPARAMSLEN;
int varStrOfst[CONFJSONDIM+1];
bool rollmode[2] = {true, true};

inline void eepromBegin(){
	int i, len;
	for(i=0,len=0;i<CONFJSONDIM;++i){
		len += confcmdp[i].length();
	}
	EEPROM.begin(FIXEDPARAMSLEN + len + i);
}

void initCommon(ESP8266WebServer *serveri,  String  *paramsi, String  *mqttJsoni, String  *confJsoni, String  *confcmdi){
	serveru=serveri; 
	paramsp=paramsi;
	mqttJsonp=mqttJsoni;
	confJsonp=confJsoni;
	confcmdp=confcmdi;
	for(int i=0; i<CONFJSONDIM+1; ++i){
		varStrOfst[i] = 0;
	}
}

const char HTTP_FORM_ROOT[] PROGMEM =
	"<html>{HD}<body>"
	"<div class='header' 'id='logo'><h1>MyTapparella</h1></div>"
		"<form action='login' method='POST'>"
			"<div class='grid-container'>"
				"<div class='col-6 col-s-12'>"
					"<label for='username'>Configuration web client username:</label>"
					"<input type='text' name='username' value='{WU}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='password'>Configuration web client password:</label>"
					 "<input type='password' name='password' placeholder='**********'>"
				"</div>"
			"</div>"
			"<div class='footer'>"
				"<input type='submit' value='Login'>"
			"</div>"
		"</form>"
	"</div></body></html>";

const char HTTP_FORM_WIFI[] PROGMEM =	
	"<html>{HD}<body>"
	"<div class='header' 'id='logo'><h1>MyTapparella</h1></div>"
	"<div id='form'>"
        "<form action='/login' method='POST'>"
			"<div class='grid-container'>"
				"<div class='col-6 col-s-12'><label for='wificlientssid'>Wifi instructure 1 SSID:</label>"
					 "<input type='text' name='wificlientssid1' value='{S1}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='wificlientpsw'>Wifi instructure 1 password:</label>"
					 "<input type='password' name='wificlientpsw1' value='{P1}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='wificlientssid'>Wifi instructure 2 SSID:</label>"
					 "<input type='text' name='wificlientssid2' value='{S2}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='wificlientpsw'>Wifi instructure 2 password:</label>"
					 "<input type='password' name='wificlientpsw2' value='{P2}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='wifiapusr'>Wifi local AP SSID:</label>"
					 "<input type='text' name='wifiapssid' value='{AS}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='wifiappsw'>Wifi local AP password:</label>"
					 "<input type='password' name='wifiAPPPSW' value='{AP}'>"
				"</div>"
				"<div class='col-2'></div>"
				"<div class='col-2 col-s-12'>"
					"<input type='submit' value='Save' formaction='/modify' formmethod='post'>"
				"</div>"
				"<div class='col-4'></div>"
				"<div class='col-2 col-s-12'>"
					"<input type='submit' value='Back'>"
				"</div>"
			"</div>"
        "</form>"
	"</div></div></body></html>";
	
const char HTTP_FORM_SYSTEM[] PROGMEM =	
	"<html>{HD}<body>"
	"<div class='header' 'id='logo'><h1>MyTapparella</h1></div>"
	"<div id='form'>"
        "<form action='/login' method='POST'>"
			"<div class='grid-container'>"
				"<div class='col-6 col-s-12'>"
					"<label for='username'>Configuration web client username:</label>"
					 "<input type='text' name='username' value='{WU}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='password'>Configuration web client password:</label>"
					 "<input type='password' name='password' value='{WP}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='reboot'>Reboot the system with default config</label>"
					 "<input type='checkbox' name='rebootd' value='y'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='reboot'>Reboot the system</label>"
					 "<input type='checkbox' name='reboot' value='y'>"
				"</div>"
#if (!AUTOCAL) 
				"<div class='col-6 col-s-12'>"
					"<label for=\startdelay'>Start delay on click of group 1:</label>"
					 "<input type='text' name='startdelay1' value='{S1}'>"
				"</div>"
				"<div class='col-6 col-s-12'>"
					"<label for=\startdelay'>Start delay on click of group 2:</label>"
					 "<input type='text' name='startdelay2' value='{S2}'>"
				"</div>"
#endif		
				"<div class='col-2'></div>"
					"<div class='col-2 col-s-12'>"
						"<input type='submit' value='Save' formaction='/modify' formmethod='post'>"
					"</div>"
					"<div class='col-4'></div>"
					"<div class='col-2 col-s-12'>"
						"<input type='submit' value='Back'>"
				"</div>"
			"</div>"
        "</form>"
	"</div></div></body></html>";
	
const char HTTP_FORM_MQTT[] PROGMEM =	
	"<html>{HD}<body>"
	"<div class='header' 'id='logo'><h1>MyTapparella</h1></div>"
	"<div id='form'>"
		"<form action='/login' method='POST'>"
			"<div class='grid-container'>"
				"<div class='col-6 col-s-12'><label for='mqttserver'>MQTT server:</label>"
					 "<input type='text' name='mqttserver' value='{MA}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='mqttclientid'>MQTT Client ID:</label>"
					 "<input type='text' name='mqttclientid' value='{MI}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='mqttusr'>MQTT user name:</label>"
					 "<input type='text' name='mqttusr' value='{MU}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='mqttpsw'>MQTT user password:</label>"
					 "<input type='text' name='mqttpsw' value='{MP}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='intopic'>In topic:</label>"
					 "<input type='text' name='intopic' value='{QI}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='outtopic'>Out topic:</label>"
					 "<input type='text' name='outtopic' value='{MO}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='btnup1'>MQTT message button 1 UP:</label>"
					 "<input type='text' name='btnup1' value='{J1}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='btndown1'>MQTT message button 1 DOWN:</label>"
					 "<input type='text' name='btndown1' value='{J2}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='btnup2'>MQTT message button 2 UP:</label>"
					 "<input type='text' name='btnup2' value='{J3}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='btndown2'>MQTT message button 2 DOWN:</label>"
					 "<input type='text' name='btndown2' value='{J4}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='btntemp'>Temperature request button:</label>"
					 "<input type='text' name='btntemp' value='{J5}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='btnmeanpwr'>Mean power request button:</label>"
					 "<input type='text' name='btnmeanpwr' value='{J6}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='btnpeakpwr'>Peak power request button:</label>"
					 "<input type='text' name='btnpeakpwr' value='{J7}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='btnall'>All states request button:</label>"
					 "<input type='text' name='btnall' value='{J8}'>"
				"</div>"				
				"<div class='col-2'></div>"
				"<div class='col-2 col-s-12'>"
					"<input type='submit' value='Save' formaction='/modify' formmethod='post'>"
				"</div>"
				"<div class='col-4'></div>"
				"<div class='col-2 col-s-12'>"
					"<input type='submit' value='Back'>"
				"</div>"
			"</div>"
		"</form>"
	"</div></body></html>";
	
const char HTTP_FORM_LOGIC[] PROGMEM =	
	"<html>{HD}<body>"
	"<div class='header' 'id='logo'><h1>MyTapparella</h1></div>"
	"<div id='form'>"
        "<form action='/login' method='POST'>"
			"<div class='grid-container'>"
				"<div class='col-6 col-s-12'><label for='swmode1'>Mode switch 1 and 2 (Switch if checked, Roller Shutter if unchecked)</label>"
					 "<input type='checkbox' name='swmode1' value='0' {H1}>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='swmode2'>Mode switch 3 and 4 (Switch if checked, Roller Shutter if unchecked)</label>"
					 "<input type='checkbox' name='swmode2' value='0' {H2}>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='thalt1'>End-of-stroke time switch 1:</label>"
					 "<input type='text' name='thalt1' value='{TU}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='thalt2'>End-of-stroke time switch 2:</label>"
					 "<input type='text' name='thalt2' value='{TD}'  {H3}>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='thalt3'>End-of-stroke time switch 3:</label>"
					 "<input type='text' name='thalt3' value='{DU}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='thalt4'>End-of-stroke time switch 4:</label>"
					 "<input type='text' name='thalt4' value='{DD}'  {H4}>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='taplength'>Rollershutter excursion:</label>"
					 "<input type='text' name='taplength' value='{TL}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='barrelrad'>Barrel radius:</label>"
					 "<input type='text' name='barrelrad' value='{BR}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='thickness'>Roller shutter thickness</label>"
					 "<input type='text' name='thickness' value='{TN}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='slatsratio'>Roller shutter slatsratio</label>"
					 "<input type='text' name='slatsratio' value='{SR}'>"
				"</div>"
				"<div class='col-2'></div>"
				"<div class='col-2 col-s-12'>"
					"<input type='submit' value='Save' formaction='/modify' formmethod='post'>"
				"</div>"
				"<div class='col-4'></div>"
				"<div class='col-2 col-s-12'>"
					"<input type='submit' value='Back'>"
				"</div>"
			"</div>"   
        "</form>"
	"</div></body></html>";
		
	//EVENT MANAGEMENT CONFIG
	const char HTTP_FORM_EVENT[] PROGMEM =	
	"<html>{HD}<body>"
	"<div class='header' 'id='logo'><h1>MyTapparella</h1></div>"
	"<div id='form'>"
		"<form action='/login' method='POST'>"
			"<div class='grid-container'>"
				"<div class='col-6 col-s-12 boxed'>"
					"<label for='c1'>Switch 1 condition:</label>"
					"<textarea id='c1' name='c1' cols='100' rows='4' {H3}>{C1}</textarea>"
					"<label for='a1'>Switch 1 action</label>"
					 "<select id='a1' name='a1'>"
						  "<option value='0'>SetReset</option>"
						  "<option value='1'>No action</option>"
						  "<option value='2'>Monostable normally open</option>"
						  "<option value='3'>Monostable normally close</option>"
					 "</select><br>"
					 "<label for='haltdelay1'>Delay for timer SW 1</label>"
					 "<input type='text' id='haltdelay1' name='haltdelay1' value='{D1}'>"
					 "<label for='smplt1'>Sample time SW 1</label>"
					 "<input type='text' id='smplt1' name='smplt1' value='{S1}'>"
					 "<label for='oe1'>Output enable SW 1</label>"
					 "<input type='checkbox' id='oe1' name='oe1' value='0' {O1}>"
				"</div>"
				"<div class='col-6 col-s-12 boxed'>"
					"<label for='c2'>Switch 2 condition:</label>"
					"<textarea id='c2' name='c2' cols='100' rows='4' {H3}>{C2}</textarea>"
					"<label for='a2'>Switch 2 action</label>"
					"<select id='a2' name='a2'>"
						  "<option value='0'>SetReset</option>"
						  "<option value='1'>No action</option>"
						  "<option value='2'>Monostable normally open</option>"
						  "<option value='3'>Monostable normally close</option>"
					"</select><br>"
					"<label for='haltdelay2'>Delay for timer SW 2</label>"
					"<input type='text' id='haltdelay2' name='haltdelay2' value='{D2}'>"
					"<label for='smplt2'>Sample time SW 2</label>"
					"<input type='text' id='smplt2' name='smplt2' value='{S2}'>"
					"<label for='oe2'>Output enable SW 2</label>"
					"<input type='checkbox' id='oe2' name='oe2' value='0' {O2}>"
				"</div>"
				"<div class='col-6 col-s-12 boxed'>"
					"<label for='c3'>Switch 3 condition:</label>"
					"<textarea id='c3' name='c3' cols='100' rows='4' {H4}>{C3}</textarea>"
					"<label for='a3'>Switch 3 action</label>"
					"<select id='a3' name='a3'>"
						  "<option value='0'>SetReset</option>"
						  "<option value='1'>No action</option>"
						  "<option value='2'>Monostable normally open</option>"
						  "<option value='3'>Monostable normally close</option>"
					"</select><br>"
					"<label for='haltdelay3'>Delay for timer SW 3</label>"
					"<input type='text' id='haltdelay3' name='haltdelay3' value='{D3}'>"
					"<label for='smplt3'>Sample time SW 3</label>"
					"<input type='text' id='smplt3' name='smplt3' value='{S3}'>"
					"<label for='oe3'>Output enable SW 3</label>"
					"<input type='checkbox' id='oe3' name='oe3' value='0' {O3}>"
				"</div>"
				"<div class='col-6 col-s-12 boxed'>"
					"<label for='c4'>Switch 4 condition:</label>"
					"<textarea id='c4' name='c4' cols='100' rows='4' {H4}>{C4}</textarea>"
					"<label for='a4'>Switch 4 action</label>"
					"<select id='a4' name='a4'>"
						  "<option value='0'>SetReset</option>"
						  "<option value='1'>No action</option>"
						  "<option value='2'>Monostable normally open</option>"
						  "<option value='3'>Monostable normally close</option>"
					"</select><br>"
					"<label for='haltdelay4'>Delay for timer SW 4</label>"
					"<input type='text' id='haltdelay4' name='haltdelay4' value='{D4}'>"
					"<label for='smplt4'>Sample time SW 4</label>"
					"<input type='text' id='smplt4' name='smplt4' value='{S4}'>"
					"<label for='oe2'>Output enable SW 4</label>"
					"<input type='checkbox' id='oe4' name='oe4' value='0' {O4}>"
				"</div>"
	
				"<div class='col-12 col-s-12 boxed'>"
					"<label for='act'>Switches action commands:</label>"
					"<textarea id='act' name='act' cols='100' rows='4' {H4}>{AC}</textarea>"
				"</div>"
				"<div class='col-12 col-s-12'>"
					"<input type='submit' value='Back'>"
				"</div>"
			"</div>"
		"</form>"
	"</div>"	
	"<script>"
		//client side dynamic event management
		"{SH}"
		"var oe1=document.getElementById('oe1');"
		"var oe2=document.getElementById('oe2');"
		"var oe3=document.getElementById('oe3');"
		"var oe4=document.getElementById('oe4');"
		"oe1.addEventListener('change', function(){condsend(0,'c1')}, false);"
		"oe2.addEventListener('change', function(){condsend(1,'c2')}, false);"
		"oe3.addEventListener('change', function(){condsend(2,'c3')}, false);"
		"oe4.addEventListener('change', function(){condsend(3,'c4')}, false);"
		"var act=document.getElementById('act');"
		"act.addEventListener('change', function(){actsend(act.value);}, false);"
		"function setfield(str,field,val){"
			"if(str.search(field)>=0){"
				"var digit = '=[0-9]+';"
				"var regex = new RegExp(field+digit, 'g');"
				"str = str.replace(regex,field+'='+val);"
			"}else{"
				"str = str+'|('+field+'='+val+')';"
			"}"
			"console.log('digit:'+ digit);"
			"console.log('field: '+field);"
			"console.log('val: '+field+'='+val);"
			"console.log('setfield: '+str);"
			"return str;"
		"}"
		"function addCheck(chkstr,act){"
			"var chk=document.getElementById(chkstr);"
			"chk.addEventListener('change', function (event) {"
				"if(chk.checked){"
					"act.value=setfield(act.value,chkstr,'1');"
				"}else{"
					"act.value=setfield(act.value,chkstr,'0');"
				"}"
				"actsend(act.value);"
			"});"
		"}"
		"function readSelect(selel,n,act){"
			//0: toggleLogic
			//1: condition output disabled
			//2: normalmente aperto, chiuso per un haltdelay alla pressione
			//3: normalmente chiuso, aperto per un haltdelay alla pressione
			"var sel=document.getElementById(selel);"
			//"var s=sel.options[sel.selectedIndex].value;"
			"sel.addEventListener('change', function (event) {"
				"console.log('selectedIndex: '+(sel.selectedIndex).toString());"
				"act.value=setfield(act.value,'ma'+n,(sel.selectedIndex).toString());"
				//"actsend(act.value);"
			"});"
		"}"
		"function readInput(inel,n,cmd,act){"
			"var inp=document.getElementById(inel);"
			//"var s=sel.options[sel.selectedIndex].value;"
			"inp.addEventListener('change', function (event) {"
				"console.log('selectedIndex: '+inp.value);"
				"act.value=setfield(act.value,cmd+n,inp.value);"
				//"actsend(act.value);"
			"});"
		"}"
		"addCheck('oe1',act);"
		"addCheck('oe2',act);"
		"addCheck('oe3',act);"
		"addCheck('oe4',act);"
		"readSelect('a1',1,act);"
		"readSelect('a2',2,act);"
		"readSelect('a3',3,act);"
		"readSelect('a4',4,act);"
		"readInput('haltdelay1',1,'td',act);"
		"readInput('haltdelay2',2,'td',act);"
		"readInput('haltdelay3',3,'td',act);"
		"readInput('haltdelay4',4,'td',act);"
		"readInput('smplt1',1,'tsmpl',act);"
		"readInput('smplt2',2,'tsmpl',act);"
		"readInput('smplt3',3,'tsmpl',act);"
		"readInput('smplt4',4,'tsmpl',act);"
	"</script>"
	"</body></html>";
	//END EVENT MANAGEMENT CONFIG*/

/*const char [] PROGMEM =		
	"<label for='action'>Action {I} delay</label>"
	"<input id='d{I}' type='text' name='d{I}' value='D{I}'>"
	"<input id='sw{I}' type='button' value='Switch {I} config' onclick='confsend({I},'d{I}','c{I}')'>";*/
	
const char HTTP_FORM_LOGIN[] PROGMEM =	
	"<html>{HD}<body>"
	"<div class='header' 'id='logo'><h1>MyTapparella</h1></div>"
	"<div class='grid-container'>"
		"<div class='col-3 col-s-12'></div>"
		"<div id='form' class='col-6 col-s-12'>"
			"<form action='/login' method='POST'>"
				"<input type='submit' formaction='systconf' formmethod='post' value='System configuration'>"
				"<input type='submit' formaction='/wificonf' formmethod='post' value='WiFi configuration'>"
				"<br><input type='submit' formaction='mqttconf' formmethod='post' value='MQTT configuration'>"
				"<br><input type='submit' formaction='logicconf' formmethod='post' value='Logic configuration'>"
				"<br><input type='submit' formaction='eventconf' formmethod='post' value='Events configuration'>"
				"<br><input type='submit' value='Exit' name='disconnect'>"
			"</form>"
		"</div>"
		"<div class='col-3 col-s-12'></div>"
	"</div></body></html>";
	
const char HTTP_FORM_SUCCESS[] PROGMEM =	
	"<html>{HD}<body>"
	"<div class='header' 'id='logo'><h1>MyTapparella</h1></div>"
	"<div class='grid-container'>"
	"<div id='form' class='col-12 col-s-12'>"
		"<form action='/login' method='POST'>"
			"<div class='col-1-1'>"
                "<label><h1><p>Modifica avvenuta con successo.</p></label>"
            "</div>"
			"<input type='submit' value='Back'>"
		"</form>"
	"</div></div></body></html>";
	
const char HTTP_FORM_CMD[] PROGMEM =	
	//{"OUTSLED":"0","up1":"1","down1":"0","up2":"50","down2":"0"}
	"<html>{HD}<body>"
	"<div class='header' 'id='logo'><h1>MyTapparella</h1></div>"
	"<div class='grid-container'>"
		"<div class='col-4 col-s-12'>"
			"<div class='aside'>"
				"<span id='temp'></span>"
			"</div>"
		"</div>"
		"<div class='col-6 col-s-12 col-s-12'>"
			"<div id='form'>"
				"<form>"
					"<input id='up1' type='button' value='Button 1 UP' onclick='press(vls[0])' onmousedown='this.style.opacity='1'' onmouseup='this.style.opacity='0.6'' ontouchstart='this.style.opacity=\"1\"' ontouchend='this.style.opacity=\"0.6\"'>"
					"<br>"
					"<input id='down1' type='button' value='Button 1 DOWN' onclick='press(vls[1])' onmousedown='this.style.opacity=\"1\"' onmouseup='this.style.opacity=\"0.6\"' ontouchstart='this.style.opacity=\"1\"' ontouchend='this.style.opacity=\"0.6\"'>"
					"<br><br><br>"
					"<input id='up2' type='button' value='Button 2 UP' onclick='press(vls[2])' onmouseup='this.style.opacity=\"0.6\"' ontouchstart='this.style.opacity=\"1\"' ontouchend='this.style.opacity=\"0.6\"'>"
					"<br>"
					"<input id='down2' type='button' value='Button 2 DOWN' onclick='press(vls[3])' onmouseup='this.style.opacity=\"0.6\"' ontouchstart='this.style.opacity=\"1\"' ontouchend='this.style.opacity=\"0.6\"'>"
				"</form>"
				//"<p id='p'></p>"
			"</div>"
		"</div>"
		"<div class='col-2 col-s-12'>"
		"</div>"
	"</div>"
	"<div class='footer'>"
		"<div class='sldcont1' style='width:90%; clear:both; margin:0 auto'>"
			"<p>Group 1: <span id='val1'></span></p>"
			"<meter id='pr1' low='{PD}' min='0' max='100' ></meter>"
			"<input type='range' min='0' max='100' value='50' list='tickmarks' class='slider' id='rng1'>"
			"<datalist id='tickmarks'>"
				"<option>0</option>"
				"<option>{PD}</option>"
				"<option id='mid'></option>"
				"<option>100</option>"
			"</datalist>"
		"</div>"
		"<div class='sldcont2' style='width:90%; margin:0 auto'>"
			"<p>Group 2: <span id='val2'></span></p>"
			"<meter id='pr2' low='{PD}' min='0' max='100'></meter>"
			"<input type='range' min='0' max='100' value='50' list='tickmarks' class='slider' id='rng2'>"
		"</div>"
	"</div>"	
	"<script>{SH}</script>"
	"<script>"
		"var a=[0,0];"
		"var updt=[0,0];"
		"var p=[0,0];"
		"var tr=[0,0];"
		"(document.getElementById('mid')).innerHTML=50-{PD};"
		//"var c=[0,0];"
		"var dir=[0,0];"
		"var ie=[0,0,0,0];"
		"var sld1 = document.getElementById('rng1');"
		"var o1 = document.getElementById('val1');"
		"var tmp = document.getElementById('temp');"
		"o1.innerHTML = sld1.value;"
		"sld1.ontouchend = function() {"
			"o1.innerHTML = this.value;"
			"var pp=Number(sld1.value)+110;"
			"var vl=vlsp[0].replace('N', pp.toString());"
			"console.log(vl);"
			"press(vl);"
		"};"
		"sld1.onmouseup=sld1.ontouchend;"
		
		"var sld2 = document.getElementById('rng2');"
		"var o2 = document.getElementById('val2');"
		"o2.innerHTML = sld2.value;"
		"sld2.ontouchend = function() {"
			"o2.innerHTML = this.value;"
			"var pp=Number(sld2.value)+110;"
			"var vl=vlsp[1].replace('N', pp.toString());"
			"console.log(vl);"
			"press(vl);"
		"};"
		"sld2.onmouseup=sld2.ontouchend;"
		"function onRcv(f) {"
			//"document.getElementById('p').innerHTML = f.data;\n"
			"var obj = JSON.parse(f.data);"
			"for(x in obj){"
				"var el = document.getElementById(x);"
				"if(el){"  //controlla se il campo esiste nel DOM della pagina
					//"console.log(x);"
					"if(x=='up1'){"
						"if(obj[x] == \"1\"){"
							"ie[0]=1;"
						"}else{"
							"ie[0]=0;"
						"}"
					"}"
					"if(x=='down1'){"
						"if(obj[x] == \"1\"){"
							"ie[1]=1;"
						"}else{"
							"ie[1]=0;"
						"}"
					"}"
					"if(x=='up2'){"
						"if(obj[x] == \"1\"){"
							"ie[2]=1;"
						"}else{"
							"ie[2]=0;"
						"}"
					"}"
					"if(x=='down2'){"
						"if(obj[x] == \"1\"){"
							"ie[3]=1;"
						"}else{"
							"ie[3]=0;"
						"}"
					"}"
					"if(obj[x] == \"1\"){"
						"el.style.backgroundColor = \"#b30000\";"
					"}else{"
						"el.style.backgroundColor = \"#00ccff\";"
					"}"
					"dir[0]=ie[0]+ie[1]*-1;"
					"dir[1]=ie[2]+ie[3]*-1;"
					//"console.log('a[0]:'+a[0]);" 
					//"console.log('x:'+x);" 
					"if(x=='pr1' && a[0]==0){"
						"a[0]=1;"
						"pr1.value=obj[x];"
						//"startPrgrBar(pr1,p[0],100,0);"
					"}"
					"if(x=='pr2' && a[1]==0){"
						"a[1]=1;"
						"pr2.value=obj[x];"
						//"startPrgrBar(pr2,p[1],100,1);"
					"}"
					"if(x=='{TP}'){"
						"tmp.innerHTML=obj[x]+' &#176;'+'C';"
						"tmp.style.backgroundColor = \"#333\";"
					"}"
				"}else{"	
					"if(x=='sp1')"
						"p[0]=Number(obj[x]);"						
					"if(x=='sp2')"
						"tr[1]=Number(obj[x]);"
					"if(x=='tr1')"
						"tr[0]=Number(obj[x]);"						
					"if(x=='tr2')"
						"tr[1]=Number(obj[x]);"
				"};"
			"}"
			"console.log('pr1:'+pr1.value+' p[0]:'+p[0]);" 
			//"console.log('x:'+x);" 
			"startPrgrBar(pr1,tr[0],p[0],100,5,0);"
			"startPrgrBar(pr2,tr[1],p[1],100,5,1);"
		"}"	
		"function startPrgrBar(p,ti,l,k,delay,n) {"
			"var t=ti;"
			//"l+=delay;"
			//"console.log('------------------------------------');"
			"var e=(dir[n]*l)/k;"
			//"console.log('dir[n]:'+dir[n]);"
			//"console.log('e:'+e);"
			//"console.log('----------------------------------');"
			"clearInterval(updt[n]);"
			"var aa=Math.round(calcLen(t,l));"
			"if(isNaN(aa))p.value=0; else p.value=aa;"
			"console.log('aa: '+aa);"
			"console.log('t:'+t+' p.value:'+p.value);"
			"updt[n]=setInterval(function(){"
				"if(dir[n]!=0){"
					"t=t+e;"
					"aa=Math.round(calcLen(t-delay*dir[n],l));"
					"console.log('aa: '+aa);"
					"if(isNaN(aa))p.value=0; else p.value=aa;"
					"console.log('++++++++++++++');"
					"console.log('t:'+(t-delay*dir[n])+' p.value:'+p.value+' dir:'+dir[n]);"
				"}else{"
					"a[n]=0;"
				"};"
			"}, l/k);"
		"};"
		/*"function calct(v){"   
			"var tl={TL};"
			"var br={BR};"
			"var tn={TN};"
			"var nmax={NM};"
			"console.log('tl:'+tl+' br:'+br+' tn:'+tn+' nmax:'+nmax);"
			"var nv;"
			"nv = (Math.sqrt((v*tl*tn)/Math.PI/100 + br*br) - br) / tn;"	
			"nv =  nv / nmax * 100;"
			"return  nv;"
		"}"*/
		"function calcLen(t,maxt){"
			"var tl={TL};"
			"var br={BR};"
			"var tn={TN};"
			"var nmax={NM};"
			"var app = t/maxt*nmax;"
			"return  Math.PI*app*(app*tn + 2*br)*100/tl;"
		"}"
		"{SH}"
		"document.onload='function(){"
			"dir[0]=1;"
			"dir[1]=1;"
			"pr1.value=10;"
			"pr2.value=20;"
			//"startPrgrBar(pr1,10000,100,0);"
			//"startPrgrBar(pr2,10000,100,1);"
		"}';"
	"</script>"
	"</body></html>";

const char HTTP_FORM_HEAD[] PROGMEM =
"<head><meta charset=\"UTF-8\"><title>Document</title>"
"<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
"<style>"
"* {"
	"box-sizing: border-box;"
"}"

"[class*='col-'] {"
	"padding: 0 15px;"
	"margin:0;"
"}"

"html, body, div {"
	"font-family: \"arial\", \"helvetica\", serif;"
	"font-size: 1.1rem;"
	"color: #fff;"
	"background-color: #333;"
"}"

".header {"
	"background-color: #333;"
	"color: #ffffff;"
	"padding: 15px;"
	"text-align: center;"
"}"

"#logo {"
	"text-align: center;"
"}"

"label{"
	"margin-left: 5%;"
	"color: #0099cc;"
"}"

"#form {"
	"margin-top: 20px;"
	"justify-content: center;"
	//"border 5px solid yellow;"
"}"

"input, textarea, select {"
	"width: 100%;"
	"min-height: 2.4rem;"
	"border-radius: 25px;"
	"margin: 20px 0;"
	"border: 1px solid #0099cc;"
	"font-size: 1.1rem;"
"}"

"meter {"
	"width: 100%;"
	"height:10px;"
"}"

".boxed {"
	"border-radius: 25px;"
	"border: 1px solid gray;"
	"margin: 25px 0;"
	"padding: 3% 3%;"
"}"

"input[type='submit'],input[type='button']{"
	"background-color: #333;"
	"color: #fff;"
	"max-width: 100%;"
	"min-width: 50%;"
	//"border-radius: 25px;"
	//"margin: 20px 0;"
	"font-size: 1.5rem;"
	"padding-left: 0px;"
	"min-height: 2.8rem;"
	"background-color: #00ccff;"
	"border: 3px solid #0099cc;"
	"opacity: 0.6;"
	"transition: 0.3s;"
"}"

".aside {"
	"text-align: center;"
	"font-size: 4.1rem;"
	"height: 500px;"
	"margin-top: auto;"
	"margin-bottom: auto;"
	"background-color: #333;"
"}"

"#temp {"
	"display:inline-block;"
	"margin: 38% 0px;"
	"font-size: 4.1rem;"
	"height: 500px;"
	"background-color: #333;"
"}"

".footer {"
	"background-color: #333;"
	"color: #ffffff;"
	"text-align: center;"
	"font-size: 12px;"
	"padding: 15px;"
"}"

".grid-container {"
	"/* For mini: */"
	"display: grid;"
	"grid-template-columns: 1fr;"
	"grid-column-gap: 2%;"
"}"

"@media only screen and (min-width: 600px) {"
	"/* For tablets: */"
	".grid-container {"
	  "grid-template-columns: 1fr 1fr 1fr 1fr 1fr 1fr 1fr 1fr 1fr 1fr 1fr 1fr;"
	"}"
	".col-s-1 {grid-column:   span 1;}"
	".col-s-2 {grid-column:   span 2;}"
	".col-s-3 {grid-column:   span 3;}"
	".col-s-4 {grid-column:   span 4;}"
	".col-s-5 {grid-column:   span 5;}"
	".col-s-6 {grid-column:   span 6;}"
	".col-s-7 {grid-column:   span 7;}"
	".col-s-8 {grid-column:   span 8;}"
	".col-s-9 {grid-column:   span 9;}"
	".col-s-10 {grid-column:  span 10;}"
	".col-s-11 {grid-column:  span 11;}"
	".col-s-12 {grid-column:  span 12;}"
"}"

"@media only screen and (min-width: 768px) {"
	"/* For desktop: */"
	".grid-container {"
	  "grid-template-columns: 1fr 1fr 1fr 1fr 1fr 1fr 1fr 1fr 1fr 1fr 1fr 1fr;"
	"}"
	".col-1 {grid-column:   span 1;}"
	".col-2 {grid-column:   span 2;}"
	".col-3 {grid-column:   span 3;}"
	".col-4 {grid-column:   span 4;}"
	".col-5 {grid-column:   span 5;}"
	".col-6 {grid-column:   span 6;}"
	".col-7 {grid-column:   span 7;}"
	".col-8 {grid-column:   span 8;}"
	".col-9 {grid-column:   span 9;}"
	".col-10 {grid-column:  span 10;}"
	".col-11 {grid-column:  span 11;}"
	".col-12 {grid-column:  span 12;}"
"}"

"<script>"
	"function pushopacity(bid){"
		"var btn=document.getElementById(bid);"
		"btn.onmousedown='btn.style.opacity=\"1\"';"
		"btn.onmouseup='btn.style.opacity=\"0.6\"';"
		"btn.ontouchstart='btn.style.opacity=\"1\"';"
		"btn.ontouchend='btn.style.opacity=\"0.6\"';"
	"}"
"</script>"
"</style>"
"</head>";


const char HTTP_WEBSOCKET[] PROGMEM =
		"var vls = ['{\"up1\":\"255\"}','{\"down1\":\"255\"}','{\"up2\":\"255\"}','{\"down2\":\"255\"}'];"
		"var vlsp = ['{\"up1\":\"N\"}','{\"up2\":\"N\"}'];"
		"var action = '{\"onaction\":\"D\"}';"
		"var cond = ['{\"oncond1\":\"C\"}','{\"oncond2\":\"C\"}','{\"oncond3\":\"C\"}','{\"oncond4\":\"C\"}'];"
		"var conn = new WebSocket('ws://{WS}:81/', ['arduino']);"
		"conn.onopen = function () {"
			"conn.send('Connect ' + new Date());"
		"};"
		"conn.onerror = function (error) {"
			"console.log('WebSocket Error ', error);"
			"conn.close();"
			"conn = new WebSocket('ws://{WS}:81/', ['arduino']);"
		"};"
		"conn.onmessage = function (e) {"
			"console.log('Server: ', e.data);"
			"onRcv(e);"
		"};"
		"conn.onclose = function () {"
			"console.log('WebSocket connection closed');"
			"conn = new WebSocket('ws://{WS}:81/', ['arduino']);"
		"};"
		"function actsend(str){"
			//"var btn=document.getElementById(eid);"
			"console.log('actsend: '+str);"
			"var v=action.replace('D',str);"
			"conn.send(v);"
		"};"
		"function condsend(i,eid){"
			"var el=document.getElementById(eid);"
			"var c=cond[i].replace('C',el.value);"
			"conn.send(c);"
			"console.log('condsend: '+c);"
		"};"
		"function confsend(i,actid,cnfid){"
			"actsend(i,actid);"
			"condsend(i,cnfid);"
		"};"
		"function press(s){"
			"conn.send(s);"
		"};";

	//-------------------------------------------------------------------
//fine variabili globali

void handleNotFound(){
	DEBUG_PRINTLN(F("Enter handleNotFound"));
	is_authentified(serverp);
}
	
void handleRoot() {   // When URI / is requested, send a web page with a button to toggle the LED
	
	String page = FPSTR(HTTP_FORM_ROOT);
	page.replace(F("{HD}"),  FPSTR(HTTP_FORM_HEAD) );
	page.replace(F("{WU}"),  paramsp[WEBUSR] ); 
	page.replace(F("{WT}"),  F("48") );
	DEBUG_PRINTLN(F("Enter handleRoot"));
	//String header;
	if (!is_authentified(serverp)) {
	serverp.send(200, "text/html", page);
		
	}else{
		serverp.sendHeader("Location", "/login");
		serverp.sendHeader("Cache-Control", "no-cache");
		serverp.sendHeader("Set-Cookie", "ESPSESSIONID=0");
		serverp.send(301);
	}
}

void handleLogin() {  // If a POST request is made to URI /login
  DEBUG_PRINTLN(F("Enter handleLogin"));
  //I parametri NON devono essere modificati
  bool ok=false;

  if(serverp.hasArg("disconnect")) {
	DEBUG_PRINTLN(F("Disconnection"));
	//like head command of PHP
    serverp.sendHeader("Location", "/");
    serverp.sendHeader("Cache-Control", "no-cache");
    serverp.sendHeader("Set-Cookie", "ESPSESSIONID=0");
    serverp.send(301);
	return;
  }
  else if(is_authentified(serverp)) 
  {
#if (DEBUG)	
	DEBUG_PRINT(F("handleLogin: found cookie: "));
    String cookie = serverp.header("Cookie");
    DEBUG_PRINTLN(cookie);
#endif
	ok=true;
  }
  else if(!serverp.hasArg("username")||!serverp.hasArg("password")||serverp.arg("username")==NULL||serverp.arg("password")==NULL)
  { // If the POST request doesn't have username and password data
     DEBUG_PRINTLN(F("Userneme "));
	 DEBUG_PRINTLN(serverp.arg("username"));
	 DEBUG_PRINTLN(F(" o password "));
	 DEBUG_PRINTLN(serverp.arg("password"));
	 DEBUG_PRINTLN(F(" assenti "));
	 ok=false;
  }
  else if(serverp.arg("username") == paramsp[WEBUSR] && serverp.arg("password") == paramsp[WEBPSW])
  {
	DEBUG_PRINTLN(F("Login di "));
	DEBUG_PRINTLN(serverp.arg("username"));
	DEBUG_PRINTLN(F(" effettuato con successo "));
	ok=true;
  }else{
	DEBUG_PRINTLN(F("Userneme "));
	DEBUG_PRINTLN(serverp.arg("username"));
	DEBUG_PRINTLN(F(" o password "));
	DEBUG_PRINTLN(serverp.arg("username"));
	DEBUG_PRINTLN(F(" scorretti"));
	ok=false;  
  }
  
  String page = FPSTR(HTTP_FORM_LOGIN);
	
  if(ok) { 
		// If both the username and the password are correct
		//Head placeholders
		page.replace(F("{HD}"), FPSTR(HTTP_FORM_HEAD));
		//page.replace(F("{WT}"), F("31.333333") );
		//set cookies OK
		//DEBUG_PRINTLN(F("Scrittura cookie login "));
		//DEBUG_PRINTLN(page);
		serverp.sendHeader("Cache-Control", "no-cache");
		serverp.sendHeader("Set-Cookie", "ESPSESSIONID=1");
		serverp.send(200, "text/html", page);
  } else {   
		DEBUG_PRINTLN(F("Login non consentito "));
		serverp.sendHeader("Location", "/");
		serverp.sendHeader("Cache-Control", "no-cache");
		serverp.send(301);
  }
}

void handleWifiConf() {  // If a POST request is made to URI /login
  //I parametri NON devono essere modificati
  bool ok=false;

  DEBUG_PRINTLN(F("Enter handleWifiConf"));
  if(is_authentified(serverp)) 
  {
#if (DEBUG)	
	DEBUG_PRINT(F("handleWifiConf: found cookie: "));
    String cookie = serverp.header("Cookie");
    DEBUG_PRINTLN(cookie);
#endif
	ok=true;
  }
    
  String page = FPSTR(HTTP_FORM_WIFI);
	
  if(ok) { 
		//Head placeholders
		page.replace(F("{HD}"), FPSTR(HTTP_FORM_HEAD) );
		page.replace(F("{WT}"), F("31.333333") );
		//Body placeholders
		page.replace(F("{S1}"), paramsp[CLNTSSID1] );
		page.replace(F("{P1}"), paramsp[CLNTPSW1] );
		page.replace(F("{S2}"), paramsp[CLNTSSID2] );
		page.replace(F("{P2}"), paramsp[CLNTPSW2] );
		page.replace(F("{AS}"), paramsp[APPSSID] );
		page.replace(F("{AP}"), paramsp[APPPSW] );
		//set cookies OK
		//DEBUG_PRINTLN(page);
		//DEBUG_PRINTLN(F("Scrittura cookie handleMQTTConf "));
		serverp.sendHeader("Cache-Control", "no-cache");
		serverp.sendHeader("Set-Cookie", "ESPSESSIONID=1");
		serverp.send(200, "text/html", page);
  } else {   
		DEBUG_PRINTLN(F("Login non consentito "));
		serverp.sendHeader("Location", "/");
		serverp.sendHeader("Cache-Control", "no-cache");
		serverp.send(301);
  }
}

void handleSystemConf() {  // If a POST request is made to URI /login
  //I parametri NON devono essere modificati
  bool ok=false;

  DEBUG_PRINTLN(F("Enter handleSystem"));
  if(is_authentified(serverp)) 
  {
#if (DEBUG)	
	DEBUG_PRINT(F("handleSystem: found cookie: "));
    String cookie = serverp.header("Cookie");
    DEBUG_PRINTLN(cookie);
#endif
	ok=true;
  }
    
  String page = FPSTR(HTTP_FORM_SYSTEM);
	
  if(ok) { 
		//Head placeholders
		page.replace(F("{HD}"), FPSTR(HTTP_FORM_HEAD) );
		page.replace(F("{WT}"), F("31.333333") );
		//Body placeholders
		page.replace(F("{WU}"), paramsp[WEBUSR] ) ;
		page.replace(F("{WP}"), paramsp[WEBPSW] );
#if (!AUTOCAL) 
		page.replace(F("{S1}"), paramsp[STDEL1] );
		page.replace(F("{S2}"), paramsp[STDEL2] );
#endif
		//set cookies OK
		//DEBUG_PRINTLN(page);
		//DEBUG_PRINTLN(F("Scrittura cookie handleMQTTConf "));
		serverp.sendHeader("Cache-Control", "no-cache");
		serverp.sendHeader("Set-Cookie", "ESPSESSIONID=1");
		serverp.send(200, "text/html", page);
  } else {   
		DEBUG_PRINTLN(F("Login non consentito "));
		serverp.sendHeader("Location", "/");
		serverp.sendHeader("Cache-Control", "no-cache");
		serverp.send(301);
  }
}

void handleMQTTConf() {  // If a POST request is made to URI /login
  //I parametri NON devono essere modificati
  bool ok=false;

  DEBUG_PRINTLN(F("Enter handleMQTTConf"));
  if(is_authentified(serverp)) 
  {
#if (DEBUG)	
	DEBUG_PRINT(F("handleMQTTConf: found cookie: "));
    String cookie = serverp.header("Cookie");
    DEBUG_PRINTLN(cookie);
#endif
	ok=true;
  }
    
  String page = FPSTR(HTTP_FORM_MQTT);
	
  if(ok) { 
		//Head placeholders
		page.replace(F("{HD}"), FPSTR(HTTP_FORM_HEAD));
		page.replace(F("{WT}"), F("31.333333") );
		//Body placeholders
		page.replace(F("{MA}"), paramsp[MQTTADDR]);
		page.replace(F("{MU}"), paramsp[MQTTUSR]);
		page.replace(F("{MP}"), paramsp[MQTTPSW]);
		page.replace(F("{MI}"), paramsp[MQTTID]);
		page.replace(F("{MO}"), paramsp[MQTTOUTTOPIC]);
		page.replace(F("{QI}"), paramsp[MQTTINTOPIC]);
		page.replace(F("{J1}"), mqttJsonp[MQTTJSONUP1]);
		page.replace(F("{J2}"), mqttJsonp[MQTTJSONDOWN1]);
		page.replace(F("{J3}"), mqttJsonp[MQTTJSONUP2]);
		page.replace(F("{J4}"), mqttJsonp[MQTTJSONDOWN2]);
		page.replace(F("{J5}"), mqttJsonp[MQTTJSONTEMP]);
		page.replace(F("{J6}"), mqttJsonp[MQTTJSONMEANPWR]);
		page.replace(F("{J7}"), mqttJsonp[MQTTJSONPEAKPWR]);
		page.replace(F("{J8}"), mqttJsonp[MQTTJSONALL]);
		//set cookies OK
		//DEBUG_PRINTLN(page);
		//DEBUG_PRINTLN(F("Scrittura cookie handleMQTTConf "));
		serverp.sendHeader("Cache-Control", "no-cache");
		serverp.sendHeader("Set-Cookie", "ESPSESSIONID=1");
		serverp.send(200, "text/html", page);
  } else {   
		DEBUG_PRINTLN(F("Login non consentito "));
		serverp.sendHeader("Location", "/");
		serverp.sendHeader("Cache-Control", "no-cache");
		serverp.send(301);
  }
}

void handleLogicConf() {  // If a POST request is made to URI /login
  //I parametri NON devono essere modificati
  bool ok=false;

  DEBUG_PRINTLN(F("Enter handleLogicConf"));
  if(is_authentified(serverp)) 
  {
#if (DEBUG)	
	DEBUG_PRINT(F("handleLogicConf: found cookie: "));
    String cookie = serverp.header("Cookie");
    DEBUG_PRINTLN(cookie);
#endif
	ok=true;
  }
    
  String page = FPSTR(HTTP_FORM_LOGIC);
  
  if(ok) { 
		//Head placeholders
		page.replace(F("{HD}"), FPSTR(HTTP_FORM_HEAD) );
		page.replace(F("{WT}"), F("31.333333") );
		//Body placeholders
		page.replace(F("{TU}"), paramsp[THALT1]);
		page.replace(F("{TD}"), paramsp[THALT2]);
		page.replace(F("{DU}"), paramsp[THALT3]);
		page.replace(F("{DD}"), paramsp[THALT4]);
		if(rollmode[0]==0){
			page.replace(F("{H3}"), "disabled");
		}else{
			page.replace(F("{H3}"), "");
		}
		if(rollmode[1]==0){
			page.replace(F("{H4}"), "disabled");
		}else{
			page.replace(F("{H4}"), "");
		}
		page.replace(F("{TL}"), paramsp[TLENGTH]);
		page.replace(F("{BR}"), paramsp[BARRELRAD]);
		page.replace(F("{TN}"), paramsp[THICKNESS]);
		page.replace(F("{SR}"), paramsp[SLATSRATIO]);
		page.replace(F("{H1}"), ((paramsp[SWROLL1]).toInt()==0)?"checked":"");
		page.replace(F("{H2}"), ((paramsp[SWROLL2]).toInt()==0)?"checked":"");
		//set cookies OK
		//DEBUG_PRINTLN(page);
		//DEBUG_PRINTLN(F("Scrittura cookie handleMQTTConf "));
		serverp.sendHeader("Cache-Control", "no-cache");
		serverp.sendHeader("Set-Cookie", "ESPSESSIONID=1");
		serverp.send(200, "text/html", page);
  } else {   
		DEBUG_PRINTLN(F("Login non consentito "));
		serverp.sendHeader("Location", "/");
		serverp.sendHeader("Cache-Control", "no-cache");
		serverp.send(301);
  }
}

//EVENT MANAGEMENT
void handleEventConf() {  // If a POST request is made to URI /login
  //I parametri NON devono essere modificati
  bool ok=false;

  DEBUG_PRINTLN(F("Enter handleEventConf"));
  if(is_authentified(serverp)) 
  {
#if (DEBUG)	
	DEBUG_PRINT(F("handleLogicConf: found cookie: "));
    String cookie = serverp.header("Cookie");
    DEBUG_PRINTLN(cookie);
#endif
	ok=true;
  }
    
  String page = FPSTR(HTTP_FORM_EVENT);
  String act = "";
  
  if(ok) { 
		//Head placeholders
		page.replace(F("{HD}"), FPSTR(HTTP_FORM_HEAD));
		page.replace(F("{SH}"), HTTP_WEBSOCKET );
		page.replace(F("{WT}"), F("48") );
		page.replace(F("{WS}"), paramsp[LOCALIP]);
		//Body placeholders
		//page.replace(F("{A1}"), paramsp[SWACTION1]);
		//page.replace(F("{A2}"), paramsp[SWACTION2]);
		//page.replace(F("{A3}"), paramsp[SWACTION3]);
		//page.replace(F("{A4}"), paramsp[SWACTION4]);
		page.replace(F("{C1}"), confcmdp[JSONCONFEVAL1]);
		page.replace(F("{C2}"), confcmdp[JSONCONFEVAL2]);
		page.replace(F("{C3}"), confcmdp[JSONCONFEVAL3]);
		page.replace(F("{C4}"), confcmdp[JSONCONFEVAL4]);
		page.replace(F("{AC}"), confcmdp[JSONACTIONEVAL]);
		page.replace(F("{D1}"), paramsp[THALT1]);
		page.replace(F("{D2}"), paramsp[THALT2]);
		page.replace(F("{D3}"), paramsp[THALT3]);
		page.replace(F("{D4}"), paramsp[THALT4]);
		page.replace(F("{S1}"), String(getCntValue(1)));
		page.replace(F("{S2}"), String(getCntValue(2)));
		page.replace(F("{S3}"), String(getCntValue(3)));
		page.replace(F("{S4}"), String(getCntValue(4)));
		/*if(rollmode[0]==1){
			page.replace(F("{H3}"), "disabled");
		}else{
			page.replace(F("{H3}"), "");
		}
		if(rollmode[1]==1){
			page.replace(F("{H4}"), "disabled");
		}else{
			page.replace(F("{H4}"), "");
		}*/
		//set cookies OK
		//DEBUG_PRINTLN(page);
		//DEBUG_PRINTLN(F("Scrittura cookie handleMQTTConf "));
		serverp.sendHeader("Cache-Control", "no-cache");
		serverp.sendHeader("Set-Cookie", "ESPSESSIONID=1");
		serverp.send(200, "text/html", page);
  } else {   
		DEBUG_PRINTLN(F("Login non consentito "));
		serverp.sendHeader("Location", "/");
		serverp.sendHeader("Cache-Control", "no-cache");
		serverp.send(301);
  }
}
//END EVENT MANAGEMENT*/

void handleCmd() {  // If a POST request is made to URI /login

	String page = FPSTR(HTTP_FORM_CMD);
	//Head placeholders
	page.replace(F("{HD}"), FPSTR(HTTP_FORM_HEAD));
	page.replace(F("{SH}"), HTTP_WEBSOCKET );
	page.replace(F("{WS}"), paramsp[LOCALIP]);
	page.replace(F("{TL}"), String(getTaplen()));
	page.replace(F("{BR}"), paramsp[BARRELRAD]);
	page.replace(F("{TN}"), paramsp[THICKNESS]);
	page.replace(F("{SR}"), paramsp[SLATSRATIO]);
	page.replace(F("{NM}"), String(getNmax()));
	page.replace(F("{PD}"), String(round(getPosdelta())));
	page.replace(F("{TP}"), mqttJsonp[MQTTJSONTEMP] );
	page.replace(F("{WT}"), F("31.333333") );
	//Body placeholders
	//DEBUG_PRINTLN(page);
	//DEBUG_PRINTLN(F("Scrittura cookie handleMQTTConf "));
	serverp.sendHeader("Cache-Control", "no-cache");
	serverp.sendHeader("Set-Cookie", "ESPSESSIONID=0");
	serverp.send(200, "text/html", page);
}
/*
void handleCmdJson(ESP8266WebServer (&serverp), String &s) {  // If a POST request is made to URI /login

//is_authentified(serverp);
if(server.hasArg("cmds")){
	String str = server.arg("cmds");
	String str2 = String("");
	
	DEBUG_PRINT(F("cmds: "));
	DEBUG_PRINTLN(str);
	
	if (str == ""){
		readStatesAndPub();
	}else{
		mqttCallback(str2, str);
	}
	
	server.sendHeader("Cache-Control", "no-cache");
	server.sendHeader("Set-Cookie", "ESPSESSIONID=0");
	server.send(200, "text/html", s);
  } 
}
*/
void handleModify(){
  
  if (!is_authentified(serverp)) {
		serverp.sendHeader("Location", "/");
		serverp.sendHeader("Cache-Control", "no-cache");
		serverp.send(301);
		return;
  }
  
  eepromBegin();
  
  if(serverp.hasArg("mqttserver") && paramsp[MQTTADDR] != serverp.arg("mqttserver") ){
	paramsp[MQTTADDR]=serverp.arg("mqttserver");
	EEPROMWriteStr(MQTTADDROFST,(paramsp[MQTTADDR]).c_str(),64);
	
	DEBUG_PRINT(F("Modified mqttAddr "));
	DEBUG_PRINTLN(paramsp[MQTTADDR]);
	//indirizzo MQTT cambiato!
	paramsp[MQTTADDRMODFIED]="true";
  }
  if(serverp.hasArg("mqttclientid") && paramsp[MQTTID] != serverp.arg("mqttclientid") ){
	paramsp[MQTTID]=serverp.arg("mqttclientid");
	EEPROMWriteStr(MQTTIDOFST,(paramsp[MQTTID]).c_str(),32);
	
	DEBUG_PRINT(F("Modified mqttID "));
	DEBUG_PRINTLN(paramsp[MQTTID]);
  }
  if(serverp.hasArg("outtopic") && (paramsp[MQTTOUTTOPIC] != serverp.arg("outtopic")) ){
	paramsp[MQTTOUTTOPIC]=serverp.arg("outtopic");
	EEPROMWriteStr(OUTTOPICOFST,(paramsp[MQTTOUTTOPIC]).c_str(),32);
	
	DEBUG_PRINT(F("Modified mqttOutTopic "));
	DEBUG_PRINTLN(paramsp[MQTTOUTTOPIC]);
	//Topic MQTT cambiato!
	paramsp[TOPICCHANGED]="true";
  }
  if(serverp.hasArg("intopic") && paramsp[MQTTINTOPIC] != serverp.arg("intopic") ){
	paramsp[MQTTINTOPIC]=serverp.arg("intopic");
	EEPROMWriteStr(INTOPICOFST,(paramsp[MQTTINTOPIC]).c_str(),32);
	
	DEBUG_PRINT(F("Modified mqttInTopic "));
	DEBUG_PRINTLN(paramsp[MQTTINTOPIC]);
	//Topic MQTT cambiato!
	paramsp[TOPICCHANGED]="true";
  }
  if(serverp.hasArg("btnup1") && paramsp[MQTTJSONUP1] != serverp.arg("btnup1") ){
	mqttJsonp[MQTTJSONUP1] = serverp.arg("btnup1");
	EEPROMWriteStr(MQTTJSONUP1OFST,(mqttJsonp[MQTTJSONUP1]).c_str(),32);
	
	DEBUG_PRINT(F("Modified mqttJsonp[1] "));
	DEBUG_PRINTLN(mqttJsonp[MQTTJSONUP1]);
  }
  if(serverp.hasArg("btndown1") && paramsp[MQTTJSONDOWN1] != serverp.arg("btndown1") ){
	mqttJsonp[MQTTJSONDOWN1] = serverp.arg("btndown1");
	EEPROMWriteStr(MQTTJSONDOWN1OFST,(mqttJsonp[MQTTJSONDOWN1]).c_str(),32);
	
	DEBUG_PRINT(F("Modified mqttJsonp[2] "));
	DEBUG_PRINTLN(mqttJsonp[MQTTJSONDOWN1]);
  }
  if(serverp.hasArg("btnup2") && paramsp[MQTTJSONUP2] != serverp.arg("btnup2") ){
	mqttJsonp[MQTTJSONUP2] = serverp.arg("btnup2");
	EEPROMWriteStr(MQTTJSONUP2OFST,(mqttJsonp[MQTTJSONUP2]).c_str(),32);
	
	DEBUG_PRINT(F("Modified mqttJsonp[3] "));
	DEBUG_PRINTLN(mqttJsonp[MQTTJSONUP2]);
  }
  if(serverp.hasArg("btndown2") && paramsp[MQTTJSONDOWN2] != serverp.arg("btndown2") ){ 
	mqttJsonp[MQTTJSONDOWN2] = serverp.arg("btndown2");
	EEPROMWriteStr(MQTTJSONDOWN2OFST,(mqttJsonp[MQTTJSONDOWN2]).c_str(),32);
	
	DEBUG_PRINT(F("Modified mqttJsonp[4] "));
	DEBUG_PRINTLN(mqttJsonp[MQTTJSONDOWN2]);
  }
  if(serverp.hasArg("btntemp") && paramsp[MQTTJSONTEMP] != serverp.arg("btntemp") ){ 
	mqttJsonp[MQTTJSONTEMP] = serverp.arg("btndown2");
	EEPROMWriteStr(MQTTJSONTEMPOFST,(mqttJsonp[MQTTJSONTEMP]).c_str(),32);
	
	DEBUG_PRINT(F("Modified mqttJsonp btntemp "));
	DEBUG_PRINTLN(mqttJsonp[MQTTJSONTEMP]);
  }
  if(serverp.hasArg("btnmeanpwr") && paramsp[MQTTJSONMEANPWR] != serverp.arg("btnmeanpwr") ){ 
	mqttJsonp[MQTTJSONMEANPWR] = serverp.arg("btnmeanpwr");
	EEPROMWriteStr(MQTTJSONMEANPWROFST,(mqttJsonp[MQTTJSONMEANPWR]).c_str(),32);
	
	DEBUG_PRINT(F("Modified mqttJsonp btnmeanpwr "));
	DEBUG_PRINTLN(mqttJsonp[MQTTJSONMEANPWR]);
  }
  if(serverp.hasArg("btnpeakpwr") && paramsp[MQTTJSONPEAKPWR] != serverp.arg("btnpeakpwr") ){ 
	mqttJsonp[MQTTJSONPEAKPWR] = serverp.arg("btnpeakpwr");
	EEPROMWriteStr(MQTTJSONPEAKPWROFST,(mqttJsonp[MQTTJSONPEAKPWR]).c_str(),32);
	
	DEBUG_PRINT(F("Modified mqttJsonp btnpeakpwr "));
	DEBUG_PRINTLN(mqttJsonp[MQTTJSONPEAKPWR]);
  }
  
  if(serverp.hasArg("btnall") && paramsp[MQTTJSONALL] != serverp.arg("btnall") ){ 
	mqttJsonp[MQTTJSONALL] = serverp.arg("btndown2");
	EEPROMWriteStr(MQTTJSONALLOFST,(mqttJsonp[MQTTJSONALL]).c_str(),32);
	
	DEBUG_PRINT(F("Modified mqttJsonp btnall "));
	DEBUG_PRINTLN(mqttJsonp[MQTTJSONALL]);
  }
  if(serverp.hasArg("wificlientssid1") && paramsp[CLNTSSID1] != serverp.arg("wificlientssid1") ){
	paramsp[CLNTSSID1]=serverp.arg("wificlientssid1");
	EEPROMWriteStr(WIFICLIENTSSIDOFST1,(paramsp[CLNTSSID1]).c_str(),32);
	
	DEBUG_PRINT(F("Modified clntSsid1 "));
	DEBUG_PRINTLN(paramsp[CLNTSSID1]);
	paramsp[WIFICHANGED]="true";
  }
  if(serverp.hasArg("wificlientpsw1") && paramsp[CLNTPSW1] != serverp.arg("wificlientpsw1") ){
	paramsp[CLNTPSW1]=serverp.arg("wificlientpsw1");
	EEPROMWriteStr(WIFICLIENTPSWOFST1,(paramsp[CLNTPSW1]).c_str(),32);
	
	DEBUG_PRINT(F("Modified clntPsw1 "));
	DEBUG_PRINTLN(paramsp[CLNTPSW1]);
	paramsp[WIFICHANGED]="true";
  }
  if(serverp.hasArg("wificlientssid2") && paramsp[CLNTSSID2] != serverp.arg("wificlientssid2") ){
	paramsp[CLNTSSID2]=serverp.arg("wificlientssid2");
	EEPROMWriteStr(WIFICLIENTSSIDOFST2,(paramsp[CLNTSSID2]).c_str(),32);
	
	DEBUG_PRINT(F("Modified clntSsid2 "));
	DEBUG_PRINTLN(paramsp[CLNTSSID2]);
	paramsp[WIFICHANGED]="true";
  }
  if(serverp.hasArg("wificlientpsw2") && paramsp[CLNTPSW2] != serverp.arg("wificlientpsw2") ){
	paramsp[CLNTPSW2]=serverp.arg("wificlientpsw2");
	EEPROMWriteStr(WIFICLIENTPSWOFST2,(paramsp[CLNTPSW2]).c_str(),32);
	
	DEBUG_PRINT(F("Modified clntPsw2 "));
	DEBUG_PRINTLN(paramsp[CLNTPSW2]);
	paramsp[WIFICHANGED]="true";
  }
  if(serverp.hasArg("wifiapssid") && paramsp[APPSSID] != serverp.arg("wifiapssid") ){
	paramsp[APPSSID]=serverp.arg("wifiapssid");
	EEPROMWriteStr(WIFIAPSSIDOFST,(paramsp[APPSSID]).c_str(),32);
	
	DEBUG_PRINT(F("Modified APSsid "));
	DEBUG_PRINTLN(paramsp[APPSSID]);
	paramsp[WIFICHANGED]="true";
  }
  if(serverp.hasArg("wifiAPPPSW") && paramsp[APPPSW] != serverp.arg("wifiAPPPSW") ){
	paramsp[APPPSW]=serverp.arg("wifiAPPPSW");
	EEPROMWriteStr(WIFIAPPPSWOFST,(paramsp[APPPSW]).c_str(),32);
	
	DEBUG_PRINT(F("Modified APPPSW "));
	DEBUG_PRINTLN(paramsp[APPPSW]);
	paramsp[WIFICHANGED]="true";
  }
  if(serverp.hasArg("username") && paramsp[WEBUSR] != serverp.arg("username") ){
	paramsp[WEBUSR]=serverp.arg("username");
	EEPROMWriteStr(WEBUSROFST,(paramsp[WEBUSR]).c_str(),32);
	
	DEBUG_PRINT(F("Modified webUsr "));
	DEBUG_PRINTLN(paramsp[WEBUSR]);
  }
  if(serverp.hasArg("username") && paramsp[WEBPSW] != serverp.arg("username") ){
	paramsp[WEBPSW]=serverp.arg("password");
	EEPROMWriteStr(WEBPSWOFST,(paramsp[WEBPSW]).c_str(),32);
	
	DEBUG_PRINT(F("Modified webPsw "));
	DEBUG_PRINTLN(paramsp[WEBPSW]);
  } 
  if(serverp.hasArg("mqttusr") && paramsp[MQTTUSR] != serverp.arg("mqttusr") ){
	paramsp[MQTTUSR]=serverp.arg("mqttusr");
	EEPROMWriteStr(MQTTUSROFST,(paramsp[MQTTUSR]).c_str(),32);
	
	DEBUG_PRINT(F("Modified MQTT username "));
	DEBUG_PRINTLN(paramsp[MQTTUSR]);
	paramsp[MQTTCONNCHANGED]="true";
  } 
  if(serverp.hasArg("mqttpsw") && paramsp[MQTTPSW] != serverp.arg("mqttpsw") ){
	paramsp[MQTTPSW]=serverp.arg("mqttpsw");
	EEPROMWriteStr(MQTTPSWOFST,(paramsp[MQTTPSW]).c_str(),32);
	
	DEBUG_PRINT(F("Modified MQTT password "));
	DEBUG_PRINTLN(paramsp[MQTTPSW]);
	paramsp[MQTTCONNCHANGED]="true";
  } 
#if (!AUTOCAL) 
  if(serverp.hasArg("startdelay1") && paramsp[STDEL1] != serverp.arg("startdelay1") ){
	paramsp[STDEL1]=serverp.arg("startdelay1");
	EEPROMWriteFloat(STDEL1OFST,(paramsp[STDEL1]).toFloat());
	
	DEBUG_PRINT(F("Motor start delay 1"));
	DEBUG_PRINTLN(paramsp[STDEL1]);
	paramsp[TIMINGCHANGED]="true";
  } 
  if(serverp.hasArg("startdelay2") && paramsp[STDEL2] != serverp.arg("startdelay2") ){
	paramsp[STDEL2]=serverp.arg("startdelay2");
	EEPROMWriteFloat(STDEL2OFST,(paramsp[STDEL2]).toFloat());
	
	DEBUG_PRINT(F("Motor start delay 2"));
	DEBUG_PRINTLN(paramsp[STDEL2]);
	paramsp[TIMINGCHANGED]="true";
  } 
#endif
  if(serverp.hasArg("thalt1") && (paramsp[THALT1] != String(serverp.arg("thalt1"))) ){
	paramsp[THALT1] = serverp.arg("thalt1");
	EEPROMWriteInt(THALT1OFST,(paramsp[THALT1]).toInt());
	
	DEBUG_PRINT(F("Modified THALT1 "));
	DEBUG_PRINTLN(paramsp[THALT1]);
	paramsp[TIMINGCHANGED]="true";
  }
  if(serverp.hasArg("thalt2") && (paramsp[THALT2] != String(serverp.arg("thalt2"))) ){
	paramsp[THALT2] = serverp.arg("thalt2");
	EEPROMWriteInt(THALT2OFST,(paramsp[THALT2]).toInt());
	
    DEBUG_PRINT(F("Modified THALT2 "));
    DEBUG_PRINTLN(paramsp[THALT2]);
    paramsp[TIMINGCHANGED]="true";
  }
  if(serverp.hasArg("thalt3") && (paramsp[THALT3] != String(serverp.arg("thalt3"))) ){
	paramsp[THALT3] = serverp.arg("thalt3");
	EEPROMWriteInt(THALT3OFST,(paramsp[THALT3]).toInt());
	
	DEBUG_PRINT(F("Modified THALT3 "));
	DEBUG_PRINTLN(paramsp[THALT3]);
	paramsp[TIMINGCHANGED]="true";
  }
  if(serverp.hasArg("thalt4") && (paramsp[THALT4] != String(serverp.arg("thalt4"))) ){
	paramsp[THALT4] = serverp.arg("thalt4");
	EEPROMWriteInt(THALT4OFST,(paramsp[THALT4]).toInt());
	
    DEBUG_PRINT(F("Modified THALT4 "));
    DEBUG_PRINTLN(paramsp[THALT4]);
    paramsp[TIMINGCHANGED]="true";
  }
  if(serverp.hasArg("taplength") && (paramsp[TLENGTH] != String(serverp.arg("taplength"))) ){
	paramsp[TLENGTH] = serverp.arg("taplength");
	EEPROMWriteFloat(TLENGTHOFST,(paramsp[TLENGTH]).toFloat());
	
	DEBUG_PRINT(F("Modified TLENGTH "));
	DEBUG_PRINTLN(paramsp[TLENGTH]);
	paramsp[TIMINGCHANGED]="true";
  }
  if(serverp.hasArg("barrelrad") && (paramsp[BARRELRAD] != String(serverp.arg("barrelrad"))) ){
	paramsp[BARRELRAD] = serverp.arg("barrelrad");
	EEPROMWriteFloat(BARRELRADOFST,(paramsp[BARRELRAD]).toFloat());
	
    DEBUG_PRINT(F("Modified BARRELRAD "));
    DEBUG_PRINTLN(paramsp[BARRELRAD]);
    paramsp[TIMINGCHANGED]="true";
  }
  if(serverp.hasArg("thickness") && (paramsp[THICKNESS] != String(serverp.arg("thickness"))) ){
	paramsp[THICKNESS] = serverp.arg("thickness");
	EEPROMWriteFloat(THICKNESSOFST,(paramsp[THICKNESS]).toFloat());
	
    DEBUG_PRINT(F("Modified THICKNESS "));
    DEBUG_PRINTLN(paramsp[THICKNESS]);
    paramsp[TIMINGCHANGED]="true";
  }
   if(serverp.hasArg("slatsratio") && (paramsp[SLATSRATIO] != String(serverp.arg("slatsratio"))) ){
	paramsp[SLATSRATIO] = serverp.arg("slatsratio");
	EEPROMWriteFloat(SLATSRATIOFST,(paramsp[SLATSRATIO]).toFloat());
	
    DEBUG_PRINT(F("Modified SLATSRATIO "));
    DEBUG_PRINTLN(paramsp[SLATSRATIO]);
    paramsp[TIMINGCHANGED]="true";
  }
  EEPROM.end();
  
  //nuovo begin da quì!
  //writeOnOffCondition(char* exp, byte sw);
  //writeOnOffAction(byte id, byte sw);
   
  DEBUG_PRINTLN(F("Disconnection"));
  
  serverp.sendHeader("Cache-Control", "no-cache");
  serverp.sendHeader("Set-Cookie", "ESPSESSIONID=1");
  
  String page = FPSTR(HTTP_FORM_SUCCESS);
  page.replace(F("{HD}"), FPSTR(HTTP_FORM_HEAD) );
  serverp.send(200, "text/html", page);
		
  if(paramsp[TIMINGCHANGED]=="true"){
	paramsp[TIMINGCHANGED]=="false";
	initIiming(false);
  }
  
  if( serverp.hasArg("rebootd") && String("y") == serverp.arg("rebootd") ){
	rebootSystem();
  }
  
  if( serverp.hasArg("reboot") && String("y") == serverp.arg("reboot") ){
	DEBUG_PRINTLN(F("Rebooting ESP"));
	ESP.restart();
  }
  
  if( serverp.hasArg("swmode1") && String("0") == serverp.arg("swmode1") ){
	setSWMode(0,0);
	writeSWMode(0,0);	
  }else{
	setSWMode(1,0);
	writeSWMode(1,0);  
  }
  
  if( serverp.hasArg("swmode2") && String("0") == serverp.arg("swmode2") ){
	setSWMode(0,1);
	writeSWMode(0,1);	
  }else{
	setSWMode(1,1);
	writeSWMode(1,1);
  }
  
}

void loadConfig() {
	char buf[250]="";
	int i;
    EEPROM.begin(THALT1OFST);
	delay(10);
	//DEBUG_PRINTln();
	DEBUG_PRINTLN();
	DEBUG_PRINT(F("Startup string: "));
	bool cfg = validateEEPROM();
	EEPROMReadStr(0,buf,2);
	varStrOfst[0] = FIXEDPARAMSLEN;  		 		//l'offset del primo variabile parte dalla fine dell'ultimo fisso
	varStrOfst[5] = EEPROMReadInt(EEPROMLENOFST);//la fine dell'ultimo variabile (offset+lunghezza campo) è conservato nella posizione EEPROMLENOFST 
	EEPROM.end();
	DEBUG_PRINTLN(buf);
	delay(1000);
	if(!cfg) {
		DEBUG_PRINTLN(F("EEPROM read error! EEPROM will be initialized, you must load a new valid system configuration."));
		EEPROM.begin(FIXEDPARAMSLEN);
		initEEPROM(FIXEDPARAMSLEN); 
		EEPROM.end();
		varStrOfst[0] = varStrOfst[1] = varStrOfst[2] = varStrOfst[3] = varStrOfst[4] = FIXEDPARAMSLEN;
		varStrOfst[5] = FIXEDPARAMSLEN;
		saveOnEEPROM(FIXEDPARAMSLEN);
		paramsp[CONFLOADED]="false";
		delay(1000);
	} else {
		EEPROM.begin(THALT1OFST);//the next after EEPROMLENOFST
		int eepromlen = EEPROMReadInt(EEPROMLENOFST);
		EEPROM.end();
		EEPROM.begin(eepromlen+1);
		DEBUG_PRINTLN(F("Reading EEPROM configuration of len "));
		DEBUG_PRINTLN(eepromlen);
		
		paramsp[THALT1]	= EEPROMReadInt(THALT1OFST);
		DEBUG_PRINT(F("THALT1: "));
		DEBUG_PRINTLN(paramsp[THALT1]);
		
		paramsp[THALT2] = EEPROMReadInt(THALT2OFST);
		DEBUG_PRINT(F("THALT2: "));
		DEBUG_PRINTLN(paramsp[THALT2]);
		
		paramsp[THALT3] = EEPROMReadInt(THALT3OFST);
		DEBUG_PRINT(F("THALT3: "));
		DEBUG_PRINTLN(paramsp[THALT3]);
		
		paramsp[THALT4]	= EEPROMReadInt(THALT4OFST);
		DEBUG_PRINT(F("THALT4: "));
		DEBUG_PRINTLN(paramsp[THALT4]);
		
		EEPROMReadStr(MQTTADDROFST,buf);
		paramsp[MQTTADDR] = buf;
		DEBUG_PRINT(F("mqttAddr: "));
		DEBUG_PRINTLN(paramsp[MQTTADDR]);
		
		EEPROMReadStr(MQTTIDOFST,buf);
		paramsp[MQTTID] = buf;
		DEBUG_PRINT(F("mqttID: "));
		DEBUG_PRINTLN(paramsp[MQTTID]);
		
		EEPROMReadStr(OUTTOPICOFST,buf);
		paramsp[MQTTOUTTOPIC] = buf;
		DEBUG_PRINT(F("mqttOutTopic: "));
		DEBUG_PRINTLN(paramsp[MQTTOUTTOPIC]);
		
		EEPROMReadStr(INTOPICOFST,buf);
		paramsp[MQTTINTOPIC] = buf;
		DEBUG_PRINT(F("mqttInTopic: "));
		DEBUG_PRINTLN(paramsp[MQTTINTOPIC]);
		
		EEPROMReadStr(MQTTJSONUP1OFST,buf);
		mqttJsonp[MQTTJSONUP1] = buf;
		DEBUG_PRINT(F("MQTTJSONUP1: "));
		DEBUG_PRINTLN(mqttJsonp[MQTTJSONUP1]);
		
		EEPROMReadStr(MQTTJSONDOWN1OFST,buf);
		mqttJsonp[MQTTJSONDOWN1] = buf;
		DEBUG_PRINT(F("MQTTJSONDOWN1: "));
		DEBUG_PRINTLN(mqttJsonp[MQTTJSONDOWN1]);
		
		EEPROMReadStr(MQTTJSONUP2OFST,buf);
		mqttJsonp[MQTTJSONUP2] = buf;
		DEBUG_PRINT(F("MQTTJSONUP2: "));
		DEBUG_PRINTLN(mqttJsonp[MQTTJSONUP2]);
		
		EEPROMReadStr(MQTTJSONDOWN2OFST,buf);
		mqttJsonp[MQTTJSONDOWN2] = buf;
		DEBUG_PRINT(F("MQTTJSONDOWN2: "));
		DEBUG_PRINTLN(mqttJsonp[MQTTJSONDOWN2]);
		
		EEPROMReadStr(MQTTJSONTEMPOFST,buf);
		mqttJsonp[MQTTJSONTEMP] = buf;
		DEBUG_PRINT(F("MQTTJSONTEMP: "));
		DEBUG_PRINTLN(mqttJsonp[MQTTJSONTEMP]);
		
		EEPROMReadStr(MQTTJSONMEANPWROFST,buf);
		mqttJsonp[MQTTJSONMEANPWR] = buf;
		DEBUG_PRINT(F("MQTTJSONMEANPWR: "));
		DEBUG_PRINTLN(mqttJsonp[MQTTJSONMEANPWR]);
		
		EEPROMReadStr(MQTTJSONPEAKPWROFST,buf);
		mqttJsonp[MQTTJSONPEAKPWR] = buf;
		DEBUG_PRINT(F("MQTTJSONPEAKPWR: "));
		DEBUG_PRINTLN(mqttJsonp[MQTTJSONPEAKPWR]);
		
		EEPROMReadStr(MQTTJSONALLOFST,buf);
		mqttJsonp[MQTTJSONALL] = buf;
		DEBUG_PRINT(F("MQTTJSONALL: "));
		DEBUG_PRINTLN(mqttJsonp[MQTTJSONALL]);
		
		EEPROMReadStr(WIFICLIENTSSIDOFST1,buf);
		paramsp[CLNTSSID1] = buf;
		DEBUG_PRINT(F("clntSsid1: "));
		DEBUG_PRINTLN(paramsp[CLNTSSID1]);
		
		EEPROMReadStr(WIFICLIENTPSWOFST1,buf);
		paramsp[CLNTPSW1] = buf;
		DEBUG_PRINT(F("clntPsw1: "));
		DEBUG_PRINTLN(paramsp[CLNTPSW1]);
		
		EEPROMReadStr(WIFICLIENTSSIDOFST2,buf);
		paramsp[CLNTSSID2] = buf;
		DEBUG_PRINT(F("clntSsid2: "));
		DEBUG_PRINTLN(paramsp[CLNTSSID2]);
		
		EEPROMReadStr(WIFICLIENTPSWOFST2,buf);
		paramsp[CLNTPSW2] = buf;
		DEBUG_PRINT(F("clntPsw2: "));
		DEBUG_PRINTLN(paramsp[CLNTPSW2]);
		
		EEPROMReadStr(WIFIAPSSIDOFST,buf);
		paramsp[APPSSID] = buf;
		DEBUG_PRINT(F("APSsid: "));
		DEBUG_PRINTLN(paramsp[APPSSID]);
		
		EEPROMReadStr(WIFIAPPPSWOFST,buf);
		paramsp[APPPSW] = buf;
		DEBUG_PRINT(F("APPPSW: "));
		DEBUG_PRINTLN(paramsp[APPPSW]);
		
		EEPROMReadStr(WEBUSROFST,buf);
		paramsp[WEBUSR] = buf;
		DEBUG_PRINT(F("webUsr: "));
		DEBUG_PRINTLN(paramsp[WEBUSR]);
		
		EEPROMReadStr(WEBPSWOFST,buf);
		paramsp[WEBPSW] = buf;
		DEBUG_PRINT(F("webPsw: "));
		DEBUG_PRINTLN(paramsp[WEBPSW]);
		
		EEPROMReadStr(MQTTUSROFST,buf);
		paramsp[MQTTUSR] = buf;
		DEBUG_PRINT(F("mqtt user name: "));
		DEBUG_PRINTLN(paramsp[MQTTUSR]);
		
		EEPROMReadStr(MQTTPSWOFST,buf);
		paramsp[MQTTPSW] = buf;
		DEBUG_PRINTLN(F("mqtt user password: "));
		DEBUG_PRINTLN(paramsp[MQTTPSW]);
#if (!AUTOCAL)		
		paramsp[STDEL1] = EEPROMReadFloat(STDEL1OFST);
		DEBUG_PRINTLN(F("motor start delay1: "));
		DEBUG_PRINTLN(paramsp[STDEL1]);
		
		paramsp[STDEL2] = EEPROMReadFloat(STDEL2OFST);
		DEBUG_PRINTLN(F("motor start delay2: "));
		DEBUG_PRINTLN(paramsp[STDEL2]);
#endif		
		paramsp[VALWEIGHT] = EEPROMReadFloat(VALWEIGHTOFST);
		DEBUG_PRINTLN(F("sensor weight: "));
		DEBUG_PRINTLN(paramsp[VALWEIGHT]);
		
		paramsp[TLENGTH] = EEPROMReadFloat(TLENGTHOFST);
		DEBUG_PRINTLN(F("barrel LENGTH: "));
		DEBUG_PRINTLN(paramsp[TLENGTH]);
		
		paramsp[BARRELRAD] = EEPROMReadFloat(BARRELRADOFST);
		DEBUG_PRINTLN(F("barrel radius: "));
		DEBUG_PRINTLN(paramsp[BARRELRAD]);
		
		paramsp[THICKNESS] = EEPROMReadFloat(THICKNESSOFST);
		DEBUG_PRINTLN(F("barrel THICKNESS: "));
		DEBUG_PRINTLN(paramsp[THICKNESS]);
		
		paramsp[SLATSRATIO] = EEPROMReadFloat(SLATSRATIOFST);
		DEBUG_PRINTLN(F("barrel SLATSRATIO: "));
		DEBUG_PRINTLN(paramsp[SLATSRATIO]);
		
		paramsp[SWROLL1] = EEPROM.read(SWROLL1OFST);
		DEBUG_PRINTLN(F("rollmode 1 SWROLL1: "));
		DEBUG_PRINTLN(paramsp[SWROLL1]);
		rollmode[0] = (paramsp[SWROLL1]).toInt();
		setSWMode(rollmode[0],0);
		
		paramsp[SWROLL2] = EEPROM.read(SWROLL2OFST);
		DEBUG_PRINTLN(F("rollmode 2 SWROLL2: "));
		DEBUG_PRINTLN(paramsp[SWROLL2]);
		rollmode[1] = (paramsp[SWROLL2]).toInt();
		setSWMode(rollmode[1],1);
		/*
		for(i=0; i<4; ++i){
			//if(rollmode[i/2]==false){
				paramsp[SWACTION1+i] = (int) EEPROM.read(SWACTION1OFST+i);
				DEBUG_PRINT(F("switches SWACTION"));
				DEBUG_PRINT(i+1);
				DEBUG_PRINT(F(": "));
				DEBUG_PRINTLN(paramsp[SWACTION1+i]);
			//}
		}
		*/
		//EEPROMReadStr read until /0 occur
		varStrOfst[0] = FIXEDPARAMSLEN;
		for(i=0; i<5; ++i){
				varStrOfst[i+1] = EEPROMReadStr(varStrOfst[i], buf) + varStrOfst[i];
				confcmdp[i]=buf;
				DEBUG_PRINT(F("sensors CONFEXPR "));
				DEBUG_PRINT(i);
				DEBUG_PRINT(F(": "));
				DEBUG_PRINTLN(confcmdp[i]);
		}
			
		paramsp[CONFLOADED]="true";
		EEPROM.end();
		DEBUG_PRINTLN(F("EEPROM configuration readed"));
	}
}

//Check if header is present and correct
bool is_authentified(ESP8266WebServer &server){
#if (DEBUG)	 
  DEBUG_PRINTLN(F("Enter is_authentified"));  
  String message = "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  DEBUG_PRINTLN(message);
  
  message = "HEADERS: ";
  message += server.headers();
  message += "\n";
  for (uint8_t i = 0; i < server.headers(); i++) {
    message += " " + server.headerName(i) + ": " + server.header(i) + "\n";
  }
  DEBUG_PRINTLN(message); 
#endif
  
  if (server.hasHeader("Cookie")) {
    DEBUG_PRINTLN(F("Found cookie: "));
    DEBUG_PRINTLN(server.header("Cookie"));
    if ((server.header("Cookie")).indexOf("ESPSESSIONID=1") != -1) {
      DEBUG_PRINTLN(F("Authentification Successful"));
      return true;
    }
  }
  DEBUG_PRINTLN(F("Authentification Failed"));
  return false;
}

void saveOnEEPROM(int len){
	int i;
	
	//I parametri devono poter essere modificati
	DEBUG_PRINTLN(F("Saving configuration...."));
  
	EEPROM.begin(len);
	
	EEPROM.write(SWROLL1OFST, (char) (paramsp[SWROLL1]).toInt());
	
	DEBUG_PRINT(F("Modified rollmode 1 SWROLL1 "));
	DEBUG_PRINTLN(paramsp[SWROLL1]);
	
	EEPROM.write(SWROLL2OFST, (char) (paramsp[SWROLL2]).toInt());
	
	DEBUG_PRINT(F("Modified rollmode 2 SWROLL2 "));
	DEBUG_PRINTLN(paramsp[SWROLL2]);
		
	EEPROMWriteInt(THALT1OFST,(paramsp[THALT1]).toInt());
	
	DEBUG_PRINT(F("Modified THALT1 "));
	DEBUG_PRINTLN(paramsp[THALT1]);

	EEPROMWriteInt(THALT2OFST,(paramsp[THALT2]).toInt());
	
	DEBUG_PRINT(F("Modified THALT2 "));
	DEBUG_PRINTLN(paramsp[THALT2]);
	
	EEPROMWriteInt(THALT3OFST,(paramsp[THALT3]).toInt());
	
	DEBUG_PRINT(F("Modified THALT3 "));
	DEBUG_PRINTLN(paramsp[THALT3]);

	EEPROMWriteInt(THALT4OFST,(paramsp[THALT4]).toInt());
	
	DEBUG_PRINT(F("Modified THALT4 "));
	DEBUG_PRINTLN(paramsp[THALT4]);
	/*
	for(i=0; i<4; ++i){
		//if(rollmode[i/2]==false){
			EEPROM.write(SWACTION1OFST+i, (char) (paramsp[SWACTION1+i]).toInt());
			DEBUG_PRINT(F("Modified action SWACTION"));
			DEBUG_PRINT(i+1);
			DEBUG_PRINT(F(": "));
			DEBUG_PRINTLN(paramsp[SWACTION1+i]);
		//}
	}
	*/
	EEPROMWriteStr(MQTTADDROFST,(paramsp[MQTTADDR]).c_str(),64);
	
	DEBUG_PRINT(F("Modified mqttAddr "));
	DEBUG_PRINTLN(paramsp[MQTTADDR]);
	//indirizzo MQTT cambiato!
	//paramsp[MQTTADDRMODFIED]="true"

	EEPROMWriteStr(MQTTIDOFST,(paramsp[MQTTID]).c_str(),32);
	
	DEBUG_PRINT(F("Modified mqttID "));
	DEBUG_PRINTLN(paramsp[MQTTID]);

	EEPROMWriteStr(OUTTOPICOFST,(paramsp[MQTTOUTTOPIC]).c_str(),32);
	
	DEBUG_PRINT(F("Modified mqttOutTopic "));
	DEBUG_PRINTLN(paramsp[MQTTOUTTOPIC]);
	//Topic MQTT cambiato!
	//paramsp[TOPICCHANGED]="true"

	EEPROMWriteStr(INTOPICOFST,(paramsp[MQTTINTOPIC]).c_str(),32);
	
	DEBUG_PRINT(F("Modified mqttInTopic "));
	DEBUG_PRINTLN(paramsp[MQTTINTOPIC]);
	//Topic MQTT cambiato!
	//paramsp[TOPICCHANGED]="true"

	EEPROMWriteStr(MQTTJSONUP1OFST,(mqttJsonp[MQTTJSONUP1]).c_str(),32);
	
	DEBUG_PRINT(F("Modified MQTTJSONUP1 "));
	DEBUG_PRINTLN(mqttJsonp[MQTTJSONUP1]);
	
	EEPROMWriteStr(MQTTJSONDOWN1OFST,(mqttJsonp[MQTTJSONDOWN1]).c_str(),32);
	
	DEBUG_PRINT(F("Modified MQTTJSONDOWN1 "));
	DEBUG_PRINTLN(mqttJsonp[MQTTJSONDOWN1]);

	EEPROMWriteStr(MQTTJSONUP2OFST,(mqttJsonp[MQTTJSONUP2]).c_str(),32);
	
	DEBUG_PRINT(F("Modified MQTTJSONUP2 "));
	DEBUG_PRINTLN(mqttJsonp[MQTTJSONUP2]);
	
	EEPROMWriteStr(MQTTJSONDOWN2OFST,(mqttJsonp[MQTTJSONDOWN2]).c_str(),32);
	
	DEBUG_PRINT(F("Modified MQTTJSONDOWN2 "));
	DEBUG_PRINTLN(mqttJsonp[MQTTJSONDOWN2]);
	
	EEPROMWriteStr(MQTTJSONTEMPOFST,(mqttJsonp[MQTTJSONTEMP]).c_str(),32);
	
	DEBUG_PRINT(F("Modified MQTTJSONTEMP "));
	DEBUG_PRINTLN(mqttJsonp[MQTTJSONTEMP]);
	
	EEPROMWriteStr(MQTTJSONMEANPWROFST,(mqttJsonp[MQTTJSONMEANPWR]).c_str(),32);
	
	DEBUG_PRINT(F("Modified MQTTJSONMEANPWR "));
	DEBUG_PRINTLN(mqttJsonp[MQTTJSONMEANPWR]);
	
	EEPROMWriteStr(MQTTJSONPEAKPWROFST,(mqttJsonp[MQTTJSONPEAKPWR]).c_str(),32);
	
	DEBUG_PRINT(F("Modified MQTTJSONPEAKPWR "));
	DEBUG_PRINTLN(mqttJsonp[MQTTJSONPEAKPWR]);
	
	EEPROMWriteStr(MQTTJSONALLOFST,(mqttJsonp[MQTTJSONALL]).c_str(),32);
	
	DEBUG_PRINT(F("Modified MQTTJSONALL "));
	DEBUG_PRINTLN(mqttJsonp[MQTTJSONALL]);
	
	EEPROMWriteStr(WIFICLIENTSSIDOFST1,(paramsp[CLNTSSID1]).c_str(),32);
	
	DEBUG_PRINT(F("Modified clntSsid1 "));
	DEBUG_PRINTLN(paramsp[CLNTSSID1]);
	//paramsp[WIFICHANGED]="true"

	EEPROMWriteStr(WIFICLIENTPSWOFST1,(paramsp[CLNTPSW1]).c_str(),32);
	
	DEBUG_PRINT(F("Modified clntPsw1 "));
	DEBUG_PRINTLN(paramsp[CLNTPSW1]);
	//paramsp[WIFICHANGED]="true"
	
	EEPROMWriteStr(WIFICLIENTSSIDOFST2,(paramsp[CLNTSSID2]).c_str(),32);
	
	DEBUG_PRINT(F("Modified clntSsid2 "));
	DEBUG_PRINTLN(paramsp[CLNTSSID2]);
	//paramsp[WIFICHANGED]="true"

	EEPROMWriteStr(WIFICLIENTPSWOFST2,(paramsp[CLNTPSW2]).c_str(),32);
	
	DEBUG_PRINT(F("Modified clntPsw2 "));
	DEBUG_PRINTLN(paramsp[CLNTPSW2]);
	//paramsp[WIFICHANGED]="true"

	EEPROMWriteStr(WIFIAPSSIDOFST,(paramsp[APPSSID]).c_str(),32);
	
	DEBUG_PRINT(F("Modified APSsid "));
	DEBUG_PRINTLN(paramsp[APPSSID]);
	//paramsp[WIFICHANGED]="true"

	EEPROMWriteStr(WIFIAPPPSWOFST,(paramsp[APPPSW]).c_str(),32);
	
	DEBUG_PRINT(F("Modified APPPSW "));
	DEBUG_PRINTLN(paramsp[APPPSW]);
	//paramsp[WIFICHANGED]="true"

	EEPROMWriteStr(WEBUSROFST,(paramsp[WEBUSR]).c_str(),32);
	
	DEBUG_PRINT(F("Modified webUsr "));
	DEBUG_PRINTLN(paramsp[WEBUSR]);
	
	EEPROMWriteStr(WEBPSWOFST,(paramsp[WEBPSW]).c_str(),32);
	
	DEBUG_PRINT(F("Modified webPsw "));
	DEBUG_PRINTLN(paramsp[WEBPSW]);
	
	EEPROMWriteStr(MQTTUSROFST,(paramsp[MQTTUSR]).c_str(),32);
	
	DEBUG_PRINT(F("Modified MQTT username "));
	DEBUG_PRINTLN(paramsp[MQTTUSR]);
	//paramsp[MQTTCONNCHANGED]="true"

	EEPROMWriteStr(MQTTPSWOFST,(paramsp[MQTTPSW]).c_str(),32);
	
	DEBUG_PRINT(F("Modified MQTT password "));
	DEBUG_PRINTLN(paramsp[MQTTPSW]);
	//paramsp[MQTTCONNCHANGED]="true"
#if (!AUTOCAL) 	
	EEPROMWriteFloat(STDEL1OFST,(paramsp[STDEL1]).toFloat());
	
	DEBUG_PRINT(F("Modified start delay 1 "));
	DEBUG_PRINTLN(paramsp[STDEL1]);
	
	EEPROMWriteFloat(STDEL2OFST,(paramsp[STDEL2]).toFloat());
	
	DEBUG_PRINT(F("Modified start delay 2 "));
	DEBUG_PRINTLN(paramsp[STDEL2]);
#endif	
	EEPROMWriteFloat(VALWEIGHTOFST,(paramsp[VALWEIGHT]).toFloat());
	
	DEBUG_PRINT(F("Modified sensor weight "));
	DEBUG_PRINTLN(paramsp[VALWEIGHT]);
	
	EEPROMWriteFloat(TLENGTHOFST,(paramsp[TLENGTH]).toFloat());
	
	DEBUG_PRINT(F("Modified barrel length "));
	DEBUG_PRINTLN(paramsp[TLENGTH]);
	
	EEPROMWriteFloat(BARRELRADOFST,(paramsp[BARRELRAD]).toFloat());
	
	DEBUG_PRINT(F("Modified barrel radius "));
	DEBUG_PRINTLN(paramsp[BARRELRAD]);
	
	EEPROMWriteFloat(THICKNESSOFST,(paramsp[THICKNESS]).toFloat());
	
	DEBUG_PRINT(F("Modified barrel THICKNESS "));
	DEBUG_PRINTLN(paramsp[THICKNESS]);
	
	EEPROMWriteFloat(SLATSRATIOFST,(paramsp[SLATSRATIO]).toFloat());
	
	DEBUG_PRINT(F("Modified barrel SLATSRATIO "));
	DEBUG_PRINTLN(paramsp[SLATSRATIO]);
	
    EEPROM.end();
	
	writeOnOffConditions();
}

void printConfig(){
		int i;
		
		DEBUG_PRINT(F("\nPrinting EEPROM configuration...."));
		
		DEBUG_PRINT(F("\nTHALT1: "));
		DEBUG_PRINT(paramsp[THALT1]);
		
		DEBUG_PRINT(F("\nTHALT2: "));
		DEBUG_PRINT(paramsp[THALT2]);
		
		DEBUG_PRINT(F("\nTHALT3: "));
		DEBUG_PRINT(paramsp[THALT3]);
		
		DEBUG_PRINT(F("\nTHALT4: "));
		DEBUG_PRINT(paramsp[THALT4]);
		
		DEBUG_PRINT(F("\nmqttAddr: "));
		DEBUG_PRINT(paramsp[MQTTADDR]);
		
		DEBUG_PRINT(F("\nmqttID: "));
		DEBUG_PRINT(paramsp[MQTTID]);
		
		DEBUG_PRINT(F("\nmqttOutTopic: "));
		DEBUG_PRINT(paramsp[MQTTOUTTOPIC]);
		
		DEBUG_PRINT(F("\nmqttInTopic: "));
		DEBUG_PRINT(paramsp[MQTTINTOPIC]);
		
		DEBUG_PRINT(F("\nMQTTJSONUP1: "));
		DEBUG_PRINT(mqttJsonp[MQTTJSONUP1]);
		
		DEBUG_PRINT(F("\nMQTTJSONDOWN1: "));
		DEBUG_PRINT(mqttJsonp[MQTTJSONDOWN1]);
		
		DEBUG_PRINT(F("\nMQTTJSONUP2: "));
		DEBUG_PRINT(mqttJsonp[MQTTJSONUP2]);
		
		DEBUG_PRINT(F("\nMQTTJSONDOWN2: "));
		DEBUG_PRINT(mqttJsonp[MQTTJSONDOWN2]);
		
		DEBUG_PRINT(F("\nMQTTJSONTEMP: "));
		DEBUG_PRINT(mqttJsonp[MQTTJSONTEMP]);
		
		DEBUG_PRINT(F("\nMQTTJSONMEANPWR: "));
		DEBUG_PRINT(mqttJsonp[MQTTJSONMEANPWR]);
		
		DEBUG_PRINT(F("\nMQTTJSONPEAKPWR: "));
		DEBUG_PRINT(mqttJsonp[MQTTJSONPEAKPWR]);
		
		DEBUG_PRINT(F("\nMQTTJSONALL: "));
		DEBUG_PRINT(mqttJsonp[MQTTJSONALL]);
		
		DEBUG_PRINT(F("\nclntSsid1: "));
		DEBUG_PRINT(paramsp[CLNTSSID1]);
		
		DEBUG_PRINT(F("\nclntPsw1: "));
		DEBUG_PRINT(paramsp[CLNTPSW1]);
		
		DEBUG_PRINT(F("\nclntSsid2: "));
		DEBUG_PRINT(paramsp[CLNTSSID2]);
		
		DEBUG_PRINT(F("\nclntPsw2: "));
		DEBUG_PRINT(paramsp[CLNTPSW2]);
		
		DEBUG_PRINT(F("\nAPSsid: "));
		DEBUG_PRINT(paramsp[APPSSID]);
		
		DEBUG_PRINT(F("\nAPPPSW: "));
		DEBUG_PRINT(paramsp[APPPSW]);
		
		DEBUG_PRINT(F("\nwebUsr: "));
		DEBUG_PRINT(paramsp[WEBUSR]);
		
		DEBUG_PRINT(F("\nwebPsw: "));
		DEBUG_PRINT(paramsp[WEBPSW]);
		
		DEBUG_PRINT(F("\nmqtt user name: "));
		DEBUG_PRINT(paramsp[MQTTUSR]);
		
		DEBUG_PRINT(F("\nmqtt user password: "));
		DEBUG_PRINT(paramsp[MQTTPSW]);
		
		DEBUG_PRINT(F("\nmotor start delay1: "));
		DEBUG_PRINT(paramsp[STDEL1]);
		
		DEBUG_PRINT(F("\nmotor start delay2: "));
		DEBUG_PRINT(paramsp[STDEL2]);
		
		DEBUG_PRINT(F("\nsensor weight: "));
		DEBUG_PRINT(paramsp[VALWEIGHT]);
		
		DEBUG_PRINT(F("\nbarrel length: "));
		DEBUG_PRINT(paramsp[TLENGTH]);
		
		DEBUG_PRINT(F("\nbarrel  radius: "));
		DEBUG_PRINT(paramsp[BARRELRAD]);
		
		DEBUG_PRINT(F("\nbarrel THICKNESS: "));
		DEBUG_PRINT(paramsp[THICKNESS]);
		
		DEBUG_PRINT(F("\nbarrel SLATSRATIO: "));
		DEBUG_PRINT(paramsp[SLATSRATIO]);
		
		DEBUG_PRINT(F("\nrollhmode SWROLL1: "));
		DEBUG_PRINT(paramsp[SWROLL1]);
		
		DEBUG_PRINT(F("\nrollhmode SWROLL2: "));
		DEBUG_PRINT(paramsp[SWROLL2]);
		/*
		//if(rollmode[0]){
			DEBUG_PRINT(F("\nswitch SWACTION1: "));
			DEBUG_PRINT(paramsp[SWACTION1]);
			
			DEBUG_PRINT(F("\nswitch SWACTION2: "));
			DEBUG_PRINT(paramsp[SWACTION2]);
		//}	
			
		//if(rollmode[1]){
			DEBUG_PRINT(F("\nswitch SWACTION3: "));
			DEBUG_PRINT(paramsp[SWACTION3]);
			
			DEBUG_PRINT(F("\nswitch SWACTION4: "));
			DEBUG_PRINT(paramsp[SWACTION4]);
		//}	
		*/	
		for(i=0; i<CONFJSONDIM; ++i){
			DEBUG_PRINT(F("\nsensors CONFEXPR "));
			DEBUG_PRINT(i);
			DEBUG_PRINT(F(": "));
			DEBUG_PRINTLN(confcmdp[i]);
			DEBUG_PRINT(F("- len "));
			DEBUG_PRINT(i);
			DEBUG_PRINT(F(": "));
			DEBUG_PRINT(varStrOfst[i]);
		}	
		
		DEBUG_PRINT(F("\neeprom EEPROMLENOFST: "));
		DEBUG_PRINT(varStrOfst[CONFJSONDIM]);
}
/*
void writeOnOffAction(byte id, byte n){
	eepromBegin();
	EEPROM.write(SWACTION1OFST+n, id);
	EEPROM.end();
	paramsp[SWACTION1+n] = String(id);
	DEBUG_PRINT(F("Modified action SWACTION"));
	DEBUG_PRINT(n+1);
	DEBUG_PRINT(F(": "));
	DEBUG_PRINTLN(paramsp[SWACTION1+n]);
}
*/
void writeSWMode(byte id, byte sw){
	eepromBegin();
	EEPROM.write(SWROLL1OFST+sw, id);
	EEPROM.end();
	paramsp[SWROLL1OFST+sw] = String(id);
	DEBUG_PRINT(F("Modified action SWROLL"));
	DEBUG_PRINT(sw+1);
	DEBUG_PRINT(F(": "));
	DEBUG_PRINTLN(paramsp[SWROLL1OFST+sw]);
}

void writeOnOffConditions(){
	int i;
	
	eepromBegin();
	varStrOfst[0] = FIXEDPARAMSLEN;
	for(i=0; i<CONFJSONDIM; ++i){
		varStrOfst[i+1] = EEPROMWriteStr(varStrOfst[i], (confcmdp[i]).c_str()) + varStrOfst[i];
		DEBUG_PRINT(F("Modified sensors CONFEXPR "));
		DEBUG_PRINT(i);
		DEBUG_PRINT(F(": "));
		DEBUG_PRINTLN(confcmdp[i]);
	}
	EEPROMWriteInt(EEPROMLENOFST,varStrOfst[CONFJSONDIM]);
	DEBUG_PRINT(F("Modified sensors EEPROMLENOFST: "));
	DEBUG_PRINTLN(varStrOfst[i]);
	EEPROM.end();
}

void writeHaltDelay(unsigned int dly, byte n){
	paramsp[THALT1+n]=dly;
	eepromBegin();
	EEPROMWriteInt(THALT1OFST,(paramsp[THALT1+n]).toInt());
	DEBUG_PRINT(F("Modified THALT"));
	DEBUG_PRINT(n+1);
	DEBUG_PRINT(F(": "));
	DEBUG_PRINTLN(paramsp[THALT1+n]);
	EEPROM.end();
}





