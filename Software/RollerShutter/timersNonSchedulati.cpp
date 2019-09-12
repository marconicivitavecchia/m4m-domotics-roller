
/*
Timers non schedulati
Due tipi:
1) singoli
N) vettoriali

*/
#include "timersNonSchedulati.h"
#include <Arduino.h>
/*
//inizio variabili timer
unsigned long startTime1;
unsigned long timelapse1;
byte timerState1;
//fine variabili timer
*/
//inizio variabili timer vettoriali
//da impostare con dimensione pari al numero di timer previsti (ad es. N)
unsigned long startTimeN[TIMERN];
unsigned long timelapseN[TIMERN]; 
byte timerStateN[TIMERN];
unsigned long crnprecn[CHRN]; 
unsigned long crnelapsn[CHRN];
bool crnStaten[CHRN]; 
short crnDir[CHRN];
long crnCount[CHRN];
long prcCount[CHRN];
long highLim[CHRN];
long lowLim[CHRN];

//------------------------------------------------------------------------------------------------------------------
//                                                  Timers vettoriali
//-------------------------------------------------------------------------------------------------------------------
//fisssa l'intervallo tra l'istante iniziale e quello finale del timer (lascia il time inattivo)
void setupTimer(unsigned long duration, byte n){
	timerStateN[n]=0;
	timelapseN[n]=duration;
}

unsigned long getTimerLapse(byte n){
	return timelapseN[n];
}

void startTimer(byte n){
	//n: numero del timer
	timerStateN[n]=1;
	startTimeN[n]=millis();
}

//fissa l'istante iniziale da cui far partire il timer
void startTimer(unsigned long duration, byte n){
	timerStateN[n]=1;
	timelapseN[n]=duration;
	startTimeN[n]=millis();
}

//verifica se è arrivato il tempo di far scattare il timer
void aggiornaTimer(byte n){
	//n: numero del timer
	//Serial.println("TS "+ String(TIMERN));
	if((millis() - startTimeN[n] >= timelapseN[n]) && timerStateN[n]>0){
		timerStateN[n]=0;
		onElapse(n, timelapseN[n]);
	}
}	

void initTimers(){
	for(int i=0; i<TIMERN; i++){
		timerStateN[i]=0;
		startTimeN[i]=0;
		timelapseN[i]=0;
	}
}

byte getTimerState(byte n){
	return timerStateN[n];
}
	
void setTimerState(byte tstate, byte n){
	timerStateN[n]=tstate;
}

void resetTimer(byte n){
	timerStateN[n]=0;
}
//-------------------------------CRONOMETRO VETTORIALE-----------------------------------------
void startCrono(byte n){
	crnelapsn[n] = 0;
	crnprecn[n] = millis();
	crnStaten[n] = 1;
	//onCronoStart(n);
}

unsigned long stopCrono(byte n){
	crnelapsn[n] = millis() - crnprecn[n] + crnelapsn[n];
	crnStaten[n] = 0;
	//onCronoStop(n);
	return crnelapsn[n];
}

unsigned long restartCrono(byte n){
	 crnprecn[n] = millis();
	 crnStaten[n] = 1;
	 return crnelapsn[n];
}

unsigned long getCronoValue(byte n){
	return crnelapsn[n];
}

void resetCrono(byte n){
	crnelapsn[n] = 0;
}

void resetCronoCount(byte n){
	crnCount[n] = 0L;
}

void setCronoCount(long cnt, byte n){
	//prcCount[n] = crnCount[n];
	crnCount[n] = cnt;
}

short getCronoDir(byte n){
	return crnDir[n];
}

void setCronoDir(short dir, byte n){
	crnDir[n] = dir;
}

long addCronoCount(long cnt, short dir, byte n){
	crnDir[n] = dir;
	
	//prcCount[n] = crnCount[n]; 
	crnCount[n] = (long) crnCount[n] + cnt*dir;
	//DEBUG_PRINT(F("addCronoCount: "));
	//DEBUG_PRINTLN(crnCount[n]);
	//DEBUG_PRINT(F("cnt: "));
	//DEBUG_PRINTLN(cnt);
	//DEBUG_PRINT(F("dir: "));
	//DEBUG_PRINTLN(dir);
	
	if(crnCount[n] < lowLim[n])
		crnCount[n] = lowLim[n];
	else if(crnCount[n] > highLim[n])
		crnCount[n] = highLim[n];
	
	return crnCount[n];
}

long getCronoCount(byte n){
	return crnCount[n];
}
/*
long getPrecCount(byte n){
	return prcCount[n];
}*/

void setCronoLimits(long low, long high, byte n){
	lowLim[n] = low;
	highLim[n] = high;
}

long getUpLimit(byte n){
	return highLim[n];
}

byte checkCronoLim(byte n){
	byte r = 0;
	if(crnCount[n] <= lowLim[n])
		r = -1;
	else if(crnCount[n] >= highLim[n])
		r = 1;
	return r;
}

byte getCronoState(int n){
	return crnStaten[n];
}
	
void setCronoState(byte cstate, int n){
	crnStaten[n]=cstate;
}
