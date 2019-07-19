#ifndef __ABPARSER__
#define __ABPARSER__
#include <Arduino.h>
#include "common.h"

extern float variables(char *);
extern float actions(char *, float);
float eval(const char *str);

#endif //__ABPARSER__
