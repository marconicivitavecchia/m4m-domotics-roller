#ifndef __TIMERS_NO_SCHED__
#define __TIMERS_NO_SCHED__
#include <Arduino.h>
#include "common.h"

#ifndef TIMERN
#define TIMERN  	0
#endif

//#define TIMERN  0
//------------------------------------------------------------------------------------------------------------------
//  
//-------------------------------------------------------------------------------------------------------------
//                                                  Timer vettoriale
//-------------------------------------------------------------------------------------------------------------------
void startTimer(byte);
void startTimer(unsigned long, byte n);
//verifica se è arrivato il tempo di far scattare il timer
void aggiornaTimer(byte);
//azione da compiere allo scadere del timer
extern void onElapse(byte, unsigned long);
void initTimers();
byte getTimerState(byte n);
void setTimerState(byte,byte n);
void resetTimer(byte n);
//fisssa l'intervallo tra l'istante iniziale e quello finale del timer (lascia il time inattivo)
void setupTimer(unsigned long, byte);
//---------------------------------------------------------------------------------------------------------
//                                       Cronometro vettoriale
//---------------------------------------------------------------------------------------------------------
unsigned long getTimerLapse(byte);
void startCrono(byte);
unsigned long stopCrono(byte);
unsigned long restartCrono(byte);
unsigned long getCronoValue(byte);
void resetCronoCount(byte);
void setCronoCount(long, byte);
void setCronoDir(short, byte);
short getCronoDir(byte);
long addCronoCount(long, short, byte);
long getCronoCount(byte);
//long getPrecCount(byte);
void setCronoLimits(long, long, byte);
byte checkCronoLim(byte);
byte getCronoState(byte);	
void setCronoState(byte, byte);
long getUpLimit(byte n);
//azione da compiere allo scadere del cronometro
//extern void onCronoStop(byte);
//azione da compiere all'attivazione del cronometro
//extern void onCronoStart(byte);
#endif //TIMERS_NO_SCHED
