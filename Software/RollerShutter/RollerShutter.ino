   #include "common.h"
//End MQTT config------------------------------------------
//inizio variabili e costanti dello schedulatore (antirimbalzo)
#define time_base     	2            // periodo base in millisecondi
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

#if(LARGEFW)
RemoteDebug telnet;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
#endif

WiFiEventHandler stationConnectedHandler;
WiFiEventHandler stationDisconnectedHandler;

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
//End JSON config------------------------------------------
//JSON config----------------------------------------------
//{"OUTSLED":"0","up1":"1","down1":"0","up2":"50","down2":"0", pr1:"12", pr2:"76"}
int ncifre=4;
//array delle proprietà
//l'ordine è importante! Le proprietà verranno ricercate nella stringa in arrivo con questo ordine.
//e salvate in un array con indici a questo corrrispondenti
//l'ordine di trasmissione da remoto dei campi è ininfluente
//I comandi della tapparella devono essere  gli ultimi!
String mqttJson[MQTTJSONDIM]={"up1","down1","up2","down2","temp","avgpwr","peakpwr","all","mac","ip","time","mqttid"};
//Valore iniziale: il suo contenuto viene poi caricato da EEPROM
unsigned int thalt1=5000;
unsigned int thalt2=5000;

String params[PARAMSDIM]={webUsr,webPsw,APSsid,APPsw,clntSsid1,clntPsw1,clntSsid2,clntPsw2,mqttAddr,mqttID,mqttOutTopic,mqttInTopic,mqttUsr,mqttPsw,String(thalt1),String(thalt2),"400","400","0.5","53","3.37","1.5","ip","false","false","false","false","false","false"};
ESP8266WebServer server(80);    	// Create a webserver object that listens for HTTP request on port 80
WebSocketsServer webSocket(81);	    // create a websocket server on port 81
ESP8266HTTPUpdateServer httpUpdater;

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
byte in[NBTN*BTNDIM], outPorts[NBTN*BTNDIM];
byte inr[MQTTJSONDIM];
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
//-----------------------------------------End of prototypes---------------------------------------------------------
inline bool switchdfn(byte val, byte n){
	//n: numero di pulsanti
	val = (val>0);								//SEMPLICE!!!!
	bool changed = (val != dprecval2[n]);
	dprecval2[n] = val;            // valore di val campionato al loop precedente 
	return changed;
}

inline bool gatedfn(float val, byte n, float rnd){
	//n: numero di porte
	bool changed = (val > asyncBuf[n] - rnd && val < asyncBuf[n] + rnd);
	asyncBuf[n] = val;            // valore di val campionato al loop precedente 
	return changed;
}

inline byte tapLogic(byte n){
	return (switchLogic(0,n) + switchLogic(1,n));
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

float getTemperature(){
	float temp = -127;
#if(LARGEFW)
	DS18B20.requestTemperatures(); 
	unsigned short cnt = 0;
	do{
		temp = DS18B20.getTempCByIndex(0);
		DEBUG_PRINT("Temperature: ");
		DEBUG_PRINTLN(temp);
		cnt++;		
	}while((temp == 85.0 || temp == (-127.0)) && cnt < 3);
#endif
	return temp;
}

void scriviOutDaStato(){
#if (SCR)
	 digitalWrite(OUT1DD,(outLogic[ENABLES] && (outLogic[DIRS]==HIGH)));	
	 digitalWrite(OUT1EU,(outLogic[ENABLES] && (outLogic[DIRS]==LOW)));		
	 digitalWrite(OUT2DD,(outLogic[ENABLES+STATUSDIM] && (outLogic[DIRS+STATUSDIM]==HIGH)));	
	 digitalWrite(OUT2EU,(outLogic[ENABLES+STATUSDIM] && (outLogic[DIRS+STATUSDIM]==LOW)));		
	 isrun[0] = (outLogic[ENABLES]==HIGH);					
	 isrun[1] = (outLogic[ENABLES+STATUSDIM]==HIGH);	
#else		
	 digitalWrite(OUT1EU,outLogic[ENABLES]);	
	 digitalWrite(OUT1DD,outLogic[DIRS]);		
	 digitalWrite(OUT2EU,outLogic[ENABLES+STATUSDIM]);		
	 digitalWrite(OUT2DD,outLogic[DIRS+STATUSDIM]);			
	 isrun[0] = (outLogic[ENABLES]==HIGH);					
	 isrun[1] = (outLogic[ENABLES+STATUSDIM]==HIGH);			 
#endif
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
	  WiFi.softAP((params[APPSSID]).c_str());
	  //WiFi.softAP("cacca9");
	  //interrupts();
	  //DEBUG_PRINT(F("Setting soft-AP ... "));
	  //DEBUG_PRINTLN(WiFi.softAP((params[APPSSID]).c_str()) ? F("Ready") : F("Failed!"));
	  //delay(500); // Without delay I've seen the IP address blank
	  delay(1000);
	  params[LOCALIP] = WiFi.softAPIP().toString();
	  Serial.print(F("Soft-AP IP address = "));
	  Serial.println(params[LOCALIP]);
	  //wifi_softap_dhcps_stop();
  }else{
	  //noInterrupts ();
	  WiFi.softAP((params[APPSSID]).c_str());
	  //WiFi.softAP(ssid, password);
	  //interrupts();
	  //DEBUG_PRINT(F("Setting soft-AP ... "));
	  //DEBUG_PRINTLN(WiFi.softAP((params[APPSSID]).c_str()) ? F("Ready") : F("Failed!"));
	  delay(1000);
	  params[LOCALIP] = WiFi.softAPIP().toString();
	  Serial.print(F("Soft-AP IP address = "));
	  Serial.println(params[LOCALIP]);
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
	wifindx = wifindx*2;  //client1 e client2 hanno indici contigui nell'array params
	// We start by connecting to a WiFi network
	Serial.println(F("Connecting to "));
	Serial.println(params[CLNTSSID1+wifindx]);
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
		if ((params[CLNTSSID1+wifindx]).c_str() != "") {
			//noInterrupts();
			Serial.print(F("Begin status: "));
			Serial.println(WiFi.begin((params[CLNTSSID1+wifindx]).c_str(), (params[CLNTPSW1+wifindx]).c_str()));
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
		params[LOCALIP] = WiFi.localIP().toString();
	}	

	Serial.println (F("\n******************* begin ***********"));
    WiFi.printDiag(Serial);
	Serial.println (F("\n******************* end ***********"));

	//WiFi.enableAP(true);
  //}
}
#if (LARGEFW)
void setup_mDNS() {
	if (MDNS.begin((params[MQTTID]).c_str())) {              // Start the mDNS responder for esp8266.local
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
	mqttClient = new MQTT((params[MQTTID]).c_str(),(params[MQTTADDR]).c_str(), 1883);
	interrupts ();
    DEBUG_PRINTLN(F("Registro i callback dell'MQTT."));
	DEBUG_PRINT(F("Attempting MQTT connection to: "));
	DEBUG_PRINT(params[MQTTADDR]);
	DEBUG_PRINT(F(", with ClientID: "));
	DEBUG_PRINT(params[MQTTID]);
	DEBUG_PRINTLN(F(" ..."));
	//mqttClient->setClientId(params[MQTTID]);
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
			mqttClient->subscribe(params[MQTTINTOPIC]);
			mqttClient->publish(params[MQTTOUTTOPIC], params[MQTTID]);
			//mqttConnected=true;
		});
		mqttClient->onDisconnected([]() {
			//DEBUG_PRINTLN("MQTT disconnected.");
		DEBUG_PRINTLN(F("MQTT: onDisconnected([]() dice mi sono disconnesso."));
			//mqttConnected=false;
		});
		DEBUG_PRINTLN(F("MQTT: Eseguo la prima connect."));
		mqttClient->setUserPwd((params[MQTTUSR]).c_str(), (params[MQTTPSW]).c_str());
		noInterrupts ();
		mqttClient->connect();
		interrupts ();
		delay(50);
		mqttClient->subscribe(params[MQTTINTOPIC]);
		mqttClient->publish(params[MQTTOUTTOPIC], params[MQTTID]);
	}
}

void mqttCallback(String &topic, String &response) {
	//funzione eseguita dal subscriber all'arrivo di una notifica
	//decodifica la stringa JSON e la trasforma nel nuovo vettore degli stati
	//il vettore corrente degli stati verrà sovrascritto
	//applica la logica ricevuta da remoto sulle uscite locali (led)
    
	int v;
#if (DEBUG || DEBUGR)	
	DEBUG_PRINT(F("Message arrived on topic: ["));
	DEBUG_PRINT(topic);
	DEBUG_PRINT(F("], "));
	DEBUG_PRINTLN(response);
#endif	
	//v = parseJsonFieldToInt(response, mqttJson[0], ncifre);
	//digitalWrite(OUTSLED, v); 
   
	parseJsonFieldArrayToInt(response, inr, mqttJson, ncifre, MQTTJSONDIM,0);
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
		readStatesAndPub();
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
  s+=mqttJson[MQTTJSONUP1]+twodot+(outLogic[ENABLES] && (outLogic[DIRS]==LOW))+comma; 	//up1 DIRS=HIGH
  s+=mqttJson[MQTTJSONDOWN1]+twodot+(outLogic[ENABLES] && (outLogic[DIRS]==HIGH))+comma;    //down1  DIRS=LOW
  s+=mqttJson[MQTTJSONUP2]+twodot+(outLogic[ENABLES+STATUSDIM] && (outLogic[DIRS+STATUSDIM]==LOW))+comma;	//up2 
  s+=mqttJson[MQTTJSONDOWN2]+twodot+(outLogic[ENABLES+STATUSDIM] && (outLogic[DIRS+STATUSDIM]==HIGH))+comma;    //down2
  s+= (String) "pr1"+twodot+String(round(calcLen(0)))+comma;		//pr1
  s+= (String) "pr2"+twodot+String(round(calcLen(1)))+comma;		//pr2
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
		s+=mqttJson[MQTTJSONTEMP]+twodot+String(asyncBuf[GTTEMP])+comma;
		s+=mqttJson[MQTTJSONMEANPWR]+opensqr+String(asyncBuf[GTMEANPWR1])+comma+String(asyncBuf[GTMEANPWR2])+closesqr2;
		s+=mqttJson[MQTTJSONPEAKPWR]+opensqr+String(asyncBuf[GTPEAKPWR1])+comma+String(asyncBuf[GTPEAKPWR2])+"\"],";
  }else{
		s+=enda;
  }
  publishStr(s);
}

void readAvgPowerAndPub(){
  //DEBUG_PRINTLN(F("\nreadPowerAndPub")); 
  String s=openbrk;
  s+=mqttJson[MQTTJSONMEANPWR]+opensqr+String(asyncBuf[GTMEANPWR1])+comma+String(asyncBuf[GTMEANPWR2])+closesqr3;
  publishStr(s);
}

void readPeakPowerAndPub(){
  //DEBUG_PRINTLN(F("\nreadPowerAndPub")); 
  String s=openbrk;
  s+=mqttJson[MQTTJSONPEAKPWR]+opensqr+String(asyncBuf[GTPEAKPWR1])+comma+String(asyncBuf[GTPEAKPWR2])+closesqr3;
  publishStr(s);
}

void readTempAndPub(){
  //DEBUG_PRINTLN(F("\nreadTempAndPub")); 
  String s=openbrk;
  //char sd[300];
  s+=mqttJson[MQTTJSONTEMP]+twodot+String(asyncBuf[GTTEMP])+enda;
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

void publishStr(String &str){
  //pubblica sul broker la stringa JSON
  //informazioni mittente
  str+="\"";
  str+=mqttJson[MQTTJSONTIME]+twodot+String(millis())+comma;
  str+=mqttJson[MQTTJSONMAC]+twodot+String(WiFi.macAddress())+comma;
  str+=mqttJson[MQTTJSONIP]+twodot+WiFi.localIP().toString()+comma;
  str+=mqttJson[MQTTJSONMQTTID]+twodot+params[MQTTID]+closebrk;
   
  if(mqttClient==NULL){
	  DEBUG_PRINTLN(F("ERROR on publishStr MQTT client is not allocated."));
  }
  else
  {
	  mqttClient->publish(params[MQTTOUTTOPIC], str);
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
  edelay[0]=(params[STDEL1]).toInt();
  edelay[1]=(params[STDEL2]).toInt();
  initTapparellaLogic(in,inr,outLogic,params,first);
#if (AUTOCAL)  
  resetAVGStats(0,0);
  resetAVGStats(0,1);
#endif  
}

void setup() {
  dosmpl = false;
  zeroCnt = 0;
  mqttcnt = 0;
  mqttofst = 2;
  //inizializza la seriale
  Serial.begin(115200);
  //importante per il debug del WIFI!
  //Serial.setDebugOutput(true);
  //WiFi.printDiag(Serial);
  //carica la configurazione dalla EEPROM
  //DEBUG_PRINTLN(F("Carico configurazione."));
  initCommon(&server,params,mqttJson);
  delay(100);
  //WiFi.mode(WIFI_AP);
  //setup_AP(true);
  loadConfig();
  delay(100);
  //WiFi.softAPdisconnect(false);
  //WiFi.persistent(true);
  delay(100);
  //WiFi.enableAP(false);
  //WiFi.persistent(false);
  wifiConn = false;
  startWait=false;
  endWait=true;
  //WiFi.softAPdisconnect(false);
  
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
  //setTimerState(wfs, CONNSTATSW);
#if(LARGEFW)
  telnet.begin((params[MQTTID]).c_str()); // Initiaze the telnet server
  telnet.setResetCmdEnabled(true); // Enable the reset command
  telnet.setCallBackProjectCmds(&processCmdRemoteDebug);
  DEBUG_PRINTLN(F("Activated remote debug"));
#endif  
  DEBUG_PRINTLN(F("Inizializzo i pulsanti."));
  initdfn(LOW, 0);  //pull DOWN init (in realtà è un pull up, c'è un not in ogni ingresso sui pulsanti)
  initdfn(LOW, 1);
  initdfn(LOW, 2);
  initdfn(LOW, 3);
  initdfn(LOW, 4);
  //initdfn((byte) WL_DISCONNECTED, CONNSTATSW);
  //Timing init
  setupTimer((params[THALT1]).toInt(),TMRHALT);				//function timer switch1
  setupTimer((params[THALT2]).toInt(),TMRHALT+TIMERDIM);	//function timer switch2 
  setupTimer(RSTTIME*1000,RESETTIMER);						//special timer btn1 
  setupTimer(APOFFT*1000,APOFFTIMER);						//special timer btn1
#if (AUTOCAL)
  weight[0] =  (params[VALWEIGHT]).toFloat();
  weight[1] = 1 - weight[0];
  params[STDEL1] = TENDCHECK*1000;
  params[STDEL2] = TENDCHECK*1000;
  //setThresholdUp((params[TRSHOLD1]).toFloat(), 0);
  //setThresholdUp((params[TRSHOLD2]).toFloat(), 1);
#endif
  //DEBUG_PRINTLN("Inizializzo i timers a zero.");
  //initTimers();
  //DEBUG_PRINTLN("Avvio il client MQTT.");
  yield();
  startWebSocket();
  delay(100);
  //avvia il client MQTT
  //DEBUG_PRINTLN("Registro i callback del server web.");
  //inizializza lista di httpCallback e corrispondenti URI
  delay(100);
  httpSetup();
  delay(100);
   
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
  for(int i=0;i<MQTTJSONDIM;i++)
		inr[i]=LOW;
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
  //initTapparellaLogic(in,inr,outLogic,(params[THALT1]).toInt(),(params[THALT2]).toInt(),(params[STDEL1]).toInt(),(params[STDEL2]).toInt());
  //esp_log_set_vprintf(_log_vprintf);
#if (DEBUG || DEBUGR)  
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
  Serial.print("\nStation connected, IP: ");
  Serial.println(WiFi.localIP());
  
  swcount = 0;
  DEBUG_PRINTLN(F(" OK"));
  DEBUG_PRINTLN(F("Last reset reason: "));
  DEBUG_PRINTLN(ESP.getResetReason());
  
  if(WiFi.status() == WL_CONNECTED)
		mqttReconnect();
}

void httpSetup(){
  //DEBUG_PRINTLN(F("Registro handleRootExt."));
  server.on("/", HTTP_GET, handleRoot);        // Call the 'handleRoot' function when a client requests URI "/"
  //DEBUG_PRINTLN(F("Registro handleLoginExt."));
  server.on("/login", HTTP_POST, handleLogin); // Call the 'handleLogin' function when a POST request is made to URI "/login"
  //DEBUG_PRINTLN(F("Registro handleModifyExt."));
  server.on("/modify", HTTP_POST, handleModify);
  server.on("/systconf", HTTP_POST, handleSystemConf);
  server.on("/wificonf", HTTP_POST, handleWifiConf);
  server.on("/mqttconf", HTTP_POST, handleMQTTConf);
  server.on("/logicconf", HTTP_POST, handleLogicConf);
  server.on("/cmd", HTTP_GET, handleCmd);
  //server.on("/cmdjson", handleCmdJsonExt);
  //DEBUG_PRINTLN(F("Registro handleNotFoundExt."));
  server.onNotFound(handleNotFound);           // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"
  //avvia il server HTTP*/
  //DEBUG_PRINTLN("Avvio il server web.");
  //here the list of headers to be recorded
  const char * headerkeys[] = {"User-Agent","Cookie"} ;
  size_t headerkeyssize = sizeof(headerkeys)/sizeof(char*);
  //ask server to track these headers
  server.collectHeaders(headerkeys, headerkeyssize );
  //avvia il responder mDNS
  //setup_mDNS();
  //OTA web page handler linkage
  httpUpdater.setup(&server);
  //start HTTP server
  server.begin();
  DEBUG_PRINTLN("HTTP server started");
#if(LARGEFW)
  MDNS.addService(F("http"), F("tcp"), 80); 
  DEBUG_PRINT("HTTPUpdateServer ready! Open http://");
  DEBUG_PRINT(params[MQTTID]);
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
  webSocket.loop();
  server.handleClient();  // Listen for HTTP requests from clients
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
  if((millis()-prec) > time_base) //schedulatore per tempo base 
  {	
	prec = millis();
	//prec += time_base;
	//calcolo dei multipli interi del tempo base
	step = (step + 1) % nsteps;
	
#if (AUTOCAL) 
	if(dosmpl){//solo a motore in moto
		currentPeakDetector();
	}
	//---------------------------------------------------------------------
	// 20 msec scheduler
	//---------------------------------------------------------------------
	if(!(step % 10)){		
		automaticStopManager();
	}//END 20ms scheduler--------------------------------------------------
#endif

	//---------------------------------------------------------------------
	// 1 sec scheduler
	//---------------------------------------------------------------------
	if(!(step % 500)){	
		if(!(isrun[0] || isrun[1])){//solo a motore fermo! Per evitare contemporaneità col currentPeakDetector
			aggiornaTimer(RESETTIMER);
			aggiornaTimer(APOFFTIMER);
			pushCnt++;
			DEBUG_PRINT(F("\n------------------------------------------"));
			DEBUG_PRINT(F("\nMean sensor: "));
			DEBUG_PRINT(m);
			DEBUG_PRINT(F(" - Conn stat: "));
			stat = WiFi.status();
			DEBUG_PRINT(stat);
			wifiConn = (stat == WL_CONNECTED);	
			DEBUG_PRINT(F(" - Wifi mode: "));
			DEBUG_PRINTLN(WiFi.getMode());
			
			sensorStatePoll();
			wifiFailoverManager();
			MQTTReconnectManager();
		}	
		paramsModificationPoll();
	}//END 1 sec scheduler-----------------------------------------------------
	
	//---------------------------------------------------------------------
	// 60 msec scheduler
	//---------------------------------------------------------------------
	if(!(step % 30)){		
		//leggi ingressi locali e mette il loro valore sull'array val[]
		leggiTastiLocali();
		leggiTastiLocaliRemoto();
		//se uno dei tasti delle tapparelle è stato premuto
		//o se è arrivato un comando dala mqttCallback
		if(tapLogic(TAP1) == 1 ||  tapLogic(TAP2) == 1){ 
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
		leggiTastiRemoti();
		//------------------------------------------------------------------------------------------------------------
		//Finestra idle di riconnessione (necessaria se il loop è molto denso di eventi e il wifi non si aggancia!!!)
		//------------------------------------------------------------------------------------------------------------
		//sostituisce la bloccante WiFi.waitForConnectResult();	
		//if((wifiConn == false && !(isrun[0] || isrun[1]))){
			//DEBUG_PRINTLN(F("to ESP stack... "));
			//delay(30);//give 30ms to the ESP stack for wifi connect
		//	wifiConn = (WiFi.status() == WL_CONNECTED);
		//}
		//------------------------------------------------------------------------------------------------------------
	}//END 60ms scheduler------------------------------------------------------------------------------------
  }//END Time base (2 msec) main scheduler------------------------------------------------------------------------  
  //POST SCHEDULERS ACTIONS-----------------
#if(LARGEFW)
  telnet.handle();
#endif  
  yield();	// Give a time for ESP8266
}//END loop


inline void leggiTastiRemoti(){
	//gestione eventi MQTT sui sensori
	if(inr[MQTTJSONTEMP]){
		inr[MQTTJSONTEMP] = LOW;
		readTempAndPub();
	}
	if(inr[MQTTJSONMEANPWR]){
		inr[MQTTJSONMEANPWR] = LOW;
		readAvgPowerAndPub();
	}
	if(inr[MQTTJSONPEAKPWR]){
		inr[MQTTJSONPEAKPWR] = LOW;
		readPeakPowerAndPub();
	}
	if(inr[MQTTJSONMAC]){
		inr[MQTTJSONMAC] = LOW;
		readMacAndPub();
	}
	if(inr[MQTTJSONIP]){
		inr[MQTTJSONIP] = LOW;
		readIpAndPub();
	}
	if(inr[MQTTJSONTIME]){
		inr[MQTTJSONTIME] = LOW;
		readTimeAndPub();
	}
	if(inr[MQTTJSONMQTTID]){
		inr[MQTTJSONMQTTID] = LOW;
		readMQTTIdAndPub();
	}
}

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

inline void automaticStopManager(){
	if((isrun[0] || isrun[1])){
			//automatic stop manager
			dd = maxx - minx;
			//EMA calculation
			//ACSVolt = (double) ex/2.0;
			//peak = (double) ex/2.0;
			//reset of peak sample value
			DEBUG_PRINT(F("\n("));
			DEBUG_PRINT(0);
			DEBUG_PRINT(F(") ADC enable: "));
			DEBUG_PRINT(dosmpl);
			
			if(isrun[0] && dosmpl){
				DEBUG_PRINT(0);
				if(isrundelay[0] == 0){
					ex[0] = dd*EMA + (1.0 - EMA)*ex[0];
					DEBUG_PRINT(F("\n("));
					DEBUG_PRINT(0);
					DEBUG_PRINT(F(") minx sensor: "));
					DEBUG_PRINT(minx);
					DEBUG_PRINT(F(" - maxx sensor: "));
					DEBUG_PRINT(maxx);
					DEBUG_PRINT(F(" - Mean sensor: "));
					DEBUG_PRINT(m);
					DEBUG_PRINT(F(" - Peak: "));
					DEBUG_PRINT(ex[0]);
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
							blocked[0] = secondPress(0,40);
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
							blocked[1] = secondPress(40,1);
							scriviOutDaStato();
						}else if(chk[1] == 2){
							blocked[1] = secondPress(40,1);
							scriviOutDaStato();
							blocked[1] = 1;
						}else if(chk[1] == 1){
							ex[1] = getAVG(1);
							//inizio conteggio timer di posizionamento
							startEndOfRunTimer(1);
						}
						readStatesAndPub();
						yield();
					}
				}else{
					DEBUG_PRINT(F("\n("));
					DEBUG_PRINT(1);
					DEBUG_PRINT(F(") aspetto: "));
					DEBUG_PRINT(isrundelay[1]);
					DEBUG_PRINT(F(" - minx sensor: "));
					DEBUG_PRINT(minx);
					DEBUG_PRINT(F(" - maxx sensor: "));
					DEBUG_PRINT(maxx);
					DEBUG_PRINT(F(" - Peak: "));
					DEBUG_PRINT(ex[1]);
					isrundelay[1]--;
					ex[1] = dd;
					DEBUG_PRINT(F(" - dd: "));
					DEBUG_PRINT(dd);
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
			DEBUG_PRINT(F("\n------------------------------------------------------------------------------------------"));
		}else{
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
				DEBUG_PRINT(F("\nZero peak sensor: "));
				DEBUG_PRINT(x);
				DEBUG_PRINT(F(" - Zero mean sensor: "));
				DEBUG_PRINT(m);
			}
		}	
}

inline void sensorStatePoll(){
	//sensor variation polling management
	//on events basis push of reports
	if(gatedfn(getTemperature(),GTTEMP, TEMPRND)){
		readTempAndPub();
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
			params[LOCALIP] = WiFi.localIP().toString();
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
				DEBUG_PRINT(F("MQTT check : "));
				DEBUG_PRINT(mqttStat);
				if(!mqttStat){
					DEBUG_PRINT(F("\nMQTT dice non sono connesso."));
					mqttConnected=false;
				}	
				else{
					DEBUG_PRINT(F("\nMQTT  dice sono connesso. Local IP: "));
					DEBUG_PRINT(WiFi.localIP());
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
					//mqttClient->setUserPwd((params[MQTTUSR]).c_str(), (params[MQTTPSW]).c_str());
				}
			}
}

inline void paramsModificationPoll(){
	//actions on parametrs saving
		//is else if per gestione priorità, l'ordine è importante! vanno fatti in momenti successivi
		if(params[WIFICHANGED]=="true"){
			params[WIFICHANGED]="false";
			wifindx=0;
			Serial.println(F("Doing WiFi disconnection"));
			WiFi.persistent(false);
			WiFi.disconnect(false);
			WiFi.mode(WIFI_OFF);    
			//WiFi.mode(WIFI_STA);
			wifindx = 0;
		}
	
		if(params[MQTTADDRMODFIED]=="true"){
			params[MQTTADDRMODFIED]="false";
			DEBUG_PRINTLN(F("params[MQTTADDRMODFIED] eseguo la reconnect()"));
			mqttReconnect();
		}else if(params[MQTTCONNCHANGED]=="true"){
			params[MQTTCONNCHANGED]="false";
			if(mqttClient==NULL){
				DEBUG_PRINTLN(F("ERROR params[TOPICCHANGED]! MQTT client is not allocated."));
				mqttReconnect();
			}
			else
			{
			#if (DEBUG || DEBUGR)	
				DEBUG_PRINTLN(F("MQTTCONNCHANGED! Eseguo la setUserPwd() con usr "));
				DEBUG_PRINTLN(params[MQTTUSR]);
				DEBUG_PRINTLN(F(" e psw "));
				DEBUG_PRINTLN(params[MQTTPSW]);
			#endif		
				mqttClient->setUserPwd((params[MQTTUSR]).c_str(), (params[MQTTPSW]).c_str());
				DEBUG_PRINTLN(F("MQTTCONNCHANGED! Eseguo la connect() ..."));
				mqttClient->connect();
			#if (DEBUG || DEBUGR)	
				DEBUG_PRINTLN(F("MQTTCONNCHANGED! Eseguo la subscribe() con "));
				DEBUG_PRINTLN(params[MQTTINTOPIC]);
				DEBUG_PRINTLN(F("..."));
			#endif	
				mqttClient->subscribe(params[MQTTINTOPIC]);
			#if (DEBUG || DEBUGR)	
				DEBUG_PRINTLN(F("MQTTCONNCHANGED! publish(): "));
				DEBUG_PRINTLN(params[MQTTOUTTOPIC]);
				DEBUG_PRINTLN(F(" Intopic: "));
				DEBUG_PRINTLN(params[MQTTINTOPIC]);
			#endif		
				mqttClient->publish(params[MQTTOUTTOPIC], params[MQTTID]);
			}
		}else if(params[TOPICCHANGED]=="true"){
			params[TOPICCHANGED]="false";
			if(mqttClient==NULL){
				DEBUG_PRINTLN(F("ERROR params[TOPICCHANGED]! MQTT client is not allocated."));
				mqttReconnect();
			}
			else
			{
			#if (DEBUG || DEBUGR)	
				DEBUG_PRINTLN(F("TOPICCHANGED! Outtopic: "));
				DEBUG_PRINTLN(params[MQTTOUTTOPIC]);
				DEBUG_PRINTLN(F(" Intopic: "));
				DEBUG_PRINTLN(params[MQTTINTOPIC]);
				DEBUG_PRINTLN(F("TOPICCHANGED! Eseguo la subscribe() con "));
				DEBUG_PRINTLN(params[MQTTINTOPIC]);
				DEBUG_PRINTLN(F("..."));
			#endif		
				mqttClient->subscribe(params[MQTTINTOPIC]);
			#if (DEBUG || DEBUGR)	
				DEBUG_PRINTLN(F("TOPICCHANGED! Eseguo la publish() con "));
				DEBUG_PRINTLN(params[MQTTOUTTOPIC]);
				DEBUG_PRINTLN(F(" ..."));
			#endif		
				mqttClient->publish(params[MQTTOUTTOPIC], params[MQTTID]);
			}
		}
}

//-----------------------------------------------INIZIO TIMER----------------------------------------------------------------------
//azione da compiere allo scadere di uno dei timer dell'array	
void onElapse(byte n){
	DEBUG_PRINT(F("\nElapse timer n: "));
	DEBUG_PRINT(n);
	DEBUG_PRINT(F("  con stato: "));
	DEBUG_PRINTLN(getGroupState(n));
	if(n == TMRHALT || n == TMRHALT+TIMERDIM) //se è scaduto il timer di attesa o di blocco  (0,1) --> state n
	{   
		DEBUG_PRINT(F("\nCount value: "));
		DEBUG_PRINTLN(getCntValue(n));
		if(getCntValue(n) == 1){
			int toffset=n*TIMERDIM;
			if(getGroupState(n)==3){ //il motore e in moto cronometrato scaduto (timer di blocco scaduto)
				DEBUG_PRINTLN(F("stato 0: il motore va in stato fermo da fine corsa (TIMER ELAPSED!)"));
				secondPress(n);
				//comanda gli attuatori per fermare (non lo fa il loop stavolta!)
				scriviOutDaStato();
				//pubblica lo stato finale su MQTT (non lo fa il loop stavolta!)
				readStatesAndPub();
			}else if(getGroupState(n)==1){	//se il motore era in attesa di partire (timer di attesa scaduto)
				DEBUG_PRINTLN(F("onElapse:  timer di attesa scaduto"));
				startEngineDelayTimer(n);
				//adesso parte...
				scriviOutDaStato();
			}
#if (!AUTOCAL)	
			else if(getGroupState(n)==2){//se il motore è in moto a vuoto
				DEBUG_PRINTLN(F("onElapse:  timer di corsa a vuoto scaduto"));
				///setGroupState(3,n);	//il motore va in moto cronometrato
				startEndOfRunTimer(n);
				//pubblica lo stato di UP o DOWN attivo su MQTT (non lo fa il loop stavolta!)
				readStatesAndPub();
			}
#else
			else if(getGroupState(n)==2){//se il motore è in moto a vuoto
				DEBUG_PRINTLN(F("onElapse:  timer di check pressione su fine corsa scaduto"));
				secondPress(n);
				//comanda gli attuatori per fermare (non lo fa il loop stavolta!)
				scriviOutDaStato();
			}
#endif
		}else if(getCntValue(n) > 1){
			if(n == 0){
				DEBUG_PRINTLN(F("onElapse:  timer 1 dei servizi a conteggio scaduto"));
				if(testCntEvnt(3,CNTSERV1)){
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
					//params[LOCALIP] = WiFi.softAPIP().toString();
					//MDNS.notifyAPChange()();
					//wifi_softap_dhcps_start();
					//delay(100);
					//WiFi.printDiag(Serial);
					//MDNS.update();
					setGroupState(0,n%2);
				}else if(testCntEvnt(4,CNTSERV1)){
					DEBUG_PRINTLN(F("-----------------------------"));
					DEBUG_PRINTLN(F("Rebooting ESP without reset of configuration"));
					DEBUG_PRINTLN(F("-----------------------------"));
					ESP.eraseConfig(); //do the erasing of wifi credentials
					ESP.restart();
				}else if(testCntEvnt(5,CNTSERV1)){
					DEBUG_PRINTLN(F("-----------------------------"));
					DEBUG_PRINTLN(F("ATTIVATA CALIBRAZIONE MANUALE BTN 1"));
					DEBUG_PRINTLN(F("-----------------------------"));
					manualCalibration(0); //BTN1
				}else if(testCntEvnt(8,CNTSERV1)){
					DEBUG_PRINTLN(F("-----------------------------"));
					DEBUG_PRINTLN(F("Reboot ESP with reset of configuration"));
					DEBUG_PRINTLN(F("-----------------------------"));
					rebootSystem();
				}else{
					setGroupState(0,n%2);
				}
				//DEBUG_PRINT(F("Resettato contatore dei servizi: "));
				resetCnt(CNTSERV1);
			}else{
				DEBUG_PRINTLN(F("onElapse:  timer 2 dei servizi a conteggio scaduto"));
				if(testCntEvnt(5,CNTSERV2)){
					DEBUG_PRINTLN(F("-----------------------------"));
					DEBUG_PRINTLN(F("ATTIVATA CALIBRAZIONE MANUALE BTN 2"));
					DEBUG_PRINTLN(F("-----------------------------"));
					manualCalibration(1); //BTN2
				}else{
					setGroupState(0,n%2);
				}
				//DEBUG_PRINT(F("Resettato contatore dei servizi: "));
				resetCnt(CNTSERV2);
			}
		}
	}else if(n == RESETTIMER)
		{
			rebootSystem();
	}else if(n == APOFFTIMER){
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
	params[haltPrm[n]] = String(app);
	//initTapparellaLogic(in,inr,outLogic,(params[THALT1]).toInt(),(params[THALT2]).toInt(),(params[STDEL1]).toInt(),(params[STDEL2]).toInt(),BTNDEL1,BTNDEL2);
	setTapThalt((params[THALT1 + n]).toInt(), n);
	DEBUG_PRINTLN(F("-----------------------------"));
#if (AUTOCAL)
	calAvg[n] = getAVG(n);
	weight[0] = (double) calAvg[0] / (calAvg[0] +  calAvg[1]);
	weight[1] = (double) calAvg[1] / (calAvg[0] +  calAvg[1]);
	params[VALWEIGHT] = String(weight[0]);
	updateUpThreshold(n);
	//params[TRSHOLD1 + n] = String(getThresholdUp(n));
	//setThresholdUp((params[TRSHOLD1 + n]).toFloat(), n);
#endif
	EEPROM.begin(EEPROMPARAMSLEN);
	
	EEPROMWriteStr(VALWEIGHTOFST,(params[VALWEIGHT]).c_str());
	EEPROM.commit();
	DEBUG_PRINT(F("Modified current weight "));
	DEBUG_PRINTLN(params[VALWEIGHT]);
	
	EEPROMWriteInt(haltOfs[n], app);
	EEPROM.commit();
	DEBUG_PRINT(F("Modified THALT "));
	DEBUG_PRINTLN(haltPrm[n]);
	DEBUG_PRINT(F(": "));
	DEBUG_PRINTLN(params[haltPrm[n]]);
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
	inr[BTN2IN + btn*BTNDIM] = 201;			//codice comando attiva calibrazione
	
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
	EEPROM.begin(EEPROMPARAMSLEN);
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
		DEBUG_PRINTLN(params[MQTTID]);
	}else{
		DEBUG_PRINT(F("\nComandi disponibili: "));
		DEBUG_PRINT(F("\nshowconf, reboot, reset, calibrate1, calibrate2, apmodeon, scanwifi, getip, getmqttstat, getadczero, gettemp, getpower, getmac, gettime, getmqttid\n"));
	}
	//telnet.flush();
}
#endif


#if (DEBUG || DEBUGR)	
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

#if (DEBUG || DEBUGR)	
void testFlash(){
  uint32_t realSize = ESP.getFlashChipRealSize();
  uint32_t ideSize = ESP.getFlashChipSize();
  FlashMode_t ideMode = ESP.getFlashChipMode();
  char s[25];
  
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
