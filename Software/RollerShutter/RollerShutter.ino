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
char IP[] = "xxx.xxx.xxx.xxx";          // buffer
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
//int haltPrm[2] = {THALT1,THALT2};
//int haltOfs[2] = {THALT1OFST,THALT2OFST};
byte blocked[2]={0,0};
unsigned long edelay[2]={0,0};
byte wsnconn = 0;
IPAddress myip(192, 168, 43, 1);
IPAddress mygateway(192, 168, 43, 1);
IPAddress mysubnet(255, 255, 255, 0);
bool roll[2] = {true, true};

byte cols[] = {7,4,2,1,6,5,3,0};
byte ledcnt = 0;
byte ledslct = 0;
byte ledpause = 0;

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
//Valore iniziale: il suo contenuto viene poi caricato da EEPROM
unsigned int thalt1=5000;
unsigned int thalt3=5000;
unsigned int thalt2=5000;
unsigned int thalt4=5000;
//End JSON config------------------------------------------
//JSON config----------------------------------------------
//{"OUTSLED":"0","up1":"1","down1":"0","up2":"50","down2":"0", pr1:"12", pr2:"76"}
int ncifre=4;
//vettori di ingresso, uscite e stato
byte in[NBTN*BTNDIM], outPorts[NBTN*BTNDIM], out[NBTN*BTNDIM];
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
#if (MCP2317) 
	Adafruit_MCP23017 mcp;
    void MCP2317_init(){
// Connect pin #12 of the expander to Analog 5 (i2c clock)
// Connect pin #13 of the expander to Analog 4 (i2c data)
// Connect pins #15, 16 and 17 of the expander to ground (address selection)
// Connect pin #9 of the expander to 5V (power)
// Connect pin #10 of the expander to ground (common ground)
// Connect pin #18 through a ~10kohm resistor to 5V (reset pin, active low)
// Output #0 is on pin 21 so connect an LED or whatever from that to ground
		mcp.begin();      		// use default address 0
	}	
#endif
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

void calibrate_pwr() {
	// Let some time to register values
	//unsigned long timeout = millis();
	//while ((millis() - timeout) < 10000) {
	//	delay(1);
	//}

	// Calibrate using a 60W bulb (pure resistive) on a 230V line
	hlw8012.expectedActivePower(static_cast<ParFloat*>(pars[p(CALPWR)])->val);
	hlw8012.expectedVoltage(static_cast<Parbyte*>(pars[p(ACVOLT)])->val);
	hlw8012.expectedCurrent((float) static_cast<ParFloat*>(pars[p(CALPWR)])->val / static_cast<ParByte*>(pars[p(ACVOLT)]->val));
	//Save parameter in the system array
	pars[p(PWRMULT)]->load(hlw8012.getPowerMultiplier());
	
	// Show corrected factors
	DEBUG_PRINT(F("[HLW] New current multiplier : ")); DEBUG_PRINTLN(hlw8012.getCurrentMultiplier());
	DEBUG_PRINT(F("[HLW] New voltage multiplier : ")); DEBUG_PRINTLN(hlw8012.getVoltageMultiplier());
	DEBUG_PRINT(F("[HLW] New power multiplier   : ")); DEBUG_PRINTLN(hlw8012.getPowerMultiplier());
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
		DEBUG_PRINT(F("[HLW] New current multiplier : ")); DEBUG_PRINTLN(hlw8012.getCurrentMultiplier());
		DEBUG_PRINT(F("[HLW] New voltage multiplier : ")); DEBUG_PRINTLN(hlw8012.getVoltageMultiplier());
		DEBUG_PRINT(F("[HLW] New power multiplier   : ")); DEBUG_PRINTLN(hlw8012.getPowerMultiplier());

		setInterrupts();
}
#endif

#if (MCP2317) 
inline void getbits(byte num, byte &r, byte &g, byte &b) 
{ 
    r = (num >> 2) & 1;
	g = (num >> 1) & 1;
	b = num & 1;
}
	
inline void setColor(byte num) 
{   
	byte r,g,b;

    getbits(num, r, g, b);
	mcp.digitalWrite(GREEN,g);	
	mcp.digitalWrite(RED,r);		
	mcp.digitalWrite(BLUE,b);
} 

void printOut(){ 
	if(ledpause == 0){
		if(ledcnt > 0){
			if((ledcnt % 2) == 0)
				(mcp.digitalRead(ledslct+BTN1U))?setColor(cols[1]):setColor(cols[0]);
			else
				setColor(cols[7]);
			ledcnt--;
		}else if(ledcnt == 0){
			if(ledslct < 4){
				ledslct++;
				ledcnt = 2*(ledslct+1);
				ledpause = 4;
			}else{
				ledpause = -1;
			}
		}
	}else if(ledpause > 0){
		ledpause--;
	}
}

void rstldcnt(byte n){   
	ledcnt = 2*(n+1);
	ledslct = n;
	ledpause = 0;
} 

#endif

void testparam(int i){
	char * param;
	
	if(pars[i] != NULL){
		param = pars[i]->formname;
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
	/*1*/pars[MQTTUP1] = new ParByte(0, "up1", "up1", MQTTUP1OFST, 'j','i', new MQTTBTN_Evnt(MQTTUP1));
	/*2*/pars[MQTTDOWN1] = new ParByte(0, "down1", "down1", MQTTDOWN1OFST, 'j','i', new MQTTBTN_Evnt(MQTTDOWN1));
	/*3*/pars[MQTTUP2] = new ParByte(0, "up2", "up2", MQTTUP2OFST, 'j','i', new MQTTBTN_Evnt(MQTTUP2));
	/*4*/pars[MQTTDOWN2] = new ParByte(0, "down2", "down2", MQTTDOWN2OFST, 'j','i', new MQTTBTN_Evnt(MQTTDOWN2));
	/*4*/pars[MQTTALL] = new ParByte(0, "all","all", 0,'n','n');
	/*4*/pars[MQTTMAC] = new ParByte(0, "mac", "mac", 0,'n','n', new MQTTMAC_Evnt());
	/*4*/pars[MQTTIP] = new ParByte(0, "ip", "ip", 0,'n','n', new MQTTIP_Evnt());
	/*4*/pars[MQTTMQTTID] = new ParByte(0, "mqttid", "mqttid", 0,'n','n', new MQTTMQTTID_Evnt());
	/*4*/pars[MQTTTEMP] = new ParByte(0, "temp", "temp", 0, 'n','n', new MQTTTEMP_Evnt());
	/*4*/pars[MQTTMEANPWR] = new ParByte(0, "avgpwr", "avgpwr", 0,'n','n', new MQTTMEANPWR_Evnt());
	/*4*/pars[MQTTPEAKPWR] = new ParByte(0, "peakpwr", "peakpwr", 0,'n','n', new MQTTPEAKPWR_Evnt());
	/*4*/pars[MQTTTIME] = new ParByte(0, "time", "time", 0, 'n','n', new MQTTTIME_Evnt());
	/*4*/pars[MQTTDATE] = new ParByte(0, "date", "date", 0, 'n','n');
#if (AUTOCAL_HLW8012) 
	/*4*/pars[DOPWRCAL] = new ParByte(0, "dopwrcal", "dopwrcal", 0, 'n','n', new DOPWRCAL_Evnt());
	/*4*/pars[INSTPWR] = new ParByte(0, "ipwr", "ipwr", 0,'n','n', new INSTPWR_Evnt());
	/*4*/pars[INSTACV] = new ParByte(0, "iacvolt", "iacvolt", 0,'n','n', new INSTACV_Evnt());
	//------------------------------------------------------------------------------------------------------------------------------------
	/*42*/pars[p(CALPWR)] = new ParFloat(60, "calpwr", "calpwr", CALPWROFST, 'p','i');
#else
	/*4*/pars[DOPWRCAL] = new ParByte(0, "dopwrcal", "dopwrcal", 0, 'n','n');
	/*4*/pars[INSTPWR] = new ParByte(0, "ipwr", "ipwr", 0,'n','n');
	/*4*/pars[INSTACV] = new ParByte(0, "iacvolt", "iacvolt", 0,'n','n');
	/*42*/pars[p(CALPWR)] = new ParFloat(60, "calpwr", "calpwr", CALPWROFST, 'n','n');
#endif
	/*5*/pars[p(LOCALIP)] = new ParStr32("ip", "localip","ip", 0);
	/*5*/pars[p(SWROLL1)] = new ParInt(ROLLMODE1, "swroll1", "swroll1", SWROLL1OFST, 'p', 'i');
	/*6*/pars[p(SWROLL2)] = new ParInt(ROLLMODE2, "swroll2", "swroll2", SWROLL2OFST, 'p', 'i');
	/*7*/pars[p(UTCSDT)] = new ParByte(1, "utcsdt", "utcsdt", NTPSDTOFST, 'p', 'i', new UTCSDT_Evnt());
	/*8*/pars[p(UTCZONE)] = new ParByte(1, "utczone", "utczone", NTPZONEOFST, 'p','i', new UTCZONE_Evnt());
	/*9*/pars[p(THALT1)] = new ParInt(thalt1,"thalt1", "thalt1", THALT1OFST, 'p','i');
	/*10*/pars[p(THALT2)] = new ParInt(thalt2, "thalt2", "thalt2", THALT2OFST, 'p','i');
	/*11*/pars[p(THALT3)] = new ParInt(thalt3,"thalt3", "thalt3", THALT3OFST, 'p','i');
	/*12*/pars[p(THALT4)] = new ParInt(thalt4, "thalt4", "thalt4", THALT4OFST, 'p','i');
	/*13*/pars[p(STDEL1)] = new ParFloat(400, "stdel1", "stdel1", STDEL1OFST, 'p','i');
	/*14*/pars[p(STDEL2)] = new ParFloat(400, "stdel2", "stdel2", STDEL2OFST, 'p','i');
	/*15*/pars[p(VALWEIGHT)] = new ParFloat(0.5, "valweight", "valweight", VALWEIGHTOFST, 'p','i');
	/*16*/pars[p(TLENGTH)] = new ParFloat(53, "tlength","tlength", TLENGTHOFST, 'p','i');
	/*17*/pars[p(BARRELRAD)] = new ParFloat(3.37, "barrelrad", "barrelrad", BARRELRADOFST, 'p','i');
	/*18*/pars[p(THICKNESS)] = new ParFloat(1.5, "thickness", "thickness", THICKNESSOFST, 'p','i');
	/*19*/pars[p(UTCADJ)] = new ParInt(0, "utcadj", "utcadj", NTPADJUSTOFST, 'p','i');
	/*20*/pars[p(SLATSRATIO)] = new ParFloat(0.8, "slatsratio", "slatsratio", SLATSRATIOFST, 'p','i');
	/*21*/pars[p(UTCSYNC)] = new ParInt(50, "utcsync", "utcsync", NTPSYNCINTOFST, 'p','i', new UTCSYNC_Evnt());
	/*22*/pars[p(MQTTID)] = new ParStr32(MQTTCLIENTID, "mqttid", "mqttid", MQTTIDOFST, 'p','i', new UTCADJ_Evnt());
	/*23*/pars[p(MQTTOUTTOPIC)] = new ParStr32(OUTTOPIC, "mqttouttopic", "mqttouttopic", OUTTOPICOFST, 'p','i');
	/*24*/pars[p(MQTTINTOPIC)] = new ParStr32(INTOPIC, "mqttintopic", "mqttintopic", INTOPICOFST, 'p','i');
	/*25*/pars[p(CLNTSSID1)] = new ParStr32(SSID1, "clntssid1", "clntssid1", WIFICLIENTSSIDOFST1, 'p','i');
	/*26*/pars[p(CLNTPSW1)] = new ParStr32(PSW1, "clntpsw1", "clntpsw1", WIFICLIENTPSWOFST1, 'p','i');
	/*27*/pars[p(CLNTSSID2)] = new ParStr32(SSID2, "clntssid2", "clntpsw1", WIFICLIENTSSIDOFST2, 'p','i');
	/*28*/pars[p(CLNTPSW2)] = new ParStr32(PSW2, "clntpsw2", "clntpsw2", WIFICLIENTPSWOFST2, 'p','i');
	/*29*/pars[p(APPSSID)] = new ParStr32(SSIDAP, "appssid", "appssid", WIFIAPSSIDOFST, 'p','i');
	/*30*/pars[p(APPPSW)] = new ParStr32(PSWAP, "apppsw", "apppsw", WIFIAPPPSWOFST, 'p','i', new WEBUSR_Evnt());
	/*31*/pars[p(WEBUSR)] = new ParStr32(WBUSR, "webusr", "webusr", WEBUSROFST, 'p','i', new WEBPSW_Evnt());
	/*32*/pars[p(WEBPSW)] = new ParStr32(WBPSW, "webpsw", "webpsw", WEBPSWOFST, 'p','i');
	/*33*/pars[p(MQTTUSR)] = new ParStr32(MQUSR, "mqttusr", "mqttusr", MQTTUSROFST, 'p','i');
	/*34*/pars[p(MQTTPSW)] = new ParStr32(MQPSW, "mqttpsw", "mqttpsw", MQTTPSWOFST, 'p','i');
	/*35*/pars[p(MQTTADDR)] = new ParStr64(MQTTSRV, "mqttaddr", "mqttaddr", MQTTADDROFST, 'p','i');
	/*36*/pars[p(MQTTPORT)] = new ParLong(MQTTPRT, "mqttport", "mqttport", MQTTPORTOFST, 'p','i');
	/*37*/pars[p(WSPORT)] = new ParStr32(WSPRT, "wsport", "wsport", WSPORTOFST, 'p','i');
	/*38*/pars[p(MQTTPROTO)] = new ParStr32(MQTTPT, "mqttproto", "mqttproto", MQTTPROTOFST, 'p','i');
	/*39*/pars[p(NTPADDR1)] = new ParStr32(NTP1, "ntpaddr1", "ntpaddr1", NTP1ADDROFST, 'p','i');
	/*40*/pars[p(NTPADDR2)] = new ParStr32(NTP2, "ntpaddr2", "ntpaddr2", NTP2ADDROFST, 'p','i');
	/*41*/pars[p(PWRMULT)] = new ParFloat(1, "pwrmult", "pwrmult", PWRMULTOFST, 'p','i');
	/*42*/pars[p(ACVOLT)] = new ParByte(230, "acvolt", "acvolt", ACVOLTOFST, 'p','i');
	/*43*/pars[p(ONCOND1)] = new ParVarStr("-1", "oncond1", "oncond1", 0, 'n','n', new ONCOND1_Evnt());
	/*44*/pars[p(ONCOND2)] = new ParVarStr("-1", "oncond2","oncond2", 0, 'n','n', new ONCOND2_Evnt());
	/*45*/pars[p(ONCOND3)] = new ParVarStr("-1", "oncond3","oncond3", 0, 'n','n', new ONCOND3_Evnt());
	/*46*/pars[p(ONCOND4)] = new ParVarStr("-1", "oncond4","oncond4", 0, 'n','n', new ONCOND4_Evnt());
	/*47*/pars[p(ONCOND5)] = new ParVarStr("(td1=4000)|(ma1=0)|(ma4=2)|(tsmpl4=4)|(oe1=1)", "oncond5","oncond5", 0, 'n','n', new ONCOND5_Evnt());
	/*48*/pars[p(ACTIONEVAL)] = new ParStr32("","onaction","onaction", 0, 'n','n', new ACTIONEVAL_Evnt());
	/*5*/pars[p(WIFICHANGED)] = new ParByte(0, "WIFICHANGED","",  0);
	/*5*/pars[p(CONFLOADED)] = new ParByte(0, "CONFLOADED","", 0);
	/*5*/pars[p(MQTTADDRMODFIED)] = new ParByte(0, "MQTTADDRMODFIED","", 0);
	/*5*/pars[p(TOPICCHANGED)] = new ParByte(0, "TOPICCHANGED","", 0);
	/*5*/pars[p(MQTTCONNCHANGED)] = new ParByte(0, "MQTTCONNCHANGED","", 0);
	/*5*/pars[p(TIMINGCHANGED)] = new ParByte(0, "TIMINGCHANGED","", 0);
	/*5*/pars[p(SWACTION1)] = new ParByte(0, "SWACTION1","", 0);
	/*5*/pars[p(SWACTION2)] = new ParByte(0, "SWACTION2","", 0);
	/*5*/pars[p(SWACTION3)] = new ParByte(0, "SWACTION3","", 0);
	/*5*/pars[p(SWACTION4)] = new ParByte(0, "SWACTION4","", 0);
	
	/*5*/pars[p(UTCVAL)] = new ParLong(0, "UTCVAL","utcval", 0, 0);

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
	DEBUG_PRINT("setSWMode: ");
	DEBUG_PRINTLN(mode);
	DEBUG_PRINTLN(n);
	//readModeAndPub(n);
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
#if (MCP2317) 
	mcp.digitalWrite(OUT1EU,out[0]);	
	mcp.digitalWrite(OUT1DD,out[1]);		
	mcp.digitalWrite(OUT2EU,out[2]);	
	mcp.digitalWrite(OUT2DD,out[3]);
	rstldcnt(0);
#else										
	digitalWrite(OUT1EU,out[0]);	
	digitalWrite(OUT1DD,out[1]);		
	digitalWrite(OUT2EU,out[2]);	
	digitalWrite(OUT2DD,out[3]);		
#endif
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
	  WiFi.softAP((const char*) static_cast<ParStr32*>(pars[p(APPSSID)])->val);
	  //WiFi.softAP("cacca9");
	  //interrupts();
	  //DEBUG_PRINT(F("Setting soft-AP ... "));
	  //DEBUG_PRINTLN(WiFi.softAP((confcmd[APPSSID]).c_str()) ? F("Ready") : F("Failed!"));
	  //delay(500); // Without delay I've seen the IP address blank
	  delay(1000);
	  pars[p(LOCALIP)]->load((char*) (WiFi.softAPIP().toString()).c_str());
	  Serial.print(F("Soft-AP IP address = "));
	  Serial.println(((ParStr32*)pars[p(LOCALIP)])->val);
	  //wifi_softap_dhcps_stop();
  }else{
	  //noInterrupts ();
	  WiFi.softAP((const char*) static_cast<ParStr32*>(pars[p(APPSSID)])->val); 
	  //WiFi.softAP(ssid, password);
	  //interrupts();
	  //DEBUG_PRINT(F("Setting soft-AP ... "));
	  //DEBUG_PRINTLN(WiFi.softAP((confcmd[APPSSID]).c_str()) ? F("Ready") : F("Failed!"));
	  delay(1000);
	  pars[p(LOCALIP)]->load(WiFi.softAPIP().toString().c_str());
	  Serial.print(F("Soft-AP IP address = "));
	  Serial.println(pars[p(LOCALIP)]->getStrVal());
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
	Serial.println(pars[p(CLNTSSID1+wifindx)]->getStrVal());
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
		if (pars[p(CLNTSSID1+wifindx)]->getStrVal() != "") {
			//noInterrupts();
			Serial.print(F("Begin status: "));
			Serial.println(WiFi.begin((const char*) static_cast<ParStr32*>(pars[p(CLNTSSID1+wifindx)])->val, (const char*) static_cast<ParStr32*>(pars[p(CLNTPSW1+wifindx)])->val));
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
		IPAddress ip = WiFi.localIP();
		ip.toString().toCharArray(IP, 16);
		pars[p(LOCALIP)]->load(IP);
		Serial.print(F("AP client IP address = "));
		Serial.println(((ParStr32*)pars[p(LOCALIP)])->val);
	}	

	Serial.println (F("\n******************* begin ***********"));
    WiFi.printDiag(Serial);
	Serial.println (F("\n******************* end ***********"));

	//WiFi.enableAP(true);
  //}
}

#if (LARGEFW)
void setup_mDNS() {
	if (MDNS.begin((const char*) static_cast<ParStr32*>(pars[p(LOCALIP)])->val)) {              // Start the mDNS responder for esp8266.local
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
	/////noInterrupts ();
	mqttClient = new MQTT((const char *)(static_cast<ParStr32*>(pars[p(MQTTID)]))->val, (const char *)(static_cast<ParStr32*>(pars[p(MQTTADDR)]))->val, (unsigned int) (static_cast<ParLong*>(pars[p(MQTTPORT)]))->val);
	/////interrupts ();
    DEBUG_PRINTLN(F("Registro i callback dell'MQTT."));
	DEBUG_PRINT(F("Attempting MQTT connection to: "));
	DEBUG_PRINT(pars[p(MQTTADDR)]->getStrVal());
	DEBUG_PRINT(F(", with ClientID: "));
	DEBUG_PRINT(pars[p(MQTTID)]->getStrVal());
	DEBUG_PRINTLN(F(" ..."));

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
			mqttClient->subscribe(static_cast<ParStr32*>(pars[p(MQTTINTOPIC)])->val); 
			mqttClient->publish((const char *)(static_cast<ParStr32*>(pars[p(MQTTOUTTOPIC)]))->val, (const char *)(static_cast<ParStr32*>(pars[p(MQTTID)]))->val, 32);
			//mqttConnected=true;
		});
		mqttClient->onDisconnected([]() {
			//DEBUG_PRINTLN("MQTT disconnected.");
		DEBUG_PRINTLN(F("MQTT: onDisconnected([]() dice mi sono disconnesso."));
			mqttConnected=false;
		});
		DEBUG_PRINTLN(F("MQTT: Eseguo la prima connect."));
		mqttClient->setUserPwd((const char*)static_cast<ParStr32*>(pars[p(MQTTUSR)])->val, (const char*) static_cast<ParStr32*>(pars[p(MQTTPSW)])->val);
		//////noInterrupts ();
		mqttClient->connect();
		//////interrupts ();
		delay(50);
		mqttClient->subscribe(static_cast<ParStr32*>(pars[p(MQTTINTOPIC)])->val);
		mqttClient->publish((const char *)(static_cast<ParStr32*>(pars[p(MQTTOUTTOPIC)]))->val, (const char *)static_cast<ParStr32*>(pars[p(MQTTID)])->val, 32);
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
   
	if(parseJsonFieldArrayToInt(response, pars, ncifre, USRMODIFICABLEFLAGS,0)){
		parseJsonFieldArrayToStr(response, pars, ncifre+500, EXTCONFDIM,0,'#',"|");
	}
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
  
  if(roll[0] == true){
	  s + pars[MQTTUP1]->getStrJsonName()+twodot+(outLogic[ENABLES] && (outLogic[DIRS]==LOW))+comma; 	//up1 DIRS=HIGH
	  s += pars[MQTTDOWN1]->getStrJsonName()+twodot+(outLogic[ENABLES] && (outLogic[DIRS]==HIGH))+comma;    //down1  DIRS=LOW
	  if(blocked[0]>0){
		  s+= (String) "blk1"+twodot+blocked[0]+comma;		//blk1
	  }
  }else{
	  s += pars[MQTTUP1]->getStrJsonName()+twodot+(out[0]==HIGH)+comma; 	//up1 DIRS=HIGH
	  s += pars[MQTTDOWN1]->getStrJsonName()+twodot+(out[1]==HIGH)+comma;    //down1  DIRS=LOW
  }
  if(roll[1] == true){
	  s += pars[MQTTUP2]->getStrJsonName()+twodot+(outLogic[ENABLES+STATUSDIM] && (outLogic[DIRS+STATUSDIM]==LOW))+comma; 	//up1 DIRS=HIGH
	  s += pars[MQTTDOWN2]->getStrJsonName()+twodot+(outLogic[ENABLES+STATUSDIM] && (outLogic[DIRS+STATUSDIM]==HIGH))+comma;    //down1  DIRS=LOW
	  s+= (String) "pr2"+twodot+String(percfdbck(1))+comma;		//pr2
	  s+= (String) "tr2"+twodot+String(getCronoCount(1))+comma;			//tr2
	  if(blocked[1]>0){
		  s+= (String) "blk2"+twodot+blocked[1]+comma;		//blk2
	  }
  }else{
	  s += pars[MQTTUP2]->getStrJsonName()+twodot+(out[2]==HIGH)+comma; 	//up1 DIRS=HIGH
	  s += pars[MQTTDOWN2]->getStrJsonName()+twodot+(out[3]==HIGH)+comma;    //down1  DIRS=LOW
  }
  s+= (String) "pr1"+twodot+String(percfdbck(0))+comma;		//pr1
  s+= (String) "tr1"+twodot+String(getCronoCount(0))+comma;			//tr1
  s+= (String) "pr2"+twodot+String(percfdbck(1))+comma;		//pr2
  s+= (String) "tr2"+twodot+String(getCronoCount(1))+comma;			//tr2
  s+= (String) "sp1"+twodot+String((long)getTapThalt(0))+comma;		//sp1
  s+= (String) "sp2"+twodot+String((long)getTapThalt(1));		//sp2
  if(all){
	    s += comma;
		s += pars[MQTTTEMP]->getStrJsonName()+twodot+String(asyncBuf[GTTEMP])+comma;
		s += pars[MQTTMEANPWR]->getStrJsonName()+opensqr+String(asyncBuf[GTMEANPWR1])+comma+String(asyncBuf[GTMEANPWR2])+closesqr2;
		s += pars[MQTTPEAKPWR]->getStrJsonName()+opensqr+String(asyncBuf[GTPEAKPWR1])+comma+String(asyncBuf[GTPEAKPWR2])+"\"],";
  }else{
		s+=enda;
  }
  publishStr(s);
}

inline byte percfdbck(byte n){
	//DEBUG_PRINT(F("Posdelta:"));
	//DEBUG_PRINTLN(getPosdelta());
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
  s+="actmode"+String(n+1)+twodot+pars[p(SWACTION1+n)]->getStrVal()+enda; 
  //sprintf(sd,"%s%s%s%f%s",openbrk,mqttJson[MQTTJSONTEMP].c_str(),twodot,asyncBuf[GTTEMP],closebrk);
  //s=String(sd);
  publishStr(s);
}

void readModeAndPub(byte n){
  //DEBUG_PRINTLN(F("\nreadTempAndPub")); 
  String s=openbrk;
  //char sd[300];
  s+="mode"+String(n+1)+twodot+pars[p(SWROLL1+n)]->getStrVal()+enda;
  //sprintf(sd,"%s%s%s%f%s",openbrk,mqttJson[MQTTJSONTEMP].c_str(),twodot,asyncBuf[GTTEMP],closebrk);
  //s=String(sd);
  publishStr(s);
}

void readPwrCalAndPub(){
  //DEBUG_PRINTLN(F("\nreadPwrCalAndPub")); 
  String s=openbrk;
  s+=pars[DOPWRCAL]->getStrJsonName()+twodot+pars[p(PWRMULT)]->getStrVal()+enda;
  publishStr(s);
}

void readAvgPowerAndPub(){
  //DEBUG_PRINTLN(F("\nreadPowerAndPub")); 
  String s=openbrk;
  s+=pars[MQTTMEANPWR]->getStrJsonName()+opensqr+String(asyncBuf[GTMEANPWR1])+comma+String(asyncBuf[GTMEANPWR2])+closesqr3;
  publishStr(s);
}

void readPeakPowerAndPub(){
  //DEBUG_PRINTLN(F("\nreadPowerAndPub")); 
  String s=openbrk;
  s+=pars[MQTTPEAKPWR]->getStrJsonName()+opensqr+String(asyncBuf[GTPEAKPWR1])+comma+String(asyncBuf[GTPEAKPWR2])+closesqr3;
  publishStr(s);
}

void readParamAndPub(byte parid, char* str){
  //DEBUG_PRINTLN(F("\nreadTempAndPub")); 
  String s=openbrk;
  //char sd[300];
  s+=pars[parid]->getStrJsonName()+twodot+String(str)+enda;
//sprintf(sd,"%s%s%s%f%s",openbrk,mqttJson[MQTTJSONTEMP].c_str(),twodot,asyncBuf[GTTEMP],closebrk);
  //s=String(sd);
  //DEBUG_PRINTLN(s);
  publishStr(s);
}

void readTempAndPub(){
  //DEBUG_PRINTLN(F("\nreadTempAndPub")); 
  String s=openbrk;
  //char sd[300];
  s+=pars[MQTTTEMP]->getStrJsonName()+twodot+String(asyncBuf[GTTEMP])+enda;
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

void readIpwrAndPub(){
  String s=openbrk2;
  publishStr(s);
}

void readIacvoltAndPub(){
  String s=openbrk2;
  publishStr(s);
}

void inline readActionConfAndSet(){
	//imposta le configurazioni dinamiche in base ad eventi esterni
	DEBUG_PRINTLN(F("readActionConfAndSet."));
	eval(((String) pars[p(ACTIONEVAL)]->getStrVal()).c_str());
}

void publishStr(String &str){
  //pubblica sul broker la stringa JSON
  //informazioni mittente
  str += "\"";
  str += pars[MQTTTIME]->getStrJsonName()+twodot+String(millis())+comma; 
  str += pars[MQTTMAC]->getStrJsonName()+twodot+String(WiFi.macAddress())+comma;
  str += pars[MQTTIP]->getStrJsonName()+twodot+pars[p(LOCALIP)]->getStrVal()+comma;
  str += pars[MQTTMQTTID]->getStrJsonName()+twodot+pars[p(MQTTID)]->getStrVal()+comma;
 #if (AUTOCAL_HLW8012) 
  str += pars[INSTPWR]->getStrJsonName()+twodot+hlw8012.getActivePower()+comma;
  str += pars[INSTACV]->getStrJsonName()+twodot+hlw8012.getVoltage()+comma;
 #endif
  str += pars[MQTTDATE]->getStrJsonName()+twodot+printUNIXTimeMin(gbuf)+closebrk;
  
  if(mqttClient==NULL){
	  DEBUG_PRINTLN(F("ERROR on publishStr MQTT client is not allocated."));
  }
  else
  {
	  mqttClient->publish((const char *)static_cast<ParStr32*>(pars[p(MQTTOUTTOPIC)])->val, str, 32);
	  DEBUG_PRINTLN(F("Published data: "));
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
  edelay[0] = static_cast<ParLong*>(pars[p(STDEL1)])->val; 
  edelay[1] = static_cast<ParLong*>(pars[p(STDEL2)])->val;
  roll[0] = static_cast<ParByte*>(pars[p(SWROLL1)])->val;
  roll[1] = static_cast<ParByte*>(pars[p(SWROLL2)])->val;
  initTapparellaLogic(in,out,outLogic,pars,first);
#if (AUTOCAL)  
  resetAVGStats(0,0);
  resetAVGStats(0,1);
#endif  
}

inline void setupNTP() {
  setNtpServer(0,(const char*) static_cast<ParStr32*>(pars[p(NTPADDR1)])->val);
  setNtpServer(1,(const char*) static_cast<ParStr32*>(pars[p(NTPADDR2)])->val); 
  setSyncInterval((unsigned long) static_cast<ParLong*>(pars[p(UTCSYNC)])->val);
  setSDT((byte) static_cast<ParByte*>(pars[p(UTCSDT)])->val);
  adjustTime((unsigned long) static_cast<ParLong*>(pars[p(UTCADJ)])->val); 
  setTimeZone((int) static_cast<ParInt*>(pars[p(UTCZONE)])->val);
  sntpInit();  
}

void setup(){
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
  DEBUG_PRINTLN(F("initCommon."));
  initCommon(&server,pars);
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
  telnet.begin((const char *) static_cast<ParStr32*>(pars[p(LOCALIP)])->val); // Initiaze the telnet server
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
  setupTimer(static_cast<ParLong*>(pars[p(THALT1)])->val,TMRHALT);				//function timer switch1
  setupTimer(static_cast<ParLong*>(pars[p(THALT2)])->val,TMRHALT+TIMERDIM);	//function timer switch2 
  setupTimer(RSTTIME*1000,RESETTIMER);						//special timer btn1 
  setupTimer(APOFFT*1000,APOFFTIMER);						//special timer btn1
  setSWMode(static_cast<ParByte*>(pars[p(SWROLL1)])->val,0);
  setSWMode(static_cast<ParByte*>(pars[p(SWROLL2)])->val,1);
#if (AUTOCAL)
  weight[0] =  static_cast<ParFloat*>(pars[p(VALWEIGHT)])->val;
  weight[1] = 1 - weight[0];
  pars[p(STDEL1)]->load(TENDCHECK*1000); 
  pars[p(STDEL2)]->load(TENDCHECK*1000);
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
#if (MCP2317) 
	MCP2317_init();
	mcp.pinMode(BTN1U, INPUT);
	mcp.pinMode(BTN1D, INPUT);
	mcp.pinMode(BTN2U, INPUT);
	mcp.pinMode(BTN2D, INPUT);
 #if (INPULLUP)
	mcp.pullUp(BTN1U, HIGH);
	mcp.pullUp(BTN1D, HIGH);
	mcp.pullUp(BTN2U, HIGH);
	mcp.pullUp(BTN2D, HIGH);
#else
	mcp.pullUp(BTN1U, LOW);
	mcp.pullUp(BTN1D, LOW);
	mcp.pullUp(BTN2U, LOW);
	mcp.pullUp(BTN2D, LOW);
 #endif
	pinMode(OUTSLED,OUTPUT);
	digitalWrite(OUTSLED, LOW);
	for(int i=0;i<NBTN*BTNDIM;i++){
		mcp.pinMode(outPorts[i],OUTPUT);
		mcp.digitalWrite(outPorts[i], LOW);
	}
	mcp.pinMode(GREEN,OUTPUT);
	mcp.pinMode(RED,OUTPUT);
	mcp.pinMode(BLUE,OUTPUT);
	mcp.digitalWrite(GREEN, LOW);
	mcp.digitalWrite(RED, LOW);
	mcp.digitalWrite(GREEN, LOW);
#else
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
	pinMode(OUTSLED,OUTPUT);
	digitalWrite(OUTSLED, LOW);
	for(int i=0;i<NBTN*BTNDIM;i++){
		pinMode(outPorts[i],OUTPUT);
		digitalWrite(outPorts[i], LOW);
	}
#endif
	
  //imposta la DIRSezione delle porte dei led, imposta inizialmente i led come spento  
  
  //------------------------------------------OTA SETUP---------------------------------------------------------------------------------------
  //------------------------------------------END OTA SETUP---------------------------------------------------------------------------------------
  delay(500);
  for(int i=0;i<NBTN*STATUSDIM;i++)
	  outLogic[i]=LOW;
  for(int i=0;i<MQTTDIM;i++)
	  static_cast<ParByte*>(pars[i])->load(LOW);
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
  server.on("/mqttcmd", HTTP_GET, handleMqttCmd);
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
  DEBUG_PRINT(pars[p(MQTTID)]->getStrVal()); 
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

#if (MCP2317) 
	if(!(step % LED_STEP)){		
		 printOut();
	}//END LED_STEP scheduler--------------------------------------------------
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
		if(in[0]>0){
			DEBUG_PRINTLN(F("\ntasto 0 premuto..."));
		}
		//leggi ingressi locali e mette il loro valore sull'array val[]
		leggiTastiLocali();
		//leggiTastiLocaliRemoto();
		//se uno dei tasti delle tapparelle è stato premuto
		//o se è arrivato un comando dalla mqttCallback
		if(cmdLogic(TAP1) == 1 ||  cmdLogic(TAP2) == 1){ 
			//once any button is pressed
			//legge lo stato finale e lo scrive sulle uscite
			scriviOutDaStato();
			//legge lo stato finale e lo pubblica su MQTT
			readStatesAndPub();
			//DEBUG_PRINTLN("Fine callback MQTT.");
			blocked[0]=blocked[1]=false;
		}
		//provenienti dalla mqttCallback
		//remote pressed button event
		//leggiTastiRemoti();
		//------------------------------------------------------------------------------------------------------------
		//Finestra idle di riconnessione (necessaria se il loop è molto denso di eventi e il wifi non si aggancia!!!)
		//------------------------------------------------------------------------------------------------------------
		//sostituisce la bloccante WiFi.waitForConnectResult();	
		if((wifiConn == false && !(isrun[0] || isrun[1]))){
//#if (AUTOCAL_ACS712) 
			DEBUG_PRINTLN(F("\nGiving time to ESP stack... "));
			delay(30);//give 30ms to the ESP stack for wifi connect
//#endif  
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
/*inline void leggiTastiLocaliRemoto(){
	for(int i=0;i<4;i++){
		(in[i]) && (in[i] = 255);
		(static_cast<ParByte*>(pars[i])->val) && (in[i] = static_cast<ParByte*>(pars[i])->val);
		static_cast<ParByte*>(pars[i])->load(LOW);
	}
}*/
//legge PERIODICAMENTE il parser delle condizioni sui sensori
inline void leggiTastiLocaliDaExp(){
	if(roll[0] == false){
		//modalità switch generico
		if(testUpCntEvnt(0,true,SMPLCNT1))
			setActionLogic(eval(((String) pars[p(0)]->getStrVal()).c_str()),0);
		if(testUpCntEvnt(0,true,SMPLCNT2))
			setActionLogic(eval(((String) pars[p(1)]->getStrVal()).c_str()),1);
		//legge lo stato finale e lo scrive sulle uscite
		scriviOutDaStato();
		//legge lo stato finale e lo pubblica su MQTT
		//readStatesAndPub();
	}else{
		//modalità tapparella
		//simula pressione di un tasto locale
		in[0] = eval(((String) pars[p(0)]->getStrVal()).c_str());
		in[1] = eval(((String) pars[p(1)]->getStrVal()).c_str());
	}
	if(roll[1] == false){
		//modalità switch generico
		if(testUpCntEvnt(0,true,SMPLCNT3))
			setActionLogic(eval(((String) pars[p(2)]->getStrVal()).c_str()),2);
		if(testUpCntEvnt(1,true,SMPLCNT4)){
			setActionLogic(eval(((String) pars[p(3)]->getStrVal()).c_str()),3);
			DEBUG_PRINTLN(getCntValue(SMPLCNT4));
		}
		//legge lo stato finale e lo scrive sulle uscite
		scriviOutDaStato();
		//legge lo stato finale e lo pubblica su MQTT
		//readStatesAndPub();
	}else{
		//modalità tapparella
		//simula pressione di un tasto locale
		in[2] = eval(((String) pars[p(2)]->getStrVal()).c_str());
		in[3] = eval(((String) pars[p(3)]->getStrVal()).c_str());
	}
	//imposta le configurazioni dinamiche in base ad eventi locali valutati periodicamente
	//eval((confcmd[JSONONCOND5]).c_str());
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
			dd = hlw8012.getExtimActivePower();
			//dd = hlw8012.getAvgdExtimActivePower();
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
			#if (MCP2317) 
				mcp.digitalWrite(BLUE, !mcp.digitalRead(BLUE));
			#else
				digitalWrite(OUTSLED, !digitalRead(OUTSLED));
			#endif
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
			#if (MCP2317) 
				mcp.digitalWrite(BLUE, LOW);
			#else
				digitalWrite(OUTSLED, LOW);
			#endif
			IPAddress ip = WiFi.localIP();
			ip.toString().toCharArray(IP, 16);
			pars[p(LOCALIP)]->load(IP);
			Serial.print(F("AP client IP address = "));
			Serial.println(((ParStr32*)pars[p(LOCALIP)])->val);
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
		if((byte) static_cast<ParByte*>(pars[p(WIFICHANGED)])->val == 1){
			pars[p(WIFICHANGED)]->load(0);
			wifindx=0;
			Serial.println(F("Doing WiFi disconnection"));
			WiFi.persistent(false);
			WiFi.disconnect(false);
			WiFi.mode(WIFI_OFF);    
			//WiFi.mode(WIFI_STA);
			wifindx = 0;
		}
	
		if((byte) static_cast<ParByte*>(pars[p(MQTTADDRMODFIED)])->val == 1){ 
			pars[p(MQTTADDRMODFIED)]->load(0);
			DEBUG_PRINTLN(F("confcmd[MQTTADDRMODFIED] eseguo la reconnect()"));
			mqttReconnect();
		}else if((byte) static_cast<ParByte*>(pars[p(MQTTCONNCHANGED)])->val == 1){ 
			pars[p(MQTTCONNCHANGED)]->load(0);
			if(mqttClient==NULL){
				DEBUG_PRINTLN(F("ERROR confcmd[TOPICCHANGED]! MQTT client is not allocated."));
				mqttReconnect();
			}
			else
			{
			#if defined (_DEBUG1) || defined (_DEBUGR)	
				DEBUG_PRINTLN(F("MQTTCONNCHANGED! Eseguo la setUserPwd() con usr "));
				DEBUG_PRINTLN(pars[p(MQTTUSR)]->getStrVal());
				DEBUG_PRINTLN(F(" e psw "));
				DEBUG_PRINTLN(pars[p(MQTTPSW)]->getStrVal());
			#endif		
				mqttClient->setUserPwd((const char*) static_cast<ParStr32*>(pars[p(MQTTUSR)])->val, (const char*) static_cast<ParStr32*>(pars[p(MQTTPSW)])->val);
				DEBUG_PRINTLN(F("MQTTCONNCHANGED! Eseguo la connect() ..."));
				mqttClient->connect();
			#if defined (_DEBUG1) || defined (_DEBUGR)	
				DEBUG_PRINTLN(F("MQTTCONNCHANGED! Eseguo la subscribe() con "));
				DEBUG_PRINTLN(pars[p(MQTTINTOPIC)]->getStrVal());
				DEBUG_PRINTLN(F("..."));
			#endif	
				mqttClient->subscribe(pars[p(MQTTINTOPIC)]->getStrVal());
			#if defined (_DEBUG1) || defined (_DEBUGR)	
				DEBUG_PRINTLN(F("MQTTCONNCHANGED! publish(): "));
				DEBUG_PRINTLN(pars[p(MQTTOUTTOPIC)]->getStrVal());
				DEBUG_PRINTLN(F(" Intopic: "));
				DEBUG_PRINTLN(pars[p(MQTTINTOPIC)]->getStrVal());
			#endif		
				mqttClient->publish((const char*) static_cast<ParStr32*>(pars[p(MQTTOUTTOPIC)])->val, (const char*) static_cast<ParStr32*>(pars[p(MQTTID)])->val, 32);
			}
		}else if((byte) static_cast<ParByte*>(pars[p(TOPICCHANGED)])->val == 1){
			pars[p(TOPICCHANGED)]->load(0);
			if(mqttClient==NULL){
				DEBUG_PRINTLN(F("ERROR confcmd[TOPICCHANGED]! MQTT client is not allocated."));
				mqttReconnect();
			}
			else
			{
			#if defined (_DEBUG1) || defined (_DEBUGR)	
				DEBUG_PRINTLN(F("TOPICCHANGED! Outtopic: "));
				DEBUG_PRINTLN(pars[p(MQTTOUTTOPIC)]->getStrVal());
				DEBUG_PRINTLN(F(" Intopic: "));
				DEBUG_PRINTLN(pars[p(MQTTINTOPIC)]->getStrVal());
				DEBUG_PRINTLN(F("TOPICCHANGED! Eseguo la subscribe() con "));
				DEBUG_PRINTLN(pars[p(MQTTINTOPIC)]->getStrVal());
				DEBUG_PRINTLN(F("..."));
			#endif		
				mqttClient->subscribe(pars[p(MQTTINTOPIC)]->getStrVal());
			#if defined (_DEBUG1) || defined (_DEBUGR)	
				DEBUG_PRINTLN(F("TOPICCHANGED! Eseguo la publish() con "));
				DEBUG_PRINTLN(pars[p(MQTTOUTTOPIC)]->getStrVal());
				DEBUG_PRINTLN(F(" ..."));
			#endif		
				mqttClient->publish((const char*) static_cast<ParStr32*>(pars[p(MQTTOUTTOPIC)])->val,(const char*) static_cast<ParStr32*>(pars[p(MQTTID)])->val, 32);
			}
		}
}

//-----------------------------------------------INIZIO TIMER----------------------------------------------------------------------
//azione da compiere allo scadere di uno dei timer dell'array	
void onElapse(byte nn, unsigned long tm){
	int n = nn / TIMERDIM;
	int sw = nn % TIMERDIM;
	
	DEBUG_PRINT(F("\nElapse timer n: "));
	DEBUG_PRINT(nn);
	DEBUG_PRINT(F("  al tempo: "));
	DEBUG_PRINT(tm);
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
					scriviOutDaStato();//15/08/19
					//pubblica lo stato di UP o DOWN attivo su MQTT (non lo fa il loop stavolta!)
					readStatesAndPub();
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
				if(getCntValue(nn)==3){
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
					//confcmd[MQTTIP] = WiFi.softAPIP().toString();
					//MDNS.notifyAPChange()();
					//wifi_softap_dhcps_start();
					//delay(100);
					//WiFi.printDiag(Serial);
					//MDNS.update();
					setGroupState(0,n%2);
				}else if(getCntValue(nn)==7){
					DEBUG_PRINTLN(F("-----------------------------"));
					DEBUG_PRINTLN(F("Rebooting ESP without reset of configuration"));
					DEBUG_PRINTLN(F("-----------------------------"));
					ESP.eraseConfig(); //do the erasing of wifi credentials
					ESP.restart();
				}else if(getCntValue(nn)==5 && roll[n]){ //solo in modalità tapparella!
					DEBUG_PRINTLN(F("-----------------------------"));
					DEBUG_PRINTLN(F("ATTIVATA CALIBRAZIONE MANUALE BTN 1"));
					DEBUG_PRINTLN(F("-----------------------------"));
					manualCalibration(0); //BTN1
				}else if(getCntValue(nn)==9){
					DEBUG_PRINTLN(F("-----------------------------"));
					DEBUG_PRINTLN(F("Reboot ESP with reset of configuration"));
					DEBUG_PRINTLN(F("-----------------------------"));
					rebootSystem();
				}
#if (AUTOCAL_HLW8012) 
				else if(getCntValue(nn)==11){
					DEBUG_PRINTLN(F("-----------------------------"));
					DEBUG_PRINTLN(F("Do power calibration"));
					DEBUG_PRINTLN(F("-----------------------------"));
					calibrate_pwr();
					saveSingleConf(PWRMULT);
				}
#endif				
				else{
					setGroupState(0,nn);
				}
				//DEBUG_PRINT(F("Resettato contatore dei servizi: "));
				resetCnt(nn);
			}else if(roll[n]){ //solo in modalità tapparella!
				DEBUG_PRINTLN(F("onElapse:  timer 2 dei servizi a conteggio scaduto"));
				if(getCntValue(CNTSERV3)==5){
					DEBUG_PRINTLN(F("-----------------------------"));
					DEBUG_PRINTLN(F("ATTIVATA CALIBRAZIONE MANUALE BTN 2"));
					DEBUG_PRINTLN(F("-----------------------------"));
					manualCalibration(1); //BTN2
				}else{
					setGroupState(0,nn);
				}
				//DEBUG_PRINT(F("Resettato contatore dei servizi: "));
				resetCnt(nn);
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
	pars[p(THALT1 + n)]->load(app);
	//initTapparellaLogic(in,inr,outLogic,(confcmd[THALT1]).toInt(),(confcmd[THALT2]).toInt(),(confcmd[STDEL1]).toInt(),(confcmd[STDEL2]).toInt(),BTNDEL1,BTNDEL2);
	setTapThalt(app, n);
	DEBUG_PRINTLN(F("-----------------------------"));
#if (AUTOCAL)
	calAvg[n] = getAVG(n);
	weight[0] = (double) calAvg[0] / (calAvg[0] +  calAvg[1]);
	weight[1] = (double) calAvg[1] / (calAvg[0] +  calAvg[1]);
	pars[p(VALWEIGHT)]->load(weight[0]);
	updateUpThreshold(n);
	//confcmd[TRSHOLD1 + n] = String(getThresholdUp(n));
	//setThresholdUp((confcmd[TRSHOLD1 + n]).toFloat(), n);
	saveSingleConf(VALWEIGHT);
	DEBUG_PRINT(F("Modified current weight "));
	DEBUG_PRINTLN(pars[p(VALWEIGHT)]->getStrVal());
#endif
	saveSingleConf(THALT1 + n);
	DEBUG_PRINT(F("Modified THALT "));
	DEBUG_PRINTLN(THALT1 + n);
	DEBUG_PRINT(F(": "));
	DEBUG_PRINTLN(pars[p(THALT1 + n)]->getStrVal());
}

void manualCalibration(byte btn){
	setGroupState(0,btn);	
	//activate the learning of the running statistics
	//setStatsLearnMode();
#if (AUTOCAL)
	//resetStatDelayCounter(btn);
	disableUpThreshold(btn);
#endif
	pars[BTN2IN + btn*BTNDIM]->load(101);			//codice comando attiva calibrazione
	
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
		DEBUG_PRINTLN(pars[p(MQTTID)]->getStrVal());
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
//-----------------------------------------------------------------------------------------------------------------------------
//richieste da remoto di valori locali
void BaseEvnt::doaction(){}
//--------------------------------------------------------------------------------------------------
//gestione eventi MQTT sui sensori (richieste e configurazioni) all'arrivo memorizzate su confcmd[]
//--------------------------------------------------------------------------------------------------
//configurazioni provenienti da remoto
//---------------------------------------------------------------
void MQTTMAC_Evnt::doaction(){
	readMacAndPub();
}
void MQTTIP_Evnt::doaction(){
	readIpAndPub();
}
void MQTTMQTTID_Evnt::doaction(){
	readMQTTIdAndPub();
}
void MQTTTIME_Evnt::doaction(){
	readTimeAndPub();
}
void MQTTTEMP_Evnt::doaction(){
	readTempAndPub();
}
void MQTTMEANPWR_Evnt::doaction(){
	readAvgPowerAndPub();
}
void MQTTPEAKPWR_Evnt::doaction(){
	readPeakPowerAndPub();
}
#if (AUTOCAL_HLW8012) 
void DOPWRCAL_Evnt::doaction(){
	calibrate_pwr();
	saveSingleConf(PWRMULT);
	readPwrCalAndPub();
}
void INSTPWR_Evnt::doaction(){
	void readIpwrAndPub();
}
void INSTACV_Evnt::doaction(){
	void readIacvoltAndPub();
}
#endif
//----------------------------------------------------------------------------
//richieste da remoto di valori locali
//----------------------------------------------------------------------------
void MQTTBTN_Evnt::doaction(){
	unsigned i = this->pid;
	byte v = static_cast<ParByte*>(pars[i])->val;
	int n = i / TIMERDIM;
	int sw = i % TIMERDIM;
	
	//DEBUG_PRINT("\npid: ");
	//DEBUG_PRINT(i);
	//DEBUG_PRINT(" val: ");
	//DEBUG_PRINTLN(v);
	(in[i]) && (in[i] = 255);
	(v) && (in[i] = v);
	static_cast<ParByte*>(pars[i])->load(0); //flag reset
	//DEBUG_PRINT(" inval: ");
	//DEBUG_PRINTLN(in[i]);
	
	if(roll[n]){
		//DEBUG_PRINTLN("\ncmdLogic: switchLogic");
		if(switchLogic(sw,n)){
			//legge lo stato finale e lo scrive sulle uscite
			scriviOutDaStato();
			//legge lo stato finale e lo pubblica su MQTT
			readStatesAndPub();
			//DEBUG_PRINTLN("Fine callback MQTT.");
			blocked[0]=blocked[1]=false;
		}
	}else{
		//DEBUG_PRINTLN("\ncmdLogic: toggleLogic");
		if(toggleLogic(sw, n)){
			//legge lo stato finale e lo scrive sulle uscite
			scriviOutDaStato();
			//legge lo stato finale e lo pubblica su MQTT
			readStatesAndPub();
			//DEBUG_PRINTLN("Fine callback MQTT.");
			blocked[0]=blocked[1]=false;
		}
	}
}
void UTCVAL_Evnt::doaction(){
	updateNTP(static_cast<ParLong*>(pars[p(UTCVAL)])->val);
}
void UTCSYNC_Evnt::doaction(){
	setSyncInterval(saveLongConf(UTCSYNC));
}
void UTCADJ_Evnt::doaction(){
	adjustTime(saveIntConf(UTCADJ));
}
void UTCSDT_Evnt::doaction(){
	setSDT(saveByteConf(UTCSDT));
}
void UTCZONE_Evnt::doaction(){
	setTimeZone(saveByteConf(UTCZONE));
}
void ACTIONEVAL_Evnt::doaction(){
	//save confs and actions on new action received event
	writeOnOffConditions();
	//run actions one time on new action received event
	readActionConfAndSet();
}
void ONCOND1_Evnt::doaction(){
	//save confs and actions on new action received event
	writeOnOffConditions();
}
void ONCOND2_Evnt::doaction(){
	//save confs and actions on new config received event
	writeOnOffConditions();
}
void ONCOND3_Evnt::doaction(){
	//save confs and actions on new config received event
	writeOnOffConditions();
}
void ONCOND4_Evnt::doaction(){
	//save confs and actions on new config received event
	writeOnOffConditions();
}
void ONCOND5_Evnt::doaction(){
	//save confs and actions on new config received event
	writeOnOffConditions();
}
void WEBUSR_Evnt::doaction(){
	//save confs and actions on new config received event
	saveSingleConf(WEBUSR);
}
void WEBPSW_Evnt::doaction(){
	//save confs and actions on new config received event
	saveSingleConf(WEBPSW);
}
#if (AUTOCAL_HLW8012) 
void CALPWR_Evnt::doaction(){
	//save confs and actions on new config received event
	saveSingleConf(CALPWR);
}
#endif
//-----------------------------------------------------------------------------------------------------------------------------
/*inline void leggiTastiRemoti(){
	//--------------------------------------------------------------------------------------------------
	//gestione eventi MQTT sui sensori (richieste e configurazioni) all'arrivo memorizzate su confcmd[]
	//--------------------------------------------------------------------------------------------------
	//configurazioni provenienti da remoto
	//---------------------------------------------------------------
	//for(int i=0; i < EXTCONFDIM; i++){
	//}
	
	if(confFlags[UTCVAL]){// 1
		confFlags[UTCVAL] = LOW;
		updateNTP(strtoul((confcmd[UTCVAL]).c_str(), NULL, 10));
		//no confcmd to save
	}
	if(confFlags[UTCSYNC]){// 2
		confFlags[UTCSYNC] = LOW;
		setSyncInterval(saveLongConf(UTCSYNC));
	}
	if(confFlags[UTCADJ]){// 3
		confFlags[UTCADJ] = LOW;
		adjustTime(saveIntConf(UTCADJ));
	}
	if(confFlags[UTCSDT]){// 4
		confFlags[UTCSDT] = LOW;
		setSDT(saveByteConf(UTCSDT));
	}
	if(confFlags[UTCZONE]){// 5
		confFlags[UTCZONE] = LOW;
		setTimeZone(saveByteConf(UTCZONE));
	}
	//parametri di configurazione di lunghezza variabile
	if(confFlags[ACTIONEVAL]){// 6
		confFlags[ACTIONEVAL] = LOW;
		//save confs and actions on new action received event
		writeOnOffConditions();
		//run actions one time on new action received event
		readActionConfAndSet();
	}
	if(confFlags[ONCOND1]){// 7
		confFlags[ONCOND1] = LOW;
		//save confs and actions on new action received event
		writeOnOffConditions();
		//are periodic actions!
	}
	if(confFlags[ONCOND2]){// 8
		confFlags[ONCOND2] = LOW;
		//save confs and actions on new config received event
		writeOnOffConditions();
		//confs are automatically runned on every loop by leggiTastiLocaliDaExp()
	}
	if(confFlags[ONCOND3]){// 9
		confFlags[ONCOND3] = LOW;
		//save confs and actions on new config received event
		writeOnOffConditions();
		//config are automatically runned on every loop by leggiTastiLocaliDaExp()
	}
	if(confFlags[ONCOND4]){// 10
		confFlags[ONCOND4] = LOW;
		//save confs and actions on new config received event
		writeOnOffConditions();
		//config are automatically runned on every loop by leggiTastiLocaliDaExp()
	}
	if(confFlags[ONCOND5]){// 11
		confFlags[ONCOND5] = LOW;
		//save confs and actions on new config received event
		writeOnOffConditions();
		//config are automatically runned on every loop by leggiTastiLocaliDaExp()
	}
	if(confFlags[WEBUSR]){// 12
		confFlags[WEBUSR] = LOW;
		//save confs and actions on new config received event
		saveSingleConf(WEBUSR);
		//config are automatically runned on every loop by leggiTastiLocaliDaExp()
	}
	if(confFlags[WEBPSW]){// 13
		confFlags[WEBPSW] = LOW;
		//save confs and actions on new config received event
		saveSingleConf(WEBPSW);
		//config are automatically runned on every loop by leggiTastiLocaliDaExp()
	}
#if (AUTOCAL_HLW8012) 
	if(confFlags[CALPWR]){// 13
		confFlags[CALPWR] = LOW;
		//save confs and actions on new config received event
		saveSingleConf(CALPWR);
		//config are automatically runned on every loop by leggiTastiLocaliDaExp()
		//do power calibration
		//calibrate_pwr();
		//saveSingleConf(PWRMULT);
	}
#endif
*/
	//----------------------------------------------------------------------------
	//richieste da remoto di valori locali
	//----------------------------------------------------------------------------
	/*
	if(inr[MQTTMAC]){// 1
		inr[MQTTMAC] = LOW;
		readMacAndPub();
	}
	if(inr[MQTTIP]){// 2
		inr[MQTTIP] = LOW;
		readIpAndPub();
	}
	if(inr[MQTTMQTTID]){// 3
		inr[MQTTMQTTID] = LOW;
		readMQTTIdAndPub();
	}
	if(inr[MQTTTIME]){// 4
		inr[MQTTTIME] = LOW;
		readTimeAndPub();
	}
	if(inr[MQTTTEMP]){// 5
		inr[MQTTTEMP] = LOW;
		readTempAndPub();
	}
	if(inr[MQTTMEANPWR]){// 6
		inr[MQTTMEANPWR] = LOW;
		readAvgPowerAndPub();
	}
	if(inr[MQTTPEAKPWR]){// 7
		inr[MQTTPEAKPWR] = LOW;
		readPeakPowerAndPub();
	}
#if (AUTOCAL_HLW8012) 
	if(inr[DOPWRCAL]){// 8
		inr[DOPWRCAL] = LOW;
		//do power calibration
		calibrate_pwr();
		saveSingleConf(PWRMULT);
		readPwrCalAndPub();
	}
	if(inr[INSTPWR]){// 9
		inr[INSTPWR] = LOW;
		void readIpwrAndPub();
	}
	if(inr[INSTACV]){// 10
		inr[INSTACV] = LOW;
		readIacvoltAndPub();
	} 
#endif
}
*/



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
