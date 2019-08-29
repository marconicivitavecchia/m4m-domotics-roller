#include "common.h"
#define serverp (*serveru)

ESP8266WebServer *serveru; 
Par **parsp;

unsigned EEaddress = FIXEDPARAMSLEN;
unsigned varStrOfst[VARCONFDIM+1];
//bool rollmode[2] = {true, true};

inline void eepromBegin(){
	int i, len;

	for(i=0, len=0; i<VARCONFDIM; ++i){
		len += (parsp[p(i)]->getStrVal()).length();
	}
	EEPROM.begin(FIXEDPARAMSLEN + len + i);
}

void initCommon(ESP8266WebServer *serveri, Par **parsi){
	serveru=serveri; 
	parsp=parsi;
	for(int i=0; i<VARCONFDIM+1; ++i){
		varStrOfst[i] = 0;
	}
}

const char HTTP_FORM_HEAD[] PROGMEM =
"<head><meta charset=\"UTF-8\"><title>Document</title>"
//"{TC}"
"<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
"<style>"
"* {"
	"box-sizing: border-box;"
"}"
"[class*='col-']{"
	"padding: 0 15px;"
	"margin:0;"
"}"
"html, body, div{"
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
"input, textarea, select{"
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
".asidetop, .asidebottom{"
	"height: 10%;"
	"margin-top: auto;"
	"margin-bottom: auto;"
	"background-color: #333;"
"}"
".aside{"
	"margin: 18% 3%;"
	"background-color: #333;"
	"padding: 0 2%;"
	"text-align: center;"
"}"

"#temp{"
	"font-size: 3rem;"
	"background-color: #333;"
	"text-align: center;"
"}"
"#time{"
	"font-size: 2.6rem;"
	"background-color: #333;"
	"text-align: center;"
"}"
"#date{"
	"font-size: 2rem;"
	"background-color: #333;"
	"text-align: center;"
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
"@media only screen and (min-width: 600px){"
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
"@media only screen and (min-width: 768px){"
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
		"var vls = ['{\"up1\":\"255\"}','{\"down1\":\"255\"}','{\"up2\":\"255\"}','{\"down2\":\"255\"}','{\"dopwrcal\":\"1\"}'];"
		"var vlsp = ['{\"up1\":\"N\"}','{\"up2\":\"N\"}','{\"calpwr\":\"N\"}'];"
		"var action = '{\"onaction\":\"D\"}';"
		"var cond = ['{\"oncond1\":\"C\"}','{\"oncond2\":\"C\"}','{\"oncond3\":\"C\"}','{\"oncond4\":\"C\"}'];"
		"var conn = new WebSocket('ws://{WS}:81', ['arduino']);"
		"conn.onopen = function () {"
			//"conn.send('Connect ' + new Date());"
			"console.log('Connected on: ' + new Date());"
		"};"
		"conn.onerror = function (error) {"
			"console.log('WebSocket Error ', error);"
			"conn.close();"
			//"conn = new WebSocket('ws://{WS}:81', ['arduino']);"
		"};"
		"conn.onmessage = function (e) {"
			"console.log('Received: ', e.data);"
			"onRcv(e.data);"
		"};"
		"conn.onclose = function () {"
			"console.log('WebSocket connection closed');"
			"conn = new WebSocket('ws://{WS}:81', ['arduino']);"
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
		
const char PAHO_SRC[] PROGMEM = "<script src='https://cdnjs.cloudflare.com/ajax/libs/paho-mqtt/1.0.2/mqttws31.min.js' type='text/javascript'></script>";

const char HTTP_MQTT[] PROGMEM =
		// Create a client instance
		"var vls = ['{\"up1\":\"255\"}','{\"down1\":\"255\"}','{\"up2\":\"255\"}','{\"down2\":\"255\"}','{\"dopwrcal\":\"1\"}'];"
		"var vlsp = ['{\"up1\":\"N\"}','{\"up2\":\"N\"}','{\"calpwr\":\"N\"}'];"
		"var action = '{\"onaction\":\"D\"}';"
		"var cond = ['{\"oncond1\":\"C\"}','{\"oncond2\":\"C\"}','{\"oncond3\":\"C\"}','{\"oncond4\":\"C\"}'];"
		// Generate a random client ID
		"var clientID = '{MI}_' + parseInt(Math.random() * 100);"
		"var conn = new Paho.MQTT.Client('{MA}', Number('{WT}'), '/{PP}', clientID);"
		//"var conn = new Paho.MQTT.Client('{MA}', Number('{MT}'), clientID);"
		"console.log(conn);"
		"console.log('broker:{MA}, port:{WT}, path:{PP}, id:'+clientID);"
		// connect the client
		"conn.connect({onSuccess:onConnect});"
		"function onConnect() {"
			//"conn.send('Connect ' + new Date());"
			"console.log('onConnect');"
			"conn.subscribe('{MO}');"
		"};"
		"conn.onMessageArrived = function (e) {"
			"console.log('onMessageArrived:'+e.payloadString);"
			"onRcv(e.payloadString);"
		"};"
		"conn.onConnectionLost  = function (responseObject) {"
			"if (responseObject.errorCode !== 0) {"
				"console.log('onConnectionLost:'+responseObject.errorMessage);"
			"}"
		"};"
		"function send(str){"
		    "console.log('send: '+str);"
			"var msg = new Paho.MQTT.Message(str);"
			"msg.destinationName = '{QI}';"
			"conn.send(msg);"
		"};"
		"function actsend(str){"
			//"var btn=document.getElementById(eid);"
			"console.log('actsend: '+str);"
			"var v=action.replace('D',str);"
			"send(v);"
		"};"
		"function condsend(i,eid){"
			"var el=document.getElementById(eid);"
			"var c=cond[i].replace('C',el.value);"
			"send(c);"
			"console.log('condsend: '+c);"
		"};"
		"function confsend(i,actid,cnfid){"
			"actsend(i,actid);"
			"condsend(i,cnfid);"
		"};"
		"function press(s){"
			"send(s);"
		"};";

//-----Pages-----------------------------------------------------------------------------------------------------------------------------
const char HTTP_FORM_ROOT[] PROGMEM =
	"<html>{HD}<body>"
	"<div class='header' id='logo'><h1>MyTapparella</h1></div>"
		"<form action='login' method='POST'>"
			"<div class='grid-container'>"
				"<div class='col-6 col-s-12'>"
					"<label for='webusr'>Configuration web client webusr:</label>"
					"<input type='text' name='webusr' value='{WU}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='webpsw'>Configuration web client webpsw:</label>"
					 "<input type='webpsw' name='webpsw' placeholder='**********'>"
				"</div>"
			"</div>"
			"<div class='footer'>"
				"<input type='submit' value='Login'>"
			"</div>"
		"</form>"
	"</div></body></html>";

const char HTTP_FORM_WIFI[] PROGMEM =	
	"<html>{HD}<body>"
	"<div class='header' id='logo'><h1>MyTapparella</h1></div>"
	"<div id='form'>"
        "<form action='/login' method='POST'>"
			"<div class='grid-container'>"
				"<div class='col-6 col-s-12'><label for='clntssid1'>Wifi instructure 1 SSID:</label>"
					 "<input type='text' name='clntssid1' value='{S1}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='clntpsw1'>Wifi instructure 1 webpsw:</label>"
					 "<input type='webpsw' name='clntpsw1' value='{P1}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='clntssid2'>Wifi instructure 2 SSID:</label>"
					 "<input type='text' name='clntssid2' value='{S2}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='clntpsw2'>Wifi instructure 2 webpsw:</label>"
					 "<input type='webpsw' name='clntpsw2' value='{P2}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='appssid'>Wifi local AP SSID:</label>"
					 "<input type='text' name='appssid' value='{AS}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='apppsw'>Wifi local AP webpsw:</label>"
					 "<input type='webpsw' name='apppsw' value='{AP}'>"
				"</div>"
				"<div class='col-2'></div>"
				"<div class='col-2 col-s-12'>"
					"<input type='submit' name='svwifi' value='Save' formaction='/modify' formmethod='post'>"
				"</div>"
				"<div class='col-4'></div>"
				"<div class='col-2 col-s-12'>"
					"<input type='submit' value='Back'>"
				"</div>"
			"</div>"
        "</form>"
	"</div></body></html>";

const char HTTP_FORM_LOG[] PROGMEM =	
	"<html>{HD}<body>"
	"<div class='header' id='logo'><h1>MyTapparella</h1></div>"
	"<div id='form'>"
        "<form action='/login' method='POST'>"
			"<div class='grid-container'>"
				"<div class='col-6 col-s-12'><label for='serlog'>Set serial log</label>"
					 "<select id='serlog' name='serlog'>"
						  "<option value='0'>No log</option>"
						  "<option value='1'>Level 1 only</option>"
						  "<option value='2'>Level 1 and level 2</option>"
					 "</select><br>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='tlntlog'>Set telnet log</label>"
					 "<select id='tlntlog' name='tlntlog'>"
						  "<option value='0'>No log</option>"
						  "<option value='1'>Level 1 only</option>"
						  "<option value='2'>Level 1 and level 2</option>"
					 "</select><br>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='mqttlog'>Set MQTT log</label>"
					 "<select id='mqttlog' name='mqttlog'>"
						  "<option value='0'>No log</option>"
						  "<option value='1'>Level 1 only</option>"
						  "<option value='2'>Level 1 and level 2</option>"
					 "</select><br>"
				"</div>"
				"<div class='col-2'></div>"
					"<div class='col-2 col-s-12'>"
						"<input type='submit' name='svlog' value='Save' formaction='/modify' formmethod='post'>"
					"</div>"
					"<div class='col-4'></div>"
					"<div class='col-2 col-s-12'>"
						"<input type='submit' value='Back'>"
					"</div>"
				"</div>"
			"</div>"
        "</form>"
	"</div>"
	"<script>"		
		"document.getElementById('serlog').selectedIndex='{L1}';"
		"document.getElementById('tlntlog').selectedIndex='{L2}';"
		"document.getElementById('mqttlog').selectedIndex='{L3}';"
	"</script>"
	"</body></html>";
	
const char HTTP_FORM_SYSTEM[] PROGMEM =	
	"<html>{HD}<body onload = 'loadTimeZoneList();showLoaded()';>"
	"<div class='header' id='logo'><h1>MyTapparella</h1></div>"
	"<div id='form'>"
        "<form action='/login' method='POST'>"
			"<div class='grid-container'>"
				"<div class='col-6 col-s-12'>"
					"<label for='webusr'>Configuration web client webusr:</label>"
					 "<input type='text' name='webusr' value='{WU}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='webpsw'>Configuration web client webpsw:</label>"
					 "<input type='webpsw' name='webpsw' value='{WP}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='ntpaddr1'>NTP server 1:</label>"
					 "<input type='text' name='ntpaddr1' value='{N1}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='ntpaddr2'>NTP server 2:</label>"
					 "<input type='text' name='ntpaddr2' value='{N2}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='utcsync'>NTP sync interval (sec):</label>"
					 "<input type='text' name='utcsync' value='{N3}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='utcadj'>NTP error adjust (msec):</label>"
					 "<input type='text' name='utcadj' value='{N4}'>"
				"</div>"	
				"<div class='col-6 col-s-12'><label for='utczone'>NTP SDT time zones (hour):</label>"
					 "<select id='utczone' name='utczone'  onchange='showSelected()'></select><br>"
				"</div>"	
				"<div class='col-6 col-s-12'><label for='report'>Selected time zone</label>"
					"<input type='text' id='report' name='report' disabled='disabled' />"
				"</div>"
				"<div class='col-6 col-s-12'><label for='utcsdt'>Set daylight save (legal time)</label>"
					 "<input type='checkbox' name='utcsdt' value='y' {N6}>"
				"</div>"
#if (AUTOCAL_HLW8012) 
				"<div class='col-6 col-s-12'><label for='acvolt'>AC voltage:</label>"
					 "<input type='text' name='acvolt' value='{N7}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='calpwr'>Calibration power:</label>"
					 "<input type='text' id='calpwr' name='calpwr' value='{N8}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='pwrmult'>Power multiplier:</label>"
					 "<input type='text' id='pwrmult' name='pwrmult' value='{N9}' disabled>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='calbtn'>Press button for calibration:</label>"
					 "<input type='button' id='calbtn' name='calbtn' value='Calibrate' onmousedown='this.style.opacity=\"1\"' onmouseup='this.style.opacity=\"0.6\"' ontouchstart='this.style.opacity=\"1\"' ontouchend='this.style.opacity=\"0.6\"'>"
				"</div>"
#endif
				"<div class='col-6 col-s-12'><label for='reboot'>Reboot the system with default config</label>"
					 "<input type='checkbox' name='rebootd' value='y'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='reboot'>Reboot the system</label>"
					 "<input type='checkbox' name='reboot' value='y'>"
				"</div>"
#if (!AUTOCAL) 
				"<div class='col-6 col-s-12'>"
					"<label for=\stdel1'>Start delay on click of group 1:</label>"
					 "<input type='text' name='stdel1' value='{S1}'>"
				"</div>"
				"<div class='col-6 col-s-12'>"
					"<label for=\stdel2'>Start delay on click of group 2:</label>"
					 "<input type='text' name='stdel2' value='{S2}'>"
				"</div>"
#endif		
				"<div class='col-2'></div>"
					"<div class='col-2 col-s-12'>"
						"<input type='submit' name='svsystem' value='Save' formaction='/modify' formmethod='post'>"
					"</div>"
					"<div class='col-4'></div>"
					"<div class='col-2 col-s-12'>"
						"<input type='submit' value='Back'>"
					"</div>"
				"</div>"
			"</div>"
        "</form>"
	"</div>"
	"<script>"
		"function loadTimeZoneList(){" 
			"var d = new Date();"
			"var n = -d.getTimezoneOffset()/60;"
			"var select = document.getElementById('utczone');"
			"select.innerHTML = '';" 
			"for(i=-11; i<13; i++){"
				"option = document.createElement('option');"      
				"option.textContent = '(GMT '+(i<0?'':'+') + i +':00)';"  
				"option.value = i;"
				"if (n == i){"
					"option.selected = true;"
				"}"
				"select.appendChild(option);"
			"}"
			"showSelected();"
		"};"
		"function showSelected(){"
			"document.getElementById('report').value=document.getElementById('utczone').value;"
		"}"
		"function showLoaded(){"
			"document.getElementById('report').value={N5};"
		"}"
#if (AUTOCAL_HLW8012) 
		"var cb=document.getElementById('calbtn');"
		"cb.addEventListener('click', function(){"
			"cp=document.getElementById('calpwr').value;"
			"var vl=vlsp[2].replace('N', cp);"
			"console.log(vl);"
			"press(vl);"//il parser in uso non rispetta l'ordine dei campi nella stringa json!
			"press(vls[4]);"//per farlo di devono inviare due json separati nell'ordine corretto
		"}, false);"

		"function onRcv(d) {"
			//"document.getElementById('p').innerHTML = f.data;\n"
			"var obj = JSON.parse(d);"
			"for(x in obj){"
				"if(x=='{PM}'){"
					"var el1 = document.getElementById('pwrmult');"
					"el1.value=obj[x];"
				"}"
			"}"
		"}"
		"{SH}"
#endif
	"</script>"
	"</body></html>";
	
const char HTTP_FORM_MQTT[] PROGMEM =	
	"<html>{HD}<body>"
	"<div class='header' id='logo'><h1>MyTapparella</h1></div>"
	"<div id='form'>"
		"<form action='/login' method='POST'>"
			"<div class='grid-container'>"
				"<div class='col-6 col-s-12'><label for='mqttaddr'>MQTT server:</label>"
					 "<input type='text' name='mqttaddr' value='{MA}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='mqttid'>MQTT Client ID:</label>"
					 "<input type='text' name='mqttid' value='{MI}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='mqttport'>MQTT port:</label>"
					 "<input type='text' name='mqttport' value='{MT}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='wsport'>WS port:</label>"
					 "<input type='text' name='wsport' value='{WT}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='mqttproto'>MQTT path:</label>"
					 "<input type='text' name='mqttproto' value='{PP}'>"
				"</div>"
				"<div class='col-6 col-s-12'>"
				"</div>"
				/*"<div class='col-6 col-s-12'><label for='mqttproto'>MQTT protocol:</label>"
					 "<select id='mqttproto' name='mqttproto'>"
						  "<option value='ws' {P1}>ws</option>"
						  "<option value='tcp' {P2}>tcp</option>"
					 "</select>"
				"</div>"*/
				"<div class='col-6 col-s-12'><label for='mqttusr'>MQTT user name:</label>"
					 "<input type='text' name='mqttusr' value='{MU}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='mqttpsw'>MQTT user webpsw:</label>"
					 "<input type='text' name='mqttpsw' value='{MP}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='mqttintopic'>In topic:</label>"
					 "<input type='text' name='mqttintopic' value='{QI}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='mqttouttopic'>Out topic:</label>"
					 "<input type='text' name='mqttouttopic' value='{MO}'>"
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
				/*
				"<div class='col-6 col-s-12'><label for='btnmeanpwr'>Mean power request button:</label>"
					 "<input type='text' name='btnmeanpwr' value='{J6}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='btnpeakpwr'>Peak power request button:</label>"
					 "<input type='text' name='btnpeakpwr' value='{J7}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='btnall'>All states request button:</label>"
					 "<input type='text' name='btnall' value='{J8}'>"
				"</div>"*/				
				"<div class='col-2'></div>"
				"<div class='col-2 col-s-12'>"
					"<input type='submit' name='svmqtt' value='Save' formaction='/modify' formmethod='post'>"
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
	"<div class='header' id='logo'><h1>MyTapparella</h1></div>"
	"<div id='form'>"
        "<form action='/login' method='POST'>"
			"<div class='grid-container'>"
				"<div class='col-6 col-s-12'><label for='swroll1'>Mode switch 1 and 2 (Switch if checked, Roller Shutter if unchecked)</label>"
					 "<input type='checkbox' name='swroll1' value='1' {H1}>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='swroll2'>Mode switch 3 and 4 (Switch if checked, Roller Shutter if unchecked)</label>"
					 "<input type='checkbox' name='swroll2' value='1' {H2}>"
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
				"<div class='col-6 col-s-12'><label for='tlength'>Rollershutter excursion:</label>"
					 "<input type='text' name='tlength' value='{TL}'>"
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
					"<input type='submit' name='svlogic' value='Save' formaction='/modify' formmethod='post'>"
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
	"<div class='header' id='logo'><h1>MyTapparella</h1></div>"
	"<div id='form'>"
		"<form action='/login' method='POST'>"
			"<div class='grid-container'>"
				"<div class='col-6 col-s-12 boxed'>"
					"<label for='c1'>Switch 1 condition:</label>"
					"<textarea id='c1' name='c1' cols='100' rows='4' {H3}>{C1}</textarea>"
					"<div id='sw1' {V1}>"
						"<label for='a1'>Switch 1 action</label>"
						 "<select id='a1' name='a1'>"
							  "<option value='0'>SetReset</option>"
							  "<option value='1'>No action</option>"
							  "<option value='2'>Monostable normally open</option>"
							  "<option value='3'>Monostable normally close</option>"
						 "</select><br>"
						 "<label for='haltdelay1'>Delay for timer SW 1</label>"
						 "<input type='text' id='haltdelay1' name='haltdelay1' value='{D1}'>"
					 "</div>"
					 "<label for='smplt1'>Sample time SW 1</label>"
					 "<input type='text' id='smplt1' name='smplt1' value='{S1}'>"
					 "<label for='oe1'>Output enable SW 1</label>"
					 "<input type='checkbox' id='oe1' name='oe1' value='0' {O1}>"
				"</div>"
				"<div class='col-6 col-s-12 boxed'>"
					"<label for='c2'>Switch 2 condition:</label>"
					"<textarea id='c2' name='c2' cols='100' rows='4' {H3}>{C2}</textarea>"
					"<div id='sw2' {V1}>"
						"<label for='a2'>Switch 2 action</label>"
						"<select id='a2' name='a2'>"
							  "<option value='0'>SetReset</option>"
							  "<option value='1'>No action</option>"
							  "<option value='2'>Monostable normally open</option>"
							  "<option value='3'>Monostable normally close</option>"
						"</select><br>"
						"<label for='haltdelay2'>Delay for timer SW 2</label>"
						"<input type='text' id='haltdelay2' name='haltdelay2' value='{D2}'>"
					"</div>"
					"<label for='smplt2'>Sample time SW 2</label>"
					"<input type='text' id='smplt2' name='smplt2' value='{S2}'>"
					"<label for='oe2'>Output enable SW 2</label>"
					"<input type='checkbox' id='oe2' name='oe2' value='0' {O2}>"
				"</div>"
				"<div class='col-6 col-s-12 boxed'>"
					"<label for='c3'>Switch 3 condition:</label>"
					"<textarea id='c3' name='c3' cols='100' rows='4' {H4}>{C3}</textarea>"
					"<div id='sw3' {V2}>"
						"<label for='a3'>Switch 3 action</label>"
						"<select id='a3' name='a3'>"
							  "<option value='0'>SetReset</option>"
							  "<option value='1'>No action</option>"
							  "<option value='2'>Monostable normally open</option>"
							  "<option value='3'>Monostable normally close</option>"
						"</select><br>"
						"<label for='haltdelay3'>Delay for timer SW 3</label>"
						"<input type='text' id='haltdelay3' name='haltdelay3' value='{D3}'>"
					"</div>"
					"<label for='smplt3'>Sample time SW 3</label>"
					"<input type='text' id='smplt3' name='smplt3' value='{S3}'>"
					"<label for='oe3'>Output enable SW 3</label>"
					"<input type='checkbox' id='oe3' name='oe3' value='0' {O3}>"
				"</div>"
				"<div class='col-6 col-s-12 boxed'>"
					"<label for='c4'>Switch 4 condition:</label>"
					"<textarea id='c4' name='c4' cols='100' rows='4' {H4}>{C4}</textarea>"
					"<div id='sw4' {V2}>"
						"<label for='a4'>Switch 4 action</label>"
						"<select id='a4' name='a4'>"
							  "<option value='0'>SetReset</option>"
							  "<option value='1'>No action</option>"
							  "<option value='2'>Monostable normally open</option>"
							  "<option value='3'>Monostable normally close</option>"
						"</select><br>"
						"<label for='haltdelay4'>Delay for timer SW 4</label>"
						"<input type='text' id='haltdelay4' name='haltdelay4' value='{D4}'>"
					"</div>"
					"<label for='smplt4'>Sample time SW 4</label>"
					"<input type='text' id='smplt4' name='smplt4' value='{S4}'>"
					"<label for='oe2'>Output enable SW 4</label>"
					"<input type='checkbox' id='oe4' name='oe4' value='0' {O4}>"
				"</div>"
				"<div class='col-12 col-s-12 boxed'>"
					"<label for='act'>Switches action commands:</label>"
					"<textarea id='act' name='act' cols='100' rows='4' {H4}>{AC}</textarea>"
				"</div>"
				"<div class='col-12 col-s-12 boxed'>"
					"<label for='call'>General action commands:</label>"
					"<textarea id='call' name='call' cols='100' rows='4' {H5}>{AD}</textarea>"
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
		"call.addEventListener('change', function(){actsend(call.value);}, false);"
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
		"function onRcv(d) {"
			//"document.getElementById('p').innerHTML = f.data;\n"
			"var obj = JSON.parse(d);"
			"for(x in obj){"
				"if(x=='mode1'){"
					"var el1 = document.getElementById('sw1');"
					"var el2 = document.getElementById('sw2');"
					"if(obj[x] == \"1\"){"
						"el1.style.display = 'none';"
						"el2.style.display = 'none';"
					"}else{"
						"el1.style.display = 'block';"
						"el2.style.display = 'block';"
					"}"
				"}"
				"if(x=='mode2'){"
					"var el3 = document.getElementById('sw3');"
					"var el4 = document.getElementById('sw4');"
					"if(obj[x] == \"1\"){"
						"el3.style.display = 'none';"
						"el4.style.display = 'none';"
					"}else{"
						"el3.style.display = 'block';"
						"el4.style.display = 'block';"
					"}"
				"}"
			"}"
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
	"<div class='header' id='logo'><h1>MyTapparella</h1></div>"
	"<div class='grid-container'>"
		"<div class='col-3 col-s-12'></div>"
		"<div id='form' class='col-6 col-s-12'>"
			"<form action='/login' method='POST'>"
				"<input type='submit' formaction='systconf' formmethod='post' value='System configuration'>"
				"<input type='submit' formaction='wificonf' formmethod='post' value='WiFi configuration'>"
				"<br><input type='submit' formaction='mqttconf' formmethod='post' value='MQTT configuration'>"
				"<br><input type='submit' formaction='logicconf' formmethod='post' value='Logic configuration'>"
				"<br><input type='submit' formaction='eventconf' formmethod='post' value='Events configuration'>"
				"<br><input type='submit' formaction='logconf' formmethod='post' value='Log configuration'>"
				"<br><input type='submit' value='Exit' name='disconnect'>"
			"</form>"
		"</div>"
		"<div class='col-3 col-s-12'></div>"
	"</div></body></html>";
	
const char HTTP_FORM_SUCCESS[] PROGMEM =	
	"<html>{HD}<body>"
	"<div class='header' id='logo'><h1>MyTapparella</h1></div>"
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
	"<html>{HD}<body>{SP}"
	"<div class='header' id='logo'><h1>MyTapparella</h1></div>"
	"<div class='grid-container'>"
		"<div class='col-4 col-s-12'>"
			"<div class='asidetop'></div>"
			"<div class='aside'>"
				"<span id='temp'></span>"
			"</div>"
			"<div class='aside'>"
				"<span id='time'></span>"
			"</div>"
			"<div class='aside'>"
				"<span id='date'></span>"
			"</div>"
			"<div class='aside'>"
				"<span id='ipwr'></span>"
			"</div>"
			"<div class='aside'>"
				"<span id='iacvolt'></span>"
			"</div>"
			"<div class='asidebottom'></div>"
		"</div>"
		"<div class='col-6 col-s-12'>"
			"<div id='form'>"
				"<form>"
					"<input id='up1' type='button' value='Button 1 UP' onmousedown='this.style.opacity=\"1\"' onmouseup='this.style.opacity=\"0.6\"' ontouchstart='this.style.opacity=\"1\"' ontouchend='this.style.opacity=\"0.6\"'>"
					"<br>"
					"<input id='down1' type='button' value='Button 1 DOWN' onmousedown='this.style.opacity=\"1\"' onmouseup='this.style.opacity=\"0.6\"' ontouchstart='this.style.opacity=\"1\"' ontouchend='this.style.opacity=\"0.6\"'>"
					"<br><br><br>"
					"<input id='up2' type='button' value='Button 2 UP'  onmousedown='this.style.opacity=\"1\"' onmouseup='this.style.opacity=\"0.6\"' ontouchstart='this.style.opacity=\"1\"' ontouchend='this.style.opacity=\"0.6\"'>"
					"<br>"
					"<input id='down2' type='button' value='Button 2 DOWN' onmousedown='this.style.opacity=\"1\"' onmouseup='this.style.opacity=\"0.6\"' ontouchstart='this.style.opacity=\"1\"' ontouchend='this.style.opacity=\"0.6\"'>"
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
	"<script>"
		"var a=[0,0];"
		"var updt=[0,0];"
		"var p=[0,0];"
		"var tr=[0,0];"
		"(document.getElementById('mid')).innerHTML=50-{PD};"
		//"var c=[0,0];"
		"var dir=[0,0];"
		"var ie=[0,0,0,0];"
		"{SH}"
		"var up1=document.getElementById('up1');"
		"var dw1=document.getElementById('down1');"
		"var up2=document.getElementById('up2');"
		"var dw2=document.getElementById('down2');"
		"up1.addEventListener('click', function(){press(vls[0]);});"
		//"up1.attachEvent('onclick', function(){press(vls[0]);});"
		"dw1.addEventListener('click', function(){press(vls[1]);});"
		//"dw1.attachEvent('onclick', function(){press(vls[1]);});"
		"up2.addEventListener('click', function(){press(vls[2]);});"
		//"up2.attachEvent('onclick', function(){press(vls[2]);});"
		"dw2.addEventListener('click', function(){press(vls[3]);});"
		//"dw2.attachEvent('onclick', function(){press(vls[3]);});"
		
		"var sld1 = document.getElementById('rng1');"
		"var o1 = document.getElementById('val1');"
		"var tmp = document.getElementById('temp');"
		"var dt = document.getElementById('date');"
		"var tm = document.getElementById('time');"
		"var pw = document.getElementById('ipwr');"
		"var ac = document.getElementById('iacvolt');"
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
		"function onRcv(d) {"
			//"document.getElementById('p').innerHTML = f.data;\n"
			"var obj = JSON.parse(d);"
			"console.log('Arrived data: '+obj);"
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
					"if(x=='up1' || x=='down1' || x=='up2' || x=='down2'){"
						"if(obj[x] == \"1\"){"
							"el.style.backgroundColor = \"#b30000\";"
						"}else{"
							"el.style.backgroundColor = \"#00ccff\";"
						"}"
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
					"if(x=='{DT}'){"
						"var res = obj[x].split('-');"
						"dt.innerHTML=res[0];"
						"tm.innerHTML=res[1];"
						"console.log('date:'+res[0]+' Time:'+res[1]);" 
						"dt.style.backgroundColor = \"#333\";"
						"tm.style.backgroundColor = \"#333\";"
					"}"
#if (AUTOCAL_HLW8012) 
					"if(x=='{PW}'){"
						"pw.innerHTML=obj[x]+' W';"
						"pw.style.backgroundColor = \"#333\";"
					"}"
					"if(x=='{AC}'){"
						"ac.innerHTML=obj[x]+' V';"
						"ac.style.backgroundColor = \"#333\";"
					"}"
#endif
				"}else{"	
					"if(x=='sp1')"
						"p[0]=Number(obj[x]);"						
					"if(x=='sp2')"
						"p[1]=Number(obj[x]);"
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
			"if(t>0){"
				//"l+=delay;"
				//"console.log('------------------------------------');"
				"var e=(dir[n]*l)/k;"
				//"console.log('dir[n]:'+dir[n]);"
				//"console.log('e:'+e);"
				//"console.log('----------------------------------');"
				"clearInterval(updt[n]);"
				"var aa=Math.round(calcLen(t,l));"
				"if(isNaN(aa) || !isFinite(aa))p.value=0; else p.value=aa;"
				"console.log('aa: '+aa);"
				"console.log('t:'+t+' p.value:'+p.value);"
				"updt[n]=setInterval(function(){"
					"if(dir[n]!=0){"
						"t=t+e;"
						"aa=Math.round(calcLen(t-delay*dir[n],l));"
						"console.log('aa: '+aa);"
						"if(isNaN(aa) || !isFinite(aa))p.value=0; else p.value=aa;"
						"console.log('++++++++++++++');"
						"console.log('t:'+(t-delay*dir[n])+' p.value:'+p.value+' dir:'+dir[n]);"
					"}else{"
						"a[n]=0;"
					"};"
				"}, l/k);"
			"};"
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
		"document.onload='function(){"
			"dir[0]=1;"
			"dir[1]=1;"
			"pr1.value=10;"
			"pr2.value=20;"
			//"startPrgrBar(pr1,10000,100,0);"
			//"startPrgrBar(pr2,10000,100,1);"
		"}';"
		//"{TC}"
	"</script>"
	"</body></html>";

	//-------------------------------------------------------------------
//fine variabili globali

void handleNotFound(){
	startPageLoad();
	DEBUG_PRINTLN(F("Enter handleNotFound"));
	is_authentified(serverp);
}
	
void handleRoot() {   // When URI / is requested, send a web page with a button to toggle the LED
    startPageLoad();
	
	String page = FPSTR(HTTP_FORM_ROOT);
	page.replace(F("{HD}"),  FPSTR(HTTP_FORM_HEAD));
	page.replace(F("{WU}"),  parsp[p(WEBUSR)]->getStrVal()); 
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
	stopPageLoad();
}

void handleLogin() {  // If a POST request is made to URI /login
  startPageLoad();
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
  else if(!serverp.hasArg("webusr")||!serverp.hasArg("webpsw")||serverp.arg("webusr")==NULL||serverp.arg("webpsw")==NULL)
  { // If the POST request doesn't have webusr and webpsw data
     DEBUG_PRINTLN(F("Userneme "));
	 DEBUG_PRINTLN(serverp.arg("webusr"));
	 DEBUG_PRINTLN(F(" o webpsw "));
	 DEBUG_PRINTLN(serverp.arg("webpsw"));
	 DEBUG_PRINTLN(F(" assenti "));
	 ok=false;
  }
  else if(serverp.arg("webusr") == static_cast<ParStr32*>(parsp[p(WEBUSR)])->getStrVal() && static_cast<ParStr32*>(parsp[p(WEBPSW)])->getStrVal())  
  {
	DEBUG_PRINTLN(F("Login di "));
	DEBUG_PRINTLN(serverp.arg("webusr"));
	DEBUG_PRINTLN(F(" effettuato con successo "));
	ok=true;
  }else{
	DEBUG_PRINTLN(F("Userneme "));
	DEBUG_PRINTLN(serverp.arg("webusr"));
	DEBUG_PRINTLN(F(" o webpsw "));
	DEBUG_PRINTLN(serverp.arg("webusr"));
	DEBUG_PRINTLN(F(" scorretti"));
	ok=false;  
  }
  
  String page = FPSTR(HTTP_FORM_LOGIN);
	
  if(ok) { 
		// If both the webusr and the webpsw are correct
		//Head placeholders
		page.replace(F("{HD}"), FPSTR(HTTP_FORM_HEAD));
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
  stopPageLoad();
}

void handleWifiConf() {  // If a POST request is made to URI /login
  startPageLoad();
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
		//Body placeholders
		page.replace(F("{S1}"), parsp[p(CLNTSSID1)]->getStrVal());
		page.replace(F("{P1}"), parsp[p(CLNTPSW1)]->getStrVal());
		page.replace(F("{S2}"), parsp[p(CLNTSSID2)]->getStrVal());
		page.replace(F("{P2}"), parsp[p(CLNTPSW2)]->getStrVal());
		page.replace(F("{AS}"), parsp[p(APPSSID)]->getStrVal());
		page.replace(F("{AP}"), parsp[p(APPPSW)]->getStrVal());
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
  stopPageLoad();
}

void handleSystemConf() {  // If a POST request is made to URI /login
  startPageLoad();
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
		//Body placeholders
		page.replace(F("{WU}"), parsp[p(WEBUSR)]->getStrVal());
		page.replace(F("{WP}"), parsp[p(WEBPSW)]->getStrVal());
		page.replace(F("{N1}"), parsp[p(NTPADDR1)]->getStrVal());
		page.replace(F("{N2}"), parsp[p(NTPADDR2)]->getStrVal());
		page.replace(F("{N3}"), parsp[p(UTCSYNC)]->getStrVal());
		page.replace(F("{N4}"), parsp[p(UTCADJ)]->getStrVal());
		page.replace(F("{N5}"), parsp[p(UTCZONE)]->getStrVal());
		page.replace(F("{N6}"), (parsp[p(UTCSDT)]->getStrVal() == "1")?"checked":"");
#if (AUTOCAL_HLW8012) 
		page.replace(F("{N7}"), parsp[p(ACVOLT)]->getStrVal());
		page.replace(F("{N8}"), parsp[p(CALPWR)]->getStrVal());	
		page.replace(F("{N9}"), parsp[p(PWRMULT)]->getStrVal());
		page.replace(F("{PM}"), parsp[p(PWRMULT)]->getStrJsonName());
		page.replace(F("{SH}"), FPSTR(HTTP_WEBSOCKET));
		page.replace(F("{WS}"), parsp[p(LOCALIP)]->getStrVal());
#endif		
#if (!AUTOCAL) 
		page.replace(F("{S1}"), parsp[p(STDEL1)]->getStrVal());
		page.replace(F("{S2}"), parsp[p(STDEL2)]->getStrVal());
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
  stopPageLoad();
}

void handleMQTTConf() {  // If a POST request is made to URI /login
  startPageLoad();
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
		//Body placeholders
		page.replace(F("{MA}"), parsp[p(MQTTADDR)]->getStrVal());
		page.replace(F("{MT}"), parsp[p(MQTTPORT)]->getStrVal());
		page.replace(F("{WT}"), parsp[p(WSPORT)]->getStrVal());
		page.replace(F("{MU}"), parsp[p(MQTTUSR)]->getStrVal());
		page.replace(F("{MP}"), parsp[p(MQTTPSW)]->getStrVal());
		page.replace(F("{MI}"), parsp[p(MQTTID)]->getStrVal());
		page.replace(F("{MO}"), parsp[p(MQTTOUTTOPIC)]->getStrVal());
		page.replace(F("{QI}"), parsp[p(MQTTINTOPIC)]->getStrVal());
		page.replace(F("{J1}"), parsp[MQTTUP1]->getStrJsonName());
		page.replace(F("{J2}"), parsp[MQTTDOWN1]->getStrJsonName());
		page.replace(F("{J3}"), parsp[MQTTUP2]->getStrJsonName());
		page.replace(F("{J4}"), parsp[MQTTDOWN2]->getStrJsonName());
		page.replace(F("{PP}"), parsp[p(MQTTPROTO)]->getStrVal());

		serverp.sendHeader("Cache-Control", "no-cache");
		serverp.sendHeader("Set-Cookie", "ESPSESSIONID=1");
		serverp.send(200, "text/html", page);
  } else {   
		DEBUG_PRINTLN(F("Login non consentito "));
		serverp.sendHeader("Location", "/");
		serverp.sendHeader("Cache-Control", "no-cache");
		serverp.send(301);
  }
  stopPageLoad();
}

void handleLogicConf() {  // If a POST request is made to URI /login
  startPageLoad();
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
		//Body placeholders
		page.replace(F("{TU}"), parsp[p(THALT1)]->getStrVal());
		page.replace(F("{TD}"), parsp[p(THALT2)]->getStrVal());
		page.replace(F("{DU}"), parsp[p(THALT3)]->getStrVal());
		page.replace(F("{DD}"), parsp[p(THALT4)]->getStrVal());
		if(static_cast<ParUint8*>(parsp[SWROLL1])->val == 0){
			page.replace(F("{H3}"), "disabled");
		}else{
			page.replace(F("{H3}"), "");
		}
		if(static_cast<ParUint8*>(parsp[SWROLL2])->val == 0){
			page.replace(F("{H4}"), "disabled");
		}else{
			page.replace(F("{H4}"), "");
		}
		page.replace(F("{TL}"), parsp[p(TLENGTH)]->getStrVal());
		page.replace(F("{BR}"), parsp[p(BARRELRAD)]->getStrVal());
		page.replace(F("{TN}"), parsp[p(THICKNESS)]->getStrVal());
		page.replace(F("{SR}"), parsp[p(SLATSRATIO)]->getStrVal());
		page.replace(F("{H1}"), (parsp[p(SWROLL1)]->getStrVal() == "0")?"checked":"");
		page.replace(F("{H2}"), (parsp[p(SWROLL2)]->getStrVal() == "0")?"checked":"");
		
		serverp.sendHeader("Cache-Control", "no-cache");
		serverp.sendHeader("Set-Cookie", "ESPSESSIONID=1");
		serverp.send(200, "text/html", page);
  } else {   
		DEBUG_PRINTLN(F("Login non consentito "));
		serverp.sendHeader("Location", "/");
		serverp.sendHeader("Cache-Control", "no-cache");
		serverp.send(301);
  }
  stopPageLoad();
}

void handleLogConf() {  // If a POST request is made to URI /login
  startPageLoad();
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
  String page = FPSTR(HTTP_FORM_LOG);
  
  if(ok) { 
		//Head placeholders
		page.replace(F("{HD}"), FPSTR(HTTP_FORM_HEAD) );
		//Body placeholders
		uint8_t num = static_cast<ParUint8*>(parsp[p(LOGSLCT)])->val;
		page.replace(F("{L1}"), String((num >> 0) & 0x3));
		page.replace(F("{L2}"), String((num >> 2) & 0x3));
		page.replace(F("{L3}"), String((num >> 4) & 0x3));
		
		DEBUG_PRINT(F("{L1]: "));
		DEBUG_PRINTLN(String((num >> 0) & 0x3));
		
		DEBUG_PRINT(F("{L2]: "));
		DEBUG_PRINTLN(String((num >> 2) & 0x3));
		
		DEBUG_PRINT(F("{L3]: "));
		DEBUG_PRINTLN(String((num >> 4) & 0x3));

		serverp.sendHeader("Cache-Control", "no-cache");
		serverp.sendHeader("Set-Cookie", "ESPSESSIONID=1");
		serverp.send(200, "text/html", page);
  } else {   
		DEBUG_PRINTLN(F("Login non consentito "));
		serverp.sendHeader("Location", "/");
		serverp.sendHeader("Cache-Control", "no-cache");
		serverp.send(301);
  }
  stopPageLoad();
}

//EVENT MANAGEMENT
void handleEventConf() {  // If a POST request is made to URI /login
	startPageLoad();
	
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
		page.replace(F("{SH}"), FPSTR(HTTP_WEBSOCKET) );
		page.replace(F("{WS}"), parsp[p(LOCALIP)]->getStrVal());
		//Body placeholders
		page.replace(F("{C1}"), parsp[p(ONCOND1)]->getStrVal());
		page.replace(F("{C2}"), parsp[p(ONCOND2)]->getStrVal());
		page.replace(F("{C3}"), parsp[p(ONCOND3)]->getStrVal());
		page.replace(F("{C4}"), parsp[p(ONCOND4)]->getStrVal());
		page.replace(F("{AC}"), parsp[p(ACTIONEVAL)]->getStrVal());
		page.replace(F("{AD}"), parsp[p(ONCOND5)]->getStrVal());
		page.replace(F("{D1}"), parsp[p(THALT1)]->getStrVal());
		page.replace(F("{D2}"), parsp[p(THALT2)]->getStrVal());
		page.replace(F("{D3}"), parsp[p(THALT3)]->getStrVal());
		page.replace(F("{D4}"), parsp[p(THALT4)]->getStrVal());
		page.replace(F("{S1}"), String(getCntValue(1)));
		page.replace(F("{S2}"), String(getCntValue(2)));
		page.replace(F("{S3}"), String(getCntValue(3)));
		page.replace(F("{S4}"), String(getCntValue(4)));
		if(static_cast<ParUint8*>(parsp[SWROLL1])->val == 0){
			page.replace(F("{V1}"), "style=\"display:block\"");
		}else{
			page.replace(F("{V1}"), "style=\"display:none\"");
		}
		if(static_cast<ParUint8*>(parsp[SWROLL2])->val == 0){
			page.replace(F("{V2}"), "style=\"display:block\"");
		}else{
			page.replace(F("{V2}"), "style=\"display:none\"");
		}
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
  stopPageLoad();
}
//END EVENT MANAGEMENT*/

void handleCmd() {  // If a POST request is made to URI /login
	startPageLoad();

	String page = FPSTR(HTTP_FORM_CMD);
	//Head placeholders
	page.replace(F("{SH}"), FPSTR(HTTP_WEBSOCKET));
	page.replace(F("{HD}"), FPSTR(HTTP_FORM_HEAD));
	page.replace(F("{SP}"), "");
	page.replace(F("{WS}"), parsp[p(LOCALIP)]->getStrVal());
	page.replace(F("{TL}"), String(getTaplen())); 
	page.replace(F("{BR}"), parsp[p(BARRELRAD)]->getStrVal());
	page.replace(F("{TN}"), parsp[p(THICKNESS)]->getStrVal());
	page.replace(F("{SR}"), parsp[p(SLATSRATIO)]->getStrVal());
	page.replace(F("{NM}"), String(getNmax()));
	page.replace(F("{PD}"), String(round(getPosdelta())));
	page.replace(F("{TP}"), parsp[MQTTTEMP]->getStrJsonName());
	page.replace(F("{DT}"), parsp[MQTTDATE]->getStrJsonName());
#if (AUTOCAL_HLW8012) 
	page.replace(F("{PW}"), parsp[INSTPWR]->getStrJsonName());
	page.replace(F("{AC}"), parsp[INSTACV]->getStrJsonName());
#endif
	//Body placeholders
	//DEBUG_PRINTLN(page);
	//DEBUG_PRINTLN(F("Scrittura cookie handleMQTTConf "));
	serverp.sendHeader("Cache-Control", "no-cache");
	serverp.sendHeader("Set-Cookie", "ESPSESSIONID=0");
	serverp.send(200, "text/html", page);
}

void handleMqttCmd() {  // If a POST request is made to URI /login
	startPageLoad();

	String page = FPSTR(HTTP_FORM_CMD);
	//Head placeholders
	page.replace(F("{HD}"), FPSTR(HTTP_FORM_HEAD));
	page.replace(F("{SH}"), FPSTR(HTTP_MQTT));
	//page.replace(F("{TC}"), FPSTR(MQTT_CLIENT));PAHO_SRC
	page.replace(F("{SP}"), FPSTR(PAHO_SRC));
	
	page.replace(F("{MA}"), parsp[p(MQTTADDR)]->getStrVal());
	page.replace(F("{MT}"), parsp[p(MQTTPORT)]->getStrVal());
	page.replace(F("{WT}"), parsp[p(WSPORT)]->getStrVal());
	page.replace(F("{MU}"), parsp[p(MQTTUSR)]->getStrVal());
	page.replace(F("{MP}"), parsp[p(MQTTPSW)]->getStrVal());
	page.replace(F("{MI}"), parsp[p(MQTTID)]->getStrVal());
	page.replace(F("{MO}"), parsp[p(MQTTOUTTOPIC)]->getStrVal());
	page.replace(F("{QI}"), parsp[p(MQTTINTOPIC)]->getStrVal());
	page.replace(F("{PP}"), parsp[p(MQTTPROTO)]->getStrVal());

	page.replace(F("{TL}"), String(getTaplen()));
	page.replace(F("{BR}"), parsp[p(BARRELRAD)]->getStrVal());
	page.replace(F("{TN}"), parsp[p(THICKNESS)]->getStrVal());
	page.replace(F("{SR}"), parsp[p(SLATSRATIO)]->getStrVal());
	page.replace(F("{NM}"), String(getNmax()));
	page.replace(F("{PD}"), String(round(getPosdelta())));
	page.replace(F("{TP}"), parsp[MQTTTEMP]->getStrJsonName());
	page.replace(F("{DT}"), parsp[MQTTDATE]->getStrJsonName());
#if (AUTOCAL_HLW8012) 
	page.replace(F("{PW}"), parsp[INSTPWR]->getStrJsonName());
	page.replace(F("{AC}"), parsp[INSTACV]->getStrJsonName());
#endif

	//Body placeholders
	//DEBUG_PRINTLN(page);
	//DEBUG_PRINTLN(F("Scrittura cookie handleMQTTConf "));
	serverp.sendHeader("Cache-Control", "no-cache");
	serverp.sendHeader("Set-Cookie", "ESPSESSIONID=0");
	serverp.sendHeader("Connection", "close");
	serverp.send(200, "text/html", page);
}

inline void savegroup(uint8_t fields[], uint8_t len){
	for(int i=0; i<len; i++){
		saveParamFromForm(fields[i]);		//save param on eeprom
		parsp[fields[i]]->doaction();		//execute onreceive param event manager
	}
}

void handleModify(){
	startPageLoad();
  
  if (!is_authentified(serverp)) {
		serverp.sendHeader("Location", "/");
		serverp.sendHeader("Cache-Control", "no-cache");
		serverp.send(301);
		return;
  }
  
  eepromBegin();
  if(serverp.hasArg("svwifi")){
	  DEBUG_PRINTLN(F("savegroup svwifi"));
	  uint8_t fields[6] ={p(APPSSID), p(APPPSW), p(CLNTSSID1), p(CLNTPSW1), p(CLNTSSID2), p(CLNTPSW2)};
	  savegroup(fields, 6);
  }else if(serverp.hasArg("svmqtt")){
	  DEBUG_PRINTLN(F("savegroup svmqtt"));
	  uint8_t fields[9] ={p(MQTTADDR), p(MQTTID), p(MQTTOUTTOPIC), p(MQTTINTOPIC), p(MQTTUSR), p(MQTTPSW), p(MQTTPORT), p(WSPORT), p(MQTTPROTO)};
	  savegroup(fields, 9);
  }else if(serverp.hasArg("svsystem")){
	  DEBUG_PRINTLN(F("savegroup svsystem"));
#if (AUTOCAL_HLW8012) 
	  uint8_t fields[11] ={p(WEBUSR), p(WEBPSW), p(NTPADDR1), p(NTPADDR2), p(UTCSYNC), p(UTCADJ), p(UTCZONE), p(ACVOLT), p(CALPWR)};
	  savegroup(fields, 11);
#else
	  uint8_t fields[8] ={p(WEBUSR), p(WEBPSW), p(NTPADDR1), p(NTPADDR2), p(UTCSYNC), p(UTCADJ), p(UTCZONE)};
	  savegroup(fields, 8);
#endif	  
	  if( serverp.hasArg("rebootd") && String("y") == serverp.arg("rebootd") ){
		rebootSystem();
	  }
	  if( serverp.hasArg("reboot") && String("y") == serverp.arg("reboot") ){
		DEBUG_PRINTLN(F("Rebooting ESP"));
		ESP.restart();
	  }  
	  if( serverp.hasArg("utcsdt") && String("y") == serverp.arg("utcsdt") ){
		setSDT(true);
		updtConf(p(UTCSDT), String(1));
	  }else{
		setSDT(false);
		updtConf(p(UTCSDT), String(0));
	  }
  }else if(serverp.hasArg("svlogic")){
	  DEBUG_PRINTLN(F("savegroup svlogic"));
	  uint8_t fields[11] ={p(THALT1), p(THALT2), p(THALT3), p(THALT4), p(STDEL1), p(STDEL2), p(VALWEIGHT), p(TLENGTH), p(BARRELRAD), p(THICKNESS), p(SLATSRATIO)};
	  savegroup(fields, 11);
	  
	  if( serverp.hasArg("swroll1") && String("1") == serverp.arg("swroll1") ){
		//writeSWMode(0,0); 
		updtConf(p(SWROLL1), String("0"));
		setSWMode(0,0); 
	  }else{
		//writeSWMode(1,0);
		updtConf(p(SWROLL1), String("1"));
		setSWMode(1,0);	
	  }
	  if( serverp.hasArg("swroll2") && String("1") == serverp.arg("swroll2") ){
		setSWMode(0,1);
		updtConf(p(SWROLL2), String("0"));
	  }else{
		setSWMode(1,1);	
		updtConf(p(SWROLL2), String("1"));
	  }
  }else if(serverp.hasArg("svlog")){
	  uint8_t num = static_cast<ParUint8*>(parsp[p(LOGSLCT)])->val;
	  uint8_t oldNum = num;
		DEBUG_PRINT(F("{L1]: "));
		DEBUG_PRINTLN(String((num >> 0) & 0x3));
		
		DEBUG_PRINT(F("{L2]: "));
		DEBUG_PRINTLN(String((num >> 2) & 0x3));
		
		DEBUG_PRINT(F("{L3]: "));
		DEBUG_PRINTLN(String((num >> 4) & 0x3));
	  if( serverp.hasArg("serlog") && String((num >> 0) & 0x3) != serverp.arg("serlog") ){
		num -= (num >>0) & 0x3;
		num += serverp.arg("serlog").toInt();
	  } 
	  if( serverp.hasArg("tlntlog") && String((num >> 2) & 0x3) != serverp.arg("tlntlog") ){
		num -= ((num >> 2) & 0x3)*4;
		num += serverp.arg("tlntlog").toInt()*4;
	  }
	  if( serverp.hasArg("mqttlog") &&String((num >> 4) & 0x3) != serverp.arg("mqttlog") ){
		num -= ((num >> 4) & 0x3)*16; 
		num += serverp.arg("mqttlog").toInt()*16;
	  }
	  if(oldNum != num){
		  DEBUG_PRINT(F("num: "));
		  DEBUG_PRINTLN(String(num));
		  static_cast<ParUint8*>(parsp[p(LOGSLCT)])->load(num);
		  saveSingleConf(LOGSLCT);
		  static_cast<ParUint8*>(parsp[p(LOGSLCT)])->doaction();
	  }
  }
  EEPROM.end();
  
  DEBUG_PRINTLN(F("Disconnection"));
  
  serverp.sendHeader("Cache-Control", "no-cache");
  serverp.sendHeader("Set-Cookie", "ESPSESSIONID=1");
  
  String page = FPSTR(HTTP_FORM_SUCCESS);
  page.replace(F("{HD}"), FPSTR(HTTP_FORM_HEAD) );
  serverp.send(200, "text/html", page);
	
  if(static_cast<ParUint8*>(parsp[TIMINGCHANGED])->val == 1){
	parsp[TIMINGCHANGED]->load(0);
	initIiming(false);
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
		varStrOfst[0] = varStrOfst[1] = varStrOfst[2] = varStrOfst[3] = varStrOfst[4] = varStrOfst[5] = FIXEDPARAMSLEN;
		DEBUG_PRINT(F("FIXEDPARAMSLEN "));
		DEBUG_PRINTLN(FIXEDPARAMSLEN);
		
		saveOnEEPROM(FIXEDPARAMSLEN);
		
		parsp[p(CONFLOADED)]->load(0);
		delay(1000);
	} else {
		EEPROM.begin(THALT1OFST);//the next after EEPROMLENOFST
		int eepromlen = EEPROMReadInt(EEPROMLENOFST);
		EEPROM.end();
		EEPROM.begin(eepromlen+1);
		DEBUG_PRINT(F("eepromlen "));
		DEBUG_PRINTLN(eepromlen);
		
		DEBUG_PRINTLN(F("Reading all fixed params... "));
		for(int i=VARCONFDIM + USRMODIFICABLEFLAGS ; i<PARAMSDIM; i++){
			loadConf(i);
		}
		
		DEBUG_PRINT(F("Reading all variables params... "));
		varStrOfst[0] = FIXEDPARAMSLEN;
		for(i=0; i<VARCONFDIM; ++i){
				varStrOfst[i+1] = EEPROMReadStr(varStrOfst[i], buf) + varStrOfst[i];
				parsp[p(i)]->load(String(buf));
				DEBUG_PRINT(F("sensors CONFEXPR "));
				DEBUG_PRINT(i);
				DEBUG_PRINT(F(": "));
				DEBUG_PRINTLN(parsp[p(i)]->getStrVal());
		}
		
		parsp[p(CONFLOADED)]->load(1);
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
	char eprom, form;
	
	//I parametri devono poter essere modificati
	DEBUG_PRINTLN(F("Saving configuration...."));
  
	EEPROM.begin(len);	
	for(int i=VARCONFDIM + USRMODIFICABLEFLAGS; i<PARAMSDIM; i++){
		saveConf(i);
	}
	EEPROM.end();
	
	DEBUG_PRINTLN(F("Fixed length params saved...."));
	writeOnOffConditions();
	
	DEBUG_PRINTLN(F("Variable length params saved...."));
}

void writeOnOffConditions(){
	int i;
	
	DEBUG_PRINTLN(F("writeOnOffConditions"));
	eepromBegin();
	varStrOfst[0] = FIXEDPARAMSLEN;  
	for(i=0; i<VARCONFDIM; ++i){								
		varStrOfst[i+1] = EEPROMWriteStr(varStrOfst[i],	(parsp[p(i)]->getStrVal()).c_str() ) + varStrOfst[i];
		DEBUG_PRINT(F("Modified sensors CONFEXPR "));
		DEBUG_PRINT(i);
		DEBUG_PRINT(F(": "));
		DEBUG_PRINTLN(parsp[p(i)]->getStrVal());
	}
	EEPROMWriteInt(EEPROMLENOFST,varStrOfst[VARCONFDIM]);
	DEBUG_PRINT(F("Modified sensors EEPROMLENOFST: "));
	DEBUG_PRINTLN(varStrOfst[i]);
	EEPROM.end();
}
	
void printConfig(){
		int i;
		
		DEBUG_PRINT(F("\nPrinting EEPROM configuration...."));
		
		for(i=0; i<VARCONFDIM; ++i){
			DEBUG_PRINT(F("\nsensors CONFEXPR "));
			DEBUG_PRINT(i);
			DEBUG_PRINT(F(": "));
			DEBUG_PRINTLN(parsp[p(i)]->getStrVal());
			DEBUG_PRINT(F("- len "));
			DEBUG_PRINT(i);
			DEBUG_PRINT(F(": "));
			DEBUG_PRINT(varStrOfst[i]);
		}
		
		for(int i=0; i<PARAMSDIM; i++){
			printFixedParam(i);
		}

		DEBUG_PRINT(F("\neeprom EEPROMLENOFST: "));
		DEBUG_PRINT(varStrOfst[VARCONFDIM]);
}		

void saveSingleConf(unsigned confofst){
	eepromBegin();
	saveConf(p(confofst));
	EEPROM.end();
}

void saveSingleJson(unsigned jsnofst){
	eepromBegin();
	saveConf(jsnofst);
	EEPROM.end();
}

void updtConf(unsigned paramofst, String v){
	char intype = parsp[paramofst]->partype;
	
	DEBUG_PRINT(F("paramofst: "));
	DEBUG_PRINTLN(paramofst);
	if(parsp[paramofst] != NULL){
		DEBUG_PRINT(F("param: "));
		DEBUG_PRINTLN(parsp[paramofst]->formname);
		DEBUG_PRINT(F("jsoname: "));
		DEBUG_PRINTLN( parsp[paramofst]->jsoname);
		DEBUG_PRINT(F("eepromofst: "));
		DEBUG_PRINTLN(parsp[paramofst]->eprom);
		DEBUG_PRINT(F("partype: "));
		DEBUG_PRINTLN(intype);
		DEBUG_PRINT(F("confofst: "));
		DEBUG_PRINTLN(getConfofstFromParamofst(paramofst));
		DEBUG_PRINT(F("formfield: "));
		DEBUG_PRINTLN(parsp[paramofst]->formfield);
		
		if(intype == 'p' || intype == 'j'){
			parsp[paramofst]->loadFromStr(v);
			parsp[paramofst]->saveOnEprom();
			DEBUG_PRINT(F("Updated param: "));
			DEBUG_PRINT(parsp[paramofst]->formname);
			DEBUG_PRINT(F(": "));
			DEBUG_PRINTLN(parsp[paramofst]->getStrVal());	
		}
		DEBUG_PRINTLN(F("---------------------------------"));
	}
}

void saveConf(unsigned paramofst){
	char intype = parsp[paramofst]->partype;
	
	DEBUG_PRINT(F("paramofst: "));
	DEBUG_PRINTLN(paramofst);
	if(parsp[paramofst] != NULL){
		DEBUG_PRINT(F("param: "));
		DEBUG_PRINTLN( parsp[paramofst]->formname);
		DEBUG_PRINT(F("jsoname: "));
		DEBUG_PRINTLN( parsp[paramofst]->jsoname);
		DEBUG_PRINT(F("eepromofst: "));
		DEBUG_PRINTLN(parsp[paramofst]->eprom);
		DEBUG_PRINT(F("partype: "));
		DEBUG_PRINTLN(intype);
		DEBUG_PRINT(F("confofst: "));
		DEBUG_PRINTLN(getConfofstFromParamofst(paramofst));
		DEBUG_PRINT(F("formfield: "));
		DEBUG_PRINTLN( parsp[paramofst]->formfield);
		
		
		if(intype == 'p' || intype == 'j'){
			parsp[paramofst]->saveOnEprom();
			DEBUG_PRINT(F("Modified param: "));
			DEBUG_PRINT(parsp[paramofst]->formname);
			DEBUG_PRINT(F(": "));
			DEBUG_PRINTLN(parsp[paramofst]->getStrVal());			
		}
			
		DEBUG_PRINTLN(F("---------------------------------"));
	}
}

void loadConf(unsigned paramofst){
	char intype = parsp[paramofst]->partype;
	
	DEBUG_PRINT(F("paramofst: "));
	DEBUG_PRINTLN(paramofst);
	if(parsp[paramofst] != NULL){
		DEBUG_PRINT(F("param: "));
		DEBUG_PRINTLN(parsp[paramofst]->formname);
		DEBUG_PRINT(F("jsoname: "));
		DEBUG_PRINTLN( parsp[paramofst]->jsoname);
		DEBUG_PRINT(F("eepromofst: "));
		DEBUG_PRINTLN(parsp[paramofst]->eprom);
		DEBUG_PRINT(F("partype: "));
		DEBUG_PRINTLN(intype);
		DEBUG_PRINT(F("confofst: "));
		DEBUG_PRINTLN(getConfofstFromParamofst(paramofst));
		DEBUG_PRINT(F("formfield: "));
		DEBUG_PRINTLN( parsp[paramofst]->formfield);
		
		if(intype == 'p' || intype == 'j'){
			DEBUG_PRINT(F("Load param: "));
			parsp[paramofst]->loadFromEprom();
			DEBUG_PRINT(parsp[paramofst]->formname);
			DEBUG_PRINT(F(": "));
			DEBUG_PRINTLN(parsp[paramofst]->getStrVal());
		}
		DEBUG_PRINTLN(F("---------------------------------"));
	}
}

void printFixedParam(unsigned paramofst){
	DEBUG_PRINT(F("paramofst: "));
	DEBUG_PRINTLN(paramofst);
	if(parsp[paramofst] != NULL){
		DEBUG_PRINT(F("formname: "));
		DEBUG_PRINTLN( parsp[paramofst]->formname);
		DEBUG_PRINT(F("jsonname: "));
		DEBUG_PRINTLN( parsp[paramofst]->jsoname);
		DEBUG_PRINT(F("eepromofst: "));
		DEBUG_PRINTLN(parsp[paramofst]->eprom);
		DEBUG_PRINT(F("partype: "));
		DEBUG_PRINTLN(parsp[paramofst]->partype);
		DEBUG_PRINT(F("confofst: "));
		DEBUG_PRINTLN(getConfofstFromParamofst(paramofst));
		DEBUG_PRINT(F("formfield: "));
		DEBUG_PRINTLN(parsp[paramofst]->formfield);
		DEBUG_PRINT(F("parval: "));
		DEBUG_PRINTLN(parsp[paramofst]->getStrVal());
		DEBUG_PRINTLN(F("---------------------------------"));
	}
}

void saveParamFromForm(unsigned paramofst){
	char intype = parsp[paramofst]->partype;
	
	DEBUG_PRINT(F("paramofst: "));
	DEBUG_PRINTLN(paramofst);
	if(parsp[paramofst] != NULL){
		DEBUG_PRINT(F("param: "));
		DEBUG_PRINTLN( parsp[paramofst]->formname);
		DEBUG_PRINT(F("jsoname: "));
		DEBUG_PRINTLN( parsp[paramofst]->jsoname);
		DEBUG_PRINT(F("eepromofst: "));
		DEBUG_PRINTLN(parsp[paramofst]->eprom);
		DEBUG_PRINT(F("partype: "));
		DEBUG_PRINTLN(intype);
		DEBUG_PRINT(F("confofst: "));
		DEBUG_PRINTLN(getConfofstFromParamofst(paramofst));
		DEBUG_PRINT(F("formfield: "));
		DEBUG_PRINTLN( parsp[paramofst]->formfield);
		
		if(intype == 'i'){//form element input or text area
			String param = String(parsp[paramofst]->formname);
			if(intype == 'P'){
				if(serverp.hasArg(param) && (parsp[paramofst]->getStrVal() != serverp.arg(param)) ){
					parsp[paramofst]->loadFromStr(serverp.arg(param));
					parsp[paramofst]->saveOnEprom();
					DEBUG_PRINT(F("Updated param: "));
					DEBUG_PRINT(param);
					DEBUG_PRINT(F(": "));
					DEBUG_PRINTLN(parsp[paramofst]->getStrVal());
				}
			}else if(intype == 'j'){
				if(serverp.hasArg(param) && (parsp[paramofst]->getStrVal() != serverp.arg(param)) ){
					parsp[paramofst]->jsoname = (char *) (serverp.arg(param)).c_str();
					DEBUG_PRINT(F("Updated json name: "));
					DEBUG_PRINT(parsp[paramofst]->jsoname);
				}
			}	
		}
	}
	DEBUG_PRINTLN(F("---------------------------------"));
}

//conservata in eeprom, acqu,isita in form, sia nome che valore campo MQTT
Par::Par(const char* pname, const char* mjson, unsigned epromofst, char ptype, char ffield, BaseEvnt *evt){
	formname = (char *) pname;
	eprom = epromofst;
	formfield = ffield;
	partype = ptype;
	e = evt;
	jsoname = (char *) mjson;
}

void Par::doaction(){
	if(e!=NULL)
		e->doaction();
}

void Par::load(uint8_t f){};
void Par::load(int f){};
void Par::load(unsigned long f){};
void Par::load(float f){};
void Par::load(double f){};
void Par::load(char* f){};
void Par::load(String f){};

String Par::getStrJsonName(){
	return String(jsoname);
}
String Par::getStrFormName(){
	return String(formname);
}
uint8_t Par::getType(){
	return partype;
}

String ParUint8::getStrVal(){
	return String(this->val);
}
void ParUint8::writeParam(String str){
	uint8_t app = str.toInt();
	EEPROM.write(eprom, app);
}
void ParUint8::load(uint8_t b){
	this->val = b;
}
void ParUint8::loadFromEprom(){
	this->val = EEPROM.read(eprom);
}
void ParUint8::saveOnEprom(){
	EEPROM.write(eprom, this->val);
}
void ParUint8::loadFromStr(String str){
	this->val = str.toInt();
}


String ParInt::getStrVal(){
	return String(this->val);
}
void ParInt::writeParam(String str){
	EEPROMWriteInt(eprom, str.toInt());
}
void ParInt::loadFromStr(String str){
	this->val = str.toInt();
}
void ParInt::load(int i){
	this->val = i;
}
void ParInt::loadFromEprom(){
	this->val = EEPROMReadInt(eprom);
}
void ParInt::saveOnEprom(){
	EEPROMWriteInt(eprom, val);
}
	

String ParLong::getStrVal(){
	return String(this->val);
}
void ParLong::writeParam(String str){
	EEPROMWriteLong(eprom,strtoul(str.c_str(), NULL, 10));
}
void ParLong::loadFromStr(String str){
	this->val = strtoul(str.c_str(), NULL, 10);
}
void ParLong::load(unsigned long l){
	this->val = l;
}
void ParLong::loadFromEprom(){
	this->val = EEPROMReadLong(eprom);
}
void ParLong::saveOnEprom(){
	EEPROMWriteLong(eprom,this->val);
}


String ParFloat::getStrVal(){
	return String(this->val);
}
void ParFloat::writeParam(String str){
	EEPROMWriteFloat(eprom,str.toFloat());
}
void ParFloat::loadFromStr(String str){
	this->val = str.toFloat();
}
void ParFloat::load(float f){
	this->val = f;
}
void ParFloat::load(double d){
	this->val = (float) d;
}
void ParFloat::loadFromEprom(){
	this->val = EEPROMReadFloat(eprom);
}
void ParFloat::saveOnEprom(){
	EEPROMWriteFloat(eprom,this->val);
}


String ParStr32::getStrVal(){
	return String(this->val);
}
void ParStr32::writeParam(String str){
	EEPROMWriteStr(eprom,str.c_str(),32);
}
void ParStr32::loadFromStr(String str){
	str.toCharArray(this->val, 32);
}
void ParStr32::load(char *str){
	memcpy(this->val, str, 32);
}
void ParStr32::loadFromEprom(){
	EEPROMReadStr(eprom, this->val, 32);
}
void ParStr32::saveOnEprom(){
	EEPROMWriteStr(eprom,this->val,32);
}


String ParStr64::getStrVal(){
	return String(this->val);
}
void ParStr64::writeParam(String str){
	EEPROMWriteStr(eprom,str.c_str(),64);
}
void ParStr64::loadFromStr(String str){
	str.toCharArray(this->val, 64);
}
void ParStr64::load(char *str){
	memcpy(this->val, str, 64);
}
void ParStr64::loadFromEprom(){
	EEPROMReadStr(eprom, this->val, 64);
}
void ParStr64::saveOnEprom(){
	EEPROMWriteStr(eprom,this->val,64);
}


String ParVarStr::getStrVal(){
	return this->val;
}
void ParVarStr::writeParam(String str){
	this->val = str;
	writeOnOffConditions();
}
void ParVarStr::saveOnEprom(){
	writeOnOffConditions();
}
void ParVarStr::loadFromStr(String str){
	this->val = str;
	writeOnOffConditions();
}
void ParVarStr::load(String str){
	this->val = str;
}
void ParVarStr::loadFromEprom(){
	writeOnOffConditions();
}


uint8_t saveByteConf(unsigned confofst){
	saveConf(p(confofst));
	return (uint8_t) static_cast<ParUint8*>(parsp[p(confofst)])->val;
}

int saveIntConf(unsigned confofst){
	saveConf(p(confofst));
	return  static_cast<ParInt*>(parsp[p(confofst)])->val;
}

long saveLongConf( unsigned confofst){
	saveConf(p(confofst));
	return  static_cast<ParLong*>(parsp[p(confofst)])->val;
}

float saveFloatConf(unsigned confofst){
	saveConf(p(confofst));
	return  static_cast<ParFloat*>(parsp[p(confofst)])->val;
}

void BaseLog::print(char* topic,  char* msg){};
void BaseLog::println(char* topic,  char* msg){};

void BaseLog::append(char*result, char* topic, char* msg){
	strcpy(result,topic); // copy string one into the result.
	strcat(result,"]: "); // append string two to the result.
	strcat(result,msg); // append string two to the result.
}
//SerialLog
void SerialLog::print(char* topic,  char* msg){
	char result[100] = "[";
	append(result, topic, msg);
	Serial.print(result);
};
void SerialLog::println(char* topic,  char* msg){
	char result[100] = "[";
	append(result, topic, msg);
	Serial.println(result);
};
//TelnetLog
void TelnetLog::print(char* topic,  char* msg){
	char result[100] = "[";
	append(result, topic, msg);
	tel->println(result);
};
void TelnetLog::println(char* topic,  char* msg){
	char result[100] = "[";
	append(result, topic, msg);
	tel->print(result);
};
//MQTTLog
void MQTTLog::print(char* topic,  char* msg){
	char result[100] = "[";
	append(result, topic, msg);
	mqttc->publish((const char *) "log", (const char *) msg, strlen(msg));
};
void MQTTLog::println(char* topic,  char* msg){
	char result[100] = "[";
	append(result, topic, msg);
	strcat(result,"\n");
	mqttc->publish((const char *) "log", (const char *) msg, strlen(msg));
};
//SerialTelnetLog
void SerialTelnetLog::print(char* topic,  char* msg){
	char result[100] = "[";
	append(result, topic, msg);
	Serial.print(result);
	tel->print(result);
};
void SerialTelnetLog::println(char* topic,  char* msg){
	char result[100] = "[";
	append(result, topic, msg);
	Serial.println(result);
	tel->println(result);
};
//SerialMqttLog
void SerialMQTTLog::print(char* topic,  char* msg){
	char result[100] = "[";
	append(result, topic, msg);
	Serial.print(result);
	mqttc->publish((const char *) "log", (const char *) msg, strlen(result));
};
void SerialMQTTLog::println(char* topic,  char* msg){
	char result[100] = "[";
	append(result, topic, msg);
	Serial.println(result);
	strcat(result,"\n");
	Serial.print(result);
	mqttc->publish((const char *) "log", (const char *) msg, strlen(result));
};
//TelnetMQTTLog
void TelnetMQTTLog::print(char* topic,  char* msg){
	char result[100] = "[";
	append(result, topic, msg);
	tel->print(result);
	mqttc->publish((const char *) "log", (const char *) msg, strlen(result));
};
void TelnetMQTTLog::println(char* topic,  char* msg){
	char result[100] = "[";
	append(result, topic, msg);
	strcat(result,"\n");
	tel->print(result);
	mqttc->publish((const char *) "log", (const char *) msg, strlen(result));
};
//SerialTelnetMQTTLog
void SerialTelnetMQTTLog::print(char* topic,  char* msg){
	char result[100] = "[";
	append(result, topic, msg);
	Serial.print(result);
	tel->print(result);
	mqttc->publish((const char *) "log", (const char *) msg, strlen(result));
};
void SerialTelnetMQTTLog::println(char* topic,  char* msg){
	char result[100] = "[";
	append(result, topic, msg);
	strcat(result,"\n");
	Serial.print(result);
	tel->print(result);
	mqttc->publish((const char *) "log", (const char *) msg, strlen(result));
};

