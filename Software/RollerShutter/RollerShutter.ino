#include "common.h"
//End MQTT config------------------------------------------
//inizio variabili e costanti dello schedulatore (antirimbalzo)
#define nsteps          12000        // numero di fasi massimo di un periodo generico

//global strings
String twodot = "\":\"";
String comma = "\",\"";
String openbrk = "{\"";
String openbrk2 = "{";
String opensqr = "\":[\"";
String closesqr = "\"]}";
String closesqr2 = "\"],\"";
String closesqr3 = "\"],";
String closebrk = "\"}";
String enda = "\",";
//end global string
//stats variables
#if (AUTOCAL)
int samples[10];
byte indx = 0;
byte stat;
float vcosfi = 220*0.8;
double ACSVolt;
unsigned int mVperAmp = 185;   // 185 for 5A, 100 for 20A and 66 for 30A Module
double ACSVoltage = 0;
//double peak = 0;
double VRMS = 0;
double VMCU = 0;
double AmpsRMS = 0;
int zero, l=0, h=1024;
unsigned long smplcnt; //sampleCount;
unsigned short zeroCnt = 0;
unsigned long mqttcnt = 0;
unsigned short mqttofst = 0;
unsigned short isrundelay[2] = {RUNDELAY, RUNDELAY};
unsigned pushCnt = 0;
//double ex=0;
unsigned long n=1;
unsigned long pn=0;
volatile int minx = 1024;
volatile int maxx = 0;
volatile int x;
int dd = 0;
volatile float m;
double ex[2] = {0,0};
double calAvg[2] = {0,0};
double weight[2] = {0,0};
short chk[2]={0,0};
//unsigned x20ms, x60ms, x1s;
#endif
bool isrun[2]={false,false};
volatile bool dosmpl = false;
byte cont=0;
//end of stats variables
unsigned long prec=0;
unsigned long current = 0;
wl_status_t wfs;
byte wifiState = WIFISTA;
volatile bool wifiConn, keepConn, startWait, endWait;
unsigned long _connectTimeout  = 10*1000;
//wifi config----------------------------------------------
const char* outTopic = OUTTOPIC;
const char* inTopic = INTOPIC;
const char* ssid1 = SSID1;
const char* password1 = PSW1;
const char* ssid2 = SSID2;
const char* password2 = PSW2;
const char* apSsid = SSIDAP;
const char* apPassword = PSWAP;
//MQTT config----------------------------------------------
const char* mqtt_server = MQTTSRV;
const char* clientID = MQTTCLIENTID;
int haltPrm[2] = {THALT1,THALT2};
int haltOfs[2] = {THALT1OFST,THALT2OFST};
byte blocked[2]={0,0};
unsigned long edelay[2]={0,0};
byte wsnconn = 0;
IPAddress myip(192, 168, 43, 1);
IPAddress mygateway(192, 168, 43, 1);
IPAddress mysubnet(255, 255, 255, 0);
bool roll[2] = {true, true};

#if(LARGEFW)
RemoteDebug telnet;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
#endif

WiFiEventHandler stationConnectedHandler;
WiFiEventHandler stationDisconnectedHandler;

char gbuf[50];
//WiFiEventHandler gotIpEventHandler, disconnectedEventHandler, connectedEventHandler;

//User config
//--------------------------------------------------------------------------------------
String APSsid(apSsid);
String APPsw(apPassword);
String clntSsid1(ssid1);
String clntPsw1(password1);
String clntSsid2(ssid2);
String clntPsw2(password2);
String mqttAddr(mqtt_server);
String mqttID(clientID);
String mqttOutTopic(outTopic);
String mqttInTopic(inTopic);
String webUsr("admin");
String webPsw("admin");
String mqttUsr("");
String mqttPsw("");
//Valore iniziale: il suo contenuto viene poi caricato da EEPROM
unsigned int thalt1=5000;
unsigned int thalt3=5000;
unsigned int thalt2=5000;
unsigned int thalt4=5000;
//End JSON config------------------------------------------
//JSON config----------------------------------------------
//{"OUTSLED":"0","up1":"1","down1":"0","up2":"50","down2":"0", pr1:"12", pr2:"76"}
int ncifre=4;
//array delle proprietà
//l'ordine è importante! Le proprietà verranno ricercate nella stringa in arrivo con questo ordine.
//e salvate in un array con indici a questo corrrispondenti
//l'ordine di trasmissione da remoto dei campi è ininfluente
//I comandi della tapparella devono essere  gli ultimi!
String mqttJson[MQTTDIM]={"up1","down1","up2","down2","temp","avgpwr","peakpwr","all","date","mac","ip","time","mqttid","oncond1","oncond2","oncond3","oncond4","oncond5","onaction","utcval","utcsync","utcadj","utcsdt","utczone","webusr","webpsw"};
byte inr[MQTTDIM];
//commands flags that generates event signalling
String confJson[EXTCONFDIM]={/*len variabile-->*/"oncond1","oncond2","oncond3","oncond4","oncond5","onaction"/*len fissa-->*/,"utcval","utcsync","utcadj","utcsdt","utczone","webusr","webpsw"/*privati-->,"appssid","apppsw","clntssid1","clntpsw1","clntssid2","clntpsw2","mqttaddr","mqttid","mqttouttopic","mqttintopic","mqttusr","mqttpsw","thalt1","thalt2","thalt3","thalt4", "stdel1","stdel2","valweight","tlength","barrelrad","thickness","slatsratio","swroll1","swroll2","localip","ntpaddr1","ntpaddr2"*/};
//default values, some modificable via MQTT, web form or event rules
String confcmd[CONFDIM]={/*len variabile-->*/"","","","","",""/*len fissa-->*/,"","50","0","1","1"/*privati-->*/,webUsr,webPsw,APSsid,APPsw,clntSsid1,clntPsw1,clntSsid2,clntPsw2,mqttAddr,mqttID, mqttOutTopic,mqttInTopic,mqttUsr,mqttPsw,String(thalt1),String(thalt2),String(thalt3),String(thalt4), "400","400", "0.5","53","3.37","1.5", "0.8", "0","0", "ip", "ntp1.inrim.it","0.it.pool.ntp.org", "false","false","false","false","false","false"};
//constant values, identify confcmd across entire system
//(MQTT parser, web page static component, web page dinamyc component, etc).
//parameter properties mapper, is dinamically created on system setup
//contain parameters of confJson[] and confcmd[] that are to be saved or modified via web
Par *pars[PARAMSDIM];

ESP8266WebServer server(80);    	// Create a webserver object that listens for HTTP request on port 80
WebSocketsServer webSocket(81);	    // create a websocket server on port 81
ESP8266HTTPUpdateServer httpUpdater;
extern "C"
{
  #include <lwip/icmp.h> // needed for icmp packet definitions
}

// Set global to avoid object removing after setup() routine
Pinger pinger;

// fine variabili e costanti dello schedulatore
// Update these with values suitabule for your network.
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//                                      End User config
//--------------------------------------------------------------------------------------
//Port config----------------------------------------------
//LED and relays switches
bool boot=true;
byte swcount=0;
byte wifindx=0;
//End port config------------------------------------------
unsigned int step;
//bool extCmd=false;
//vettori di ingresso, uscite e stato
byte in[NBTN*BTNDIM], outPorts[NBTN*BTNDIM], out[NBTN*BTNDIM];
byte acts[4];
//numerazione porte
byte outLogic[NBTN*STATUSDIM];
//inizio variabili globali antirimbalzo----------------------------------------------
//fine variabili globali antirimbalzo----------------------------------------------
//dichiara una variabile oggetto della classe WiFiClient (instanzia il client WiFi)
//WiFiClient espClient;
//dichiara una variabile oggetto della classe PubSubClient (instanzia il client MQTT)
// create MQTT
MQTT *mqttClient=0;
bool configLoaded=true;
bool mqttConnected=false;
bool mqttAddrChanged=true;
bool dscnct=false;
String pr[3] = {"{\"pr1\":\"", "{\"pr2\":\"", "\"}"};
//-----------------------------------------Begin of prototypes---------------------------------------------------------
#if (AUTOCAL_HLW8012 && LARGEFW) 
HLW8012 hlw8012;

// When using interrupts we have to call the library entry point
		// whenever an interrupt is triggered
void ICACHE_RAM_ATTR hlw8012_cf1_interrupt() {
	hlw8012.cf1_interrupt();
}

void ICACHE_RAM_ATTR hlw8012_cf_interrupt() {
	hlw8012.cf_interrupt();
}

// Library expects an interrupt on both edges
void setInterrupts() {
	attachInterrupt(CF1_PIN, hlw8012_cf1_interrupt, CHANGE);
	attachInterrupt(CF_PIN, hlw8012_cf_interrupt, CHANGE);
}

void calibrate() {
	// Let some time to register values
	unsigned long timeout = millis();
	while ((millis() - timeout) < 10000) {
		delay(1);
	}

	// Calibrate using a 60W bulb (pure resistive) on a 230V line
	hlw8012.expectedActivePower(60.0);
	hlw8012.expectedVoltage(230.0);
	hlw8012.expectedCurrent(60.0 / 230.0);

	// Show corrected factors
	Serial.print("[HLW] New current multiplier : "); Serial.println(hlw8012.getCurrentMultiplier());
	Serial.print("[HLW] New voltage multiplier : "); Serial.println(hlw8012.getVoltageMultiplier());
	Serial.print("[HLW] New power multiplier   : "); Serial.println(hlw8012.getPowerMultiplier());
}

void HLW8012_init(){
		// Initialize HLW8012
		// void begin(unsigned char cf_pin, unsigned char cf1_pin, unsigned char sel_pin, unsigned char currentWhen = HIGH, bool use_interrupts = false, unsigned long pulse_timeout = PULSE_TIMEOUT);
		// * cf_pin, cf1_pin and sel_pin are GPIOs to the HLW8012 IC
		// * currentWhen is the value in sel_pin to select current sampling
		// * set use_interrupts to true to use interrupts to monitor pulse widths
		// * leave pulse_timeout to the default value, recommended when using interrupts
		hlw8012.begin(CF_PIN, CF1_PIN, SEL_PIN, CURRENT_MODE, true);

		// These values are used to calculate current, voltage and power factors as per datasheet formula
		// These are the nominal values for the Sonoff POW resistors:
		// * The CURRENT_RESISTOR is the 1milliOhm copper-manganese resistor in series with the main line
		// * The VOLTAGE_RESISTOR_UPSTREAM are the 5 470kOhm resistors in the voltage divider that feeds the V2P pin in the HLW8012
		// * The VOLTAGE_RESISTOR_DOWNSTREAM is the 1kOhm resistor in the voltage divider that feeds the V2P pin in the HLW8012
		hlw8012.setResistors(CURRENT_RESISTOR, VOLTAGE_RESISTOR_UPSTREAM, VOLTAGE_RESISTOR_DOWNSTREAM);

		// Show default (as per datasheet) multipliers
		Serial.print("[HLW] Default current multiplier : "); Serial.println(hlw8012.getCurrentMultiplier());
		Serial.print("[HLW] Default voltage multiplier : "); Serial.println(hlw8012.getVoltageMultiplier());
		Serial.print("[HLW] Default power multiplier   : "); Serial.println(hlw8012.getPowerMultiplier());
		Serial.println();

		setInterrupts();
		//calibrate();
}
#endif

void testparam(int i){
	char * param;
	
	if(pars[i] != NULL){
		param = pars[i]->parname;
		DEBUG_PRINT(F("param("));
		DEBUG_PRINT(i);
		DEBUG_PRINT(F("): "));
		DEBUG_PRINTLN(param);
	}
}

void printparams(){
	DEBUG_PRINT(F("printparams "));
	DEBUG_PRINTLN(PARAMSDIM);
	for(int i=0; i<PARAMSDIM; i++){
		testparam(i);
	}
}

unsigned getConfofstFromParamofst(unsigned pofst){
	if(pars[pofst] != NULL){
		if(pofst < USRMODIFICABLEFLAGS)
			return pofst;
		else
			return pofst - USRMODIFICABLEFLAGS;
	}
}

//-----------------------------------------End of prototypes---------------------------------------------------------
inline void initOfst(){
	//------------------------------------------
	//pars[i][0] - parameter eeprom offset
	//------------------------------------------
	//pars[i][1] - type of eeprom saved data
	//------------------------------------------
	//i:integer
	//b:byte
	//f:float
	//s:string 32
	//t:string 64
	//l:long
	//n:no save
	//------------------------------------------
	//pars[i][2] - type of form field
	//------------------------------------------
	//c:checkbox
	//i:input, textarea
	//s:select
	//n:no form
	//------------------------------------------
	//pars[i][3] - type of parameter
	//------------------------------------------
	//p:parameter
	//j:jsonname
	//----------------------------------------------
	for(int i=0; i<PARAMSDIM; i++){
		pars[i] = NULL;
	}
	/*23*/pars[MQTTUP1] = new ParStr32("up1", MQTTUP1OFST, 'j','i');
	/*24*/pars[MQTTDOWN1] = new ParStr32("down1", MQTTDOWN1OFST, 'j','i');
	/*25*/pars[MQTTUP2] = new ParStr32("up2", MQTTUP2OFST, 'j','i');
	/*26*/pars[MQTTDOWN2] = new ParStr32("down2", MQTTDOWN2OFST, 'j','i');
	//--------------------------------------------------------------------------------------------------------------
	/*1*/pars[LOCALIP + USRMODIFICABLEFLAGS] = new ParInt("localip");
	/*2*/pars[SWROLL1 + USRMODIFICABLEFLAGS] = new ParInt("swroll1", SWROLL1OFST, 'p', 'i');
	/*3*/pars[SWROLL2 + USRMODIFICABLEFLAGS] = new ParInt("swroll2", SWROLL1OFST, 'p', 'i');
	/*4*/pars[UTCSDT + USRMODIFICABLEFLAGS] = new ParByte("utcsdt", NTPSDTOFST, 'p', 'i');
	/*5*/pars[UTCZONE + USRMODIFICABLEFLAGS] = new ParByte("utczone", NTPZONEOFST, 'p','i');
	/*6*/pars[THALT1 + USRMODIFICABLEFLAGS] = new ParInt("thalt1", THALT1OFST, 'p','i');
	/*7*/pars[THALT2 + USRMODIFICABLEFLAGS] = new ParInt("thalt2", THALT2OFST, 'p','i');
	/*8*/pars[THALT3 + USRMODIFICABLEFLAGS] = new ParInt("thalt3", THALT3OFST, 'p','i');
	/*9*/pars[THALT4 + USRMODIFICABLEFLAGS] = new ParInt("thalt4", THALT4OFST, 'p','i');
	/*10*/pars[STDEL1 + USRMODIFICABLEFLAGS] = new ParFloat("stdel1", STDEL1OFST, 'p','i');
	/*10*/pars[STDEL2 + USRMODIFICABLEFLAGS] = new ParFloat("stdel2", STDEL2OFST, 'p','i');
	/*12*/pars[VALWEIGHT + USRMODIFICABLEFLAGS] = new ParFloat("valweight", VALWEIGHTOFST, 'n','i');
	/*13*/pars[TLENGTH + USRMODIFICABLEFLAGS] = new ParFloat("tlength", TLENGTHOFST, 'p','i');
	/*14*/pars[BARRELRAD + USRMODIFICABLEFLAGS] = new ParFloat("barrelrad", BARRELRADOFST, 'p','i');
	/*15*/pars[THICKNESS + USRMODIFICABLEFLAGS] = new ParFloat("thickness", THICKNESSOFST, 'p','i');
	/*16*/pars[UTCADJ + USRMODIFICABLEFLAGS] = new ParInt("utcadj", NTPADJUSTOFST, 'p','i');
	/*17*/pars[SLATSRATIO + USRMODIFICABLEFLAGS] = new ParFloat("slatsratio", SLATSRATIOFST, 'p','i');
	/*19*/pars[UTCSYNC + USRMODIFICABLEFLAGS] = new ParInt("utcsync", NTPSYNCINTOFST, 'p','i');
	/*20*/pars[MQTTID + USRMODIFICABLEFLAGS] = new ParStr32("mqttid", MQTTIDOFST, 'p','i');
	/*21*/pars[MQTTOUTTOPIC + USRMODIFICABLEFLAGS] = new ParStr32("mqttouttopic", OUTTOPICOFST, 'p','i');
	/*22*/pars[MQTTINTOPIC + USRMODIFICABLEFLAGS] = new ParStr32("mqttintopic", INTOPICOFST, 'p','i');
	/*27*/pars[CLNTSSID1 + USRMODIFICABLEFLAGS] = new ParStr32("clntssid1", WIFICLIENTSSIDOFST1, 'p','i');
	/*28*/pars[CLNTPSW1 + USRMODIFICABLEFLAGS] = new ParStr32("clntpsw1", WIFICLIENTPSWOFST1, 'p','i');
	/*29*/pars[CLNTSSID2 + USRMODIFICABLEFLAGS] = new ParStr32("clntssid2", WIFICLIENTSSIDOFST2, 'p','i');
	/*30*/pars[CLNTPSW2 + USRMODIFICABLEFLAGS] = new ParStr32("clntpsw2", WIFICLIENTPSWOFST2, 'p','i');
	/*18*/pars[APPSSID + USRMODIFICABLEFLAGS] = new ParStr32("appssid", WIFIAPSSIDOFST, 'p','i');
	/*32*/pars[APPPSW + USRMODIFICABLEFLAGS] = new ParStr32("apppsw", WIFIAPPPSWOFST, 'p','i');
	/*33*/pars[WEBUSR + USRMODIFICABLEFLAGS] = new ParStr32("webusr", WEBUSROFST, 'p','i');
	/*34*/pars[WEBPSW + USRMODIFICABLEFLAGS] = new ParStr32("webpsw", WEBPSWOFST, 'p','i');
	/*35*/pars[MQTTUSR + USRMODIFICABLEFLAGS] = new ParStr32("mqttusr", MQTTUSROFST, 'p','i');
	/*36*/pars[MQTTPSW + USRMODIFICABLEFLAGS] = new ParStr32("mqttpsw", MQTTPSWOFST, 'p','i');
	/*37*/pars[MQTTADDR + USRMODIFICABLEFLAGS] = new ParStr64("mqttaddr", MQTTADDROFST, 'p','i');
	/*38*/pars[NTPADDR1 + USRMODIFICABLEFLAGS] = new ParStr32("ntpaddr1", NTP1ADDROFST, 'p','i');
	/*39*/pars[NTPADDR2 + USRMODIFICABLEFLAGS] = new ParStr32("ntpaddr2", NTP2ADDROFST, 'p','i');
	/*40*/pars[ONCOND1 + USRMODIFICABLEFLAGS] = new ParStr32("oncond1");
	/*41*/pars[ONCOND2 + USRMODIFICABLEFLAGS] = new ParStr32("oncond2");
	/*42*/pars[ONCOND3 + USRMODIFICABLEFLAGS] = new ParStr32("oncond3");
	/*43*/pars[ONCOND4 + USRMODIFICABLEFLAGS] = new ParStr32("oncond4");
	/*44*/pars[ONCOND5 + USRMODIFICABLEFLAGS] = new ParStr32("oncond5");
	/*45*/pars[ACTIONEVAL + USRMODIFICABLEFLAGS] = new ParStr32("onaction");

	printparams();
}

inline bool switchdfn(byte val, byte n){
	//n: numero di pulsanti
	val = (val>0);								//SEMPLICE!!!!
	bool changed = (val != dprecval2[n]);
	dprecval2[n] = val;            // valore di val campionato al loop precedente 
	return changed;
}

inline bool gatedfn(float val, byte n, float rnd){
	//n: numero di porte
	bool changed = (val < asyncBuf[n] - rnd || val > asyncBuf[n] + rnd);
	asyncBuf[n] = val;            // valore di val campionato al loop precedente 
	return changed;
}

inline byte cmdLogic(byte n){
	if(roll[n]){
		//DEBUG_PRINTLN("\ncmdLogic: switchLogic");
		return (switchLogic(0,n) + switchLogic(1,n));
	}else{
		//DEBUG_PRINTLN("\ncmdLogic: toggleLogic");
		return (toggleLogic(0, n) + toggleLogic(1, n));
	}
}

float getAmpRMS(float ACSVolt){
	ACSVolt = (double) (ACSVolt * 5.0) / 1024.0;
	VRMS = ACSVolt * 0.707;
	AmpsRMS = (double) (VRMS * 1000) / mVperAmp;
	if((AmpsRMS > -0.015) && (AmpsRMS < 0.008)){ 
		AmpsRMS = 0.0;
	}
	return AmpsRMS*vcosfi;
}

void setSWMode(byte mode,byte n){
	roll[n] = mode;
	isrun[n] = false;
	//confcmd[SWROLL1+n] = String(mode);
	DEBUG_PRINT("setSWMode: ");
	DEBUG_PRINTLN(confcmd[SWROLL1+n]);
	DEBUG_PRINTLN(n);
	readModeAndPub(n);
}

byte inline getSWMode(byte n){
	return roll[n];
}

float getTemperature(){
	float temp = -127;
#if(LARGEFW)
	DS18B20.requestTemperatures(); 
	unsigned short cnt = 0;
	do{
		temp = DS18B20.getTempCByIndex(0);
		//DEBUG_PRINT("Temperature: ");
		//DEBUG_PRINTLN(temp);
		cnt++;		
	}while((temp == 85.0 || temp == (-127.0)) && cnt < 3);
#endif
	return temp;
}

//parser actions callBack (assignements)
//configurazioni provenienti da eventi locali
float actions(char *key, float val)
{	
	byte n=-1;
	if(key[0]=='t'){
		if(roll[0] == false){
			if(strcmp(key,"tsmpl1")==0){
				n = 0;
			}else if(strcmp(key,"tsmpl2")==0){
				n = 1;
			}
		}
		if(roll[1] == false){
			if(strcmp(key,"tsmpl3")==0){
				n = 2;
			}else if(strcmp(key,"tsmpl4")==0){
				n = 3;
			}
		}
		if(n>=0){
			startCnt(0,(unsigned long)val,SMPLCNT1+n);
			DEBUG_PRINT("\nstartCnt: ");
			DEBUG_PRINTLN(val);
		}
		if(key[1]=='d'){
			if(roll[0] == false){
				if(key[2]=='1'){
					n = 0;
				}else if(key[2]=='2'){
					n = 1;
				}
			}
			if(roll[1] == false){
				if(key[2]=='3'){
					n = 2;
				}else if(key[2]=='4'){
					n = 3;
				}
			}
			if(n>=0){
				//writeHaltDelay(val,n);
				updtConf(THALT1+n, String(val));
				setHaltDelay(val,n);
				readActModeAndPub(n);
			}
		}
	}else if(key[0]=='m'){
		byte act=0;
		if(key[1]=='a'){
			if(roll[0] == false){
				if(key[2]=='1'){
					n = 0;
				}else if(key[2]=='2'){
					n = 1;
				}
				act=val;
			}
			if(roll[1] == false){
				if(key[2]=='3'){
					n = 2;
				}else if(key[2]=='4'){
					n = 3;
				}
				act=val;
			}
			if(n>=0){
				//writeOnOffAction(act,n);
				setSWAction(act,n);
				readActModeAndPub(n);
			}
		}else if(strcmp(key,"mode1")==0){
			if(val==1){
				setSWMode(1,0);
				//writeSWMode(1,0);
				updtConf(SWROLL1OFST, String(1));
			}else if(val==0){
				setSWMode(0,0);
				//writeSWMode(0,0);
				updtConf(SWROLL1OFST, String(0));
			}
		}else if(strcmp(key,"mode2")==0){
			if(val==1){
				setSWMode(1,1);
				//writeSWMode(1,1);
				updtConf(SWROLL1OFST+1, String(0));
			}else if(val==0){
				setSWMode(0,1);
				//writeSWMode(0,1);
				updtConf(SWROLL1OFST+1, String(1));
			}
		}
		return 1;
	}else if(key[0]=='r'){
		unsigned long cnt = val;
		if(strcmp(key,"r1")==0){
			setCntValue(cnt,CNTIME1);
		}else if(strcmp(key,"r2")==0){
			setCntValue(cnt,CNTIME2);
		}else if(strcmp(key,"r3")==0){
			setCntValue(cnt,CNTIME3);
		}else if(strcmp(key,"r4")==0){
			setCntValue(cnt,CNTIME4);
		}
		return val;
	}else if(key[0]=='o'){
		bool oe = val;
		if(strcmp(key,"oe1")==0){
			setOE(oe,0);
		}else if(strcmp(key,"oe2")==0){
			setOE(oe,1);
		}else if(strcmp(key,"oe3")==0){
			setOE(oe,2);
		}else if(strcmp(key,"oe4")==0){
			setOE(oe,3);
		}
		return val;
	}else if(key[0]=='s'){
		if(strcmp(key,"sdt")==0){
			updtConf(UTCSDT, String(val));
			setSDT((byte) val);
		}
		return val;
	}
}

//parser function calls
float variables(char *key){
	float result=1;
	
	if(key[0]=='t'){
		if(strcmp(key,"tsec")==0){
			result = second();
		}else if(strcmp(key,"tmin")==0){
			result = minute();
		}else if(strcmp(key,"thour")==0){
			result = hour();
		}else if(key[1]=='e' && key[2]==':'){//2019:07:30/03:57:30
			key += strlen("te:");
			result = makeTime(fromStrToTimeEl(key));
		}else if(strcmp(key,"telem")==0){
			result = getUNIXTime();
		}else if(strlen(strstr(key,"tdst:M")) == strlen(key)){
			key += strlen("tdst:M");//M4.5.0/02:00:00
			result = makeTime(DSTToTimeEl(key));
		}
	}else if(key[0]=='d'){	
		if(strcmp(key,"dyear")==0){
			result =  year();
		}else if(strcmp(key,"dmonth")==0){
			result =  month();
		}else if(strcmp(key,"dwkday")==0){
			result =  weekday();
		}else if(strcmp(key,"day")==0){
			result = day();
		}
	}else if(key[0]=='c'){
		if(strcmp(key,"c1")==0){
			result = getCntValue(CNTIME1);
		}else if(strcmp(key,"c2")==0){
			result = getCntValue(CNTIME2);
		}else if(strcmp(key,"c3")==0){
			result = getCntValue(CNTIME3);
		}else if(strcmp(key,"c4")==0){
			result = getCntValue(CNTIME4);
		}else{
			result = 0;
		}
	}else if(key[0]=='o'){
		if(strcmp(key,"o1")==0){
			result = outLogic[ENABLES];
		}else if(strcmp(key,"o2")==0){
			result = outLogic[DIRS];
		}else if(strcmp(key,"o3")==0){
			result = outLogic[ENABLES+STATUSDIM];
		}else if(strcmp(key,"o4")==0){
			result = outLogic[DIRS+STATUSDIM];
		}else{
			result = 0;
		}
	}else if(key[0]=='s'){
		if(key[1]==':'==0){
			key += strlen("s:");
			IPAddress ip;
			if (!WiFi.hostByName(key, ip))
				ip.fromString(key);
			result = pinger.Ping(ip);
		}
	}else if(key[0] =='i'){
		if(strcmp(key,"p1")==0){
			result = getAmpRMS(getAVG(0)/2);
		}else if(strcmp(key,"p2")==0){
			result = getAmpRMS(getAVG(1)/2);
		}
	}else if(key[0]=='p'){
		char *app;
		if(strcmp(key,"isPM")==0){
			result = isPM();
		}else if(strcmp(key,"isAM")==0){
			result = isAM();
		}else if(strcmp(key,"isMQTT")==0){
			result = mqttConnected;
		}
	}else if(strcmp(key,"wifi")==0){
		result = wifiConn;
	}else if(strcmp(key,"temp")==0){
		result = getTemperature();
	}else if(strcmp(key,"wifi")==0){
		result = wifiConn;
	}
	return result;
}

void scriviOutDaStato(){
	 digitalWrite(OUT1EU,out[0]);	
	 digitalWrite(OUT1DD,out[1]);		
	 digitalWrite(OUT2EU,out[2]);	
	 digitalWrite(OUT2DD,out[3]);		
	 isrun[0] = (outLogic[ENABLES]==HIGH) && roll[0];					
	 isrun[1] = (outLogic[ENABLES+STATUSDIM]==HIGH) && roll[1];	
}

void setup_AP(bool apmode) {
  //ESP.wdtFeed();
  Serial.println(F("Configuring access point..."));
  // You can remove the password parameter if you want the AP to be open. 
  //WiFi.softAP(APSsid.c_str(), APPsw.c_str());
  
  if(apmode){
	  Serial.print(F("Setting soft-AP configuration ... "));
	  Serial.println(WiFi.softAPConfig(myip, mygateway, mysubnet) ? F("Ready") : F("Failed!"));
	  //noInterrupts ();
	  delay(100);
	  WiFi.softAP((confcmd[APPSSID]).c_str());
	  //WiFi.softAP("cacca9");
	  //interrupts();
	  //DEBUG_PRINT(F("Setting soft-AP ... "));
	  //DEBUG_PRINTLN(WiFi.softAP((confcmd[APPSSID]).c_str()) ? F("Ready") : F("Failed!"));
	  //delay(500); // Without delay I've seen the IP address blank
	  delay(1000);
	  confcmd[LOCALIP] = WiFi.softAPIP().toString();
	  Serial.print(F("Soft-AP IP address = "));
	  Serial.println(confcmd[LOCALIP]);
	  //wifi_softap_dhcps_stop();
  }else{
	  //noInterrupts ();
	  WiFi.softAP((confcmd[APPSSID]).c_str());
	  //WiFi.softAP(ssid, password);
	  //interrupts();
	  //DEBUG_PRINT(F("Setting soft-AP ... "));
	  //DEBUG_PRINTLN(WiFi.softAP((confcmd[APPSSID]).c_str()) ? F("Ready") : F("Failed!"));
	  delay(1000);
	  confcmd[LOCALIP] = WiFi.softAPIP().toString();
	  Serial.print(F("Soft-AP IP address = "));
	  Serial.println(confcmd[LOCALIP]);
	  //wifi_softap_dhcps_stop();
  }
}

void scan_wifi() {
int numberOfNetworks = WiFi.scanNetworks();
 
  for(int i =0; i<numberOfNetworks; i++){
 
     DEBUG_PRINT("\nNetwork name: ");
     DEBUG_PRINT(WiFi.SSID(i));
     DEBUG_PRINT("\nSignal strength: ");
     DEBUG_PRINT(WiFi.RSSI(i));
     DEBUG_PRINT("\n-----------------------");
 
  }
}

//wifi setup function
void setup_wifi(int wifindx) {
	//ESP.wdtFeed();
	wifindx = wifindx*2;  //client1 e client2 hanno indici contigui nell'array confcmd
	// We start by connecting to a WiFi network
	Serial.println(F("Connecting to "));
	Serial.println(confcmd[CLNTSSID1+wifindx]);
    Serial.println(F(" as wifi client..."));
	if (wifiConn) {
		//importante! Altrimenti tentativi potrebbero collidere con una connessione appena instaurata
		Serial.println(F("Already connected. Bailing out."));
	}else{
		//WiFi.setAutoConnect(true);		//inibisce la connessione automatica al boot
		//WiFi.setAutoReconnect(true);	//inibisce la riconnessione automatica dopo una disconnesione accidentale
		//wifi_set_sleep_type(NONE_SLEEP_T);
		//WiFi.persistent(false);			//inibisce la memorizzazione dei parametri della connessione
		//WiFi.mode(WIFI_OFF);    //otherwise the module will not reconnect
		//WiFi.mode(WIFI_STA);	
		//wifiState = WIFISTA;
		//check if we have ssid and pass and force those, if not, try with last saved values
		if ((confcmd[CLNTSSID1+wifindx]).c_str() != "") {
			//noInterrupts();
			Serial.print(F("Begin status: "));
			Serial.println(WiFi.begin((confcmd[CLNTSSID1+wifindx]).c_str(), (confcmd[CLNTPSW1+wifindx]).c_str()));
			//interrupts();
		} else {
			if (WiFi.SSID()) {
				Serial.println(F("Using last saved values, should be faster"));
				//trying to fix connection in progress hanging
				ETS_UART_INTR_DISABLE();
				wifi_station_disconnect();
				ETS_UART_INTR_ENABLE();
				WiFi.begin();
			} else {
				Serial.println(F("No saved credentials"));
			}
		}
		confcmd[LOCALIP] = WiFi.localIP().toString();
	}	

	Serial.println (F("\n******************* begin ***********"));
    WiFi.printDiag(Serial);
	Serial.println (F("\n******************* end ***********"));

	//WiFi.enableAP(true);
  //}
}

#if (LARGEFW)
void setup_mDNS() {
	if (MDNS.begin((confcmd[MQTTID]).c_str())) {              // Start the mDNS responder for esp8266.local
		DEBUG_PRINTLN(F("mDNS responder started"));
	} else {
		DEBUG_PRINTLN(F("Error setting up MDNS responder!"));
	}
}
#endif

void startWebSocket() { // Start a WebSocket server
  webSocket.begin();                          // start the websocket server
  webSocket.onEvent(webSocketEvent);          // if there's an incomming websocket message, go to function 'webSocketEvent'
  DEBUG_PRINTLN("WebSocket server started.");
}

void mqttReconnect() {
	// Loop until we're mqttReconnecte
	if(mqttClient!=NULL){
		DEBUG_PRINTLN(F("Distruggo l'oggetto MQTT client."));
		DEBUG_PRINTLN(F("Mi disconetto dal server MQTT"));
		if(mqttClient->isConnected()){
			noInterrupts ();
			mqttClient->disconnect();
			interrupts ();
		}
		delay(50);
		DEBUG_PRINTLN(F("Chiamo il distruttore dell'oggetto MQTT"));
		//noInterrupts ();
		ESP.wdtFeed();
		mqttClient->~MQTT();
		delay(100);
		//free((mqttClient));
		//DEBUG_PRINTLN(F("Cancello la vechia istanza dell'oggetto MQTT"));
		//delete mqttClient;
		DEBUG_PRINTLN(F("Annullo istanza dell'oggetto MQTT"));
		ESP.wdtFeed();
		mqttClient = NULL;
		//interrupts ();
		delay(50);
	}
	DEBUG_PRINTLN(F("Instanzio un nuovo oggetto MQTT client."));
	noInterrupts ();
	mqttClient = new MQTT((confcmd[MQTTID]).c_str(),(confcmd[MQTTADDR]).c_str(), 1883);
	interrupts ();
    DEBUG_PRINTLN(F("Registro i callback dell'MQTT."));
	DEBUG_PRINT(F("Attempting MQTT connection to: "));
	DEBUG_PRINT(confcmd[MQTTADDR]);
	DEBUG_PRINT(F(", with ClientID: "));
	DEBUG_PRINT(confcmd[MQTTID]);
	DEBUG_PRINTLN(F(" ..."));
	//mqttClient->setClientId(confcmd[MQTTID]);
	delay(50);
	if(mqttClient==NULL){
		DEBUG_PRINTLN(F("ERROR on mqttReconnect! MQTT client is not allocated."));
	}
	else
	{
		mqttClient->onData(mqttCallback);
		mqttClient->onConnected([]() {
		DEBUG_PRINTLN(F("mqtt: onConnected([]() dice mi sono riconnesso."));
			mqttcnt = 0;
			//Altrimenti dice che è connesso ma non comunica
			mqttClient->subscribe(confcmd[MQTTINTOPIC]);
			mqttClient->publish(confcmd[MQTTOUTTOPIC], confcmd[MQTTID]);
			//mqttConnected=true;
		});
		mqttClient->onDisconnected([]() {
			//DEBUG_PRINTLN("MQTT disconnected.");
		DEBUG_PRINTLN(F("MQTT: onDisconnected([]() dice mi sono disconnesso."));
			//mqttConnected=false;
		});
		DEBUG_PRINTLN(F("MQTT: Eseguo la prima connect."));
		mqttClient->setUserPwd((confcmd[MQTTUSR]).c_str(), (confcmd[MQTTPSW]).c_str());
		noInterrupts ();
		mqttClient->connect();
		interrupts ();
		delay(50);
		mqttClient->subscribe(confcmd[MQTTINTOPIC]);
		mqttClient->publish(confcmd[MQTTOUTTOPIC], confcmd[MQTTID]);
	}
}

void mqttCallback(String &topic, String &response) {
	//funzione eseguita dal subscriber all'arrivo di una notifica
	//decodifica la stringa JSON e la trasforma nel nuovo vettore degli stati
	//il vettore corrente degli stati verrà sovrascritto
	//applica la logica ricevuta da remoto sulle uscite locali (led)
    
	int v;
#if defined (_DEBUG1) || defined (_DEBUGR)	
	DEBUG_PRINT(F("Message arrived on topic: ["));
	DEBUG_PRINT(topic);
	DEBUG_PRINT(F("], "));
	DEBUG_PRINTLN(response);
#endif	
	//v = parseJsonFieldToInt(response, mqttJson[0], ncifre);
	//digitalWrite(OUTSLED, v); 
   
	parseJsonFieldArrayToInt(response, inr, mqttJson, ncifre, MQTTDIM,0);
	parseJsonFieldArrayToStr(response, confcmd, confJson, ncifre+500, CONFDIM,0,'#',"|");
    //inr: memoria tampone per l'evento asincrono scrittura da remoto
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) { // When a WebSocket message is received
  DEBUG_PRINT("webSocketEvent(");
  DEBUG_PRINT(num);
  DEBUG_PRINT(", ");
  DEBUG_PRINT(type);
  char s[300];
  
  switch (type) {
    case WStype_DISCONNECTED:             // if the websocket is disconnected
		sprintf(s,"\n[%u] Disconnected!", num);
		DEBUG_PRINT(s);
		wsnconn--;
    break;
    case WStype_CONNECTED: {              // if a new websocket connection is established
		wsnconn++;
        IPAddress wip = webSocket.remoteIP(num);
		sprintf(s,"[%u] Connected from %d.%d.%d.%d url: %s\n", num, wip[0], wip[1], wip[2], wip[3], payload);
		DEBUG_PRINT(s);
		readStatesAndPub(true);
	}
    break;
    case WStype_TEXT:                     // if new text data is received 
		sprintf(s,"[%u] get Text: %s\r\n", num, payload);
		DEBUG_PRINT(s);
		String str = String((char *)payload);
		String str2 = String("");
		mqttCallback(str2, str);
	break;
  }
}

//legge il valore dello stato dei toggle e li pubblica sul broker come stringa JSON
void readStatesAndPub(bool all){

  //DEBUG_PRINTLN(F("\nreadStatesAndPub")); 
  
  //crea una stringa JSON con i valori  dello stato corrente dei pulsanti

  //vals=digitalRead(OUTSLED); //legge lo stato del led di stato
  //crea una stringa JSON con i valori  dello stato corrente dei pulsanti
  String s=openbrk;	
  s+=mqttJson[MQTTUP1]+twodot+(outLogic[ENABLES] && (outLogic[DIRS]==LOW))+comma; 	//up1 DIRS=HIGH
  s+=mqttJson[MQTTDOWN1]+twodot+(outLogic[ENABLES] && (outLogic[DIRS]==HIGH))+comma;    //down1  DIRS=LOW
  s+=mqttJson[MQTTUP2]+twodot+(outLogic[ENABLES+STATUSDIM] && (outLogic[DIRS+STATUSDIM]==LOW))+comma;	//up2 
  s+=mqttJson[MQTTDOWN2]+twodot+(outLogic[ENABLES+STATUSDIM] && (outLogic[DIRS+STATUSDIM]==HIGH))+comma;    //down2
  s+= (String) "pr1"+twodot+String(percfdbck(0))+comma;		//pr1
  s+= (String) "pr2"+twodot+String(percfdbck(1))+comma;		//pr2
  s+= (String) "tr1"+twodot+String(getCronoCount(0))+comma;			//tr1
  s+= (String) "tr2"+twodot+String(getCronoCount(1))+comma;			//tr2
  if(blocked[0]>0){
	  s+= (String) "blk1"+twodot+blocked[0]+comma;		//blk1
  }
  if(blocked[1]>0){
	  s+= (String) "blk2"+twodot+blocked[1]+comma;		//blk2
  }
  s+= (String) "sp1"+twodot+String((long)getTapThalt(0))+comma;		//sp1
  s+= (String) "sp2"+twodot+String((long)getTapThalt(1));
  
  if(all){
	    s+=comma;
		s+=mqttJson[MQTTTEMP]+twodot+String(asyncBuf[GTTEMP])+comma;
		s+=mqttJson[MQTTMEANPWR]+opensqr+String(asyncBuf[GTMEANPWR1])+comma+String(asyncBuf[GTMEANPWR2])+closesqr2;
		s+=mqttJson[MQTTPEAKPWR]+opensqr+String(asyncBuf[GTPEAKPWR1])+comma+String(asyncBuf[GTPEAKPWR2])+"\"],";
  }else{
		s+=enda;
  }
  publishStr(s);
}

inline byte percfdbck(byte n){
	DEBUG_PRINT(F("Posdelta:"));
	DEBUG_PRINTLN(getPosdelta());
	if(getDelayedCmd(n) <= 100){
		return round(calcLen(n) - getPosdelta());  
	}else{
		return round(calcLen(n));  
	}
}

void readActModeAndPub(byte n){
  //DEBUG_PRINTLN(F("\nreadTempAndPub")); 
  String s=openbrk;
  //char sd[300];
  s+="actmode"+String(n+1)+twodot+confcmd[SWACTION1+n]+enda;
  //sprintf(sd,"%s%s%s%f%s",openbrk,mqttJson[MQTTJSONTEMP].c_str(),twodot,asyncBuf[GTTEMP],closebrk);
  //s=String(sd);
  publishStr(s);
}

void readModeAndPub(byte n){
  //DEBUG_PRINTLN(F("\nreadTempAndPub")); 
  String s=openbrk;
  //char sd[300];
  s+="mode"+String(n+1)+twodot+confcmd[SWROLL1+n]+enda;
  //sprintf(sd,"%s%s%s%f%s",openbrk,mqttJson[MQTTJSONTEMP].c_str(),twodot,asyncBuf[GTTEMP],closebrk);
  //s=String(sd);
  publishStr(s);
}

void readAvgPowerAndPub(){
  //DEBUG_PRINTLN(F("\nreadPowerAndPub")); 
  String s=openbrk;
  s+=mqttJson[MQTTMEANPWR]+opensqr+String(asyncBuf[GTMEANPWR1])+comma+String(asyncBuf[GTMEANPWR2])+closesqr3;
  publishStr(s);
}

void readPeakPowerAndPub(){
  //DEBUG_PRINTLN(F("\nreadPowerAndPub")); 
  String s=openbrk;
  s+=mqttJson[MQTTPEAKPWR]+opensqr+String(asyncBuf[GTPEAKPWR1])+comma+String(asyncBuf[GTPEAKPWR2])+closesqr3;
  publishStr(s);
}

void readParamAndPub(byte parid, char* str){
  //DEBUG_PRINTLN(F("\nreadTempAndPub")); 
  String s=openbrk;
  //char sd[300];
  s+=mqttJson[parid]+twodot+String(str)+enda;
//sprintf(sd,"%s%s%s%f%s",openbrk,mqttJson[MQTTJSONTEMP].c_str(),twodot,asyncBuf[GTTEMP],closebrk);
  //s=String(sd);
  //DEBUG_PRINTLN(s);
  publishStr(s);
}

void readTempAndPub(){
  //DEBUG_PRINTLN(F("\nreadTempAndPub")); 
  String s=openbrk;
  //char sd[300];
  s+=mqttJson[MQTTTEMP]+twodot+String(asyncBuf[GTTEMP])+enda;
//sprintf(sd,"%s%s%s%f%s",openbrk,mqttJson[MQTTJSONTEMP].c_str(),twodot,asyncBuf[GTTEMP],closebrk);
  //s=String(sd);
  publishStr(s);
}

void readMacAndPub(){
  String s=openbrk2;
  publishStr(s);
}

void readIpAndPub(){
  String s=openbrk2;
  publishStr(s);
}

void readTimeAndPub(){
  String s=openbrk2;
  publishStr(s);
}

void readMQTTIdAndPub(){
  String s=openbrk2;
  publishStr(s);
}

void inline readActionConfAndSet(){
	//imposta le configurazioni dinamiche in base ad eventi esterni
	DEBUG_PRINTLN(F("readActionConfAndSet."));
	eval((confcmd[ACTIONEVAL]).c_str());
}

void publishStr(String &str){
  //pubblica sul broker la stringa JSON
  //informazioni mittente
  str+="\"";
  str+=mqttJson[MQTTTIME]+twodot+String(millis())+comma;
  str+=mqttJson[MQTTMAC]+twodot+String(WiFi.macAddress())+comma;
  str+=mqttJson[LOCALIP]+twodot+WiFi.localIP().toString()+comma;
  str+=mqttJson[MQTTID]+twodot+confcmd[MQTTID]+closebrk;
   
  if(mqttClient==NULL){
	  DEBUG_PRINTLN(F("ERROR on publishStr MQTT client is not allocated."));
  }
  else
  {
	  mqttClient->publish(confcmd[MQTTOUTTOPIC], str);
	  DEBUG_PRINTLN(str);
  }
  //if(!webSocket){
	  //DEBUG_PRINTLN(F("ERROR on readStatesAndPub webSocket server is not allocated."));
  //}
  //else
  //{
  webSocket.broadcastTXT(str);
  //}
}

void initIiming(bool first){
  edelay[0]=(confcmd[STDEL1]).toInt();
  edelay[1]=(confcmd[STDEL2]).toInt();
  roll[0] = (confcmd[SWROLL1]).toInt();
  roll[1] = (confcmd[SWROLL2]).toInt();
  initTapparellaLogic(in,out,inr,outLogic,confcmd,first);
#if (AUTOCAL)  
  resetAVGStats(0,0);
  resetAVGStats(0,1);
#endif  
}

inline void setupNTP() {
  setNtpServer(0,(confcmd[NTPADDR1]).c_str());  
  setNtpServer(1,(confcmd[NTPADDR2]).c_str()); 
  setSyncInterval(strtoul((confcmd[UTCSYNC]).c_str(), NULL, 10));
  setSDT((confcmd[UTCSDT]).toInt());
  adjustTime((confcmd[UTCADJ]).toInt()); 
  setTimeZone((confcmd[UTCZONE]).toInt());
  sntpInit();  
}

void setup() {
  //delay(5000);
  DEBUG_PRINTLN(F("Inizializzo i parametri."));
  //initOfst();
  dosmpl = false;
  zeroCnt = 0;
  mqttcnt = 0;
  mqttofst = 2;
  //inizializza la seriale
  Serial.begin(115200);
  //importante per il _DEBUG1 del WIFI!
  //Serial.setDebugOutput(true);
  //WiFi.printDiag(Serial);
  //carica la configurazione dalla EEPROM
  //DEBUG_PRINTLN(F("Carico configurazione."));
  //for(int i=0;i<CONFDIM;i++)
  //	 confcmd[i]="bhocmd ";
  confcmd[0]="-1";
  confcmd[1]="-1";
  confcmd[2]="-1";
  confcmd[3]="-1";
  confcmd[4]="(td1=4000)|(ma1=0)|(ma4=2)|(tsmpl4=4)|(oe1=1)";
  DEBUG_PRINTLN(F("initCommon."));
  initCommon(&server,pars,mqttJson,confJson,confcmd);
  delay(10000);
  initOfst();
  DEBUG_PRINTLN(F("loadConfig."));
  loadConfig();
  delay(100);
  wifiConn = false;
  startWait=false;
  endWait=true;
  WiFi.printDiag(Serial);
  //WiFi.enableAP(false);
  //delay(100);
  //WiFi.persistent(true);
  //WiFi.softAPdisconnect(true);
  //ESP.eraseConfig();
  WiFi.persistent(false);
  //ESP.eraseConfig();
  //WiFi.softAPdisconnect(true);
  //ESP.eraseConfig();
  //inizializza l'AP wifi
  //setup_AP(true);
  wifiState = WIFISTA;
  WiFi.mode(WIFI_STA);
  setup_wifi(wifindx); 
  setupNTP();
  //setTimerState(wfs, CONNSTATSW);
#if(LARGEFW)
#if (AUTOCAL_HLW8012) 
  HLW8012_init();
#endif    
  telnet.begin((confcmd[MQTTID]).c_str()); // Initiaze the telnet server
  telnet.setResetCmdEnabled(true); // Enable the reset command
  telnet.setCallBackProjectCmds(&processCmdRemoteDebug);
  DEBUG_PRINTLN(F("Activated remote _DEBUG1"));
#endif  
  DEBUG_PRINTLN(F("Inizializzo i pulsanti."));
  initdfn(LOW, 0);  //pull DOWN init (in realtà è un pull up, c'è un not in ogni ingresso sui pulsanti)
  initdfn(LOW, 1);
  initdfn(LOW, 2);
  initdfn(LOW, 3);
  initdfn(LOW, 4);
  //initdfn((byte) WL_DISCONNECTED, CONNSTATSW);

  //Timing init
  setupTimer((confcmd[THALT1]).toInt(),TMRHALT);				//function timer switch1
  setupTimer((confcmd[THALT2]).toInt(),TMRHALT+TIMERDIM);	//function timer switch2 
  setupTimer(RSTTIME*1000,RESETTIMER);						//special timer btn1 
  setupTimer(APOFFT*1000,APOFFTIMER);						//special timer btn1
  setSWMode((confcmd[SWROLL1]).toInt(),0);
  setSWMode((confcmd[SWROLL2]).toInt(),1);
#if (AUTOCAL)
  weight[0] =  (confcmd[VALWEIGHT]).toFloat();
  weight[1] = 1 - weight[0];
  confcmd[STDEL1] = TENDCHECK*1000;
  confcmd[STDEL2] = TENDCHECK*1000;
  //setThresholdUp((confcmd[TRSHOLD1]).toFloat(), 0);
  //setThresholdUp((confcmd[TRSHOLD2]).toFloat(), 1);
#endif
  delay(100);
  httpSetup();
  delay(100);
  //must be after http setup almeno 100ms se no si pianta!
  startWebSocket();
  delay(100);
  for(int i=0; i<4;i++){
	startCnt(0,0,SMPLCNT1+i);
  }
  //startCnt(0,60,TIMECNT);
  //read and set dynamic configurations
  readActionConfAndSet(); 
  
  outPorts[0]=OUT1EU;
  outPorts[1]=OUT1DD;
  outPorts[2]=OUT2EU;
  outPorts[3]=OUT2DD;
  //imposta la Direzione delle porte dei pulsanti  
#if (INPULLUP)
	pinMode(BTN1U, INPUT_PULLUP);
	pinMode(BTN1D, INPUT_PULLUP);
	pinMode(BTN2U, INPUT_PULLUP);
	pinMode(BTN2D, INPUT_PULLUP);
#else
	pinMode(BTN1U, INPUT);
	pinMode(BTN1D, INPUT);
	pinMode(BTN2U, INPUT);
	pinMode(BTN2D, INPUT);
#endif
	pinMode(A0, INPUT);
  //imposta la DIRSezione delle porte dei led, imposta inizialmente i led come spenti
  pinMode(OUTSLED,OUTPUT);
  digitalWrite(OUTSLED, LOW);
  for(int i=0;i<NBTN*BTNDIM;i++){
	  pinMode(outPorts[i],OUTPUT);
	  digitalWrite(outPorts[i], LOW);
  }
  //------------------------------------------OTA SETUP---------------------------------------------------------------------------------------
  //------------------------------------------END OTA SETUP---------------------------------------------------------------------------------------
  //segnala  la corretta accensione della macchina con un blink dei led dei pulsanti
  delay(500);
  for(int i=0;i<NBTN*STATUSDIM;i++)
	  outLogic[i]=LOW;
  for(int i=0;i<MQTTDIM;i++)
	  inr[i]=LOW;
  for(int i=0;i<4;i++)
	  acts[i]=LOW;
  initIiming(true);
  // Register event handlers.
  // Callback functions will be called as long as these handler objects exist.
  // Call "onStationConnected" each time a station connects
  stationConnectedHandler = WiFi.onSoftAPModeStationConnected(&onStationConnected);
  // Call "onStationDisconnected" each time a station disconnects
  stationDisconnectedHandler = WiFi.onSoftAPModeStationDisconnected(&onStationDisconnected);
/*  
  gotIpEventHandler = WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP& event)
  {
	Serial.print("DHCP got an IP. Station connected, IP: ");
	Serial.println(WiFi.localIP());
	//wifiConn = true;
  });
  
  disconnectedEventHandler = WiFi.onStationModeDisconnected([](const WiFiEventStationModeDisconnected& event)
  {
    //Serial.println("Station disconnected");
	//wifiConn = false;
	//keepConn = false; //rallenta il loop()
  });
  
  connectedEventHandler =  WiFi.onStationModeConnected([](const WiFiEventStationModeConnected& event){
    //Serial.print("Station connected");
	//wifiConn = true;
  });
*/
  //initTapparellaLogic(in,inr,outLogic,(confcmd[THALT1]).toInt(),(confcmd[THALT2]).toInt(),(confcmd[STDEL1]).toInt(),(confcmd[STDEL2]).toInt());
  //esp_log_set_vprintf(_log_vprintf);
#if defined (_DEBUG1) || defined (_DEBUGR)  
  testFlash();
#endif
#if (AUTOCAL)  
  zeroDetect();
#endif
  cont=0;
  while (WiFi.status() != WL_CONNECTED && cont<30000/500) {
     delay(500);
	 cont++;
     Serial.print(".");
  }

  Serial.print(":");
  Serial.print(500*cont);
  if(cont==30000/500){
	Serial.print("\nStation not connected!");  
  }else{
	Serial.print("\nStation connected, IP: ");
	Serial.println(WiFi.localIP());
  }
  
  swcount = 0;
  DEBUG_PRINTLN(F(" OK"));
  DEBUG_PRINTLN(F("Last reset reason: "));
  DEBUG_PRINTLN(ESP.getResetReason());
  
  if(WiFi.status() == WL_CONNECTED)
		mqttReconnect();
  boot = false;
  
  DEBUG_PRINTLN(F("sampleCurrTime()"));
  sampleCurrTime();
}

void httpSetup(){
  DEBUG_PRINTLN(F("Registro handleRootExt."));
  server.on("/", HTTP_GET, handleRoot);        // Call the 'handleRoot' function when a client requests URI "/"
  DEBUG_PRINTLN(F("Registro handleLoginExt."));
  server.on("/login", HTTP_POST, handleLogin); // Call the 'handleLogin' function when a POST request is made to URI "/login"
  DEBUG_PRINTLN(F("Registro handleModifyExt."));
  server.on("/modify", HTTP_POST, handleModify);
  server.on("/systconf", HTTP_POST, handleSystemConf);
  server.on("/wificonf", HTTP_POST, handleWifiConf);
  server.on("/mqttconf", HTTP_POST, handleMQTTConf);
  server.on("/logicconf", HTTP_POST, handleLogicConf);
  server.on("/eventconf", HTTP_POST, handleEventConf);
  server.on("/cmd", HTTP_GET, handleCmd);
  //server.on("/cmdjson", handleCmdJsonExt);
  //DEBUG_PRINTLN(F("Registro handleNotFoundExt."));
  server.onNotFound(handleNotFound);           // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"
  //avvia il server HTTP*/
  DEBUG_PRINTLN("Inizio avvio il server web.");
  //here the list of headers to be recorded
  const char * headerkeys[] = {"User-Agent","Cookie"} ;
  size_t headerkeyssize = sizeof(headerkeys)/sizeof(char*);
  //ask server to track these headers
  server.collectHeaders(headerkeys, headerkeyssize );
  //OTA web page handler linkage
  httpUpdater.setup(&server);
  //start HTTP server
  server.begin();
  DEBUG_PRINTLN("HTTP server started");
#if(LARGEFW)
  DEBUG_PRINTLN("Avvio il responder mDNS.");
  delay(100);
  //setup_mDNS();
  MDNS.addService(F("http"), F("tcp"), 80); 
  DEBUG_PRINT("HTTPUpdateServer ready! Open http://");
  DEBUG_PRINT(confcmd[MQTTID]);
  DEBUG_PRINTLN(".local/update in your browser");
#endif
}

void zeroDetect(){
	for(int i = 0, m = 0; i < 2000; i++) {
		m = (float) m + analogRead(A0);
		delay(2);
	}
	m /= 2000;
	smplcnt = 0;
	minx = 1024;
	maxx = 0;
}


void loop(){
	if(boot == false){
		//busy loop
		loop2();
	}else{
		//Dummy loop! Finestra solo wifi!
		//workaround for the DHCP offer problem ERROR: send_offer (error -13)
		//no busy loop! For difficult (intensive time consuming) wifi operation
		delay(3000);
		boot = false;
	}
}
	
inline void loop2() {
  //PRE SCHEDULERS ACTIONS ----------------------------------------------
  //ArduinoOTA.handle();
  //funzioni eseguite ad ogni loop (istante di esecuzione dipendente dal clock della CPU)
  aggiornaTimer(TMRHALT);
  aggiornaTimer(TMRHALT+TIMERDIM); 
  aggiornaTimer(TMRHALT2);
  aggiornaTimer(TMRHALT2+TIMERDIM); 
  server.handleClient();  // Listen for HTTP requests from clients
  webSocket.loop();
#if(LARGEFW)
  //MDNS.update();
#endif  
  //FINE PRE SCHEDULERS ACTIONS -----------------------------------------  
  
  //SCHEDULATED LOOPS------------------------------------------------------------------------------
  //Linee guida:
  //- azioni pesanti si dovrebbero eseguire molto raramente (o per pochi loop)
  //- azioni leggere possono essere eseguite molto frequentemente (o per molti loop)
  //- vie di mezzo di complessità da eseguire con tempi intermedi
  //- evitare il più possibile la contemporaneità di azioni pesanti 
  //-----------------------------------------------------------------------------------------------
  
  //---------------------------------------------------------------------
  // 2 msec scheduler (main scheduler)
  //------------------------------------------------------------------------
  if((millis()-prec) > TBASE) //schedulatore per tempo base 
  {	
	prec = millis();
	//calcolo dei multipli interi del tempo base
	step = (step + 1) % nsteps;
	
#if (AUTOCAL) 
	if(dosmpl){//solo a motore in moto
#if (AUTOCAL_ACS712) 
		currentPeakDetector();
#endif
	}

	//---------------------------------------------------------------------
	// 10-20 msec scheduler
	//---------------------------------------------------------------------
	if(!(step % STOP_STEP)){		
		automaticStopManager();
	}//END 20ms scheduler--------------------------------------------------
#endif

	//---------------------------------------------------------------------
	// 1 sec scheduler
	//---------------------------------------------------------------------
	if(!(step % ONESEC_STEP)){
		updateCounters();
		
		if(!(isrun[0] || isrun[1])){//solo a motore fermo! Per evitare contemporaneità col currentPeakDetector
			aggiornaTimer(RESETTIMER);
			aggiornaTimer(APOFFTIMER);
			pushCnt++;
			//DEBUG_PRINT(F("\n------------------------------------------"));
			//DEBUG_PRINT(F("\nMean sensor: "));
			//DEBUG_PRINT(m);
			//DEBUG_PRINT(F(" - Conn stat: "));
			stat = WiFi.status();
			//DEBUG_PRINT(stat);
			wifiConn = (stat == WL_CONNECTED);	
			//DEBUG_PRINT(F(" - Wifi mode: "));
			//DEBUG_PRINTLN(WiFi.getMode());
			
			sensorStatePoll();
			wifiFailoverManager();
			MQTTReconnectManager();
			paramsModificationPoll();
		}
		if(sampleCurrTime()){//1min
			readParamAndPub(MQTTDATE,printUNIXTimeMin(gbuf));
		}
		leggiTastiLocaliDaExp();
	}//END 1 sec scheduler-----------------------------------------------------
	
	//---------------------------------------------------------------------
	// 50-60 msec scheduler
	//---------------------------------------------------------------------
	if(!(step % MAINPROCSTEP)){		
		//leggi ingressi locali e mette il loro valore sull'array val[]
		leggiTastiLocali();
		leggiTastiLocaliRemoto();
		//se uno dei tasti delle tapparelle è stato premuto
		//o se è arrivato un comando dalla mqttCallback
		if(cmdLogic(TAP1) == 1 ||  cmdLogic(TAP2) == 1){ 
			//once any button is pressed
			//legge lo stato finale e lo scrive sulle uscite
			scriviOutDaStato();
			//legge lo stato finale e lo pubblica su MQTT
			//readStatesAndPub();
			//DEBUG_PRINTLN("Fine callback MQTT.");
			blocked[0]=blocked[1]=false;
		}
		//provenienti dalla mqttCallback
		//remote pressed button event
		leggiTastiRemoti();
		//------------------------------------------------------------------------------------------------------------
		//Finestra idle di riconnessione (necessaria se il loop è molto denso di eventi e il wifi non si aggancia!!!)
		//------------------------------------------------------------------------------------------------------------
		//sostituisce la bloccante WiFi.waitForConnectResult();	
		if((wifiConn == false && !(isrun[0] || isrun[1]))){
#if (AUTOCAL_ACS712) 
			DEBUG_PRINTLN(F("\nGiving time to ESP stack... "));
			delay(30);//give 30ms to the ESP stack for wifi connect
#endif  
			wifiConn = (WiFi.status() == WL_CONNECTED);
		}
		//------------------------------------------------------------------------------------------------------------
	}//END 50-60ms scheduler------------------------------------------------------------------------------------
  }//END Time base (2-20 msec) main scheduler------------------------------------------------------------------------  
  //POST SCHEDULERS ACTIONS-----------------
#if(LARGEFW)
  telnet.handle();
#endif  
  yield();	// Give a time for ESP8266
}//END loop

inline void updateCounters(){
	 incCnt(CNTIME1);
	 incCnt(CNTIME2);
	 incCnt(CNTIME3);
	 incCnt(CNTIME4);
	 incCnt(SMPLCNT1);
	 incCnt(SMPLCNT2);
	 incCnt(SMPLCNT3);
	 incCnt(SMPLCNT4);
	 //incCnt(TIMECNT);
}

//legge gli ingressi remoti e mette il loro valore sull'array val[]
//inr: memoria tampone per l'evento asincrono scrittura da remoto
//si deve mischiare con la lettura locale DOPO che questa viene scritta
//al giro di loop() successivo in[] locale riporta a livello basso l'eccitazione remota
//legge gli ingressi dei tasti già puliti dai rimbalzi	
inline void leggiTastiLocaliRemoto(){
	for(int i=0;i<4;i++){
		(in[i]) && (in[i] = 255);
		(inr[i]) && (in[i] = inr[i]);
		inr[i]=LOW;
	}
}
//legge PERIODICAMENTE il parser delle condizioni sui sensori
inline void leggiTastiLocaliDaExp(){
	if(roll[0] == false){
		//modalità switch generico
		if(testUpCntEvnt(0,true,SMPLCNT1))
			setActionLogic(eval((confcmd[0]).c_str()),0);
		if(testUpCntEvnt(0,true,SMPLCNT2))
			setActionLogic(eval((confcmd[1]).c_str()),1);
		//legge lo stato finale e lo scrive sulle uscite
		scriviOutDaStato();
		//legge lo stato finale e lo pubblica su MQTT
		//readStatesAndPub();
	}else{
		//modalità tapparella
		//simula pressione di un tasto locale
		in[0] = eval((confcmd[0]).c_str());
		in[1] = eval((confcmd[1]).c_str());
	}
	if(roll[1] == false){
		//modalità switch generico
		if(testUpCntEvnt(0,true,SMPLCNT3))
			setActionLogic(eval((confcmd[2]).c_str()),2);
		if(testUpCntEvnt(1,true,SMPLCNT4)){
			setActionLogic(eval((confcmd[3]).c_str()),3);
			DEBUG_PRINTLN(F("\nSMPLCNT4: "));
			DEBUG_PRINTLN(getCntValue(SMPLCNT4));
		}
		//legge lo stato finale e lo scrive sulle uscite
		scriviOutDaStato();
		//legge lo stato finale e lo pubblica su MQTT
		//readStatesAndPub();
	}else{
		//modalità tapparella
		//simula pressione di un tasto locale
		in[2] = eval((confcmd[2]).c_str());
		in[3] = eval((confcmd[3]).c_str());
	}
	//imposta le configurazioni dinamiche in base ad eventi locali valutati periodicamente
	//eval((confcmd[JSONONCOND5]).c_str());
}

inline void leggiTastiRemoti(){
	//--------------------------------------------------------------------------------------------------
	//gestione eventi MQTT sui sensori (richieste e configurazioni) all'arrivo memorizzate su confcmd[]
	//--------------------------------------------------------------------------------------------------
	//configurazioni provenienti da remoto
	//---------------------------------------------------------------
	//for(int i=0; i < EXTCONFDIM; i++){
	//}
	if(inr[UTCFLAG]){
		inr[UTCFLAG] = LOW;
		updateNTP(strtoul((confcmd[UTCVAL]).c_str(), NULL, 10));
		//no confcmd to save
	}
	if(inr[UTCSYNCFLAG]){
		inr[UTCSYNCFLAG] = LOW;
		setSyncInterval(saveLongConf(UTCSYNC));
	}
	if(inr[UTCADJFLAG]){
		inr[UTCADJFLAG] = LOW;
		adjustTime(saveIntConf(UTCADJ));
	}
	if(inr[UTCSDTFLAG]){
		inr[UTCSDTFLAG] = LOW;
		setSDT(saveByteConf(UTCSDT));
	}
	if(inr[UTCZONEFLAG]){
		inr[UTCZONEFLAG] = LOW;
		setTimeZone(saveByteConf(UTCZONE));
	}
	//parametri di configurazione di lunghezza variabile
	if(inr[ACTIONFLAG]){
		inr[ACTIONFLAG] = LOW;
		//save confs and actions on new action received event
		writeOnOffConditions();
		//run actions one time on new action received event
		readActionConfAndSet();
	}
	if(inr[CONFFLAG5]){
		inr[CONFFLAG5] = LOW;
		//save confs and actions on new action received event
		writeOnOffConditions();
		//are periodic actions!
	}
	if(inr[CONFFLAG1]){
		inr[CONFFLAG1] = LOW;
		//save confs and actions on new config received event
		writeOnOffConditions();
		//confs are automatically runned on every loop by leggiTastiLocaliDaExp()
	}
	if(inr[CONFFLAG2]){
		inr[CONFFLAG2] = LOW;
		//save confs and actions on new config received event
		writeOnOffConditions();
		//config are automatically runned on every loop by leggiTastiLocaliDaExp()
	}
	if(inr[CONFFLAG3]){
		inr[CONFFLAG3] = LOW;
		//save confs and actions on new config received event
		writeOnOffConditions();
		//config are automatically runned on every loop by leggiTastiLocaliDaExp()
	}
	if(inr[CONFFLAG4]){
		inr[CONFFLAG4] = LOW;
		//save confs and actions on new config received event
		writeOnOffConditions();
		//config are automatically runned on every loop by leggiTastiLocaliDaExp()
	}
	if(inr[WEBUSERFLAG]){
		inr[WEBUSERFLAG] = LOW;
		//save confs and actions on new config received event
		saveSingleParam(WEBUSR);
		//config are automatically runned on every loop by leggiTastiLocaliDaExp()
	}
	if(inr[WEBPSWFLAG]){
		inr[WEBPSWFLAG] = LOW;
		//save confs and actions on new config received event
		saveSingleParam(WEBPSW);
		//config are automatically runned on every loop by leggiTastiLocaliDaExp()
	}
	//----------------------------------------------------------------------------
	//richieste da remoto di valori locali
	//----------------------------------------------------------------------------
	if(inr[MQTTTEMP]){
		inr[MQTTTEMP] = LOW;
		readTempAndPub();
	}
	if(inr[MQTTMEANPWR]){
		inr[MQTTMEANPWR] = LOW;
		readAvgPowerAndPub();
	}
	if(inr[MQTTPEAKPWR]){
		inr[MQTTPEAKPWR] = LOW;
		readPeakPowerAndPub();
	}
	if(inr[MQTTMAC]){
		inr[MQTTMAC] = LOW;
		readMacAndPub();
	}
	if(inr[MQTTIP]){
		inr[MQTTIP] = LOW;
		readIpAndPub();
	}
	if(inr[MQTTTIME]){
		inr[MQTTTIME] = LOW;
		readTimeAndPub();
	}
	if(inr[MQTTMQTTID]){
		inr[MQTTMQTTID] = LOW;
		readMQTTIdAndPub();
	}
}

#if (AUTOCAL_ACS712) 
inline void currentPeakDetector(){
	//AC current peak detector
        system_soft_wdt_stop();
        ets_intr_lock( ); //close interrupt
        noInterrupts();
		x = system_adc_read() - m;
		//samples[indx] = system_adc_read();
		//x = samples[indx] - m;	
		(x > maxx) && (maxx = x);
		(x < minx) && (minx = x);
		/*if(x > maxx) 					
		{    							
			maxx = x; 					
		}
		if(x < minx) 					
		{       						
			minx = x;					
		}*/
		//indx++;
		interrupts();
        ets_intr_unlock(); //open interrupt
        system_soft_wdt_restart();
}
#endif

inline void sensorStatePoll(){
	//sensor variation polling management
	//on events basis push of reports
	if(gatedfn(getTemperature(),GTTEMP, TEMPRND)){
		readTempAndPub();
		DEBUG_PRINT(F("\nTemperatura cambiata"));
	}
	bool updatePwr = false;
	if(gatedfn(getAmpRMS(getAVG(0)/2),GTMEANPWR1, MEANPWR1RND)){
		updatePwr == true;
	}
	if(gatedfn(getAmpRMS(getAVG(1)/2),GTMEANPWR2, MEANPWR2RND)){
		updatePwr == true;
	}
	if(updatePwr){
		readAvgPowerAndPub();
		updatePwr = false;
	}
	if(gatedfn(getAmpRMS(getAVG(0)+getThresholdUp(0)/2),GTPEAKPWR1, PEAKPWR1RND)){
		updatePwr == true;
	}
	if(gatedfn(getAmpRMS(getAVG(1)+getThresholdUp(1)/2),GTPEAKPWR2, PEAKPWR2RND)){
		updatePwr == true;
	}
	if(updatePwr){
		readPeakPowerAndPub();
	}
	//periodic push of all reports
	if(pushCnt == PUSHINTERV){
		pushCnt = 0;
		readStatesAndPub(true);
	}
}

inline void automaticStopManager(){
	if((isrun[0] || isrun[1])){
			//automatic stop manager
#if (AUTOCAL_ACS712) 
			dd = maxx - minx;
#endif
#if (AUTOCAL_HLW8012) 
			//dd = hlw8012.getActivePower();
			//dd = hlw8012.getExtimActivePower();
			dd = hlw8012.getAvgdExtimActivePower();
#endif
			//EMA calculation
			//ACSVolt = (double) ex/2.0;
			//peak = (double) ex/2.0;
			//reset of peak sample value
			DEBUG_PRINT(F("\n("));
			DEBUG_PRINT(0);
			DEBUG_PRINT(F(") sensor enable: "));
			DEBUG_PRINT(dosmpl);
			
			if(isrun[0] && dosmpl){
				DEBUG_PRINT(0);
				if(isrundelay[0] == 0){
					ex[0] = dd*EMA + (1.0 - EMA)*ex[0];
					DEBUG_PRINT(F("\n("));
					DEBUG_PRINT(0);
#if (AUTOCAL_ACS712) 
					DEBUG_PRINT(F(") minx sensor: "));
					DEBUG_PRINT(minx);
					DEBUG_PRINT(F(" - maxx sensor: "));
					DEBUG_PRINT(maxx);
					DEBUG_PRINT(F(" - Mean sensor: "));
					DEBUG_PRINT(m);
					DEBUG_PRINT(F(" - Peak: "));
					DEBUG_PRINT(ex[0]);
#endif
					DEBUG_PRINT(F(" - diff: "));
					DEBUG_PRINT(dd);
					//DEBUG_PRINT(F("\n("));
					//DEBUG_PRINT(0);
					//DEBUG_PRINT(F(") Samples: "));
					//for(int i=0;i<10;i++){
					//	DEBUG_PRINT(samples[i]);
					//	DEBUG_PRINT(F(", "));
					//	samples[i] = 0;
					//}
					//DEBUG_PRINT(F("\nSample count: "));
					//DEBUG_PRINTLN(sampleCount);
					//sampleCount = 0;
					chk[0] = checkRange((double) ex[0]*(1 - weight[1]*isMoving(1)),0);
					//DEBUG_PRINT(F("Ampere: "));
					//float amp = getAmpRMS();
					//DEBUG_PRINTLN(amp);
					if(chk[0] != 0){
						DEBUG_PRINT(F("\n("));
						DEBUG_PRINT(0);
						if(chk[0] == -1){
							DEBUG_PRINTLN(F(") Stop: sottosoglia"));
							//fine dorsa raggiunto
							blocked[0] = secondPress(0,40,true);
							scriviOutDaStato();
						}else if(chk[0] == 2){
							DEBUG_PRINTLN(F(") Stop: soprasoglia"));
							blocked[0] = secondPress(0,40);
							scriviOutDaStato();
							blocked[0] = 1;
						}else if(chk[0] == 1){
							ex[0] = getAVG(0);
							DEBUG_PRINTLN(F(") Start: fronte di salita"));					
							//inizio conteggio timer di posizionamento
							startEndOfRunTimer(0);
						}
						readStatesAndPub();
						yield();
					}
				}else{
					DEBUG_PRINT(F("\n("));
					DEBUG_PRINT(0);
					DEBUG_PRINT(F(") aspetto: "));
					DEBUG_PRINT(isrundelay[0]);
					DEBUG_PRINT(F(" - minx sensor: "));
					DEBUG_PRINT(minx);
					DEBUG_PRINT(F(" - maxx sensor: "));
					DEBUG_PRINT(maxx);
					DEBUG_PRINT(F(" - Peak: "));
					DEBUG_PRINT(ex[0]);
					isrundelay[0]--;
					ex[0] = dd;
					DEBUG_PRINT(F(" - dd: "));
					DEBUG_PRINT(dd);
				}
			}else{
				isrundelay[0] = RUNDELAY;
				//reset dei fronti su blocco marcia (sia manuale che automatica) 
				resetEdges(0);
			}
			
			if(isrun[1] && dosmpl){
				if(isrundelay[1] == 0){
					ex[1] = dd*EMA + (1.0 - EMA)*ex[1];
					DEBUG_PRINT(F("\n("));
					DEBUG_PRINT(1);
					DEBUG_PRINT(F(") minx sensor: "));
					DEBUG_PRINT(minx);
					DEBUG_PRINT(F(" - maxx sensor: "));
					DEBUG_PRINT(maxx);
					DEBUG_PRINT(F(" - Mean sensor: "));
					DEBUG_PRINT(m);
					DEBUG_PRINT(F(" - Peak: "));
					DEBUG_PRINT(ex[1]);
					DEBUG_PRINT(F(" - ADC enable: "));
					DEBUG_PRINT(dosmpl);
					//DEBUG_PRINT(F("\nSample count: "));
					//DEBUG_PRINTLN(sampleCount);
					//sampleCount = 0;
					chk[1] = checkRange((double) ex[1]*(1 - weight[0]*isMoving(0)),1);
					if(chk[1] != 0){
						DEBUG_PRINT(F("\n("));
						DEBUG_PRINT(1);
						if(chk[1] == -1){
							DEBUG_PRINTLN(F(") Stop: sottosoglia"));
							//fine dorsa raggiunto
							blocked[1] = secondPress(1,40,true);
							scriviOutDaStato();
						}else if(chk[1] == 2){
							DEBUG_PRINTLN(F(") Stop: soprasoglia"));
							blocked[1] = secondPress(1,40);
							scriviOutDaStato();
							blocked[1] = 1;
						}else if(chk[1] == 1){
							DEBUG_PRINTLN(F(") Start: fronte di salita"));	
							ex[1] = getAVG(1);
							//inizio conteggio timer di posizionamento
							startEndOfRunTimer(1);
						}
						readStatesAndPub();
						yield();
					}
				}else{
					//DEBUG_PRINT(F("\n("));
					//DEBUG_PRINT(1);
					//DEBUG_PRINT(F(") aspetto: "));
					//DEBUG_PRINT(isrundelay[1]);
					//DEBUG_PRINT(F(" - minx sensor: "));
					//DEBUG_PRINT(minx);
					//DEBUG_PRINT(F(" - maxx sensor: "));
					//DEBUG_PRINT(maxx);
					//DEBUG_PRINT(F(" - Peak: "));
					//DEBUG_PRINT(ex[1]);
					isrundelay[1]--;
					ex[1] = dd;
					//DEBUG_PRINT(F(" - dd: "));
					//DEBUG_PRINT(dd);
				}
			}else{
				isrundelay[1] = RUNDELAY;
				//reset dei fronti su blocco marcia (sia manuale che automatica)
				resetEdges(1);
			}
			//AC peak measure init
			//indx = 0;
			minx = 1024;
			maxx = 0;
			dosmpl = true;
			//DEBUG_PRINT(F("\n------------------------------------------------------------------------------------------"));
		}else{
#if (AUTOCAL_ACS712) 
			//zero detection manager
			//zero detection scheduler
			zeroCnt = (zeroCnt + 1) % 50;
			dosmpl = false;
			//all motors are stopped
			if(zeroCnt < 3){
				//zero detection activation (2 values every second)
				system_soft_wdt_stop();
				ets_intr_lock( ); //close interrupt
				noInterrupts();
				x = system_adc_read();
				interrupts();
				ets_intr_unlock(); //open interrupt
				system_soft_wdt_restart();
				//running mean calculation
				smplcnt++;
				smplcnt && (m += (float) (x - m) / smplcnt);  //protected against overflow by a logic short circuit
				//DEBUG_PRINT(F("\nZero peak sensor: "));
				//DEBUG_PRINT(x);
				//DEBUG_PRINT(F(" - Zero mean sensor: "));
				//DEBUG_PRINT(m);
			}
#endif
		}	
}

inline void wifiFailoverManager(){
	//wifi failover management
	//DEBUG_PRINTLN(wl_status_to_string(wfs));
	if(WiFi.getMode() == WIFI_OFF || WiFi.getMode() == WIFI_STA || WiFi.getMode() == WIFI_AP_STA){
		if(!wifiConn){
			//lampeggia led di connessione
			digitalWrite(OUTSLED, !digitalRead(OUTSLED));
			//yield();
			DEBUG_PRINT(F("\nSwcount roll: "));
			DEBUG_PRINTLN(swcount);
			
			if((swcount == 0)){
				DEBUG_PRINTLN(F("Connection timed out"));
				WiFi.persistent(false);
				WiFi.disconnect(true);
				WiFi.mode(WIFI_OFF);    
				DEBUG_PRINTLN(F("Do new connection"));
				WiFi.setAutoReconnect(true);	
				WiFi.mode(WIFI_STA);
				WiFi.setAutoReconnect(true);
				WiFi.config(0U, 0U, 0U);
				setup_wifi(wifindx);	//tetativo di connessione
				//wifi_station_dhcpc_start();
				//WiFi.waitForConnectResult();	//non necessaria se il loop ha sufficienti slot di idle!
				stat = WiFi.status();
				wifiConn = (stat == WL_CONNECTED);
				wifindx = (wifindx +1) % 2; //0 or 1 are the index of the two alternative SSID
				//if(wifiConn)
				//	mqttReconnect();
			}
			swcount = (swcount + 1) % TCOUNT;
		}else{
			digitalWrite(OUTSLED, LOW);
			confcmd[LOCALIP] = WiFi.localIP().toString();
		}
	}
}

inline void MQTTReconnectManager(){
	//MQTT reconnect management
			//a seguito di disconnessioni accidentali tenta una nuova procedura di riconnessione
			if(wifiConn && mqttClient!=NULL){
				//noInterrupts ();
				byte mqttStat = mqttClient->isConnected();
				//interrupts ();
				//delay(50);
				//noInterrupts ();
				//mqttStat = mqttClient->isConnected();
				//interrupts ();
				//DEBUG_PRINT(F("MQTT check : "));
				//DEBUG_PRINT(mqttStat);
				if(!mqttStat){
					DEBUG_PRINT(F("\nMQTT dice non sono connesso."));
					mqttConnected=false;
				}	
				else{
					//DEBUG_PRINT(F("\nMQTT  dice sono connesso. Local IP: "));
					//DEBUG_PRINT(WiFi.localIP());
					mqttConnected=true;
					mqttcnt = 0;
					mqttofst = 2;
				}
			}
			else
			{
				mqttConnected=false;
			}
			
			if((wifiConn == true)&&(!mqttConnected) && WiFi.status()==WL_CONNECTED && WiFi.getMode()==WIFI_STA) {
				mqttcnt++;
				
				if(mqttClient==NULL){
					DEBUG_PRINTLN(F("ERROR! MQTT client is not allocated."));
					mqttReconnect();
					mqttcnt = 0;
					mqttofst = 2;
				}else if(mqttcnt == 20){
					DEBUG_PRINTLN(F("ERROR! MQTT client is not allocated."));
					mqttofst = 4;
				}else if(mqttcnt == 40){
					mqttofst = 8;
				}else if(mqttcnt == 100){
					mqttofst = 10;
				}else{
					if(!(mqttcnt % mqttofst)){
						//non si può fare perchè dopo pochi loop crasha
						if(dscnct){
							dscnct=false;
							DEBUG_PRINT(F("eseguo la MQTT connect()...Cnt: "));
							//noInterrupts ();
							mqttClient->connect();
							//interrupts ();
							delay(50);
						}
						else
						{
							dscnct=true;
							DEBUG_PRINT(F("eseguo la MQTT disconnect()...Cnt: "));
							//noInterrupts ();
							mqttClient->disconnect();
							//interrupts ();
							delay(50);
						}
						DEBUG_PRINT(mqttcnt);
						DEBUG_PRINT(F(" - Passo: "));
						DEBUG_PRINTLN(mqttofst);
					}
					//non si può fare senza disconnect perchè dopo pochi loop crasha
					//mqttClient->setUserPwd((confcmd[MQTTUSR]).c_str(), (confcmd[MQTTPSW]).c_str());
				}
			}
}

inline void paramsModificationPoll(){
	//actions on parametrs saving
		//is else if per gestione priorità, l'ordine è importante! vanno fatti in momenti successivi
		if(confcmd[WIFICHANGED]=="true"){
			confcmd[WIFICHANGED]="false";
			wifindx=0;
			Serial.println(F("Doing WiFi disconnection"));
			WiFi.persistent(false);
			WiFi.disconnect(false);
			WiFi.mode(WIFI_OFF);    
			//WiFi.mode(WIFI_STA);
			wifindx = 0;
		}
	
		if(confcmd[MQTTADDRMODFIED]=="true"){
			confcmd[MQTTADDRMODFIED]="false";
			DEBUG_PRINTLN(F("confcmd[MQTTADDRMODFIED] eseguo la reconnect()"));
			mqttReconnect();
		}else if(confcmd[MQTTCONNCHANGED]=="true"){
			confcmd[MQTTCONNCHANGED]="false";
			if(mqttClient==NULL){
				DEBUG_PRINTLN(F("ERROR confcmd[TOPICCHANGED]! MQTT client is not allocated."));
				mqttReconnect();
			}
			else
			{
			#if defined (_DEBUG1) || defined (_DEBUGR)	
				DEBUG_PRINTLN(F("MQTTCONNCHANGED! Eseguo la setUserPwd() con usr "));
				DEBUG_PRINTLN(confcmd[MQTTUSR]);
				DEBUG_PRINTLN(F(" e psw "));
				DEBUG_PRINTLN(confcmd[MQTTPSW]);
			#endif		
				mqttClient->setUserPwd((confcmd[MQTTUSR]).c_str(), (confcmd[MQTTPSW]).c_str());
				DEBUG_PRINTLN(F("MQTTCONNCHANGED! Eseguo la connect() ..."));
				mqttClient->connect();
			#if defined (_DEBUG1) || defined (_DEBUGR)	
				DEBUG_PRINTLN(F("MQTTCONNCHANGED! Eseguo la subscribe() con "));
				DEBUG_PRINTLN(confcmd[MQTTINTOPIC]);
				DEBUG_PRINTLN(F("..."));
			#endif	
				mqttClient->subscribe(confcmd[MQTTINTOPIC]);
			#if defined (_DEBUG1) || defined (_DEBUGR)	
				DEBUG_PRINTLN(F("MQTTCONNCHANGED! publish(): "));
				DEBUG_PRINTLN(confcmd[MQTTOUTTOPIC]);
				DEBUG_PRINTLN(F(" Intopic: "));
				DEBUG_PRINTLN(confcmd[MQTTINTOPIC]);
			#endif		
				mqttClient->publish(confcmd[MQTTOUTTOPIC], confcmd[MQTTID]);
			}
		}else if(confcmd[TOPICCHANGED]=="true"){
			confcmd[TOPICCHANGED]="false";
			if(mqttClient==NULL){
				DEBUG_PRINTLN(F("ERROR confcmd[TOPICCHANGED]! MQTT client is not allocated."));
				mqttReconnect();
			}
			else
			{
			#if defined (_DEBUG1) || defined (_DEBUGR)	
				DEBUG_PRINTLN(F("TOPICCHANGED! Outtopic: "));
				DEBUG_PRINTLN(confcmd[MQTTOUTTOPIC]);
				DEBUG_PRINTLN(F(" Intopic: "));
				DEBUG_PRINTLN(confcmd[MQTTINTOPIC]);
				DEBUG_PRINTLN(F("TOPICCHANGED! Eseguo la subscribe() con "));
				DEBUG_PRINTLN(confcmd[MQTTINTOPIC]);
				DEBUG_PRINTLN(F("..."));
			#endif		
				mqttClient->subscribe(confcmd[MQTTINTOPIC]);
			#if defined (_DEBUG1) || defined (_DEBUGR)	
				DEBUG_PRINTLN(F("TOPICCHANGED! Eseguo la publish() con "));
				DEBUG_PRINTLN(confcmd[MQTTOUTTOPIC]);
				DEBUG_PRINTLN(F(" ..."));
			#endif		
				mqttClient->publish(confcmd[MQTTOUTTOPIC], confcmd[MQTTID]);
			}
		}
}

//-----------------------------------------------INIZIO TIMER----------------------------------------------------------------------
//azione da compiere allo scadere di uno dei timer dell'array	
void onElapse(byte nn){
	int n = nn / TIMERDIM;
	int sw = nn % TIMERDIM;
	
	DEBUG_PRINT(F("\nElapse timer n: "));
	DEBUG_PRINT(nn);
	DEBUG_PRINT(F("  con stato: "));
	DEBUG_PRINTLN(getGroupState(nn));
	DEBUG_PRINT(F("  con n: "));
	DEBUG_PRINTLN(n);
	DEBUG_PRINT(F("  con sw: "));
	DEBUG_PRINTLN(sw);
	
	if(nn != RESETTIMER || nn != APOFFTIMER) //se è scaduto il timer di attesa o di blocco  (0,1) --> state n
	{   
		DEBUG_PRINT(F("\nCount value: "));
		DEBUG_PRINTLN(getCntValue(nn));
		if(getCntValue(nn) == 1){ 
			if(roll[n]){//se è in modalità tapparella!
				if(getGroupState(nn)==3){ //il motore e in moto cronometrato scaduto (timer di blocco scaduto)
					DEBUG_PRINTLN(F("stato 0 roll mode: il motore va in stato fermo da fine corsa (TIMER ELAPSED!)"));
					secondPress(n);
					//comanda gli attuatori per fermare (non lo fa il loop stavolta!)
					scriviOutDaStato();
					//pubblica lo stato finale su MQTT (non lo fa il loop stavolta!)
					readStatesAndPub();
				}else if(getGroupState(nn)==1){	//se il motore era in attesa di partire (timer di attesa scaduto)
					DEBUG_PRINTLN(F("onElapse roll mode:  timer di attesa scaduto"));
					startEngineDelayTimer(n);
					//adesso parte...
					scriviOutDaStato();
				}
	#if (!AUTOCAL)	
				else if(getGroupState(nn)==2){//se il motore è in moto a vuoto
					DEBUG_PRINTLN(F("onElapse roll mode manual:  timer di corsa a vuoto scaduto"));
					///setGroupState(3,n);	//il motore va in moto cronometrato
					startEndOfRunTimer(n);
					//pubblica lo stato di UP o DOWN attivo su MQTT (non lo fa il loop stavolta!)
					readStatesAndPub();
				}
	#else
				else if(getGroupState(nn)==2){//se il motore è in moto a vuoto
					DEBUG_PRINTLN(F("onElapse roll mode autocal:  timer di check pressione su fine corsa scaduto"));
					secondPress(n,0,true);
					//comanda gli attuatori per fermare (non lo fa il loop stavolta!)
					scriviOutDaStato();
				}
#endif
			}else{//se è in modalità switch
				if(getGroupState(nn)==1){//se lo switch era inibito (timer di attesa scaduto)
					DEBUG_PRINTLN(F("onElapse switch mode:  timer di attesa scaduto"));
					startSimpleSwitchDelayTimer(nn);
					//adesso commuta...
				}else if(getGroupState(nn)==2){ //se lo switch è monostabile (timer di eccitazione scaduto)
					DEBUG_PRINTLN(F("stato 0 switch mode: il motore va in stato fermo da fine corsa (TIMER ELAPSED!)"));
					endPress(nn);
				}
				//comanda gli attuatori per fermare (non lo fa il loop stavolta!)
				scriviOutDaStato();
				//pubblica lo stato finale su MQTT (non lo fa il loop stavolta!)
				readStatesAndPub();
			}
		}else if(getCntValue(nn) > 1){ //in tutte le modalità
			if(n == 0){
				DEBUG_PRINTLN(F("onElapse:  timer 1 dei servizi a conteggio scaduto"));
				if(getCntValue(CNTSERV1)==3){
					wifiState = WIFIAP;
					//wifi_station_dhcpc_stop();
					//WiFi.enableAP(true);
					//wifi_softap_dhcps_start();
					DEBUG_PRINTLN(F("-----------------------------"));
					DEBUG_PRINTLN(F("Attivato AP mode"));
					DEBUG_PRINTLN(F("-----------------------------"));
					startTimer(APOFFTIMER);
					//WiFi.enableSTA(false);
					DEBUG_PRINTLN(F("AP mode on"));
					//WiFi.setAutoConnect(false);
					//WiFi.setAutoReconnect(false);	
					//ETS_UART_INTR_DISABLE();
					if(!WiFi.isConnected()){
						// disconnect sta, start ap
						WiFi.persistent(false);      
						WiFi.disconnect();  //cancella la connessione corrente memorizzata
						WiFi.mode(WIFI_AP);
						WiFi.persistent(true);
						DEBUG_PRINTLN(F("SET AP"));
						wifiConn = false;
					}else{
						WiFi.mode(WIFI_AP_STA);
						DEBUG_PRINTLN(F("SET AP STA"));
					}
					setup_AP(true);
					delay(500);
					yield();
					//WiFi.persistent(true);
					//setup_AP(true);
					//WiFi.printDiag(Serial);
					//wifi_station_dhcpc_stop();
					//interrupts ();
					//WiFi.enableAP(true); //is STA + AP
					//setup_AP();
					//confcmd[LOCALIP] = WiFi.softAPIP().toString();
					//MDNS.notifyAPChange()();
					//wifi_softap_dhcps_start();
					//delay(100);
					//WiFi.printDiag(Serial);
					//MDNS.update();
					setGroupState(0,n%2);
				}else if(getCntValue(CNTSERV1)==4){
					DEBUG_PRINTLN(F("-----------------------------"));
					DEBUG_PRINTLN(F("Rebooting ESP without reset of configuration"));
					DEBUG_PRINTLN(F("-----------------------------"));
					ESP.eraseConfig(); //do the erasing of wifi credentials
					ESP.restart();
				}else if(getCntValue(CNTSERV1)==5 && roll[n]){ //solo in modalità tapparella!
					DEBUG_PRINTLN(F("-----------------------------"));
					DEBUG_PRINTLN(F("ATTIVATA CALIBRAZIONE MANUALE BTN 1"));
					DEBUG_PRINTLN(F("-----------------------------"));
					manualCalibration(0); //BTN1
				}else if(getCntValue(CNTSERV1)==8){
					DEBUG_PRINTLN(F("-----------------------------"));
					DEBUG_PRINTLN(F("Reboot ESP with reset of configuration"));
					DEBUG_PRINTLN(F("-----------------------------"));
					rebootSystem();
				}else{
					setGroupState(0,nn);
				}
				//DEBUG_PRINT(F("Resettato contatore dei servizi: "));
				resetCnt(CNTSERV1);
			}else if(roll[n]){ //solo in modalità tapparella!
				DEBUG_PRINTLN(F("onElapse:  timer 2 dei servizi a conteggio scaduto"));
				if(getCntValue(CNTSERV2)==5){
					DEBUG_PRINTLN(F("-----------------------------"));
					DEBUG_PRINTLN(F("ATTIVATA CALIBRAZIONE MANUALE BTN 2"));
					DEBUG_PRINTLN(F("-----------------------------"));
					manualCalibration(1); //BTN2
				}else{
					setGroupState(0,nn);
				}
				//DEBUG_PRINT(F("Resettato contatore dei servizi: "));
				resetCnt(CNTSERV2);
			}
		}
	}else if(nn == RESETTIMER)
		{
			rebootSystem();
	}else if(nn == APOFFTIMER){
		if(WiFi.softAPgetStationNum() == 0){
			DEBUG_PRINTLN(F("WIFI: reconnecting to AP"));
			byte stat = WiFi.status();
			wifiConn = (stat == WL_CONNECTED);	
			WiFi.mode(WIFI_STA);	
			wifiState = WIFISTA;
			setup_wifi(wifindx);
			//mqttReconnect();
			//wifi_station_dhcpc_start();
			DEBUG_PRINTLN(F("-----------------------------"));
			DEBUG_PRINTLN(F("Nussun client si è ancora connesso, disatttivato AP mode"));
			DEBUG_PRINTLN(F("-----------------------------"));
		}
		//setGroupState(0,n%2);				 								//stato 0: il motore va in stato fermo
		DEBUG_PRINTLN(F("stato 0: il motore va in stato fermo da stato configurazione"));
	}
	
	//DEBUG_PRINTLN(F("Fine timer"));
}

void onTapStop(byte n){
#if (AUTOCAL)
	resetStatDelayCounter(n);
#endif
}
		
void onCalibrEnd(unsigned long app, byte n){		
	confcmd[haltPrm[n]] = String(app);
	//initTapparellaLogic(in,inr,outLogic,(confcmd[THALT1]).toInt(),(confcmd[THALT2]).toInt(),(confcmd[STDEL1]).toInt(),(confcmd[STDEL2]).toInt(),BTNDEL1,BTNDEL2);
	setTapThalt((confcmd[THALT1 + n]).toInt(), n);
	DEBUG_PRINTLN(F("-----------------------------"));
#if (AUTOCAL)
	calAvg[n] = getAVG(n);
	weight[0] = (double) calAvg[0] / (calAvg[0] +  calAvg[1]);
	weight[1] = (double) calAvg[1] / (calAvg[0] +  calAvg[1]);
	confcmd[VALWEIGHT] = String(weight[0]);
	updateUpThreshold(n);
	//confcmd[TRSHOLD1 + n] = String(getThresholdUp(n));
	//setThresholdUp((confcmd[TRSHOLD1 + n]).toFloat(), n);
#endif
	EEPROM.begin(FIXEDPARAMSLEN);
	
	EEPROMWriteStr(VALWEIGHTOFST,(confcmd[VALWEIGHT]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified current weight "));
	DEBUG_PRINTLN(confcmd[VALWEIGHT]);
	
	EEPROMWriteInt(haltOfs[n], app);
	EEPROM.commit();
	DEBUG_PRINT(F("Modified THALT "));
	DEBUG_PRINTLN(haltPrm[n]);
	DEBUG_PRINT(F(": "));
	DEBUG_PRINTLN(confcmd[haltPrm[n]]);
	EEPROM.end();
	//deactivate the learning of the running statistics
	//clrStatsLearnMode();
}

void manualCalibration(byte btn){
	setGroupState(0,btn);	
	//activate the learning of the running statistics
	//setStatsLearnMode();
#if (AUTOCAL)
	//resetStatDelayCounter(btn);
	disableUpThreshold(btn);
#endif
	inr[BTN2IN + btn*BTNDIM] = 101;			//codice comando attiva calibrazione
	
	DEBUG_PRINTLN(F("-----------------------------"));
	DEBUG_PRINT(F("FASE 1 CALIBRAZIONE MANUALE BTN "));
	DEBUG_PRINTLN(btn+1);
	DEBUG_PRINTLN(F("-----------------------------"));
	//-------------------------------------------------------------
	DEBUG_PRINTLN(F("LA TAPPARELLA STA SCENDENDO......"));
	DEBUG_PRINT(F("PREMERE UN PULSANTE QUALSIASI DEL GRUPPO "));
	DEBUG_PRINTLN(btn+1);
	DEBUG_PRINTLN(F("-----------------------------"));
}

void rebootSystem(){
	EEPROM.begin(FIXEDPARAMSLEN);
	alterEEPROM();
	EEPROM.end();
	DEBUG_PRINTLN(F("Resetting ESP"));
	ESP.eraseConfig(); //do the erasing of wifi credentials
	ESP.restart();
}
//----------------------------------------------------FINE TIMER----------------------------------------------------------------------
void onStationConnected(const WiFiEventSoftAPModeStationConnected& evt) {
	DEBUG_PRINT(F("\nAP mode: Station connected: "));
	DEBUG_PRINTLN(macToString(evt.mac));
	if(WiFi.softAPgetStationNum() == 1){
		boot = true;
		wifiState = WIFIAP;
	}
}

void onStationDisconnected(const WiFiEventSoftAPModeStationDisconnected& evt) {
	DEBUG_PRINTLN(F("\nStation disconnected: "));
	DEBUG_PRINTLN(macToString(evt.mac));
	if(WiFi.softAPgetStationNum() == 0){
		resetTimer(APOFFTIMER);
		DEBUG_PRINTLN(F("WIFI: reconnecting to AP"));
		byte stat = WiFi.status();
		wifiConn = (stat == WL_CONNECTED);	
		WiFi.mode(WIFI_STA);	
		wifiState = WIFISTA;
		setup_wifi(wifindx);
		//mqttReconnect();
	}
}

String macToString(const unsigned char* mac) {
  char buf[20];
  snprintf(buf, sizeof(buf), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return String(buf);
}

#if(LARGEFW)
void processCmdRemoteDebug() {
	String lastCmd = telnet.getLastCommand();
	
	if(lastCmd == "showconf"){
		// overall config print
		printConfig();
	}else if(lastCmd == "reboot"){
		// Rebooting ESP without reset of configuration
		DEBUG_PRINT(F("\nRebooting ESP without reset of configuration"));
		ESP.restart();
	}else if(lastCmd == "reset"){
		//Reboot ESP with reset of configuration
		DEBUG_PRINT(F("\nReboot ESP with reset of configuration"));
		rebootSystem();
	}else if(lastCmd == "calibrate1"){
		//ATTIVATA CALIBRAZIONE MANUALE BTN 1
		manualCalibration(0);
	}else if(lastCmd == "calibrate2"){
		//ATTIVATA CALIBRAZIONE MANUALE BTN 1
		manualCalibration(1);
	}else if(lastCmd == "apmodeon"){
		DEBUG_PRINT(F("\nAtttivato AP mode"));
		startTimer(APOFFTIMER);
	}else if(lastCmd == "scanwifi"){
		//scansione reti wifi disponibili
		scan_wifi();
	}else if(lastCmd == "getip"){
		DEBUG_PRINT("\nLocal IP: ");
		DEBUG_PRINTLN(WiFi.localIP());
	}else if(lastCmd == "getmqttstat"){
		if(!(mqttClient->isConnected())){
			DEBUG_PRINTLN(F("\nMQTT non connesso."));
		}
		else
		{
			DEBUG_PRINTLN(F("\nMQTT connesso"));
		}
	}else if(lastCmd == "gettemp"){
		DEBUG_PRINT(F("\nTemperature: "));
		DEBUG_PRINT(asyncBuf[GTTEMP]);
	}else if(lastCmd == "getadczero"){
		DEBUG_PRINT(F("\nMean sensor: "));
		DEBUG_PRINTLN(m);
	}else if(lastCmd == "getpower"){
		DEBUG_PRINT(F("\nAvg power: ["));
		DEBUG_PRINT(asyncBuf[GTMEANPWR1]);
		DEBUG_PRINT(F(","));
		DEBUG_PRINT(asyncBuf[GTMEANPWR2]);
		DEBUG_PRINTLN(F("]"));
		DEBUG_PRINT(F("\nPeak power: ["));
		DEBUG_PRINT(asyncBuf[GTPEAKPWR1]);
		DEBUG_PRINT(F(","));
		DEBUG_PRINT(asyncBuf[GTPEAKPWR2]);
		DEBUG_PRINTLN(F("]")); 
	}else if(lastCmd == "getmac"){
		DEBUG_PRINT(F("\nMAC: "));
		DEBUG_PRINTLN(WiFi.macAddress());
	}else if(lastCmd == "gettime"){
		DEBUG_PRINT(F("\nTime (ms): "));
		DEBUG_PRINTLN(millis());
	}else if(lastCmd == "getmqttid"){
		DEBUG_PRINT(F("\nMQTT ID: "));
		DEBUG_PRINTLN(confcmd[MQTTID]);
	}else if(lastCmd == "testflash"){
		testFlash();
	}else{
		DEBUG_PRINT(F("\nComandi disponibili: "));
		DEBUG_PRINT(F("\nshowconf, reboot, reset, calibrate1, calibrate2, apmodeon, scanwifi, getip, getmqttstat, getadczero, gettemp, getpower, getmac, gettime, getmqttid, testflash\n"));
	}
	//telnet.flush();
}
#endif


#if defined (_DEBUG1) || defined (_DEBUGR)	
const char* wl_status_to_string(wl_status_t status) {
  switch (status) {
    case WL_NO_SHIELD: return "WL_NO_SHIELD";
    case WL_IDLE_STATUS: return "WL_IDLE_STATUS";
    case WL_NO_SSID_AVAIL: return "WL_NO_SSID_AVAIL";
    case WL_SCAN_COMPLETED: return "WL_SCAN_COMPLETED";
    case WL_CONNECTED: return "WL_CONNECTED";
    case WL_CONNECT_FAILED: return "WL_CONNECT_FAILED";
    case WL_CONNECTION_LOST: return "WL_CONNECTION_LOST";
    case WL_DISCONNECTED: return "WL_DISCONNECTED";
  }
}
#endif

#if defined (_DEBUG1) || defined (_DEBUGR)	
void testFlash(){
  uint32_t realSize = ESP.getFlashChipRealSize();
  uint32_t ideSize = ESP.getFlashChipSize();
  FlashMode_t ideMode = ESP.getFlashChipMode();
  char s[100];
  
  sprintf(s,"\nFlash real id:   %08X\n", ESP.getFlashChipId());
  DEBUG_PRINT(s);
  sprintf(s,"Flash real size: %u bytes\n", realSize);
  DEBUG_PRINT(s);
  sprintf(s,"Flash ide  size: %u bytes\n", ideSize);
  DEBUG_PRINT(s);
  sprintf(s,"Flash ide speed: %u Hz\n", ESP.getFlashChipSpeed());
  DEBUG_PRINT(s);
  sprintf(s,"Flash ide mode:  %s\n", (ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN"));
  DEBUG_PRINT(s);

  if (ideSize != realSize) {
    DEBUG_PRINT("\nFlash Chip configuration wrong!\n");
  } else {
    DEBUG_PRINT("\nFlash Chip configuration ok.\n");
  }
}
#endif

/**
 * Disconnect from the network
 * @param wifioff
 * @return  one value of wl_status_t enum
 */
/*
bool ESP8266WiFiSTAClass::disconnect(bool wifioff) {
    bool ret;
    struct station_config conf;
    *conf.ssid = 0;
    *conf.password = 0;

    ETS_UART_INTR_DISABLE();
    if(WiFi._persistent) {
        wifi_station_set_config(&conf);
    } else {
        wifi_station_set_config_current(&conf);
    }
    ret = wifi_station_disconnect();
    ETS_UART_INTR_ENABLE();

    if(wifioff) {
        WiFi.enableSTA(false);
    }

    return ret;
}*/
/*
  uint8_t ESP8266WiFiSTAClass::waitForConnectResult() {

    //1 and 3 have STA enabled

    if((wifi_get_opmode() & 1) == 0) {

        return WL_DISCONNECTED;

    }

    while(status() == WL_DISCONNECTED) {

        delay(100);

    }

    return status();

	}
	wl_status_t ESP8266WiFiSTAClass::status() {

    station_status_t status = wifi_station_get_connect_status();



    switch(status) {

        case STATION_GOT_IP:

            return WL_CONNECTED;

        case STATION_NO_AP_FOUND:

            return WL_NO_SSID_AVAIL;

        case STATION_CONNECT_FAIL:

        case STATION_WRONG_PASSWORD:

            return WL_CONNECT_FAILED;

        case STATION_IDLE:

            return WL_IDLE_STATUS;

        default:

            return WL_DISCONNECTED;

    }

}

typedef enum { WL_NO_SHIELD = 255, 
// for compatibility with WiFi Shield library WL_IDLE_STATUS = 0, 
WL_NO_SSID_AVAIL = 1, 
WL_SCAN_COMPLETED = 2, 
WL_CONNECTED = 3, 
WL_CONNECT_FAILED = 4,
 WL_CONNECTION_LOST = 5, 
 WL_DISCONNECTED = 6 } wl_status_t;
 
 typedef enum WiFiMode 
{
    WIFI_OFF = 0, WIFI_STA = 1, WIFI_AP = 2, WIFI_AP_STA = 3
} WiFiMode_t;
*/
