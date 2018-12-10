#include "statistics.h"

double avg[2] = {0.0, 0.0};
double stdDev[2] = {0.0, 0.0};
unsigned long count[2] = {1, 1};
short count2[2] = {0, 0};
short countd[2] = {0, 0};
double thresholdUp[2] = {1024, 1024};
double thresholdDown[2] = {0, 0};
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

inline bool switchd(byte dval, byte d, byte n){
	//passo di campionamento
	count2[n] ++;
	bool changed = false;
	if(count2[n] >= d){
		count2[n] = 0;
		changed = (dval != precdval[n]);
		precdval[n] = dval;            // valore di val campionato al loop precedente 
	}
	return changed;
}
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
short checkRange(double mval, byte n) {
	short res = 0; //res init!!
	
	DEBUG_PRINT(F("mval: "));
	DEBUG_PRINTLN(mval);
	DEBUG_PRINT(F("thresholdUp[n]: "));
	DEBUG_PRINTLN(thresholdUp[n]);
	DEBUG_PRINT(F("thresholddown[n]: "));
	DEBUG_PRINTLN(thresholdDown[n]);

	if(switchd(mval > thresholdDown[n],2,n)){
		//sono su un fronte
		if (mval > thresholdDown[n]){
			//Fronte di salita
			DEBUG_PRINTLN(F("Fronte di salita sensore"));
			res = 1;
		}else{
			//Fronte di discesa
			DEBUG_PRINTLN(F("Fronte di discesa sensore "));
			DEBUG_PRINTLN(F("Sotto minimo"));
			res = -1;
		}
	}
	
	//level evaluation
	if(mval > thresholdDown[n]){
		//sono sul livello alto
		//calcolo statistiche solo con motore in movimento		
		DEBUG_PRINT(F("avg[n]: "));
		DEBUG_PRINTLN(avg[n]);
			
		if(mval > thresholdUp[n]) {
			countd[n]++;
			DEBUG_PRINTLN(F("Sopra massimo"));
			//res = 2;
			//res = (countd[n] >= 2) + 1;
		}else{
			countd[n]=0;
		}
		
		count[n]++;		
		double delta = (double) mval - avg[n];
		count[n] && (avg[n] += (double) delta / count[n]);  //protected against overflow by a logic short circuit
		stdDev[n] += (double) delta * (mval - avg[n]);
		if (count[n] > 1) {
			thresholdUp[n] = (double) avg[n] + (getSTDDEV(n) * NSIGMA);
			thresholdDown[n] = (double) avg[n]/3;
		}
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


