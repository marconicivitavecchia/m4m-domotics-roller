#ifndef __ABPARSER__
#define __ABPARSER__
#include <Arduino.h>
#include "common.h"

extern double variables(char *);
extern double actions(char *, double);
double eval(const char *str);

#endif //__ABPARSER__
