#include "common.h"
#define serverp (*serveru)

ESP8266WebServer *serveru; 
String  *paramsp;
String  *mqttJsonp;

void initCommon(ESP8266WebServer *serveri,  String  *paramsi, String  *mqttJsoni){
	serveru=serveri; 
	paramsp=paramsi;
	mqttJsonp=mqttJsoni;
}

const char HTTP_FORM_ROOT[] PROGMEM =
	"<html>{HD}<body>"
	"<div class=\"container\">"
	"<div id=\"logo\"><img src=\"tapparella.png\" alt="" width=\"50px\" height=\"auto\"><h1>MyTapparella</h1></div>"
	"<div id=\"form\">"
		"<form action=\"/login\" method=\"POST\">"
			"<div class=\"col-1-2\">"
                "<label for=\"username\">Configuration web client username:</label>"
                "<input type=\"text\" name=\"username\" value=\"{WU}\">"
            "</div>"
            "<div class=\"col-1-2\"><label for=\"password\">Configuration web client password:</label>"
                 "<input type=\"password\" name=\"password\" placeholder=\"**********\">"
            "</div>"
			"<input type=\"submit\" value=\"Login\">"
		"</form>"
	"</div></div></body></html>";

const char HTTP_FORM_WIFI[] PROGMEM =	
	"<html>{HD}<body>"
	"<div class=\"container\">"
	"<div id=\"logo\"><img src=\"tapparella.png\" alt="" width=\"50px\" height=\"auto\"><h1>MyTapparella</h1></div>"
	"<div id=\"form\">"
        "<form action=\"/login\" method=\"POST\">"
            "<div class=\"col-1-2\"><label for=\"wificlientssid\">Wifi instructure 1 SSID:</label>"
                 "<input type=\"text\" name=\"wificlientssid1\" value=\"{S1}\">"
            "</div>"
			"<div class=\"col-1-2\"><label for=\"wificlientpsw\">Wifi instructure 1 password:</label>"
                 "<input type=\"password\" name=\"wificlientpsw1\" value=\"{P1}\">"
            "</div>"
			"<div class=\"col-1-2\"><label for=\"wificlientssid\">Wifi instructure 2 SSID:</label>"
                 "<input type=\"text\" name=\"wificlientssid2\" value=\"{S2}\">"
            "</div>"
			"<div class=\"col-1-2\"><label for=\"wificlientpsw\">Wifi instructure 2 password:</label>"
                 "<input type=\"password\" name=\"wificlientpsw2\" value=\"{P2}\">"
            "</div>"
            "<div class=\"col-1-2\"><label for=\"wifiapusr\">Wifi local AP SSID:</label>"
                 "<input type=\"text\" name=\"wifiapssid\" value=\"{AS}\">"
            "</div>"
            "<div class=\"col-1-2\"><label for=\"wifiappsw\">Wifi local AP password:</label>"
                 "<input type=\"password\" name=\"wifiAPPPSW\" value=\"{AP}\">"
            "</div>"
            "<input type=\"submit\" value=\"Save\" formaction=\"/modify\" formmethod=\"post\">"
			"<input type=\"submit\" value=\"Back\">"
        "</form>"
	"</div></div></body></html>";
	
const char HTTP_FORM_SYSTEM[] PROGMEM =	
	"<html>{HD}<body>"
	"<div class=\"container\">"
	"<div id=\"logo\"><img src=\"tapparella.png\" alt="" width=\"50px\" height=\"auto\"><h1>MyTapparella</h1></div>"
	"<div id=\"form\">"
        "<form action=\"/login\" method=\"POST\">"
            "<div class=\"col-1-2\">"
                "<label for=\"username\">Configuration web client username:</label>"
                 "<input type=\"text\" name=\"username\" value=\"{WU}\">"
            "</div>"
            "<div class=\"col-1-2\"><label for=\"password\">Configuration web client password:</label>"
                 "<input type=\"password\" name=\"password\" value=\"{WP}\">"
            "</div>"
            "<div class=\"col-1-2\"><label for=\"reboot\">Reboot the system with default config</label>"
                 "<input type=\"checkbox\" name=\"rebootd\" value=\"y\">"
            "</div>"
			"<div class=\"col-1-2\"><label for=\"reboot\">Reboot the system</label>"
                 "<input type=\"checkbox\" name=\"reboot\" value=\"y\">"
            "</div>"
#if (!AUTOCAL) 
			"<div class=\"col-1-2\">"
                "<label for=\startdelay\">Start delay on click of group 1:</label>"
                 "<input type=\"text\" name=\"startdelay1\" value=\"{S1}\">"
            "</div>"
			"<div class=\"col-1-2\">"
                "<label for=\startdelay\">Start delay on click of group 2:</label>"
                 "<input type=\"text\" name=\"startdelay2\" value=\"{S2}\">"
            "</div>"
#endif
            "<input type=\"submit\" value=\"Save\" formaction=\"/modify\" formmethod=\"post\">"
			"<input type=\"submit\" value=\"Back\">"
        "</form>"
	"</div></div></body></html>";
	
const char HTTP_FORM_MQTT[] PROGMEM =	
	"<html>{HD}<body>"
	"<div class=\"container\">"
	"<div id=\"logo\"><img src=\"tapparella.png\" alt="" width=\"50px\" height=\"auto\"><h1>MyTapparella</h1></div>"
	"<div id=\"form\">"
        "<form action=\"/login\" method=\"POST\">"
			"<div class=\"col-1-2\"><label for=\"mqttserver\">MQTT server:</label>"
                 "<input type=\"text\" name=\"mqttserver\" value=\"{MA}\">"
            "</div>"
			"<div class=\"col-1-2\"><label for=\"mqttclientid\">MQTT Client ID:</label>"
                 "<input type=\"text\" name=\"mqttclientid\" value=\"{MI}\">"
            "</div>"
            "<div class=\"col-1-2\"><label for=\"mqttusr\">MQTT user name:</label>"
                 "<input type=\"text\" name=\"mqttusr\" value=\"{MU}\">"
            "</div>"
            "<div class=\"col-1-2\"><label for=\"mqttpsw\">MQTT user password:</label>"
                 "<input type=\"text\" name=\"mqttpsw\" value=\"{MP}\">"
            "</div>"
			"<div class=\"col-1-2\"><label for=\"intopic\">In topic:</label>"
                 "<input type=\"text\" name=\"intopic\" value=\"{QI}\">"
            "</div>"
            "<div class=\"col-1-2\"><label for=\"outtopic\">Out topic:</label>"
                 "<input type=\"text\" name=\"outtopic\" value=\"{MO}\">"
            "</div>"
			"<div class=\"col-1-2\"><label for=\"btnup1\">MQTT message button 1 UP:</label>"
                 "<input type=\"text\" name=\"btnup1\" value=\"{J1}\">"
            "</div>"
			"<div class=\"col-1-2\"><label for=\"btndown1\">MQTT message button 1 DOWN:</label>"
                 "<input type=\"text\" name=\"btndown1\" value=\"{J2}\">"
            "</div>"
            "<div class=\"col-1-2\"><label for=\"btnup2\">MQTT message button 2 UP:</label>"
                 "<input type=\"text\" name=\"btnup2\" value=\"{J3}\">"
            "</div>"
			"<div class=\"col-1-2\"><label for=\"btndown2\">MQTT message button 2 DOWN:</label>"
                 "<input type=\"text\" name=\"btndown2\" value=\"{J4}\">"
            "</div>"
			"<div class=\"col-1-2\"><label for=\"btntemp\">Temperature request button:</label>"
                 "<input type=\"text\" name=\"btntemp\" value=\"{J5}\">"
            "</div>"
			"<div class=\"col-1-2\"><label for=\"btnmeanpwr\">Mean power request button:</label>"
                 "<input type=\"text\" name=\"btnmeanpwr\" value=\"{J6}\">"
            "</div>"
			"<div class=\"col-1-2\"><label for=\"btnpeakpwr\">Peak power request button:</label>"
                 "<input type=\"text\" name=\"btnpeakpwr\" value=\"{J7}\">"
            "</div>"
			"<div class=\"col-1-2\"><label for=\"btnall\">All states request button:</label>"
                 "<input type=\"text\" name=\"btnall\" value=\"{J8}\">"
            "</div>"
			"<input type=\"submit\" value=\"Save\" formaction=\"/modify\" formmethod=\"post\">"
			"<input type=\"submit\" value=\"Back\">"
        "</form>"
	"</div></div></body></html>";
	
const char HTTP_FORM_LOGIC[] PROGMEM =	
	"<html>{HD}<body>"
	"<div class=\"container\">"
	"<div id=\"logo\"><img src=\"tapparella.png\" alt="" width=\"50px\" height=\"auto\"><h1>MyTapparella</h1></div>"
	"<div id=\"form\">"
        "<form action=\"/login\" method=\"POST\">"
			"<div class=\"col-1-2\"><label for=\"thalt1\">End-of-stroke time switch 1:</label>"
                 "<input type=\"text\" name=\"thalt1\" value=\"{TU}\">"
            "</div>"
            "<div class=\"col-1-2\"><label for=\"thalt2\">End-of-stroke time switch 2:</label>"
                 "<input type=\"text\" name=\"thalt2\" value=\"{TD}\">"
            "</div>"
			"<div class=\"col-1-2\"><label for=\"taplength\">Rollershutter excursion:</label>"
                 "<input type=\"text\" name=\"taplength\" value=\"{TL}\">"
            "</div>"
            "<div class=\"col-1-2\"><label for=\"barrelrad\">Barrel radius:</label>"
                 "<input type=\"text\" name=\"barrelrad\" value=\"{BR}\">"
            "</div>"
			"<div class=\"col-1-2\"><label for=\"thickness\">Roller shutter thickness</label>"
                 "<input type=\"text\" name=\"thickness\" value=\"{TN}\">"
            "</div>"
			"<div class=\"col-1-2\"><label for=\"slatsratio\">Roller shutter slatsratio</label>"
                 "<input type=\"text\" name=\"slatsratio\" value=\"{SR}\">"
            "</div>"
            "<input type=\"submit\" value=\"Save\" formaction=\"/modify\" formmethod=\"post\">"
			"<input type=\"submit\" value=\"Back\">"
        "</form>"
	"</div></div></body></html>";
	
const char HTTP_FORM_LOGIN[] PROGMEM =	
	"<html>{HD}<body>"
	"<div class=\"container\">"
		"<div id=\"logo\"><h1>MyTapparella</h1></div>"
		"<div id=\"form\">"
			"<form action=\"/login\" method=\"POST\">"
				"<input type=\"submit\" formaction=\"/systconf\" formmethod=\"post\" value=\"System configuration\">"
				"<input type=\"submit\" formaction=\"/wificonf\" formmethod=\"post\" value=\"WiFi configuration\">"
				"<br><input type=\"submit\" formaction=\"/mqttconf\" formmethod=\"post\" value=\"MQTT configuration\">"
				"<br><input type=\"submit\" formaction=\"/logicconf\" formmethod=\"post\" value=\"Logic configuration\">"
				"<br><input type=\"submit\" value=\"Exit\" name=\"disconnect\">"
			"</form>"
	"</div></div></body></html>";
	
const char HTTP_FORM_SUCCESS[] PROGMEM =	
	"<html>{HD}<body>"
	"<div class=\"container\">"
	"<div id=\"logo\"><img src=\"tapparella.png\" alt="" width=\"50px\" height=\"auto\"><h1>MyTapparella</h1></div>"
	"<div id=\"form\">"
		"<form action=\"/login\" method=\"POST\">"
			"<div class=\"col-1-1\">"
                "<label><h1><p>Modifica avvenuta con successo.</p></label>"
            "</div>"
			"<input type=\"submit\" value=\"Back\">"
		"</form>"
	"</div></div></body></html>";
	
const char HTTP_FORM_CMD[] PROGMEM =	
	//{"OUTSLED":"0","up1":"1","down1":"0","up2":"50","down2":"0"}
	"<html>{HD}<body>"
	"<div class='container'>"
		"<div id='logo'><h1>MyTapparella</h1></div>"
		"<div id='form'>"
			"<form>"
				"<input id='up1' type='button' value='Button 1 UP' onclick='press(vls[0])' onmousedown='this.style.opacity=\"1\"' onmouseup='this.style.opacity=\"0.6\"' ontouchstart='this.style.opacity=\"1\"' ontouchend='this.style.opacity=\"0.6\"'>"
				"<br>"
				"<input id='down1' type='button' value='Button 1 DOWN' onclick='press(vls[1])' onmousedown='this.style.opacity=\"1\"' onmouseup='this.style.opacity=\"0.6\"' ontouchstart='this.style.opacity=\"1\"' ontouchend='this.style.opacity=\"0.6\"'>"
				"<br><br><br>"
				"<input id='up2' type='button' value='Button 2 UP' onclick='press(vls[2])' onmouseup='this.style.opacity=\"0.6\"' ontouchstart='this.style.opacity=\"1\"' ontouchend='this.style.opacity=\"0.6\"'>"
				"<br>"
				"<input id='down2' type='button' value='Button 2 DOWN' onclick='press(vls[3])' onmouseup='this.style.opacity=\"0.6\"' ontouchstart='this.style.opacity=\"1\"' ontouchend='this.style.opacity=\"0.6\"'>"
			"</form>"
			//"<p id='p'></p>"
		"</div>"
		"<div class='sldcont1'>"
			"<input type='range' min='0' max='100' value='50' class='slider' id='rng1'>"
			"<p>Group 1: <span id='val1'></span></p>"
		"</div>"
		"<meter id='pr1' low='{PD}' min='0' max='100' ></meter>"
		"<div class='sldcont2'>"
			"<input type='range' min='0' max='100' value='50' class='slider' id='rng2'>"
			"<p>Group 2: <span id='val2'></span></p>"
		"</div>"
		"<meter id='pr2' low='{PD}' min='0' max='100'></meter>"
	"</div>"
	"<script>"
		"var a=[0,0];"
		"var updt=[0,0];"
		"var p=[0,0];"
		"var tr=[0,0];"
		//"var c=[0,0];"
		"var dir=[0,0];"
		"var ie=[0,0,0,0];"
		"var sld1 = document.getElementById('rng1');"
		"var o1 = document.getElementById('val1');"
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
		"function press(s){"
			"conn.send(s);"
		"};"
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
			"p.value=calcLen(t,l);"
			"console.log('++++++++++++++');"
			"console.log('t:'+t+' p.value:'+p.value);"
			"updt[n]=setInterval(function(){"
				"if(dir[n]!=0){"
					"t=t+e;"
					"p.value=calcLen(t-delay*dir[n],l);"
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
/*	
const char HTTP_FORM_CMD[] PROGMEM =	
	//{"OUTSLED":"0","up1":"1","down1":"0","up2":"50","down2":"0"}
	"<html>{HD}<body>"
	"<div class='container'>"
		"<div id='logo'><h1>MyTapparella</h1></div>"
		"<div id='form'>"
			"<form>"
				"<input id='up1' type='button' value='Button 1 UP' onclick=\"pressed('%7B%22up1%22%3A%221%22%7D',feedback)\">"
				"<br><input id='down1' type='button' value='Button 1 DOWN' onclick=\"pressed('%7B%22down1%22%3A%221%22%7D',feedback)\">"
				"<br><br><br>"
				"<input id='up2' type='button' value='Button 2 UP' onclick=\"pressed('%7B%22up2%22%3A%221%22%7D',feedback)\">"
				"<br><input id='down2' type='button' value='Button 2 DOWN' onclick=\"pressed('%7B%22down2%22%3A%221%22%7D',feedback)\">"
			"</form>"
			"<p id='p'></p>"
	"</div></div>"
	"{SH}"
	"<script>"
		"function onRcv(e){"
			"feedback(e.data);"
		"};"
		"function pressed(btn, cfunc) {"
			"var xhttp;"
			"xhttp=new XMLHttpRequest();"
			"xhttp.onreadystatechange = function() {"
				"if (xhttp.readyState == 4 && xhttp.status == 200) {"
					"cfunc(xhttp.responseText);"
					"}"
				"};"
			"xhttp.open(\"GET\",'/cmdjson?cmds='+btn, true);"
			"xhttp.send();"
		"};"
		"function feedback(rspTxt) {"
			"var obj = JSON.parse(rspTxt);"
			"document.getElementById('p').innerHTML = rspTxt;"
			"for(x in obj){"
				"var el = document.getElementById(x);"
				"if(el){"  //controlla se il campo esiste nel DOM della pagina
					//"console.log(x);"
					"if(obj[x] == \"1\"){"
						"el.style.backgroundColor = \"#b30000\";"
					"}else{"
						"el.style.backgroundColor = \"#00ccff\";"
					"}"
				"}"	
			"}"
		"};"
		//"var intervalID = setInterval(function(){pressed('',feedback);}, 1000);"
	"</script>"
	"</body></html>;";*/

const char HTTP_FORM_HEAD[] PROGMEM =
"<head><meta charset=\"UTF-8\"><title>Document</title>"
"<style type=\"text/css\">"
    "* {"
        "box-sizing: border-box;"
    "}"
    "html, body {"
        "font-family: \"arial\", \"helvetica\", serif;"
        "font-size: 1.1rem;"
        "color: #fff;"
        "background-color: #333;"
    "}"
    ".container {"
        "position: absolute;"
        "top:0;"
        "bottom: 0;"
        "right: 0;"
        "left:0;"
		//"border: 2px solid yellow;"
    "}"
    "#form {"
        "margin-top: 20px;"
        "display: flex;"
        "flex-direction:column;"
        "justify-content: center;"
        "align-items: center;"
        "align-self: center;"
        "align-content: center;"
    "}"
    "#logo {"
        "text-align: center;"

    "}"
    "h1 {"
        "display:inline-block;"
    "}"
    ".col-1-2 {"
        "margin: 0 1%;"
        "width: {WT}%;"
        "float: left;"
        "padding-bottom: 2%;"
		//"border: 2px solid red;"
    "}"
	".col-1-1 {"
            "margin: auto 1%;"
            "width: 100%;"
        "}"
    "label{"
        "margin-left: 5%;"
        "color: #0099cc;"
    "}"
    "input {"
        "width: 100%;"
        "min-height: 2.4rem;"
        "padding-left: 5%;"
        "border-radius: 25px;"
        "border: 1px solid #0099cc;"
		"font-size: 1.1rem;"
    "}"
	"meter{"
        "width: 100%;"
    "}"
    "input[type=\"submit\"],input[type=\"button\"]{"
        "background-color: #333;"
        "color: #fff;"
        "max-width: 40%;"
		"min-width: 550px;"
        "border-radius: 25px;"
        "margin: 20px 30%;"
        "font-size: 1.5rem;"
        "padding-left: 0px;"
        "min-height: 2.8rem;"
        "background-color: #00ccff;"
        "border: 3px solid #0099cc;"
		"opacity: 0.6;"
		"transition: 0.3s;"
    "}"
    "@media screen and (max-width: 1100px) {"
        ".col-1-2 {"
            "margin: 0 1%;"
            "width: 48%;"
        "}"
        "input[type=\"submit\"],input[type=\"button\"]{"
           //"max-width: 100%;"
		   "max-width: 60%;"
		   "min-width: 550px;"
           "margin: 20px 25%;"
        "}"
    "}"
    "@media screen and (max-width: 700px),(max-width: 1440px) and (orientation: portrait) {"
        ".col-1-2 {"
            "margin: 0 2%;"
            "width: 96%;"
            "padding-bottom: 2%;"
			 "font-size: 2.5rem;"
        "}"
		"input {"
			"width: 100%;"
			"min-height: 4.4rem;"
			"font-size: 2.5rem;"
		"}"
        "input[type=\"submit\"],input[type=\"button\"]{"
            "max-width: 100%;"
            "margin: 20px 2%;"
			"min-height: 4.4rem;"
			"font-size: 2.5rem;"
        "}"
    "}"
	"</style>"
"</head>";

const char HTTP_WEBSOCKET[] PROGMEM =
		"var vls = ['{\"up1\":\"255\"}','{\"down1\":\"255\"}','{\"up2\":\"255\"}','{\"down2\":\"255\"}'];"
		"var vlsp = ['{\"up1\":\"N\"}','{\"up2\":\"N\"}'];"
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
		page.replace(F("{HD}"), FPSTR(HTTP_FORM_HEAD) );
		page.replace(F("{WT}"), F("31.333333") );
		//Body placeholders
		page.replace(F("{MA}"), paramsp[MQTTADDR] );
		page.replace(F("{MU}"), paramsp[MQTTUSR] );
		page.replace(F("{MP}"), paramsp[MQTTPSW] );
		page.replace(F("{MI}"), paramsp[MQTTID] );
		page.replace(F("{MO}"), paramsp[MQTTOUTTOPIC] );
		page.replace(F("{QI}"), paramsp[MQTTINTOPIC] );
		page.replace(F("{J1}"), mqttJsonp[MQTTJSONUP1] );
		page.replace(F("{J2}"), mqttJsonp[MQTTJSONDOWN1] );
		page.replace(F("{J3}"), mqttJsonp[MQTTJSONUP2] );
		page.replace(F("{J4}"), mqttJsonp[MQTTJSONDOWN2] );
		page.replace(F("{J5}"), mqttJsonp[MQTTJSONTEMP] );
		page.replace(F("{J6}"), mqttJsonp[MQTTJSONMEANPWR] );
		page.replace(F("{J7}"), mqttJsonp[MQTTJSONPEAKPWR] );
		page.replace(F("{J8}"), mqttJsonp[MQTTJSONALL] );
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
		page.replace(F("{TU}"), paramsp[THALT1] );
		page.replace(F("{TD}"), paramsp[THALT2] );
		page.replace(F("{TL}"), paramsp[TLENGTH] );
		page.replace(F("{BR}"), paramsp[BARRELRAD] );
		page.replace(F("{TN}"), paramsp[THICKNESS] );
		page.replace(F("{SR}"), paramsp[SLATSRATIO] );
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

void handleCmd() {  // If a POST request is made to URI /login

	String page = FPSTR(HTTP_FORM_CMD);
	//Head placeholders
	page.replace(F("{HD}"), FPSTR(HTTP_FORM_HEAD) );
	page.replace(F("{SH}"), HTTP_WEBSOCKET );
	page.replace(F("{WS}"), paramsp[LOCALIP]);
	page.replace(F("{TL}"), String(getTaplen()));
	page.replace(F("{BR}"), paramsp[BARRELRAD]);
	page.replace(F("{TN}"), paramsp[THICKNESS]);
	page.replace(F("{SR}"), paramsp[SLATSRATIO]);
	page.replace(F("{NM}"), String(getNmax()));
	page.replace(F("{PD}"), String(getPosdelta()));
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
  
  EEPROM.begin(EEPROMPARAMSLEN);
  
  if(serverp.hasArg("mqttserver") && paramsp[MQTTADDR] != serverp.arg("mqttserver") ){
	paramsp[MQTTADDR]=serverp.arg("mqttserver");
	EEPROMWriteStr(MQTTADDROFST,(paramsp[MQTTADDR]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified mqttAddr "));
	DEBUG_PRINTLN(paramsp[MQTTADDR]);
	//indirizzo MQTT cambiato!
	paramsp[MQTTADDRMODFIED]="true";
  }
  if(serverp.hasArg("mqttclientid") && paramsp[MQTTID] != serverp.arg("mqttclientid") ){
	paramsp[MQTTID]=serverp.arg("mqttclientid");
	EEPROMWriteStr(MQTTIDOFST,(paramsp[MQTTID]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified mqttID "));
	DEBUG_PRINTLN(paramsp[MQTTID]);
  }
  if(serverp.hasArg("outtopic") && (paramsp[MQTTOUTTOPIC] != serverp.arg("outtopic")) ){
	paramsp[MQTTOUTTOPIC]=serverp.arg("outtopic");
	EEPROMWriteStr(OUTTOPICOFST,(paramsp[MQTTOUTTOPIC]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified mqttOutTopic "));
	DEBUG_PRINTLN(paramsp[MQTTOUTTOPIC]);
	//Topic MQTT cambiato!
	paramsp[TOPICCHANGED]="true";
  }
  if(serverp.hasArg("intopic") && paramsp[MQTTINTOPIC] != serverp.arg("intopic") ){
	paramsp[MQTTINTOPIC]=serverp.arg("intopic");
	EEPROMWriteStr(INTOPICOFST,(paramsp[MQTTINTOPIC]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified mqttInTopic "));
	DEBUG_PRINTLN(paramsp[MQTTINTOPIC]);
	//Topic MQTT cambiato!
	paramsp[TOPICCHANGED]="true";
  }
  if(serverp.hasArg("btnup1") && paramsp[MQTTJSONUP1] != serverp.arg("btnup1") ){
	mqttJsonp[MQTTJSONUP1] = serverp.arg("btnup1");
	EEPROMWriteStr(MQTTJSONUP1OFST,(mqttJsonp[MQTTJSONUP1]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified mqttJsonp[1] "));
	DEBUG_PRINTLN(mqttJsonp[MQTTJSONUP1]);
  }
  if(serverp.hasArg("btndown1") && paramsp[MQTTJSONDOWN1] != serverp.arg("btndown1") ){
	mqttJsonp[MQTTJSONDOWN1] = serverp.arg("btndown1");
	EEPROMWriteStr(MQTTJSONDOWN1OFST,(mqttJsonp[MQTTJSONDOWN1]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified mqttJsonp[2] "));
	DEBUG_PRINTLN(mqttJsonp[MQTTJSONDOWN1]);
  }
  if(serverp.hasArg("btnup2") && paramsp[MQTTJSONUP2] != serverp.arg("btnup2") ){
	mqttJsonp[MQTTJSONUP2] = serverp.arg("btnup2");
	EEPROMWriteStr(MQTTJSONUP2OFST,(mqttJsonp[MQTTJSONUP2]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified mqttJsonp[3] "));
	DEBUG_PRINTLN(mqttJsonp[MQTTJSONUP2]);
  }
  if(serverp.hasArg("btndown2") && paramsp[MQTTJSONDOWN2] != serverp.arg("btndown2") ){ 
	mqttJsonp[MQTTJSONDOWN2] = serverp.arg("btndown2");
	EEPROMWriteStr(MQTTJSONDOWN2OFST,(mqttJsonp[MQTTJSONDOWN2]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified mqttJsonp[4] "));
	DEBUG_PRINTLN(mqttJsonp[MQTTJSONDOWN2]);
  }
  if(serverp.hasArg("btntemp") && paramsp[MQTTJSONTEMP] != serverp.arg("btntemp") ){ 
	mqttJsonp[MQTTJSONTEMP] = serverp.arg("btndown2");
	EEPROMWriteStr(MQTTJSONTEMPOFST,(mqttJsonp[MQTTJSONTEMP]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified mqttJsonp btntemp "));
	DEBUG_PRINTLN(mqttJsonp[MQTTJSONTEMP]);
  }
  if(serverp.hasArg("btnmeanpwr") && paramsp[MQTTJSONMEANPWR] != serverp.arg("btnmeanpwr") ){ 
	mqttJsonp[MQTTJSONMEANPWR] = serverp.arg("btnmeanpwr");
	EEPROMWriteStr(MQTTJSONMEANPWROFST,(mqttJsonp[MQTTJSONMEANPWR]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified mqttJsonp btnmeanpwr "));
	DEBUG_PRINTLN(mqttJsonp[MQTTJSONMEANPWR]);
  }
  if(serverp.hasArg("btnpeakpwr") && paramsp[MQTTJSONPEAKPWR] != serverp.arg("btnpeakpwr") ){ 
	mqttJsonp[MQTTJSONPEAKPWR] = serverp.arg("btnpeakpwr");
	EEPROMWriteStr(MQTTJSONPEAKPWROFST,(mqttJsonp[MQTTJSONPEAKPWR]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified mqttJsonp btnpeakpwr "));
	DEBUG_PRINTLN(mqttJsonp[MQTTJSONPEAKPWR]);
  }
  
  if(serverp.hasArg("btnall") && paramsp[MQTTJSONALL] != serverp.arg("btnall") ){ 
	mqttJsonp[MQTTJSONALL] = serverp.arg("btndown2");
	EEPROMWriteStr(MQTTJSONALLOFST,(mqttJsonp[MQTTJSONALL]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified mqttJsonp btnall "));
	DEBUG_PRINTLN(mqttJsonp[MQTTJSONALL]);
  }
  if(serverp.hasArg("wificlientssid1") && paramsp[CLNTSSID1] != serverp.arg("wificlientssid1") ){
	paramsp[CLNTSSID1]=serverp.arg("wificlientssid1");
	EEPROMWriteStr(WIFICLIENTSSIDOFST1,(paramsp[CLNTSSID1]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified clntSsid1 "));
	DEBUG_PRINTLN(paramsp[CLNTSSID1]);
	paramsp[WIFICHANGED]="true";
  }
  if(serverp.hasArg("wificlientpsw1") && paramsp[CLNTPSW1] != serverp.arg("wificlientpsw1") ){
	paramsp[CLNTPSW1]=serverp.arg("wificlientpsw1");
	EEPROMWriteStr(WIFICLIENTPSWOFST1,(paramsp[CLNTPSW1]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified clntPsw1 "));
	DEBUG_PRINTLN(paramsp[CLNTPSW1]);
	paramsp[WIFICHANGED]="true";
  }
  if(serverp.hasArg("wificlientssid2") && paramsp[CLNTSSID2] != serverp.arg("wificlientssid2") ){
	paramsp[CLNTSSID2]=serverp.arg("wificlientssid2");
	EEPROMWriteStr(WIFICLIENTSSIDOFST2,(paramsp[CLNTSSID2]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified clntSsid2 "));
	DEBUG_PRINTLN(paramsp[CLNTSSID2]);
	paramsp[WIFICHANGED]="true";
  }
  if(serverp.hasArg("wificlientpsw2") && paramsp[CLNTPSW2] != serverp.arg("wificlientpsw2") ){
	paramsp[CLNTPSW2]=serverp.arg("wificlientpsw2");
	EEPROMWriteStr(WIFICLIENTPSWOFST2,(paramsp[CLNTPSW2]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified clntPsw2 "));
	DEBUG_PRINTLN(paramsp[CLNTPSW2]);
	paramsp[WIFICHANGED]="true";
  }
  if(serverp.hasArg("wifiapssid") && paramsp[APPSSID] != serverp.arg("wifiapssid") ){
	paramsp[APPSSID]=serverp.arg("wifiapssid");
	EEPROMWriteStr(WIFIAPSSIDOFST,(paramsp[APPSSID]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified APSsid "));
	DEBUG_PRINTLN(paramsp[APPSSID]);
	paramsp[WIFICHANGED]="true";
  }
  if(serverp.hasArg("wifiAPPPSW") && paramsp[APPPSW] != serverp.arg("wifiAPPPSW") ){
	paramsp[APPPSW]=serverp.arg("wifiAPPPSW");
	EEPROMWriteStr(WIFIAPPPSWOFST,(paramsp[APPPSW]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified APPPSW "));
	DEBUG_PRINTLN(paramsp[APPPSW]);
	paramsp[WIFICHANGED]="true";
  }
  if(serverp.hasArg("username") && paramsp[WEBUSR] != serverp.arg("username") ){
	paramsp[WEBUSR]=serverp.arg("username");
	EEPROMWriteStr(WEBUSROFST,(paramsp[WEBUSR]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified webUsr "));
	DEBUG_PRINTLN(paramsp[WEBUSR]);
  }
  if(serverp.hasArg("username") && paramsp[WEBPSW] != serverp.arg("username") ){
	paramsp[WEBPSW]=serverp.arg("password");
	EEPROMWriteStr(WEBPSWOFST,(paramsp[WEBPSW]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified webPsw "));
	DEBUG_PRINTLN(paramsp[WEBPSW]);
  } 
  if(serverp.hasArg("mqttusr") && paramsp[MQTTUSR] != serverp.arg("mqttusr") ){
	paramsp[MQTTUSR]=serverp.arg("mqttusr");
	EEPROMWriteStr(MQTTUSROFST,(paramsp[MQTTUSR]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified MQTT username "));
	DEBUG_PRINTLN(paramsp[MQTTUSR]);
	paramsp[MQTTCONNCHANGED]="true";
  } 
  if(serverp.hasArg("mqttpsw") && paramsp[MQTTPSW] != serverp.arg("mqttpsw") ){
	paramsp[MQTTPSW]=serverp.arg("mqttpsw");
	EEPROMWriteStr(MQTTPSWOFST,(paramsp[MQTTPSW]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified MQTT password "));
	DEBUG_PRINTLN(paramsp[MQTTPSW]);
	paramsp[MQTTCONNCHANGED]="true";
  } 
#if (!AUTOCAL) 
  if(serverp.hasArg("startdelay1") && paramsp[STDEL1] != serverp.arg("startdelay1") ){
	paramsp[STDEL1]=serverp.arg("startdelay1");
	EEPROMWriteStr(STDEL1OFST,(paramsp[STDEL1]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Motor start delay 1"));
	DEBUG_PRINTLN(paramsp[STDEL1]);
	paramsp[TIMINGCHANGED]="true";
  } 
  if(serverp.hasArg("startdelay2") && paramsp[STDEL2] != serverp.arg("startdelay2") ){
	paramsp[STDEL2]=serverp.arg("startdelay2");
	EEPROMWriteStr(STDEL2OFST,(paramsp[STDEL2]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Motor start delay 2"));
	DEBUG_PRINTLN(paramsp[STDEL2]);
	paramsp[TIMINGCHANGED]="true";
  } 
#endif
  if(serverp.hasArg("thalt1") && (paramsp[THALT1] != String(serverp.arg("thalt1"))) ){
	paramsp[THALT1] = serverp.arg("thalt1");
	EEPROMWriteInt(THALT1OFST,(paramsp[THALT1]).toInt());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified THALT1 "));
	DEBUG_PRINTLN(paramsp[THALT1]);
	paramsp[TIMINGCHANGED]="true";
  }
  if(serverp.hasArg("thalt2") && (paramsp[THALT2] != String(serverp.arg("thalt2"))) ){
	paramsp[THALT2] = serverp.arg("thalt2");
	EEPROMWriteInt(THALT2OFST,(paramsp[THALT2]).toInt());
	EEPROM.commit();
    DEBUG_PRINT(F("Modified THALT2 "));
    DEBUG_PRINTLN(paramsp[THALT2]);
    paramsp[TIMINGCHANGED]="true";
  }
  if(serverp.hasArg("taplength") && (paramsp[TLENGTH] != String(serverp.arg("taplength"))) ){
	paramsp[TLENGTH] = serverp.arg("taplength");
	EEPROMWriteStr(TLENGTHOFST,(paramsp[TLENGTH]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified TLENGTH "));
	DEBUG_PRINTLN(paramsp[TLENGTH]);
	paramsp[TIMINGCHANGED]="true";
  }
  if(serverp.hasArg("barrelrad") && (paramsp[BARRELRAD] != String(serverp.arg("barrelrad"))) ){
	paramsp[BARRELRAD] = serverp.arg("barrelrad");
	EEPROMWriteStr(BARRELRADOFST,(paramsp[BARRELRAD]).c_str());
	EEPROM.commit();
    DEBUG_PRINT(F("Modified BARRELRAD "));
    DEBUG_PRINTLN(paramsp[BARRELRAD]);
    paramsp[TIMINGCHANGED]="true";
  }
  if(serverp.hasArg("thickness") && (paramsp[THICKNESS] != String(serverp.arg("thickness"))) ){
	paramsp[THICKNESS] = serverp.arg("thickness");
	EEPROMWriteStr(THICKNESSOFST,(paramsp[THICKNESS]).c_str());
	EEPROM.commit();
    DEBUG_PRINT(F("Modified THICKNESS "));
    DEBUG_PRINTLN(paramsp[THICKNESS]);
    paramsp[TIMINGCHANGED]="true";
  }
   if(serverp.hasArg("slatsratio") && (paramsp[SLATSRATIO] != String(serverp.arg("slatsratio"))) ){
	paramsp[SLATSRATIO] = serverp.arg("slatsratio");
	EEPROMWriteStr(SLATSRATIOFST,(paramsp[SLATSRATIO]).c_str());
	EEPROM.commit();
    DEBUG_PRINT(F("Modified SLATSRATIO "));
    DEBUG_PRINTLN(paramsp[SLATSRATIO]);
    paramsp[TIMINGCHANGED]="true";
  }
  EEPROM.end();
   
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
}

void loadConfig() {
	char buf[32]="";
    EEPROM.begin(EEPROMPARAMSLEN);
	delay(10);
	//DEBUG_PRINTln();
	//DEBUG_PRINTLN();
	//DEBUG_PRINT(F("Startup string: "));
	bool cfg = validateEEPROM();
	EEPROMReadStr(0,buf);
	EEPROM.end();
	//DEBUG_PRINTLN(buf);
	delay(1000);
	if(!cfg) {
		DEBUG_PRINTLN(F("EEPROM read error! EEPROM will be initialized, you must load a new valid system configuration."));
		EEPROM.begin(EEPROMPARAMSLEN);
		initEEPROM(EEPROMPARAMSLEN); 
		EEPROM.end();
		saveOnEEPROM();
		paramsp[CONFLOADED]="false";
		delay(1000);
	} else {
		EEPROM.begin(EEPROMPARAMSLEN);
		DEBUG_PRINTLN(F("Reading EEPROM configuration...."));
		
		paramsp[THALT1]	= EEPROMReadInt(THALT1OFST);
		DEBUG_PRINT(F("THALT1: "));
		DEBUG_PRINTLN(paramsp[THALT1]);
		
		paramsp[THALT2] = EEPROMReadInt(THALT2OFST);
		DEBUG_PRINT(F("THALT2: "));
		DEBUG_PRINTLN(paramsp[THALT2]);
		
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
		EEPROMReadStr(STDEL1OFST,buf);
		paramsp[STDEL1] = buf;
		DEBUG_PRINTLN(F("motor start delay1: "));
		DEBUG_PRINTLN(paramsp[STDEL1]);
		
		EEPROMReadStr(STDEL2OFST,buf);
		paramsp[STDEL2] = buf;
		DEBUG_PRINTLN(F("motor start delay2: "));
		DEBUG_PRINTLN(paramsp[STDEL2]);
#endif		
		EEPROMReadStr(VALWEIGHTOFST,buf);
		paramsp[VALWEIGHT] = buf;
		DEBUG_PRINTLN(F("sensor weight: "));
		DEBUG_PRINTLN(paramsp[VALWEIGHT]);
		
		EEPROMReadStr(TLENGTHOFST,buf);
		paramsp[TLENGTH] = buf;
		DEBUG_PRINTLN(F("barrel LENGTH: "));
		DEBUG_PRINTLN(paramsp[TLENGTH]);
		
		EEPROMReadStr(BARRELRADOFST,buf);
		paramsp[BARRELRAD] = buf;
		DEBUG_PRINTLN(F("barrel radius: "));
		DEBUG_PRINTLN(paramsp[BARRELRAD]);
		
		EEPROMReadStr(THICKNESSOFST,buf);
		paramsp[THICKNESS] = buf;
		DEBUG_PRINTLN(F("barrel THICKNESS: "));
		DEBUG_PRINTLN(paramsp[THICKNESS]);
		
		EEPROMReadStr(SLATSRATIOFST,buf);
		paramsp[SLATSRATIO] = buf;
		DEBUG_PRINTLN(F("barrel SLATSRATIO: "));
		DEBUG_PRINTLN(paramsp[SLATSRATIO]);
		
		/*
		EEPROMReadStr(TRSHOLD1OFST,buf);
		paramsp[TRSHOLD1] = buf;
		DEBUG_PRINTLN(F("current variance 1: "));
		DEBUG_PRINTLN(paramsp[TRSHOLD1]);
		
		EEPROMReadStr(TRSHOLD2OFST,buf);
		paramsp[TRSHOLD2] = buf;
		DEBUG_PRINTLN(F("current variance 2: "));
		DEBUG_PRINTLN(paramsp[TRSHOLD2]);
		*/
		
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

void saveOnEEPROM(){
	//I parametri devono poter essere modificati
	DEBUG_PRINTLN(F("Saving configuration...."));
  
	EEPROM.begin(EEPROMPARAMSLEN);
	
	EEPROMWriteInt(THALT1OFST,(paramsp[THALT1]).toInt());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified THALT1 "));
	DEBUG_PRINTLN(paramsp[THALT1]);

	EEPROMWriteInt(THALT2OFST,(paramsp[THALT2]).toInt());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified THALT2 "));
	DEBUG_PRINTLN(paramsp[THALT2]);
	
	EEPROMWriteStr(MQTTADDROFST,(paramsp[MQTTADDR]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified mqttAddr "));
	DEBUG_PRINTLN(paramsp[MQTTADDR]);
	//indirizzo MQTT cambiato!
	//paramsp[MQTTADDRMODFIED]="true"

	EEPROMWriteStr(MQTTIDOFST,(paramsp[MQTTID]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified mqttID "));
	DEBUG_PRINTLN(paramsp[MQTTID]);

	EEPROMWriteStr(OUTTOPICOFST,(paramsp[MQTTOUTTOPIC]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified mqttOutTopic "));
	DEBUG_PRINTLN(paramsp[MQTTOUTTOPIC]);
	//Topic MQTT cambiato!
	//paramsp[TOPICCHANGED]="true"

	EEPROMWriteStr(INTOPICOFST,(paramsp[MQTTINTOPIC]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified mqttInTopic "));
	DEBUG_PRINTLN(paramsp[MQTTINTOPIC]);
	//Topic MQTT cambiato!
	//paramsp[TOPICCHANGED]="true"

	EEPROMWriteStr(MQTTJSONUP1OFST,(mqttJsonp[MQTTJSONUP1]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified MQTTJSONUP1 "));
	DEBUG_PRINTLN(mqttJsonp[MQTTJSONUP1]);
	
	EEPROMWriteStr(MQTTJSONDOWN1OFST,(mqttJsonp[MQTTJSONDOWN1]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified MQTTJSONDOWN1 "));
	DEBUG_PRINTLN(mqttJsonp[MQTTJSONDOWN1]);

	EEPROMWriteStr(MQTTJSONUP2OFST,(mqttJsonp[MQTTJSONUP2]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified MQTTJSONUP2 "));
	DEBUG_PRINTLN(mqttJsonp[MQTTJSONUP2]);
	
	EEPROMWriteStr(MQTTJSONDOWN2OFST,(mqttJsonp[MQTTJSONDOWN2]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified MQTTJSONDOWN2 "));
	DEBUG_PRINTLN(mqttJsonp[MQTTJSONDOWN2]);
	
	EEPROMWriteStr(MQTTJSONTEMPOFST,(mqttJsonp[MQTTJSONTEMP]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified MQTTJSONTEMP "));
	DEBUG_PRINTLN(mqttJsonp[MQTTJSONTEMP]);
	
	EEPROMWriteStr(MQTTJSONMEANPWROFST,(mqttJsonp[MQTTJSONMEANPWR]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified MQTTJSONMEANPWR "));
	DEBUG_PRINTLN(mqttJsonp[MQTTJSONMEANPWR]);
	
	EEPROMWriteStr(MQTTJSONPEAKPWROFST,(mqttJsonp[MQTTJSONPEAKPWR]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified MQTTJSONPEAKPWR "));
	DEBUG_PRINTLN(mqttJsonp[MQTTJSONPEAKPWR]);
	
	EEPROMWriteStr(MQTTJSONALLOFST,(mqttJsonp[MQTTJSONALL]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified MQTTJSONALL "));
	DEBUG_PRINTLN(mqttJsonp[MQTTJSONALL]);
	
	EEPROMWriteStr(WIFICLIENTSSIDOFST1,(paramsp[CLNTSSID1]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified clntSsid1 "));
	DEBUG_PRINTLN(paramsp[CLNTSSID1]);
	//paramsp[WIFICHANGED]="true"

	EEPROMWriteStr(WIFICLIENTPSWOFST1,(paramsp[CLNTPSW1]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified clntPsw1 "));
	DEBUG_PRINTLN(paramsp[CLNTPSW1]);
	//paramsp[WIFICHANGED]="true"
	
	EEPROMWriteStr(WIFICLIENTSSIDOFST2,(paramsp[CLNTSSID2]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified clntSsid2 "));
	DEBUG_PRINTLN(paramsp[CLNTSSID2]);
	//paramsp[WIFICHANGED]="true"

	EEPROMWriteStr(WIFICLIENTPSWOFST2,(paramsp[CLNTPSW2]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified clntPsw2 "));
	DEBUG_PRINTLN(paramsp[CLNTPSW2]);
	//paramsp[WIFICHANGED]="true"

	EEPROMWriteStr(WIFIAPSSIDOFST,(paramsp[APPSSID]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified APSsid "));
	DEBUG_PRINTLN(paramsp[APPSSID]);
	//paramsp[WIFICHANGED]="true"

	EEPROMWriteStr(WIFIAPPPSWOFST,(paramsp[APPPSW]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified APPPSW "));
	DEBUG_PRINTLN(paramsp[APPPSW]);
	//paramsp[WIFICHANGED]="true"

	EEPROMWriteStr(WEBUSROFST,(paramsp[WEBUSR]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified webUsr "));
	DEBUG_PRINTLN(paramsp[WEBUSR]);
	
	EEPROMWriteStr(WEBPSWOFST,(paramsp[WEBPSW]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified webPsw "));
	DEBUG_PRINTLN(paramsp[WEBPSW]);
	
	EEPROMWriteStr(MQTTUSROFST,(paramsp[MQTTUSR]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified MQTT username "));
	DEBUG_PRINTLN(paramsp[MQTTUSR]);
	//paramsp[MQTTCONNCHANGED]="true"

	EEPROMWriteStr(MQTTPSWOFST,(paramsp[MQTTPSW]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified MQTT password "));
	DEBUG_PRINTLN(paramsp[MQTTPSW]);
	//paramsp[MQTTCONNCHANGED]="true"
#if (!AUTOCAL) 	
	EEPROMWriteStr(STDEL1OFST,(paramsp[STDEL1]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified start delay 1 "));
	DEBUG_PRINTLN(paramsp[STDEL1]);
	
	EEPROMWriteStr(STDEL2OFST,(paramsp[STDEL2]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified start delay 2 "));
	DEBUG_PRINTLN(paramsp[STDEL2]);
#endif	
	EEPROMWriteStr(VALWEIGHTOFST,(paramsp[VALWEIGHT]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified sensor weight "));
	DEBUG_PRINTLN(paramsp[VALWEIGHT]);
	
	EEPROMWriteStr(TLENGTHOFST,(paramsp[TLENGTH]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified barrel length "));
	DEBUG_PRINTLN(paramsp[TLENGTH]);
	
	EEPROMWriteStr(BARRELRADOFST,(paramsp[BARRELRAD]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified barrel radius "));
	DEBUG_PRINTLN(paramsp[BARRELRAD]);
	
	EEPROMWriteStr(THICKNESSOFST,(paramsp[THICKNESS]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified barrel THICKNESS "));
	DEBUG_PRINTLN(paramsp[THICKNESS]);
	
	EEPROMWriteStr(SLATSRATIOFST,(paramsp[SLATSRATIO]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified barrel SLATSRATIO "));
	DEBUG_PRINTLN(paramsp[SLATSRATIO]);
	
	/*
	EEPROMWriteStr(TRSHOLD1OFST,(paramsp[TRSHOLD1]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified current variance 1 "));
	DEBUG_PRINTLN(paramsp[TRSHOLD1]);
	
	EEPROMWriteStr(TRSHOLD1OFST,(paramsp[TRSHOLD2]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified current variance 2 "));
	DEBUG_PRINTLN(paramsp[TRSHOLD2]);
	*/

    EEPROM.end();
}

void printConfig(){
		DEBUG_PRINT(F("\nPrinting EEPROM configuration...."));
		
		DEBUG_PRINT(F("\nTHALT1: "));
		DEBUG_PRINT(paramsp[THALT1]);
		
		DEBUG_PRINT(F("\nTHALT2: "));
		DEBUG_PRINT(paramsp[THALT2]);
		
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
}
