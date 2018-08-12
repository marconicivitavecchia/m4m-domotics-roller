#ifndef __EEPROM_UTILS__
#define __EEPROM_UTILS__

void initEEPROM(int);
bool validateEEPROM();
void alterEEPROM();
void EEPROMReadStr(int, char*);
void EEPROMWriteStr(int, const char*);
void EEPROMWriteInt(int, int); 
int EEPROMReadInt(int);

#endif //EEPROM_UTILS


