/*
Logica di comando per tasti
Gli esempi sono tutti senza antirimbalzo da implementare in una delle maniere che si preferisce.

Tipi di tasto:
1) switch       logica switch attiva sui livelli (non implementato in questa libreria)
2) switchf      logica switch attivo su uno dei fronti
3) switchdf     logica switch attivo su entrambi i fronti
4) toggle       logica con memoria di stato attiva su uno dei fronti
5) versioni n e m   varianti vettoriali di tutte le logiche viste sopra

*/
//-------------------------------------------------------------------
//fine variabili globali
#include "logicaTasti.h"
#include "common.h"

byte dprecval2[SWITCHND]; 
float asyncBuf[GATEND]; 
//#define SWITCHN 	5 
//#define stateN 	0 

//byte state, precval, dprecval, tprecval;
//byte state2[STATEN]; 
//byte tmstate2[STATEN]; 
//byte precval2[SWITCHN]; 
//byte dprecval2[SWITCHND]; 
//unsigned long p[SWITCHND];
//unsigned long np[SWITCHND];
//unsigned long i = 0;
//byte dir = 0;
//byte tprecval2[SWITCHNT]; 
//byte tmprecval2[SWITCHNTM]; 
/*
void initfn(byte val,  byte n){
		precval2[n]=val;
}
*/
void initdfn(byte val, byte n){
	dprecval2[n]=val;
}
/*
void initDelay(unsigned long step, byte n){
	np[n]=step;
}
*/
/*
void initToggle(byte val,  byte n){
		state2[n]=val;
}
*/
/*
//switch per un solo pulsante attivo sui fronti
bool switchf(byte val, byte edge){
	//edge == HIGH --> fronte di salita
	//edge == LOW  --> fronte di discesa
	//n: numero di pulsanti
	bool changed=false;
	if ((val == edge) && (precval == !edge)){ //campiona solo le transizioni in un senso
		changed=true;
	}   
	precval = val;            // valore di val campionato al loop precedente 
	return changed;
}
*/
/*
//switch per più pulsanti attivo sui fronti
bool switchfn(byte val, byte edge, byte n){
	//edge == HIGH --> fronte di salita
	//edge == LOW  --> fronte di discesa
	//n: numero di pulsanti
	bool changed=false;
	if ((val == edge) && (precval2[n] == !edge)){ //campiona solo le transizioni in un senso
		changed=true;
	}   
	precval2[n] = val;            // valore di val campionato al loop precedente 
	return changed;
}
*/
/*
//switch per un solo pulsante attivo su entrambi i fronti
bool switchdf(byte val){
	bool changed=false;
	if (val != dprecval){ //campiona tutte le transizioni
		changed=true;
	}   
	precval = val;            // valore di val campionato al loop precedente 
	return changed;
}
*/
/*
//switch per più pulsanti attivo su entrambi i fronti
bool static inline switchdfn(byte val, byte n){
	//n: numero di pulsanti
	val = (val>0);								//SEMPLICE!!!!
	bool changed = (val != dprecval2[n]);
	dprecval2[n] = val;            // valore di val campionato al loop precedente 
	return changed;
}
*/
/*
byte switchdfn(byte val, byte &draise, byte n){
//	draise:		delayed change trigger 	(1 for rise, -1 for falling)
//	changed:	immediate raise trigger (1 for rise, -1 for falling)
//	n: 			number of switches

	p[n] = (p[n] + i) % np[n];
	
	draise = 0;
	if (!p[n]){
		draise = dir;
		i=0;                           //COMPLESSO!!!!
	}
		
	val = (val>0);
	if(val!=dprecval2[n]){
		i=1;
		dir=(val > dprecval2[n]) - (val < dprecval2[n]);
	}	
	dprecval2[n] = val; 
	
	return dir;
}
*/
/*
byte switchdfn2(byte val, byte &draise, byte n){
//	draise:		delayed change trigger 	(1 for rise, -1 for falling)
//	changed:	immediate raise trigger (1 for rise, -1 for falling)
//	n: 			number of switches

	byte changed;
	
	val = (val>0);
	changed = (val > dprecval2[n]) - (val < dprecval2[n]);
	dprecval2[n] = val;      

	p[n] =  (p[n] + i) % np[n];
	
	if(draise){
		i=0;
		dir = 0;
	}
	//i *= !draise								//COMPLESSO!!!!
	if(changed!=0){
		i=1;
		dir=changed; 
	}	
	
	//if (!p[n]){
	//	draise = true;
	//}
	draise = (!(p[n])*dir;
	
	return changed;
}
*/

/*
//toggle per un solo pulsante
bool toggle(byte val, byte edge){
	//edge == HIGH --> fronte di salita
	//edge == LOW  --> fronte di discesa
	bool changed=false;
	if ((val == edge) && (tprecval == !edge)){ //campiona solo le transizioni da basso a alto 
		 state = !state;  
		 changed=true;
	}   
	tprecval = val;            // valore di val campionato al loop precedente 
	return changed;
}
*/
/*
//toggle per più pulsanti
bool togglen(byte val, byte edge, byte n){
	//edge == HIGH --> fronte di salita
	//edge == LOW  --> fronte di discesa
	//n: numero di pulsanti
	bool changed=false;
	if ((val == edge) && (tprecval2[n] == !edge)){ //campiona solo le transizioni da basso a alto 
		state2[n] = !state2[n];
		changed=true;
	}   
	tprecval2[n] = val;            // valore di val campionato al loop precedente 
	return changed;
}
*/
//logica toggle multipla su fronte di salita o di discesa dell'ingresso val, 
//restituisce la variabile changed che indica la variazione di almeno uno stato
/*bool togglem(byte val[] , byte edge, byte n){
	//edge == HIGH --> fronte di salita
	//edge == LOW  --> fronte di discesa
	//n: numero di pulsanti
	//variabile flag. Indica se almeno un pulsante ha cambiato state
	bool changed=false;
	//calcola la funzione toggle per tutti i valori dell'array degli ingressi val
	for(int i=0;i<n;i++){
		if((val[i] == (edge)) && (tmprecval2[i] == !edge)){
			tmstate2[i]=!tmstate2[i];
			changed=true;
		}
		tmprecval2[i]=val[i];
    }
	return changed;
}
*/
