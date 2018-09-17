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
unsigned long thaltp[2]={THALTMAX/2,THALTMAX/2};
#if (!AUTOCAL) 
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
#endif	
	deltal=DELTAL;
	nmax = ( (double) sqrt((double) (taplen*tapthick)/TWO_PI + barrad*barrad) - barrad) / tapthick /2;		//100% of excursion
	//nmax = ((double) sqrt((double) ((taplen-deltal)*tapthick)/TWO_PI + barrad*barrad) - barrad) / tapthick /2 + deltal/TWO_PI/barrad;		//100% of excursion
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

short secondPress(byte n){
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
		setGroupState(0,n);												//stato 0: il motore va in stato fermo
		addCronoCount(stopCrono(n), (short) getCronoDir(n),n); 
		long app = getCronoCount(n);
#if (AUTOCAL)  
		if(getCronoDir(n)==UP){
			//versione con correzzione continua in base alla lettura del sensore
			if(getCronoCount(n) > (long) thaltp[n]*0.9 && getCronoCount(n) < (long) thaltp[n]*1.1){
				setCronoCount(thaltp[n], n);
				rslt = 0;
				//thaltp[n] = getCronoCount(n);
				DEBUG_PRINTLN(F("tapparella impiega un tempo diverso dalla stima per apertura totale: correzione..."));
			}else if(getCronoCount(n) > (long) thaltp[n]*1.1){
				setCronoCount(thaltp[n], n);
				rslt = 2;
				DEBUG_PRINTLN(F("tapparella molto oltre il fine corsa alto, possibile forzatura"));
			}
		}else{
			if(getCronoCount(n) < (long) thaltp[n]*0.1 && getCronoCount(n) > (long) -thaltp[n]*0.1){
				resetCronoCount(n);
				rslt = 0;
				DEBUG_PRINTLN(F("tapparella impiega un tempo diverso dalla stima per la chiusura totale: correzione..."));
			}else if(getCronoCount(n) < (long) -thaltp[n]*0.1){
				resetCronoCount(n);
				rslt = 3;
				DEBUG_PRINTLN(F("tapparella molto oltre il fine corsa basso, ricalibrare"));
			}
		}//*/
		
		/*
		if(getCronoDir(n)==UP){		
			//versione con correzzione in base alla lettura del sensore solo se 
			//la differenza tra il valore misurato e quello stimato supera una certa soglia
			if(getCronoCount(n) > (long) thaltp[n]*0.95 && getCronoCount(n) < (long) thaltp[n]*1.05){
				rslt = 0;
				//do nothing
			}else if(getCronoCount(n) > (long) thaltp[n]*1.05 && getCronoCount(n) < (long) thaltp[n]*1.2){
				setCronoCount(thaltp[n], n);
				rslt = 0;
				DEBUG_PRINTLN(F("tapparella impiega un tempo diverso dalla stima per apertura totale: correzione..."));
			}if(getCronoCount(n) > (long) thaltp[n]*1.2){
				setCronoCount(thaltp[n], n);
				rslt = 2;
				DEBUG_PRINTLN(F("tapparella molto oltre il fine corsa alto, possibile forzatura"));
				}
		}else{
			if(getCronoCount(n) < (long) thaltp[n]*0.05 && getCronoCount(n) > (long) -thaltp[n]*0.05){
				rslt = 0;
				//do nothing
			}else if(getCronoCount(n) < (long) -thaltp[n]*0.05 && getCronoCount(n) > (long)  -thaltp[n]*0.2){
				resetCronoCount(n);
				rslt = 0;
				DEBUG_PRINTLN(F("tapparella impiega un tempo diverso dalla stima per apertura totale: correzione..."));
			}if(getCronoCount(n) < (long) -thaltp[n]*0.2){
				rslt = 3;
				DEBUG_PRINTLN(F("tapparella molto oltre il fine corsa basso, ricalibrare"));
				stopCrono(n);
				resetCronoCount(n);
			}
		}//*/
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
		unsigned int app =addCronoCount(stopCrono(n), (short) getCronoDir(n),n);
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
	//onTapStop(n);
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
		target[n] = 1.5*thaltp[n];
#else
		if(first[n] == true){
			target[n] = 1.5*thaltp[n];
		}else{
			target[n] = (thaltp[n]) * (!sw);
			target[n] = (long) (target[n]-getCronoCount(n))*getCronoDir(n);
		}
		//DEBUG_PRINT(F("first: "));
		//DEBUG_PRINTLN(first[n]);
		//DEBUG_PRINT(F("Target: in moto verso "));
		//DEBUG_PRINTLN(target[n]);
#endif
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
	nv = ((double) sqrt((double) (v*taplen*tapthick)/TWO_PI/100 + barrad*barrad) - barrad) / tapthick /2;	
	/*if(v*taplen < deltal){
		nv = v*deltal/TWO_PI/barrad;
	}else{
		nv = ((double) sqrt((double) ((v*taplen -deltal)*tapthick)/TWO_PI/100 + barrad*barrad) - barrad) / tapthick /2 + deltal/TWO_PI/barrad;
	}*/
	
	nv =  ((double) nv / nmax * 100);
	return  nv;
}

/*
byte tapparellaLogic(byte n){
	//ad ogni pressione del tasto 1 entro il tempo prefissato attiva il motore nella DIRSezione A
	//inp=digitalReaoutd(tasto1);
	//switch attivo su entrambi i fronti
	byte changed2 = 0;	
	
	//button UP
	changed2 = switchLogic(0,n);
	//button DOWN
	changed2 = switchLogic(1,n);
	
	return changed2;
}
*/


/*
//pulsante UP
	if(switchdfn(inp[BTN1IN+poffset],BTN1IN+poffset))
	{
		DEBUG_PRINT(F("fronte di SW1ONS: "));
		DEBUG_PRINTLN(inp[BTN1IN+poffset]);
		DEBUG_PRINT(F("stato switch 1:  "));
		DEBUG_PRINTLN(getGroupState(n));
		
		byte s = getGroupState(n);
		//siamo su uno dei fronti del pulsante UP
		if(inp[BTN1IN+poffset]>0)  
		{			
			changed = 0;
			//fronte di salita
			startTimer(RESETTIMER);
			//DEBUG_PRINTLN(F("Partito il timer di reset"));
			//DEBUG_PRINT(F("fronte di salita SW1ONS: "));
			//DEBUG_PRINTLN(outlogicp[SW1ONS+offset]);			
			//eseguo il lock del pulsante di DOWN	
			//inizio sezione critica
			//noInterrupts ();
			outlogicp[SW1ONS+offset]=true;
			if(!outlogicp[SW2ONS+offset]){  //evita attivazioni con pressione contemporanea di due tasti (interblocco)
				//interrupts ();
				//fine sezione critica
				//lastCmd[BTN1IN+poffset]=inp[BTN1IN+poffset];
				DEBUG_PRINTLN(F("dopo interblocco "));
				//effettuata prima pressione
				
				if(s==0)
				{ 	
					setCronoDir(UP,n);
					if(btndelay[n]>0 && inp[BTN1IN+poffset] < 201){
						//se il motore è fermo
						lastCmd[BTN1IN + poffset] = inp[BTN1IN+poffset];
						DEBUG_PRINTLN(F("tapparellaLogic: stato 1: il motore va in attesa da stato 0 (fermo)"));
						startTimer(btndelay[n],TMRHALT+toffset);	
						setGroupState(1,n);	
						//stato 1: il motore va in attesa
						updateCnt(n);
					}else{
						setGroupState(2,n);	//il motore è in moto a vuoto
						firstPress(0,n);
						startTimer(engdelay[0],TMRHALT+n*TIMERDIM);
						DEBUG_PRINTLN(F("stato 2: il motore va in moto a vuoto"));
						changed=1;
					}
				}else if(s==1)
				{//se il motore è in attesa
					//sono pressioni di configurazione
					resetTimer(TMRHALT+toffset);
					startTimer(btndelay[n],TMRHALT+toffset);	
					updateCnt(n);
				}else  if(s==2 || s==3)//se il motore è in moto a vuoto o in moto cronometrato
				{
					resetCnt(n);
					secondPress(n);
					changed=1;
					DEBUG_PRINTLN(F("stato 0: il motore va in stato fermo "));
				}
				
				//onUpPressed(n);
			}
		}
		else
		{
			DEBUG_PRINTLN(F("fronte di discesa "));
			//fronte di discesa
			//DEBUG_PRINTLN(F("Rilascio pulsanti"));
			//rilascio interblocco
			//inizio sezione critica
			//noInterrupts ();
			outlogicp[SW1ONS+offset]=false;
			//interrupts ();
			//fine sezione critica
			changed=255;
			//ferma il timer di reset
			//Tasto rilasciato: blocca il timer di reset
			resetTimer(RESETTIMER);
			//DEBUG_PRINTLN(F("Bloccato il timer di reset"));
		}
	
	}
	
	//pulsante DOWN
	if(switchdfn(inp[BTN2IN+poffset],BTN2IN+poffset)){
		
		DEBUG_PRINT(F("fronte di SW2ONS: "));
		DEBUG_PRINTLN(inp[BTN1IN+poffset]);
		
		byte s = getGroupState(n);
		//siamo su uno dei fronti del pulsante Down
		if(inp[BTN2IN+poffset]>0) 
		{
			changed = 0;
			DEBUG_PRINT(F("fronte di salita con stato: "));
			DEBUG_PRINTLN(s);
			//fronte di salita
			//eseguo il lock del pulsante di UP nointerruppt()
			//inizio sezione critica
			//noInterrupts ();
			outlogicp[SW2ONS+offset]=true;
			if(!outlogicp[SW1ONS+offset]){  //evita attivazioni con pressione contemporanea di due tasti (interblocco)
				//interrupts ();
				//fine sezione critica
				//lastCmd[BTN2IN+poffset]=inp[BTN2IN+poffset];
				DEBUG_PRINTLN(F("dopo interblocco "));
				
				//effettuata prima pressione
			    if(s==0)
				{ //se il motore è fermo
					setCronoDir(DOWN,n);
					if(btndelay[n]>0 && inp[BTN1IN+poffset] < 201){
						lastCmd[BTN2IN + poffset] = inp[BTN2IN+poffset];
						DEBUG_PRINTLN(F("stato 1: il motore va in attesa "));
						setGroupState(1,n);												//stato 1: il motore va in attesa
						startTimer(btndelay[n],TMRHALT+toffset);	
						updateCnt(n);
					}else{
						setGroupState(2,n);												//stato 2: il motore va in moto a vuoto
						firstPress(1,n);
						startTimer(engdelay[1],TMRHALT+n*TIMERDIM);
						DEBUG_PRINTLN(F("stato 2: il motore va in moto a vuoto"));
						changed=1;
					}
					//firstPressDown(n);
				}else if(s==1)
				{//se il motore è in attesa
					//pressioni di configurazione del sistema, riavvio lo stato di attesa
					resetTimer(TMRHALT+toffset);
					startTimer(btndelay[n],TMRHALT+toffset);						
					updateCnt(n);
				}else if(s==2 || s==3)//se il motore è in moto a vuoto o in moto cronometrato
				{
					resetCnt(n);
					secondPress(n);
					changed=1;
					DEBUG_PRINTLN(F("stato 0: il motore va in stato fermo "));
				}
				
				//onDownPressed(n);
			}
		}
		else
		{
			DEBUG_PRINTLN(F("fronte di discesa "));
			//fronte di discesa
			//DEBUG_PRINTLN(F("Rilascio pulsanti"));
			//rilascio interblocco
			//inizio sezione critica
			//noInterrupts ();
			outlogicp[SW2ONS+offset]=false;
			//interrupts ();
			//fine sezione critica
			changed= 255;
			
		}
		
	}
	*/
