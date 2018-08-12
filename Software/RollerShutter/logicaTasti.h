#ifndef __LOGICA_TASTI__
#define __LOGICA_TASTI__

#include <Arduino.h>
#include "common.h"

//DIMENSIONE SWITCH E TIMERS VETTORIALI
/*
#ifndef SWITCHN
#define SWITCHN  	0
#endif
//#define SWITCHN 	0 
#ifndef STATEN
#define STATEN  	0
#endif
//#define STATEN 		0 
#ifndef SWITCHNT
#define SWITCHNT  	0
#endif
//#define SWITCHNT 	0 
#ifndef SWITCHNTM
#define SWITCHNTM  	0
#endif
*/
//#define SWITCHNTM 	0 
//#ifndef SWITCHND
//#define SWITCHND  	0
//#endif
//#define SWITCHND 	0 

//variabili globali
//toggle per un solo pulsante
extern byte dprecval2[SWITCHND]; 
void initdfn(byte,  byte);

//void initTogglen(byte,  byte);

//switch per un solo pulsante attivo sui fronti
//bool switchf(byte, byte);
//switch per più pulsanti attivo sui fronti
//bool switchfn(byte, byte, byte);
//switch per un solo pulsante attivo su entrambi i fronti
//bool switchdf(byte);
//switch per più pulsanti attivo su entrambi i fronti
//bool inline switchdfn(byte, byte);


//byte switchdfn(byte val, byte &, byte n);
//void initDelay(unsigned long, byte n);
//toggle per un solo pulsante
//bool toggle(byte, byte);
//toggle per più pulsanti
//bool togglen(byte, byte, byte);
//logica toggle multipla su fronte di salita o di discesa dell'ingresso val, 
//restituisce la variabile changed che indica la variazione di almeno uno stato
//bool togglem(byte [] , byte, byte);



#endif //LOGICA_TASTI


