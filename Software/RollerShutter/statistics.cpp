#include "statistics.h"

double avg[2] = {0.0, 0.0};
double stdDev[2] = {0.0, 0.0};
unsigned long count[2] = {1, 1};
short count2[2] = {0, 0};
double thresholdUp[2] = {1024, 1024};
double thresholdDown[2] = {-1024, -1024};
bool passed=false;
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
	short res = 0;
	//res first init to 1 or 0	
	bool chg = switchd(mval > thresholdDown[n],2,n);
	DEBUG_PRINTLN(chg);
	
	if(chg && (mval > thresholdDown[n])){
		DEBUG_PRINTLN(F("Fronte di salita sensore"));
		passed = true;
		res = 1;
	}
	
	//global variable passed evaluation
	if(passed){	
		DEBUG_PRINT(F("mval: "));
		DEBUG_PRINTLN(mval);
		DEBUG_PRINT(F("avg[n]: "));
		DEBUG_PRINTLN(avg[n]);
		
		//count2[n] += (count2[n] < 6);
		count[n]++;		
		double delta = (double) mval - avg[n];
		count[n] && (avg[n] += (double) delta / count[n]);  //protected against overflow by a logic short circuit
		stdDev[n] += (double) delta * (mval - avg[n]);
		if (count[n] > 1) {
			thresholdUp[n] = (double) avg[n] + (getSTDDEV(n) * NSIGMA);
			thresholdDown[n] = (double) avg[n]/2;
			DEBUG_PRINT(F("thresholdUp[n]: "));
			DEBUG_PRINTLN(thresholdUp[n]);
		}

		if((mval > thresholdUp[n]) && count2[n] >= 2) {
			res = 2;
			DEBUG_PRINT(F("Sopra massimo"));
			//count2[n]=0;
			//passed = false;
		}
		
		if(chg && (mval < thresholdDown[n])){
			DEBUG_PRINTLN(F("Fronte di discesa sensore "));
			res = -1;
			passed = false;
			//count2[n]=0;
			DEBUG_PRINT(F("Sotto minimo"));
		}
		
	}else{
		res = 0;
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


