#ifndef __TAPPARELLA_LOGIC__
#define __TAPPARELLA_LOGIC__

#include <Arduino.h>
#include "common.h"

void initTapparellaLogic(byte *, byte *, byte *, unsigned long, unsigned long, unsigned long, unsigned long, unsigned long, unsigned long);
byte tapparellaLogic(byte *, byte *, byte*, unsigned long , byte n);
byte tapparellaLogic(byte);
void setTapThalt(unsigned long thalt, byte n);
//extern void onTapStop(byte);
//extern void onTapStart(byte);
extern void onCalibrEnd(unsigned long, byte);
long getTarget(byte);
//void firstPressDown(byte);
void firstPress(byte, byte);
//void firstPressUp(byte);
void secondPress(byte);
byte getGroupState(byte);
void setGroupState(byte, byte);
byte getDelayedCmd(byte);
void setDelayedCmd(byte, byte);
void setBtnDelay(byte dly, byte i);
void startEndOfRunTimer(byte);
bool startEngineDelayTimer(byte);

//bool inline initdfn(byte,  byte);
//extern void onBtnPress(n)
//extern void onDownPressed(byte);
//extern void onUpPressed(byte);
#endif //__TAPPARELLA_LOGIC__