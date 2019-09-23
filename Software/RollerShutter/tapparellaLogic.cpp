#include "tapparellaLogic.h"

int count1 = 0;
byte groupState[4];
byte *inp;
byte *outp;
byte *outlogicp;
byte act[4] = {0,0,0,0};
float taplen, deltal;
float barrad;
float tapthick; 
long thaltp[2]={THALTMAX/2,THALTMAX/2};
long base[2]={0,0};
bool oe[4];  //output enable
short endrun[2] = {false, false};
#if (AUTOCAL) 
float fact;
#endif
byte lastCmd[4]; //{sw1,sw2,sw3,sw4}
byte triggered[4]= {false, false, false, false};
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
bool rollmode[2] = {true,true};

inline bool switchdfn(byte val, byte n){
	//n: numero di pulsanti
	val = (val>0);								//SEMPLICE!!!!
	bool changed = (val != dprecval2[n]);
	dprecval2[n] = val;            // valore di val campionato al loop precedente 
	return changed;
}

inline void copytout(byte n){
#if (SCR)
	 outp[0 + n*BTNDIM]=(outlogicp[ENABLES + n*STATUSDIM] && (outlogicp[DIRS + n*STATUSDIM]==LOW));	
	 outp[1 + n*BTNDIM]=(outlogicp[ENABLES + n*STATUSDIM] && (outlogicp[DIRS + n*STATUSDIM]==HIGH));		
#else		
	 outp[0 + n*BTNDIM]=outlogicp[ENABLES + n*STATUSDIM];	
	 outp[1 + n*BTNDIM]=outlogicp[DIRS + n*STATUSDIM];					 
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
		DEBUG2_PRINT(F("fronte di SW: "));
		DEBUG2_PRINTLN(BTN1IN+poffset+sw+1);
		DEBUG2_PRINT(F("stato switch "));
		DEBUG2_PRINTLN(toffset);
		DEBUG2_PRINT(F(": "));
		DEBUG2_PRINTLN(getGroupState(toffset));
		
		byte s = getGroupState(toffset);
		//siamo su uno dei fronti del pulsante 
		if(inp[BTN1IN+poffset+sw]>0)  
		{			
			changed = 0;
			//fronte di salita
			startTimer(RESETTIMER);
			outlogicp[SW1ONS+offset+sw]=true;
			if(!outlogicp[SW1ONS+offset+!sw]){  //evita attivazioni con pressione contemporanea di due tasti (interblocco)
				DEBUG2_PRINTLN(F("dopo interblocco "));
				//modalità tapparella
				//effettuata prima pressione
				if(s==0)
				{ 	
					//se il motore è fermo
					setCronoDir(upmap[sw],n);  //in base a questo viene decisa la direzione della partenza differita
					lastCmd[BTN1IN + poffset+sw] = inp[BTN1IN+poffset+sw];
					DEBUG2_PRINTLN(F("tapparellaLogic: stato 1: il motore va in attesa da stato 0 (fermo)"));
					//stato 1: il motore va in attesa del moto a vuoto
					startTimer(btndelay[n],TMRHALT+toffset);	
					setGroupState(1,toffset);	
					incCnt(toffset);
					changed = 0;
					DEBUG2_PRINT(F("incCnt0("));
					DEBUG2_PRINT(toffset);
					DEBUG2_PRINT(F("): "));
					DEBUG2_PRINTLN(getCntValue(toffset));
				}else if(s==1)
				{//se il motore è in attesa
					//sono pressioni di configurazione
					//faccio ripartire il timer di attesa del moto a vuoto
					resetTimer(TMRHALT+toffset);
					startTimer(btndelay[n],TMRHALT+toffset);	
					incCnt(toffset);
					changed = 0;
					DEBUG2_PRINT(F("\nincCnt1("));
					DEBUG2_PRINT(toffset);
					DEBUG2_PRINT(F("): "));
					DEBUG2_PRINTLN(getCntValue(toffset));
				}else  if(s==2 || s==3)//se il motore è in moto a vuoto o in moto cronometrato
				{
					secondPress(n);
					changed = 1; //solo adesso il loop deve intervenire!
					DEBUG2_PRINTLN(F("stato 0: il motore va in stato fermo "));
				}
			}
		}
		else
		{
			DEBUG2_PRINTLN(F("fronte di discesa "));
			//fronte di discesa
			//rilascio interblocco
			outlogicp[SW1ONS+offset+sw]=false;
			changed=255;
			//ferma il timer di reset
			//Tasto rilasciato: blocca il timer di reset
			resetTimer(RESETTIMER);
			//DEBUG2_PRINTLN(F("Bloccato il timer di reset"));
		}
	}
	//DEBUG2_PRINT(F("Changed: "));
	//DEBUG2_PRINTLN(changed);
	return changed;
}

void resetOutlogic(byte n){
	outlogicp[SW1ONS+n*STATUSDIM+0]=false;
	outlogicp[SW1ONS+n*STATUSDIM+1]=false;
}

void printOutlogic(){
	DEBUG1_PRINT(F("Outlogic: "));
	DEBUG1_PRINT(outlogicp[SW1ONS+0*STATUSDIM+0]);
	DEBUG1_PRINT(F(", "));
	DEBUG1_PRINT(outlogicp[SW1ONS+0*STATUSDIM+1]);
	DEBUG1_PRINT(F(", "));
	DEBUG1_PRINT(outlogicp[SW1ONS+1*STATUSDIM+0]);
	DEBUG1_PRINT(F(", "));
	DEBUG1_PRINTLN(outlogicp[SW1ONS+1*STATUSDIM+1]);
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
	DEBUG2_PRINT(F("setHaltDelay: "));
	DEBUG2_PRINTLN(dly);
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
void setSWModeTap(uint8_t mode, uint8_t n){
	rollmode[n] = mode;
	DEBUG2_PRINT("setSWModeTap");
	DEBUG2_PRINTLN(n);
	DEBUG2_PRINT(": ");
	DEBUG2_PRINTLN(mode);
	
	//readModeAndPub(n);
}

uint8_t getSWMode(uint8_t n){
	return rollmode[n];
}

void setSTDelays(unsigned long dly1, unsigned long dly2){
	engdelay[0] = dly1;
	engdelay[1] = dly2;
	engdelay[2] = dly1;
	engdelay[3] = dly2;
}

void setTHalts(unsigned long hlt1, unsigned long hlt2, unsigned long hlt3, unsigned long hlt4){
	haltdelay[0] = hlt1;
	haltdelay[1] = hlt2;
	haltdelay[2] = hlt3;
	haltdelay[3] = hlt4;
	
	thaltp[0] = hlt1;
	thaltp[1] = hlt3;	
	
	setupTimer(hlt1, TMRHALT);
	setupTimer(hlt2, TMRHALT + TIMERDIM); 
	setupTimer(hlt3, TMRHALT2);
	setupTimer(hlt4, TMRHALT2 + TIMERDIM);
}

void setSLRatio(float slratio){
	float r = slratio;
	taplen = taplen*(1 + r);
	posdelta = r / (1 + r)*100;
}

void setBarrRadius(float rad){
	barrad = rad;
}

void setThickness(float thick){
	tapthick = thick;
}

void setTapLen(float len){
	taplen = len;
}

void initTapparellaLogic(byte *in, byte *out, byte *outlogic, bool firstTime=false){
	inp=in;
	outp=out;
	outlogicp=outlogic;
	/*
	thaltp[0] = static_cast<ParLong*>(parsl[p(THALT1)])->val;
	thaltp[1] = static_cast<ParLong*>(parsl[p(THALT3)])->val;
	engdelay[0] = static_cast<ParLong*>(parsl[p(STDEL1)])->val;
	engdelay[1] = static_cast<ParLong*>(parsl[p(STDEL2)])->val;
	engdelay[2] = static_cast<ParLong*>(parsl[p(STDEL1)])->val;
	engdelay[3] = static_cast<ParLong*>(parsl[p(STDEL2)])->val;
	haltdelay[0] = static_cast<ParLong*>(parsl[p(THALT1)])->val;
	haltdelay[1] = static_cast<ParLong*>(parsl[p(THALT2)])->val;
	haltdelay[2] = static_cast<ParLong*>(parsl[p(THALT3)])->val;
	haltdelay[3] = static_cast<ParLong*>(parsl[p(THALT4)])->val;
	taplen = static_cast<ParFloat*>(parsl[p(TLENGTH)])->val;
	//correzzione per tapparelle a fisarmonica
	float r = static_cast<ParFloat*>(parsl[p(SLATSRATIO)])->val;
	taplen = taplen*(1 + r);
	posdelta = r / (1 + r)*100;
	barrad = static_cast<ParFloat*>(parsl[p(BARRELRAD)])->val;
	tapthick = static_cast<ParFloat*>(parsl[p(THICKNESS)])->val;
	*/
	btndelay[0] = BTNDEL1;
	btndelay[1] = BTNDEL2;
	btndelay[2] = BTNDEL1;
	btndelay[3] = BTNDEL2;
	//correzzione per tapparelle a fisarmonica
	resetCronoCount(0);
	resetCronoCount(1);
	setCronoLimits(-THALTMAX,THALTMAX,0);
	setCronoLimits(-THALTMAX,THALTMAX,1);
#if (!AUTOCAL) 
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

bool isCalibr(){
	return calibr == 1 || calibr == 2;
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
	startCrono(n); //////////////////////////////
	setGroupState(3,toffset);	//														stato 3: il motore va in moto cronometrato
	DEBUG1_PRINT(F("stato 3: il motore " ));
	DEBUG1_PRINT(n);
	DEBUG1_PRINT(F(" al tempo del timer 0 e al tempo del cronometro 0 è in moto cronometrato verso il tempo " ));
	DEBUG1_PRINTLN(target[toffset]);	
}

bool startEngineDelayTimer(byte n){
	    int toffset=n*TIMERDIM;
		
		moving[n]=false;
		endrun[n] = 0;
		setGroupState(2,n);												//stato 2: il motore va in moto a vuoto (o libero)
		byte btn = (short) (1-getCronoDir(n))/2+ n*BTNDIM;  //conversion from direction to index
		//DEBUG2_PRINTLN(F("startEngineDelayTimer: getGroupState(n), btn, inp[btn]"));
		//DEBUG2_PRINTLN(getGroupState(n));
		//DEBUG2_PRINTLN(btn);
		//DEBUG2_PRINTLN(inp[btn]);
		inp[btn] = lastCmd[btn];
		firstPress((getCronoDir(n)==DOWN), n);
		//parte il timer di attesa del moto cronometrato
		startTimer(engdelay[toffset],TMRHALT+toffset);
		
		DEBUG2_PRINTLN(F("stato 2: il motore va in moto a vuoto"));
		return true;
		//}else{
		//	return false;
		//}	
}

short isEndRun(byte n){
	DEBUG1_PRINT(F("isEndRun: "));
	DEBUG1_PRINTLN(endrun[n]);
	return endrun[n];
}

bool isOnTarget(byte tgt, byte n){
	unsigned short p = 0;
	if(tgt <= 100){
		p = tgt + posdelta;
		if(p == posdelta){
			//obbliga chiusura di lume porta + doghe
			p = 0;
		}
	}else if(tgt <= 210){
		p = tgt - 110;
	}else if(tgt == 255){
		p = 100;
	}
		
	unsigned long t = (unsigned long) (thaltp[n]*calcTiming(p))/100;
	long dlt = (long) (t - getCronoCount(n));
	bool ont = (dlt > 0) && (dlt < 40) || (dlt < 0) && (-dlt < 40);
	DEBUG1_PRINT(F("tgt: "));
	DEBUG1_PRINT(tgt);
	DEBUG1_PRINT(F("p: "));
	DEBUG1_PRINT(p);
	DEBUG1_PRINT(F("isOnTarget: "));
	DEBUG1_PRINT(ont);
	DEBUG1_PRINT(F(", t: "));
	DEBUG1_PRINT((t));
	DEBUG1_PRINT(F(", delta: "));
	DEBUG1_PRINTLN(dlt);
	return ont;
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
		addCronoCount(stopCrono(n) + delay*getCronoDir(n), (short) getCronoDir(n),n); 
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
					DEBUG2_PRINT(F("tapparella impiega un tempo leggermente diverso dalla stima per apertura totale. Correzione posizione necessaria: "));
					DEBUG2_PRINTLN(base[n]);
					//elastico parte alta (correzione ampiezza escursione)
					//thaltp[n] = getCronoCount(n);	//elastico parte alta (correzione escursione)
					rslt = 0;
				}else{
					//correzione posizione tetto escursione (necessaria all'avvio!)
					setCronoCount(thaltp[n], n);
					rslt = 2;
					DEBUG2_PRINTLN(F("Correzione posizione tetto escursione. Prima escursione o forzatura...può essere necessaria ricalibrazione"));
				}
			}else{
				if(getCronoCount(n) < (long) thaltp[n]*fact && getCronoCount(n) > (long) -thaltp[n]*fact){
					//resetCronoCount(n);
					rslt = 0;
					DEBUG2_PRINT(F("tapparella impiega un tempo leggermente diverso dalla stima per chiusura totale. Correzione posizione necassaria: "));
					//stima base escursione da parte del sensore L (basso)
					long app = (long) getCronoCount(n);
					//calcolo media delle due stime sulla base escursione
					app = (app + base[n])/2;
					DEBUG2_PRINTLN(app);
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
					DEBUG2_PRINTLN(F("Grande correzione della posizione della base escursione, può essere necessaria ricalibrazione"));
				}
			}
		}else if(getGroupState(n) == 2 && end){
			if(getCronoDir(n)==UP){
					//correzione posizione tetto escursione (necessaria all'avvio!)
					setCronoCount(thaltp[n], n);
					rslt = 2;
					DEBUG2_PRINTLN(F("Correzione posizione tetto escursione. Prima escursione o forzatura...può essere necessaria ricalibrazione"));
			}else{
					//correzione posizione base escursione (necessaria all'avvio!)
					resetCronoCount(n);
					rslt = 3;
					DEBUG2_PRINTLN(F("Grande correzione della posizione della base escursione, può essere necessaria ricalibrazione"));
			}
		}
#else		
		if(first[n] == true){
			if(getCronoCount(n) >= (long) thaltp[n]){
				first[n] = false;
				setCronoCount(thaltp[n], n);
				DEBUG2_PRINT(F("max rilevato: "));
				DEBUG2_PRINTLN(first[n]);
			}else if(getCronoCount(n) <= (long) -thaltp[n]){
				first[n] = false;
				setCronoCount(0, n);
				DEBUG2_PRINT(F("min rilevato: "));
				DEBUG2_PRINTLN(first[n]);
			}
		}
#endif
		DEBUG1_PRINTLN(F("Second press: motore fermo al tempo cronometrato "));
		DEBUG1_PRINTLN(app);
		setGroupState(0,n);
	}else if(calibr == 1){
		resetTimer(TMRHALT+toffset);//blocca timer di fine corsa		
		//blocca il motore
		outlogicp[ENABLES+offset]=LOW;
		nrun--;
		outlogicp[DIRS+offset]=LOW;
		DEBUG2_PRINTLN(F("2)reset del cronometro immediatamente prima della salita"));
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
		DEBUG2_PRINTLN(btn);
		DEBUG2_PRINTLN(lastCmd[btn]);
		DEBUG2_PRINT(F("stato 2: il motore va in moto a vuoto per calibrazione verso "));
		DEBUG2_PRINTLN(getCronoDir(n));
		DEBUG2_PRINT(F("FASE 2 CALIBRAZIONE MANUALE"));
		DEBUG2_PRINTLN(F("-----------------------------"));
		DEBUG2_PRINTLN(F("LA TAPPARELLA STA salendo"));
		DEBUG2_PRINT(F("PREMERE UN PULSANTE QUALSIASI DEL GRUPPO ATTIVO"));
		DEBUG2_PRINTLN(F("-----------------------------"));
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
		setCronoCount(app*(getCronoDir(n)==UP),n);
		
		if(app < CNTIME*1000)
			app = CNTIME*1000;
		//setCronoLimits(0,app,n);
		thaltp[n] = app;
		DEBUG2_PRINT(F("getCronoValue(n): "));
		DEBUG2_PRINTLN(getCronoValue(n));
		DEBUG2_PRINT(F("getCronoCount(n): "));
		DEBUG2_PRINTLN(getCronoCount(n));
		DEBUG2_PRINTLN(F("-----------------------------"));
		DEBUG2_PRINT(F("FASE 3 CALIBRAZIONE MANUALE BTN "));
		DEBUG2_PRINTLN(n+1);
		DEBUG2_PRINT(F("SALVATAGGIO TEMPO DI SEC "));
		DEBUG2_PRINTLN(app);
		onCalibrEnd(app,n);
	}else{
		calibr = 0;
	}
	copytout(n);
	onTapStop(n);
	return rslt;
}

void firstPress(byte sw, byte n){
	int offset=n*STATUSDIM;
	int toffset=n*TIMERDIM;
	int poffset=n*BTNDIM;
	
	//da effettuare solo se il motore è fermo
	DEBUG2_PRINT(F("\nPrima pressione UP: motore "));
	DEBUG2_PRINT(n+1);
	DEBUG2_PRINT(F("\ninp[BTN1IN+poffset] "));
	DEBUG2_PRINTLN(inp[BTN1IN+poffset]);
		
	//resetTimer(TMRHALT+toffset);			
	if(inp[BTN1IN+poffset+sw] == 255){
		DEBUG2_PRINT(F(" in moto verso "));
		DEBUG2_PRINTLN(sw);
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
		
		DEBUG2_PRINT(F("first: "));
		DEBUG2_PRINTLN(first[n]);
		DEBUG2_PRINT(F("Target: in moto verso "));
		DEBUG2_PRINTLN(target[n]);

	}else if(inp[BTN1IN+poffset+sw] == 101){
		DEBUG2_PRINT(F("Calibrazione: in moto verso "));
		DEBUG2_PRINTLN(sw);
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
		//DEBUG2_PRINTLN(F(" in moto verso l'alto perc"));

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
	copytout(n);
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
		
		DEBUG2_PRINT(F("\nSwitchLogic: fronte di SWOnOff tasto "));
		DEBUG2_PRINT(n+1);
		DEBUG2_PRINT(F(" stato "));
		DEBUG2_PRINT(s);
		
		//siamo su uno dei fronti del pulsante 
		if(inp[n]>0)  
		{	//fronte di salita
			changed = 0;
			//fronte di salita
			startTimer(RESETTIMER);
			if(s==0)
				{ 	
					//se il pulsante è aperto
					DEBUG2_PRINTLN(F("\nSwitchLogic: stato 0: lo switch è inibito e va in attesa da stato 0 (inibito)"));
					DEBUG2_PRINT(F("n: "));
					DEBUG2_PRINTLN(n);
					//stato 0: lo switch va in stato inibito
					//cambio lo stato dell'uscita (TOGGLE)
					lastCmd[n] = !outp[n];
					startTimer(btndelay[n],n);		
					setGroupState(1,n);	
					incCnt(n);
					changed = 0;
				}else if(s==1)
				{	
					DEBUG2_PRINTLN(F("\nSwitchLogic: stato 1: lo switch è inibito e va in attesa da stato 1 (inibito)"));
					DEBUG2_PRINT(F("n: "));
					DEBUG2_PRINTLN(n);
					//se lo switch è inibito
					//sono pressioni di configurazione
					//faccio ripartire il timer di attesa di abilitazione
					resetTimer(n);
					startTimer(btndelay[n],n);	
					incCnt(n);
					changed = 0;
				}
		}else{
			DEBUG2_PRINTLN(F("\nSwitchLogic: fronte di discesa onOff"));
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
	DEBUG2_PRINT(F("outp: "));
	DEBUG2_PRINT(n);
	DEBUG2_PRINT(F(" val: "));
	DEBUG2_PRINTLN(outp[n]);
}

void setDiffLogic(byte in, byte n){
	(outp[n] != in) && (outp[n] = in);
	DEBUG2_PRINT(F("outp: "));
	DEBUG2_PRINT(n);
	DEBUG2_PRINT(F(" val: "));
	DEBUG2_PRINTLN(outp[n]);
}

void setOE(bool in, byte n){
	oe[n] = in;
	DEBUG2_PRINT(F("OE: "));
	DEBUG2_PRINT(n);
	DEBUG2_PRINT(F(" val: "));
	DEBUG2_PRINTLN(oe[n]);
}

void setActionLogic(int in, byte nn){
	//int n = nn / TIMERDIM;
	//0: setReset
	//1: nessuna azione
	//2: normalmente aperto, chiuso per un haltdelay alla pressione
	//3: normalmente chiuso, aperto per un haltdelay alla pressione
	//if(rollmode[n] == false){
	DEBUG1_PRINT(F("setActionLogic nn: "));
	DEBUG1_PRINT(nn);
	DEBUG1_PRINT(F(" , in: "));
	DEBUG1_PRINT(in);
	DEBUG1_PRINT(F(" , oe[nn]: "));
	DEBUG1_PRINTLN(oe[nn]);
	if(in != -1 && oe[nn]){
		if(act[nn]==0){
			DEBUG1_PRINT(F("setActionLogic setReset: "));
			DEBUG1_PRINTLN(in);
			setLogic(in,nn);
		}else if(act[nn]==1){
			DEBUG1_PRINT(F("setActionLogic doNothing: "));
			DEBUG2_PRINTLN(in);	
		}else if(act[nn]==2){
			DEBUG1_PRINT(F("setActionLogic monoNormalAperto: "));
			DEBUG1_PRINTLN(in);
			lastCmd[nn] = HIGH;
			if(in==HIGH)
				startSimpleSwitchDelayTimer(nn);	
			else
				setLogic(LOW,nn);
		}else if(act[nn]==3){
			DEBUG1_PRINT(F("setActionLogic monoNormalChiuso: "));
			DEBUG1_PRINTLN(in);
			lastCmd[nn] = LOW; 
			if(in==HIGH)
				startSimpleSwitchDelayTimer(nn);	
			else
				setLogic(HIGH,nn);
		}
	}
	//}else{
		//oe[nn] == false;
	//}
}

void setDiffActionLogic(int in, byte nn){
	//int n = nn / TIMERDIM;
	//0: setReset
	//1: nessuna azione
	//2: normalmente aperto, chiuso per un haltdelay alla pressione
	//3: normalmente chiuso, aperto per un haltdelay alla pressione
	//if(rollmode[n] == false){
	DEBUG1_PRINT(F("setDiffActionLogic nn: "));
	DEBUG1_PRINT(nn);
	DEBUG1_PRINT(F(" , in: "));
	DEBUG1_PRINT(in);
	DEBUG1_PRINT(F(" , oe[nn]: "));
	DEBUG1_PRINTLN(oe[nn]);
	if(in != -1 && oe[nn]){
		if(act[nn]==0){
			DEBUG1_PRINT(F("setDiffActionLogic setReset: "));
			DEBUG1_PRINTLN(in);
			setDiffLogic(in,nn);
		}else if(act[nn]==1){
			DEBUG1_PRINT(F("setDiffActionLogic doNothing: "));
			DEBUG2_PRINTLN(in);	
		}else if(act[nn]==2){
			DEBUG1_PRINT(F("setDiffActionLogic monoNormalAperto: "));
			DEBUG1_PRINTLN(in);
			if(triggered[nn] == false){
				DEBUG1_PRINT(F("setDiffActionLogic diff ok: "));
				DEBUG1_PRINTLN(outp[nn]);
				if(in==HIGH){
					triggered[nn] = true;
					lastCmd[nn] = HIGH;
					startSimpleSwitchDelayTimer(nn);	
				}else{
					triggered[nn] = false;
					setLogic(LOW,nn);
				}
			}
		}else if(act[nn]==3){
			DEBUG1_PRINT(F("setDiffActionLogic monoNormalChiuso: "));
			DEBUG1_PRINTLN(in);
			if(triggered[nn] == false){
				lastCmd[nn] = LOW; 
				if(in==HIGH){
					triggered[nn] = true;
					startSimpleSwitchDelayTimer(nn);	
				}else{
					triggered[nn] = false;
					setLogic(HIGH,nn);
				}
			}
		}
	}
	//}else{
		//oe[nn] == false;
	//}
}

void setSWAction(byte in, byte n){
	DEBUG1_PRINT(F("setSWAction n: "));
	DEBUG1_PRINT(n);
	DEBUG1_PRINT(F(" , in: "));
	DEBUG1_PRINT(in);
	DEBUG1_PRINT(F(" , oe[nn]: "));
	DEBUG1_PRINTLN(oe[n]);
	//if(rollmode[n] == false){
	act[n] = in;
	//0: toggleLogic
	//1: condition output disabled
	//2: normalmente aperto, chiuso per un haltdelay alla pressione
	//3: normalmente chiuso, aperto per un haltdelay alla pressione
	if(in==0){
		DEBUG2_PRINT(F("setSWAction toggleLogic: "));
		DEBUG2_PRINTLN(in);
		//oe[n]=true;
		haltdelay[n] = 0;
	}else if(in==1){
		triggered[n] = false;
		setLogic(LOW,n);
		DEBUG2_PRINT(F("setSWAction output disabled: "));
		DEBUG2_PRINTLN(in);
		haltdelay[n] = 0;
		DEBUG2_PRINT(F("act==1: "));
	}else if(in==2){
		triggered[n] = false;
		DEBUG2_PRINT(F("setSWAction normalmente aperto: "));
		DEBUG2_PRINTLN(in);
		//oe[n]=true;
		//haltdelay[n] = static_cast<ParLong*>(parsl[p(THALT1+n)])->val;
		setLogic(LOW,n);
		DEBUG2_PRINT(F("act==2: "));
	}else if(in==3){
		triggered[n] = false;
		DEBUG2_PRINT(F("setSWAction normalmente chiuso: "));
		DEBUG2_PRINTLN(in);
		//oe[n]=true;
		//haltdelay[n] = static_cast<ParLong*>(parsl[p(THALT1+n)])->val;
		setLogic(HIGH,n);
		DEBUG2_PRINT(F("act==3: "));
	}
	//}
}
	
bool startSimpleSwitchDelayTimer(byte n){	
	//moving[n]=false;
	//stato 5: switch in contatto chiuso		
	setGroupState(0,n);	
	DEBUG2_PRINTLN(F("startSimpleSwitchDelayTimer: getGroupState(n), n, lastCmd[n], act[n]"));
	DEBUG2_PRINTLN(getGroupState(n));
	DEBUG2_PRINTLN(n);
	DEBUG2_PRINTLN(lastCmd[n]);
	DEBUG2_PRINTLN(act[n]);
	setLogic(lastCmd[n],n);
	//il timer di inversione contatto parte solo se il tempo è > 0
	if((act[n]==2 || act[n]==3) && haltdelay[n]>0){
		startTimer(haltdelay[n],n);
		setGroupState(2,n);	
		setCntValue(1,n);
	}else{
		resetCnt(n);
	}
	DEBUG2_PRINTLN(F("startSimpleSwitchDelayTimer switch mode: il contatto va in stato modifica abilitata"));
	onSWStateChange(n);
	return true;
}

void startPress(byte state, byte n){
	//cambio lo stato dell'uscita (SET)
	lastCmd[n]=state;
	startTimer(haltdelay[n],n);	
	setGroupState(1,n);	
	DEBUG2_PRINT(F("startPress: getGroupState(n): "));
	DEBUG2_PRINT(getGroupState(n));
	DEBUG2_PRINT(F(", n: "));
	DEBUG2_PRINT(n);
	DEBUG2_PRINT(F(", lastCmd[n]: "));
	DEBUG2_PRINTLN(lastCmd[n]);
}

void endPress(byte n){
	setLogic(!lastCmd[n],n);
	setGroupState(0,n);	
	resetCnt(n);
	DEBUG2_PRINTLN(F("endPress: getGroupState(n), n, lastCmd[n]"));
	DEBUG2_PRINTLN(getGroupState(n));
	DEBUG2_PRINTLN(n);
	DEBUG2_PRINTLN(!lastCmd[n]);
	DEBUG2_PRINTLN(haltdelay[n]);
	onSWStateChange(n);
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