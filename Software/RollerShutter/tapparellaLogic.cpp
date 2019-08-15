#include "tapparellaLogic.h"

int count1 = 0;
byte groupState[4];
byte *inp;
byte *outp;
byte *inrp;
byte *outlogicp;
String  *confcmdl;
byte act[4] = {0,0,0,0};
float taplen, deltal;
float barrad;
float tapthick; 
long thaltp[2]={THALTMAX/2,THALTMAX/2};
long base[2]={0,0};
bool oe[4];  //output enable
#if (AUTOCAL) 
float fact;
#endif
byte lastCmd[4]; //{sw1,sw2,sw3,sw4}
unsigned long target[4];			 //allineati con i timers corrispondenti
unsigned long engdelay[4]={0,0,0,0}; //allineati con i timers corrispondenti
unsigned long btndelay[4]={0,0,0,0}; //allineati con i timers corrispondenti
unsigned long haltdelay[4]={0,0,0,0}; //allineati con i timers corrispondenti
//int timermap[4]={TMRHALT,AUX1TIMER,1,AUX2TIMER}; //mappa timers
int outmap[4]={ENABLES,DIRS,4,5}; 	 //mappa uscite
//int groupmap[4]={0,2,1,3}; 	 		 //mappa gruppi
short upmap[2]={1,-1};
byte calibr=0;
byte nrun=0;
byte moving[2]={false,false};
bool first[2]={true,true};
double nmax;
float posdelta = 0;
//bool rollmode[2] = {true,true};

inline bool switchdfn(byte val, byte n){
	//n: numero di pulsanti
	val = (val>0);								//SEMPLICE!!!!
	bool changed = (val != dprecval2[n]);
	dprecval2[n] = val;            // valore di val campionato al loop precedente 
	return changed;
}

inline void copytout(){
#if (SCR)
	 outp[0]=(outlogicp[ENABLES] && (outlogicp[DIRS]==LOW));	
	 outp[1]=(outlogicp[ENABLES] && (outlogicp[DIRS]==HIGH));		
	 outp[2]=(outlogicp[ENABLES+STATUSDIM] && (outlogicp[DIRS+STATUSDIM]==LOW));	
	 outp[3]=(outlogicp[ENABLES+STATUSDIM] && (outlogicp[DIRS+STATUSDIM]==HIGH));		
#else		
	 outp[0]=outlogicp[ENABLES];	
	 outp[1]=outlogicp[DIRS];		
	 outp[2]=outlogicp[ENABLES+STATUSDIM];		
	 outp[3]=outlogicp[DIRS+STATUSDIM];					 
#endif
}

byte switchLogic(byte sw, byte n){
	int offset=n*STATUSDIM;
	int toffset=n*TIMERDIM;
	int poffset=n*BTNDIM;
	byte changed = 0;
	
	//pulsante UP o DOWN (sw: 0 o 1)
	if(switchdfn(inp[BTN1IN+poffset+sw],BTN1IN+poffset+sw))
	{
		DEBUG_PRINT(F("fronte di SW: "));
		DEBUG_PRINTLN(BTN1IN+poffset+sw+1);
		DEBUG_PRINT(F("stato switch:  "));
		DEBUG_PRINTLN(getGroupState(toffset));
		
		byte s = getGroupState(toffset);
		//siamo su uno dei fronti del pulsante 
		if(inp[BTN1IN+poffset+sw]>0)  
		{			
			changed = 0;
			//fronte di salita
			startTimer(RESETTIMER);
			outlogicp[SW1ONS+offset+sw]=true;
			if(!outlogicp[SW1ONS+offset+!sw]){  //evita attivazioni con pressione contemporanea di due tasti (interblocco)
				DEBUG_PRINTLN(F("dopo interblocco "));
				//modalità tapparella
				//effettuata prima pressione
				if(s==0)
				{ 	
					//se il motore è fermo
					setCronoDir(upmap[sw],n);  //in base a questo viene decisa la direzione della partenza differita
					lastCmd[BTN1IN + poffset+sw] = inp[BTN1IN+poffset+sw];
					DEBUG_PRINTLN(F("tapparellaLogic: stato 1: il motore va in attesa da stato 0 (fermo)"));
					//stato 1: il motore va in attesa del moto a vuoto
					startTimer(btndelay[n],TMRHALT+toffset);	
					setGroupState(1,toffset);	
					incCnt(toffset);
					changed = 0;
					DEBUG_PRINT(F("incCnt0("));
					DEBUG_PRINT(toffset);
					DEBUG_PRINT(F("): "));
					DEBUG_PRINTLN(getCntValue(toffset));
				}else if(s==1)
				{//se il motore è in attesa
					//sono pressioni di configurazione
					//faccio ripartire il timer di attesa del moto a vuoto
					resetTimer(TMRHALT+toffset);
					startTimer(btndelay[n],TMRHALT+toffset);	
					incCnt(toffset);
					changed = 0;
					DEBUG_PRINT(F("\nincCnt1("));
					DEBUG_PRINT(toffset);
					DEBUG_PRINT(F("): "));
					DEBUG_PRINTLN(getCntValue(toffset));
				}else  if(s==2 || s==3)//se il motore è in moto a vuoto o in moto cronometrato
				{
					secondPress(n);
					changed = 1; //solo adesso il loop deve intervenire!
					DEBUG_PRINTLN(F("stato 0: il motore va in stato fermo "));
				}
			}
		}
		else
		{
			DEBUG_PRINTLN(F("fronte di discesa "));
			//fronte di discesa
			//rilascio interblocco
			outlogicp[SW1ONS+offset+sw]=false;
			changed=255;
			//ferma il timer di reset
			//Tasto rilasciato: blocca il timer di reset
			resetTimer(RESETTIMER);
			//DEBUG_PRINTLN(F("Bloccato il timer di reset"));
		}
	}
	//DEBUG_PRINT(F("Changed: "));
	//DEBUG_PRINTLN(changed);
	return changed;
}

byte getDelayedCmd(byte i){
	return lastCmd[i];
}

void setDelayedCmd(byte in, byte i){
	lastCmd[i]=in;
}

void setBtnDelay(byte dly, byte i){
	btndelay[i]=dly;
}

void setHaltDelay(unsigned int dly, byte n){
	DEBUG_PRINT(F("setHaltDelay: "));
	DEBUG_PRINTLN(dly);
	haltdelay[n]=dly;
}
/*
byte tapparellaLogic(byte *in, byte *inr, byte *outlogic, unsigned long thalt, byte n){
	inp=in;
	inrp=inr;
	outlogicp=outlogic;
	thaltp[n]=thalt;
	tapparellaLogic(n);
}
*/
void initTapparellaLogic(byte *in, byte *out, byte *inr, byte *outlogic, String  *confcmdi, bool firstTime=false){
	confcmdl=confcmdi;
	inp=in;
	outp=out;
	inrp=inr;
	outlogicp=outlogic;
	thaltp[0]=(confcmdl[THALT1]).toInt();
	thaltp[1]=(confcmdl[THALT3]).toInt();
	engdelay[0]=(confcmdl[STDEL1]).toInt();
	engdelay[1]=(confcmdl[STDEL2]).toInt();
	btndelay[0]=BTNDEL1;
	btndelay[1]=BTNDEL2;
	btndelay[2]=BTNDEL1;
	btndelay[3]=BTNDEL2;
	haltdelay[0]=(confcmdl[THALT1]).toInt();
	haltdelay[1]=(confcmdl[THALT2]).toInt();
	haltdelay[2]=(confcmdl[THALT3]).toInt();
	haltdelay[3]=(confcmdl[THALT4]).toInt();
	engdelay[2]=(confcmdl[STDEL1]).toInt();
	engdelay[3]=(confcmdl[STDEL2]).toInt();
	taplen=(confcmdl[TLENGTH]).toFloat();
	//correzzione per tapparelle a fisarmonica
	float r = (confcmdl[SLATSRATIO]).toFloat();
	taplen = taplen*(1 + r);
	posdelta = r / (1 + r)*100;
	barrad=(confcmdl[BARRELRAD]).toFloat();
	tapthick=(confcmdl[THICKNESS]).toFloat();
	resetCronoCount(0);
	resetCronoCount(1);
	setCronoLimits(-THALTMAX,THALTMAX,0);
	setCronoLimits(-THALTMAX,THALTMAX,1);
	//rollmode[0] = (confcmdl[SWROLL1]).toInt();
	//rollmode[1] = (confcmdl[SWROLL2]).toInt();
#if (!AUTOCAL) 
	//thaltp[0]=(confcmdl[THALT1]).toInt();
	//thaltp[1]=(confcmdl[THALT3]).toInt();
	first[0] = first[1] = firstTime;
#else
	fact = (float) ENDFACT/100.0;
#endif	
	deltal=DELTAL;
	nmax = (double) ((double) sqrt((double) taplen*tapthick/PI + barrad*barrad) - barrad) / tapthick;		//100% of excursion
	//nmax = ((double) sqrt((double) ((taplen-deltal)*tapthick)/TWO_PI + barrad*barrad) - barrad) / tapthick /2 + deltal/TWO_PI/barrad;		//100% of excursion
}

float getNmax(){
	return nmax;
}

float getTaplen(){
	return taplen;
}

void setTapThalt(unsigned long thalt,byte n){
	thaltp[n]=thalt;
}

unsigned long getTapThalt(byte n){
	return thaltp[n];
}

long getTarget(byte n){
	return target[n];
}

void setTarget(long val, byte n){
	target[n] = val;
}

byte getGroupState(byte n){
	return groupState[n];
}
	
void setGroupState(byte tstate, byte n){
	groupState[n]=tstate;
}

void startEndOfRunTimer(byte n){
	int toffset=n*TIMERDIM;
	
	if(calibr==1)
		resetCronoCount(n);
	//il motore è in moto libero
	moving[n]=true;	
	//faccio partire il timer di attesa dell'arrivo nella posizione finale
	//il motore va in moto cronometrato
	startTimer(target[toffset],TMRHALT+toffset);
	//comincia a cronometrare la corsa
	startCrono(n); 
	setGroupState(3,toffset);	//														stato 3: il motore va in moto cronometrato
	DEBUG_PRINT(F("stato 3: il motore " ));
	DEBUG_PRINT(n);
	DEBUG_PRINT(F(" al tempo  " ));
	DEBUG_PRINT(getCronoCount(n));
	DEBUG_PRINT(F(" è cronometrato verso "));
	DEBUG_PRINTLN(target[toffset]);	
}

bool startEngineDelayTimer(byte n){
	    int toffset=n*TIMERDIM;
		
		moving[n]=false;	
		setGroupState(2,n);												//stato 2: il motore va in moto a vuoto (o libero)
		byte btn = (short) (1-getCronoDir(n))/2+ n*BTNDIM;  //conversion from direction to index
		//DEBUG_PRINTLN(F("startEngineDelayTimer: getGroupState(n), btn, inp[btn]"));
		//DEBUG_PRINTLN(getGroupState(n));
		//DEBUG_PRINTLN(btn);
		//DEBUG_PRINTLN(inp[btn]);
		inp[btn] = lastCmd[btn];
		firstPress((getCronoDir(n)==DOWN), n);
		//parte il timer di attesa del moto cronometrato
		startTimer(engdelay[toffset],TMRHALT+toffset);
		
		DEBUG_PRINTLN(F("stato 2: il motore va in moto a vuoto"));
		return true;
		//}else{
		//	return false;
		//}	
}

short secondPress(byte n, int delay, bool end){
	int offset=n*STATUSDIM;
	int toffset=n*TIMERDIM;
	int poffset=n*BTNDIM;
	short rslt = 0;
	
	moving[n] = false;
	//reset service count
	resetCnt(toffset);
	if(calibr == 0){
		//either UP or DOWN
		//da effettuare solo se il motore è inp moto
		//LIST OF STOP ACTIONS
		resetTimer(TMRHALT+toffset);//blocca timer di fine corsa		
		//blocca il motore
		outlogicp[ENABLES+offset]=LOW;
		nrun--;
		outlogicp[DIRS+offset]=LOW;
		//setGroupState(0,n);												//stato 0: il motore va in stato fermo
		addCronoCount(stopCrono(n)-delay, (short) getCronoDir(n),n); 
		long app = getCronoCount(n);
#if (AUTOCAL) 
		if(getGroupState(n) == 3 && end){
			if(getCronoDir(n)==UP ){
				//versione con correzzione continua della posizione escursione in base alla lettura dei due sensori H e L
				//l'ampiezza dell'escursione attualmente non viene modificata ed è stimata solo in fase di calibrazione
				if(getCronoCount(n) > (long) thaltp[n]*(1-fact) && getCronoCount(n) < (long) thaltp[n]*(1+fact)){
					//correzione posizione tetto escursione 
					//setCronoCount(thaltp[n], n);
					//stima base escursione da parte del sensore H (alto)
					base[n] = (long) getCronoCount(n)-thaltp[n];
					DEBUG_PRINT(F("tapparella impiega un tempo leggermente diverso dalla stima per apertura totale. Correzione posizione necessaria: "));
					DEBUG_PRINTLN(base[n]);
					//elastico parte alta (correzione ampiezza escursione)
					//thaltp[n] = getCronoCount(n);	//elastico parte alta (correzione escursione)
					rslt = 0;
				}else{
					//correzione posizione tetto escursione (necessaria all'avvio!)
					setCronoCount(thaltp[n], n);
					rslt = 2;
					DEBUG_PRINTLN(F("Correzione posizione tetto escursione. Prima escursione o forzatura...può essere necessaria ricalibrazione"));
				}
			}else{
				if(getCronoCount(n) < (long) thaltp[n]*fact && getCronoCount(n) > (long) -thaltp[n]*fact){
					//resetCronoCount(n);
					rslt = 0;
					DEBUG_PRINT(F("tapparella impiega un tempo leggermente diverso dalla stima per chiusura totale. Correzione posizione necassaria: "));
					//stima base escursione da parte del sensore L (basso)
					long app = (long) getCronoCount(n);
					//calcolo media delle due stime sulla base escursione
					app = (app + base[n])/2;
					DEBUG_PRINTLN(app);
					//elastico parte bassa (correzione ampiezza escursione)
					//app = thaltp[n] + getCronoCount(n);  
					//thaltp[n] = app;
					//correzione posizione base escursione 
					setCronoCount(app, n);
					//resetCronoCount(n);
				}else{
					//correzione posizione base escursione (necessaria all'avvio!)
					resetCronoCount(n);
					rslt = 3;
					DEBUG_PRINTLN(F("Grande correzione della posizione della base escursione, può essere necessaria ricalibrazione"));
				}
			}
		}else if(getGroupState(n) == 2 && end){
			if(getCronoDir(n)==UP){
					//correzione posizione tetto escursione (necessaria all'avvio!)
					setCronoCount(thaltp[n], n);
					rslt = 2;
					DEBUG_PRINTLN(F("Correzione posizione tetto escursione. Prima escursione o forzatura...può essere necessaria ricalibrazione"));
			}else{
					//correzione posizione base escursione (necessaria all'avvio!)
					resetCronoCount(n);
					rslt = 3;
					DEBUG_PRINTLN(F("Grande correzione della posizione della base escursione, può essere necessaria ricalibrazione"));
			}
		}
#else		
		if(first[n] == true){
			if(getCronoCount(n) >= (long) thaltp[n]){
				first[n] = false;
				setCronoCount(thaltp[n], n);
				DEBUG_PRINT(F("max rilevato: "));
				DEBUG_PRINTLN(first[n]);
			}else if(getCronoCount(n) <= (long) -thaltp[n]){
				first[n] = false;
				setCronoCount(0, n);
				DEBUG_PRINT(F("min rilevato: "));
				DEBUG_PRINTLN(first[n]);
			}
		}
#endif
		DEBUG_PRINT(F("\nSecond press: motore fermo al tempo "));
		DEBUG_PRINTLN(app);
		setGroupState(0,n);
	}else if(calibr == 1){
		resetTimer(TMRHALT+toffset);//blocca timer di fine corsa		
		//blocca il motore
		outlogicp[ENABLES+offset]=LOW;
		nrun--;
		outlogicp[DIRS+offset]=LOW;
		DEBUG_PRINTLN(F("2)reset del cronometro immediatamente prima della salita"));
		//Tapparella completamente abbassata: imposto a zero il contatore di stato
		resetCronoCount(n);
		incCnt(toffset);
		setCronoDir((short)-getCronoDir(n),n);  //reverse direction
		byte btn = (short) (1-getCronoDir(n))/2+ n*BTNDIM;  //conversion from direction to index 
		lastCmd[btn] = 101;
		setGroupState(1,n);			//stato 1: il sitema va in stato di attesa		
#if(AUTOCAL)
		//tempo di ripartenza (non corto per far riposare il motore ed evitare spikes di corrente)
		startTimer(1500,TMRHALT+toffset);
#else
		startTimer(500,TMRHALT+toffset);
#endif	
		calibr = 2;
		DEBUG_PRINTLN(btn);
		DEBUG_PRINTLN(lastCmd[btn]);
		DEBUG_PRINT(F("stato 2: il motore va in moto a vuoto per calibrazione verso "));
		DEBUG_PRINTLN(getCronoDir(n));
		DEBUG_PRINT(F("FASE 2 CALIBRAZIONE MANUALE"));
		DEBUG_PRINTLN(F("-----------------------------"));
		DEBUG_PRINTLN(F("LA TAPPARELLA STA salendo"));
		DEBUG_PRINT(F("PREMERE UN PULSANTE QUALSIASI DEL GRUPPO ATTIVO"));
		DEBUG_PRINTLN(F("-----------------------------"));
		//resetStatDelayCounter(n);
	}else if(calibr == 2){
		calibr = 0;
		resetTimer(TMRHALT+toffset);//blocca timer di fine corsa		
		//blocca il motore
		outlogicp[ENABLES+offset]=LOW;
		nrun--;
		outlogicp[DIRS+offset]=LOW;
		setGroupState(0,n);												//stato 0: il motore va in stato fermo
		//blocca il cronometro di DOWN
		//addCronoCount(stopCrono(n), (short) getCronoDir(n),n);
		//stopCrono(n);
		addCronoCount((long)stopCrono(n)-delay, (short) getCronoDir(n),n);
		long app = (long) getCronoValue(n) - delay;
		//unsigned int app = getCronoCount(n);
		setCronoCount( app*(getCronoDir(n)==UP),n);
		
		if(app < CNTIME*1000)
			app = CNTIME*1000;
		//setCronoLimits(0,app,n);
		thaltp[n] = app;
		DEBUG_PRINT(F("getCronoValue(n): "));
		DEBUG_PRINTLN(getCronoValue(n));
		DEBUG_PRINT(F("getCronoCount(n): "));
		DEBUG_PRINTLN(getCronoCount(n));
		DEBUG_PRINTLN(F("-----------------------------"));
		DEBUG_PRINT(F("FASE 3 CALIBRAZIONE MANUALE BTN "));
		DEBUG_PRINTLN(n+1);
		DEBUG_PRINT(F("SALVATAGGIO TEMPO DI SEC "));
		DEBUG_PRINTLN(app);
		onCalibrEnd(app,n);
	}else{
		calibr = 0;
	}
	onTapStop(n);
	
	copytout();
	return rslt;
}

void firstPress(byte sw, byte n){
	int offset=n*STATUSDIM;
	int toffset=n*TIMERDIM;
	int poffset=n*BTNDIM;
	
	//da effettuare solo se il motore è fermo
	//DEBUG_PRINT(F("\nPrima pressione UP: motore "));
	//DEBUG_PRINT(n+1);
	//DEBUG_PRINT(F("\ninp[BTN1IN+poffset] "));
	//DEBUG_PRINTLN(inp[BTN1IN+poffset]);
		
	//resetTimer(TMRHALT+toffset);			
	if(inp[BTN1IN+poffset+sw] == 255){
		//DEBUG_PRINT(F(" in moto verso "));
		//DEBUG_PRINTLN(sw);
		//LIST OF UP ACTIONSalt 
		//imposta la DIRSezione
		outlogicp[DIRS+offset]=sw;	
		//fai partire il timer di fine corsa
		setCronoDir(upmap[sw],n);
#if (AUTOCAL)
		//Blocco di sicurezza in caso di rottura della sensoristica di fine corsa	
		//target[n] = ENDFACT*(thaltp[n]) * (!sw);
		//posizionamento agli estremi stabilito dai sensori fino ad un tetto massimo, dopo scatta il posizionamento col timer
		target[toffset] = thaltp[n] * (!sw) + 2*fact*thaltp[n]*getCronoDir(n);
#else
		if(first[n] == true){
			target[toffset] = 1.5*thaltp[n];
		}else{
			target[toffset] = (thaltp[n]) * (!sw);
		}
#endif			
		target[toffset] = (long) (target[toffset]-getCronoCount(n))*getCronoDir(n);
		
		//DEBUG_PRINT(F("first: "));
		//DEBUG_PRINTLN(first[n]);
		//DEBUG_PRINT(F("Target: in moto verso "));
		//DEBUG_PRINTLN(target[n]);

	}else if(inp[BTN1IN+poffset+sw] == 101){
		DEBUG_PRINT(F("Calibrazione: in moto verso "));
		DEBUG_PRINTLN(sw);
		//imposta la DIRSezione
		outlogicp[DIRS+offset]=sw;	
		setCronoDir(upmap[sw],n);
		//setCronoLimits(0,THALTMAX,n);
		if(calibr==0){
			calibr = 1;
		}
		if(calibr == 1 || calibr == 2){
			target[toffset] = 2*THALTMAX;
		}
		//target[n] = (long) THALTMAX;
	}else{
		//aperture percentuali
		unsigned short p = 0;
		//DEBUG_PRINTLN(F(" in moto verso l'alto perc"));

		if(inp[BTN1IN+poffset+sw] <= 100){
			p = inp[BTN1IN+poffset+sw] + posdelta;
			if(p == posdelta){
				//obbliga chiusura di lume porta + doghe
				p = 0;
			}
		}else if(inp[BTN1IN+poffset+sw] <= 210){
			p = inp[BTN1IN+poffset+sw] - 110;
		}
		
		target[toffset] = (unsigned long) (thaltp[n]*calcTiming(p))/100;
		long delta = (long) (target[toffset] - getCronoCount(n));
		if(delta > 0){
			target[toffset] = delta;
			//LIST OF UP ACTIONS (TARGET ABOVE CURRENT POS)
			outlogicp[DIRS+offset]=sw;
			setCronoDir(upmap[sw],n);
		}else
		{ 	//TARGET UNDER CURRENT POS
			target[toffset] = -delta;
			outlogicp[DIRS+offset]=!sw;
			setCronoDir(upmap[!sw],n);
		}
	}
	//abilita il motore
	outlogicp[ENABLES+offset]=HIGH;
	nrun++;
	copytout();
}

bool isRunning(byte n){
	return (outlogicp[ENABLES+n*STATUSDIM]==HIGH);
}

bool isMoving(byte n){
	return moving[n];
}

byte nRunning(){
	return nrun;
}

inline double calcTiming(byte v){
	double nv;
	nv = ((double) sqrt((double) (v*taplen*tapthick)/PI/100 + barrad*barrad) - barrad) / tapthick;	
	/*if(v*taplen < deltal){
		nv = v*deltal/TWO_PI/barrad;
	}else{
		nv = ((double) sqrt((double) ((v*taplen -deltal)*tapthick)/TWO_PI/100 + barrad*barrad) - barrad) / tapthick /2 + deltal/TWO_PI/barrad;
	}*/
	
	nv =  ((double) nv / nmax * 100);
	return  nv;
}

double calcLen(byte n){
	double app = (double) getCronoCount(n)/thaltp[n]*nmax;
	return  (double) PI*app*((double) app*tapthick + 2*barrad)*100/taplen;
}

float getPosdelta(){
	return posdelta;
}

byte toggleLogic(byte sw, byte nn){
	int n=sw+nn*TIMERDIM;
	byte changed = 0;
	
	if(switchdfn(inp[n],n))
	{
		byte s = getGroupState(n);
		
		DEBUG_PRINT(F("\nSwitchLogic: fronte di SWOnOff tasto "));
		DEBUG_PRINT(n+1);
		DEBUG_PRINT(F(" stato "));
		DEBUG_PRINT(s);
		
		//siamo su uno dei fronti del pulsante 
		if(inp[n]>0)  
		{	//fronte di salita
			changed = 0;
			//fronte di salita
			startTimer(RESETTIMER);
			if(s==0)
				{ 	
					//se il pulsante è aperto
					DEBUG_PRINTLN(F("\nSwitchLogic: stato 0: lo switch è inibito e va in attesa da stato 0 (inibito)"));
					DEBUG_PRINT(F("n: "));
					DEBUG_PRINTLN(n);
					//stato 0: lo switch va in stato inibito
					//cambio lo stato dell'uscita (TOGGLE)
					lastCmd[n] = !outp[n];
					startTimer(btndelay[n],n);		
					setGroupState(1,n);	
					incCnt(n);
					changed = 0;
				}else if(s==1)
				{	
					DEBUG_PRINTLN(F("\nSwitchLogic: stato 1: lo switch è inibito e va in attesa da stato 1 (inibito)"));
					DEBUG_PRINT(F("n: "));
					DEBUG_PRINTLN(n);
					//se lo switch è inibito
					//sono pressioni di configurazione
					//faccio ripartire il timer di attesa di abilitazione
					resetTimer(n);
					startTimer(btndelay[n],n);	
					incCnt(n);
					changed = 0;
				}
		}else{
			DEBUG_PRINTLN(F("\nSwitchLogic: fronte di discesa onOff"));
			//fronte di discesa
			changed=255;
			//ferma il timer di reset
			//Tasto rilasciato: blocca il timer di reset
			resetTimer(RESETTIMER);
		}
	}
	return changed;
}

void setLogic(byte in, byte n){
	outp[n] = in;
	DEBUG_PRINT(F("outp: "));
	DEBUG_PRINT(n);
	DEBUG_PRINT(F(" val: "));
	DEBUG_PRINTLN(outp[n]);
}

void setOE(bool in, byte n){
	oe[n] = in;
	DEBUG_PRINT(F("OE: "));
	DEBUG_PRINT(n);
	DEBUG_PRINT(F(" val: "));
	DEBUG_PRINTLN(oe[n]);
}

void setActionLogic(int in, byte n){
	//0: setReset
	//1: nessuna azione
	//2: normalmente aperto, chiuso per un haltdelay alla pressione
	//3: normalmente chiuso, aperto per un haltdelay alla pressione
	if(act[n]==0){
		DEBUG_PRINT(F("setActionLogic setReset: "));
		DEBUG_PRINTLN(in);
		if(in>=LOW && oe[n])
			setLogic(in,n);
	}else if(act[n]==1){
		DEBUG_PRINT(F("setActionLogic doNothing: "));
		DEBUG_PRINTLN(in);	
	}else if(act[n]==2){
		DEBUG_PRINT(F("setActionLogic monoNormalAperto: "));
		DEBUG_PRINTLN(in);
		lastCmd[n] = HIGH;
		if(in>=LOW && oe[n])
			if(in==HIGH)
				startSimpleSwitchDelayTimer(n);	
			else
				setLogic(LOW,n);
	}else if(act[n]==3){
		DEBUG_PRINT(F("setActionLogic monoNormalChiuso: "));
		DEBUG_PRINTLN(in);
		lastCmd[n] = LOW; 
		if(in>=LOW && oe[n])
			if(in==HIGH)
				startSimpleSwitchDelayTimer(n);	
			else
				setLogic(HIGH,n);
	}
}

void setSWAction(byte in, byte n){
	act[n] = in;
	//0: toggleLogic
	//1: condition output disabled
	//2: normalmente aperto, chiuso per un haltdelay alla pressione
	//3: normalmente chiuso, aperto per un haltdelay alla pressione
	if(in==0){
		DEBUG_PRINT(F("setSWAction toggleLogic: "));
		DEBUG_PRINTLN(in);
		oe[n]=true;
		haltdelay[n] = 0;
	}else if(in==1){
		DEBUG_PRINT(F("setSWAction output disabled: "));
		DEBUG_PRINTLN(in);
		haltdelay[n] = 0;
		DEBUG_PRINT(F("act==1: "));
	}else if(in==2){
		DEBUG_PRINT(F("setSWAction normalmente aperto: "));
		DEBUG_PRINTLN(in);
		oe[n]=true;
		haltdelay[n]=(confcmdl[THALT1+n]).toInt();
		setLogic(LOW,n);
		DEBUG_PRINT(F("act==2: "));
	}else if(in==3){
		DEBUG_PRINT(F("setSWAction normalmente chiuso: "));
		DEBUG_PRINTLN(in);
		oe[n]=true;
		haltdelay[n]=(confcmdl[THALT1+n]).toInt();
		setLogic(HIGH,n);
		DEBUG_PRINT(F("act==3: "));
	}
}
	
bool startSimpleSwitchDelayTimer(byte n){	
	//moving[n]=false;
	//stato 5: switch in contatto chiuso		
	setGroupState(0,n);	
	DEBUG_PRINTLN(F("startSimpleSwitchDelayTimer: getGroupState(n), n, lastCmd[n], act[n]"));
	DEBUG_PRINTLN(getGroupState(n));
	DEBUG_PRINTLN(n);
	DEBUG_PRINTLN(lastCmd[n]);
	DEBUG_PRINTLN(act[n]);
	setLogic(lastCmd[n],n);
	//il timer di inversione contatto parte solo se il tempo è > 0
	if((act[n]==2 || act[n]==3) && haltdelay[n]>0){
		startTimer(haltdelay[n],n);
		setGroupState(2,n);	
		setCntValue(1,n);
	}else{
		resetCnt(n);
	}
	DEBUG_PRINTLN(F("\nstartSimpleSwitchDelayTimer switch mode: il contatto va in stato modifica abilitata"));
	return true;
}

void startPress(byte state,byte n){
	//cambio lo stato dell'uscita (SET)
	lastCmd[n]=state;
	startTimer(haltdelay[n],n);	
	setGroupState(1,n);	
	DEBUG_PRINTLN(F("startPress: getGroupState(n), n, lastCmd[n]"));
	DEBUG_PRINTLN(getGroupState(n));
	DEBUG_PRINTLN(n);
	DEBUG_PRINTLN(lastCmd[n]);
}

void endPress(byte n){
	setLogic(!lastCmd[n],n);
	setGroupState(0,n);	
	resetCnt(n);
	DEBUG_PRINTLN(F("endPress: getGroupState(n), n, lastCmd[n]"));
	DEBUG_PRINTLN(getGroupState(n));
	DEBUG_PRINTLN(n);
	DEBUG_PRINTLN(!lastCmd[n]);
	DEBUG_PRINTLN(haltdelay[n]);
}
/*
*******************************************************************
* FORMULE ADOPERATE PER LA STIMA DELLA POSIZIONE DELLE TAPPARELLE *
*******************************************************************
l: 		lunghezza arrotolata = altezza percorsa dal suolo
lmax: 	lunghezza tapparella 
t: 		tempo di arrivo alla posizione finale
tmax: 	tempo di escursione massimo della tapparella
tf: 	spessore tapparella
br: 	raggio tamburo
n: 		numero di giri percorso fino al posizionamento
nmax: 	numero di giri percorso da completamente aperto a completamente chiuso
omega1: velocità angolare tapparella all'istante 1
omega2: velocità angolare tapparella all'istante 2

	lp = l/lmax = lp(%) / 100 		frazione della lunghezza di arrivo sulla lunghezza massima
	tp = t/tmax	= tp(%) / 100 		frazione del tempo di arrivo sul tempo massimo di escursione della tapparella


n 	 = omega1 * t
nmax = omega2 * tmax	

ipotesi di base (abbastanza vera):   	omega1 = omega2


calcTiming:  lp ---> tp(%)
***************************

n = (sqrt(l*tf/PI + br^2) - br) / tf   --->  n = (sqrt(lp*lmax*tf/PI + br^2) - br) / tf

se omega1 = omega2   --->   tp(%) = t/tmax = n/nmax * 100 


calcLen:  t ---> lp(%)
***********************
                        n = t / tmax * nmax = tp * nmax
						
						l = PI*n*(n*tf + 2*br) 
						
						lp(%) = l/lmax * 100 
*/
/*
*******************************************************************
* FORMULE ADOPERATE PER LA GESTIONE DELLE TAPPARELLE A FISRMONICA *
*******************************************************************
Grandezze in gioco:
hd: altezza di una stecca
h1: altezza del lume della finestra (da terra a bordo superiore infisso)
h2: misura calcolata (stimata) della lunghezza effettiva della tapprella nella sua massima lunghezza (fisarmonica aperta)
dh: altezza massima fessura cioè della ceriera tra una stecca e l'altra
r: rapporto di "allungamento" tra altezza fessura (cerniera tra stecche) e altezza stecca
n: numero di stecche totali
da: rapporto in percentuale tra allungamento della tapparella da fisarmonica completamente chiusa a fisarmonica tutta aperta

Si sono usate le relazioni:

def a)			h2 = h1 + n*dh

def b)			dn = n * dh/h2

def c)			r = dh/hd

rel 1) 			h2 = h1 * (1 + r)  nell'ipotesi (approssimata) che h1 = n * hd , cioè che non ci siano stecche fuori dal lume della finestra

rel 2)			da = r / (1 + r) * 100	

*/