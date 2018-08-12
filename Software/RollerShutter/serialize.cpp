#include "serialize.h"


int parseJsonFieldToInt(String &srcJSONStr, String &field, int valueLen){
	int start, ends;
	
	valueLen+=3;
	start = srcJSONStr.indexOf(field + "\":\"") + field.length() + 3; // ":" --> 3
	if(start > srcJSONStr.length() + 2){ //se la sottostringa ha effettivamente un valore dopo :
		ends = start + valueLen; // "0"= n+\0+\n+\r --> 4
		return (srcJSONStr.substring(start, ends)).toInt();
	}else
		return -1;
}

void parseJsonFieldArrayToInt(String srcJSONStr, byte destIntArr[], String (&fieldArr)[MQTTJSONDIM], int valueLen, int arrLen, int first=0){
	int start, ends, last;
	valueLen+=3;
	last=0;
	if(first < 0 || first > arrLen)
		first=0;

	Serial.println(F("Dentro il parser"));
	for(int i=first; i<arrLen; i++){
		//Serial.print(", ");
		///calcola l'inizio del valore
		start = srcJSONStr.indexOf(fieldArr[i] + "\":\"") + (fieldArr[i]).length() + 3; // ":" --> 3
		//calcola la fine del valore 
		ends = start + valueLen; // "0"= n+\0+\n+\r --> 4
		//estrae il campo e lo converte in intero e aggiorna gli ingressi dello switchf X
		//outLogic[mapLogic[i-1]] = (response.substring(start, ends)).toInt();
		destIntArr[i-first] = (srcJSONStr.substring(start, ends)).toInt();
		//Serial.println(srcJSONStr.substring(start, ends));
		//Serial.println(destIntArr[i-first]);
	}  
}
