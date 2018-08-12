#ifndef __SCHED_SERVICES__
#define __SCHED_SERVICES__
#include <Arduino.h>
#include "common.h"

#ifndef NCNT
#define NCNT  	0
#endif
//#define TIMERN  0
//-------------------------------------------------------------------------------------------------------------*/
//                                                  Counter vettoriale
//-------------------------------------------------------------------------------------------------------------------
bool testCntEvnt(unsigned long, byte);
unsigned long getAndRstCnt(byte);
void updateCnt(byte);
unsigned long getCntValue(byte);
void resetCnt(byte);
void stopCnt(byte);
void startCnt(byte n);
void setCntValue(unsigned long, byte);
#endif //COUNTERS_SCHED


