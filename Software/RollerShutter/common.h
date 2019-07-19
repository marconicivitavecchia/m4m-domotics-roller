#ifndef __COMMON_H__
#define __COMMON_H__
//#if ARDUINO_VERSION <= 106
//-#pragma "test is true"
//-#endif
//--------------------------_DEBUG1 SWITCH-------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------
//  FILE DI DEFINIZIONI COMUNI VISIBILI A TUTTE  LE LIBRERIE INCLUSE CON LA DIRETTIVA #include
//  DEVE ESSERE INCLUSO CON  #include "common.h"  IN TUTTI GLI HEADERS FILES DELLE LIBRERIE
//----------------------------------------------------------------------------------------------------------------------------------
//libreria col codice del client wifi
//extern "C" {
//    #include "user_interface.h"
//}

#include <Arduino.h>
#include <EEPROM.h>
//#include <sched.h>
#include <ESP8266WiFi.h>
#include <MQTT.h>                         // https://github.com/i-n-g-o/esp-mqtt-arduino
#include <WebSocketsServer.h>             //  https://github.com/Links2004/arduinoWebSockets
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>

//#include <ArduinoOTA.h>
#include "eepromUtils.h"
#include "timersNonSchedulati.h"
#include "schedservices.h"
#include "abparser.h"

//DEFAULT CONFIGURATIONS
//wifi config----------------------------------------------
#define OUTTOPIC		"sonoff17/out"
#define INTOPIC			"sonoff17/in"
#define SSID1			"WebPocket-E280"
#define PSW1			"dorabino.7468!"
#define SSID2			"AndroidAP1"
#define PSW2			"pippo2503"
#define SSIDAP			"admin"
#define PSWAP			"admin"
#define MQTTSRV			"iot.eclipse.org"
#define MQTTCLIENTID 	"mytapparella"
//END DEFAULTS
//_DEBUG1 LEVELS---------------------
#define _DEBUG1   	1		//ACTIVATE LOCAL AND REMOTE _DEBUG1 MODE
#define _DEBUGR   	0		//ACTIVATE ONLY REMOTE _DEBUG1 MODE
//LARGE FW OTA UPLOAD---------------------
#define LARGEFW 		1
//----------------------------------------
//Definizione modello
#define SONOFF_4CH		1
#define ROLLERSHUTTER 	0

#if (ROLLERSHUTTER)
  #define SCR    1  
  #define INPULLUP  0  			//disable internal pullup
  #define OUTSLED  	2     
  #define OUT1EU  	12      	// OUT1 =  MOTOR1 UP   
  #define OUT1DD  	4 //5    	// OUT2 =  MOTOR1 DOWN     
  #define OUT2EU  	5 //4     	// OUT3 =  MOTOR2 UP  
  #define OUT2DD  	15       	// OUT4 =  MOTOR2 DOWN
  //local buttons
  #define BTN1U    	0    		// IN1   =  MOTOR1 UP    
  #define BTN1D    	13    		// IN2   =  MOTOR1 DOWN    
  #define BTN2U    	16    		// IN3   =  MOTOR2 UP    
  #define BTN2D    	14    		// IN4   =  MOTOR2 DOWN
#elif (SONOFF_4CH)
  #define SCR		1 
  #define INPULLUP  1   		//enable internal pullup
  #define OUTSLED	13     
  #define OUT1EU	12  
  #define OUT1DD	5   
  #define OUT2EU	4     
  #define OUT2DD	15    
  //local buttons
  #define BTN1U		0    
  #define BTN1D		9  
  #define BTN2U		10   
  #define BTN2D		14   
#else
  #error Wrong version defined - cannot continue!
#endif

//#define THRESHOLD1	13
//#define THRESHOLD2	13
//#define RAMPDELAY1	1  		//n*20ms
//#define RAMPDELAY2	1  		//n*20ms
#define ONGAP		20			//divisioni ADC della soglia di ON motore
#define ENDFACT		2	// (%) margine di posizionamento con i sensori in percentuale dell'escursione totale, dopo avviene col timer
#define PUSHINTERV	 60 // in sec
#define ONE_WIRE_BUS 2  // DS18B20 pin
#define RUNDELAY  	1
#define DELTAL		4
#define AUTOCAL		1
#define NSIGMA 		3
#define EMA  		0.8
#define THALTMAX   	90000 
#define	TCOUNT		5		//MAX FAILED CONNECTION ATTEMPTS BEFORE WIFI CLIENT COMMUTATION
#define RSTTIME		20		//DEFINE HOW MANY SECONDS BUTTON1 MUST BE PRESSED UNTIL A RESET OCCUR 
#define CNTIME		4		//DEFINE HOW MANY SECONDS HAVE TO LAST THALT PARAMETER AT LEAST
#define CHROLLER	0
#define CHTEMP		1
#define CHPOWER		2
#define ALL			3
//#define CONFTIME	4		//DEFINE HOW MANY SECONDS 
#define APOFFT		120		//DEFINE HOW MANY SECONDS   
#define MAINBTN		3		//BUTTON WITH PROGRAMMING AND RESET FUNCTIONS
#define UP			1
#define DOWN		-1
//Dimensione dei vettori di switch, timers e counters
#define CHRN	 	2		//OVERALL NUMBER OF CHRONOMETERS (FUNCTION COUNTERS + SPECIAL COUNTERS) 
#define TIMERN  	6		//OVERALL NUMBER OF TIMERS (FUNCTION TIMERS + SPECIAL TIMERS)      
#define SWITCHND 	4		//OVERALL NUMBER OF SWITCHES (FUNCTION SWITCHES + SPECIAL SWITCHES) 
#define JSONN   	4		//JSON ARRAY DIMENSION 
//INIZIO COSTANTI LOGICA DI COMANDO---------------------------------------------------
#define TENDCHECK	2		//MAX TIME IN SEC BEFORE THE EFFECTIVE START OF THE MOTOR 
#define NBTN     	2     	//NUMBER OF BUTTONS
#define	BTNDEL1   	500    	//BTN DELAY (ms)
#define	BTNDEL2  	500    	//BTN DELAY (ms)
#define ENABLES    	0		//ENABLES STATE
#define DIRS       	1		//DIR SELECTION STATE
#define SW1ONS      2		//SWITCH 1 (UP) ON STATE
#define SW2ONS     	3		//SWITCH 2 (DOWN) ON STATE
#define STATUSDIM  	4		//STATUS ARRAY DIMENSION (NUMBER OF STATES FOR BUTTON GROUPS)
#define TMRHALT   	0		//INDICE TIMER DI FINE CORSA
#define TMRHALT2 	1		//INDEX OF 
#define TIMERDIM  	2		//FUNCTION TIMERS ARRAY DIMENSION (NUMBER OF FUNCTION TIMERS FOR BUTTON GROUPS)
#define BTN1IN      0       //UP BUTTON INDEX
#define BTN2IN     	1		//DOWN BUTTON INDEX
#define BTNDIM     	2     	//FUNCTION SWITCH ARRAY DIMENSION (NUMBER OF FUNCTION SWITCHES FOR BUTTON GROUPS)
#define TAP1      	0		//INDICE TAPPARELLA 1
#define TAP2      	1		//INDICE TAPPARELLA 2
#define OUTDIM    	2     	//OUT PORT ARRAY DIMENSION (NUMBER OF OUT PORTS FOR BUTTON GROUPS)
//#define JSONLEN   	8		//NUMBER OF FIELDS OF THE JSON MESSAGE
#define STATBTNNDX  0  	    //STATUS BUTTON INDEX
//special array elements (not used for normal functions in all button groups, are always after normal elements)
//#define CONNSTATSW  4		//INDEX OF CONNECTION SWITCH (DETECTS WIFI STATUS RISE FRONTS)
#define RESETTIMER  4		//INDEX OF RESET TIMER (DETECTS IF A RESET COMMAND OCCUR)
#define APOFFTIMER	5		//INDEX OF AP TIMER (DETECTS IF A AP ACTIVATION COMMAND OCCUR)
#define CNTSERV1	0		//INDEX OF SERVICE COUNTER GROUP 1 (DETECTS AND COUNT MAIN BUTTON CLICKS)
#define CNTSERV2	1		//INDEX OF SERVICE COUNTER GROUP 2 (DETECTS AND COUNT MAIN BUTTON CLICKS)
#define CNTSERV3	2		//INDEX OF SERVICE COUNTER GROUP 1 (DETECTS AND COUNT MAIN BUTTON CLICKS)
#define CNTSERV4	3		//INDEX OF SERVICE COUNTER GROUP 2 (DETECTS AND COUNT MAIN BUTTON CLICKS)
#define CNTIME1		4		//INDEX OF
#define CNTIME2		5		//INDEX OF
#define CNTIME3		6		//INDEX OF
#define CNTIME4		7		//INDEX OF
#define SMPLCNT1	8		//INDEX OF
#define SMPLCNT2	9		//INDEX OF
#define SMPLCNT3	10		//INDEX OF
#define SMPLCNT4	11		//INDEX OF
#define NCNT	 	12		//OVERALL NUMBER OF COUNTERS (FUNCTION COUNTERS + SPECIAL COUNTERS) 
#define BTNUP		0		//INDEX OF CALIBRATION CRONO (DETECTS UP TIME AND DOWN TIME)
#define BTNDOWN		1		//INDEX OF CALIBRATION CRONO (DETECTS UP TIME AND DOWN TIME)
#define WIFISTA		0
#define WIFIAP		1
//Sensor gates
#define GTTEMP		0
#define GTMEANPWR1	1
#define GTMEANPWR2	2
#define GTPEAKPWR1	3
#define GTPEAKPWR2	4
#define GATEND 		5	//OVERALL NUMBER OF GATES
//Async R/W buffers
#define ASYNCDIM	5	//OVERALL NUMBER OF R/W BUFFERS
#define TEMPRND			0
#define MEANPWR1RND		1
#define MEANPWR2RND		1	
#define PEAKPWR1RND		1
#define PEAKPWR2RND		1
//--------------------------EEPROM offsets-------------------------------------------
//First two byte reserved for EEPROM check
//1 byte offets (char)
#define RESERVEBYTE1OFST		2
#define RESERVEBYTE2OFST		3
#define RESERVEBYTE3OFST		4
#define RESERVEBYTE4OFST		5
#define SWROLL1OFST				6
#define SWROLL2OFST				7
#define RESERVEBYTE5OFST		8
#define RESERVEBYTE6OFST		9
//2 byte offets (int)
#define EEPROMLENOFST			10
#define THALT1OFST             	12
#define THALT2OFST				14
#define THALT3OFST				16
#define THALT4OFST				18   
//4 byte offets (float)
#define RESERVEFLOAT1OFST		20
#define STDEL1OFST				24
#define STDEL2OFST				28
#define VALWEIGHTOFST			32
#define	TLENGTHOFST				36
#define	BARRELRADOFST			40
#define	THICKNESSOFST			44
#define SLATSRATIOFST			48
//8 byte offsets (fixed short String)
#define NAMEOFST				52
//32 byte offsets (fixed medium String)
#define	MQTTIDOFST				60
#define	OUTTOPICOFST			92
#define	INTOPICOFST				124
#define	MQTTJSONUP1OFST			156
#define	MQTTJSONDOWN1OFST		188
#define	MQTTJSONUP2OFST			220
#define	MQTTJSONDOWN2OFST		252
#define	MQTTJSONTEMPOFST		284
#define	MQTTJSONMEANPWROFST		316
#define	MQTTJSONPEAKPWROFST		348
#define	MQTTJSONALLOFST			380
#define	WIFICLIENTSSIDOFST1		412
#define	WIFICLIENTPSWOFST1		444
#define	WIFICLIENTSSIDOFST2		476
#define	WIFICLIENTPSWOFST2		508
#define	WIFIAPSSIDOFST			540
#define	WIFIAPPPSWOFST			572
#define	WEBUSROFST				604
#define	WEBPSWOFST				636
#define	MQTTUSROFST				668
#define	MQTTPSWOFST				700
//64 byte offsets (fixed long String)
#define MQTTADDROFST			732
#define NTPADDROFST				796
#define RESERVEADDROFST			860
//end fixed lenght params
#define FIXEDPARAMSLEN			924
//x byte offsets (variable String)


//--------------------------Fine EEPROM offsets-------------------------------------------
//--------------------------Inizio params array indexes-----------------------------------
#define WEBUSR					0
#define WEBPSW					1
#define APPSSID					2
#define APPPSW					3
#define CLNTSSID1				4
#define CLNTPSW1				5
#define CLNTSSID2				6
#define CLNTPSW2				7
#define MQTTADDR				8
#define MQTTID					9
#define MQTTOUTTOPIC			10
#define MQTTINTOPIC				11
#define MQTTUSR					12
#define MQTTPSW					13
#define THALT1					14
#define THALT2					15
#define THALT3					16
#define THALT4					17
#define STDEL1					18
#define STDEL2					19
#define VALWEIGHT				20
#define	TLENGTH					21
#define	BARRELRAD				22
#define	THICKNESS				23
#define	SLATSRATIO				24
#define SWROLL1					25
#define SWROLL2					26
#define SWACTION1				27
#define SWACTION2				28
#define SWACTION3				29
#define SWACTION4				30
#define LOCALIP					31
#define WIFICHANGED				32
#define CONFLOADED				33
#define MQTTADDRMODFIED			34
#define TOPICCHANGED			35
#define MQTTCONNCHANGED			36
#define	TIMINGCHANGED			37
#define PARAMSDIM				38
//--------------------------Inizio mqttJson array indexes-----------------------------------
#define MQTTJSONUP1				0
#define MQTTJSONDOWN1			1
#define MQTTJSONUP2				2
#define MQTTJSONDOWN2			3
#define MQTTJSONTEMP			4
#define MQTTJSONMEANPWR			5
#define MQTTJSONPEAKPWR			6
#define MQTTJSONALL				7
#define MQTTJSONMAC				8
#define MQTTJSONIP				9
#define MQTTJSONTIME			10
#define MQTTJSONMQTTID			11
#define JSONCONFFLAG1			12
#define JSONCONFFLAG2			13
#define JSONCONFFLAG3			14
#define JSONCONFFLAG4			15
#define JSONACTIONFLAG			16
#define MQTTJSONDIM				17
//--------------------------Inizio mqttJson config array indexes-----------------------------------
#define JSONCONFEVAL1			0
#define JSONCONFEVAL2			1
#define JSONCONFEVAL3			2
#define JSONCONFEVAL4			3
#define JSONACTIONEVAL			4
#define CONFJSONDIM				5
//--------------------------Fine array indexes-----------------------------------

#if (LARGEFW)
	//#include <WiFiUdp.h>
	#include <ESP8266mDNS.h>
	#include <RemoteDebug.h>                  // https://github.com/JoaoLopesF/RemoteDebug
	#include <OneWire.h>                      //  https://www.pjrc.com/teensy/td_libs_OneWire.html
	#include <DallasTemperature.h>            //  https://github.com/milesburton/Arduino-Temperature-Control-Library
	#include <Pinger.h>
	//#include "ntp.h"
	extern RemoteDebug telnet;
	void setup_mDNS();
#else
	#define _DEBUGR			0	
	#define _DEBUG1   	    0		
#endif

//-----------------------_DEBUG1 MACRO------------------------------------------------------------
//#define F(string_literal) (reinterpret_cast<const __FlashStringHelper *>(PSTR(string_literal)))
//#define PGMT( pgm_ptr ) ( reinterpret_cast< const __FlashStringHelper * >( pgm_ptr ) )

//legge gli ingressi dei tasti già puliti dai rimbalzi
#define leggiTastiLocali()  in[BTN1IN] =!digitalRead(BTN1U);	\
    in[BTN2IN] =!digitalRead(BTN1D);	\
    in[BTN1IN+BTNDIM] =!digitalRead(BTN2U); 	\
    in[BTN2IN+BTNDIM] =!digitalRead(BTN2D)

#if (_DEBUG1)
 //#define telnet_print(x) 	if (telnet.isActive(telnet.ANY)) 	telnet.print(x)
 #define DEBUG_PRINT(x)   Serial.print (x);telnet.print(x)	
 //#define DEBUG_PRINTDEC(x)     Serial.print (x, DEC);  telnet.print(x)
 #define DEBUG_PRINTLN(x)   Serial.println (x);telnet.println(x)
#elif (_DEBUGR)
  #define DEBUG_PRINT(x)   telnet.print(x)
  #define DEBUG_PRINTLN(x) telnet.println(x)
#else 
 #define DEBUG_PRINT(x)
 //#define DEBUG_PRINTDEC(x)
 #define DEBUG_PRINTLN(x) 
#endif	

//PRIMA DEFINISCO LE COSTANTI, POI INCLUDO I FILES HEADERS (.h) CHE LE USANO
#include "tapparellaLogic.h"
#include "serialize.h"
#include "logicaTasti.h"

#if (AUTOCAL)  
#include "statistics.h"
#endif

void setup_AP(bool);
void setup_wifi();
void mqttReconnect();
//void mqttCallback(String (&)[PARAMSDIM], String (&)[MQTTJSONDIM]);
void mqttCallback(String &, String &);
void readStatesAndPub(bool all = false);
void readAvgPowerAndPub();
void readPeakPowerAndPub();
void readTempAndPub();
void readMacAndPub();
void readIpAndPub();
void readTimeAndPub();
void readMQTTIdAndPub();
void publishStr(String &);
float getAmpRMS(float);
float getTemperature();
//void leggiTasti();
void scriviOutDaStato();
void saveOnEEPROM(int);
void loadConfig();
void rebootSystem();
void onStationConnected(const WiFiEventSoftAPModeStationConnected&);
void onStationDisconnected(const WiFiEventSoftAPModeStationDisconnected&);
void testFlash();
void initIiming(bool);
void printConfig();
void processCmdRemoteDebug();
void webSocketEvent(uint8_t, WStype_t, uint8_t *, size_t);
void setSWMode(byte, byte);

/*
//http server callback function prototypes
void handleRoot(ESP8266WebServer (&), String const (&)[PARAMSDIM]);        // function prototypes for HTTP handlers
void handleLogin(ESP8266WebServer (&),  String const (&)[PARAMSDIM], String const (&)[MQTTJSONDIM]);
void handleNotFound(ESP8266WebServer (&));
void handleModify(ESP8266WebServer (&), String (&)[PARAMSDIM], String (&)[MQTTJSONDIM]);
void handleWifiConf(ESP8266WebServer (&),  String const (&)[PARAMSDIM], String const (&)[MQTTJSONDIM]);
void handleSystemConf(ESP8266WebServer (&),  String const (&)[PARAMSDIM], String const (&)[MQTTJSONDIM]);
void handleMQTTConf(ESP8266WebServer (&),  String const (&)[PARAMSDIM], String const (&)[MQTTJSONDIM]);
void handleLogicConf(ESP8266WebServer (&),  String const (&)[PARAMSDIM], String const (&)[MQTTJSONDIM]);
void handleCmd(ESP8266WebServer (&),  String const (&)[PARAMSDIM], String const (&)[MQTTJSONDIM]);
*/
//void handleCmdJson(ESP8266WebServer (&), String&);
bool is_authentified(ESP8266WebServer&);
// function prototypes for HTTP handlers
void initCommon(ESP8266WebServer *,  String  *, String  *, String  *, String  *);

void handleRoot();              
void handleLogin();
void handleNotFound();
void handleModify();
void handleSystemConf();
void handleWifiConf();
void handleMQTTConf();
void handleCmd();
void handleLogicConf();
void handleModify();
void handleEventConf();
void writeOnOffConditions();
void writeOnOffAction(byte, byte);
void writeSWMode(byte, byte);
void writeHaltDelay(unsigned int, byte);
//void readMqttConfAndSet(int);
#endif
/*
{ "Sonoff 4CH",      // Sonoff 4CH (ESP8285)

     GPIO_KEY1,        // GPIO00 Button 1

     GPIO_USER,        // GPIO01 Serial RXD and Optional sensor

     GPIO_USER,        // GPIO02 Optional sensor (riservato alla selezione del boot mode, può essere in o out ma al boot deve essere sempre alto)

     GPIO_USER,        // GPIO03 Serial TXD and Optional sensor

     GPIO_REL3,        // GPIO04 Sonoff 4CH Red Led and Relay 3 (0 = Off, 1 = On)

     GPIO_REL2,        // GPIO05 Sonoff 4CH Red Led and Relay 2 (0 = Off, 1 = On)

     0, 0, 0,          // Flash connection

     GPIO_KEY2,        // GPIO09 Button 2

     GPIO_KEY3,        // GPIO10 Button 3

     0,                // Flash connection

     GPIO_REL1,        // GPIO12 Red Led and Relay 1 (0 = Off, 1 = On)

     GPIO_LED1_INV,    // GPIO13 Blue Led (0 = On, 1 = Off)

     GPIO_KEY4,        // GPIO14 Button 4

     GPIO_REL4,        // GPIO15 Red Led and Relay 4 (0 = Off, 1 = On)

     0, 0

  },
  
 */
