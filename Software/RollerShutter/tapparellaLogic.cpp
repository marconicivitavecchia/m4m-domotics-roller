#include "tapparellaLogic.h"

int count1 = 0;
unsigned long target[2];
byte groupState[2];
byte *inp;
byte *inrp;
byte *outlogicp;
String  *paramsl;
float taplen, deltal;
float barrad;
float tapthick; 
long thaltp[2]={THALTMAX/2,THALTMAX/2};
long base[2]={0,0};
#if (AUTOCAL) 
float fact;
#endif
unsigned long engdelay[2]={0,0};
unsigned long btndelay[2]={0,0};
byte lastCmd[4];
short upmap[2]={1,-1};
byte calibr=0;
byte nrun=0;
byte moving[2]={false,false};
bool first[2]={true,true};
double nmax;

inline bool switchdfn(byte val, byte n){
	//n: numero di pulsanti
	val = (val>0);								//SEMPLICE!!!!
	bool changed = (val != dprecval2[n]);
	dprecval2[n] = val;            // valore di val campionato al loop precedente 
	return changed;
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
		DEBUG_PRINTLN(getGroupState(n));
		
		byte s = getGroupState(n);
		//siamo su uno dei fronti del pulsante 
		if(inp[BTN1IN+poffset+sw]>0)  
		{			
			changed = 0;
			//fronte di salita
			startTimer(RESETTIMER);
			outlogicp[SW1ONS+offset+sw]=true;
			if(!outlogicp[SW1ONS+offset+!sw]){  //evita attivazioni con pressione contemporanea di due tasti (interblocco)
				DEBUG_PRINTLN(F("dopo interblocco "));
				//effettuata prima pressione
				if(s==0)
				{ 	
					//se il motore è fermo
					setCronoDir(upmap[sw],n);  //in base a questo viene decisa la direzione della partenza differita
					lastCmd[BTN1IN + poffset+sw] = inp[BTN1IN+poffset+sw];
					DEBUG_PRINTLN(F("tapparellaLogic: stato 1: il motore va in attesa da stato 0 (fermo)"));
					startTimer(btndelay[n],TMRHALT+toffset);	
					setGroupState(1,n);	
					//stato 1: il motore va in attesa
					updateCnt(n);
					changed = 0;
				}else if(s==1)
				{//se il motore è in attesa
					//sono pressioni di configurazione
					resetTimer(TMRHALT+toffset);
					startTimer(btndelay[n],TMRHALT+toffset);	
					updateCnt(n);
					changed = 0;
				}else  if(s==2 || s==3)//se il motore è in moto a vuoto o in moto cronometrato
				{
					secondPress(n);
					changed = 1;
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
/*
byte tapparellaLogic(byte *in, byte *inr, byte *outlogic, unsigned long thalt, byte n){
	inp=in;
	inrp=inr;
	outlogicp=outlogic;
	thaltp[n]=thalt;
	tapparellaLogic(n);
}
*/
void initTapparellaLogic(byte *in, byte *inr, byte *outlogic, String  *paramsi, bool firstTime=false){
	paramsl=paramsi;
	inp=in;
	inrp=inr;
	outlogicp=outlogic;
	thaltp[0]=(paramsl[THALT1]).toInt();
	thaltp[1]=(paramsl[THALT2]).toInt();
	engdelay[0]=(paramsl[STDEL1]).toInt();
	engdelay[1]=(paramsl[STDEL2]).toInt();
	btndelay[0]=BTNDEL1;
	btndelay[1]=BTNDEL2;
	taplen=(paramsl[TLENGTH]).toFloat();
	barrad=(paramsl[BARRELRAD]).toFloat();
	tapthick=(paramsl[THICKNESS]).toFloat();
	resetCronoCount(0);
	resetCronoCount(1);
	setCronoLimits(-THALTMAX,THALTMAX,0);
	setCronoLimits(-THALTMAX,THALTMAX,1);
#if (!AUTOCAL) 
	//thaltp[0]=(paramsl[THALT1]).toInt();
	//thaltp[1]=(paramsl[THALT2]).toInt();
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

void setTapThalt(unsigned long thalt,byte n){
	thaltp[n]=thalt;
}

unsigned long getTapThalt(byte n){
	return thaltp[n];
}

long getTarget(byte n){
	return target[n];
}

byte getGroupState(byte n){
	return groupState[n];
}
	
void setGroupState(byte tstate, byte n){
	groupState[n]=tstate;
}

void startEndOfRunTimer(byte n){
	if(calibr==1)
		resetCronoCount(n);
	//il motore è in moto libero
	moving[n]=true;	
	//decide il tempo di corsa ovvero la posizione di arrivo	
	startTimer(target[n],TMRHALT+n*TIMERDIM);
	//comincia a cronometrare la corsa
	startCrono(n); 
	setGroupState(3,n);	//														stato 3: il motore va in moto cronometrato
	DEBUG_PRINT(F("stato 3: il motore " ));
	DEBUG_PRINT(n);
	DEBUG_PRINT(F(" al tempo  " ));
	DEBUG_PRINT(getCronoCount(n));
	DEBUG_PRINT(F(" è cronometrato verso "));
	DEBUG_PRINTLN(target[n]);	
}

bool startEngineDelayTimer(byte n){
		moving[n]=false;	
		setGroupState(2,n);												//stato 2: il motore va in moto a vuoto (o libero)
		byte btn = (short) (1-getCronoDir(n))/2+ n*BTNDIM;  //conversion from direction to index
		//DEBUG_PRINTLN(F("startEngineDelayTimer: getGroupState(n), btn, inp[btn]"));
		//DEBUG_PRINTLN(getGroupState(n));
		//DEBUG_PRINTLN(btn);
		//DEBUG_PRINTLN(inp[btn]);
		inp[btn] = lastCmd[btn];
		firstPress((getCronoDir(n)==DOWN), n);
		startTimer(engdelay[n],TMRHALT+n*TIMERDIM);
		
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
	resetCnt(n);
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
		updateCnt(n);
		setCronoDir((short)-getCronoDir(n),n);  //reverse direction
		byte btn = (short) (1-getCronoDir(n))/2+ n*BTNDIM;  //conversion from direction to index 
		lastCmd[btn] = 201;
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
	if(inp[BTN1IN+poffset+sw] == 1 || inp[BTN1IN+poffset+sw] == 2){
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
		target[n] = thaltp[n] * (!sw) + 2*fact*thaltp[n]*getCronoDir(n);
#else
		if(first[n] == true){
			target[n] = 1.5*thaltp[n];
		}else{
			target[n] = (thaltp[n]) * (!sw);
		}
#endif			
		target[n] = (long) (target[n]-getCronoCount(n))*getCronoDir(n);
		
		//DEBUG_PRINT(F("first: "));
		//DEBUG_PRINTLN(first[n]);
		//DEBUG_PRINT(F("Target: in moto verso "));
		//DEBUG_PRINTLN(target[n]);

	}else if(inp[BTN1IN+poffset+sw] == 201){
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
			target[n] = 2*THALTMAX;
		}
		//target[n] = (long) THALTMAX;
	}else if(inp[BTN1IN+poffset+sw] > 2){ //aperture percentuali
		//DEBUG_PRINTLN(F(" in moto verso l'alto perc"));
		if(inp[BTN1IN+poffset+sw] > 100){
			inp[BTN1IN+poffset+sw] = 100;
		}else if(inp[BTN1IN+poffset+sw] < 4){
			inp[BTN1IN+poffset+sw] = 0;
		}
		target[n] = (unsigned long) (thaltp[n]*calcTiming(inp[BTN1IN+poffset+sw]))/100;
		long delta = (long) (target[n] - getCronoCount(n));
		if(delta > 0){
			target[n] = delta;
			//LIST OF UP ACTIONS (TARGET ABOVE CURRENT POS)
			outlogicp[DIRS+offset]=sw;
			setCronoDir(upmap[sw],n);
		}else
		{ 	//TARGET UNDER CURRENT POS
			target[n] = -delta;
			outlogicp[DIRS+offset]=!sw;
			setCronoDir(upmap[!sw],n);
		}
	}
	//abilita il motore
	outlogicp[ENABLES+offset]=HIGH;
	nrun++;
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
