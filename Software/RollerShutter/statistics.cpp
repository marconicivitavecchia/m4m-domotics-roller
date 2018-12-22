#include "statistics.h"
#define MAXDELAY 4
double avg[2] = {0.0, 0.0};
double stdDev[2] = {0.0, 0.0};
unsigned long count[2] = {1, 1};
short count2[2] = {0, 0};
short countd[2] = {0, 0};
unsigned short swdelay[2] = {1, 1};
unsigned short nup[2] = {0, 0};
double thresholdUp[2] = {1024, 1024};
double thresholdDown[2] = {0, 0};
unsigned fixedThreshld = 13;
//bool started[2] = {false, false};
//bool highLevel[2]={false, false};
byte precdval[2]={false, false};
//bool learn = false;

//double sigma = NSIGMA;
/*
void setStatsLearnMode(){
	learn = true;
	thresholdUp[0] = 1024;
	thresholdUp[1] = 1024;
	avg[0] = 0.0;
	avg[1] = 0.0;
}

void clrStatsLearnMode(){
	learn = false;
}

bool isStatsLearnMode(){
	return learn;
}
*/

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
	count2[n] ++;
	bool changed = false;
	if(count2[n] >= d[n]){
		count2[n] = 0;
		changed = (dval != precdval[n]);
		precdval[n] = dval;            // valore di val campionato al loop precedente 
	}
	return changed;
}
*/

inline bool switchd(byte dval, unsigned short d[], byte n){
	//passo di campionamento
	count2[n] ++;
	bool changed = false;
	if(count2[n] >= d[n]){
		count2[n] = 0;
		changed = (dval != precdval[n]);
		precdval[n] = dval;            // valore di val campionato al loop precedente 
	}
	return changed;
}
/*
inline bool switchd(byte dval, byte n){
	bool changed = false;
	changed = (dval != precdval[n]);
	precdval[n] = dval;            // valore di val campionato al loop precedente 
	return changed;
}
*/
/*
void setSigma(byte i) {
	if(i > 0){
		sigma = (double) i*NSIGMA;
	}
}

double getSigma() {
  return sigma;
}
*/
short checkRange2(double mval, byte n) {
	short res = 0; //res init!!
	DEBUG_PRINTLN(n);
	DEBUG_PRINT(F(") mval: "));
	DEBUG_PRINT(mval);
	DEBUG_PRINT(F(" - thresholdUp[n]: "));
	DEBUG_PRINT(thresholdUp[n]);
	DEBUG_PRINT(F(" - thresholddown[n]: "));
	DEBUG_PRINT(thresholdDown[n]);
	
	//level evaluation
	//started[n] = started[n] && (mval > thresholdUp[n]); mval > thresholdUp[n]
	if(mval > thresholdDown[n]){
		//sono sul livello alto
		//calcolo statistiche solo con motore in movimento	
		DEBUG_PRINTLN(n);
		DEBUG_PRINT(F(") avg[n]: "));
		DEBUG_PRINT(avg[n]);
		
		double delta = (double) mval - avg[n];
		count[n] && (avg[n] += (double) delta / count[n]);  //protected against overflow by a logic short circuit
		stdDev[n] += (double) delta * (mval - avg[n]);
		thresholdDown[n] = (double) avg[n]/4;
		(count[n] > 1) && (thresholdUp[n] = (double) avg[n] + (getSTDDEV(n) * NSIGMA));	//protected against overflow by a logic short circuit
		
		if(mval > thresholdUp[n] && mval > fixedThreshld) {
			//filtro picco di avvio
			DEBUG_PRINTLN(n);
			DEBUG_PRINT(F(") Sopra massimo - nup[n]: "));
			DEBUG_PRINT(nup[n]);
			if(nup[n] > 0){
				res = 2;
			}else{
				//first rising is allowed
				DEBUG_PRINT(F(" - Primo picco"));
				avg[n] = mval;
			}
			nup[n]++;
		}
	}
	
	if(switchd(mval > thresholdDown[n],swdelay,n)){
	//if(switchd(mval > thresholdDown[n],n)){
		//sono su un fronte
		DEBUG_PRINTLN(n);
		if (mval > thresholdDown[n]){
			//Fronte di salita
			DEBUG_PRINT(F(")Fronte di salita sensore"));
			res = 1;
		}else{
			//Fronte di discesa
			DEBUG_PRINT(F(") Fronte di discesa sensore - sotto minimo"));
			res = -1;   
			thresholdUp[n] = 1024;
			nup[n] = 0;
		}
	}
	
	return res;
}

short checkRange(double mval, byte n) {
	short res = 0; //res init!!
	
	DEBUG_PRINTLN(n);
	DEBUG_PRINT(F(") mval: "));
	DEBUG_PRINT(mval);
	DEBUG_PRINT(F(" - thresholdUp[n]: "));
	DEBUG_PRINT(thresholdUp[n]);
	DEBUG_PRINT(F(" - thresholddown[n]: "));
	DEBUG_PRINT(thresholdDown[n]);
	
	if(switchd(mval > thresholdDown[n],swdelay,n)){
	//if(switchd(mval > thresholdDown[n],n)){
		//sono su un fronte
		DEBUG_PRINTLN(n);
		if (mval > thresholdDown[n]){
			//Fronte di salita
			DEBUG_PRINT(F(")Fronte di salita sensore"));
			res = 1;
		}else{
			//Fronte di discesa
			DEBUG_PRINT(F(") Fronte di discesa sensore - sotto minimo"));
			res = -1;   
			thresholdUp[n] = 1024;
			nup[n] = 0;
		}
	}
	
	//level evaluation
	if(mval > thresholdDown[n]){
		DEBUG_PRINTLN(n);
		//sono sul livello alto
		//calcolo statistiche solo con motore in movimento		
		DEBUG_PRINT(F("avg[n]: "));
		DEBUG_PRINT(avg[n]);
			
		if(mval > thresholdUp[n] && mval > fixedThreshld) {
			//filtro picco di avvio
			DEBUG_PRINTLN(n);
			DEBUG_PRINT(F(") Sopra massimo - nup[n]: "));
			DEBUG_PRINT(nup[n]);
			if(nup[n] > 0){
				res = 2;
			}else{
				//first rising is allowed
				DEBUG_PRINT(F(" - Primo picco"));
				avg[n] = mval;
			}
			nup[n]++;
		}
		
		count[n]++;		
		double delta = (double) mval - avg[n];
		count[n] && (avg[n] += (double) delta / count[n]);  //protected against overflow by a logic short circuit
		stdDev[n] += (double) delta * (mval - avg[n]);
		thresholdDown[n] = (double) avg[n]/4;
		(count[n] > 1) && (thresholdUp[n] = (double) avg[n] + (getSTDDEV(n) * NSIGMA)); //protected against overflow by a logic short circuit
	}

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


