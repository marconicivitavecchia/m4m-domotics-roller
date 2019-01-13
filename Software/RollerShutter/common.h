#ifndef __COMMON_H__
#define __COMMON_H__
//#if ARDUINO_VERSION <= 106
//-#pragma "test is true"
//-#endif
//--------------------------DEBUG SWITCH-------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------
//  FILE DI DEFINIZIONI COMUNI VISIBILI A TUTTE  LE LIBRERIE INCLUSE CON LA DIRETTIVA #include
//  DEVE ESSERE INCLUSO CON  #include "common.h"  IN TUTTI GLI HEADERS FILES DELLE LIBRERIE
//----------------------------------------------------------------------------------------------------------------------------------
//libreria col codice del client wifi
extern "C" {
    #include "user_interface.h"
}

#include <Arduino.h>
#include <EEPROM.h>
//#include <sched.h>
#include <ESP8266WiFi.h>
#include <RemoteDebug.h> 
//#include <telnet.h>
#include <MQTT.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <WiFiUdp.h>
#include <WebSocketsServer.h>
//#include <Base64.h>
#include <OneWire.h>
#include <DallasTemperature.h>
//#include <ArduinoOTA.h>
#include "eepromUtils.h"
#include "timersNonSchedulati.h"
#include "schedservices.h"

extern RemoteDebug telnet;
//DEFAULT CONFIGURATIONS
//wifi config----------------------------------------------
#define OUTTOPIC		"sonoff17/out"
#define INTOPIC			"sonoff17/in"
#define SSID1			"OpenWrt"
#define PSW1			"dorabino.7468!"
#define SSID2			"AndroidAP1"
#define PSW2			"pippo2503"
#define SSIDAP			"admin"
#define PSWAP			"admin"
#define MQTTSRV			"iot.eclipse.org"
#define MQTTCLIENTID 	"NodeMCUtanzo2344378"
//END DEFAULTS

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
#define PUSHINTERV	 60 // in sec
#define ONE_WIRE_BUS 2  // DS18B20 pin
#define RUNDELAY  	3
#define DELTAL		4
#define AUTOCAL		1
#define NSIGMA 		10
#define EMA  		0.7
#define THALTMAX   	90000 
#define DEBUG   	1		//ACTIVATE DEBUG MODE
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
#define NCNT	 	2		//OVERALL NUMBER OF COUNTERS (FUNCTION COUNTERS + SPECIAL COUNTERS)  
#define TIMERN  	4		//OVERALL NUMBER OF TIMERS (FUNCTION TIMERS + SPECIAL TIMERS)      
#define SWITCHND 	4		//OVERALL NUMBER OF SWITCHES (FUNCTION SWITCHES + SPECIAL SWITCHES) 
#define JSONN   	4		//JSON ARRAY DIMENSION 
//INIZIO COSTANTI LOGICA DI COMANDO---------------------------------------------------
#define NBTN     	2     	//NUMBER OF BUTTONS
#define	BTNDEL1   	500    	//BTN DELAY (ms)
#define	BTNDEL2  	500    	//BTN DELAY (ms)
#define ENABLES    	0		//ENABLES STATE
#define DIRS       	1		//DIR SELECTION STATE
#define SW1ONS      2		//SWITCH 1 (UP) ON STATE
#define SW2ONS     	3		//SWITCH 2 (DOWN) ON STATE
#define STATUSDIM  	4		//STATUS ARRAY DIMENSION (NUMBER OF STATES FOR BUTTON GROUPS)
#define TMRHALT   	0		//INDICE TIMER DI FINE CORSA
#define TIMERDIM  	1		//FUNCTION TIMERS ARRAY DIMENSION (NUMBER OF FUNCTION TIMERS FOR BUTTON GROUPS)
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
#define RESETTIMER  2		//INDEX OF RESET TIMER (DETECTS IF A RESET COMMAND OCCUR)
#define APOFFTIMER	3		//INDEX OF AP TIMER (DETECTS IF A AP ACTIVATION COMMAND OCCUR)
#define CNTSERV1	0		//INDEX OF SERVICE COUNTER GROUP 1 (DETECTS AND COUNT MAIN BUTTON CLICKS)
#define CNTSERV2	1		//INDEX OF SERVICE COUNTER GROUP 2 (DETECTS AND COUNT MAIN BUTTON CLICKS)
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
#define MEANPWR1RND		0
#define MEANPWR2RND		0	
#define PEAKPWR1RND		0
#define PEAKPWR2RND		0
//--------------------------EEPROM offsets-------------------------------------------
#define NAMEOFST				0
#define THALT1OFST             	8
#define THALT2OFST				24
#define MQTTADDROFST			40
#define MQTTIDOFST				72
#define OUTTOPICOFST			104
#define INTOPICOFST				136
#define MQTTJSONUP1OFST			168
#define MQTTJSONDOWN1OFST		200
#define MQTTJSONUP2OFST			232
#define MQTTJSONDOWN2OFST		264
#define MQTTJSONTEMPOFST		296
#define MQTTJSONMEANPWROFST		328
#define MQTTJSONPEAKPWROFST		360
#define MQTTJSONALLOFST			392
#define WIFICLIENTSSIDOFST1		424
#define WIFICLIENTPSWOFST1		456
#define WIFICLIENTSSIDOFST2		488
#define WIFICLIENTPSWOFST2		520
#define WIFIAPSSIDOFST			552
#define WIFIAPPPSWOFST			584
#define WEBUSROFST      		616
#define WEBPSWOFST				648
#define MQTTUSROFST				680
#define MQTTPSWOFST				712
#define STDEL1OFST				744
#define STDEL2OFST				776
#define VALWEIGHTOFST			808
#define	TLENGTHOFST				840
#define	BARRELRADOFST			872
#define	THICKNESSOFST			904
//#define TRSHOLD1OFST			872
//#define TRSHOLD2OFST			712
#define EEPROMPARAMSLEN			936
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
#define STDEL1					16
#define STDEL2					17
#define VALWEIGHT				18
#define	TLENGTH					19
#define	BARRELRAD				20
#define	THICKNESS				21
#define LOCALIP					22
#define WIFICHANGED				23
#define CONFLOADED				24
#define MQTTADDRMODFIED			25
#define TOPICCHANGED			26
#define MQTTCONNCHANGED			27
#define	TIMINGCHANGED			28
#define PARAMSDIM				29
//--------------------------Inizio mqttJson array indexes-----------------------------------
#define MQTTJSONUP1				0
#define MQTTJSONDOWN1			1
#define MQTTJSONUP2				2
#define MQTTJSONDOWN2			3
#define MQTTJSONTEMP			4
#define MQTTJSONMEANPWR			5
#define MQTTJSONPEAKPWR			6
#define MQTTJSONALL				7
#define MQTTJSONDIM				8
//--------------------------Fine array indexes-----------------------------------
//-----------------------DEBUG MACRO------------------------------------------------------------
//#define F(string_literal) (reinterpret_cast<const __FlashStringHelper *>(PSTR(string_literal)))
//#define PGMT( pgm_ptr ) ( reinterpret_cast< const __FlashStringHelper * >( pgm_ptr ) )
#ifdef DEBUG
 //#define telnet_print(x) 	if (telnet.isActive(telnet.ANY)) 	telnet.print(x)
 #define DEBUG_PRINT(x)   Serial.print (x); telnet.print(x)
 //#define DEBUG_PRINTDEC(x)     Serial.print (x, DEC);  telnet.print(x)
 #define DEBUG_PRINTLN(x)   Serial.println (x);  telnet.println(x)
#else
 #define DEBUG_PRINT(x)
 //#define DEBUG_PRINTDEC(x)
 #define DEBUG_PRINTLN(x) 
#endif

//legge gli ingressi dei tasti già puliti dai rimbalzi
#define leggiTastiLocali()  in[BTN1IN] =!digitalRead(BTN1U);	\
    in[BTN2IN] =!digitalRead(BTN1D);	\
    in[BTN1IN+BTNDIM] =!digitalRead(BTN2U); 	\
    in[BTN2IN+BTNDIM] =!digitalRead(BTN2D)
//legge gli ingressi remoti e mette il loro valore sull'array val[]
//inr: memoria tampone per l'evento asincrono scrittura da remoto
//si deve mischiare con la lettura locale DOPO che questa venga scritta
//al giro di loop() successivo in[] locale riporta a livello basso l'eccitazione remota
//legge gli ingressi dei tasti già puliti dai rimbalzi	
#define leggiTastiLocaliRemoto() in[BTN1IN] = in[BTN1IN] + inr[BTN1IN];	\
    in[BTN2IN] = in[BTN2IN] + inr[BTN2IN];	\
    in[BTN1IN+BTNDIM] = in[BTN1IN+BTNDIM] + inr[BTN1IN+BTNDIM];	  \
    in[BTN2IN+BTNDIM] = in[BTN2IN+BTNDIM] + inr[BTN2IN+BTNDIM]; 	\
	for(int i=0;i<4;i++)	\
		inr[i]=LOW	
//legge legge lo stato dei pulsanti e scrive il loro valore sulle porte di uscita dei led
#if (SCR)
#define scriviOutDaStato()  digitalWrite(OUT1DD,(outLogic[ENABLES] && (outLogic[DIRS]==HIGH)));	\
	 digitalWrite(OUT1EU,(outLogic[ENABLES] && (outLogic[DIRS]==LOW)));		\
	 digitalWrite(OUT2DD,(outLogic[ENABLES+STATUSDIM] && (outLogic[DIRS+STATUSDIM]==HIGH)));	\
	 digitalWrite(OUT2EU,(outLogic[ENABLES+STATUSDIM] && (outLogic[DIRS+STATUSDIM]==LOW)));		\
	 isrun[0] = (outLogic[ENABLES]==HIGH);					\
	 isrun[1] = (outLogic[ENABLES+STATUSDIM]==HIGH)		
#else		
#define scriviOutDaStato()	digitalWrite(OUT1EU,outLogic[ENABLES]);	\
	 digitalWrite(OUT1DD,outLogic[DIRS]);		\
	 digitalWrite(OUT2EU,outLogic[ENABLES+STATUSDIM]);		\
	 digitalWrite(OUT2DD,outLogic[DIRS+STATUSDIM]);			\
	 isrun[0] = (outLogic[ENABLES]==HIGH);					\
	 isrun[1] = (outLogic[ENABLES+STATUSDIM]==HIGH)			 
#endif

//#define tapLogic(n)	(switchLogic(0,n) + switchLogic(1,n))

//PRIMA DEFINISCO LE COSTANTI, POI INCLUDO I FILES HEADERS (.h) CHE LE USANO
#include "tapparellaLogic.h"
#include "serialize.h"
#include "logicaTasti.h"

#if (AUTOCAL)  
#include "statistics.h"
#endif

void setup_AP(bool);
void setup_wifi();
void setup_mDNS();
void mqttReconnect();
//void mqttCallback(String (&)[PARAMSDIM], String (&)[MQTTJSONDIM]);
void mqttCallback(String &, String &);
void readStatesAndPub(bool all = false);
void readAvgPowerAndPub();
void readPeakPowerAndPub();
void readTempAndPub();
void publishStr(String &);
//void leggiTasti();
//void scriviOutDaStato();
void saveOnEEPROM();
void loadConfig();
void rebootSystem();
void onStationConnected(const WiFiEventSoftAPModeStationConnected&);
void onStationDisconnected(const WiFiEventSoftAPModeStationDisconnected&);
void testFlash();
void initIiming(bool);
void printConfig();
void processCmdRemoteDebug();
void webSocketEvent(uint8_t, WStype_t, uint8_t *, size_t);
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
void initCommon(ESP8266WebServer *,  String  *, String  *);

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
  
  

int Sensor_Value=0;
float voltage;
float ARDUINO_ANALOG_SCALING = 0.00488758;
float Actual_voltage;
float Current_Sensor_Value;
void setup()
{
  Serial.begin(9600);
  pinMode(A0, INPUT) ;

}

void loop()
{

  CS_Output();
  Serial.println("........................");
  delay(1000);
}


void CS_Output()
{
  float average = 0;
  for(int i = 0; i < 1000; i++) {
    average = average + (.0264 * analogRead(A0) -13.51) / 1000;
    delay(1);
  }
  Serial.print("before calibration:");
  Serial.println(average);
  average=average+0.15;
  Serial.print("after calibration:");
  Serial.println(average);  
}


 */
