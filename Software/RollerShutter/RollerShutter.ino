   #include "common.h"
//End MQTT config------------------------------------------
//inizio variabili e costanti dello schedulatore (antirimbalzo)
#define time_base     	2            // periodo base in millisecondi
#define nsteps          12000        // numero di fasi massimo di un periodo generico

//stats variables
#if (AUTOCAL)
byte stat;
double ACSVolt;
unsigned int mVperAmp = 100;   // 185 for 5A, 100 for 20A and 66 for 30A Module
double ACSVoltage = 0;
double peak = 0;
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
int minx = 1024;
int maxx = 0;
int x;
int dd = 0;
float m;
double ex = 0;
double calAvg[2] = {0,0};
double weight[2] = {0,0};
short chk[2]={0,0};
//unsigned x20ms, x60ms, x1s;
#endif
bool isrun[2]={false,false};
bool dosmpl = false;
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

RemoteDebug telnet;
WiFiEventHandler stationConnectedHandler;
WiFiEventHandler stationDisconnectedHandler;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
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
int ncifre=3;
//array delle proprietà
//l'ordine è importante! Le proprietà verranno ricercate nella stringa in arrivo con questo ordine.
//e salvate in un array con indici a questo corrrispondenti
//l'ordine di trasmissione da remoto dei campi è ininfluente
//I comandi della tapparella devono essere  gli ultimi!
String mqttJson[MQTTJSONDIM]={"up1","down1","up2","down2","temp","avgpwr","peakpwr","all"};
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

inline float getAmpRMS(float ACSVolt){
	ACSVolt = (double) (ACSVolt * 5.0) / 1024.0;
	VRMS = ACSVolt * 0.707;
	AmpsRMS = (double) (VRMS * 1000) / mVperAmp;
	if((AmpsRMS > -0.015) && (AmpsRMS < 0.008)){ 
		AmpsRMS = 0.0;
	}
	return AmpsRMS;
}

inline float getTemperature(){
	DS18B20.requestTemperatures(); 
	float temp;
	unsigned short cnt = 0;
	do{
		temp = DS18B20.getTempCByIndex(0);
		DEBUG_PRINT("Temperature: ");
		DEBUG_PRINTLN(temp);
		cnt++;		
	}while((temp == 85.0 || temp == (-127.0)) && cnt < 3);
	return temp;
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

void setup_mDNS() {
	if (MDNS.begin((params[MQTTID]).c_str())) {              // Start the mDNS responder for esp8266.local
		DEBUG_PRINTLN(F("mDNS responder started"));
	} else {
		DEBUG_PRINTLN(F("Error setting up MDNS responder!"));
	}
}

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
#if (DEBUG)	
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
  telnet.printf("webSocketEvent(%d, %d, ...)\r\n", num, type);
  switch (type) {
    case WStype_DISCONNECTED:             // if the websocket is disconnected
	  wsnconn--;
      telnet.printf("[%u] Disconnected!\n", num);
    break;
    case WStype_CONNECTED: {              // if a new websocket connection is established
		wsnconn++;
        IPAddress wip = webSocket.remoteIP(num);
        telnet.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, wip[0], wip[1], wip[2], wip[3], payload);
		readStatesAndPub();
	}
    break;
    case WStype_TEXT:                     // if new text data is received 
		telnet.printf("[%u] get Text: %s\r\n", num, payload);
		String str = String((char *)payload);
		String str2 = String("");
		mqttCallback(str2, str);
	}
}

//legge il valore dello stato dei toggle e li pubblica sul broker come stringa JSON
void readStatesAndPub(bool all){
  int vals; 

  //DEBUG_PRINTLN(F("\nreadStatesAndPub")); 
  
  //crea una stringa JSON con i valori  dello stato corrente dei pulsanti

  //vals=digitalRead(OUTSLED); //legge lo stato del led di stato
  //crea una stringa JSON con i valori  dello stato corrente dei pulsanti
  String s=F("{\"");	
  s+=mqttJson[MQTTJSONUP1]+F("\":\"")+(outLogic[ENABLES] && (outLogic[DIRS]==LOW))+F("\",\""); 	//up1 DIRS=HIGH
  s+=mqttJson[MQTTJSONDOWN1]+F("\":\"")+(outLogic[ENABLES] && (outLogic[DIRS]==HIGH))+F("\",\"");    //down1  DIRS=LOW
  s+=mqttJson[MQTTJSONUP2]+F("\":\"")+(outLogic[ENABLES+STATUSDIM] && (outLogic[DIRS+STATUSDIM]==LOW))+F("\",\"");	//up2 
  s+=mqttJson[MQTTJSONDOWN2]+F("\":\"")+(outLogic[ENABLES+STATUSDIM] && (outLogic[DIRS+STATUSDIM]==HIGH))+F("\",\"");    //down2
  s+= (String) F("pr1\":\"")+String(round(calcLen(0)))+F("\",\"");		//pr1
  s+= (String) F("pr2\":\"")+String(round(calcLen(1)))+F("\",\"");		//pr2
  if(blocked[0]>0){
	  s+= (String) F("blk1\":\"")+blocked[0]+F("\",\"");		//blk1
  }
  if(blocked[1]>0){
	  s+= (String) F("blk1\":\"")+blocked[1]+F("\",\"");		//blk2
  }
  s+= (String) F("sp1\":\"")+String((long)getTapThalt(0))+F("\",\"");		//sp1
  s+= (String) F("sp2\":\"")+String((long)getTapThalt(1));
  
  if(all){
		s+=F("\",\"");		//sp2
		s+=mqttJson[MQTTJSONTEMP]+F("\":\"")+String(asyncBuf[GTTEMP])+F("\",\"");
		s+=mqttJson[MQTTJSONMEANPWR]+F("\":[\"")+String(asyncBuf[GTMEANPWR1])+F("\",\"")+String(asyncBuf[GTMEANPWR2])+F("\"],\"");
		s+=mqttJson[MQTTJSONPEAKPWR]+F("\":[\"")+String(asyncBuf[GTPEAKPWR1])+F("\",\"")+String(asyncBuf[GTPEAKPWR2])+F("\"]}");
  }else{
		s+=F("\"}");		//sp2
  }
  publishStr(s);
}

void readAvgPowerAndPub(){
  int vals; 

  //DEBUG_PRINTLN(F("\nreadPowerAndPub")); 
  String s=F("{\"");	
  s+=mqttJson[MQTTJSONMEANPWR]+F("\":[\"")+String(asyncBuf[GTMEANPWR1])+F("\",\"")+String(asyncBuf[GTMEANPWR2])+F("\"]}");
  publishStr(s);
}

void readPeakPowerAndPub(){
  int vals; 

  //DEBUG_PRINTLN(F("\nreadPowerAndPub")); 
  String s=F("{\"");	
  s+=mqttJson[MQTTJSONPEAKPWR]+F("\":[\"")+String(asyncBuf[GTPEAKPWR1])+F("\",\"")+String(asyncBuf[GTPEAKPWR2])+F("\"]}");
  publishStr(s);
}

void readTempAndPub(){
  int vals; 

  //DEBUG_PRINTLN(F("\nreadTempAndPub")); 
  String s=F("{\"");
  //char sd[300];
  s+=mqttJson[MQTTJSONTEMP]+F("\":\"")+String(asyncBuf[GTTEMP])+F("\"}");
//sprintf(sd,"%s%s%s%f%s",F("{\""),mqttJson[MQTTJSONTEMP].c_str(),F("\":\""),asyncBuf[GTTEMP],F("\"}"));
  //s=String(sd);
  publishStr(s);
}

void publishStr(String &str){
//pubblica sul broker la stringa JSON
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
  //WiFi.mode(WIFI_AP);
  //delay(6000);
  //WiFi.mode(WIFI_AP);
  //delay(6000);
  //delay(TCOUNT*1000);
  //delay(1000);
  
  //setTimerState(wfs, CONNSTATSW);
  //telnet.begin();
  telnet.begin((params[MQTTID]).c_str()); // Initiaze the telnet server
  telnet.setResetCmdEnabled(true); // Enable the reset command
  telnet.setCallBackProjectCmds(&processCmdRemoteDebug);
  DEBUG_PRINTLN(F("Activated remote debug"));
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
  for(int i=0;i<4;i++)
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
#if (DEBUG)  
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
  //MDNS.addService(F("http"), F("tcp"), 80); 
  DEBUG_PRINT("HTTPUpdateServer ready! Open http://");
  DEBUG_PRINT(params[MQTTID]);
  DEBUG_PRINTLN(".local/update in your browser");
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
		//workaround for the DHCP offer problem ERROR: send_offer (error -13)
		//no busy loop!
		delay(3000);
		boot = false;
	}
}
	
inline void loop2() {
  //ArduinoOTA.handle();
  //funzioni eseguite ad ogni loop (istante di esecuione dipendente dal clock della CPU)
  aggiornaTimer(TMRHALT);
  aggiornaTimer(TMRHALT+TIMERDIM); 

  webSocket.loop();
  server.handleClient();  // Listen for HTTP requests from clients 
  
  if((millis()-prec) > time_base) //schedulatore con tempo base 
  {	
	prec = millis();
	//prec += time_base;
	//calcolo dei multipli interi del tempo base
	step = (step + 1) % nsteps;
	
	//current = millis();
#if (AUTOCAL) 
	if(dosmpl){
		noInterrupts();
		x = (int) analogRead(A0) - m;	
		(x > maxx) && (maxx = x);
		(x < minx) && (minx = x);
		interrupts();
		/*if(x > maxx) 					
		{    							
			maxx = x; 					
		}
		if(x < minx) 					
		{       						
			minx = x;					
		}*/
		//sampleCount++;
	}
	
	//ogni 20ms
	if(!(step % 10)){
		//zero detection scheduler
		zeroCnt = (zeroCnt + 1) % 50;
		
		if((isrun[0] || isrun[1])){
			dd = maxx - minx;
			//EMA calculation
			ex = dd*EMA + (1.0 - EMA)*ex;
			//ACSVolt = (double) ex/2.0;
			//peak = (double) ex/2.0;
			peak = ex;
			//reset of peak sample value
			DEBUG_PRINT(F("\n("));
			DEBUG_PRINT(0);
			DEBUG_PRINT(F(") ADC enable: "));
			DEBUG_PRINT(dosmpl);
			if(isrun[0] && dosmpl){
				DEBUG_PRINT(0);
				if(isrundelay[0] == 0){
					DEBUG_PRINT(F("\n("));
					DEBUG_PRINT(0);
					DEBUG_PRINT(F(") minx sensor: "));
					DEBUG_PRINT(minx);
					DEBUG_PRINT(F(" - maxx sensor: "));
					DEBUG_PRINT(maxx);
					DEBUG_PRINT(F(" - Mean sensor: "));
					DEBUG_PRINT(m);
					DEBUG_PRINT(F(" - Peak: "));
					DEBUG_PRINT(peak);
					//DEBUG_PRINT(F("\nSample count: "));
					//DEBUG_PRINTLN(sampleCount);
					//sampleCount = 0;
					chk[0] = checkRange((double) peak*(1 - weight[1]*isMoving(1)),0);
					//DEBUG_PRINT(F("Ampere: "));
					//float amp = getAmpRMS();
					//DEBUG_PRINTLN(amp);
					if(chk[0] != 0){
						DEBUG_PRINT(F("\n("));
						DEBUG_PRINT(0);
						if(chk[0] == -1){
							DEBUG_PRINTLN(F(") Stop: sottosoglia"));
							blocked[0] = secondPress(0);
							scriviOutDaStato();
						}else if(chk[0] == 2){
							DEBUG_PRINTLN(F(") Stop: soprasoglia"));
							blocked[0] = secondPress(0);
							scriviOutDaStato();
							blocked[0] = 1;
						}else if(chk[0] == 1){
							DEBUG_PRINTLN(F(") Start: fronte di salita"));
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
					DEBUG_PRINT(peak);
					isrundelay[0]--;
					ex = dd;
					DEBUG_PRINT(F(" - dd: "));
					DEBUG_PRINT(dd);
				}
			}else{
				isrundelay[0] = RUNDELAY;
			}
			
			if(isrun[1] && dosmpl){
				if(isrundelay[1] == 0){
					DEBUG_PRINT(F("\n("));
					DEBUG_PRINT(1);
					DEBUG_PRINT(F(") minx sensor: "));
					DEBUG_PRINT(minx);
					DEBUG_PRINT(F(" - maxx sensor: "));
					DEBUG_PRINT(maxx);
					DEBUG_PRINT(F(" - Mean sensor: "));
					DEBUG_PRINT(m);
					DEBUG_PRINT(F(" - Peak: "));
					DEBUG_PRINT(peak);
					DEBUG_PRINT(F(" - ADC enable: "));
					DEBUG_PRINT(dosmpl);
					//DEBUG_PRINT(F("\nSample count: "));
					//DEBUG_PRINTLN(sampleCount);
					//sampleCount = 0;
					chk[1] = checkRange((double) peak*(1 - weight[0]*isMoving(0)),1);
					if(chk[1] != 0){
						DEBUG_PRINT(F("\n("));
						DEBUG_PRINT(1);
						if(chk[1] == -1){
							blocked[1] = secondPress(1);
							scriviOutDaStato();
						}else if(chk[1] == 2){
							blocked[1] = secondPress(1);
							scriviOutDaStato();
							blocked[1] = 1;
						}else if(chk[1] == 1){
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
					DEBUG_PRINT(peak);
					isrundelay[1]--;
					ex = dd;
					DEBUG_PRINT(F(" - dd: "));
					DEBUG_PRINT(dd);
				}
			}else{
				isrundelay[1] = RUNDELAY;
			}
			//AC peak measure init
			minx = 1024;
			maxx = 0;
			dosmpl = true;
			DEBUG_PRINT(F("\n------------------------------------------------------------------------------------------"));
		}else{
			dosmpl = false;
			//all motors are stopped
			if(zeroCnt < 3){
				//zero detection activation (2 values every second)
				x = (int) analogRead(A0) - m;
				//running mean calculation
				smplcnt++;
				smplcnt && (m += (float) x / smplcnt);  //protected against overflow by a logic short circuit
				DEBUG_PRINT(F("\nZero mean sensor: "));
				DEBUG_PRINT(m);
			}
		}	
	}
#endif

	//1 sec
	if(!(step % 500)){
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
		
		if(!(isrun[0] || isrun[1])){
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
		
		//DEBUG_PRINTLN(wl_status_to_string(wfs));
		if(WiFi.getMode() == WIFI_OFF || WiFi.getMode() == WIFI_STA || WiFi.getMode() == WIFI_AP_STA){
			if(!wifiConn && !(isrun[0] || isrun[1])){
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
					//WiFi.waitForConnectResult();	
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
			#if (DEBUG)	
				DEBUG_PRINTLN(F("MQTTCONNCHANGED! Eseguo la setUserPwd() con usr "));
				DEBUG_PRINTLN(params[MQTTUSR]);
				DEBUG_PRINTLN(F(" e psw "));
				DEBUG_PRINTLN(params[MQTTPSW]);
			#endif		
				mqttClient->setUserPwd((params[MQTTUSR]).c_str(), (params[MQTTPSW]).c_str());
				DEBUG_PRINTLN(F("MQTTCONNCHANGED! Eseguo la connect() ..."));
				mqttClient->connect();
			#if (DEBUG)	
				DEBUG_PRINTLN(F("MQTTCONNCHANGED! Eseguo la subscribe() con "));
				DEBUG_PRINTLN(params[MQTTINTOPIC]);
				DEBUG_PRINTLN(F("..."));
			#endif	
				mqttClient->subscribe(params[MQTTINTOPIC]);
			#if (DEBUG)	
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
			#if (DEBUG)	
				DEBUG_PRINTLN(F("TOPICCHANGED! Outtopic: "));
				DEBUG_PRINTLN(params[MQTTOUTTOPIC]);
				DEBUG_PRINTLN(F(" Intopic: "));
				DEBUG_PRINTLN(params[MQTTINTOPIC]);
				DEBUG_PRINTLN(F("TOPICCHANGED! Eseguo la subscribe() con "));
				DEBUG_PRINTLN(params[MQTTINTOPIC]);
				DEBUG_PRINTLN(F("..."));
			#endif		
				mqttClient->subscribe(params[MQTTINTOPIC]);
			#if (DEBUG)	
				DEBUG_PRINTLN(F("TOPICCHANGED! Eseguo la publish() con "));
				DEBUG_PRINTLN(params[MQTTOUTTOPIC]);
				DEBUG_PRINTLN(F(" ..."));
			#endif		
				mqttClient->publish(params[MQTTOUTTOPIC], params[MQTTID]);
			}
		}
	}
	
	//ogni 50ms
	if(!(step % 30)){		
		//leggi ingressi locali e mette il loro valore sull'array val[]
		leggiTasti();
		leggiRemoto();
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
		//Finestra di riconnessione
		if((wifiConn == false && !(isrun[0] || isrun[1]))){
			//DEBUG_PRINTLN(F("to ESP stack... "));
			//delay(30);//give 30ms to the ESP stack for wifi connect
			wifiConn = (WiFi.status() == WL_CONNECTED);
		}
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
	}
  }//FINE Time base
  
  telnet.handle();
  yield();	// Give a time for ESP8266
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
				DEBUG_PRINTLN(F("stato 0: il motore va in stato fermo da fine corsa"));
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
	}else if(lastCmd == "testflash"){
		testFlash();
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
	}else{
		DEBUG_PRINT(F("\nComandi disponibili: "));
		DEBUG_PRINT(F("\nshowconf, reboot, reset, calibrate1, calibrate2, apmodeon, scanwifi, getip, getmqttstat, testflash,getadczero,gettemp,getpower\n"));
	}
	//telnet.flush();
}

#if (DEBUG)	
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
#if (DEBUG)	
void testFlash(){
  uint32_t realSize = ESP.getFlashChipRealSize();
  uint32_t ideSize = ESP.getFlashChipSize();
  FlashMode_t ideMode = ESP.getFlashChipMode();

  telnet.printf("Flash real id:   %08X\n", ESP.getFlashChipId());
  telnet.printf("Flash real size: %u bytes\n\n", realSize);

  telnet.printf("Flash ide  size: %u bytes\n", ideSize);
  telnet.printf("Flash ide speed: %u Hz\n", ESP.getFlashChipSpeed());
  telnet.printf("Flash ide mode:  %s\n", (ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN"));

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
