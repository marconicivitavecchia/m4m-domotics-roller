#include "statistics.h"
double avg[2] = {0.0, 0.0};
double stdDev[2] = {0.0, 0.0};
unsigned long count[2] = {1, 1};
//short count2[2] = {0, 0};
//short countd[2] = {0, 0};
//unsigned short swdelay[2] = {RAMPDELAY1, RAMPDELAY2};
unsigned short nup[2] = {0, 0};
unsigned short npeak[2] = {0, 0};
double thresholdUp[2] = {0, 0};
double thresholdDown[2] = {0, 0};
byte precdval[6]={false, false,false, false,false, false};
bool run[2] = {false, false};

double getAVG(byte n) {
  return avg[n];
}

double getThresholdUp(byte n) {
  return thresholdUp[n];
}

void setThresholdUp(double trsh, byte n){
	thresholdUp[n] = trsh;
}

double getThresholdDown(byte n) {
  return thresholdDown[n];
}

double inline getSTDDEV(byte n) {
  return sqrt(stdDev[n] / (count[n] - 1));
}
/*
inline bool switchd(byte dval, unsigned short d[], byte n){
	//passo di campionamento
	bool changed = false;
	if(count2[n] >= d[n]){
		count2[n] = 0;
		changed = (dval != precdval[n]);
		precdval[n] = dval;            // valore di val campionato al loop precedente 
	}
	count2[n] ++;
	return changed;
}
*/
inline bool switchd(byte dval, byte n){
	bool changed = false;
	changed = (dval != precdval[n]);
	precdval[n] = dval;            // valore di val campionato al loop precedente 
	return changed;
}

short checkRange(double mval, byte n) {
	short res = 0; //res init!!
	
	DEBUG_PRINT(F("\n("));
	DEBUG_PRINT(n);
	DEBUG_PRINT(F(") mval: "));
	DEBUG_PRINT(mval);
	DEBUG_PRINT(F(" - thresholdUp[n]: "));
	DEBUG_PRINT(thresholdUp[n]);
	DEBUG_PRINT(F(" - thresholddown[n]: "));
	DEBUG_PRINT(thresholdDown[n]);
	//DEBUG_PRINT(F(" - fixedThreshld[n]: "));
	//DEBUG_PRINT(fixedThreshld[n]);
	
	//Edges evaluations -----------------------------------------------------------------------------------
	//soglia cronometro marcia
	if(switchd((mval > ONGAP),n+4)){
		//sono su un fronte
		DEBUG_PRINT(F("\n("));
		DEBUG_PRINT(n);
		if (mval > ONGAP){
			//Fronte di salita
			DEBUG_PRINT(F(")Fronte marcia cronometrata: sopra ONGAP...Start cronometro!"));
			res = 1;
			run[n] = true;
		}
	}
	//soglia blocco motore
	if(switchd(mval < thresholdDown[n],n)){
		//sono su un fronte
		DEBUG_PRINT(F("\n("));
		DEBUG_PRINT(n);
		if(mval < thresholdDown[n]){
			//Fronte di discesa
			DEBUG_PRINT(F(") Fronte di blocco marcia motore: sotto minimo...Stop!"));
			res = -1;   
			nup[n] = 0;
			run[n] = false;
		}
	}
	//End of edges evaluations ---------------------------------------------------------------------------
	
	//level evaluation -----------------------------------------------------------------------------------
	//soglia di marcia motore
	if(mval > thresholdDown[n]){
		//se il motore è in marcia
		DEBUG_PRINT(F("\n("));
		DEBUG_PRINT(n);
		DEBUG_PRINT(F(") avg[n]: "));
		DEBUG_PRINT(avg[n]);
       	
		//fronte soglia di urto potenziale
		if(switchd(mval > thresholdUp[n], n+2)) {
			if(mval > thresholdUp[n] && mval > ONGAP){
				//filtro picco di avvio
				DEBUG_PRINT(F("- Sopra massimo nup[n]: "));
				DEBUG_PRINT(nup[n]);
				if(nup[n] > npeak[n]){
					res = 2;
					run[n] = false;
					DEBUG_PRINT(F(" - Urto...Stop!"));
				}else{
					//first rising is allowed
					DEBUG_PRINT(F(" - Primo picco...Partenza!")); //non è un urto, è la partenza!
					//avg[n] = mval;
				}
				nup[n]++;
			}else{
				nup[n] = 0;
			}
		}
		
        //calcolo delle medie attivo solo sulla marcia cronometrata!
		if(run){
			//calcolo statistiche (media e varianza) solo con motore in movimento	
			count[n]++;		
			double delta = (double) mval - avg[n];
			count[n] && (avg[n] += (double) delta / count[n]);  //protected against overflow by a logic short circuit
			stdDev[n] += (double) delta * (mval - avg[n]);
			thresholdDown[n] = (double) avg[n]/4;
			(count[n] > 1) && (thresholdUp[n] = (double) avg[n] + (getSTDDEV(n) * NSIGMA)); //protected against overflow by a logic short circuit
		}else{
			DEBUG_PRINT(F(" - marcia fittizia: medie congelate! "));
		}
	}else{
		//se il motore è fermo
		nup[n] = 0;	
		//reset Fronte marcia cronometrata!!!
		//precdval[n+4] = false;
	}
	//End of level evaluation --------------------------------------------------------------------------------------------------------- 
	
	return res;
}
/*
// https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance
 count = count + 1 
    delta = newValue - mean
    mean = mean + delta / count
    delta2 = newValue - mean
    M2 = M2 + delta * delta2
*/

void updateUpThreshold(byte n) {
  //fixedThreshld[n] = mesrdThreshld[n] * 3 / 4; 
  npeak[n] = 0; 
}

void disableUpThreshold(byte n) {
  npeak[n] = 255; 
}

void resetEdges(byte n) {
  //reset Fronte marcia cronometrata!!!
  precdval[n+4] = false;
  precdval[n+2] = false;
  precdval[n] = true;
}

void resetAVGStats(double val, byte n) {
  avg[n] = val;
  stdDev[n] = 0.0;
  count[n] = 1;
  //count2[n] = 0;
}

void resetStatDelayCounter(byte n) {
  //count2[n] = 0;
  precdval[n]=false;
}

/*if(mval > thresholdUp[n] && mval > fixedThreshld[n]) {
			//filtro picco di avvio
			DEBUG_PRINT(F("- fixedThreshld: "));
			DEBUG_PRINT(fixedThreshld[n]);
			mesrdThreshld[n] = mval;
			fixedThreshld[n] = mesrdThreshld[n] * 3 / 4; 
		}*/

