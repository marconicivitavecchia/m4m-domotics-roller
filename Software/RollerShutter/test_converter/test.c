#ifndef PROGMEM
#define PROGMEM
#endif

#include <stdio.h>

#include "./HTTP_FORM_HEAD.html.c"

int main() {
    printf("converter test\n");
    printf("%c%c%c",HTTP_FORM_HEAD[0],HTTP_FORM_HEAD[1],HTTP_FORM_HEAD[2]);
    return 0;
}