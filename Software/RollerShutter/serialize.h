#ifndef __SERIALIZE__
#define __SERIALIZE__
#include <Arduino.h>
#include "common.h"

bool parseJsonFieldArrayToInt(String, Par*[], byte [], String (&)[MQTTDIM], int, int,int);
void parseJsonFieldArrayToStr(String, Par*[], String (&)[CONFDIM], String (&)[EXTCONFDIM], byte [], int, int, int, char, String);
#endif //__SERIALIZE__