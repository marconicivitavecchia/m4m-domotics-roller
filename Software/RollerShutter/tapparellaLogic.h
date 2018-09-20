#ifndef __TAPPARELLA_LOGIC__
#define __TAPPARELLA_LOGIC__

#include <Arduino.h>
#include "common.h"

void initTapparellaLogic(byte *, byte *, byte *, String  *, bool);
byte tapparellaLogic(byte *, byte *, byte*, unsigned long , byte n);
//byte tapparellaLogic(byte);
void setTapThalt(unsigned long, byte);
unsigned long getTapThalt(byte);
//extern void onTapStop(byte);
//extern void onTapStart(byte);
extern void onCalibrEnd(unsigned long, byte);
long getTarget(byte);
//void firstPressDown(byte);
void firstPress(byte, byte);
//void firstPressUp(byte);
short secondPress(byte);
byte getGroupState(byte);
void setGroupState(byte, byte);
byte getDelayedCmd(byte);
void setDelayedCmd(byte, byte);
void setBtnDelay(byte, byte);
void startEndOfRunTimer(byte);
bool startEngineDelayTimer(byte);
bool isRunning(byte);
bool isMoving(byte);
byte nRunning();
void enableShortRun(byte);
void disableShortRun(byte);
byte switchLogic(byte, byte);
double calcTiming(byte);
double calcLen(byte n);
//bool inline initdfn(byte,  byte);
//extern void onBtnPress(n)
//extern void onDownPressed(byte);
//extern void onUpPressed(byte);
#endif //__TAPPARELLA_LOGIC__