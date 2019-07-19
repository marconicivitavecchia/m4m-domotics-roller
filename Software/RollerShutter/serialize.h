#ifndef __SERIALIZE__
#define __SERIALIZE__
#include <Arduino.h>
#include "common.h"

void parseJsonFieldArrayToInt(String, byte [], String (&)[MQTTJSONDIM], int, int,int);
void parseJsonFieldArrayToStr(String , String (&)[CONFJSONDIM], String (&)[CONFJSONDIM], int, int, int);
#endif //__SERIALIZE__