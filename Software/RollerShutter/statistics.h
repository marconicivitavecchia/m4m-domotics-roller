#ifndef __STATISTICS__
#define	__STATISTICS__

#include <Arduino.h>
#include "common.h"

double getAVG(byte);
//void setSigma(byte);
//double getSigma();
void resetAVGStats(double, byte);
void resetStatDelayCounter(byte);
double getSTDDEV(byte);
short checkRange(double, byte);
//short checkRange2(double, byte);
//short checkRange3(double, byte);
double getThresholdUp(byte);
double getThresholdDown(byte);
//void setStatsLearnMode();
//void clrStatsLearnMode();
//bool isStatsLearnMode();
void setThresholdUp(double, byte);
//double gethreshold();
void disableUpThreshold(byte);
void updateUpThreshold(byte);
#endif
