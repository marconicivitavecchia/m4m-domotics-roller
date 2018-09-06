#include "tapparellaLogic.h"

int count1 = 0;
unsigned long target[2];
byte groupState[2];
byte *inp;
byte *inrp;
byte *outlogicp;
unsigned long halfThaltMax = THALTMAX/2;
unsigned long thaltp[2]={THALTMAX/2,THALTMAX/2};
#if (!AUTOCAL) 
unsigned long thaltp2[2]={THALTMAX,THALTMAX};
#endif
unsigned long engdelay[2]={0,0};
unsigned long btndelay[2]={0,0};
byte lastCmd[4];
short upmap[2]={1,-1};
byte calibr=0;
byte nrun=0;
byte moving[2]={false,false};

bool inline switchdfn(byte val, byte n){
	//n: numero di pulsanti
	val = (val>0);								//SEMPLICE!!!!
	bool changed = (val != dprecval2[n]);
	dprecval2[n] = val;            // valore di val campionato al loop precedente 
	return changed;
}

byte tapparellaLogic(byte n){
	//ad ogni pressione del tasto 1 entro il tempo prefissato attiva il motore nella DIRSezione A
	//inp=digitalReaoutd(tasto1);
	//switch attivo su entrambi i fronti
	int offset=n*STATUSDIM;
	int toffset=n*TIMERDIM;
	int poffset=n*BTNDIM;
	byte changed = 0;	
			
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
						setGroupState(1,n);												//stato 1: il motore va in attesa
					}else{
						setGroupState(2,n);	//il motore è in moto a vuoto
						firstPress(0,n);
						startTimer(engdelay[0],TMRHALT+n*TIMERDIM);
						DEBUG_PRINTLN(F("stato 2: il motore va in moto a vuoto"));
						changed=1;
					}
				}else if(s==1)
				{//se il motore è in attesa
					setGroupState(4,n);												//stato 4: il motore va in modo configurazione
					//pressioni di configurazione del sistema, riavvio lo stato di attesa
					//se la seconda pressione di una sequenza di configurazione è fatta entro 0.4 sec
					//da li in poi vai in stato di configurazione che dura per 4 sec, 
					//entro il quale si possono completare le pressioni della sequenza.
					resetTimer(TMRHALT+toffset);
					resetCnt(n);
					setCntValue(2, n);
					startTimer(CNTIMER1+n);
					DEBUG_PRINTLN(F("stato 4: il motore va in modo configurazione da stato 1 (attesa)"));
					DEBUG_PRINT(F("Partito il timer di attivazione servizi a conteggio gruppo "));
					DEBUG_PRINTLN(n+1);
				}else if(s==4)
				{//se il motore è in configurazione
					updateCnt(n);
					DEBUG_PRINT(F("Count value: "));
					DEBUG_PRINTLN(getCntValue(n));
					DEBUG_PRINT(F("stato dopo update count:  "));
					DEBUG_PRINTLN(getGroupState(n));
				}else  if(s==2 || s==3)//se il motore è in moto a vuoto o in moto cronometrato
				{
					DEBUG_PRINT(F("tapparellaLogic: Stato 0: il motore va in stato 0 (fermo) da stato "));
					DEBUG_PRINTLN(getGroupState(n));
					secondPress(n);
					//setGroupState(0,n);												//stato 0: il motore va in stato fermo
					changed=1;
					/*if(getGroupState(n)==4){ 
						resetTimer(CNTIMER1+n);
						resetCnt(CNTSERV1);
						setGroupState(0,n);	//il motore è fermo
					}*/
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
				resetCnt(CNTSERV1);
				resetCnt(CNTSERV2);
				
				//effettuata prima pressione
			    if(s==0)
				{ //se il motore è fermo
					setCronoDir(DOWN,n);
					if(btndelay[n]>0 && inp[BTN1IN+poffset] < 201){
						lastCmd[BTN2IN + poffset] = inp[BTN2IN+poffset];
						DEBUG_PRINTLN(F("stato 1: il motore va in attesa "));
						setGroupState(1,n);												//stato 1: il motore va in attesa
						startTimer(btndelay[n],TMRHALT+toffset);	
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
					setGroupState(4,n);												//stato 4: il motore va in modo configurazione
					//pressioni di configurazione del sistema, riavvio lo stato di attesa
					resetTimer(TMRHALT+toffset);	
					resetCnt(n);
					setCntValue(2, n);
					startTimer(CNTIMER1+n);
					DEBUG_PRINTLN(F("stato 4: il motore va in configurazione "));
					DEBUG_PRINT(F("Partito il timer di attivazione servizi a conteggio gruppo "));
					DEBUG_PRINTLN(n+1);
					//è il motore è in moto
				}else if(s==4)
				{//se il motore è in configurazione
					updateCnt(n);
					DEBUG_PRINT(F("Count value: "));
					DEBUG_PRINTLN(getCntValue(n));
				}else if(s==2 || s==3)//se il motore è in moto a vuoto o in moto cronometrato
				{
					DEBUG_PRINTLN(F("stato 0: il motore va in stato fermo "));
					secondPress(n);
					changed=1;
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

byte tapparellaLogic(byte *in, byte *inr, byte *outlogic, unsigned long thalt, byte n){
	inp=in;
	inrp=inr;
	outlogicp=outlogic;
	thaltp[n]=thalt;
	tapparellaLogic(n);
}

void initTapparellaLogic(byte *in, byte *inr, byte *outlogic, unsigned long thalt1, unsigned long thalt2, unsigned long engdelay1, unsigned long engdelay2, unsigned long bdelay1, unsigned long bdelay2, bool first=false){
	inp=in;
	inrp=inr;
	outlogicp=outlogic;
	thaltp[0]=thalt1;
	thaltp[1]=thalt2;
	engdelay[0]=engdelay1;
	engdelay[1]=engdelay2;
	btndelay[0]=bdelay1;
	btndelay[1]=bdelay2;
#if (!AUTOCAL) 
	thaltp2[0]=thalt1;
	thaltp2[1]=thalt2;
	if(first){
		thaltp[0]=thaltp[1]=THALTMAX/2;
	}
#endif	
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
	moving[n]=true;
	if(calibr == 1 || calibr == 2){
		target[n] = THALTMAX;
		DEBUG_PRINTLN(F("Start Timer calibrazione "));
	}
		
	startTimer(target[n],TMRHALT+n*TIMERDIM);
	startCrono(n); 		//comincia a cronometrare la corsa
	setGroupState(3,n);	//														stato 3: il motore va in moto cronometrato
	DEBUG_PRINT(F("stato 3: il motore va in moto cronometrato verso "));
	DEBUG_PRINTLN(target[n]);	
}

bool startEngineDelayTimer(byte n){
		setGroupState(2,n);												//stato 2: il motore va in moto a vuoto
		byte btn = (short) (1-getCronoDir(n))/2+ n*BTNDIM;  //conversion from direction to index
		DEBUG_PRINTLN(F("startEngineDelayTimer: igetGroupState(n), btn, inp[btn]"));
		DEBUG_PRINTLN(getGroupState(n));
		DEBUG_PRINTLN(btn);
		DEBUG_PRINTLN(inp[btn]);
		//outLogic[SW1ONS+n*STATUSDIM]==true || outLogic[SW2ONS+n*STATUSDIM]==true	
		//if(outlogicp[(1-getCronoDir(n))/2+2+n*STATUSDIM]==true){	
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
	if(calibr == 0){
		//either UP or DOWN
		//da effettuare solo se il motore è inp moto
		//effettuata seconda pressione
		DEBUG_PRINT(F("\nSecondPress: seconda pressione: motore "));
		DEBUG_PRINT(n+1);
		DEBUG_PRINT(F(" fermo al tempo "));
		DEBUG_PRINTLN(getCronoCount(n));
		
		//LIST OF STOP ACTIONS
		resetTimer(TMRHALT+toffset);//blocca timer di fine corsa		
		//blocca il motore
		outlogicp[ENABLES+offset]=LOW;
		nrun--;
		outlogicp[DIRS+offset]=LOW;
		setGroupState(0,n);												//stato 0: il motore va in stato fermo
#if (AUTOCAL)  
		addCronoCount(stopCrono(n), (short) getCronoDir(n),n);
		if(getCronoDir(n)==UP){
			if(getCronoCount(n) > thaltp[n] && getCronoCount(n) < thaltp[n]*1.2){
				rslt = 0;
				thaltp[n] = getCronoCount(n);
				DEBUG_PRINTLN(F("tapparella impiega più tempo della stima per apertura totale: correzione..."));
			}else if(getCronoCount(n) > thaltp[n]*1.2){
				rslt = 2;
				DEBUG_PRINTLN(F("tapparella molto oltre il fine corsa alto, possibile forzatura"));
				setCronoCount(thaltp[n], n);
			}
		}else{
			if(getCronoCount(n) < 0 && getCronoCount(n) > -thaltp[n]*1.2){
				rslt = 0;
				DEBUG_PRINTLN(F("tapparella impiega più tempo della stima per la chiusura totale: correzione..."));
			}else if(getCronoCount(n) < -thaltp[n]*1.2){
				rslt = 3;
				DEBUG_PRINTLN(F("tapparella molto oltre il fine corsa basso, ricalibrare"));
			}
			stopCrono(n);
			resetCronoCount(n);
		}
#else	
		//somma (o sottrai) il valore cronometrato al vecchio valore del contatore di posizione (ultima posizione registrata)
		addCronoCount(stopCrono(n), (short) getCronoDir(n),n);
		DEBUG_PRINT(F("thaltp[n]: "));
		DEBUG_PRINTLN(thaltp[n]);
		if(thaltp[n] == halfThaltMax){
			DEBUG_PRINT(F("dentro if: "));
			if(getCronoCount(n) >= THALTMAX){
				thaltp[n] = thaltp2[n];
				setCronoCount(thaltp[n], n);
			}else if(getCronoCount(n) <= 0){
				thaltp[n] = thaltp2[n];
				setCronoCount(0, n);
			}
		}
#endif
	}else if(calibr == 1){
		resetTimer(TMRHALT+toffset);//blocca timer di fine corsa		
		//blocca il motore
		outlogicp[ENABLES+offset]=LOW;
		nrun--;
		outlogicp[DIRS+offset]=LOW;
		
		DEBUG_PRINTLN(F("2)reset del cronometro immediatamente prima della salita"));
		//startCrono(n);
		//Tapparella completamente abbassata: imposto a zero il contatore di stato
		resetCronoCount(n);
		byte btn = (short) (1+getCronoDir(n))/2+ n*BTNDIM;  //reverse conversion from direction to index
		inp[btn] = 201;
		firstPress((getCronoDir(n)==UP), n);				//reverse direction
		startTimer(engdelay[n],TMRHALT+n*TIMERDIM);
		calibr = 2;
		setGroupState(2,n);												//stato 2: il motore va in moto a vuoto
		DEBUG_PRINTLN(btn);
		DEBUG_PRINTLN(inp[btn]);
		DEBUG_PRINT(F("stato 2: il motore va in moto a vuoto per calibrazione verso "));
		DEBUG_PRINTLN(getCronoDir(n)==DOWN);
		DEBUG_PRINT(F("FASE 2 CALIBRAZIONE MANUALE"));
		DEBUG_PRINTLN(F("-----------------------------"));
		DEBUG_PRINTLN(F("LA TAPPARELLA STA "));
		DEBUG_PRINT(F("PREMERE UN PULSANTE QUALSIASI DEL GRUPPO ATTIVO"));
		DEBUG_PRINTLN(F("-----------------------------"));
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
		stopCrono(n);
		unsigned int app = getCronoValue(n);
		setCronoCount( app*(getCronoDir(n)==UP),n);
		if(app < CNTIME*1000)
			app = CNTIME*1000;
		setCronoLimits(0,app,n);
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
		DEBUG_PRINT(F(" in moto verso "));
		DEBUG_PRINTLN(sw);
		//LIST OF UP ACTIONSalt 
		//imposta la DIRSezione
		outlogicp[DIRS+offset]=sw;	
		//fai partire il timer di fine corsa
		setCronoDir(upmap[sw],n);
#if (AUTOCAL)  
		target[n] = THALTMAX;
#else
		target[n] = (thaltp[n]) * (!sw);
		target[n] = (long) (target[n]-getCronoCount(n))*getCronoDir(n);
#endif
	}else if(inp[BTN1IN+poffset+sw] == 201){
		DEBUG_PRINT(F("Calibrazione: in moto verso "));
		DEBUG_PRINTLN(sw);
		//thaltp[n] = THALTMAX;
		//imposta la DIRSezione
		outlogicp[DIRS+offset]=sw;	
		setCronoDir(upmap[sw],n);
		setCronoLimits(0,THALTMAX,n);
		calibr = 1;
		target[n] = THALTMAX*(!sw);
		target[n] = (long) (target[n]-getCronoCount(n))*getCronoDir(n);
	}else if(inp[BTN1IN+poffset+sw] > 2){ //aperture percentuali
		//DEBUG_PRINTLN(F(" in moto verso l'alto perc"));
		target[n] = (unsigned long) (thaltp[n]/100)*inp[BTN1IN+poffset+sw];
		long delta = (long) (target[n] - getCronoCount(n));
		//target[n] = (long) (target[n]-getCronoCount(n))*getCronoDir(n);
		DEBUG_PRINT(F("\nDelta:  "));
		DEBUG_PRINTLN(target[n]);
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

/*
void firstPressDown(byte n){
	int offset=n*STATUSDIM;
	int toffset=n*TIMERDIM;
	int poffset=n*BTNDIM;
	
	//da effettuare solo se il motore è fermo
	//DEBUG_PRINT(F("\nPrima pressione DOWN: motore "));
	//DEBUG_PRINT(n+1);
	//DEBUG_PRINT(F(" in moto verso il basso al tempo "));
	//DEBUG_PRINTLN(getCronoCount(n));
	
	//resetTimer(TMRHALT+toffset);
	if(inp[BTN2IN+poffset] == 1 || inp[BTN2IN+poffset] == 2){
		DEBUG_PRINT(F(" in moto verso il basso noper "));
		//LIST OF DOWN ACTIONS
		//imposta la DIRSezione
		outlogicp[DIRS+offset]=HIGH;	
		//abilita il motore
		outlogicp[ENABLES+offset]=HIGH;	
		target[n] = 0;
		setCronoDir(DOWN,n);
	}else if(inp[BTN2IN+poffset] > 2){ //aperture percentuali
		target[n] = (unsigned long) (thaltp[n]/100)*inp[BTN2IN+poffset];
		DEBUG_PRINT(F("\BTN2IN "));
		DEBUG_PRINTLN(inp[BTN2IN+poffset]);
		DEBUG_PRINT(F("\nTARGET "));
		DEBUG_PRINTLN(target[n]);
		DEBUG_PRINT(F("\getCronoCount(n) "));
		DEBUG_PRINTLN(getCronoCount(n));
		if(getCronoCount(n)> target[n]){
			//LIST OF STOP ACTIONS  (TARGET ABOVE CURRENT POS)
			//imposta la DIRSezione
			outlogicp[DIRS+offset]=HIGH;	
			//abilita il motore
			outlogicp[ENABLES+offset]=HIGH;
			setCronoDir(DOWN,n);
		}else{ //TARGET UNDER CURRENT POS
			setGroupState(1,n);	//il motore è in attesa
			//LIST OF DOWN ACTIONS
			DEBUG_PRINT(F("\nrev "));		
			DEBUG_PRINTLN(inp[BTN1IN+poffset]);
			DEBUG_PRINT(F("\nStop DOWN"));
			//il motore è inp inversione di marcia (stato 3)
			inp[BTN1IN+poffset] = inp[BTN2IN+poffset];
			inp[BTN2IN+poffset] = 0;
			firstPressUp(n);
			//tapparellaLogic(inrp,inrp,outlogicp,thaltp[n],n);
		}
	}						
}

void firstPressUp(byte n){
	int offset=n*STATUSDIM;
	int toffset=n*TIMERDIM;
	int poffset=n*BTNDIM;
	
	//da effettuare solo se il motore è fermo
	//DEBUG_PRINT(F("\nPrima pressione UP: motore "));
	//DEBUG_PRINT(n+1);
	//DEBUG_PRINT(F("\ninp[BTN1IN+poffset] "));
	//DEBUG_PRINTLN(inp[BTN1IN+poffset]);
		
	//resetTimer(TMRHALT+toffset);			
	if(inp[BTN1IN+poffset] == 1 || inp[BTN1IN+poffset] == 2){
		DEBUG_PRINTLN(F(" in moto verso l'alto noperc"));
		//LIST OF UP ACTIONSalt 
		//imposta la DIRSezione
		outlogicp[DIRS+offset]=LOW;	
		//abilita il motore
		outlogicp[ENABLES+offset]=HIGH;
		//fai partire il timer di fine corsa
		target[n] = thaltp[n];
		setCronoDir(UP,n);
	}else if(inp[BTN1IN+poffset] > 2){ //aperture percentuali
		DEBUG_PRINTLN(F(" in moto verso l'alto perc"));
		target[n] = (unsigned long) (thaltp[n]/100)*inp[BTN1IN+poffset];
		DEBUG_PRINT(F("\nTARGET "));
		DEBUG_PRINTLN(target[n]);
		if(target[n] > getCronoCount(n)){
			//LIST OF UP ACTIONS (TARGET ABOVE CURRENT POS)
			outlogicp[DIRS+offset]=LOW;	
			//abilita il motore
			outlogicp[ENABLES+offset]=HIGH;
			setCronoDir(UP,n);
		}else{ //TARGET UNDER CURRENT POS
			setGroupState(1,n);	//il motore è in attesa
			DEBUG_PRINT(F("\nrev "));
			DEBUG_PRINTLN(inp[BTN1IN+poffset]);
			//LIST OF UP ACTIONS
			DEBUG_PRINT(F("\nStop UP"));
			//il motore è inp inversione di marcia (stato 3)
			DEBUG_PRINT(F("\nStart DOWN"));
			//si trasferisce l'ingresso percentuale nell'altra direzione
			inp[BTN2IN+poffset] = inp[BTN1IN+poffset];
			inp[BTN1IN+poffset] = 0;
			firstPressDown(n);
			//tapparellaLogic(inrp,inrp,outlogicp,thaltp[n],n);
			//il motore è in inversione di marcia (stato 3)
		}
	}
}
*/