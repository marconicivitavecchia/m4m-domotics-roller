#include "common.h"
//End MQTT config------------------------------------------
//inizio variabili e costanti dello schedulatore (antirimbalzo)
#define nsteps          12000        // numero di fasi massimo di un periodo generico

//global strings
String twodot = "\":\"";
String comma = "\",\"";
String openbrk = "{\"";
//String openbrk2 = "{";
String opensqr = "\":[\"";
String closesqr = "\"]}";
String closesqr2 = "\"],\"";
String closesqr3 = "\"],";
String closesqr4 = "\"]";
String closebrk = "\"}";
String enda = "\",";
String end = "\"";
String endbrk = "}";

byte firstTemp = NOMEDIATEMP;
char IP[] = "xxx.xxx.xxx.xxx";          // buffer
//end global string
//stats variables
#if (AUTOCAL)
float overallSwPower = 0;
float overallSwPower2 = 0;
//float overallSwPower2 = 0;
int samples[10];
uint8_t indx = 0;
uint8_t stat;
bool mov = false;
uint8_t lastPing[4] = {0, 0, 0, 0};
unsigned pingCnt[4] = {0, 0, 0, 0};
bool toPing[4] = {false, false, false, false};
uint8_t nping[4] = {3, 3, 3, 3};
uint8_t pingPer[4] = {10, 10, 10, 10};
uint8_t pingPer2[4] = {10, 10, 10, 10};
bool pingRes[4] = {false, false, false, false};
//char * destips[4] = {"", "", "", ""};
float vcosfi = 220*0.8;
double ACSVolt;
unsigned int mVperAmp = 185;   // 185 for 5A, 100 for 20A and 66 for 30A Module
double ACSVoltage = 0;
//double peak = 0;
double VRMS = 0;
double VMCU = 0;
double AmpsRMS = 0;
int zero, l=0, h=1024;
unsigned wificnt = 0;
boolean pageLoad = false;
unsigned long smplcnt, smplcnt2; //sampleCount;
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
float dd = 0;
byte tempCount = TEMPCOUNT;
volatile double st = 0;
volatile double stv = 85;
volatile float m, m2;
double ex[2] = {0,0};
double calAvg[2] = {0,0};
double weight[2] = {0,0};
short chk[2]={0,0};
#endif
bool isrun[2]={false,false};
volatile bool dosmpl = false;
bool zsampled = false;
uint8_t cont=0;
//end of stats variables
unsigned long prec=0;
unsigned long current = 0;
wl_status_t wfs;
uint8_t wifiState = WIFISTA;
volatile bool wifiConn, keepConn, startWait, endWait;
unsigned long _connectTimeout  = 10*1000;
//int haltPrm[2] = {THALT1,THALT2};
//int haltOfs[2] = {THALT1OFST,THALT2OFST};
uint8_t blocked[2]={0,0};
//unsigned long edelay[2]={0,0};
uint8_t wsnconn = 0;
IPAddress myip(192, 168, 43, 1);
IPAddress mygateway(192, 168, 43, 1);
IPAddress mysubnet(255, 255, 255, 0);
bool roll[2] = {true, true};

uint8_t cols[] = {7,4,2,1,6,5,3,0};
uint8_t ledcnt = 0;
uint8_t ledslct = 0;
uint8_t ledpause = 0;
uint8_t halfStop = STOP_STEP * TBASE / 2;
uint8_t halfProc = MAINPROCSTEP * TBASE / 2;

#if (MCP2317) 
Adafruit_MCP23017_MY mcp;	
#endif

#if(LARGEFW)
RemoteDebug telnet;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
#endif

WiFiEventHandler stationConnectedHandler;
WiFiEventHandler stationDisconnectedHandler;

//char gbuf[50];
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
uint8_t in[NBTN*BTNDIM], out[NBTN*BTNDIM], outPorts[NBTN*BTNDIM];;
float outPwr[NBTN*BTNDIM];
byte doPwrSpl[NBTN*BTNDIM] = {255, 255, 255, 255};
short doZeroSampl = -1;
short doPwrSampl = -1;
bool inflag = false;
Par *pars[PARAMSDIM];
unsigned long *inl = (unsigned long *)in;

ESP8266WebServer server(80);    	// Create a webserver object that listens for HTTP request on port 80
WebSocketsServer webSocket(81);	    // create a websocket server on port 81
ESP8266HTTPUpdateServer httpUpdater;
extern "C"
{
  #include <lwip/icmp.h> // needed for icmp packet definitions
}

// Set global to avoid object removing after setup() routine
Pinger pinger[4];

// fine variabili e costanti dello schedulatore
// Update these with values suitabule for your network.
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//                                      End User config
//--------------------------------------------------------------------------------------
//Port config----------------------------------------------
//LED and relays switches
bool boot=true;
uint8_t swcount=0;
uint8_t wifindx=0;
//End port config------------------------------------------
unsigned int step;
//bool extCmd=false;
uint8_t acts[4];
//numerazione porte
uint8_t outLogic[NBTN*STATUSDIM];
//inizio variabili globali antirimbalzo----------------------------------------------
//fine variabili globali antirimbalzo----------------------------------------------
//dichiara una variabile oggetto della classe WiFiClient (instanzia il client WiFi)
//WiFiClient espClient;
//dichiara una variabile oggetto della classe PubSubClient (instanzia il client MQTT)

// create MQTT
MQTT *mqttClient = NULL;
MQTTC *mqttc = new MQTTC(mqttClient);

bool configLoaded=true;
bool mqttConnected=false;
bool mqttAddrChanged=true;
bool dscnct=false;
//String pr[3] = {"{\"pr1\":\"", "{\"pr2\":\"", "\"}"};

void printIn(){
	DEBUG1_PRINT(F("in array: "));
	DEBUG1_PRINT(in[0]);
	DEBUG1_PRINT(F(", "));
	DEBUG1_PRINT(in[1]);
	DEBUG1_PRINT(F(", "));
	DEBUG1_PRINT(in[2]);
	DEBUG1_PRINT(F(", "));
	DEBUG1_PRINTLN(in[3]);
}

inline float sampletemp(byte c, byte t = 0){
	double temp = -127;
#if(LARGEFW)
	DS18B20.requestTemperatures(); 
	unsigned short cnt = 0;
	do{
		temp = DS18B20.getTempCByIndex(0);
		//DEBUG2_PRINT("Temperature: ");
		//DEBUG2_PRINTLN(temp);
		cnt++;	
		delay(t);
	}while((temp == 85.0 || temp == (-127.0)) && cnt < c);
#endif
return temp;
}

/*inline void initTemp(){
	st = sampletemp(4,30);
	stv = 85;
	DEBUG1_PRINT(F("firstTemp: "));
	DEBUG1_PRINTLN(st);
  /*for(int i=0;i<3;i++){
	  DEBUG1_PRINT(F("Temp: "));
	  DEBUG1_PRINTLN(getTemperature());
	  delay(1000);
  }
}*/

//-----------------------------------------Begin of prototypes---------------------------------------------------------
#if (AUTOCAL_HLW8012 && LARGEFW) 
HLW8012 hlw8012;

// When using interrupts we have to call the library entry point
		// whenever an interrupt is triggered
inline void ICACHE_RAM_ATTR hlw8012_cf1_interrupt() {
	hlw8012.cf1_interrupt();
}

inline void ICACHE_RAM_ATTR hlw8012_cf_interrupt() {
	hlw8012.cf_interrupt();
}

// Library expects an interrupt on both edges
void setInterrupts() {
	attachInterrupt(CF1_PIN, hlw8012_cf1_interrupt, CHANGE);
	attachInterrupt(CF_PIN, hlw8012_cf_interrupt, CHANGE);
}

void calibrate_pwr(double pwr = 0) {
	// Let some time to register values
	//unsigned long timeout = millis();
	//while ((millis() - timeout) < 10000) {
	//	delay(1);
	//}
	DEBUG1_PRINT(F("CALPWR : ")); DEBUG1_PRINTLN(static_cast<ParFloat*>(pars[p(CALPWR)])->val);
	hlw8012.resetMultipliers();
	// Calibrate using a 60W bulb (pure resistive) on a 230V line
	if(pwr > 0){
		hlw8012.expectedActivePower(static_cast<ParFloat*>(pars[p(CALPWR)])->val, pwr);
	}else{
		hlw8012.expectedActivePower(static_cast<ParFloat*>(pars[p(CALPWR)])->val);
	}
	hlw8012.expectedVoltage(static_cast<ParUint8*>(pars[p(ACVOLT)])->val);
	hlw8012.expectedCurrent((float) static_cast<ParFloat*>(pars[p(CALPWR)])->val / static_cast<ParUint8*>(pars[p(ACVOLT)])->val);
	
	DEBUG1_PRINT(F("[HLW] Expected power : ")); DEBUG1_PRINTLN(static_cast<ParFloat*>(pars[p(CALPWR)])->val);
	DEBUG1_PRINT(F("[HLW] Expected voltage : ")); DEBUG1_PRINTLN(static_cast<ParUint8*>(pars[p(ACVOLT)])->val);
	DEBUG1_PRINT(F("[HLW] Expected current : ")); DEBUG1_PRINTLN((float) static_cast<ParFloat*>(pars[p(CALPWR)])->val / static_cast<ParUint8*>(pars[p(ACVOLT)])->val);
	//Save parameter in the system array
	static_cast<ParFloat*>(pars[p(PWRMULT)])->load(hlw8012.getPowerMultiplier());
	static_cast<ParFloat*>(pars[p(CURRMULT)])->load(hlw8012.getCurrentMultiplier());
	static_cast<ParFloat*>(pars[p(VACMULT)])->load(hlw8012.getVoltageMultiplier());
	
	// Show corrected factors
	DEBUG1_PRINT(F("[HLW] New current multiplier : ")); DEBUG1_PRINTLN(hlw8012.getCurrentMultiplier());
	DEBUG1_PRINT(F("[HLW] New voltage multiplier : ")); DEBUG1_PRINTLN(hlw8012.getVoltageMultiplier());
	DEBUG1_PRINT(F("[HLW] New power multiplier   : ")); DEBUG1_PRINTLN(hlw8012.getPowerMultiplier());
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
		DEBUG1_PRINT(F("[HLW] New current multiplier : ")); DEBUG1_PRINTLN(hlw8012.getCurrentMultiplier());
		DEBUG1_PRINT(F("[HLW] New voltage multiplier : ")); DEBUG1_PRINTLN(hlw8012.getVoltageMultiplier());
		DEBUG1_PRINT(F("[HLW] New power multiplier   : ")); DEBUG1_PRINTLN(hlw8012.getPowerMultiplier());

		setInterrupts();
}
#endif

#if (MCP2317) 
inline void getbits(uint8_t num, uint8_t &r, uint8_t &g, uint8_t &b) 
{ 
    r = (num >> 2) & 1;
	g = (num >> 1) & 1;
	b = num & 1;
}
	
inline void setColor(uint8_t num) 
{   
	uint8_t r,g,b;

    //getbits(num, r, g, b);
	//mcp.digitalWrite(GREEN,g);	
	//mcp.digitalWrite(RED,r);		
	//mcp.digitalWrite(BLUE,b);
} 

void printOut(){ 
	if(ledpause == 0){
		if(ledcnt > 0){
			if((ledcnt % 2) == 0)
				//(mcp.digitalRead(ledslct+BTN1U))?setColor(cols[1]):setColor(cols[0]);
				(out[ledslct+BTN1U])?setColor(cols[1]):setColor(cols[0]);
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

void rstldcnt(uint8_t n){   
	ledcnt = 2*(n+1);
	ledslct = n;
	ledpause = 0;
} 

#endif
/*
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
*/
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
	//pars[i][2] - type of form field
	//------------------------------------------
	//i:input, textarea
	//s:select
	//n:no form
	//------------------------------------------
	//p:parameter
	//j:jsonname
	//----------------------------------------------
	//pars[PARAM_NAME] = new ParParam_type(initial_value_saved_on_eeprom_reset, "param_name", json_name", EEPROM_PARAM_OFST, 'j!p!n (type_of_load_from_eeprom)','i|s|n (type_of_load_from_form), new PARAM_NAME_MANAGER_Evnt(PARAM_NAME));
	for(int i=0; i<PARAMSDIM; i++){
		pars[i] = NULL;
	}
	/*1*/pars[MQTTUP1] = new ParUint8(0, "up1", "up1", MQTTUP1OFST, 'j','i', new MQTTBTN_Evnt(MQTTUP1));
	/*2*/pars[MQTTDOWN1] = new ParUint8(0, "down1", "down1", MQTTDOWN1OFST, 'j','i', new MQTTBTN_Evnt(MQTTDOWN1));
	/*3*/pars[MQTTUP2] = new ParUint8(0, "up2", "up2", MQTTUP2OFST, 'j','i', new MQTTBTN_Evnt(MQTTUP2));
	/*4*/pars[MQTTDOWN2] = new ParUint8(0, "down2", "down2", MQTTDOWN2OFST, 'j','i', new MQTTBTN_Evnt(MQTTDOWN2));
	/*4*/pars[MQTTALL] = new ParUint8(0, "all","all", 2,'n','n');
	/*4*/pars[MQTTMAC] = new ParUint8(0, "mac", "mac", 2,'n','n', new MQTTMAC_Evnt());
	/*4*/pars[MQTTIP] = new ParUint8(0, "ip", "ip", 2,'n','n', new MQTTIP_Evnt());
	/*4*/pars[MQTTTEMP] = new ParUint8(0, "temp", "temp", 2, 'n','n', new MQTTTEMP_Evnt());
	/*4*/pars[MQTTMEANPWR] = new ParUint8(0, "avgpwr", "avgpwr", 2,'n','n', new MQTTMEANPWR_Evnt());
	/*4*/pars[MQTTPEAKPWR] = new ParUint8(0, "peakpwr", "peakpwr", 2,'n','n', new MQTTPEAKPWR_Evnt());
	/*4*/pars[MQTTTIME] = new ParUint8(0, "time", "time", 2, 'n','n', new MQTTTIME_Evnt());
	/*4*/pars[MQTTDATE] = new ParUint8(0, "date", "date", 2, 'n','n');
#if (AUTOCAL_HLW8012) 
	/*4*/pars[DOPWRCAL] = new ParUint8(0, "dopwrcal", "dopwrcal", 2, 'n','n', new DOPWRCAL_Evnt());
	/*4*/pars[INSTACV] = new ParUint8(0, "iacvolt", "iacvolt", 2,'n','n', new INSTACV_Evnt());
	/*4*/pars[INSTPWR] = new ParUint8(0, "ipwr", "ipwr", 2,'n','n', new INSTPWR_Evnt());
	//------------------------------------------------------------------------------------------------------------------------------------
	/*42*/pars[p(ACVOLT)] = new ParUint8(230, "acvolt", "acvolt", ACVOLTOFST, 'p','i', new ACVOLT_Evnt());
	/*42*/pars[p(CALPWR)] = new ParFloat(60, "calpwr", "calpwr", CALPWROFST, 'p', 'i', new CALPWR_Evnt());//Must be after ACVOLT!
#else
	/*4*/pars[DOPWRCAL] = new ParUint8(0, "dopwrcal", "dopwrcal", 2, 'n','n');
	/*4*/pars[INSTACV] = new ParUint8(0, "iacvolt", "iacvolt", 2,'n','n');
	/*4*/pars[INSTPWR] = new ParUint8(0, "ipwr", "ipwr", 2,'n','n');
	/*42*/pars[p(ACVOLT)] = new ParUint8(230, "acvolt", "acvolt", ACVOLTOFST, 'n','n');
	/*42*/pars[p(CALPWR)] = new ParFloat(60, "calpwr", "calpwr", CALPWROFST, 'n', 'n');
#endif
	/*5*/pars[p(LOCALIP)] = new ParStr32("ip", "localip","ip");
	/*5*/pars[p(SWROLL1)] = new ParUint8(ROLLMODE1, "swroll1", "swroll1", SWROLL1OFST, 'p', 'i', new SWROLL1_Evnt());
	/*6*/pars[p(SWROLL2)] = new ParUint8(ROLLMODE2, "swroll2", "swroll2", SWROLL2OFST, 'p', 'i', new SWROLL2_Evnt());
	/*7*/pars[p(UTCSDT)] = new ParUint8(1, "utcsdt", "utcsdt", NTPSDTOFST, 'p', 'n', new UTCSDT_Evnt());
	/*8*/pars[p(UTCZONE)] = new ParInt(1, "utczone", "utczone", NTPZONEOFST, 'p', 'i', new UTCZONE_Evnt());
	/*9*/pars[p(THALT1)] = new ParLong(thalt1,"thalt1", "thalt1", THALT1OFST, 'p','i', new THALTX_Evnt());
	/*10*/pars[p(THALT2)] = new ParLong(thalt2, "thalt2", "thalt2", THALT2OFST, 'p','i', new THALTX_Evnt());
	/*11*/pars[p(THALT3)] = new ParLong(thalt3,"thalt3", "thalt3", THALT3OFST, 'p','i', new THALTX_Evnt());
	/*12*/pars[p(THALT4)] = new ParLong(thalt4, "thalt4", "thalt4", THALT4OFST, 'p','i', new THALTX_Evnt());
#if (AUTOCAL) 	
	/*13*/pars[p(STDEL1)] = new ParLong(TENDCHECK*1000, "stdel1", "stdel1", STDEL1OFST, 'p','i', new STDELX_Evnt());
	/*14*/pars[p(STDEL2)] = new ParLong(TENDCHECK*1000, "stdel2", "stdel2", STDEL2OFST, 'p','i', new STDELX_Evnt());
#else	
	/*13*/pars[p(STDEL1)] = new ParLong(400, "stdel1", "stdel1", STDEL1OFST, 'p','i', new STDELX_Evnt());
	/*14*/pars[p(STDEL2)] = new ParLong(400, "stdel2", "stdel2", STDEL2OFST, 'p','i', new STDELX_Evnt());
#endif	
	/*15*/pars[p(VALWEIGHT)] = new ParFloat(0.5, "valweight", "valweight", VALWEIGHTOFST, 'p','i', new VALWEIGHT_Evnt());
	/*16*/pars[p(TLENGTH)] = new ParFloat(53, "tlength","tlength", TLENGTHOFST, 'p','i', new TLENGTH_Evnt());
	/*17*/pars[p(BARRELRAD)] = new ParFloat(3.37, "barrelrad", "barrelrad", BARRELRADOFST, 'p','i', new BARRELRAD_Evnt());
	/*18*/pars[p(THICKNESS)] = new ParFloat(1.5, "thickness", "thickness", THICKNESSOFST, 'p','i', new THICKNESS_Evnt());
	/*19*/pars[p(UTCADJ)] = new ParInt(0, "utcadj", "utcadj", NTPADJUSTOFST, 'p','i', new UTCADJ_Evnt());
	/*20*/pars[p(SLATSRATIO)] = new ParFloat(0.8, "slatsratio", "slatsratio", SLATSRATIOFST, 'p','i', new SLATSRATIO_Evnt());
	/*21*/pars[p(UTCSYNC)] = new ParInt(50, "utcsync", "utcsync", NTPSYNCINTOFST, 'p','i', new UTCSYNC_Evnt());
	/*22*/pars[p(MQTTID)] = new ParStr32(String(WiFi.macAddress()).c_str(), "mqttid", "devid", MQTTIDOFST, 'p','i', new MQTTID_Evnt());
	/*23*/pars[p(MQTTOUTTOPIC)] = new ParStr32(OUTTOPIC, "mqttouttopic", "mqttouttopic", OUTTOPICOFST, 'p','i');
	/*23*/pars[p(MQTTLOG)] = new ParStr32(LOGPATH, "mqttlog", "mqttlog", MQTTLOGOFST, 'p','i');
	/*24*/pars[p(MQTTINTOPIC)] = new ParStr32(INTOPIC, "mqttintopic", "mqttintopic", INTOPICOFST, 'p','i', new MQTTINTOPIC_Evnt());
	/*25*/pars[p(CLNTSSID1)] = new ParStr32(SSID1, "clntssid1", "clntssid1", WIFICLIENTSSIDOFST1, 'p','i', new WIFICHANGED_Evnt());
	/*26*/pars[p(CLNTPSW1)] = new ParStr32(PSW1, "clntpsw1", "clntpsw1", WIFICLIENTPSWOFST1, 'p','i', new WIFICHANGED_Evnt());
	/*27*/pars[p(CLNTSSID2)] = new ParStr32(SSID2, "clntssid2", "clntpsw1", WIFICLIENTSSIDOFST2, 'p','i', new WIFICHANGED_Evnt());
	/*28*/pars[p(CLNTPSW2)] = new ParStr32(PSW2, "clntpsw2", "clntpsw2", WIFICLIENTPSWOFST2, 'p','i', new WIFICHANGED_Evnt());
	/*29*/pars[p(APPSSID)] = new ParStr32(SSIDAP, "appssid", "appssid", WIFIAPSSIDOFST, 'p','i');
	/*30*/pars[p(APPPSW)] = new ParStr32(PSWAP, "apppsw", "apppsw", WIFIAPPPSWOFST, 'p','i');
	/*31*/pars[p(WEBUSR)] = new ParStr32(WBUSR, "webusr", "webusr", WEBUSROFST, 'p','i');
	/*32*/pars[p(WEBPSW)] = new ParStr32(WBPSW, "webpsw", "webpsw", WEBPSWOFST, 'p','i');
	/*33*/pars[p(MQTTUSR)] = new ParStr32(MQUSR, "mqttusr", "mqttusr", MQTTUSROFST, 'p','i');//uno dei due!
	/*34*/pars[p(MQTTPSW)] = new ParStr32(MQPSW, "mqttpsw", "mqttpsw", MQTTPSWOFST, 'p','i', new MQTTCONNCHANGED_Evnt());
	/*35*/pars[p(MQTTADDR)] = new ParStr64(MQTTSRV, "mqttaddr", "mqttaddr", MQTTADDROFST, 'p','i', new MQTTADDR_Evnt(),true);//va prima!
	/*36*/pars[p(MQTTPORT)] = new ParLong(MQTTPRT, "mqttport", "mqttport", MQTTPORTOFST, 'p','i', pars[p(MQTTADDR)]->e,true);
	/*37*/pars[p(WSPORT)] = new ParStr32(WSPRT, "wsport", "wsport", WSPORTOFST, 'p','i');
	/*38*/pars[p(MQTTPROTO)] = new ParStr32(MQTTPT, "mqttproto", "mqttproto", MQTTPROTOFST, 'p','i');
	/*39*/pars[p(NTPADDR1)] = new ParStr64(NTP1, "ntpaddr1", "ntpaddr1", NTP1ADDROFST, 'p','i', new NTPADDR1_Evnt());
	/*40*/pars[p(NTPADDR2)] = new ParStr64(NTP2, "ntpaddr2", "ntpaddr2", NTP2ADDROFST, 'p','i', new NTPADDR2_Evnt());
#if (AUTOCAL_HLW8012) 
	/*41*/pars[p(VACMULT)] = new ParFloat(hlw8012.getVoltageMultiplier(), "vacmult", "vacmult", VACMULTOFST, 'p','n', new VACMULT_Evnt());
	/*41*/pars[p(PWRMULT)] = new ParFloat(hlw8012.getPowerMultiplier(), "pwrmult", "pwrmult", PWRMULTOFST, 'p','i', new PWRMULT_Evnt());
	/*41*/pars[p(CURRMULT)] = new ParFloat(hlw8012.getCurrentMultiplier(), "currmult", "currmult", CURRMULTOFST, 'p','n', new CURRMULT_Evnt());
#else	
	/*41*/pars[p(PWRMULT)] = new ParFloat(1, "pwrmult", "pwrmult", PWRMULTOFST, 'n','n');
	/*41*/pars[p(VACMULT)] = new ParFloat(1, "vacmult", "vacmult", VACMULTOFST, 'n','n');
	/*41*/pars[p(CURRMULT)] = new ParFloat(1, "currmult", "currmult", CURRMULTOFST, 'n','n');
#endif
	/*43*/pars[p(ONCOND1)] = new ParVarStr("-1", "oncond1", "oncond1", 2, 'p','n', new ONCOND1_Evnt());
	/*44*/pars[p(ONCOND2)] = new ParVarStr("-1", "oncond2","oncond2", 2, 'p','n', new ONCOND2_Evnt());
	/*45*/pars[p(ONCOND3)] = new ParVarStr("-1", "oncond3","oncond3", 2, 'p','n', new ONCOND3_Evnt());
	/*46*/pars[p(ONCOND4)] = new ParVarStr("-1", "oncond4","oncond4", 2, 'p','n', new ONCOND4_Evnt());
	///*47*/pars[p(ONCOND5)] = new ParVarStr("(td1=4000)|(ma1=0)|(ma4=2)|(tsmpl4=4)|(oe1=1)", "oncond5","oncond5", 0, 'p','n', new ONCOND5_Evnt());
	/*47*/pars[p(ONCOND5)] = new ParVarStr("-1", "oncond5","oncond5", 0, 'p','n', new ONCOND5_Evnt());
	/*48*/pars[p(ACTIONEVAL)] = new ParVarStr("-1","onaction","onaction", 2, 'p','n', new ACTIONEVAL_Evnt());
	///*5*/pars[p(WIFICHANGED)] = new ParUint8(0, "WIFICHANGED","", new WIFICHANGED_Evnt());
	///*5*/pars[p(CONFLOADED)] = new ParUint8(0, "CONFLOADED","");
	///*5*/pars[p(MQTTADDRMODFIED)] = new ParUint8(0, "MQTTADDRMODFIED","");
	///*5*/pars[p(TOPICCHANGED)] = new ParUint8(0, "TOPICCHANGED","", new TOPICCHANGED_Evnt());
	///*5*/pars[p(MQTTCONNCHANGED)] = new ParUint8(0, "MQTTCONNCHANGED","");
	///*5*/pars[p(TIMINGCHANGED)] = new ParUint8(0, "TIMINGCHANGED","");
	/*5*/pars[p(SWACTION1)] = new ParUint8(0, "SWACTION1","");
	/*5*/pars[p(SWACTION2)] = new ParUint8(0, "SWACTION2","");
	/*5*/pars[p(SWACTION3)] = new ParUint8(0, "SWACTION3","");
	/*5*/pars[p(SWACTION4)] = new ParUint8(0, "SWACTION4","");
	/*5*/pars[p(UTCVAL)] = new ParLong(0, "UTCVAL","utcval");
	/*5*/pars[p(LOGSLCT)] = new ParUint8(LOGSEL, "logslct","logslct", LOGSLCTOFST, 'n', 'n', new LOGSLCT_Evnt());
	///*5*/pars[p(DEVICEID)] = new ParStr32(DEVID, "devid","deviceid", DEVICEIDOFST, 'p', 'i', new DEVICEID_Evnt());
	///*5*/pars[p(SWSPLDPWR1)] = new ParUint8(0, "SWSPLDPWR1","", SWSPLDPWR1OFST1, 'n', 'n', new SWSPLDPWR1_Evnt());
	///*5*/pars[p(SWSPLDPWR2)] = new ParUint8(0, "SWSPLDPWR1","", SWSPLDPWR1OFST2, 'n', 'n', new SWSPLDPWR2_Evnt());
	///*5*/pars[p(SWSPLDPWR3)] = new ParUint8(0, "SWSPLDPWR1","", SWSPLDPWR1OFST3, 'n', 'n', new SWSPLDPWR3_Evnt());
	///*5*/pars[p(SWSPLDPWR4)] = new ParUint8(0, "SWSPLDPWR1","", SWSPLDPWR1OFST4, 'n', 'n', new SWSPLDPWR4_Evnt());
	//printparams();
}

inline bool gatedfn(float val, uint8_t n, float rnd){
	//n: numero di porte
	changed[n] = (val < asyncBuf[n] - rnd || val > asyncBuf[n] + rnd);
	asyncBuf[n] = val;            // valore di val campionato al loop precedente 
	return changed;
}

#if (AUTOCAL_ACS712) 	
float getAmpRMS(float ACSVolt){
	ACSVolt = (double) (ACSVolt * 5.0) / 1024.0;
	VRMS = ACSVolt * 0.707;
	AmpsRMS = (double) (VRMS * 1000) / mVperAmp;
	if((AmpsRMS > -0.015) && (AmpsRMS < 0.008)){ 
		AmpsRMS = 0.0;
	}
	return AmpsRMS*vcosfi;
}
#endif

void setSWMode(uint8_t mode, uint8_t n){
	roll[n] = mode;
	isrun[n] = false;
	DEBUG2_PRINT("setSWMode");
	DEBUG2_PRINTLN(n);
	DEBUG2_PRINT(": ");
	DEBUG2_PRINTLN(mode);
	//setSWModeTap(mode,0);	
	//readModeAndPub(n);
}

uint8_t inline getSWMode(uint8_t n){
	return roll[n];
}

double getTemperature(){
	double temp = -127;
	
#if(LARGEFW)
	temp = sampletemp(3);

	if(firstTemp > 0){
		firstTemp--;
		st = temp;
		stv = 85.0;
		DEBUG1_PRINT("firstTemp: ");
		DEBUG1_PRINTLN(st);
	}else{
		if((temp >= (double) st - TSIGMA*stv && temp <= (double) st + TSIGMA*stv) || tempCount == 0){
			if(tempCount == 0){
				DEBUG1_PRINT("Dentro veloce: ");
				tempCount = TEMPCOUNT;
				st = (double) st*(1-EMAA*TEMPCOUNT) + temp*EMAA*TEMPCOUNT;
				if(temp >= st){
					stv = (double) stv*(1-EMAB*TEMPCOUNT) + (temp - st)*EMAB*TEMPCOUNT;
				}else{
					stv = (double) stv*(1-EMAB*TEMPCOUNT) + (st - temp)*EMAB*TEMPCOUNT;
				}
			}else{
				DEBUG1_PRINT("Dentro: ");
				st = (double) st*(1-EMAA) + temp*EMAA;
				if(temp >= st){
					stv = (double) stv*(1-EMAB) + (temp - st)*EMAB;
				}else{
					stv = (double) stv*(1-EMAB) + (st - temp)*EMAB;
				}
			}
		}else{
			DEBUG1_PRINT("Fuori: ");
			//after TEMPCOUNT times are to be considered valid values!
			tempCount--;
		}
	}
#endif
	DEBUG1_PRINT(", instTemp: ");
	DEBUG1_PRINT(temp);
	DEBUG1_PRINT(", tmedia: ");
	DEBUG1_PRINT(st);
	DEBUG1_PRINT(", finestra: ");
	DEBUG1_PRINTLN(TSIGMA*stv);
	
	return st;
}

//parser actions callBack (assignements)
//configurazioni provenienti da eventi locali
double actions(char *key, double val)
{	
	uint8_t n=-1;
	if(key[0]=='t'){
		if(roll[0] == false){
			if(strcmp(key,"tsmpl1")==0){
				startCnt(0,(unsigned long)val,SMPLCNT1);
			}else if(strcmp(key,"tsmpl2")==0){
				startCnt(0,(unsigned long)val,SMPLCNT2);
			}
			//haldelay 1 e 2
			if(key[1]=='d'){
				n = key[2] - 49;
				DEBUG2_PRINT("d");
				DEBUG2_PRINTLN(n);
				if(n>=0){
					//writeHaltDelay(val,n);
					updtConf(p(THALT1+n), String(val));
					setHaltDelay(val,n);
					readActModeAndPub(n);
				}
			}
		}
		
		if(roll[1] == false){
			if(strcmp(key,"tsmpl3")==0){
				startCnt(0,(unsigned long)val,SMPLCNT3);
			}else if(strcmp(key,"tsmpl4")==0){
				startCnt(0,(unsigned long)val,SMPLCNT4);
			}
			//haldelay 3 e 4
			if(key[1]=='d'){
				n = key[2] - 49;
				DEBUG2_PRINT("d");
				DEBUG2_PRINTLN(n);
				if(n>=0){
					//writeHaltDelay(val,n);
					updtConf(p(THALT1+n), String(val));
					setHaltDelay(val,n);
					readActModeAndPub(n);
				}
			}
		}
	}else if(key[0]=='m'){
		uint8_t act=0;
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
				updtConf(p(SWROLL1), String(1));
			}else if(val==0){
				setSWMode(0,0);
				//writeSWMode(0,0);
				updtConf(p(SWROLL1), String(0));
			}
		}else if(strcmp(key,"mode2")==0){
			if(val==1){
				setSWMode(1,1);
				//writeSWMode(1,1);
				updtConf(p(SWROLL2), String(0));
			}else if(val==0){
				setSWMode(0,1);
				//writeSWMode(0,1);
				updtConf(p(SWROLL2), String(1));
			}
		}
	}else if(key[0]=='u'){
		uint8_t act=0;
		if(key[1]=='p'){
			if(roll[0] == true){
				DEBUG2_PRINT("up1: ");
				DEBUG2_PRINTLN(val);
				if(key[2] == '1' && val > 0 && !isOnTarget(val, 0) && !isrun[0] && !isCalibr()){
					static_cast<ParUint8*>(pars[MQTTUP1])->load((int)val);				
					static_cast<ParUint8*>(pars[MQTTUP1])->doaction(0);
				}
			}
			if(roll[1] == true){
				if(key[2] == '2' && val > 0 && !isOnTarget(val, 1) && !isrun[1] && !isCalibr()){
					static_cast<ParUint8*>(pars[MQTTUP2])->load((int)val);		
					static_cast<ParUint8*>(pars[MQTTUP2])->doaction(0);
				}
			}
		}
		act=val;
	}else if(key[0]=='a'){
		uint8_t act=0;
		if(roll[0] == false){
			bool pub = false;
			if(strcmp(key,"actlgcd1")==0){
				pub = setDiffActionLogic(val,0);
			}else if(strcmp(key,"actlgcd2")==0){
				pub = setDiffActionLogic(val,1);
			}else if(strcmp(key,"actlgc1")==0){
				pub = setActionLogic(val,0);
			}else if(strcmp(key,"actlgc2")==0){
				pub = setActionLogic(val,1);
			}
			scriviOutDaStato(0);
			readStatesAndPub();
				
		}
		if(roll[1] == false){
			bool pub = false;
			if(strcmp(key,"actlgcd3")==0){
				pub = setDiffActionLogic(val,2);
			}else if(strcmp(key,"actlgcd4")==0){
				pub = setDiffActionLogic(val,3);
			}else if(strcmp(key,"actlgc3")==0){
				pub = setActionLogic(val,2);
			}else if(strcmp(key,"actlgc4")==0){
				pub = setActionLogic(val,3);
			}
			scriviOutDaStato(1);
			readStatesAndPub();
		}
		act=val;
	}else if(key[0]=='d'){		
		uint8_t act=0;
		if(key[1]=='w'){
			if(roll[0] == true){
				DEBUG2_PRINT("dw1: ");
				DEBUG2_PRINTLN(val);
				if(key[2] == '1' && val > 0 && !isOnTarget(val, 0) && !isrun[0] && !isCalibr()){
					DEBUG2_PRINT("dw1: ");
					DEBUG2_PRINTLN(val);
					static_cast<ParUint8*>(pars[MQTTDOWN1])->load((int)val);			
					static_cast<ParUint8*>(pars[MQTTDOWN1])->doaction(0);
				}
			}
			if(roll[1] == true){
				if(key[2] == '2' && val > 0 && !isOnTarget(val, 1) && !isrun[1] && !isCalibr()){
					static_cast<ParUint8*>(pars[MQTTDOWN2])->load((int)val);		
					static_cast<ParUint8*>(pars[MQTTDOWN2])->doaction(0);
				}
			}
		}
		act=val;
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
		////return val;
	}else if(key[0]=='o'){
		bool oe = val;
		DEBUG2_PRINT("oeeeeee: ");
				DEBUG2_PRINTLN(oe);
		if(strcmp(key,"oe1")==0){
			setOE(oe,0);
		}else if(strcmp(key,"oe2")==0){
			setOE(oe,1);
		}else if(strcmp(key,"oe3")==0){
			setOE(oe,2);
		}else if(strcmp(key,"oe4")==0){
			setOE(oe,3);
		}
		//return val;
	}else if(key[0]=='s'){
		if(strcmp(key,"sdt")==0){
			updtConf(p(UTCSDT), String(val));
			setSDT((uint8_t) val);
		}
	}
	
	return true;
}

//parser function calls
double variables(char *key){
	double result;
	
	if(key[0]=='t'){
		if(strcmp(key,"tsec")==0){ //secondi 
			result = second();
		}else if(strcmp(key,"tmin")==0){//minuti 
			result = minute();
		}else if(strcmp(key,"thour")==0){//ora
			result = hour();
		}else if(key[1]=='e' && key[2]==':'){// 2019:07:30/03:57:30 o 2019-10-08T09:28:40
			key += strlen("te:");
			result = makeTime(fromStrToTimeEl(key));
			//DEBUG1_PRINT("te: ");
			//DEBUG1_PRINTLN(result);
		}else if(strcmp(key,"tenow")==0){
			result = (unsigned long) getUNIXTime();
			//DEBUG1_PRINT("tenow: ");
			//DEBUG1_PRINTLN(result);
			
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
	}else /*if(key[0]=='c'){
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
	}else */if(key[0]=='o'){
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
		byte n = key[1] - 49;
		pingCnt[n] = (pingCnt[n] + 1) % 3600;
		if(!(pingCnt[n] % pingPer2[n])){
			if(key[2]==':'){
				key += strlen("s1:");
				DEBUG2_PRINT("\nkey: ");
				DEBUG2_PRINT(key);
				
				char* k;
				bool esci = false;
				int i;
				for(i=0; key[i] != ':' && key[i] != '\0'; i++);
				if(key[i] == ':'){
					key[i] = '\0';
					k = (key+i+1);
					for(i=0; k[i] != ':' && key[i] != '\0'; i++);
					if(k[i] == ':'){
						k[i] = '\0';
						nping[n] = atoi(k);
						k = (k+i+1);
						for(i=0; k[i] != ':' && key[i] != '\0'; i++);
						if(k[i] == ':')
							k[i] = '\0';
						pingPer[n] = atoi(k);
						if(pingPer[n] < 0)
							pingPer[n] = 0;
						
					}
				}
				
				DEBUG2_PRINT(", n: ");
				DEBUG2_PRINT(n); 
				DEBUG2_PRINT(", key: ");
				DEBUG2_PRINT(key); 
				DEBUG2_PRINT(", nping[n]: ");
				DEBUG2_PRINT(nping[n]); 
				DEBUG2_PRINT(", pingPer[n]: ");
				DEBUG2_PRINT(pingPer[n]); 
				

				//(tokens[1] != NULL) && (nping[n] = atoi(tokens[1]));
				//(tokens[2] != NULL) && (pingPer[n] = atoi(tokens[2]));
				
				DEBUG2_PRINT(", ip: ");
				//DEBUG2_PRINT(key);
				IPAddress ip;
				if (!WiFi.hostByName(key, ip))
					ip.fromString(key);
				DEBUG2_PRINT(ip.toString());
				pinger[n].Ping(ip);
				
				if(lastPing[n] >= nping[n]){
					pingRes[n] = 1;
				}else if(lastPing[n] == 0){
					pingRes[n] = 0;
				}
			}
		}
		result = pingRes[n];
		DEBUG2_PRINT(", result: ");
		DEBUG2_PRINTLN(result);
	}else if(key[0] =='p'){
		if(strcmp(key,"percpos2")==0){
			result = percfdbck(1);
		}else if(strcmp(key,"pwrall")==0){
			result = overallSwPower;
		}else
#if (AUTOCAL_ACS712) 
		if(strcmp(key,"ptapavg1")==0){
			result = getAmpRMS(getAVG(0)/2);
		}else if(strcmp(key,"ptapavg2")==0){
			result = getAmpRMS(getAVG(1)/2);
		}else
#endif 		
		if(strcmp(key,"percpos1")==0){
			result = percfdbck(0);
			DEBUG2_PRINT(" percpos1: ");
			DEBUG2_PRINTLN(result);
		}
	}else if(key[0]=='i'){
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
	}/*else if(strcmp(key,"temp")==0){
		result = getTemperature();
	}*/
	
	return result;
}
/*
void printMcpRealOut(){
	char s[47];
	uint16_t m;

#if (MCP2317)	
	m = mcp.readGPIOAB();
	sprintf(s, "Lettura uscite MCP: - %d%d%d%d ", mcp.digitalRead(OUT1EU), mcp.digitalRead(OUT1DD), mcp.digitalRead(OUT2EU), mcp.digitalRead(OUT2DD));
	DEBUG1_PRINT(s);
	//sprintf(s," readGPIOAB: "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" ", BYTE_TO_BINARY(m>>8), BYTE_TO_BINARY(m));
	//DEBUG1_PRINT(s);
#else
	m = (uint16_t)GPI;
	sprintf(s,"GPIO: "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" ", BYTE_TO_BINARY(m>>8), BYTE_TO_BINARY(m));
	DEBUG1_PRINT(s);
#endif
	sprintf(s,"%d%d%d%d\n", out[0], out[1], out[2], out[3]);
	DEBUG1_PRINTLN(s);
}
*/
void scriviOutDaStato(byte n){
	DEBUG2_PRINT("Evento scriviOutDaStato n: ");
	DEBUG2_PRINT(n);
	DEBUG2_PRINT(" out 1: ");
	DEBUG2_PRINT(out[n*BTNDIM]);
	DEBUG2_PRINT(" out 2: ");
	DEBUG2_PRINT(out[1+n*BTNDIM]);
#if (MCP2317) 
	//uint8_t out2[4];
	//out2[0]=out2[1]=out2[2]=out2[3]=HIGH;
	mcp.writeOuts(out,n);
	//mcp.digitalWrite(OUT1EU + n*BTNDIM,out[n*BTNDIM]);	
	//mcp.digitalWrite(OUT1DD + n*BTNDIM,out[1 + n*BTNDIM]);	
#else										
	digitalWrite(outPorts[n*BTNDIM],out[n*BTNDIM]);	
	digitalWrite(outPorts[1+n*BTNDIM],out[1 + n*BTNDIM]);			
#endif
	isrun[0] = (outLogic[ENABLES]==HIGH) && roll[0] > 0;						
	isrun[1] = (outLogic[ENABLES+STATUSDIM]==HIGH) && roll[1] > 0;		
	mov = isrun[0] || isrun[1];
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
	  //DEBUG2_PRINTLN(WiFi.softAP((confcmd[APPSSID]).c_str()) ? F("Ready") : F("Failed!"));
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
	  //DEBUG2_PRINTLN(WiFi.softAP((confcmd[APPSSID]).c_str()) ? F("Ready") : F("Failed!"));
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
 
     DEBUG2_PRINT("\nNetwork name: ");
     DEBUG2_PRINT(WiFi.SSID(i));
     DEBUG2_PRINT("\nSignal strength: ");
     DEBUG2_PRINT(WiFi.RSSI(i));
     DEBUG2_PRINT("\n-----------------------");
 
  }
}

//wifi setup function
void setup_wifi(int wifindx) {
	//ESP.wdtFeed();
	wifindx = wifindx*2;  //client1 e client2 hanno indici contigui nell'array confcmd
	// We start by connecting to a WiFi network
	//DEBUG2_PRINT(F("Connecting to "));
	DEBUG1_PRINTLN(pars[p(CLNTSSID1+wifindx)]->getStrVal());
    DEBUG1_PRINTLN(F(" as wifi client..."));
	if (wifiConn) {
		//importante! Altrimenti tentativi potrebbero collidere con una connessione appena instaurata
		DEBUG1_PRINTLN(F("Already connected. Bailing out."));
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
			DEBUG1_PRINT(F("Begin status: "));
			DEBUG1_PRINTLN(WiFi.begin((const char*) static_cast<ParStr32*>(pars[p(CLNTSSID1+wifindx)])->val, (const char*) static_cast<ParStr32*>(pars[p(CLNTPSW1+wifindx)])->val));
			//interrupts();
		} else {
			if (WiFi.SSID()) {
				DEBUG1_PRINTLN(F("Using last saved values, should be faster"));
				//trying to fix connection in progress hanging
				ETS_UART_INTR_DISABLE();
				wifi_station_disconnect();
				ETS_UART_INTR_ENABLE();
				WiFi.begin();
			} else {
				DEBUG1_PRINTLN(F("No saved credentials"));
			}
		}
		IPAddress ip = WiFi.localIP();
		ip.toString().toCharArray(IP, 16);
		pars[p(LOCALIP)]->load(IP);
		DEBUG1_PRINT(F("AP client IP address = "));
		DEBUG1_PRINTLN(((ParStr32*)pars[p(LOCALIP)])->val);
	}	

	DEBUG1_PRINTLN (F("\n******************* begin ***********"));
    WiFi.printDiag(Serial);
	DEBUG1_PRINTLN (F("\n******************* end ***********"));

	//WiFi.enableAP(true);
  //}
}

#if (LARGEFW)
void setup_mDNS() {
	if (MDNS.begin((const char*) static_cast<ParStr32*>(pars[p(LOCALIP)])->val)) {              // Start the mDNS responder for esp8266.local
		DEBUG1_PRINTLN(F("mDNS responder started"));
	} else {
		DEBUG1_PRINTLN(F("Error setting up MDNS responder!"));
	}
}
#endif

void startWebSocket() { // Start a WebSocket server
  webSocket.begin();                          // start the websocket server
  webSocket.onEvent(webSocketEvent);          // if there's an incomming websocket message, go to function 'webSocketEvent'
  DEBUG1_PRINTLN("WebSocket server started.");
}

void mqttReconnect() {
	// Loop until we're mqttReconnecte
	if(mqttClient!=NULL){
		DEBUG_PRINTLN(F("Distruggo l'oggetto MQTT client."));
		DEBUG_PRINTLN(F("Mi disconetto dal server MQTT"));
		//if(mqttClient->isConnected()){
		if(mqttConnected){
			mqttClient->disconnect();
		}
		delay(100);
		//ESP.wdtFeed();
		//mqttClient->~MQTT();
		//noInterrupts ();
		while(mqttClient->isConnected())
			delay(50);
		DEBUG_PRINTLN(F("Sono disconesso dal server MQTT"));
		DEBUG_PRINTLN(F("Chiamo il distruttore dell'oggetto MQTT"));
		//ESP.wdtFeed();
		//ESP.wdtDisable(); 
		//noInterrupts ();
		mqttClient->~MQTT();
		//interrupts ();
		//ESP.wdtEnable(0); 
		//ESP.wdtFeed();
		//interrupts ();
		delay(50);
		//DEBUG2_PRINTLN(F("Cancello la vechia istanza dell'oggetto MQTT"));
		DEBUG_PRINTLN(F("Annullo istanza dell'oggetto MQTT"));
		//ESP.wdtFeed();
		mqttClient = NULL;
		//interrupts ();
	}
	DEBUG_PRINTLN(F("Instanzio un nuovo oggetto MQTT client."));
	/////noInterrupts ();
	mqttClient = new MQTT((const char *)(static_cast<ParStr32*>(pars[p(MQTTID)]))->val, (const char *)(static_cast<ParStr64*>(pars[p(MQTTADDR)]))->val, (unsigned int) (static_cast<ParLong*>(pars[p(MQTTPORT)]))->val);
	mqttc->setMQTTClient(mqttClient);
	/////interrupts ();
    DEBUG_PRINTLN(F("Registro i callback dell'MQTT."));
	DEBUG_PRINT(F("Attempting MQTT connection to: "));
	DEBUG_PRINT(pars[p(MQTTADDR)]->getStrVal());
	DEBUG_PRINT(F(", with ClientID: "));
	DEBUG_PRINT(pars[p(MQTTID)]->getStrVal());
	DEBUG_PRINTLN(F(" ..."));

	delay(100);
	if(mqttClient==NULL){
		DEBUG1_PRINTLN(F("ERROR on mqttReconnect! MQTT client is not allocated."));
	}
	else
	{
		mqttClient->onData(mqttCallback);
		mqttClient->onConnected([]() {
			DEBUG_PRINTLN(F("mqtt: onConnected([]() dice mi sono riconnesso."));
			mqttcnt = 0;
			//Altrimenti dice che è connesso ma non comunica
			mqttClient->subscribe(static_cast<ParStr32*>(pars[p(MQTTINTOPIC)])->val); 
			delay(50);
			DEBUG1_PRINT(F("mqtt: Subsribed to: "));
			DEBUG1_PRINTLN(static_cast<ParStr32*>(pars[p(MQTTINTOPIC)])->val);
			mqttClient->publish((const char *)(static_cast<ParStr32*>(pars[p(MQTTOUTTOPIC)]))->val, (const char *)(static_cast<ParStr32*>(pars[p(MQTTID)]))->val, 32);
			pars[p(LOGSLCT)]->doaction(0);	
			mqttConnected=true;//bho
			readParamAndPub(MQTTDATE,printUNIXTimeMin(gbuf));
			readStatesAndPub();
			sensorStatePoll();
		});
		
		mqttClient->onDisconnected([]() {
			//DEBUG2_PRINTLN("MQTT disconnected.");
			DEBUG_PRINTLN(F("MQTT: onDisconnected([]() dice mi sono disconnesso."));
			mqttConnected=false;
		});
		
		mqttClient->setUserPwd((const char*)static_cast<ParStr32*>(pars[p(MQTTUSR)])->val, (const char*) static_cast<ParStr32*>(pars[p(MQTTPSW)])->val);
		//////noInterrupts ();
		if((wifiConn == true)&& WiFi.status()==WL_CONNECTED && WiFi.getMode()==WIFI_STA){
			DEBUG1_PRINTLN(F("MQTT: Eseguo la prima connect."));
			mqttClient->connect();
			delay(50);
		}
		
		//////interrupts ();
		//delay(50);
		//mqttClient->subscribe(static_cast<ParStr32*>(pars[p(MQTTINTOPIC)])->val);
		//mqttClient->publish((const char *)(static_cast<ParStr32*>(pars[p(MQTTOUTTOPIC)]))->val, (const char *)static_cast<ParStr32*>(pars[p(MQTTID)])->val, 32);
	}
}

void mqttCallback(String &topic, String &response) {
	//funzione eseguita dal subscriber all'arrivo di una notifica
	//decodifica la stringa JSON e la trasforma nel nuovo vettore degli stati
	//il vettore corrente degli stati verr� sovrascritto
	//applica la logica ricevuta da remoto sulle uscite locali (led)
    
	int v;
#if defined (_DEBUG) || defined (_DEBUGR)	
	DEBUG1_PRINT(F("Message arrived on topic: ["));
	DEBUG1_PRINT(topic);
	DEBUG1_PRINT(F("], "));
	DEBUG1_PRINTLN(response);
#endif	
	//v = parseJsonFieldToInt(response, mqttJson[0], ncifre);
	//digitalWrite(OUTSLED, v); 
   
	parseJsonFieldArrayToStr(response, pars, ncifre+500, EXTCONFDIM,0,'#',"|");
    //inr: memoria tampone per l'evento asincrono scrittura da remoto
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) { // When a WebSocket message is received
  DEBUG2_PRINT("webSocketEvent(");
  DEBUG2_PRINT(num);
  DEBUG2_PRINT(", ");
  DEBUG2_PRINT(type);
  char s[300];
  
  switch (type) {
    case WStype_DISCONNECTED:             // if the websocket is disconnected
		sprintf(s,"\n[%u] Disconnected!", num);
		DEBUG2_PRINT(s);
		wsnconn--;
    break;
    case WStype_CONNECTED: {              // if a new websocket connection is established
		wsnconn++;
        IPAddress wip = webSocket.remoteIP(num);
		sprintf(s,"[%u] Connected from %d.%d.%d.%d url: %s\n", num, wip[0], wip[1], wip[2], wip[3], payload);
		DEBUG2_PRINT(s);
		readStatesAndPub();
	}
    break;
    case WStype_TEXT:                     // if new text data is received 
		sprintf(s,"[%u] get Text: %s\r\n", num, payload);
		DEBUG2_PRINT(s);
		String str = String((char *)payload);
		String str2 = String("");
		mqttCallback(str2, str);
	break;
  }
}

//legge il valore dello stato dei toggle e li pubblica sul broker come stringa JSON
void readStatesAndPub(bool all){

  //DEBUG2_PRINTLN(F("\nreadStatesAndPub")); 
  
  //crea una stringa JSON con i valori  dello stato corrente dei pulsanti

  //vals=digitalRead(OUTSLED); //legge lo stato del led di stato
  //crea una stringa JSON con i valori  dello stato corrente dei pulsanti
  String s=openbrk;	
  DEBUG1_PRINTLN(F("\openbrk")); 
  DEBUG1_PRINTLN(openbrk); 
  if(roll[0] == true){
	  s += pars[MQTTUP1]->getStrJsonName()+twodot+(outLogic[ENABLES] && (outLogic[DIRS]==LOW))+comma; 	//up1 DIRS=HIGH
	  s += pars[MQTTDOWN1]->getStrJsonName()+twodot+(outLogic[ENABLES] && (outLogic[DIRS]==HIGH))+comma;    //down1  DIRS=LOW
	  if(blocked[0]>0){
		  s+= (String) pars[MQTTUP1]->getStrJsonName()+"blk1"+twodot+blocked[0]+comma;		//blk1
	  }
	  s+= "pr1"+twodot+String(percfdbck(0))+comma;		//pr1
	  s+= "tr1"+twodot+String(getCronoCount(0))+comma;			//tr1
  }else{
	  s += pars[MQTTUP1]->getStrJsonName()+twodot+(out[0]==HIGH)+comma; 	//up1 DIRS=HIGH
	  s += pars[MQTTDOWN1]->getStrJsonName()+twodot+(out[1]==HIGH)+comma;    //down1  DIRS=LOW
  }
  if(roll[1] == true){
	  s += pars[MQTTUP2]->getStrJsonName()+twodot+(outLogic[ENABLES+STATUSDIM] && (outLogic[DIRS+STATUSDIM]==LOW))+comma; 	//up1 DIRS=HIGH
	  s += pars[MQTTDOWN2]->getStrJsonName()+twodot+(outLogic[ENABLES+STATUSDIM] && (outLogic[DIRS+STATUSDIM]==HIGH))+comma;    //down1  DIRS=LOW
	  s+= "pr2"+twodot+String(percfdbck(1))+comma;		//pr2
	  s+= "tr2"+twodot+String(getCronoCount(1))+comma;			//tr2
	  if(blocked[1]>0){
		  s+= "blk2"+twodot+blocked[1]+comma;		//blk2
	  }
	  s+= "pr2"+twodot+String(percfdbck(1))+comma;		//pr2
	  s+= "tr2"+twodot+String(getCronoCount(1))+comma;			//tr2
  }else{
	  s += pars[MQTTUP2]->getStrJsonName()+twodot+(out[2]==HIGH)+comma; 	//up1 DIRS=HIGH
	  s += pars[MQTTDOWN2]->getStrJsonName()+twodot+(out[3]==HIGH)+comma;    //down1  DIRS=LOW
  }
  s+= "sp1"+twodot+String((long)getTapThalt(0))+comma;		//sp1
  s+= "sp2"+twodot+String((long)getTapThalt(1));		//sp2
  if(all){
	    s += comma;
		s += pars[MQTTTEMP]->getStrJsonName()+twodot+String(asyncBuf[GTTEMP])+comma;
#if (AUTOCAL_HLW8012)
		s += pars[INSTPWR]->getStrJsonName()+twodot+asyncBuf[GTIPWR]+comma;
		//str += pars[INSTPWR]->getStrJsonName()+twodot+hlw8012.getExtimActivePower()+comma;
		s += pars[INSTACV]->getStrJsonName()+twodot+asyncBuf[GTIVAC];
#else
		s += pars[MQTTMEANPWR]->getStrJsonName()+opensqr+String(asyncBuf[GTMEANPWR1])+comma+String(asyncBuf[GTMEANPWR2])+closesqr2;
		s += pars[MQTTPEAKPWR]->getStrJsonName()+opensqr+String(asyncBuf[GTPEAKPWR1])+comma+String(asyncBuf[GTPEAKPWR2])+"\"]";
#endif
  }else{
		s+=end;
  }
  publishStr2(s);
  //printMcpRealOut();
}

inline uint8_t percfdbck(uint8_t n){
	//DEBUG2_PRINT(F("Posdelta:"));
	//DEBUG2_PRINTLN(getPosdelta());
	if(getDelayedCmd(n) <= 100){
		return round(calcLen(n) - getPosdelta());  
	}else{
		return round(calcLen(n));  
	}
}

void readActModeAndPub(uint8_t n){
  DEBUG2_PRINTLN(F("\nreadActionsAndPub")); 
  String s=openbrk;
  //char sd[300];
  s+="actmode"+String(n+1)+twodot+pars[p(SWACTION1+n)]->getStrVal()+end; 
  //sprintf(sd,"%s%s%s%f%s",openbrk,mqttJson[MQTTJSONTEMP].c_str(),twodot,asyncBuf[GTTEMP],closebrk);
  //s=String(sd);
  publishStr(s);
}

void readModeAndPub(uint8_t n){
  //DEBUG2_PRINTLN(F("\nreadTempAndPub")); 
  String s=openbrk;
  //char sd[300];
  s+="mode"+String(n+1)+twodot+pars[p(SWROLL1+n)]->getStrVal()+end;
  //sprintf(sd,"%s%s%s%f%s",openbrk,mqttJson[MQTTJSONTEMP].c_str(),twodot,asyncBuf[GTTEMP],closebrk);
  //s=String(sd);
  publishStr(s);
}

void readPwrCalAndPub(){
  //DEBUG2_PRINTLN(F("\nreadPwrCalAndPub")); 
  String s=openbrk;
  s+=pars[p(PWRMULT)]->getStrJsonName()+twodot+pars[p(PWRMULT)]->getStrVal()+end;
  publishStr(s);
}

void readAvgPowerAndPub(){
  //DEBUG2_PRINTLN(F("\nreadPowerAndPub")); 
  String s=openbrk;
  s+=pars[MQTTMEANPWR]->getStrJsonName()+opensqr+String(asyncBuf[GTMEANPWR1])+comma+String(asyncBuf[GTMEANPWR2])+closesqr4;
  publishStr(s);
}

void readPeakPowerAndPub(){
  //DEBUG2_PRINTLN(F("\nreadPowerAndPub")); 
  String s=openbrk;
  s+=pars[MQTTPEAKPWR]->getStrJsonName()+opensqr+String(asyncBuf[GTPEAKPWR1])+comma+String(asyncBuf[GTPEAKPWR2])+closesqr4;
  publishStr(s);
}

void readParamAndPub(uint8_t parid, char* str){
  //DEBUG2_PRINTLN(F("\nreadTempAndPub")); 
  String s=openbrk;
  //char sd[300];
  s+=pars[parid]->getStrJsonName()+twodot+String(str)+end;
//sprintf(sd,"%s%s%s%f%s",openbrk,mqttJson[MQTTJSONTEMP].c_str(),twodot,asyncBuf[GTTEMP],closebrk);
  //s=String(sd);
  //DEBUG2_PRINTLN(s);
  publishStr(s);
}

void readTempAndPub(){
  //DEBUG2_PRINTLN(F("\nreadTempAndPub")); 
  String s=openbrk;
  //char sd[300];
  s+=pars[MQTTTEMP]->getStrJsonName()+twodot+String(asyncBuf[GTTEMP])+end;
//sprintf(sd,"%s%s%s%f%s",openbrk,mqttJson[MQTTJSONTEMP].c_str(),twodot,asyncBuf[GTTEMP],closebrk);
  //s=String(sd);
  publishStr(s);
}

void readIpwrAndPub(){
  //DEBUG2_PRINTLN(F("\nreadTempAndPub")); 
  String s=openbrk;
  //char sd[300];
  s+=pars[INSTPWR]->getStrJsonName()+twodot+asyncBuf[GTIPWR]+end;
  publishStr2(s);
}

void readVacAndPub(){
  //DEBUG2_PRINTLN(F("\nreadTempAndPub")); 
  String s=openbrk;
  //char sd[300];
  s+=pars[INSTACV]->getStrJsonName()+twodot+asyncBuf[GTIVAC]+end;
  publishStr2(s);
}
		
void readMacAndPub(){
  //String s=openbrk2;
  //publishStr(s);
  readTempAndPub();
}

void readIpAndPub(){
  //String s=openbrk2;
  //publishStr(s);
  readTempAndPub();
}

void readTimeAndPub(){
  //String s=openbrk2;
  //publishStr(s);
  readTempAndPub();
}

void readMQTTIdAndPub(){
  //String s=openbrk2;
  //publishStr(s);
  readTempAndPub();
}

void readIacvoltAndPub(){
  //String s=openbrk2;
  //publishStr(s);
  readTempAndPub();
}

void inline readActionConfAndSet(){
	//imposta le configurazioni dinamiche in base ad eventi esterni
	DEBUG2_PRINTLN(F("readActionConfAndSet."));
	eval( (static_cast<ParVarStr*>(pars[p(ACTIONEVAL)])->getStrVal()).c_str() );
}

void publishStr(String &str){
  //pubblica sul broker la stringa JSON
  //informazioni mittente
  str += ",\"";
  str += pars[MQTTTIME]->getStrJsonName()+twodot+String(millis())+comma; 
  //str += pars[MQTTMAC]->getStrJsonName()+twodot+String(WiFi.macAddress())+comma;
  str += pars[MQTTIP]->getStrJsonName()+twodot+pars[p(LOCALIP)]->getStrVal()+comma;
  //str += pars[MQTTMQTTID]->getStrJsonName()+twodot+pars[p(MQTTID)]->getStrVal()+comma;
 #if (AUTOCAL_HLW8012) 
  str += pars[INSTPWR]->getStrJsonName()+twodot+asyncBuf[GTIPWR]+comma;
  //str += pars[INSTPWR]->getStrJsonName()+twodot+hlw8012.getExtimActivePower()+comma;
  str += pars[INSTACV]->getStrJsonName()+twodot+asyncBuf[GTIVAC]+comma;
 #endif
  str += pars[MQTTDATE]->getStrJsonName()+twodot+printUNIXTimeMin(gbuf)+closebrk;
  
  if(mqttClient==NULL){
	  DEBUG1_PRINTLN(F("ERROR on publishStr MQTT client is not allocated."));
  }
  else
  {
	  //str deve essere convertita in array char altrimenti la libreria introduce un carattere spurio all'inizio del messaggio
	  mqttClient->publish((const char *)static_cast<ParStr32*>(pars[p(MQTTOUTTOPIC)])->val, str.c_str(), str.length());
	  DEBUG2_PRINT(F("Published data 1: "));
	  DEBUG2_PRINTLN(str);
  }
  //if(!webSocket){
	  //DEBUG2_PRINTLN(F("ERROR on readStatesAndPub webSocket server is not allocated."));
  //}
  //else
  //{
  webSocket.broadcastTXT(str);
  //}
}

void publishStr2(String &str){
  //pubblica sul broker la stringa JSON
  //informazioni mittente
  //str += "\"";
  str += endbrk;
  
  if(mqttClient==NULL){
	  DEBUG2_PRINTLN(F("ERROR on publishStr MQTT client is not allocated."));
  }
  else
  {
	  //str deve essere convertita in array char altrimenti la libreria introduce un carattere spurio all'inizio del messaggio
	  mqttClient->publish((const char *)static_cast<ParStr32*>(pars[p(MQTTOUTTOPIC)])->val, str.c_str(), str.length());
	  DEBUG2_PRINT(F("Published data 2: "));
	  DEBUG2_PRINTLN(str);
  }
  //if(!webSocket){
	  //DEBUG2_PRINTLN(F("ERROR on readStatesAndPub webSocket server is not allocated."));
  //}
  //else
  //{
  webSocket.broadcastTXT(str);
  //}
}

void initIiming(bool first){
  //edelay[0] = static_cast<ParLong*>(pars[p(STDEL1)])->val; 
  //edelay[1] = static_cast<ParLong*>(pars[p(STDEL2)])->val;
  //roll[0] = static_cast<ParUint8*>(pars[p(SWROLL1)])->val;
  //roll[1] = static_cast<ParUint8*>(pars[p(SWROLL2)])->val;
  DEBUG2_PRINT(F("Roll1: "));
  DEBUG2_PRINTLN(roll[0]);
  DEBUG2_PRINT(F("Roll2: "));
  DEBUG2_PRINTLN(roll[1]);
  mov = false;
  initTapparellaLogic(in,out,outLogic,first);
#if (AUTOCAL)  
  resetAVGStats(0,0);
  resetAVGStats(0,1);
#endif  
}

inline void setupNTP() {
  setNtpServer(0,(const char*) static_cast<ParStr32*>(pars[p(NTPADDR1)])->val);
  setNtpServer(1,(const char*) static_cast<ParStr32*>(pars[p(NTPADDR2)])->val); 
  setSyncInterval((unsigned long) static_cast<ParLong*>(pars[p(UTCSYNC)])->val);
  setSDT((uint8_t) static_cast<ParUint8*>(pars[p(UTCSDT)])->val);
  adjustTime((unsigned long) static_cast<ParInt*>(pars[p(UTCADJ)])->val); 
  setTimeZone((int) static_cast<ParInt*>(pars[p(UTCZONE)])->val);
  sntpInit();  
}

#if (AUTOCAL)
void setValweight(float wht){
	DEBUG1_PRINT(F("setValweight: "));
	DEBUG1_PRINTLN(wht);
	weight[0] = wht;
	weight[1] = 1 - weight[0];
}
#endif

void setup(){
	Serial.begin(115200);
#if (MCP2317) 
		mcp.begin(); 
	//pinMode INPUT
	mcp.pinMode(BTN1U, INPUT);
	mcp.pinMode(BTN1D, INPUT);
	mcp.pinMode(BTN2U, INPUT);
	mcp.pinMode(BTN2D, INPUT);
	//pinMode OUTPUT
	mcp.pinMode(OUT1EU,OUTPUT);
	mcp.pinMode(OUT1DD,OUTPUT);
	mcp.pinMode(OUT2EU,OUTPUT);
	mcp.pinMode(OUT2DD,OUTPUT);
	mcp.pinMode(GREEN,OUTPUT);
	mcp.pinMode(RED,OUTPUT);
	mcp.pinMode(BLUE,OUTPUT);
	mcp.pinMode(UNUSE1,INPUT);
	mcp.pinMode(UNUSE2,INPUT);
	mcp.pinMode(UNUSE3,INPUT);
	mcp.pinMode(UNUSE4,INPUT);
	mcp.pinMode(UNUSE5,INPUT);
	mcp.digitalWrite(OUT1EU, LOW);
	mcp.digitalWrite(OUT1DD, LOW);
	mcp.digitalWrite(OUT2EU, LOW);
	mcp.digitalWrite(OUT2DD, LOW);
	mcp.digitalWrite(GREEN, LOW);
	mcp.digitalWrite(RED, LOW);
	mcp.digitalWrite(BLUE, LOW);

	mcp.pullUp(BTN1U, LOW);
	mcp.pullUp(BTN1D, LOW);
	mcp.pullUp(BTN2U, LOW);
	mcp.pullUp(BTN2D, LOW);
	
	mcp.pullUp(UNUSE1, HIGH);
	mcp.pullUp(UNUSE2, HIGH);
	mcp.pullUp(UNUSE3, HIGH);
	mcp.pullUp(UNUSE4, HIGH);
	mcp.pullUp(UNUSE5, HIGH);
	//pinMode(OUTSLED,OUTPUT);
	//digitalWrite(OUTSLED, LOW);
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
	pinMode(OUT1EU,OUTPUT);
	pinMode(OUT1DD,OUTPUT);
	pinMode(OUT2EU,OUTPUT);
	pinMode(OUT2DD,OUTPUT);
	digitalWrite(OUT1EU, LOW);
	digitalWrite(OUT1DD, LOW);
	digitalWrite(OUT2EU, LOW);
	digitalWrite(OUT2DD, LOW);
	outPorts[0] = OUT1EU;
	outPorts[1] = OUT1DD;
	outPorts[2] = OUT2EU;
	outPorts[3] = OUT2DD;
#endif
  dbg1 = new SerialLog(1);
  dbg2 = new SerialLog(2);
  mqttClient = NULL;
  //dbg1 = new SerialLog(1);
  //dbg2 = new SerialLog(2);
  //delay(5000);
  DEBUG2_PRINTLN(F("Inizializzo i parametri."));
  //initOfst();
  dosmpl = false;
  zeroCnt = 0;
  mqttcnt = 0;
  mqttofst = 2;
  //inizializza la seriale
  //importante per il _DEBUG del WIFI!
  //Serial.setDebugOutput(true);
  //WiFi.printDiag(Serial);
  //carica la configurazione dalla EEPROM
  //DEBUG2_PRINTLN(F("Carico configurazione."));
  //for(int i=0;i<CONFDIM;i++)
  delay(7000);
  DEBUG2_PRINTLN(F("initCommon."));
  initCommon(&server,pars);
 #if (AUTOCAL_HLW8012) //////
  HLW8012_init();		////////
#endif 
  initOfst();
  DEBUG2_PRINTLN(F("loadConfig."));
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
  ESP.eraseConfig();
  WiFi.persistent(false);
  //ESP.eraseConfig();
  //WiFi.softAPdisconnect(true);
  //ESP.eraseConfig();
  //inizializza l'AP wifi
  //setup_AP(true);
  wifiState = WIFISTA;
  //WiFi.mode(WIFI_STA);
  //WiFi.mode(WIFI_OFF); 
  initdfn(LOW, 0);  //pull DOWN init (in realt� � un pull up, c'è un not in ogni ingresso sui pulsanti)
  initdfn(LOW, 1);
  initdfn(LOW, 2);
  initdfn(LOW, 3);
  
  startCnt(0, 1, CONDCNT1);
  startCnt(0, 1, CONDCNT2);
  startCnt(0, 1, CONDCNT3);
  startCnt(0, 1, CONDCNT4);
  //Timing init
  initIiming(true);
  setSWMode(static_cast<ParUint8*>(pars[p(SWROLL1)])->val,0);
  setSWMode(static_cast<ParUint8*>(pars[p(SWROLL2)])->val,1);
  
  setup_wifi(wifindx); 
  setupNTP();
  //setTimerState(wfs, CONNSTATSW);
#if(LARGEFW)
  telnet.begin((const char *) static_cast<ParStr32*>(pars[p(LOCALIP)])->val); // Initiaze the telnet server
  telnet.setResetCmdEnabled(true); // Enable the reset command
  telnet.setCallBackProjectCmds(&processCmdRemoteDebug);
  DEBUG2_PRINTLN(F("Activated remote _DEBUG"));
#endif  
  DEBUG2_PRINTLN(F("Inizializzo i pulsanti."));
  
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
  //DEBUG1_PRINTLN(F("bho3."));
  //imposta la DIRSezione delle porte dei led, imposta inizialmente i led come spento  
  
  //------------------------------------------OTA SETUP---------------------------------------------------------------------------------------
  //------------------------------------------END OTA SETUP---------------------------------------------------------------------------------------
  delay(500);
  //DEBUG1_PRINTLN(F("bho4."));
  for(int i=0;i<NBTN*STATUSDIM;i++)
	  outLogic[i]=LOW;
  for(int i=0;i<MQTTDIM;i++)
	  static_cast<ParUint8*>(pars[i])->load(LOW);
  for(int i=0;i<4;i++)
	  acts[i]=LOW;
  
  // Register event handlers.
  // Callback functions will be called as long as these handler objects exist.
  // Call "onStationConnected" each time a station connects
  stationConnectedHandler = WiFi.onSoftAPModeStationConnected(&onStationConnected);
  // Call "onStationDisconnected" each time a station disconnects
  stationDisconnectedHandler = WiFi.onSoftAPModeStationDisconnected(&onStationDisconnected);
  	
#if defined (_DEBUG) || defined (_DEBUGR)  
  testFlash();
#endif
#if (AUTOCAL_ACS712)  
  zeroDetect();
#endif
  cont=0;
  while (WiFi.status() != WL_CONNECTED && cont<3000/500) {
     delay(500);
	 cont++;
     Serial.print(".");
  }
  delay(3000);
  if(WiFi.status() != WL_CONNECTED){
	wifindx++;
	setup_wifi(wifindx);
	cont = 0;
	while (WiFi.status() != WL_CONNECTED && cont<30000/500) {
		delay(500);
		cont++;
		Serial.print(".");
	}
  }
  
  delay(1000);
  DEBUG1_PRINT(":");
  DEBUG1_PRINT(500*cont);
  if(cont==300000/500){
	DEBUG1_PRINT("\nStation not connected!");  
  }else{
	DEBUG1_PRINT("\nStation connected, IP: ");
	DEBUG1_PRINTLN((WiFi.localIP()).toString());
  }
  
  swcount = 0;
  DEBUG1_PRINTLN(F(" OK"));
  DEBUG1_PRINTLN(F("Last reset reason: "));
  DEBUG1_PRINTLN(ESP.getResetReason());
  
  if(WiFi.status() == WL_CONNECTED){
	  IPAddress ip = WiFi.localIP();
	  ip.toString().toCharArray(IP, 16);
	  pars[p(LOCALIP)]->load(IP);
	  DEBUG1_PRINTLN(F("AP client IP address = "));
	  DEBUG1_PRINTLN(((ParStr32*)pars[p(LOCALIP)])->val);
	  //MQTT INIT
	  mqttReconnect();
	  wifiConn = true;	
  }else{
	  wifiConn = false;
  }
  boot = false;
  
  DEBUG2_PRINTLN(F("sampleCurrTime()"));
  sampleCurrTime();
  
  pinger[0].SetPacketsId(0);
  pinger[1].SetPacketsId(1);
  pinger[2].SetPacketsId(2);
  pinger[3].SetPacketsId(3);
  
  pinger[0].OnReceive([](const PingerResponse& response)
  {
    if (response.ReceivedResponse)
    {
      Serial.printf(
        "Reply from %s: bytes=%d time=%lums TTL=%d\n",
        response.DestIPAddress.toString().c_str(),
        response.EchoMessageSize - sizeof(struct icmp_echo_hdr),
        response.ResponseTime,
        response.TimeToLive);
		
		if(lastPing[0] < nping[0]){
			lastPing[0]++;
			pingPer2[0] = 1;
		}else{
			pingPer2[0] = pingPer[0];
		}
		DEBUG2_PRINT(F(" ok-pingPer2[0]: "));
		DEBUG2_PRINT(pingPer2[0]);
		DEBUG2_PRINT(F(" ok-lastPing[0]: "));
		DEBUG2_PRINT(lastPing[0]);
		DEBUG2_PRINT(F(" ok-nping[0]: "));
		DEBUG2_PRINTLN(nping[0]);
    }
    else
    {
	  if(lastPing[0] > 0){
			lastPing[0]--;
			pingPer2[0] = 1;
		}else{
			pingPer2[0] = pingPer[0];
		}
		DEBUG2_PRINT(F(" ko-pingPer2[0]: "));
		DEBUG2_PRINTLN(pingPer2[0]);
		DEBUG2_PRINT(F(" ko-lastPing[0]: "));
		DEBUG2_PRINTLN(lastPing[0]);
		DEBUG2_PRINT(F(" ko-nping[0]: "));
		DEBUG2_PRINTLN(nping[0]);
	  //toPing[id] = true;
      Serial.printf("Request ping 0 timed out.\n");
    }
	DEBUG2_PRINT(F(" n: "));
	DEBUG2_PRINTLN(0);
    // Return true to continue the ping sequence.
    // If current event returns false, the ping sequence is interrupted.
    return true;
  });
  
  pinger[1].OnReceive([](const PingerResponse& response)
  {
    if (response.ReceivedResponse)
    {
      Serial.printf(
        "Reply from %s: bytes=%d time=%lums TTL=%d\n",
        response.DestIPAddress.toString().c_str(),
        response.EchoMessageSize - sizeof(struct icmp_echo_hdr),
        response.ResponseTime,
        response.TimeToLive);
		
		if(lastPing[1] < nping[1]){
			lastPing[1]++;
			pingPer2[1] = 1;
		}else{
			pingPer2[1] = pingPer[1];
		}
    }
    else
    {
	  if(lastPing[1] > 0){
			lastPing[1]--;
			pingPer2[1] = 1;
		}else{
			pingPer2[1] = pingPer[1];
		}
	  //toPing[id] = true;
      Serial.printf("Request ping 2 timed out.\n");
    }
	DEBUG2_PRINT(F(" n: "));
    DEBUG2_PRINTLN(1);
	
    // Return true to continue the ping sequence.
    // If current event returns false, the ping sequence is interrupted.
    return true;
  });
  
  pinger[2].OnReceive([](const PingerResponse& response)
  {
   if (response.ReceivedResponse)
    {
      Serial.printf(
        "Reply from %s: bytes=%d time=%lums TTL=%d\n",
        response.DestIPAddress.toString().c_str(),
        response.EchoMessageSize - sizeof(struct icmp_echo_hdr),
        response.ResponseTime,
        response.TimeToLive);
		
		if(lastPing[2] < nping[2]){
			lastPing[2]++;
			pingPer2[2] = 1;
		}else{
			pingPer2[2] = pingPer[2];
		}
    }
    else
    {
	  if(lastPing[2] > 0){
			lastPing[2]--;
			pingPer2[2] = 1;
		}else{
			pingPer2[2] = pingPer[2];
		}
	  //toPing[id] = true;
      Serial.printf("Request ping 2 timed out.\n");
    }
	DEBUG2_PRINT(F(" n: "));
	DEBUG2_PRINTLN(2);
    // Return true to continue the ping sequence.
    // If current event returns false, the ping sequence is interrupted.
    return true;
  });

  pinger[3].OnReceive([](const PingerResponse& response)
  {
    if (response.ReceivedResponse)
    {
      Serial.printf(
        "Reply from %s: bytes=%d time=%lums TTL=%d\n",
        response.DestIPAddress.toString().c_str(),
        response.EchoMessageSize - sizeof(struct icmp_echo_hdr),
        response.ResponseTime,
        response.TimeToLive);
		
		if(lastPing[3] < nping[3]){
			lastPing[3]++;
			pingPer2[3] = 1;
		}else{
			pingPer2[3] = pingPer[3];
		}
    }
    else
    {
	  if(lastPing[3] > 0){
			lastPing[3]--;
			pingPer2[3] = 1;
		}else{
			pingPer2[3] = pingPer[3];
		}
	  //toPing[id] = true;
      Serial.printf("Request ping 3 timed out.\n");
    }
	DEBUG2_PRINT(F(" n: "));
	DEBUG2_PRINTLN(3);
    // Return true to continue the ping sequence.
    // If current event returns false, the ping sequence is interrupted.
    return true;
  });
  //Turn off WiFi
  //WiFi.mode(WIFI_OFF);    //This also w
  //initTemp();
}

void httpSetup(){
  DEBUG2_PRINTLN(F("Registro handleRootExt."));
  server.on("/", HTTP_GET, handleRoot);        // Call the 'handleRoot' function when a client requests URI "/"
  DEBUG2_PRINTLN(F("Registro handleLoginExt."));
  server.on("/login", HTTP_POST, handleLogin); // Call the 'handleLogin' function when a POST request is made to URI "/login"
  DEBUG2_PRINTLN(F("Registro handleModifyExt."));
  server.on("/modify", HTTP_POST, handleModify);
  server.on("/systconf", HTTP_POST, handleSystemConf);
  server.on("/wificonf", HTTP_POST, handleWifiConf);
  server.on("/mqttconf", HTTP_POST, handleMQTTConf);
  server.on("/logicconf", HTTP_POST, handleLogicConf);
  server.on("/logconf", HTTP_POST, handleLogConf);
  server.on("/eventconf", HTTP_POST, handleEventConf);
  server.on("/cmd", HTTP_GET, handleCmd);
  server.on("/mqttcmd", HTTP_GET, handleMqttCmd);
  //server.on("/cmdjson", handleCmdJsonExt);
  //DEBUG2_PRINTLN(F("Registro handleNotFoundExt."));
  server.onNotFound(handleNotFound);           // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"
  //avvia il server HTTP*/
  DEBUG2_PRINTLN("Inizio avvio il server web.");
  //here the list of headers to be recorded
  const char * headerkeys[] = {"User-Agent","Cookie"} ;
  size_t headerkeyssize = sizeof(headerkeys)/sizeof(char*);
  //ask server to track these headers
  server.collectHeaders(headerkeys, headerkeyssize );
  //OTA web page handler linkage
  httpUpdater.setup(&server);
  //start HTTP server
  server.begin();
  DEBUG2_PRINTLN("HTTP server started");
#if(LARGEFW)
  DEBUG2_PRINTLN("Avvio il responder mDNS.");
  delay(100);
  //setup_mDNS();
  //MDNS.addService(F("http"), F("tcp"), 80); 
  DEBUG2_PRINT("HTTPUpdateServer ready! Open http://");
  DEBUG2_PRINT(pars[p(MQTTID)]->getStrVal()); 
  DEBUG2_PRINTLN(".local/update in your browser");
#endif
}

#if (AUTOCAL_ACS712) 
void zeroDetect(){
	for(int i = 0, m = 0; i < 2000; i++) {
		m = (float) m + analogRead(A0);
		delay(2);
	}
	m /= 2000;
	smplcnt = 0;
	minx = 1024;
	maxx = 0;
	smplcnt2 = 0;
}
#endif

void startPageLoad(){
	pageLoad = true;
}

void stopPageLoad(){
	pageLoad = false;
}

void leggiTastiLocali2(){
#if (MCP2317) 
	char s[18];
	uint8_t regA = mcp.readInputs();
	uint8_t inmask = regA & 0xF;	//00001111 (15)
	uint8_t inmask2;
	
	//uint8_t inmask = regA & 0xF0;	//00001111 (15)
	if(inmask != 15){ //pullup!
	//if(inmask != 240){ //pullup!
		inmask2 = 15;
		DEBUG1_PRINT(F("Inmask: "));
		DEBUG1_PRINTLN(inmask2);
		sprintf(s,"MASK  : "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(inmask2));
		DEBUG1_PRINT(s);
		sprintf(s,"GPIOIN: "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(regA));
		DEBUG1_PRINT(s);
		if(!bitRead(regA, BTN1U)){
			static_cast<ParUint8*>(pars[MQTTUP1])->load((uint8_t)255); 
			static_cast<ParUint8*>(pars[MQTTUP1])->doaction();
		}else if(!bitRead(regA, BTN1D)){
			static_cast<ParUint8*>(pars[MQTTDOWN1])->load((uint8_t)255); 
			static_cast<ParUint8*>(pars[MQTTDOWN1])->doaction();
		}else if(!bitRead(regA, BTN2U)){
			static_cast<ParUint8*>(pars[MQTTUP2])->load((uint8_t)255); 
			static_cast<ParUint8*>(pars[MQTTUP2])->doaction();
		}else if(!bitRead(regA, BTN2D)){
			static_cast<ParUint8*>(pars[MQTTDOWN2])->load((uint8_t)255); 
			static_cast<ParUint8*>(pars[MQTTDOWN2])->doaction();
		}
		//printMcpRealOut();
	}else if(inflag){
		DEBUG2_PRINTLN(F("Fronte di discesa "));
		inflag = false;
		initdfnUL(LOW,4);
		resetTimer(RESETTIMER);
		//rilascio interblocco gruppo 1
		if(roll[0] == true){
			resetOutlogic(0);
		}
		//rilascio interblocco gruppo 2
		if(roll[1] == true){
			resetOutlogic(1);
		}
	}
#else	
	uint16_t gpioread = (uint16_t)GPI;
	uint16_t inmask = gpioread & 0x4601; //01000110 00000001 (17921)
	uint16_t inmask2;									 //10111001 1
	char s[26];
	
	if(inmask != 17921){ //pullup!
		inmask2 = 17921;
		DEBUG1_PRINT(F("Inmask: "));
		DEBUG1_PRINTLN(inmask2);
		sprintf(s,"MASK  : "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(inmask2>>8), BYTE_TO_BINARY(inmask2));
		DEBUG1_PRINT(s);
		sprintf(s,"GPIOIN: "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(inmask>>8), BYTE_TO_BINARY(inmask));
		DEBUG1_PRINT(s);
		if(!GPIP(BTN1U)){
			static_cast<ParUint8*>(pars[MQTTUP1])->load((uint8_t)255); 
			static_cast<ParUint8*>(pars[MQTTUP1])->doaction();
		}else if(!GPIP(BTN1D)){
			static_cast<ParUint8*>(pars[MQTTDOWN1])->load((uint8_t)255); 
			static_cast<ParUint8*>(pars[MQTTDOWN1])->doaction();
		}else if(!GPIP(BTN2U)){
			static_cast<ParUint8*>(pars[MQTTUP2])->load((uint8_t)255); 
			static_cast<ParUint8*>(pars[MQTTUP2])->doaction();
		}else if(!GPIP(BTN2D)){
			static_cast<ParUint8*>(pars[MQTTDOWN2])->load((uint8_t)255); 
			static_cast<ParUint8*>(pars[MQTTDOWN2])->doaction();
		}
	}else if(inflag){
		DEBUG2_PRINTLN(F("Fronte di discesa "));
		inflag = false;
		initdfnUL(LOW,4);
		resetTimer(RESETTIMER);
		//rilascio interblocco gruppo 1
		if(roll[0] == true){
			resetOutlogic(0);
		}
		//rilascio interblocco gruppo 2
		if(roll[1] == true){
			resetOutlogic(1);
		}
	}
#endif	
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
  //- vie di mezzo di complessit� da eseguire con tempi intermedi
  //- evitare il più possibile la contemporaneit� di azioni pesanti 
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
#if (AUTOCAL_ACS712) 
	if(dosmpl){//solo a motore in moto
		currentPeakDetector();
	}
#endif
	//---------------------------------------------------------------------
	// 10-20 msec scheduler
	//---------------------------------------------------------------------
	if(!(step % STOP_STEP)){
		automaticStopManager();
	}//END 20ms scheduler--------------------------------------------------
#endif

#if (MCP2317) 
	if(!(step % LED_STEP)){		
		 //printOut();
	}//END LED_STEP scheduler--------------------------------------------------
#endif

	//---------------------------------------------------------------------
	// 1 sec scheduler
	//---------------------------------------------------------------------
	if(!(step % ONESEC_STEP)){
		updateCounters();
		
		//sempre vero se si è in modalit� switch!	
		if(!mov){//solo a motore fermo! Per evitare contemporaneità col currentPeakDetector
			aggiornaTimer(RESETTIMER);
			aggiornaTimer(APOFFTIMER);
			pushCnt++;
			//DEBUG2_PRINT(F("\n------------------------------------------"));
			//DEBUG2_PRINT(F("\nMean sensor: "));
			//DEBUG2_PRINT(m);
			//DEBUG2_PRINT(F(" - Conn stat: "));
			stat = WiFi.status();
			//DEBUG2_PRINT(stat);
			wifiConn = (stat == WL_CONNECTED);	
			//DEBUG2_PRINT(F(" - Wifi mode: "));
			//DEBUG2_PRINTLN(WiFi.getMode());
			//sensorStatePoll();
			if(wificnt > WIFISTEP){
				wifiFailoverManager();
			}
			MQTTReconnectManager();
			pwrSampler();
		}
		if(sampleCurrTime()){//1min
			readParamAndPub(MQTTDATE,printUNIXTimeMin(gbuf));
			readStatesAndPub();
		}
		//leggiTastiLocaliDaExp();
		sensorStatePoll();
	}//END 1 sec scheduler-----------------------------------------------------
	
	//---------------------------------------------------------------------
	// 50-60 msec scheduler
	//---------------------------------------------------------------------
	if(!(step % MAINPROCSTEP)){	
		//leggi ingressi locali e mette il loro valore sull'array val[]
		leggiTastiLocali2();
		//leggiTastiLocaliRemoto();
		//se uno dei tasti delle tapparelle � stato premuto
		//o se � arrivato un comando dalla mqttCallback
		//provenienti dalla mqttCallback
		//remote pressed button event
		//leggiTastiRemoti();
		//------------------------------------------------------------------------------------------------------------
		//Finestra idle di riconnessione (necessaria se il loop � molto denso di eventi e il wifi non si aggancia!!!)
		//------------------------------------------------------------------------------------------------------------
		//sostituisce la bloccante WiFi.waitForConnectResult();	
		if(wifiConn == false && !mov && pageLoad == false){ //sempre vero se si è in modalità switch!	
//#if (AUTOCAL_ACS712) 
			if(wificnt > WIFISTEP){
				wificnt = 0;
				DEBUG1_PRINTLN(F("\nGiving time to ESP stack... "));
				delay(30);//give 30ms to the ESP stack for wifi connect
	//#endif  
			}else{
				wificnt++;
			}
			wifiConn = (WiFi.status() == WL_CONNECTED);
		}
		//------------------------------------------------------------------------------------------------------------
	}//END 50-60ms scheduler------------------------------------------------------------------------------------
  }//END Time base (2-20 msec) main scheduler------------------------------------------------------------------------  
  //POST SCHEDULERS ACTIONS-----------------
#if(LARGEFW)
  if(dbg1->isTelnet() || dbg2->isTelnet()){
	telnet.handle();
  }
#endif  
  yield();	// Give a time for ESP8266
}//END loop

inline void updateCounters(){
	 incCnt(CNTIME1);
	 incCnt(CNTIME2);
	 incCnt(CNTIME3);
	 incCnt(CNTIME4);
	 //incCnt(TIMECNT);
}

//legge PERIODICAMENTE il parser delle condizioni sui sensori
inline void leggiTastiLocaliDaExp(){
	int app;
	//imposta le configurazioni dinamiche in base ad eventi locali valutati periodicamente
	DEBUG1_PRINT(F("Periodic local cmds: "));	
	DEBUG1_PRINTLN( eval( (static_cast<ParVarStr*>(pars[p(ONCOND5)])->getStrVal()).c_str() ) );
	
	if(roll[0] == false){
		bool pub = false;
		//modalità switch generico
		if(incAndtestUpCntEvnt(0,true,SMPLCNT1)){
			app = eval( (static_cast<ParVarStr*>(pars[p(ONCOND1)])->getStrVal()).c_str() );
			if(app != -1){
				if(app){
					if(incAndtestUpCntEvnt(0, false, CONDCNT1)){
						pub = setActionLogic(app, 0);
						//legge lo stato finale e lo scrive sulle uscite
						scriviOutDaStato(0);
					}
				}else{
					startCnt(0, 1, CONDCNT1);
					pub = setActionLogic(app, 0);
					//legge lo stato finale e lo scrive sulle uscite
					scriviOutDaStato(1);
				}
				readStatesAndPub();
			}
		}
		if(incAndtestUpCntEvnt(0,true,SMPLCNT2)){
			app = eval( (static_cast<ParVarStr*>(pars[p(ONCOND2)])->getStrVal()).c_str() );
			if(app != -1){				
				if(app){
					if(incAndtestUpCntEvnt(0, false, CONDCNT2)){
						pub = setActionLogic(app, 1);
						//legge lo stato finale e lo scrive sulle uscite
						scriviOutDaStato(2);
					}
				}else{
					startCnt(0, 1, CONDCNT2);
					pub = setActionLogic(app, 1);
					//legge lo stato finale e lo scrive sulle uscite
					scriviOutDaStato(3);
				}
				readStatesAndPub();
			}
		}
		//legge lo stato finale e lo pubblica su MQTT
		//readStatesAndPub();
	}else{
		//modalit� tapparella
		//simula pressione di un tasto locale
		app = eval( (static_cast<ParVarStr*>(pars[p(ONCOND1)])->getStrVal()).c_str() );
		DEBUG2_PRINT(F("MQTTUP1: "));	
		DEBUG2_PRINTLN(app);
		if(app > 0 && !isOnTarget(app, 0) && !isrun[0] && !isCalibr()){
			static_cast<ParUint8*>(pars[MQTTUP1])->load((uint8_t) 255);			
			static_cast<ParUint8*>(pars[MQTTUP1])->doaction(0);
		}
		app = eval( (static_cast<ParVarStr*>(pars[p(ONCOND2)])->getStrVal()).c_str() );
		DEBUG2_PRINT(F("MQDOWN1: "));	
		DEBUG2_PRINTLN(app);
		if(app > 0 && !isOnTarget(app, 0) && !isrun[0] && !isCalibr()){
			static_cast<ParUint8*>(pars[MQTTDOWN1])->load((uint8_t) 255);			
			static_cast<ParUint8*>(pars[MQTTDOWN1])->doaction(0);
		}
	}
	if(roll[1] == false){
		bool pub = false;
		//modalità switch generico
		if(incAndtestUpCntEvnt(0,true,SMPLCNT3)){
			app = eval( (static_cast<ParVarStr*>(pars[p(ONCOND3)])->getStrVal()).c_str() );
			DEBUG2_PRINT(F("MQTTUP2: "));	
			DEBUG2_PRINTLN(app);
			if(app != -1){
				if(app){
					if(incAndtestUpCntEvnt(0, false, CONDCNT3)){
						pub = setActionLogic(app, 2);
						//legge lo stato finale e lo scrive sulle uscite
						scriviOutDaStato(0);
					}
				}else{
					startCnt(0, 1, CONDCNT3);
					pub = setActionLogic(app, 2);
					//legge lo stato finale e lo scrive sulle uscite
					scriviOutDaStato(0);
				}
				readStatesAndPub();
			}
		}
		if(incAndtestUpCntEvnt(0,true,SMPLCNT4)){
			app = eval( (static_cast<ParVarStr*>(pars[p(ONCOND4)])->getStrVal()).c_str() );
			DEBUG2_PRINT(F("MQTTDOWN2: "));	
			DEBUG2_PRINTLN(app);
			if(app != -1){				
				if(app){
					if(incAndtestUpCntEvnt(0, false, CONDCNT4)){
						pub = setActionLogic(app, 3);
						//legge lo stato finale e lo scrive sulle uscite
						scriviOutDaStato(1);
					}
				}else{
					startCnt(0, 1, CONDCNT4);
					pub = setActionLogic(app, 3);
					//legge lo stato finale e lo scrive sulle uscite
					scriviOutDaStato(1);
				}
				readStatesAndPub();
			}
		}
		//legge lo stato finale e lo pubblica su MQTT
		//readStatesAndPub();
	}else{
		//modalità tapparella
		//simula pressione di un tasto locale
		app = eval( (static_cast<ParVarStr*>(pars[p(ONCOND3)])->getStrVal()).c_str() );
		if(app > 0 && !isOnTarget(app, 1) && !isrun[1] && !isCalibr()){
			static_cast<ParUint8*>(pars[MQTTUP2])->load((uint8_t) 255);			
			static_cast<ParUint8*>(pars[MQTTUP2])->doaction(0);
		}
		app = eval( (static_cast<ParVarStr*>(pars[p(ONCOND4)])->getStrVal()).c_str() );
		if(app > 0 && !isOnTarget(app, 1) && !isrun[1] && !isCalibr()){
			static_cast<ParUint8*>(pars[MQTTDOWN2])->load((uint8_t) 255);			
			static_cast<ParUint8*>(pars[MQTTDOWN2])->doaction(0);
		}
	}
}

inline void sensorStatePoll(){
	//sensor variation polling management
	//on events basis push of reports
	
#if (AUTOCAL_HLW8012)
 	if(mov){//
		gatedfn(overallSwPower,GTIPWR, IPWRRND);//only memorization!
		//gatedfn(getTemperature(),GTTEMP, TEMPRND);
		//no power feedback to grant more precision into time calculation
		//no voltage measurement to grant more precision into time calculation	
	}else{
		if(gatedfn(hlw8012.getActivePower(),GTIPWR, IPWRRND)){
			readIpwrAndPub();
		}
		if(gatedfn(hlw8012.getVoltage(),GTIVAC, IVACRND)){
			readAvgPowerAndPub();
		}
		if(gatedfn(getTemperature(),GTTEMP, TEMPRND)){
			readTempAndPub();
			DEBUG2_PRINT(F("\nTemperatura cambiata"));
		}
	}
#else
	if(mov){//
		//gatedfn(overallSwPower2,GTIPWR, IPWRRND);//only memorization!
		//gatedfn(getTemperature(),GTTEMP, TEMPRND);
		//no power feedback to grant more precision into time calculation
		//no voltage measurement to grant more precision into time calculation	
	}else{
		if(gatedfn(getTemperature(),GTTEMP, TEMPRND)){
			readTempAndPub();
			DEBUG2_PRINT(F("\nTemperatura cambiata"));
		}
	}
#endif	
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

void inline zeroAndPwrSampler(){
	//zero detection manager and scheduler
	if(doZeroSampl > 0){
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
		DEBUG2_PRINTLN(F("Zero detection manager"));	
		DEBUG2_PRINT(F("doZeroSampl: "));	
		DEBUG2_PRINT(doZeroSampl);	
		DEBUG2_PRINT(F(", dosmpl: "));	
		DEBUG2_PRINT(dosmpl);	
		DEBUG2_PRINT(F(", x: "));	
		DEBUG2_PRINT(x);	
		DEBUG2_PRINT(F(", smplcnt: "));	
		DEBUG2_PRINTLN(smplcnt);
		--doZeroSampl;
	}
	if(doPwrSampl > 0){
		dd = maxx - minx;
		DEBUG2_PRINT(F("doPwrSampl: "));	
		DEBUG2_PRINT(doPwrSampl);	
		DEBUG2_PRINT(F(", dosmpl: "));	
		DEBUG2_PRINT(dosmpl);	
		DEBUG2_PRINT(F(", dd: "));	
		DEBUG2_PRINT(dd);
		DEBUG2_PRINT(F(", m2: "));	
		DEBUG2_PRINT(m2);
		DEBUG2_PRINT(F(", smplcnt2: "));	
		DEBUG2_PRINTLN(smplcnt2);
		
		smplcnt2++;
		smplcnt2 && (m2 += (float) (dd- m2) / smplcnt2);
		minx = 1024;
		maxx = 0;
		--doPwrSampl;
		overallSwPower = m2;
		sampleOne(m2);
	}else if(doPwrSampl == 0){
		dosmpl = false;
		overallSwPower = m2;
		sampleOne(m2);
		m2 = 0;
		smplcnt2 = 0;
		--doPwrSampl;
		DEBUG2_PRINT(F(", final dd: "));	
		DEBUG2_PRINT(dd);
		DEBUG2_PRINT(F(", overallSwPower: "));	
		DEBUG2_PRINT(overallSwPower);
		DEBUG2_PRINT(F(", final smplcnt2: "));	
		DEBUG2_PRINTLN(smplcnt2);
	}
}	
#endif

inline void sampleOne(float pwr){
	if(roll[1] && roll[0] == 0){
		//if(out[0] && (out[1] + out[2] + out[3]) == 0)
		(out[0] && out[1] == 0) && (outPwr[0] = pwr);
		//if(out[1] && (out[0] + out[2] + out[3]) == 0)
		(out[1] && out[0] == 0) && (outPwr[1] = pwr);
		//(out[0] == 0 && out[1] == 0) && (overallSwPower = 0);
	}else if(roll[0] && roll[1] == 0){
		//if(out[2] && (out[0] + out[1] + out[3]) == 0)
		(out[2] && out[3] == 0) && (outPwr[2] = pwr);
		//if(out[3] && (out[0] + out[1] + out[2]) == 0) 
		(out[3] && out[2] == 0) && (outPwr[3] = pwr);
		//(out[2] == 0 && out[3] == 0) && (overallSwPower = 0);
	}
}

inline float getMotorPower(){
#if (AUTOCAL_ACS712) 
	//potenza netta motore
	dd = maxx - minx - overallSwPower;
	minx = 1024;
	maxx = 0;
#elif (AUTOCAL_HLW8012) 
	//potenza lorda motore + luci
	overallSwPower2 = hlw8012.getExtimActivePower();
	//potenza netta motore
	dd = overallSwPower2 - overallSwPower;
#endif
return dd;
}

inline void pwrSampler(){//only if !mov
#if (AUTOCAL_HLW8012) 
	if(out[0] + out[1] + out[2] + out[3] != 0){
		overallSwPower = hlw8012.getExtimActivePower();
	}else{
		overallSwPower = 0;
	}
	
	sampleOne(overallSwPower);
#elif (AUTOCAL_ACS712) 
	if(out[0] + out[1] + out[2] + out[3] != 0){
		dosmpl = true;
		doZeroSampl = -1;
		doPwrSampl = NPWRSMPL;
	}else{
		dosmpl = false;
		doPwrSampl = -1;
		doZeroSampl = NZEROSMPL;
		overallSwPower = 0;
	}
	DEBUG2_PRINT(F("getMotorPower: doPwrSampl: "));	
	DEBUG2_PRINT(doPwrSampl);
	DEBUG2_PRINT(F(", getMotorPower: doZeroSampl: "));	
	DEBUG2_PRINT(doZeroSampl);
	DEBUG2_PRINT(F(", dosmpl: "));	
	DEBUG2_PRINT(dosmpl);
#endif
	DEBUG2_PRINT(F(", OverallSwPower: "));	
	DEBUG2_PRINT(overallSwPower);	
	DEBUG2_PRINT(F(", pwr1: "));	
	DEBUG2_PRINT(outPwr[0]);
	DEBUG2_PRINT(F(", pwr2: "));	
	DEBUG2_PRINT(outPwr[1]);
	DEBUG2_PRINT(F(", pwr3: "));	
	DEBUG2_PRINT(outPwr[2]);
	DEBUG2_PRINT(F(", pwr4: "));	
	DEBUG2_PRINTLN(outPwr[3]);
}

void onSWStateChange(uint8_t nn){
	int n = nn / TIMERDIM;
	//int sw = nn % TIMERDIM;
	DEBUG2_PRINT(F("onSWStateChange nn: "));
	DEBUG2_PRINT(nn);
	DEBUG2_PRINT(F(", n: "));
	DEBUG2_PRINT(n);

	if(roll[n] == 0){
		if(out[nn] == HIGH){
			DEBUG2_PRINT(F(", out: "));
			DEBUG2_PRINT(out[nn]);
			//OFF --> ON
			if(mov){
				//modifica stima potenza tapparella solo se questa è in movimento
				overallSwPower += outPwr[nn]; //add previously sampled value
			}
			DEBUG2_PRINT(F(", out HIGH corrected overallSwPower: "));
			DEBUG2_PRINTLN(overallSwPower);
		}else if(mov){
			//modifica stima potenza tapparella solo se questa è in movimento
			//ON --> OFF
			overallSwPower -= outPwr[nn]; //subtract previously sampled value
			DEBUG2_PRINT(F(", mov corrected overallSwPower: "));
			DEBUG2_PRINTLN(overallSwPower);
		}
		//unsigned short p = (packetBuffer[1] << 8) | packetBuffer[2];
	}
	
}

inline void automaticStopManager(){
	if(mov){ //sempre falso se si è in modalità switch!	
			getMotorPower();
			DEBUG2_PRINT(F(" \nOverallSwPower: "));
			DEBUG2_PRINT(overallSwPower);
			//EMA calculation
			//ACSVolt = (double) ex/2.0;
			//peak = (double) ex/2.0;
			//reset of peak sample value
			DEBUG2_PRINT(" Isrun: ");
			DEBUG2_PRINT(isrun[0]);
			DEBUG2_PRINT(", dosmpl: ");
			DEBUG2_PRINT(dosmpl);
			DEBUG2_PRINT(", dd: ");
			DEBUG2_PRINTLN(dd);
			
#if (AUTOCAL_ACS712) 
			//doZeroSampl = -1;
			//doPwrSampl = -1;
			if(isrun[0] && dosmpl){
#elif (AUTOCAL_HLW8012) 
			if(isrun[0]){
#endif
				DEBUG2_PRINT(0);
				if(isrundelay[0] == 0){
					ex[0] = dd*EMA + (1.0 - EMA)*ex[0];
					DEBUG2_PRINT(F("\n("));
					DEBUG2_PRINT(0);
#if (AUTOCAL_ACS712) 
					DEBUG2_PRINT(F(") minx sensor: "));
					DEBUG2_PRINT(minx);
					DEBUG2_PRINT(F(" - maxx sensor: "));
					DEBUG2_PRINT(maxx);
					DEBUG2_PRINT(F(" - Mean sensor: "));
					DEBUG2_PRINT(m);
#endif
					DEBUG2_PRINT(F(" -rundelay: "));
					DEBUG2_PRINT(isrundelay[0]);
					DEBUG2_PRINT(F(" - EMA: "));
					DEBUG2_PRINT(ex[0]);
					DEBUG2_PRINT(F(" - Inst: "));
					DEBUG2_PRINT(dd);
					DEBUG2_PRINT(F(" - weight[1]: "));
					DEBUG2_PRINT(weight[1]);
					chk[0] = checkRange((double) ex[0]*(1 - weight[1]*isMoving(1)),0);
					//chk[0] = checkRange((double) ex[0],0);
					if(chk[0] != 0){
						DEBUG2_PRINT(F("\n("));
						DEBUG2_PRINT(0);
						if(chk[0] == -1){
							DEBUG2_PRINTLN(F(") Stop: sottosoglia"));
							//fine dorsa raggiunto
							blocked[0] = secondPress(0,halfStop,true);
							scriviOutDaStato(0);
						}else if(chk[0] == 2){
							DEBUG2_PRINTLN(F(") Stop: soprasoglia"));
							blocked[0] = secondPress(0,halfStop);
							scriviOutDaStato(0);
							blocked[0] = 1;
						}else if(chk[0] == 1){
							ex[0] = getAVG(0);
							DEBUG2_PRINTLN(F(") Start: fronte di salita"));					
							//inizio conteggio timer di posizionamento
							startEndOfRunTimer(0);
							DEBUG2_PRINT(F("Timer lapse: "));		
							DEBUG2_PRINTLN(getTimerLapse(0));
						}
						readStatesAndPub();
						//ex[0] = getAVG(0);
						yield();
					}
				}else{
					DEBUG2_PRINT(F("\n("));
					DEBUG2_PRINT(0);
					DEBUG2_PRINT(F(") aspetto: "));
					DEBUG2_PRINT(isrundelay[0]);
#if (AUTOCAL_ACS712) 
					DEBUG2_PRINT(F(" - minx sensor: "));
					DEBUG2_PRINT(minx);
					DEBUG2_PRINT(F(" - maxx sensor: "));
					DEBUG2_PRINT(maxx);
#endif
					DEBUG2_PRINT(F(" - Peak: "));
					DEBUG2_PRINT(ex[0]);
					isrundelay[0]--;
					ex[0] = dd;
					DEBUG2_PRINT(F(" - dd: "));
					DEBUG2_PRINT(dd);
				}
			}else{
				isrundelay[0] = RUNDELAY;
				//reset dei fronti su blocco marcia (sia manuale che automatica) 
				resetEdges(0);
			}
			
#if (AUTOCAL_ACS712) 
			if(isrun[1] && dosmpl){
#elif (AUTOCAL_HLW8012) 
			if(isrun[1]){
#endif
				if(isrundelay[1] == 0){
					ex[1] = dd*EMA + (1.0 - EMA)*ex[1];
					DEBUG2_PRINT(F("\n("));
					DEBUG2_PRINT(1);
#if (AUTOCAL_ACS712) 
					DEBUG2_PRINT(F(") minx sensor: "));
					DEBUG2_PRINT(minx);
					DEBUG2_PRINT(F(" - maxx sensor: "));
					DEBUG2_PRINT(maxx);
					DEBUG2_PRINT(F(" - Mean sensor: "));
					DEBUG2_PRINT(m);
#endif
					DEBUG2_PRINT(F(" -rundelay: "));
					DEBUG2_PRINT(isrundelay[1]);
					DEBUG2_PRINT(F(" - Peak: "));
					DEBUG2_PRINT(ex[1]);
					//DEBUG2_PRINT(F(" - ADC enable: "));
					//DEBUG2_PRINT(dosmpl);
					chk[1] = checkRange((double) ex[1]*(1 - weight[0]*isMoving(0)),1);
					if(chk[1] != 0){
						DEBUG2_PRINT(F("\n("));
						DEBUG2_PRINT(1);
						if(chk[1] == -1){
							DEBUG2_PRINTLN(F(") Stop: sottosoglia"));
							//fine dorsa raggiunto
							blocked[1] = secondPress(1,halfStop,true);
							scriviOutDaStato(1);
						}else if(chk[1] == 2){
							DEBUG2_PRINTLN(F(") Stop: soprasoglia"));
							blocked[1] = secondPress(1,halfStop);
							scriviOutDaStato(1);
							blocked[1] = 1;
						}else if(chk[1] == 1){
							DEBUG2_PRINTLN(F(") Start: fronte di salita"));	
							ex[1] = getAVG(1);
							//inizio conteggio timer di posizionamento
							startEndOfRunTimer(1);
							DEBUG2_PRINT(F("Timer lapse: "));		
							DEBUG2_PRINTLN(getTimerLapse(3));
						}
						readStatesAndPub();
						//ex[1] = getAVG(1);
						yield();
					}
				}else{		
					isrundelay[1]--;
					ex[1] = dd;
				}
			}else{
				//NOT is mov
				isrundelay[1] = RUNDELAY;
				//reset dei fronti su blocco marcia (sia manuale che automatica)
				resetEdges(1);
			}
			//AC peak measure init
			//indx = 0;
#if (AUTOCAL_ACS712) 
			dosmpl = true;
#endif
			//DEBUG2_PRINT(F("\n------------------------------------------------------------------------------------------"));
		}else{
			isrundelay[0] = isrundelay[1] = RUNDELAY;
			//reset dei fronti su blocco marcia (sia manuale che automatica)
			resetEdges(0);
			resetEdges(1);
			//overallSwPower = hlw8012.getExtimActivePower();
#if (AUTOCAL_ACS712) 
			zeroAndPwrSampler();
#endif
		}

}

inline void wifiFailoverManager(){
	//wifi failover management
	//DEBUG2_PRINTLN(wl_status_to_string(wfs));
	if(WiFi.getMode() == WIFI_OFF || WiFi.getMode() == WIFI_STA || WiFi.getMode() == WIFI_AP_STA){
		if(!wifiConn){
			//lampeggia led di connessione
			#if (MCP2317) 
				//mcp.digitalWrite(BLUE, !mcp.digitalRead(BLUE));
			#else
				digitalWrite(OUTSLED, !digitalRead(OUTSLED));
			#endif
			//yield();
			DEBUG2_PRINT(F("\nSwcount roll: "));
			DEBUG2_PRINTLN(swcount);
			
			if((swcount == 0)){
				DEBUG1_PRINTLN(F("Connection timed out"));
				WiFi.persistent(false);
				WiFi.disconnect(true);
				WiFi.mode(WIFI_OFF);    
				DEBUG1_PRINTLN(F("Do new connection"));
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
				//mcp.digitalWrite(BLUE, LOW);
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
			
			if((wifiConn == true)&&(!mqttConnected) && WiFi.status()==WL_CONNECTED && WiFi.getMode()==WIFI_STA) {
				mqttcnt++;
				
				DEBUG2_PRINTLN(F("ERROR! MQTT client is not connected."));
				
				if(mqttClient==NULL){
					DEBUG2_PRINTLN(F("ERROR! MQTT client is not allocated: doing reconnect..."));
					mqttReconnect();
					mqttcnt = 0;
					mqttofst = 4;
				}else{ 
					
					if(mqttcnt <= 8){
						DEBUG2_PRINTLN(F("ERROR! MQTT client is not connected: 4."));
						mqttofst = 4;
					}if(mqttcnt <= 16){
						DEBUG2_PRINTLN(F("ERROR! MQTT client is not connected: 8."));
						mqttofst = 8;
					}else if(mqttcnt <= 32){
						DEBUG2_PRINTLN(F("ERROR! MQTT client is not connected: 16."));
						mqttofst = 16;
					}else if(mqttcnt <= 128){
						DEBUG2_PRINTLN(F("ERROR! MQTT client is not connected: 32."));
						mqttofst = 32;
					}else if(mqttcnt <= 256){
						DEBUG2_PRINTLN(F("ERROR! MQTT client is not connected: 64."));
						mqttofst = 64;
					}else if(mqttcnt <= 512){
						DEBUG2_PRINTLN(F("ERROR! MQTT client is not connected: 128."));
						mqttofst = 128;
					}else{ 
						DEBUG2_PRINTLN(F("ERROR! MQTT client is not connected: 256."));
						mqttofst = 256;
					}
					
					if(!(mqttcnt % mqttofst)){
						//non si può fare perch� dopo pochi loop crasha
						if(dscnct){
							dscnct=false;
							DEBUG1_PRINT(F("eseguo la MQTT connect()...Cnt: "));
							//noInterrupts ();
							mqttClient->connect();
							//interrupts ();
							//delay(30);
						}
						else
						{
							dscnct=true;
							DEBUG1_PRINT(F("eseguo la MQTT disconnect()...Cnt: "));
							//noInterrupts ();
							mqttClient->disconnect();
							//interrupts ();
							//delay(30);
						}
						DEBUG1_PRINT(mqttcnt);
						DEBUG1_PRINT(F(" - Passo: "));
						DEBUG1_PRINTLN(mqttofst);
					}
					//non si pu� fare senza disconnect perch� dopo pochi loop crasha
					//mqttClient->setUserPwd((confcmd[MQTTUSR]).c_str(), (confcmd[MQTTPSW]).c_str());
					
				}
			}
}

//-----------------------------------------------INIZIO TIMER----------------------------------------------------------------------
//azione da compiere allo scadere di uno dei timer dell'array	
void onElapse(uint8_t nn, unsigned long tm){
	int n = nn / TIMERDIM;
	int sw = nn % TIMERDIM;
	
	DEBUG2_PRINT(F("\nElapse timer n: "));
	DEBUG2_PRINT(nn);
	DEBUG2_PRINT(F("  al tempo del timer: "));
	DEBUG2_PRINT(tm);
	DEBUG2_PRINT(F("  al tempo del cronometro: "));
	DEBUG2_PRINT(getCronoCount(n));
	DEBUG2_PRINT(F("  con stato: "));
	DEBUG2_PRINT(getGroupState(nn));
	DEBUG2_PRINT(F("  con n: "));
	DEBUG2_PRINT(n);
	DEBUG2_PRINT(F("  con sw: "));
	DEBUG2_PRINT(sw);
	DEBUG2_PRINT(F("  con count value: "));
	DEBUG2_PRINTLN(getCntValue(nn));
	DEBUG2_PRINTLN(F("-----------------"));
	DEBUG2_PRINTLN(getCntValue(0));
	DEBUG2_PRINTLN(getCntValue(1));
	DEBUG2_PRINTLN(getCntValue(2));
	DEBUG2_PRINTLN(getCntValue(3));
	DEBUG2_PRINTLN(F("-----------------"));
	
	if(nn != RESETTIMER || nn != APOFFTIMER) //se è scaduto il timer di attesa o di blocco  (0,1) --> state n
	{   
		DEBUG2_PRINT(F("\nCount value: "));
		DEBUG2_PRINTLN(getCntValue(nn));
		if(getCntValue(nn) == 1){ 
			if(roll[n]){//se è in modalit� tapparella!
				if(getGroupState(nn)==3){ //il motore e in moto cronometrato scaduto (timer di blocco scaduto)
					DEBUG2_PRINTLN(F("stato 0 roll mode: il motore va in stato fermo da fine corsa (TIMER ELAPSED!)"));
					secondPress(n);
					//comanda gli attuatori per fermare (non lo fa il loop stavolta!)
					scriviOutDaStato(n);
					//pubblica lo stato finale su MQTT (non lo fa il loop stavolta!)
					readStatesAndPub();
				}else if(getGroupState(nn)==1){	//se il motore era in attesa di partire (timer di attesa scaduto)
					DEBUG2_PRINTLN(F("onElapse roll mode:  timer di attesa scaduto"));
					startEngineDelayTimer(n);
					//adesso parte...
					scriviOutDaStato(n);
					//pubblica lo stato finale su MQTT (non lo fa il loop stavolta!)
					readStatesAndPub();
				}
	#if (!AUTOCAL)	
				else if(getGroupState(nn)==2){//se il motore è in moto a vuoto
					DEBUG2_PRINTLN(F("onElapse roll mode manual:  timer di corsa a vuoto scaduto"));
					///setGroupState(3,n);	//il motore va in moto cronometrato
					startEndOfRunTimer(n);
					//pubblica lo stato di UP o DOWN attivo su MQTT (non lo fa il loop stavolta!)
					readStatesAndPub();
				}
	#else
				else if(getGroupState(nn)==2){//se il motore è in moto a vuoto
					DEBUG2_PRINTLN(F("onElapse roll mode autocal:  timer di check pressione su fine corsa scaduto"));
					secondPress(n,halfProc,true);
					//comanda gli attuatori per fermare (non lo fa il loop stavolta!)
					scriviOutDaStato(n);//15/08/19
					//pubblica lo stato di UP o DOWN attivo su MQTT (non lo fa il loop stavolta!)
					readStatesAndPub();
				}
	#endif
			}else{//se è in modalità switch
				if(getGroupState(nn)==1){//se lo switch era inibito (timer di attesa scaduto)
					DEBUG2_PRINTLN(F("onElapse switch mode:  timer di attesa scaduto"));
					startSimpleSwitchDelayTimer(nn);
					//adesso commuta...
				}else if(getGroupState(nn)==2){ //se lo switch è monostabile (timer di eccitazione scaduto)
					DEBUG2_PRINTLN(F("stato 0 switch mode: il motore va in stato fermo da fine corsa (TIMER ELAPSED!)"));
					endPress(nn);
				}
				//comanda gli attuatori per fermare (non lo fa il loop stavolta!)
				scriviOutDaStato(n);
				//pubblica lo stato finale su MQTT (non lo fa il loop stavolta!)
				readStatesAndPub();
			}
			///////Reset count///////
			resetCnt(nn); //20/10/19 Deve essere alla fine casomai qualcuno prima lo modficasse...
		}else if(getCntValue(nn) > 1){ //in tutte le modalit�
			if(n == 0){
				DEBUG2_PRINTLN(F("onElapse:  timer 1 dei servizi a conteggio scaduto"));
				if(getCntValue(nn)==3){
					//DEBUG2_PRINT(F("Resettato contatore dei servizi: "));
					resetCnt(nn);
					wifiState = WIFIAP;
					//wifi_station_dhcpc_stop();
					//WiFi.enableAP(true);
					//wifi_softap_dhcps_start();
					DEBUG2_PRINTLN(F("-----------------------------"));
					DEBUG2_PRINTLN(F("Attivato AP mode"));
					DEBUG2_PRINTLN(F("-----------------------------"));
					startTimer(APOFFTIMER);
					//WiFi.enableSTA(false);
					DEBUG2_PRINTLN(F("AP mode on"));
					//WiFi.setAutoConnect(false);
					//WiFi.setAutoReconnect(false);	
					//ETS_UART_INTR_DISABLE();
					if(!WiFi.isConnected()){
						// disconnect sta, start ap
						WiFi.persistent(false);      
						WiFi.disconnect();  //cancella la connessione corrente memorizzata
						WiFi.mode(WIFI_AP);
						WiFi.persistent(true);
						DEBUG1_PRINTLN(F("SET AP"));
						wifiConn = false;
					}else{
						WiFi.mode(WIFI_AP_STA);
						DEBUG1_PRINTLN(F("SET AP STA"));
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
					//DEBUG2_PRINT(F("Resettato contatore dei servizi: "));
					resetCnt(nn);
					DEBUG2_PRINTLN(F("-----------------------------"));
					DEBUG1_PRINTLN(F("Rebooting ESP without reset of configuration"));
					DEBUG2_PRINTLN(F("-----------------------------"));
					ESP.eraseConfig(); //do the erasing of wifi credentials
					ESP.restart();
				}else if(getCntValue(nn)==5 && roll[n]){ //solo in modalit� tapparella!
					//DEBUG2_PRINT(F("Resettato contatore dei servizi: "));
					resetCnt(nn);
					DEBUG2_PRINTLN(F("-----------------------------"));
					DEBUG1_PRINTLN(F("ATTIVATA CALIBRAZIONE MANUALE BTN 1"));
					DEBUG2_PRINTLN(F("-----------------------------"));
					manualCalibration(0); //BTN1
				}else if(getCntValue(nn)==9){
					//DEBUG2_PRINT(F("Resettato contatore dei servizi: "));
					resetCnt(nn);
					DEBUG2_PRINTLN(F("-----------------------------"));
					DEBUG1_PRINTLN(F("Reboot ESP with reset of configuration"));
					DEBUG2_PRINTLN(F("-----------------------------"));
					rebootSystem();
				}
#if (AUTOCAL_HLW8012) 
				else if(getCntValue(nn)==11){
					//DEBUG2_PRINT(F("Resettato contatore dei servizi: "));
					resetCnt(nn);
					DEBUG2_PRINTLN(F("-----------------------------"));
					DEBUG1_PRINTLN(F("Do power calibration"));
					DEBUG2_PRINTLN(F("-----------------------------"));
					calibrate_pwr();
					saveSingleConf(PWRMULT);
					saveSingleConf(CURRMULT);
					saveSingleConf(VACMULT);
					readPwrCalAndPub();
				}
#endif				
				else{
					//DEBUG2_PRINT(F("Resettato contatore dei servizi: "));
					resetCnt(nn);
					setGroupState(0,nn);
				}
			}else if(roll[n]){ //solo in modalit� tapparella!
				DEBUG1_PRINTLN(F("onElapse:  timer 2 dei servizi a conteggio scaduto"));
				if(getCntValue(CNTSERV3)==5){
					//DEBUG2_PRINT(F("Resettato contatore dei servizi: "));
					resetCnt(nn);
					DEBUG2_PRINTLN(F("-----------------------------"));
					DEBUG1_PRINTLN(F("ATTIVATA CALIBRAZIONE MANUALE BTN 2"));
					DEBUG2_PRINTLN(F("-----------------------------"));
					manualCalibration(1); //BTN2
				}else{
					setGroupState(0,nn);
				}
			}
		}
	}else if(nn == RESETTIMER)
		{
			rebootSystem();
	}else if(nn == APOFFTIMER){
		if(WiFi.softAPgetStationNum() == 0){
			DEBUG1_PRINTLN(F("WIFI: reconnecting to AP"));
			uint8_t stat = WiFi.status();
			wifiConn = (stat == WL_CONNECTED);	
			WiFi.mode(WIFI_STA);	
			wifiState = WIFISTA;
			setup_wifi(wifindx);
			//mqttReconnect();
			//wifi_station_dhcpc_start();
			DEBUG2_PRINTLN(F("-----------------------------"));
			DEBUG1_PRINTLN(F("Nussun client si è ancora connesso, disatttivato AP mode"));
			DEBUG2_PRINTLN(F("-----------------------------"));
		}
		//setGroupState(0,n%2);				 								//stato 0: il motore va in stato fermo
		DEBUG1_PRINTLN(F("stato 0: il motore va in stato fermo da stato configurazione"));
	}
	
	//DEBUG2_PRINTLN(F("Fine timer"));
}

void onTapStop(uint8_t n){
#if (AUTOCAL)
	resetStatDelayCounter(n);
#endif
	//comanda gli attuatori per fermare (non lo fa il loop stavolta!)
	scriviOutDaStato(n);
	//pubblica lo stato di UP o DOWN attivo su MQTT (non lo fa il loop stavolta!)
	readStatesAndPub();
}
		
void onCalibrEnd(unsigned long app, uint8_t n){
	static_cast<ParLong*>(pars[p(THALT1 + n)])->load(app);
	//initTapparellaLogic(in,inr,outLogic,(confcmd[THALT1]).toInt(),(confcmd[THALT2]).toInt(),(confcmd[STDEL1]).toInt(),(confcmd[STDEL2]).toInt(),BTNDEL1,BTNDEL2);
	setTapThalt(app, n);
	DEBUG2_PRINTLN(F("-----------------------------"));
#if (AUTOCAL)
	calAvg[n] = getAVG(n);
	weight[0] = (double) calAvg[0] / (calAvg[0] +  calAvg[1]);
	weight[1] = (double) calAvg[1] / (calAvg[0] +  calAvg[1]);
	static_cast<ParFloat*>(pars[p(VALWEIGHT)])->load(weight[0]);
	updateUpThreshold(n);
	//confcmd[TRSHOLD1 + n] = String(getThresholdUp(n));
	//setThresholdUp((confcmd[TRSHOLD1 + n]).toFloat(), n);
	saveSingleConf(VALWEIGHT);
	DEBUG2_PRINT(F("Modified current weight "));
	DEBUG2_PRINTLN(static_cast<ParFloat*>(pars[p(VALWEIGHT)])->getStrVal());
#if (AUTOCAL_HLW8012) 	
	/*if(n == 0){
		calibrate_pwr(getAVG(n));
		saveSingleConf(PWRMULT);
		saveSingleConf(CURRMULT);
		saveSingleConf(VACMULT);
		readPwrCalAndPub();
	}*/
#endif
#endif
	saveSingleConf(THALT1 + n);
	DEBUG2_PRINT(F("Modified THALT "));
	DEBUG2_PRINTLN(THALT1 + n);
	DEBUG2_PRINT(F(": "));
	DEBUG2_PRINTLN(static_cast<ParLong*>(pars[p(THALT1 + n)])->getStrVal());
}

void manualCalibration(uint8_t btn){
	setGroupState(0,btn);	
	//activate the learning of the running statistics
	//setStatsLearnMode();
#if (AUTOCAL)
	//resetStatDelayCounter(btn);
	disableUpThreshold(btn);
#endif
	
    //printIn();
	//printDfn();
	//printOutlogic();
	
	//DEBUG1_PRINTLN(getCntValue(0));
	//DEBUG1_PRINTLN(getCntValue(1));
	//DEBUG1_PRINTLN(getCntValue(2));
	//DEBUG1_PRINTLN(getCntValue(3));
		
	DEBUG2_PRINTLN(F("-----------------------------"));
	DEBUG1_PRINT(F("FASE 1 CALIBRAZIONE MANUALE BTN "));
	DEBUG1_PRINTLN(btn+1);
	DEBUG2_PRINTLN(F("-----------------------------"));
	//-------------------------------------------------------------
	DEBUG2_PRINTLN(F("LA TAPPARELLA STA SCENDENDO......"));
	DEBUG1_PRINT(F("PREMERE UN PULSANTE QUALSIASI DEL GRUPPO "));
	DEBUG1_PRINTLN(btn+1);
	DEBUG2_PRINTLN(F("-----------------------------"));
	
	//set initial power balancement extimation
	setValweight(0.5);
	static_cast<ParUint8*>(pars[BTN2IN + btn*BTNDIM])->load((uint8_t) 101);			//codice comando attiva calibrazione
	static_cast<ParUint8*>(pars[BTN2IN + btn*BTNDIM])->doaction(0);
}

void rebootSystem(){
	EEPROM.begin(FIXEDPARAMSLEN);
	alterEEPROM();
	EEPROM.end();
	DEBUG1_PRINTLN(F("Resetting ESP"));
	ESP.eraseConfig(); //do the erasing of wifi credentials
	ESP.restart();
}
//----------------------------------------------------FINE TIMER----------------------------------------------------------------------
void onStationConnected(const WiFiEventSoftAPModeStationConnected& evt) {
	DEBUG1_PRINT(F("\nAP mode: Station connected: "));
	DEBUG1_PRINTLN(macToString(evt.mac));
	if(WiFi.softAPgetStationNum() == 1){
		boot = true;
		wifiState = WIFIAP;
	}
}

void onStationDisconnected(const WiFiEventSoftAPModeStationDisconnected& evt) {
	DEBUG1_PRINTLN(F("\nStation disconnected: "));
	DEBUG1_PRINTLN(macToString(evt.mac));
	if(WiFi.softAPgetStationNum() == 0){
		resetTimer(APOFFTIMER);
		DEBUG1_PRINTLN(F("WIFI: reconnecting to AP"));
		uint8_t stat = WiFi.status();
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
		DEBUG1_PRINT(F("\nRebooting ESP without reset of configuration"));
		ESP.restart();
	}else if(lastCmd == "reset"){
		//Reboot ESP with reset of configuration
		DEBUG1_PRINT(F("\nReboot ESP with reset of configuration"));
		rebootSystem();
	}else if(lastCmd == "calibrate1"){
		//ATTIVATA CALIBRAZIONE MANUALE BTN 1
		manualCalibration(0);
	}else if(lastCmd == "calibrate2"){
		//ATTIVATA CALIBRAZIONE MANUALE BTN 1
		manualCalibration(1);
	}else if(lastCmd == "apmodeon"){
		DEBUG1_PRINT(F("\nAtttivato AP mode"));
		startTimer(APOFFTIMER);
	}else if(lastCmd == "scanwifi"){
		//scansione reti wifi disponibili
		scan_wifi();
	}else if(lastCmd == "getip"){
		DEBUG1_PRINT("\nLocal IP: ");
		DEBUG1_PRINTLN((WiFi.localIP()).toString());
	}else if(lastCmd == "getmqttstat"){
		if(!(mqttClient->isConnected())){
			DEBUG1_PRINTLN(F("\nMQTT non connesso."));
		}
		else
		{
			DEBUG1_PRINTLN(F("\nMQTT connesso"));
		}
	}else if(lastCmd == "gettemp"){
		DEBUG1_PRINT(F("\nTemperature: "));
		DEBUG1_PRINT(asyncBuf[GTTEMP]);
	}else if(lastCmd == "getadczero"){
		DEBUG1_PRINT(F("\nMean sensor: "));
		DEBUG1_PRINTLN(m);
	}else if(lastCmd == "getMotorPower"){
		DEBUG1_PRINT(F("\nAvg power: ["));
		DEBUG1_PRINT(asyncBuf[GTMEANPWR1]);
		DEBUG1_PRINT(F(","));
		DEBUG1_PRINT(asyncBuf[GTMEANPWR2]);
		DEBUG1_PRINTLN(F("]"));
		DEBUG1_PRINT(F("\nPeak power: ["));
		DEBUG1_PRINT(asyncBuf[GTPEAKPWR1]);
		DEBUG1_PRINT(F(","));
		DEBUG1_PRINT(asyncBuf[GTPEAKPWR2]);
		DEBUG1_PRINTLN(F("]")); 
	}else if(lastCmd == "getmac"){
		DEBUG1_PRINT(F("\nMAC: "));
		DEBUG2_PRINTLN(WiFi.macAddress());
	}else if(lastCmd == "gettime"){
		DEBUG1_PRINT(F("\nTime (ms): "));
		DEBUG2_PRINTLN(millis());
	}else if(lastCmd == "getmqttid"){
		DEBUG1_PRINT(F("\nMQTT ID: "));
		DEBUG1_PRINTLN(pars[p(MQTTID)]->getStrVal());
	}else if(lastCmd == "testflash"){
		testFlash();
	}else{
		DEBUG1_PRINT(F("\nComandi disponibili: "));
		DEBUG1_PRINT(F("\nshowconf, reboot, reset, calibrate1, calibrate2, apmodeon, scanwifi, getip, getmqttstat, getadczero, gettemp, getMotorPower, getmac, gettime, getmqttid, testflash\n"));
	}
	//telnet.flush();
}
#endif


#if defined (_DEBUG) || defined (_DEBUGR)	
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

#if defined (_DEBUG) || defined (_DEBUGR)	
void testFlash(){
  uint32_t realSize = ESP.getFlashChipRealSize();
  uint32_t ideSize = ESP.getFlashChipSize();
  FlashMode_t ideMode = ESP.getFlashChipMode();
  char s[100];
  
  sprintf(s,"\nFlash real id:   %08X\n", ESP.getFlashChipId());
  DEBUG1_PRINT(s);
  sprintf(s,"Flash real size: %u uint8_ts\n", realSize);
  DEBUG1_PRINT(s);
  sprintf(s,"Flash ide  size: %u uint8_ts\n", ideSize);
  DEBUG1_PRINT(s);
  sprintf(s,"Flash ide speed: %u Hz\n", ESP.getFlashChipSpeed());
  DEBUG1_PRINT(s);
  sprintf(s,"Flash ide mode:  %s\n", (ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN"));
  DEBUG1_PRINT(s);

  if (ideSize != realSize) {
    DEBUG1_PRINT("\nFlash Chip configuration wrong!\n");
  } else {
    DEBUG1_PRINT("\nFlash Chip configuration ok.\n");
  }
}
#endif
//-----------------------------------------------------------------------------------------------------------------------------
//richieste da remoto di valori locali
//--------------------------------------------------------------------------------------------------
//gestione eventi MQTT sui sensori (richieste e configurazioni) all'arrivo memorizzate su confcmd[]
//--------------------------------------------------------------------------------------------------
//configurazioni provenienti da remoto
//---------------------------------------------------------------
void MQTTMAC_Evnt::doaction(uint8_t save){
	readMacAndPub();
}
void MQTTIP_Evnt::doaction(uint8_t save){
	readIpAndPub();
}
void MQTTTIME_Evnt::doaction(uint8_t save){
	readTimeAndPub();
}
void MQTTTEMP_Evnt::doaction(uint8_t save){
	readTempAndPub();
}
void MQTTMEANPWR_Evnt::doaction(uint8_t save){
	readAvgPowerAndPub();
}
void MQTTPEAKPWR_Evnt::doaction(uint8_t save){
	readPeakPowerAndPub();
}

#if (AUTOCAL_HLW8012) 
void DOPWRCAL_Evnt::doaction(uint8_t save){
	calibrate_pwr();
	saveSingleConf(PWRMULT);
	saveSingleConf(CURRMULT);
	saveSingleConf(VACMULT);
	readPwrCalAndPub();
}
void INSTPWR_Evnt::doaction(uint8_t save){
	void readIpwrAndPub();
}
void INSTACV_Evnt::doaction(uint8_t save){
	void readIacvoltAndPub();
}
void CALPWR_Evnt::doaction(uint8_t save){
	if(save==1)   
		saveSingleConf(CALPWR);
}
void PWRMULT_Evnt::doaction(uint8_t save){
	if(save==1)   
		saveSingleConf(PWRMULT);
	hlw8012.setPowerMultiplier(static_cast<ParFloat*>(pars[p(PWRMULT)])->val);
		// Show corrected factors
	DEBUG1_PRINT(F("[HLW] New power multiplier   : ")); DEBUG1_PRINTLN(hlw8012.getPowerMultiplier());
}
void CURRMULT_Evnt::doaction(uint8_t save){
	if(save==1)   
		saveSingleConf(CURRMULT);
	hlw8012.setCurrentMultiplier(static_cast<ParFloat*>(pars[p(CURRMULT)])->val);
		// Show corrected factors
	DEBUG1_PRINT(F("[HLW] New current multiplier : ")); DEBUG1_PRINTLN(hlw8012.getCurrentMultiplier());
}
void VACMULT_Evnt::doaction(uint8_t save){
	if(save==1)   
		saveSingleConf(VACMULT);
	hlw8012.setVoltageMultiplier(static_cast<ParFloat*>(pars[p(VACMULT)])->val);
		// Show corrected factors
	DEBUG1_PRINT(F("[HLW] New voltage multiplier : ")); DEBUG1_PRINTLN(hlw8012.getVoltageMultiplier());
}
void ACVOLT_Evnt::doaction(uint8_t save){
	if(save==1)   
		saveSingleConf(ACVOLT);
}
#endif
//----------------------------------------------------------------------------
//richieste da remoto di valori locali
//----------------------------------------------------------------------------
//void BaseEvnt::loadPid(uint8_t id){
//	this->pid = id;
//}
void MQTTBTN_Evnt::doaction(uint8_t save){
	unsigned i = this->pid;
	uint8_t v = static_cast<ParUint8*>(pars[i])->val;
	int n = i / TIMERDIM;
	int sw = i % TIMERDIM;
	
	DEBUG1_PRINT("pid: ");
	DEBUG1_PRINT(i);
	DEBUG1_PRINT(" val: ");
	DEBUG1_PRINT(v);
	in[i] = v;
	static_cast<ParUint8*>(pars[i])->load((uint8_t) 0); //flag reset
	inflag = true;
	DEBUG1_PRINT(" inval: ");
	DEBUG1_PRINTLN(in[i]);
	DEBUG1_PRINTLN("Prima");
	printDfn();
	printOutlogic();
	
	if(roll[n]){
		switchLogic(sw, n);
	}else{
		toggleLogic(sw, n);
	}
	DEBUG1_PRINTLN("Dopo");
	printDfn();
	printOutlogic();
}

void WIFICHANGED_Evnt::doaction(uint8_t save){	
	wifindx=0;
	Serial.println(F("Doing WiFi disconnection"));
	WiFi.persistent(false);
	WiFi.disconnect(false);
	WiFi.mode(WIFI_OFF);    
	//WiFi.mode(WIFI_STA);
	wifindx = 0;
}

//cambio di indirizzo o porta
void MQTTADDR_Evnt::doaction(uint8_t save){
	DEBUG1_PRINT(F("save: "));
	DEBUG1_PRINTLN(save);
	if(save == 2){
		acc = acc || active;
		count++;
		DEBUG1_PRINT(F("count: "));
		DEBUG1_PRINTLN(count);
		if(count >= 2 && acc){
			count = 0;
			if(wifiConn == true){
				DEBUG2_PRINTLN(F("confcmd[MQTTADDRMODFIED] eseguo la reconnect()"));
				mqttReconnect();
			}
		}
	}
}

//cambio di username o psw
void MQTTCONNCHANGED_Evnt::doaction(uint8_t save){
	if(wifiConn == true){
		if(mqttClient==NULL){
			DEBUG2_PRINTLN(F("ERROR confcmd[TOPICCHANGED]! MQTT client is not allocated."));
			mqttReconnect();
			delay(50);
		}else{
			if((wifiConn == true)&& WiFi.status()==WL_CONNECTED && WiFi.getMode()==WIFI_STA){
				if(mqttConnected){
					DEBUG1_PRINT(F("eseguo la MQTT disconnect()...Cnt: "));
					//noInterrupts ();
					mqttClient->disconnect();
					delay(50);
				}
				DEBUG1_PRINTLN(F("MQTTCONNCHANGED! Eseguo la setUserPwd() con usr "));
				DEBUG2_PRINTLN(pars[p(MQTTUSR)]->getStrVal());
				DEBUG2_PRINTLN(F(" e psw "));
				DEBUG2_PRINTLN(pars[p(MQTTPSW)]->getStrVal());
				mqttClient->setUserPwd((const char*)static_cast<ParStr32*>(pars[p(MQTTUSR)])->val, (const char*) static_cast<ParStr32*>(pars[p(MQTTPSW)])->val);
				//////noInterrupts ();
				delay(50);
				DEBUG1_PRINTLN(F("MQTT: Eseguo la re-connect."));
				mqttClient->connect();
				delay(50);
			}
		}
	}
		
}

//cambio di intopic (subscribe)
void MQTTINTOPIC_Evnt::doaction(uint8_t save){	
	if(wifiConn == true && !save){
		if(mqttClient==NULL){
			DEBUG2_PRINTLN(F("ERROR confcmd[INTOPICCHANGED]! MQTT client is not allocated."));
			mqttReconnect();
			delay(50);
		}else{
			DEBUG2_PRINTLN(F("MQTTCONNCHANGED! Eseguo la subscribe() con "));
			DEBUG2_PRINTLN(pars[p(MQTTINTOPIC)]->getStrVal());
			DEBUG2_PRINTLN(F("..."));
			mqttClient->subscribe(pars[p(MQTTINTOPIC)]->getStrVal());
			delay(50);
		}
	}
}

void VALWEIGHT_Evnt::doaction(uint8_t){
	setValweight(static_cast<ParFloat*>(pars[p(VALWEIGHT)])->val);
}
void STDELX_Evnt::doaction(uint8_t){
	setSTDelays(static_cast<ParLong*>(pars[p(STDEL1)])->val, static_cast<ParLong*>(pars[p(STDEL2)])->val);
}
void THALTX_Evnt::doaction(uint8_t){
	setTHalts(static_cast<ParLong*>(pars[p(THALT1)])->val, static_cast<ParLong*>(pars[p(THALT2)])->val, static_cast<ParLong*>(pars[p(THALT3)])->val, static_cast<ParLong*>(pars[p(THALT4)])->val);
}
void THICKNESS_Evnt::doaction(uint8_t){
	setThickness(static_cast<ParFloat*>(pars[p(THICKNESS)])->val);
}
void BARRELRAD_Evnt::doaction(uint8_t){
	setBarrRadius(static_cast<ParFloat*>(pars[p(BARRELRAD)])->val);
}
void SLATSRATIO_Evnt::doaction(uint8_t){
	setSLRatio(static_cast<ParFloat*>(pars[p(SLATSRATIO)])->val);
}
void TLENGTH_Evnt::doaction(uint8_t){
	setTapLen(static_cast<ParFloat*>(pars[p(TLENGTH)])->val);
}
void UTCVAL_Evnt::doaction(uint8_t){
	updateNTP(static_cast<ParLong*>(pars[p(UTCVAL)])->val);
}
void NTPADDR1_Evnt::doaction(uint8_t save){
	if(save==1)   
		saveSingleConf(NTPADDR1);
	setNtpServer(0,static_cast<ParStr64*>(pars[p(NTPADDR1)])->val);
}
void NTPADDR2_Evnt::doaction(uint8_t save){
	if(save==1)   
		saveSingleConf(NTPADDR2);
	setNtpServer(1,static_cast<ParStr64*>(pars[p(NTPADDR2)])->val);
}
void UTCSYNC_Evnt::doaction(uint8_t save){
	if(save==1) 
		setSyncInterval(saveLongConf(UTCSYNC));
	else
		setSyncInterval(static_cast<ParLong*>(pars[p(UTCSYNC)])->val);
}
void UTCADJ_Evnt::doaction(uint8_t save){
	if(save==1) 
		adjustTime(saveIntConf(UTCADJ));
}
void UTCSDT_Evnt::doaction(uint8_t save){
	if(save==1) 
		setSDT(saveByteConf(UTCSDT));
	else
		setSDT(static_cast<ParUint8*>(pars[p(UTCSDT)])->val);
}
void UTCZONE_Evnt::doaction(uint8_t save){
	if(save==1) 
		setTimeZone((int) saveIntConf(UTCZONE));
	else
		setTimeZone((int)static_cast<ParInt*>(pars[p(UTCZONE)])->val);
}
void SWROLL1_Evnt::doaction(uint8_t save){
	if(save==1) 
		saveSingleConf(SWROLL1);	
	setSWMode((uint8_t) static_cast<ParUint8*>(pars[p(SWROLL1)])->val,0); 
}
void SWROLL2_Evnt::doaction(uint8_t save){
	if(save==1) 
		saveSingleConf(SWROLL2);	
	setSWMode((uint8_t) static_cast<ParUint8*>(pars[p(SWROLL2)])->val,0); 
}
void ACTIONEVAL_Evnt::doaction(uint8_t save){
	//save confs and actions on new action received event
	if(save==1) 
		writeOnOffConditions();
	//run actions one time on new action received event
	readActionConfAndSet();
}
void ONCOND1_Evnt::doaction(uint8_t save){
	//save confs and actions on new action received event
	if(save==1) 
		writeOnOffConditions();
}
void ONCOND2_Evnt:: doaction(uint8_t save){
	//save confs and actions on new config received event
	if(save==1) 
		writeOnOffConditions();
}
void ONCOND3_Evnt::doaction(uint8_t save){
	//save confs and actions on new config received event
	if(save==1) 
		writeOnOffConditions();
}
void ONCOND4_Evnt::doaction(uint8_t save){
	//save confs and actions on new config received event
	if(save==1) 
		writeOnOffConditions();
}
void ONCOND5_Evnt::doaction(uint8_t save){
	//save confs and actions on new config received event
	if(save==1) 
		writeOnOffConditions();
}
void MQTTID_Evnt::doaction(uint8_t save){
	openbrk = "{\""+pars[p(MQTTID)]->getStrJsonName()+twodot+pars[p(MQTTID)]->getStrVal()+"\",\"";
}
/*void DEVICEID_Evnt::doaction(uint8_t save){
	openbrk = "{\""+pars[p(DEVICEID)]->getStrFormName()+twodot+pars[p(DEVICEID)]->getStrVal()+"\",\"";
}*/
/*
void SWSPLDPWR1_Evnt::doaction(uint8_t save){
	//save confs and actions on new config received event
	if(save==1) 
		saveFloatConf(SWSPLDPWR1);
	outPwr[0] = static_cast<ParFloat*>(pars[p(SWSPLDPWR1)])->val;
}
void SWSPLDPWR2_Evnt::doaction(uint8_t save){
	//save confs and actions on new config received event
	if(save==1) 
		saveFloatConf(SWSPLDPWR2);
	outPwr[1] = static_cast<ParFloat*>(pars[p(SWSPLDPWR2)])->val;
}
void SWSPLDPWR3_Evnt::doaction(uint8_t save){
	//save confs and actions on new config received event
	if(save==1) 
		saveFloatConf(SWSPLDPWR3);
	outPwr[2] = static_cast<ParFloat*>(pars[p(SWSPLDPWR3)])->val;
}
void SWSPLDPWR4_Evnt::doaction(uint8_t save){
	//save confs and actions on new config received event
	if(save==1) 
		saveFloatConf(SWSPLDPWR4);
	outPwr[3] = static_cast<ParFloat*>(pars[p(SWSPLDPWR4)])->val;
}
*/
void LOGSLCT_Evnt::doaction(uint8_t save){
	uint8_t ser, tlnt, mqtt, num;
	
	DEBUG_PRINT("LOGSLCT_Evnt ");
	
	num = static_cast<ParUint8*>(pars[p(LOGSLCT)])->val;
	
	DEBUG_PRINT("saveByteConf ");
	if(save==1) saveByteConf(LOGSLCT);		////
	
	ser = (num >> 0) & 0x3;
	tlnt = (num >> 2) & 0x3;
	mqtt = (num >> 4) & 0x3;
	//1, 0, 2 = 33 (100001)
	//1, 0, 0 = 33 (000001)
	
	
	DEBUG_PRINT("ser: ");
	DEBUG_PRINT(ser);
	DEBUG_PRINT(", tlnt: ");
	DEBUG_PRINT(tlnt);
	DEBUG_PRINT(", mqtt: ");
	DEBUG_PRINTLN(mqtt);
	
	DEBUG_PRINTLN("delete ");
	//if(dbg1 != NULL)
		//dbg1->destroy();
		delete dbg1;
	//if(dbg2 != NULL)
		//dbg2->destroy();
		delete dbg2;
		
	DEBUG_PRINTLN(F("Oggetti log distrutti "));
	
	dbg1 = NULL;
	dbg2 = NULL;
	
	DEBUG_PRINTLN(F("Oggetti log resi NULL "));
	
	if(ser > 0 && !tlnt & !mqtt){
		DEBUG_PRINTLN(F("if(ser && !tlnt & !mqtt)"));
		//00 00 01
		//00 00 10
		if(ser == 1){
			DEBUG_PRINTLN(F("if(ser == 1) "));
			dbg1 = new SerialLog(1);
			dbg2 = new BaseLog(2);
		}else if(ser == 2){
			DEBUG_PRINTLN(F("if(ser == 2)"));
			dbg1 = new SerialLog(1);
			dbg2 = new SerialLog(2);
		}
	}else if(!ser && tlnt > 0 & !mqtt){
		DEBUG_PRINTLN(F("if(!ser && tlnt & !mqtt)"));
		//00 01 00
		//00 10 00
		if(tlnt == 1){
			DEBUG_PRINTLN(F("iif(tlnt == 1)"));
			dbg1 = new TelnetLog(1, &telnet);
			dbg2 = new BaseLog(2);
		}else if(tlnt == 2){
			DEBUG_PRINTLN(F("if(tlnt == 2)"));
			dbg1 = new TelnetLog(1, &telnet);
			dbg2 = new TelnetLog(2, &telnet);
		}
	}else if(!ser && !tlnt && mqtt > 0){
		DEBUG_PRINTLN(F("if(!ser && !tlnt && mqtt)"));
		//01 00 00
		//10 00 00
		if(mqtt == 1){
			DEBUG_PRINTLN(F("if(mqtt == 1)"));
			dbg1 = new MQTTLog(1, mqttc);
			dbg2 = new BaseLog(2);
		}else if(mqtt == 2){
			DEBUG_PRINTLN(F("iif(mqtt == 2)"));
			dbg1 = new MQTTLog(1, mqttc);
			dbg2 = new MQTTLog(2, mqttc);
		}
	}else if(ser > 0 && tlnt > 0 & !mqtt){
		DEBUG_PRINTLN(F("if(ser && tlnt & !mqtt)"));
		serialTelnet(ser, tlnt);
	}else if(ser > 0 && mqtt > 0 & !tlnt){
		DEBUG_PRINTLN(F("if(ser && mqtt & !tlnt)"));
		serialMQTT(ser, mqtt);
	}else if(tlnt > 0 && mqtt > 0 & !ser){
		DEBUG_PRINTLN(F("f(tlnt && mqtt & !ser)"));
		telnetMQTT(tlnt, mqtt);
	}else if(tlnt == 2 && mqtt == 2 & ser == 2){
		DEBUG_PRINTLN(F("if(tlnt == 2 && mqtt == 2 & ser == 2)"));
		dbg1 = new SerialTelnetMQTTLog(2, &telnet, mqttc);
		dbg2 = new SerialTelnetMQTTLog(2, &telnet, mqttc);
	}else if(tlnt == 2 && mqtt > 0 & ser > 0){
		DEBUG_PRINTLN(F("if(tlnt == 2 && mqtt & ser)"));
		serialMQTT2(ser, mqtt);
	}else if(ser == 2 && tlnt > 0 & mqtt > 0){
		DEBUG_PRINTLN(F("if(ser == 2 && tlnt & mqtt)"));
		telnetMQTT2(tlnt, mqtt);
	}else if(mqtt == 2 && tlnt > 0 & ser > 0){
		DEBUG_PRINTLN(F("if(mqtt == 2 && tlnt & ser)"));
		serialTelnet2(ser, tlnt);
	}else{
		dbg1 = new BaseLog(1);
		dbg2 = new BaseLog(2);
	}
}

void serialTelnet(uint8_t ser, uint8_t tlnt){
	if(ser == 1 && tlnt == 1){
		DEBUG_PRINTLN(F("if(ser == 1 && tlnt == 1)"));
		dbg1 = new SerialTelnetLog(1, &telnet);
		dbg2 = new BaseLog(2);
	}else if(ser == 2 && tlnt == 2){
		DEBUG_PRINTLN(F("if(ser == 2 && tlnt == 2)"));
		dbg1 = new SerialTelnetLog(1, &telnet);
		dbg2 = new SerialTelnetLog(2, &telnet);
	}else if(ser == 2 && tlnt == 1){
		DEBUG_PRINTLN(F("iif(ser == 2 && tlnt == 1)"));
		dbg1 = new SerialTelnetLog(1, &telnet);
		dbg2 = new SerialLog(2);
	}else if(ser == 1 && tlnt == 2){
		DEBUG_PRINTLN(F("if(ser == 1 && tlnt == 2)"));
		dbg1 = new SerialTelnetLog(1, &telnet);
		dbg2 = new TelnetLog(2, &telnet);
	}
}

void telnetMQTT(uint8_t tlnt, uint8_t mqtt){
	if(tlnt == 1 && mqtt == 1){
		DEBUG_PRINTLN(F("if(tlnt == 1 && mqtt == 1)"));
		dbg1 = new TelnetMQTTLog(1, &telnet, mqttc);
		dbg2 = new BaseLog(2);
	}else if(tlnt == 2 && mqtt == 2){
		DEBUG_PRINTLN(F("if(tlnt == 2 && mqtt == 2)"));
		dbg1 = new TelnetMQTTLog(1, &telnet, mqttc);
		dbg2 = new TelnetMQTTLog(2, &telnet, mqttc);
	}else if(tlnt == 2 && mqtt == 1){
		DEBUG_PRINTLN(F("if(tlnt == 2 && mqtt == 1)"));
		dbg1 = new TelnetMQTTLog(1, &telnet, mqttc);
		dbg2 = new TelnetLog(2, &telnet);
	}else if(tlnt == 1 && mqtt == 2){
		DEBUG_PRINTLN(F("if(tlnt == 1 && mqtt == 2)"));
		dbg1 = new TelnetMQTTLog(1, &telnet, mqttc);
		dbg2 = new MQTTLog(2, mqttc);
	}
}

void serialMQTT(uint8_t ser, uint8_t mqtt){
	if(ser == 1 && mqtt == 1){
		DEBUG_PRINTLN(F("\nif(ser == 1 && mqtt == 1)"));
		dbg1 = new SerialMQTTLog(1, mqttc);
		dbg2 = new BaseLog(2);
	}else if(ser == 2 && mqtt == 2){
		DEBUG_PRINTLN(F("if(ser == 2 && mqtt == 2)"));
		dbg1 = new SerialMQTTLog(1, mqttc);
		dbg2 = new SerialMQTTLog(2, mqttc);
	}else if(ser == 2 && mqtt == 1){
		DEBUG_PRINTLN(F("if(ser == 2 && mqtt == 1)"));
		dbg1 = new SerialMQTTLog(1, mqttc);
		dbg2 = new SerialLog(2);
	}else if(ser == 1 && mqtt == 2){
		DEBUG_PRINTLN(F("if(ser == 1 && mqtt == 2)"));
		dbg1 = new SerialMQTTLog(1, mqttc);
		dbg2 = new MQTTLog(2, mqttc);
	}
}

void serialTelnet2(uint8_t ser, uint8_t tlnt){
	if(ser == 1 && tlnt == 1){
		DEBUG_PRINTLN(F("if(ser == 1 && tlnt == 1)"));
		dbg1 = new SerialTelnetMQTTLog(1, &telnet, mqttc);
		dbg2 = new BaseLog(2);
	}else if(ser == 2 && tlnt == 2){
		DEBUG_PRINTLN(F("if(ser == 2 && tlnt == 2)"));
		dbg1 = new SerialTelnetLog(1, &telnet);
		dbg2 = new SerialTelnetLog(2, &telnet);
	}else if(ser == 2 && tlnt == 1){
		DEBUG_PRINTLN(F("if(ser == 2 && tlnt == 1)"));
		dbg1 = new SerialTelnetLog(1, &telnet);
		dbg2 = new SerialLog(2);
	}else if(ser == 1 && tlnt == 2){
		DEBUG_PRINTLN(F("if(ser == 1 && tlnt == 2)"));
		dbg1 = new SerialTelnetLog(1, &telnet);
		dbg2 = new TelnetLog(2, &telnet);
	}
}

void telnetMQTT2(uint8_t tlnt, uint8_t mqtt){
	if(tlnt == 1 && mqtt == 1){
		DEBUG_PRINT(F("if(tlnt == 1 && mqtt == 1)"));
		dbg1 = new SerialTelnetMQTTLog(1, &telnet, mqttc);
		dbg2 = new BaseLog(2);
	}else if(tlnt == 2 && mqtt == 2){
		DEBUG_PRINT(F("if(tlnt == 2 && mqtt == 2)"));
		dbg1 = new TelnetMQTTLog(1, &telnet, mqttc);
		dbg2 = new TelnetMQTTLog(2, &telnet, mqttc);
	}else if(tlnt == 2 && mqtt == 1){
		DEBUG_PRINT(F("if(tlnt == 2 && mqtt == 1)"));
		dbg1 = new TelnetMQTTLog(1, &telnet, mqttc);
		dbg2 = new TelnetLog(2, &telnet);
	}else if(tlnt == 1 && mqtt == 2){
		DEBUG_PRINT(F("if(tlnt == 1 && mqtt == 2)"));
		dbg1 = new TelnetMQTTLog(1, &telnet, mqttc);
		dbg2 = new MQTTLog(2, mqttc);
	}
}

void serialMQTT2(uint8_t ser, uint8_t mqtt){
	if(ser == 1 && mqtt == 1){
		DEBUG_PRINT(F("if(ser == 1 && mqtt == 1)"));
		dbg1 = new SerialTelnetMQTTLog(1, &telnet, mqttc);
		dbg2 = new BaseLog(2);
	}else if(ser == 2 && mqtt == 2){
		DEBUG_PRINT(F("if(ser == 2 && mqtt == 2)"));
		dbg1 = new SerialTelnetMQTTLog(1, &telnet, mqttc);
		dbg2 = new SerialTelnetMQTTLog(2, &telnet, mqttc);
	}else if(ser == 2 && mqtt == 1){
		DEBUG_PRINT(F("if(ser == 2 && mqtt == 1)"));
		dbg1 = new SerialTelnetMQTTLog(1, &telnet, mqttc);
		dbg2 = new SerialLog(2);
	}else if(ser == 1 && mqtt == 2){
		DEBUG_PRINT(F("if(ser == 1 && mqtt == 2)"));	
		dbg1 = new SerialTelnetMQTTLog(1, &telnet, mqttc);
		dbg2 = new MQTTLog(2, mqttc);
	}
}
/*
void WEBUSR_Evnt:: doaction(uint8_t save){
	//save confs and actions on new config received event
	saveSingleConf(WEBUSR);
}
void WEBPSW_Evnt:: doaction(uint8_t save){
	//save confs and actions on new config received event
	saveSingleConf(WEBPSW);
}
*/
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
