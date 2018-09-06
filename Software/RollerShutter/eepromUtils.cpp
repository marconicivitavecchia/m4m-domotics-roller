//-------------------------------------------------------------------
//fine variabili globali
#include "eepromUtils.h"
#include <EEPROM.h>
#include <Arduino.h>

void alterEEPROM() {
  EEPROM.write(5, 'U');
}

void initEEPROM(int len) {
  for (int i = 0 ; i < len; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.write(0, 'S');
  EEPROM.write(1, 'O');
  EEPROM.write(2, 'N');
  EEPROM.write(3, 'O');
  EEPROM.write(4, 'F');
  EEPROM.write(5, 'F');
}

bool validateEEPROM() {
  return EEPROM.read(0) == 'S'
    && EEPROM.read(1) == 'O'
    && EEPROM.read(2) == 'N'
    && EEPROM.read(3) == 'O'
    && EEPROM.read(4) == 'F'
    && EEPROM.read(5) == 'F';
}

void EEPROMReadStr(int offset, char* buf){
	for (int i =0; i < 32; ++i)
		{
			buf[i] = char(EEPROM.read(i+offset));
		}
}

void EEPROMWriteStr(int offset, const char* buf){
	for (int i = 0; i < 32; ++i)
		{
			EEPROM.write(i+offset, buf[i]);
		}
}

void EEPROMWriteInt(int offset, int value)
{
  byte two = (value & 0xFF);
  byte one = ((value >> 8) & 0xFF);
  
  EEPROM.write(offset, two);
  EEPROM.write(offset + 1, one);
}
 
int EEPROMReadInt(int offset)
{
  long two = EEPROM.read(offset);
  long one = EEPROM.read(offset + 1);
 
  return ((two << 0) & 0xFFFFFF) + ((one << 8) & 0xFFFFFFFF);  //  x<<y == x*2^y
}

