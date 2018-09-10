#include "statistics.h"

double avg[2] = {0.0, 0.0};
double stdDev[2] = {0.0, 0.0};
unsigned long count[2] = {1, 1};
short count2[2] = {0, 0};
double thresholdUp[2] = {1024,1024};
double thresholdDown[2] = {-1024,-1024};
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
void setSigma(byte i) {
	if(i > 0){
		sigma = (double) i*NSIGMA;
	}
}

double getSigma() {
  return sigma;
}
*/
short checkRange(double val, byte n) {
  byte res = 0;
  
  
  DEBUG_PRINT(F("val: "));
  DEBUG_PRINTLN(val);
  DEBUG_PRINT(F("avg[n]: "));
  DEBUG_PRINTLN(avg[n]);
  
  count2[n] += (count2[n] < 6);
   
  count[n]++;
  //count[n] = (count[n] +1) % 256;
  double delta = (double) val - avg[n];
  count[n] && (avg[n] += (double) delta / count[n]);  //protected against overflow by a logic short circuit
  stdDev[n] += (double) delta * (val - avg[n]);
  if (count[n] > 1) {
	thresholdUp[n] = (double) avg[n] + (getSTDDEV(n) * NSIGMA);
	thresholdDown[n] = (double) avg[n]/2.8;
	DEBUG_PRINT(F("thresholdUp[n]: "));
	DEBUG_PRINTLN(thresholdUp[n]);
  }

  if(val > thresholdUp[n]) {
		res = 1;
  }
  
  if(val > 0.01 && val < thresholdDown[n] && count2[n] > 4) {
		res = -1;
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
  count2[n] = 0;
}

void resetStatDelayCounter(byte n) {
  count2[n] = 0;
}


