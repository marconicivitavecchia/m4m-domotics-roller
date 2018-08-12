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
unsigned long staten[NCNT];
//------------------------------------------------------------------------------------------------------------------
//                                                  Timers vettoriali
//-------------------------------------------------------------------------------------------------------------------
bool testCntEvnt(unsigned long limit, byte n){
	if(countn[n] == limit){
		return true;	
	}
	return false;	
}

unsigned long getAndRstCnt(byte n){
	unsigned long val = countn[n];
	countn[n] = staten[n] = 0;
	return val;
}

void updateCnt(byte n){
	//n: numero del counter
	countn[n]++;
}

unsigned long getCntValue(byte n){
	return countn[n];
}

void resetCnt(byte n){
	countn[n] = staten[n] = 0;
}

void stopCnt(byte n){
	staten[n] = countn[n];
}

void startCnt(byte n){
	 countn[n] = staten[n];
}

void setCntValue(unsigned long val, byte n){
	countn[n] = val;
}


