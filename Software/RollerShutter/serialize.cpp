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
	int start, ends=0;
	valueLen+=3;
	if(first < 0 || first > arrLen)
		first=0;

	DEBUG_PRINT(F("\nDentro il parser: "));
	DEBUG_PRINT(srcJSONStr);
	DEBUG_PRINT(F("- campi: "));
	for(int i=first; i<arrLen; i++){
		///calcola l'inizio del valore
		start = srcJSONStr.indexOf("\""+fieldArr[i] + "\":\"");
		if(start > 0){
			start += (fieldArr[i]).length() + 4;
			for(ends=start+1; ends < start + valueLen && srcJSONStr.charAt(ends)!='"'; ends++);
			//calcola la fine del valore 
			//estrae il campo e lo converte in intero e aggiorna gli ingressi dello switchf X
			destIntArr[i-first] = (srcJSONStr.substring(start, ends)).toInt();
			DEBUG_PRINT(srcJSONStr.substring(start, ends));
			DEBUG_PRINT(F(" - start: "));
			DEBUG_PRINT(start);
			DEBUG_PRINT(F(" - end: "));
			DEBUG_PRINT(ends);
			DEBUG_PRINT(F(" - "));
		}
	}  
}
