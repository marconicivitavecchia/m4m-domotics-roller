#ifndef __STATISTICS__
#define	__STATISTICS__

#include <Arduino.h>
#include "common.h"

double getAVG(byte);
//void setSigma(byte);
//double getSigma();
void resetAVGStats(byte);
double getSTDDEV(byte);
short checkRange(double, byte);
double getThresholdUp(byte);
double getThresholdDown(byte);
void setStatsLearnMode();
void clrStatsLearnMode();
bool isStatsLearnMode();
//double gethreshold();

#endif
