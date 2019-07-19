#include "serialize.h"


void parseJsonFieldArrayToStr(String srcJSONStr, String (&destStrArr)[CONFJSONDIM], String (&fieldArr)[CONFJSONDIM], int valueLen, int arrLen, int first=0){
	int start, ends=0;
	valueLen+=3;
	if(first < 0 || first > arrLen)
		first=0;

	DEBUG_PRINT(F("\nParser configurazioni: "));
	DEBUG_PRINT(srcJSONStr);
	DEBUG_PRINTLN(F("- campi: "));
	for(int i=first; i<arrLen; i++){
		///calcola l'inizio del valore
		start = srcJSONStr.indexOf("\""+fieldArr[i] + "\":\"");
		if(start > 0){
			start += (fieldArr[i]).length() + 4;
			for(ends=start+1; ends < start + valueLen && srcJSONStr.charAt(ends)!='"'; ends++);
			//calcola la fine del valore 
			destStrArr[i-first] = srcJSONStr.substring(start, ends);
			//flagArr[i-first] = 1;
			DEBUG_PRINT(srcJSONStr.substring(start, ends));
		}
	}
}

void parseJsonFieldArrayToInt(String srcJSONStr, byte destIntArr[], String (&fieldArr)[MQTTJSONDIM], int valueLen, int arrLen, int first=0){
	int start, ends=0;
	valueLen+=3;
	if(first < 0 || first > arrLen)
		first=0;

	DEBUG_PRINT(F("\nParser comandi: "));
	DEBUG_PRINT(srcJSONStr);
	DEBUG_PRINTLN(F("- campi: "));
	
	for(int i=first; i<arrLen; i++){
		start = srcJSONStr.indexOf("\""+fieldArr[i] + "\":\"");
		if(start > 0){
			start += (fieldArr[i]).length() + 4;
			for(ends=start+1; ends < start + valueLen && srcJSONStr.charAt(ends)!='"'; ends++);
			//calcola la fine del valore 
			//estrae il campo e lo converte in intero e aggiorna gli ingressi dello switchf X
			destIntArr[i-first] = (srcJSONStr.substring(start, ends)).toInt();
			if(destIntArr[i-first] == 0){
				DEBUG_PRINT(F("\n:Stringa to num "));
				DEBUG_PRINTLN(srcJSONStr.charAt(start));
				if(srcJSONStr.charAt(start) != '0'){
					destIntArr[i-first] = 1;
				}
			}				
			DEBUG_PRINT(srcJSONStr.substring(start, ends));
			DEBUG_PRINT(F(" - start: "));
			DEBUG_PRINT(start);
			DEBUG_PRINT(F(" - end: "));
			DEBUG_PRINT(ends);
			DEBUG_PRINT(F(" - "));
		}
	}  
}
