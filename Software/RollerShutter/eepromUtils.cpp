//-------------------------------------------------------------------
//fine variabili globali
#include "eepromUtils.h"
#include <EEPROM.h>
#include <Arduino.h>

void alterEEPROM() { // 1 byte
  EEPROM.write(2, 'U');
  
  EEPROM.commit();
}

void initEEPROM(int len) { //2 byte
  EEPROM.write(0, 'S');
  EEPROM.write(1, 'O');
  
  for (int i = 2 ; i < len; i++) {
    EEPROM.write(i, 0);
  }
 
  EEPROM.commit();
}

bool validateEEPROM() { //2 byte
  return EEPROM.read(0) == 'S'
    && EEPROM.read(1) == 'O';
}

int EEPROMWriteStr(int ofst,const char* buf, int) //variable lenght
{
  int _size, len, i;
  
  if(len == 0)
	_size = strlen(buf);
  else
	_size =len;  
   
  for(i=0;i<_size;i++)
  {
    EEPROM.write(ofst+i,buf[i]);
  }
  EEPROM.write(ofst+_size,'\0');   //Add termination null character for String Data
  
  EEPROM.commit();
  return _size + 1; //plus null character
}

int EEPROMReadStr(int ofst,char* buf, int len) //variable lenght
{
  int i;
  unsigned char k;
  int c = 0;
  //Read until null character
  do{    
    k=EEPROM.read(ofst+c);
    buf[c]=k;
    ++c;
  }while(k != '\0' && c<len);
	  
  return c;
}

/*
void EEPROMReadStr32(int offset, char* buf){ //32 byte
	for (int i =0; i < 32; ++i)
		{
			buf[i] = char(EEPROM.read(i+offset));
		}
}

void EEPROMWriteStr32(int offset, const char* buf){ //32 byte
	for (int i = 0; i < 32; ++i)
		{
			EEPROM.write(i+offset, buf[i]);
		}
	
	EEPROM.write(--i+offset, '\0');	
	EEPROM.commit();
}
*/
void EEPROMWriteInt(int offset, int value) //2 byte
{
  byte lByte = (value & 0xFF);
  byte hByte = ((value >> 8) & 0xFF);
  
  EEPROM.write(offset, lByte);
  EEPROM.write(offset + 1, hByte);
  
  EEPROM.commit();
}
 
int EEPROMReadInt(int offset) //2 byte
{
  long lByte = EEPROM.read(offset);
  long hByte = EEPROM.read(offset + 1);
 
  return ((lByte << 0) & 0xFFFFFF) + ((hByte << 8) & 0xFFFFFFFF);  //  x<<y == x*2^y
}

void EEPROMWriteFloat(int offset, float value) //4 byte
{
	EEPROM.put(offset,value);
	
	EEPROM.commit();
}
 
float EEPROMReadFloat(int offset) //4 byte
{
	float v;
	EEPROM.get(offset,v);
	return v;
}

void EEPROMWriteLong(int offset, long value) //4 byte
{
	EEPROM.put(offset,value);
	
	EEPROM.commit();
}
 
long EEPROMReadLong(int offset) //4 byte
{
	long v;
	EEPROM.get(offset,v);
	return v;
}
/*
void EEPROMWriteDouble(int ee, double value) //4 o byte
{
   byte* p = (byte*)(void*)&value;
   for (int i = 0; i < sizeof(value); i++)
       EEPROM.write(ee++, *p++);
}

double EEPROMReadDouble(int ee) //4 o byte
{
   double value = 0.0;
   byte* p = (byte*)(void*)&value;
   for (int i = 0; i < sizeof(value); i++) 
       *p++ = EEPROM.read(ee++);
   return value;
}
*/