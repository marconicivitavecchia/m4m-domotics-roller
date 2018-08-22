#include "statistics.h"

double avg[2] = {0.0, 0.0};
double stdDev[2] = {0.0, 0.0};
unsigned int count[2] = {1, 1};
double thresholdUp[2] = {1024,1024};
double thresholdDown[2] = {-1024,-1024};
bool learn = false;
//double sigma = NSIGMA;

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
  
   
  count[n]++;
  double delta = (double) val - avg[n];
  avg[n] += (double) delta / count[n];
  
  if(learn){
	stdDev[n] += (double) delta * (val - avg[n]);
	if (count[n] > 1) {
		thresholdUp[n] = (double) avg[n] + (getSTDDEV(n) * NSIGMA);
		DEBUG_PRINT(F("thresholdUp[n]: "));
		DEBUG_PRINTLN(thresholdUp[n]);
	}
  }
  else if(val > thresholdUp[n]) {
		res = 1;
  }
  
  if (count[n] > 1) {
	thresholdDown[n] = (double) avg[n]/3;
  }
  
  if(val < thresholdDown[n]) {
	res = -1;
  }	
  
  return res;
}

/*
 count = count + 1 
    delta = newValue - mean
    mean = mean + delta / count
    delta2 = newValue - mean
    M2 = M2 + delta * delta2
*/

void resetAVGStats(byte n) {
  avg[n] = 0.0;
  stdDev[n] = 0.0;
  count[n] = 1;
}

