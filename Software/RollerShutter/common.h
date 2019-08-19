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
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <MQTT.h> 
//#include <WebSockets.h> 
//#include <WebSocketsClient.h>                     
#include <WebSocketsServer.h>           
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
//#define SSID1			"WebPocket-E280"
#define SSID1			"D-Link-6A30CC"
#define PSW1			"FabSeb050770250368120110"
#define SSID2			"AndroidAP1"
#define PSW2			"pippo2503"
#define SSIDAP			"admin"
#define PSWAP			"admin"
#define MQTTSRV			"broker.hivemq.com"
#define MQTTPRT			"1883"
#define WSPRT			"8000"
#define MQTTPT			"mqtt"
#define MQTTCLIENTID 	"mytapparella"
#define ROLLMODE1 		0
#define ROLLMODE2 		0
//END DEFAULTS
//_DEBUG1 LEVELS---------------------
#define _DEBUG1   		1		//ACTIVATE LOCAL AND REMOTE _DEBUG1 MODE
#define _DEBUGR   		0		//ACTIVATE ONLY REMOTE _DEBUG1 MODE
//LARGE FW OTA UPLOAD---------------------
#define LARGEFW 		1
//----------------------------------------
//Definizione modello
#define SONOFF_4CH				1
#define ROLLERSHUTTER 			0
//-------------------------------------------------------------------------------------------------------------------------------------------------
//-------------NON MODIFICARE LA PARTE SOTTOSTATNTE------------------------------------------------------------------------------------------------
//#define AUTOCAL_HLW8012			0
//#define AUTOCAL_ACS712			1
//#define MCP2317					1

#if (ROLLERSHUTTER)
  #define SCR    1  
  #define INPULLUP  0  			//disable internal pullup
  #define OUTSLED  	2     
  #define OUT1EU  	8      		// OUT1 =  MOTOR1 UP   
  #define OUT1DD  	9 //5    	// OUT2 =  MOTOR1 DOWN     
  #define OUT2EU  	10 //4     	// OUT3 =  MOTOR2 UP  
  #define OUT2DD  	11      		// OUT4 =  MOTOR2 DOWN
  #define GREEN  	12  
  #define RED  		13
  #define BLUE  	14      		
  //local buttons
  #define BTN1U    	0    		// IN1   =  MOTOR1 UP    
  #define BTN1D    	1    		// IN2   =  MOTOR1 DOWN    
  #define BTN2U    	2    		// IN3   =  MOTOR2 UP    
  #define BTN2D    	3    		// IN4   =  MOTOR2 DOWN
  #define AUTOCAL_HLW8012			1
  #define AUTOCAL_ACS712			0
  #define MCP2317					1
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
  #define AUTOCAL_ACS712			1
  #define AUTOCAL_HLW8012			0
  #define MCP2317					0
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
#define	BTNDEL1   	600    	//BTN DELAY (ms)
#define	BTNDEL2  	600    	//BTN DELAY (ms)
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
//#define TIMECNT		12		//INDEX OF
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
#define DONOTUSE				2
#define RESERVEBYTE2OFST		3
#define RESERVEBYTE3OFST		4
#define ACVOLTOFST				5
#define SWROLL1OFST				6
#define SWROLL2OFST				7
#define NTPSDTOFST				8
#define NTPZONEOFST				9
//2 byte offets (int)
#define EEPROMLENOFST			10
#define THALT1OFST             	12
#define THALT2OFST				14
#define THALT3OFST				16
#define THALT4OFST				18
#define NTPADJUSTOFST  			20
//4 byte offets (float)
#define PWRMULTOFST				22
#define STDEL1OFST				26
#define STDEL2OFST				30
#define VALWEIGHTOFST			34
#define	TLENGTHOFST				38
#define	BARRELRADOFST			42
#define	THICKNESSOFST			46
#define SLATSRATIOFST			50
#define NTPSYNCINTOFST			54
#define CALPWROFST				58
#define RESERVED1FLOTAFST		62
//32 byte offsets (fixed medium String)
#define	MQTTIDOFST				66
#define	OUTTOPICOFST			98
#define	INTOPICOFST				130
#define	MQTTUP1OFST				162
#define	MQTTDOWN1OFST			194
#define	MQTTUP2OFST				226
#define	MQTTDOWN2OFST			258
#define	MQTTPORTOFST			290
#define	WSPORTOFST				322
#define	MQTTPROTOFST			354
#define	RESERVEDSTR3			386
#define	WIFICLIENTSSIDOFST1		418
#define	WIFICLIENTPSWOFST1		450
#define	WIFICLIENTSSIDOFST2		482
#define	WIFICLIENTPSWOFST2		514
#define	WIFIAPSSIDOFST			546
#define	WIFIAPPPSWOFST			578
#define	WEBUSROFST				610
#define	WEBPSWOFST				642
#define	MQTTUSROFST				674
#define	MQTTPSWOFST				706
//64 byte offsets (fixed long String)
#define MQTTADDROFST			738
#define NTP1ADDROFST			802
#define NTP2ADDROFST			866
#define NTP3ADDROFST			930
//end fixed lenght params
#define FIXEDPARAMSLEN			994
//x byte offsets (variable String)
//--------------------------Fine EEPROM offsets-------------------------------------------

//--------------------------Inizio MQTT array indexes-----------------------------------
//Indici array MQTT[MQTTDIM] dei NOMI dei campi json --> array inr[MQTTDIM] dei VALORI numerici di input (valori numerici di ingresso, 
//flag di segnalazione arrivo in INPUT: richieste di valori di parametri, richieste di esecuzione di comandi.
//valori di risposta in OUTPUT: feedbacks broadcast di arrivo di un nuovo comando, parametro o richiesta, valori di risposta all'esecuzione di un comando, 
//valori di risposta alla richiesta di lettura di un parametro (si noti che tutti questi, non dovendo essere processati dal parser MQTT, 
//potrebbero alternativamente essere hardcoded, è opportuno che stiano quì se il loro nome COINCIDE con quello di un corrispondente flag di richiesta).
#define MQTTUP1				0
#define MQTTDOWN1			1
#define MQTTUP2				2
#define MQTTDOWN2			3
//richiesta parametri
#define MQTTMAC				4
#define MQTTIP				5
#define MQTTMQTTID			6
#define MQTTTIME			7
#define MQTTTEMP			8
#define MQTTMEANPWR			9
#define MQTTPEAKPWR			10
#define MQTTALL				11
#define MQTTDATE			12
#define INSTPWR				13
#define INSTACV				14
#define DOPWRCAL			15
//end user modificable flags

#define MQTTDIM				16
#define USRMODIFICABLEFLAGS 16
//--------------------------Inizio MQTT config array indexes-----------------------------------------------------
//Indici array confJson[CONFDIM] dei NOMI dei campi json dei valori di configurazione --> array confcmd[CONFDIM] 
//dei VALORI stringa di configurazione corrispondenti a flags attivi
//---------------------------------------------------------------------------------------------------------------
//Parametri di IN/OUT (no IN o OUT separatamente) da esporre via MQTT (hanno corrispettivo in array dei flag E VANNO PRIMA SEMPRE)
//Sono aggiornati anche via MQTT (oltre che da eeprom e da form)
//---------------------------------------------------------------------------------------------------------------
//parametri di lunghezza variabile (vanno prima sempre)
#define ONCOND1				0
#define ONCOND2				1
#define ONCOND3				2
#define ONCOND4				3
#define ONCOND5				4
#define ACTIONEVAL			5
//parametri di lunghezza fissa (vanno subito dopo sempre)
#define UTCVAL				6
#define UTCSYNC				7
#define UTCADJ				8
#define UTCSDT				9
#define UTCZONE				10
#define WEBUSR				11
#define WEBPSW				12
#define CALPWR				13
//-------------------------------------------------------------------------------------------------------
//Parametri di stato da non eporre (non hanno corrispettivo in array dei flag, vanno subito dopo sempre)
//Non sono aggiornati via MQTT (solo attraverso eeprom e attraverso form)
//-------------------------------------------------------------------------------------------------------
#define APPSSID				14
#define APPPSW				15
#define CLNTSSID1			16
#define CLNTPSW1			17
#define CLNTSSID2			18
#define CLNTPSW2			19
#define MQTTADDR			20
#define MQTTPORT			21
#define WSPORT				22
#define MQTTPROTO			23
#define MQTTID				24
#define MQTTOUTTOPIC		25
#define MQTTINTOPIC			26
#define MQTTUSR				27
#define MQTTPSW				28
#define THALT1				29
#define THALT2				30
#define THALT3				31
#define THALT4				32
#define STDEL1				33
#define STDEL2				34
#define VALWEIGHT			35
#define	TLENGTH				36
#define	BARRELRAD			37
#define	THICKNESS			38
#define	SLATSRATIO			39
#define SWROLL1				40
#define SWROLL2				41
#define LOCALIP				42
#define NTPADDR1			43
#define NTPADDR2			44
#define PWRMULT				45  
#define ACVOLT				46
//parametri di stato (da non esporre)
#define WIFICHANGED			47
#define CONFLOADED			48
#define MQTTADDRMODFIED		49
#define TOPICCHANGED		50
#define MQTTCONNCHANGED		51
#define	TIMINGCHANGED		52
#define SWACTION1			53
#define SWACTION2			54
#define SWACTION3			55
#define SWACTION4			56
#define CONFDIM				57
#define VARCONFDIM			6
#define EXTCONFDIM			14
#define TOSAVEPARAMS		46
#define PARAMSDIM 			TOSAVEPARAMS + USRMODIFICABLEFLAGS
//--------------------------Fine array indexes-----------------------------------
#if (AUTOCAL_HLW8012 || AUTOCAL_ACS712) 
#define	AUTOCAL		1
#endif

#if (AUTOCAL_HLW8012) 
#define TBASE 			20	
#define MAINPROCSTEP	2
#define ONESEC_STEP		50
#define	LED_STEP		10
#define STOP_STEP		1
#define SEL_PIN			5
#define CF1_PIN			13
#define CF_PIN			14
// Set SEL_PIN to HIGH to sample current
// This is the case for Itead's Sonoff POW, where a
// the SEL_PIN drives a transistor that pulls down
// the SEL pin in the HLW8012 when closed
#define CURRENT_MODE                    HIGH

// These are the nominal values for the resistors in the circuit
#define CURRENT_RESISTOR                0.001
#define VOLTAGE_RESISTOR_UPSTREAM       ( 5 * 470000 ) // Real: 2280k
#define VOLTAGE_RESISTOR_DOWNSTREAM     ( 1000 ) // Real 1.009k
#endif

#if (AUTOCAL_ACS712) 
#define TBASE 			2	
#define MAINPROCSTEP 	40
#define ONESEC_STEP		500
#define STOP_STEP		10
#endif

#if (LARGEFW)
	#if (AUTOCAL_HLW8012) 
		#include "HLW8012.h"
	#endif
	//#include <WiFiUdp.h>
	#include <ESP8266mDNS.h>
	#include <RemoteDebug.h>                  // https://github.com/JoaoLopesF/RemoteDebug
	#include <OneWire.h>                      //  https://www.pjrc.com/teensy/td_libs_OneWire.html
	#include <DallasTemperature.h>            //  https://github.com/milesburton/Arduino-Temperature-Control-Library
	//#include "AsyncPing.h"
	#include <Pinger.h>
	#include "ntp.h"
	extern RemoteDebug telnet;
	void setup_mDNS();
#else
	#define _DEBUGR			0	
	#define _DEBUG1   	    0		
#endif

#if (MCP2317) 
	#include <Wire.h>
	#include <Adafruit_MCP23017.h>
#endif

//#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))
//-----------------------_DEBUG1 MACRO------------------------------------------------------------
//#define F(string_literal) (reinterpret_cast<const __FlashStringHelper *>(PSTR(string_literal)))
//#define PGMT( pgm_ptr ) ( reinterpret_cast< const __FlashStringHelper * >( pgm_ptr ) )

//legge gli ingressi dei tasti già puliti dai rimbalzi

#if (MCP2317) 
    #define leggiTastiLocali()  in[BTN1IN] = !mcp.digitalRead(BTN1U);	\		
		in[BTN2IN] = !mcp.digitalRead(BTN1D);	\
		in[BTN1IN+BTNDIM] = !mcp.digitalRead(BTN2U); 	\
		in[BTN2IN+BTNDIM] = !mcp.digitalRead(BTN2D)	
#else										
	#define leggiTastiLocali()  in[BTN1IN] =!digitalRead(BTN1U);	\
		in[BTN2IN] = !digitalRead(BTN1D);	\
		in[BTN1IN+BTNDIM] = !digitalRead(BTN2U); 	\
		in[BTN2IN+BTNDIM] = !digitalRead(BTN2D)				
#endif
	
#define p(x) 	x + USRMODIFICABLEFLAGS
#define pp(x) 	(x < USRMODIFICABLEFLAGS)?x:x + USRMODIFICABLEFLAGS

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

//Event classes---------------------------------------------------------------------------------------------
class BaseEvnt{
	public:
		byte flagname;
		
		BaseEvnt(byte fname);
		virtual void doaction();
};

class Par{
	public:
		char* parname = "empty";
		unsigned eprom;
		char formfield;
		char partype;
		BaseEvnt *e = NULL;
		
		Par(const char* x = "empty", unsigned y = 2, char z = 'n', char t = 'n', BaseEvnt * u = NULL);
		
		virtual String getParam();
		virtual void writeParam(String);
		void doaction();
};

class MQTTMAC_Evnt: public BaseEvnt{
	public:
		MQTTMAC_Evnt(byte x):BaseEvnt(x){};
		void doaction();
};
class MQTTIP_Evnt: public BaseEvnt{
	public:
		MQTTIP_Evnt(byte x):BaseEvnt(x){};
		void doaction();
};
class MQTTMQTTID_Evnt: public BaseEvnt{
	public:
		MQTTMQTTID_Evnt(byte x):BaseEvnt(x){};
		void doaction();
};
class MQTTTIME_Evnt: public BaseEvnt{
	public:
		MQTTTIME_Evnt(byte x):BaseEvnt(x){};
		void doaction();
};
class MQTTTEMP_Evnt: public BaseEvnt{
	public:
		MQTTTEMP_Evnt(byte x):BaseEvnt(x){};
		void doaction();
};
class MQTTMEANPWR_Evnt: public BaseEvnt{
	public:
		MQTTMEANPWR_Evnt(byte x):BaseEvnt(x){};
		void doaction();
};
class MQTTPEAKPWR_Evnt: public BaseEvnt{
	public:
		MQTTPEAKPWR_Evnt(byte x):BaseEvnt(x){};
		void doaction();
};
#if (AUTOCAL_HLW8012) 
class DOPWRCAL_Evnt: public BaseEvnt{
	public:
		DOPWRCAL_Evnt(byte x):BaseEvnt(x){};
		void doaction();
};
class INSTPWR_Evnt: public BaseEvnt{
	public:
		INSTPWR_Evnt(byte x):BaseEvnt(x){};
		void doaction();
};
class INSTACV_Evnt: public BaseEvnt{
	public:
		INSTACV_Evnt(byte x):BaseEvnt(x){};
		void doaction();
};
#endif
//-----------------------------------------------------------------------
class UTCVAL_Evnt: public BaseEvnt{
	public:
		UTCVAL_Evnt(byte x):BaseEvnt(x){};
		void doaction();
};
class UTCSYNC_Evnt: public BaseEvnt{
	public:
		UTCSYNC_Evnt(byte x):BaseEvnt(x){};
		void doaction();
};
class UTCADJ_Evnt: public BaseEvnt{
	public:
		UTCADJ_Evnt(byte x):BaseEvnt(x){};
		void doaction();
};
class UTCSDT_Evnt: public BaseEvnt{
	public:
		UTCSDT_Evnt(byte x):BaseEvnt(x){};
		void doaction();
};
class UTCZONE_Evnt: public BaseEvnt{
	public:
		UTCZONE_Evnt(byte x):BaseEvnt(x){};
		void doaction();
};
class ACTIONEVAL_Evnt: public BaseEvnt{
	public:
		ACTIONEVAL_Evnt(byte x):BaseEvnt(x){};
		void doaction();
};
class ONCOND1_Evnt: public BaseEvnt{
	public:
		ONCOND1_Evnt(byte x):BaseEvnt(x){};
		void doaction();
};
class ONCOND2_Evnt: public BaseEvnt{
	public:
		ONCOND2_Evnt(byte x):BaseEvnt(x){};
		void doaction();
};
class ONCOND3_Evnt: public BaseEvnt{
	public:
		ONCOND3_Evnt(byte x):BaseEvnt(x){};
		void doaction();
};
class ONCOND4_Evnt: public BaseEvnt{
	public:
		ONCOND4_Evnt(byte x):BaseEvnt(x){};
		void doaction();
};
class ONCOND5_Evnt: public BaseEvnt{
	public:
		ONCOND5_Evnt(byte x):BaseEvnt(x){};
		void doaction();
};
class WEBUSR_Evnt: public BaseEvnt{
	public:
		WEBUSR_Evnt(byte x):BaseEvnt(x){};
		void doaction();
};
class WEBPSW_Evnt: public BaseEvnt{
	public:
		WEBPSW_Evnt(byte x):BaseEvnt(x){};
		void doaction();
};
#if (AUTOCAL_HLW8012)
class CALPWR_Evnt: public BaseEvnt{
	public:
		CALPWR_Evnt(byte x):BaseEvnt(x){};
		void doaction();
};	
#endif

class ParByte : public Par{
	
	public:
		
		ParByte(const char* x = "empty", unsigned y = 2, char z = 'n', char t = 'n', BaseEvnt * u = NULL):Par(x,y,z,t,u){};
		
		String getParam();
		void writeParam(String);
};

class ParInt : public Par{
	
	public:
		
		ParInt(const char* x = "empty", unsigned y = 2, char z = 'n', char t = 'n', BaseEvnt * u = NULL):Par(x,y,z,t,u){};
		
		String getParam();
		void writeParam(String);
		void doaction();
};

class ParLong : public Par{
	
	public:
		
		ParLong(const char* x = "empty", unsigned y = 2, char z = 'n', char t = 'n', BaseEvnt * u = NULL):Par(x,y,z,t,u){};
		
		String getParam();
		void writeParam(String);
};

class ParFloat : public Par{
	
	public:
		
		ParFloat(const char* x = "empty", unsigned y = 2, char z = 'n', char t = 'n', BaseEvnt * u = NULL):Par(x,y,z,t,u){};
		
		String getParam();
		void writeParam(String);
};

class ParStr32 : public Par{
	
	public:
		
		ParStr32(const char* x = "empty", unsigned y = 2, char z = 'n', char t = 'n', BaseEvnt * u = NULL):Par(x,y,z,t,u){};
		
		String getParam();
		void writeParam(String);
};

class ParStr64 : public Par{
	
	public:
		
		ParStr64(const char* x = "empty", unsigned y = 2, char z = 'n', char t = 'n', BaseEvnt * u = NULL):Par(x,y,z,t,u){};
		
		String getParam();
		void writeParam(String);
};
//End of event classes------------------------------------------------------------------------------------------------------------


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
//void mqttCallback(String (&)[PARAMSDIM], String (&)[MQTTDIM]);
void mqttCallback(String &, String &);
void readStatesAndPub(bool all = false);
void readAvgPowerAndPub();
void readPeakPowerAndPub();
void readTempAndPub();
void readMacAndPub();
void readIpAndPub();
void readTimeAndPub();
void readMQTTIdAndPub();
void readParamAndPub(byte, char*);
void readModeAndPub(byte);
void readActModeAndPub(byte);
void readPwrCalAndPub();
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
void saveConf(unsigned);
void loadConf(unsigned);
void saveSingleConf(unsigned);
void saveSingleJson(unsigned);
void printFixedParam(unsigned);
void saveParamFromForm(unsigned);
void setParProps(unsigned, unsigned, char, char, char);
float writeFloatConf(unsigned, float);
float saveFloatConf(unsigned);
long saveLongConf(unsigned);
int saveIntConf(unsigned);
byte saveByteConf(unsigned);
void updtConf(unsigned, String);
unsigned getConfofstFromParamofst(unsigned);
void printOut();
void rstldcnt(byte);
#if (AUTOCAL_HLW8012) 
void calibrate_pwr();
void readIpwrAndPub();
void readIacvoltAndPub();
#endif
/*
//http server callback function prototypes
void handleRoot(ESP8266WebServer (&), String const (&)[PARAMSDIM]);        // function prototypes for HTTP handlers
void handleLogin(ESP8266WebServer (&),  String const (&)[PARAMSDIM], String const (&)[MQTTDIM]);
void handleNotFound(ESP8266WebServer (&));
void handleModify(ESP8266WebServer (&), String (&)[PARAMSDIM], String (&)[MQTTDIM]);
void handleWifiConf(ESP8266WebServer (&),  String const (&)[PARAMSDIM], String const (&)[MQTTDIM]);
void handleSystemConf(ESP8266WebServer (&),  String const (&)[PARAMSDIM], String const (&)[MQTTDIM]);
void handleMQTTConf(ESP8266WebServer (&),  String const (&)[PARAMSDIM], String const (&)[MQTTDIM]);
void handleLogicConf(ESP8266WebServer (&),  String const (&)[PARAMSDIM], String const (&)[MQTTDIM]);
void handleCmd(ESP8266WebServer (&),  String const (&)[PARAMSDIM], String const (&)[MQTTDIM]);
*/
//void handleCmdJson(ESP8266WebServer (&), String&);
bool is_authentified(ESP8266WebServer&);
// function prototypes for HTTP handlers
void initCommon(ESP8266WebServer *,  Par**, String  *, String  *, String  *);

void handleMqttCmd();
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
