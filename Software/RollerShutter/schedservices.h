#ifndef __SCHED_SERVICES__
#define __SCHED_SERVICES__
#include <Arduino.h>
#include "common.h"

#ifndef NCNT
#define NCNT  	0
#endif
//#define TIMERN  0
//-------------------------------------------------------------------------------------------------------------
//                                                  Counter vettoriale
//--------------------------------------------------------------------------------------------------------
bool testUpCntEvnt(unsigned long, bool, byte);
bool testDownCntEvnt(unsigned long, bool, byte);
void incCnt(byte);
void decCnt(byte);
unsigned long getCntValue(byte);
void setCntValue(unsigned long, byte);
void startCnt(unsigned long, unsigned long, byte);
void stopCnt(byte);
void resetCnt(byte);
#endif //COUNTERS_SCHED


