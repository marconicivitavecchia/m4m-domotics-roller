#ifndef __SERIALIZE__
#define __SERIALIZE__
#include <Arduino.h>
#include "common.h"

int parseJsonFieldToInt(String &, String &, int valueLen);
void parseJsonFieldArrayToInt(String, byte [], String (&)[MQTTJSONDIM], int, int,int);

#endif //__SERIALIZE__