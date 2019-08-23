#ifndef __SERIALIZE__
#define __SERIALIZE__
#include <Arduino.h>
#include "common.h"

//bool parseJsonFieldArrayToInt(String, Par*[], int, int,int);
void parseJsonFieldArrayToStr(String, Par*[], int, int, int, char, String);
#endif //__SERIALIZE__