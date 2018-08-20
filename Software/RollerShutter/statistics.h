#ifndef __STATISTICS__
#define	__STATISTICS__

#include <Arduino.h>
#include "common.h"

double getAVG(byte);
//void setSigma(byte);
//double getSigma();
void resetAVGStats(byte);
double getSTDDEV(byte);
short checkMaxVal(double, byte);
double getThresholdUp(byte);
double getThresholdDown(byte);
//double gethreshold();

#endif
