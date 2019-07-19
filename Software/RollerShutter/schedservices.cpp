/*
Counter schedulati
Due tipi:
1) singoli
N) vettoriali

*/
#include "schedservices.h"
#include <Arduino.h>
//inizio variabili counter vettoriali
//da impostare con dimensione pari al numero di timer previsti (ad es. N)
unsigned long countn[NCNT];
unsigned long targetn[NCNT];
bool staten[NCNT];
//------------------------------------------------------------------------------------------------------------------
//                                                  Contatori vettoriali
//-------------------------------------------------------------------------------------------------------------------
bool testUpCntEvnt(unsigned long cnt, bool repeat, byte n){
	if(countn[n] >= targetn[n] && staten[n]){
		staten[n] = repeat;
		countn[n] = cnt;
		return true;	
	}
	return false;	
}

bool testDownCntEvnt(unsigned long cnt, bool repeat, byte n){
	if(countn[n] <= targetn[n] && staten[n]){
		staten[n] = repeat;
		countn[n] = cnt;
		return true;	
	}
	return false;	
}

void incCnt(byte n){
	//n: numero del counter
	countn[n]++;
}

void decCnt(byte n){
	//n: numero del counter
	countn[n]--;
}

unsigned long getCntValue(byte n){
	return countn[n];
}

void stopCnt(byte n){
	staten[n] = false;
}

void startCnt(unsigned long cnt, unsigned long target, byte n){
	 countn[n] = cnt;
	 targetn[n]= target;
	 staten[n] = true;
}

void setCntValue(unsigned long val, byte n){
	countn[n] = val;
}

void resetCnt(byte n){
	countn[n] = 0;
}


