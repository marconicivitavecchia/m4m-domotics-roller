#ifndef __EEPROM_UTILS__
#define __EEPROM_UTILS__

void initEEPROM(int);
bool validateEEPROM();
void alterEEPROM();
int EEPROMReadStr(int, char*,int len = 1000);
int EEPROMWriteStr(int,const char* ,int len = 0);
void EEPROMWriteInt(int, int); 
int EEPROMReadInt(int);
void EEPROMWriteFloat(int, float); //4 byte
float EEPROMReadFloat(int); //4 byte

#endif //EEPROM_UTILS


