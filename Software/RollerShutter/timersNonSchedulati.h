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
/*                                                Timer singolo
//-------------------------------------------------------------------------------------------------------------------
//fisssa l'intervallo tra l'istante iniziale e quello finale del timer (lascia il time inattivo)
void setupTimer(unsigned long);
//fissa l'istante iniziale da cui far partire il timer
void startTimer();
//verifica se è arrivato il tempo di far scattare il timer
void aggiornaTimer();
byte getTimerState();
void setTimerState(byte);
void resetTimer();
//azione da compiere allo scadere del timer
//void onElapse();
//-------------------------------------------------------------------------------------------------------------*/
//                                                  Timer vettoriale
//-------------------------------------------------------------------------------------------------------------------
//fisssa l'intervallo tra l'istante iniziale e quello finale del timer (lascia il time inattivo)
void setupTimer(unsigned long, byte);
//fissa l'istante iniziale da cui far partire il timer
void startTimer(byte);
void startTimer(unsigned long, byte n);
//verifica se è arrivato il tempo di far scattare il timer
void aggiornaTimer(byte);
//azione da compiere allo scadere del timer
extern void onElapse(byte);
void initTimers();
byte getTimerState(byte n);
void setTimerState(byte,byte n);
void resetTimer(byte n);
//---------------------------------------------------------------------------------------------------------
//                                       Cronometro vettoriale
//---------------------------------------------------------------------------------------------------------
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


