#include "common.h"
//End MQTT config------------------------------------------
//inizio variabili e costanti dello schedulatore (antirimbalzo)
#define time_base     	50        // periodo base in millisecondi
#define nsteps          12000        // numero di fasi massimo di un periodo generico
unsigned long prec=0;
//wifi config----------------------------------------------
const char* outTopic = "T_Garage/out";
const char* inTopic = "T_Garage/in";
const char* ssid1 = "ZyXEL";
const char* password1 = "1234567890";
const char* ssid2 = "ZyXEL";
const char* password2 = "1234567890";
const char* apSsid = "admin";
const char* apPassword = "admin";
//MQTT config----------------------------------------------
const char* mqtt_server = "192.168.1.2";
const char* clientID = "Tap_Garage";
int haltPrm[2] = {THALT1,THALT2};
int haltOfs[2] = {THALT1OFST,THALT2OFST};
unsigned long edelay[2]={0,0};
byte wsnconn = 0;

IPAddress ip(192, 168, 43, 1);
IPAddress gateway(192, 168, 43, 1);
IPAddress subnet(255, 255, 255, 0);

RemoteDebug telnet;
WiFiEventHandler stationConnectedHandler;
WiFiEventHandler stationDisconnectedHandler;
    
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
String mqttJson[5]={"outsled","up1","down1","up2","down2"};
//Valore iniziale: il suo contenuto viene poi caricato da EEPROM
unsigned int thalt1=5000;
unsigned int thalt2=2500;

String params[PARAMSDIM]={webUsr,webPsw,APSsid,APPsw,clntSsid1,clntPsw1,clntSsid2,clntPsw2,mqttAddr,mqttID,mqttOutTopic,mqttInTopic,mqttUsr,mqttPsw,String(thalt1),String(thalt2),"400","400","ip","false","false","false","false","false","false"};
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
byte swcount=0;
byte wifindx=0;
//End port config------------------------------------------
String  s;  //global: contain current json message
unsigned int step;
//bool extCmd=false;
//vettori di ingresso, uscite e stato
byte in[NBTN*BTNDIM], inr[NBTN*BTNDIM], outPorts[NBTN*BTNDIM];
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
byte calibr=0;
String pr[3] = {"{\"pr1\":\"", "{\"pr2\":\"", "\"}"};
//-----------------------------------------Begin of prototypes---------------------------------------------------------
//-----------------------------------------End of prototypes---------------------------------------------------------
bool inline switchdfn(byte val, byte n){
	//n: numero di pulsanti
	val = (val>0);								//SEMPLICE!!!!
	bool changed = (val != dprecval2[n]);
	dprecval2[n] = val;            // valore di val campionato al loop precedente 
	return changed;
}

void setup_AP() {
  DEBUG_PRINTLN(F("Configuring access point..."));
  /* You can remove the password parameter if you want the AP to be open. */
  //WiFi.softAP(APSsid.c_str(), APPsw.c_str());
  
  // Disable the WiFi persistence.  The ESP8266 will not load and save WiFi settings in the flash memory.
  WiFi.persistent(false);
  //WiFi.setOutputPower (12);
 
  //WiFi.softAPConfig(ip, gateway, subnet);
  DEBUG_PRINT(F("Setting soft-AP configuration ... "));
  DEBUG_PRINTLN(WiFi.softAPConfig(ip, gateway, subnet) ? F("Ready") : F("Failed!"));
  
  //WiFi.softAP(APSsid.c_str());
  DEBUG_PRINT(F("Setting soft-AP ... "));
  DEBUG_PRINTLN(WiFi.softAP((params[APPSSID]).c_str()) ? F("Ready") : F("Failed!"));
  
  //ArduinoOTA.begin();
  DEBUG_PRINT(F("Soft-AP IP address = "));
  DEBUG_PRINTLN(WiFi.softAPIP());
  
  WiFi.mode(WIFI_STA);
}

//wifi setup function
void setup_wifi(int wifindx) {
  //if(WiFi.getMode() == WIFI_STA || WiFi.getMode() == WIFI_AP_STA){
	wifindx = wifindx*2;  //client1 e client2 hanno indici contigui nell'array params
  
	// We start by connecting to a WiFi network
	DEBUG_PRINTLN(F("Connecting to "));
	DEBUG_PRINTLN(params[CLNTSSID1+wifindx]);
  
	//WiFi.disconnect();
	WiFi.persistent(false);
	//WiFi.mode(WIFI_OFF);   // this is a temporary line, to be removed after SDK update to 1.5.4
	WiFi.mode(WIFI_AP_STA);

	WiFi.begin((params[CLNTSSID1+wifindx]).c_str(), (params[CLNTPSW1+wifindx]).c_str());
	
	WiFi.printDiag(Serial);
	
	if(WiFi.status() == WL_CONNECTED) {
		//ArduinoOTA.begin();
		//ho ottenuto una connessione
		DEBUG_PRINTLN(F(""));
		DEBUG_PRINTLN(F("WiFi connected"));
		digitalWrite(OUTSLED, LOW);
		DEBUG_PRINTLN(F("IP address: "));
		DEBUG_PRINTLN(WiFi.localIP());
		params[LOCALIP] = WiFi.localIP().toString();
	}
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
	// Loop until we're mqttReconnected
	DEBUG_PRINTLN(F("Distruggo l'oggetto MQTT client."));
	if(mqttClient!=NULL){
		delete mqttClient;
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
	DEBUG_PRINT(F(" ..."));
	//mqttClient->setClientId(params[MQTTID]);
	if(mqttClient==NULL){
		DEBUG_PRINTLN(F("ERROR on mqttReconnect! MQTT client is not allocated."));
	}
	else
	{
		mqttClient->onData(mqttCallback);
		mqttClient->onConnected([]() {
		DEBUG_PRINTLN(F("onConnected([]() dice sono connesso."));
			
			//Altrimenti dice che è connesso ma non comunica
			mqttClient->subscribe(params[MQTTINTOPIC]);
			mqttClient->publish(params[MQTTOUTTOPIC], params[MQTTID]);
			//mqttConnected=true;
		});
		mqttClient->onDisconnected([]() {
			//DEBUG_PRINTLN("MQTT disconnected.");
		DEBUG_PRINTLN(F("onDisconnected([]() dice non sono connesso."));
			//mqttConnected=false;
		});
		
		mqttClient->setUserPwd((params[MQTTUSR]).c_str(), (params[MQTTPSW]).c_str());
		mqttClient->connect();
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
	v = parseJsonFieldToInt(response, mqttJson[0], ncifre);
	//digitalWrite(OUTSLED, v); 
   
	parseJsonFieldArrayToInt(response, inr, mqttJson, ncifre, JSONLEN,1);
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
        IPAddress ip = webSocket.remoteIP(num);
        telnet.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
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
void readStatesAndPub(){
  int vals; 

  DEBUG_PRINTLN(F("\nreadStatesAndPub")); 
  
  //crea una stringa JSON con i valori  dello stato corrente dei pulsanti

  vals=digitalRead(OUTSLED); //legge lo stato del led di stato
  //crea una stringa JSON con i valori  dello stato corrente dei pulsanti
  s=F("{\"");	
  s+=mqttJson[STATBTNNDX]+F("\":\"")+vals+F("\",\"");
  s+=mqttJson[1]+F("\":\"")+(outLogic[ENABLES] && (outLogic[DIRS]==LOW))+F("\",\""); 	//up1 DIRS=HIGH
  s+=mqttJson[2]+F("\":\"")+(outLogic[ENABLES] && (outLogic[DIRS]==HIGH))+F("\",\"");    //down1  DIRS=LOW
  s+=mqttJson[3]+F("\":\"")+(outLogic[ENABLES+STATUSDIM] && (outLogic[DIRS+STATUSDIM]==LOW))+F("\",\"");	//up2 
  s+=mqttJson[4]+F("\":\"")+(outLogic[ENABLES+STATUSDIM] && (outLogic[DIRS+STATUSDIM]==HIGH))+F("\",\"");    //down2
  s+= (String) F("pr1\":\"")+String((long)(getCronoCount(0)*100)/getUpLimit(0))+F("\",\"");		//pr1
  s+= (String) F("pr2\":\"")+String((long)(getCronoCount(1)*100)/getUpLimit(1))+F("\",\"");		//pr2
  s+= (String) F("sp1\":\"")+String((long)getUpLimit(0))+F("\",\"");		//pr1
  s+= (String) F("sp2\":\"")+String((long)getUpLimit(1))+F("\"}");		//pr2
  //pubblica sul broker la stringa JSON
  if(mqttClient==NULL){
	  DEBUG_PRINTLN(F("ERROR on readStatesAndPub MQTT client is not allocated."));
  }
  else
  {
	  mqttClient->publish(params[MQTTOUTTOPIC], s);
	  DEBUG_PRINTLN(s);
  }
  //if(!webSocket){
	  //DEBUG_PRINTLN(F("ERROR on readStatesAndPub webSocket server is not allocated."));
  //}
  //else
  //{
	  webSocket.broadcastTXT(s);
  //}

}

void initIiming(){
  edelay[0]=(params[STDEL1]).toInt();
  edelay[1]=(params[STDEL2]).toInt();
  //initTapparellaLogic(in,inr,outLogic,(params[THALT1]).toInt(),(params[THALT2]).toInt(),(params[STDEL1]).toInt(),(params[STDEL2]).toInt(),BTNDEL);
  initTapparellaLogic(in,inr,outLogic,(params[THALT1]).toInt(),(params[THALT2]).toInt(),(params[STDEL1]).toInt(),(params[STDEL2]).toInt(),BTNDEL1,BTNDEL2);
  //cthalt[0]=(params[THALT1]).toInt();
  //cthalt[1]=(params[THALT2]).toInt();
}

void setup() {
  //inizializza la seriale
  Serial.begin(115200);
  //Serial.setdebugOutput(true);ù
  //carica la configurazione dalla EEPROM
  //DEBUG_PRINTLN(F("Carico configurazione."));
  initCommon(&server,params,mqttJson);
  loadConfig();
  //inizializza il client wifi
  //setup_wifi(wifindx);
  //inizializza l'AP wifi
  setup_AP();
  setup_wifi(wifindx);
  delay(TCOUNT*1000);
  //telnet.begin();
  telnet.begin((params[MQTTID]).c_str()); // Initiaze the telnet server
  telnet.setResetCmdEnabled(true); // Enable the reset command
  DEBUG_PRINTLN(F("Activated remote debug"));
  DEBUG_PRINTLN(F("Inizializzo i pulsanti."));
  initdfn(LOW, 0);  //pull DOWN init (in realtà è un pull up, c'è un not in ogni ingresso sui pulsanti)
  initdfn(LOW, 1);
  initdfn(LOW, 2);
  initdfn(LOW, 3);
  initdfn(LOW, 4);
  initdfn((byte) WL_DISCONNECTED, CONNSTATSW);
  //Timing init
  setupTimer((params[THALT1]).toInt(),TMRHALT);				//function timer switch1
  setupTimer((params[THALT2]).toInt(),TMRHALT+TIMERDIM);	//function timer switch2 
  setupTimer(RSTTIME*1000,RESETTIMER);						//special timer btn1 
  setupTimer(CNTIME*1000,CNTIMER1);							//special timer btn1 
  setupTimer(CNTIME*1000,CNTIMER2);							//special timer btn2 
  setupTimer(APOFFT*1000,APOFFTIMER);						//special timer btn1
  setCronoLimits(0,(params[THALT1]).toInt(),TAP1);
  setCronoLimits(0,(params[THALT2]).toInt(),TAP2);
  //DEBUG_PRINTLN("Inizializzo i timers a zero.");
  //initTimers();
  //DEBUG_PRINTLN("Avvio il client MQTT.");
  mqttReconnect();
  //avvia il client MQTT
  //DEBUG_PRINTLN("Registro i callback del server web.");
  //inizializza lista di httpCallback e corrispondenti URI
  startWebSocket();
  delay(10);
  httpSetup();
  delay(10);
   
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
  digitalWrite(OUTSLED, HIGH);
  delay(100);
  digitalWrite(OUT1EU, HIGH);
  delay(50);
  digitalWrite(OUT1DD, HIGH);
  delay(50);
  digitalWrite(OUT2EU, HIGH);
  delay(50);
  digitalWrite(OUT2DD, HIGH);
  delay(50);
  digitalWrite(OUT2DD, LOW);
  delay(50); 
  digitalWrite(OUT2EU, LOW);
  delay(50);
  digitalWrite(OUT1DD, LOW);
  delay(50);
  digitalWrite(OUT1EU, LOW);
  delay(500);
  for(int i=0;i<NBTN*STATUSDIM;i++)
	  outLogic[i]=LOW;
  for(int i=0;i<4;i++)
		inr[i]=LOW;
  initIiming();
  // Register event handlers.
  // Callback functions will be called as long as these handler objects exist.
  // Call "onStationConnected" each time a station connects
  stationConnectedHandler = WiFi.onSoftAPModeStationConnected(&onStationConnected);
  // Call "onStationDisconnected" each time a station disconnects
  stationDisconnectedHandler = WiFi.onSoftAPModeStationDisconnected(&onStationDisconnected);
  //initTapparellaLogic(in,inr,outLogic,(params[THALT1]).toInt(),(params[THALT2]).toInt(),(params[STDEL1]).toInt(),(params[STDEL2]).toInt());
  //esp_log_set_vprintf(_log_vprintf);
#if (DEBUG)  
  testFlash();
#endif
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
  setup_mDNS();
  //OTA web page handler linkage
  httpUpdater.setup(&server);
  //start HTTP server
  server.begin();
  DEBUG_PRINTLN("HTTP server started");
  MDNS.addService(F("http"), F("tcp"), 80); 
  DEBUG_PRINT("HTTPUpdateServer ready! Open http://");
  DEBUG_PRINT(params[MQTTID]);
  DEBUG_PRINTLN(".local/update in your browser");
}

void loop() {
  //ArduinoOTA.handle();
  //funzioni eseguite ad ogni loop (istante di esecuione dipendente dal clock della CPU)
  aggiornaTimer(TMRHALT);
  aggiornaTimer(TMRHALT+TIMERDIM); 
  webSocket.loop();
  server.handleClient();  // Listen for HTTP requests from clients

  if((millis()-prec) > time_base) //schedulatore con tempo base con funzione di antirimbalzo (legge ogni 50 mSec)
  {
	aggiornaTimer(CNTIMER1); 
	aggiornaTimer(CNTIMER2); 
	
	prec = millis();
	//calcolo dei multipli interi del tempo base
	step = (step + 1) % nsteps;
	
	//DEBUG_PRINT("NStep: ");  
	//DEBUG_PRINTLN(step,DEC);
	//codice eseguito ogni 100*50 msec = 5 sec
	//riconnessione MQTT
	
	if(!(step % 100)){
		//aggiornaTimer(APOFFTIMER);
		
		//a seguito di disconnessioni accidentali tenta una nuova procedura di riconnessione
        if(mqttClient!=NULL){
			if(!(mqttClient->isConnected())){
				DEBUG_PRINTLN(F("isConnected() dice non sono connesso."));
				mqttConnected=false;
			}	
			else{
				DEBUG_PRINTLN(F("isConnected() dice sono connesso."));
				mqttConnected=true;
				DEBUG_PRINTLN(getGroupState(0));
				DEBUG_PRINTLN(getGroupState(1));
				DEBUG_PRINTLN(in[0]);
				DEBUG_PRINTLN(in[1]);
				DEBUG_PRINTLN(in[2]);
				DEBUG_PRINTLN(in[3]);
			}
		}
		else
		{
			mqttConnected=false;
		}
		
		if((!mqttConnected) && WiFi.status()==WL_CONNECTED && WiFi.getMode()==WIFI_STA) {
			DEBUG_PRINT(F("MQTT client loop: provo a riconnettermi..."));
			if(mqttClient==NULL){
				DEBUG_PRINTLN(F("ERROR! MQTT client is not allocated."));
				mqttReconnect();
				DEBUG_PRINTLN(F("mqttReconnect() dice sono connesso."));
				//mqttConnected=true;
			}
			else
			{
				//non si può fare perchè dopo pochi loop crasha
				if(dscnct){
					dscnct=false;
					DEBUG_PRINTLN(F("eseguo la connect()..."));
					noInterrupts ();
					mqttClient->connect();
					interrupts ();
				}
				else
				{
					dscnct=true;
					DEBUG_PRINTLN(F("eseguo la disconnect()..."));
					noInterrupts ();
					mqttClient->disconnect();
					interrupts ();
				}
				//non si può fare senza disconnect perchè dopo pochi loop crasha
				//mqttClient->setUserPwd((params[MQTTUSR]).c_str(), (params[MQTTPSW]).c_str());
			}
		}
		//is else if per gestione priorità, l'ordine è importante! vanno fatti in momenti successivi
		if(params[WIFICHANGED]=="true"){
			params[WIFICHANGED]="false";
			setup_AP();
			wifindx=0;
			setup_wifi(wifindx);
			//httpSetup();
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
		
		//delay(0);
	}
	
	//codice eseguito ogni 20*50 msec = 1000 msec
	//riconnessione WiFi
	if(!(step % 20)){
		aggiornaTimer(RESETTIMER);
		aggiornaTimer(APOFFTIMER);
		
		/*if (telnet.read() == 'R') {
			telnet.stop();
			delay(100);
			ESP.reset();
		}*/
		
		noInterrupts ();
		wl_status_t wfs = WiFi.status();
		interrupts ();
		
		//DEBUG_PRINTLN(wl_status_to_string(wfs));
		
		if(wfs != WL_CONNECTED){
				//lampeggia led di connessione
				digitalWrite(OUTSLED, !digitalRead(OUTSLED));
				if(WiFi.getMode() == WIFI_STA || WiFi.getMode() == WIFI_AP_STA){
					swcount++;
					DEBUG_PRINT(F("swcount roll: "));
					DEBUG_PRINTLN(swcount);
					if((swcount >= TCOUNT) ){
						swcount = 0;
						setup_wifi(wifindx);
						wifindx = (wifindx +1) % 2;
					}
				}
		}
		if(switchdfn(wfs, CONNSTATSW)){
			if(wfs == WL_CONNECTED){
				//WiFi.reconnect();
				WiFi.enableAP(false);
				WiFi.mode(WIFI_STA);
				//ho ottenuto una connessione
				DEBUG_PRINTLN(F(""));
				DEBUG_PRINTLN(F("WiFi connected"));
				digitalWrite(OUTSLED, LOW);
				DEBUG_PRINTLN(F("IP address: "));
				DEBUG_PRINTLN(WiFi.localIP());
				params[LOCALIP] = WiFi.localIP().toString();
			}
			else
			{
				DEBUG_PRINTLN(F("AP mode on"));
				//setup_AP();
				WiFi.enableAP(true);
				params[LOCALIP] = WiFi.softAPIP().toString();
			}
			//httpSetup();
		}
	}
	
	//codice eseguito ogni tempo base (50ms)
	//leggi ingressi locali e mette il loro valore sull'array val[]
	leggiTasti();
	leggiRemoto();
	//se uno dei tasti delle tapparelle è stato premuto
	if(tapparellaLogic(TAP1) == 1 ||  tapparellaLogic(TAP2)== 1){ 
		//once any button is pressed
		//legge lo stato finale e lo scrive sulle uscite
		scriviOutDaStato();
		//legge lo stato finale e lo pubblica su MQTT
		readStatesAndPub();
		//DEBUG_PRINTLN("Fine callback MQTT.");
    }
	//FINE Time base
  }
  
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
		int toffset=n*TIMERDIM;
		if(getGroupState(n)==3){ //il motore e in moto cronometrato scaduto (timer di blocco scaduto)
			secondPress(n);
			DEBUG_PRINTLN(F("stato 0: il motore va in stato fermo da fine corsa"));
			//comanda gli attuatori per fermare (non lo fa il loop stavolta!)
			scriviOutDaStato();
			//pubblica lo stato finale su MQTT (non lo fa il loop stavolta!)
			readStatesAndPub();
		}else if(getGroupState(n)==1){	//se il motore era in attesa di partire (timer di attesa scaduto) 
			//setGroupState(2,n);	//il motore è in moto a vuoto
			DEBUG_PRINTLN(F("onElapse:  timer di attesa scaduto"));
			startEngineDelayTimer(n);
			scriviOutDaStato();
		}else if(getGroupState(n)==2){//se il motore è in moto a vuoto
			DEBUG_PRINTLN(F("onElapse:  timer di corsa a vuoto scaduto"));
			///setGroupState(3,n);	//il motore va in moto cronometrato
			startEndOfRunTimer(n);
			//pubblica lo stato finale su MQTT (non lo fa il loop stavolta!)
			readStatesAndPub();
		}
	}else if(n == CNTIMER1 || n == CNTIMER2)   //  (2,3) --> state n%2
	{	DEBUG_PRINTLN(F("\nNon è il timer THALT: "));
		if(n == RESETTIMER)
		{
			rebootSystem();
		}else if(n == APOFFTIMER){
			if(WiFi.softAPgetStationNum() == 0){
				WiFi.enableAP(false);
				WiFi.enableSTA(true);
				DEBUG_PRINTLN(F("-----------------------------"));
				DEBUG_PRINTLN(F("Nussun client si è ancora connesso, disatttivato AP mode"));
				DEBUG_PRINTLN(F("-----------------------------"));
			}
		}else if(n == CNTIMER1)
		{
			DEBUG_PRINTLN(F("onElapse:  timer 1 dei servizi a conteggio scaduto"));
			if(testCntEvnt(3,CNTSERV1)){
				WiFi.enableAP(true);
				DEBUG_PRINTLN(F("-----------------------------"));
				DEBUG_PRINTLN(F("Atttivato AP mode"));
				DEBUG_PRINTLN(F("-----------------------------"));
				startTimer(APOFFTIMER);
				//WiFi.enableSTA(false);
				WiFi.setAutoConnect(false);
				WiFi.setAutoReconnect(false);
				setGroupState(0,n%2);
			}else if(testCntEvnt(4,CNTSERV1)){
				DEBUG_PRINTLN(F("-----------------------------"));
				DEBUG_PRINTLN(F("Rebooting ESP"));
				DEBUG_PRINTLN(F("-----------------------------"));
				ESP.restart();
			}else if(testCntEvnt(5,CNTSERV1)){
				DEBUG_PRINTLN(F("-----------------------------"));
				DEBUG_PRINTLN(F("ATTIVATA CALIBRAZIONE MANUALE BTN 1"));
				DEBUG_PRINTLN(F("-----------------------------"));
				manualCalibration(0); //BTN1
			}else{
				setGroupState(0,n%2);
			}
			//DEBUG_PRINT(F("Resettato contatore dei servizi: "));
			resetCnt(CNTSERV1);
		}else if(n == CNTIMER2)
		{
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
		}else{
			setGroupState(0,n%2);		
		}
		//setGroupState(0,n%2);				 								//stato 0: il motore va in stato fermo
		DEBUG_PRINTLN(F("stato 0: il motore va in stato fermo da stato configurazione"));
	}
	
	//DEBUG_PRINTLN(F("Fine timer"));
}

void onTapStop(byte n){
	if(calibr == 1){
		setBtnDelay(0,n);
		//è stato chiamato un tasto reale
		//il motore viene bloccato
		//nel contempo viene fatto partire in UP, ma servono:
		//-------------------------------------------------------------
		//1)fronte di discesa simulato (rilascio simulato dei pulsanti)
		//si usa inr per evitare interferenze con i tasti reali
		DEBUG_PRINTLN(F("1)fronte di discesa simulato (rilascio simulato dei pulsanti)"));
		in[BTN1IN + n*BTNDIM] = 0; 
		in[BTN2IN + n*BTNDIM] = 0; 
		//viene impostato il tempo massimo di durata della corsa del motore THALTMAX
		tapparellaLogic(n);
		//-------------------------------------------------------------
		//2)reset del cronometro immediatamente prima della salita
		DEBUG_PRINTLN(F("2)reset del cronometro immediatamente prima della salita"));
		resetCrono(n);
		//Tapparella completamente abbassata: imposto a zero il contatore di stato
		resetCronoCount(n);
		setCronoDir(UP,n);
		//-------------------------------------------------------------
		//3)fronte di salita simulato (pressione simulata dei pulsanti)
		DEBUG_PRINTLN(F("3)fronte di salita simulato (pressione simulata dei pulsanti)"));
		//si usa inr per evitare interferenze con i tasti reali
		inr[BTN1IN + n*BTNDIM] = 1;
		tapparellaLogic(n);
		//in[BTN1IN + n*BTNDIM] = 0; 
		//in[BTN2IN + n*BTNDIM] = 0; 
		//Si devono mantenere gli ingressi alti per evitare transizioni (altrimenti possono simulare una pressione)
		//in[BTN1IN + n*BTNDIM] = 1;
		//in[BTN2IN + n*BTNDIM] = 1;
		//-------------------------------------------------------------
		//4)aggiornamento stato della calibrazione
		DEBUG_PRINTLN(F("4)aggiornamento stato della calibrazione"));
		calibr =2;
		//il motore è attivo => non c'è chiamata a cronoStop()
		//si ultima il fronte di sapita della chiamata reale
		//-------------------------------------------------------------
		//5)rilascio reale dei pulsanti al prossimo ciclo di loop()
		DEBUG_PRINTLN(F("5)rilascio reale dei pulsanti al prossimo ciclo di loop()"));
		DEBUG_PRINTLN(F("-----------------------------"));
		DEBUG_PRINT(F("FASE 2 CALIBRAZIONE MANUALE"));
		DEBUG_PRINTLN(F("-----------------------------"));
		DEBUG_PRINTLN(F("LA TAPPARELLA STA SALENDO........."));
		DEBUG_PRINT(F("PREMERE UN PULSANTE QUALSIASI DEL GRUPPO ATTIVO"));
		DEBUG_PRINTLN(F("-----------------------------"));
	}else if(calibr == 2){
		//è stato chiamato un tasto reale
		//il motore viene bloccato
		//nel contempo vengono effettuati:
		DEBUG_PRINTLN(F("-----------------------------"));
		DEBUG_PRINT(F("FASE 3 CALIBRAZIONE MANUALE BTN "));
		DEBUG_PRINTLN(n+1);
		DEBUG_PRINT(F("SALVATAGGIO TEMPO DI SEC "));
		DEBUG_PRINTLN(getCronoValue(n));
		//------------------------------------------------------------
		//1)aggiornamento stato della calibrazione
		calibr = 0;
		//------------------------------------------------------------
		//2)lettura del cronometro e salvataggio su EEPROM della lettura
		//il tempo non deve scendere al di sotto di un tempo minimo
		unsigned int app = getCronoCount(n);
		DEBUG_PRINT(F("getCronoCount(n): "));
		DEBUG_PRINTLN(getCronoCount(n));
		if(app < CNTIME*1000)
			app = CNTIME*1000;
		
		//IMPORTANTE evita il rolloff in setupTimer(thalt - getCronoCount(n),TMRHALT+toffset);
		//nella tapparellaLogic
		setGroupState(0,n);			
		setCronoLimits(0,app,n);
		params[haltPrm[n]] = String(app);
		initTapparellaLogic(in,inr,outLogic,(params[THALT1]).toInt(),(params[THALT2]).toInt(),(params[STDEL1]).toInt(),(params[STDEL2]).toInt(),BTNDEL1,BTNDEL2);
		setGroupState(0,n);												//stato 0: il motore va in stato fermo
		//INCREMENTO DI TEMPO DI UN SINCOLO ELEMENTO DELLA BARRA DI STATO (MISURATO IN TEMPI BASE)
		EEPROM.begin(EEPROMPARAMSLEN);
		EEPROMWriteInt(haltOfs[n], app);
		EEPROM.end();
		DEBUG_PRINT(F("Modified THALT "));
		DEBUG_PRINTLN(haltPrm[n]);
		DEBUG_PRINT(F(": "));
		DEBUG_PRINTLN(params[haltPrm[n]]);
		DEBUG_PRINTLN(F("-----------------------------"));
	}
	//do nothing
}

void onTapStart(byte n){
	//do nothing
}

void manualCalibration(byte btn){
	//è stato chiamata la funzione di calibrazione
	//nel contempo viene fatto partire in DOWN, ma servono:
	//-------------------------------------------------------------
	//1)attivazione stato di calibrazione
	calibr = 1;
	DEBUG_PRINTLN(F("-----------------------------"));
	DEBUG_PRINT(F("FASE 1 CALIBRAZIONE MANUALE BTN "));
	DEBUG_PRINTLN(btn+1);
	DEBUG_PRINTLN(F("-----------------------------"));
	//-------------------------------------------------------------
	//2)attivazione simulata pulsante di DOWN
	//scrivi l'eccitazione asincrona simulata sulla coda dei mesaggi remoti
	//il motore è attivo => non c'è chiamata a cronoStop()
	//initTapparellaLogic(in,inr,outLogic,(params[THALT1]).toInt(),(params[THALT2]).toInt(),(params[STDEL1]).toInt(),(params[STDEL2]).toInt(),BTNDEL1,BTNDEL2);
	setTapThalt(THALTMAX,btn);
	setBtnDelay(0,btn);
	setGroupState(0,btn);												//stato 0: il motore va in stato fermo
	setCronoLimits(0,THALTMAX,btn);
	setCronoCount(THALTMAX,btn);
	params[haltPrm[btn]] = THALTMAX;
	inr[BTN2IN + btn*BTNDIM] = 1; 
	DEBUG_PRINTLN(F("LA TAPPARELLA STA SCENDENDO......"));
	DEBUG_PRINT(F("PREMERE UN PULSANTE QUALSIASI DEL GRUPPO "));
	DEBUG_PRINTLN(btn+1);
	DEBUG_PRINTLN(F("-----------------------------"));
	//-------------------------------------------------------------
	//3)diseccitazione (rilascio pulsanti) reale al loop() succesivo perchè i tasti non sono premuti
}
void rebootSystem(){
	EEPROM.begin(EEPROMPARAMSLEN);
	alterEEPROM();
	EEPROM.end();
	DEBUG_PRINTLN(F("Resetting ESP"));
	ESP.restart();
}
//----------------------------------------------------FINE TIMER----------------------------------------------------------------------
/*void handleCmdJsonExt(){
	//DEBUG_PRINTLN(F("Completata richiesta di scrittura di nuove con,figurazioni da handleCmdJsonExt"));
	handleCmdJson(server, s);
	//DEBUG_PRINTLN(F("Ricevuta richiesta di scrittura di nuove configurazioni da handleCmdJsonExt"));
}*/

void onStationConnected(const WiFiEventSoftAPModeStationConnected& evt) {
	DEBUG_PRINTLN(F("Station connected: "));
	DEBUG_PRINTLN(macToString(evt.mac));
	if(WiFi.softAPgetStationNum() == 1){
		DEBUG_PRINTLN(F("WIFI: disconnecting from AP"));
		WiFi.setAutoConnect(false);
		WiFi.setAutoReconnect(false);
		WiFi.enableSTA(false);
		//WiFi.mode(WIFI_AP);
	}
}

void onStationDisconnected(const WiFiEventSoftAPModeStationDisconnected& evt) {
	DEBUG_PRINTLN(F("Station disconnected: "));
	DEBUG_PRINTLN(macToString(evt.mac));
	if(WiFi.softAPgetStationNum() == 0){
		DEBUG_PRINTLN(F("WIFI: reconnecting to AP"));
		swcount = TCOUNT;  //importante!
		WiFi.enableSTA(true);
		WiFi.setAutoConnect(true);
		WiFi.setAutoReconnect(true);
	}
}

String macToString(const unsigned char* mac) {
  char buf[20];
  snprintf(buf, sizeof(buf), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return String(buf);
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
    DEBUG_PRINTLN("Flash Chip configuration wrong!\n");
  } else {
    DEBUG_PRINTLN("Flash Chip configuration ok.\n");
  }
}
#endif
