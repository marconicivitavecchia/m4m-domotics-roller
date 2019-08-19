#include "serialize.h"


void parseJsonFieldArrayToStr(String srcJSONStr, Par*p[], String (&destStrArr)[CONFDIM], String (&fieldArr)[EXTCONFDIM], byte confFlags[], int valueLen, int arrLen, int first, char delim, String op){
	int start, ends=0;
	byte count = 0;
	
	valueLen+=3;
	if(first < 0 || first > arrLen)
		first=0;
	String app;
	
	DEBUG_PRINT(F("\nParser configurazioni, MSG: "));
	DEBUG_PRINTLN(srcJSONStr);
	
	//field counter
	for(ends = 0; ends < srcJSONStr.length(); ends++)
		if(srcJSONStr.charAt(ends) == ',') count++;
	count++;
	
	for(int i=first; i<arrLen && count > 0; i++){
		///calcola l'inizio del valore
		start = srcJSONStr.indexOf("\""+fieldArr[i] + "\":\"");
		if(start > 0){
			count--;
			DEBUG_PRINT(F("- campo: "));
			DEBUG_PRINT("\""+fieldArr[i] + "\":\"");
			DEBUG_PRINT(F("TROVATO"));
			
			confFlags[i] = HIGH;
			
			start += (fieldArr[i]).length() + 4;
			for(ends=start+1; ends < start + valueLen && srcJSONStr.charAt(ends)!='"' && ends < srcJSONStr.length(); ends++);
			app = srcJSONStr.substring(start, ends);
			
			DEBUG_PRINT(app);
			DEBUG_PRINT(F(" - start: "));
			DEBUG_PRINT(start);
			DEBUG_PRINT(F(" - end: "));
			DEBUG_PRINT(ends);
			DEBUG_PRINT(F(" - "));
			
			if(app[1] != delim){//default append
				destStrArr[i-first] = app;
			}else{
				if(app[0] == 'a'){//append
					destStrArr[i-first] += " " + op + " "+ app.substring(2);
				}else if(app[0] == 'w'){//overwrite
					destStrArr[i-first] = app.substring(2);
				}else{//default append
					destStrArr[i-first] += " " + op + " "+ app.substring(2);
				}
			}
			DEBUG_PRINT(F("- index: "));
			DEBUG_PRINT(i);
			DEBUG_PRINT(F("-"));
			DEBUG_PRINT(p(i));
			DEBUG_PRINT(F("- flagname: "));
			DEBUG_PRINT(p[p(i)]->parname);
			DEBUG_PRINT(F("- destStrArr[i-first]: "));
			DEBUG_PRINTLN(destStrArr[i-first]);

			p[p(i)]->doaction();
			
		}else{
			DEBUG_PRINT(F("- campo: "));
			DEBUG_PRINT("\""+fieldArr[i] + "\":\"");
			DEBUG_PRINTLN(F(" non trovato o non necessario"));
		}
	}
}

bool parseJsonFieldArrayToInt(String srcJSONStr, Par*p[], byte destIntArr[], String (&fieldArr)[MQTTDIM], int valueLen, int arrLen, int first=0){
	int start, ends=0;
	byte count = 0;
	
	valueLen+=3;
	if(first < 0 || first > arrLen)
		first=0;

	DEBUG_PRINT(F("\nParser comandi:, MSG: "));
	DEBUG_PRINTLN(srcJSONStr);
	
	//field counter
	for(ends = 0; ends < srcJSONStr.length(); ends++)
		if(srcJSONStr.charAt(ends) == ',') count++;
	count++;

	for(int i=first; i<arrLen && count > 0; i++){
		start = srcJSONStr.indexOf("\""+fieldArr[i] + "\":\"");
		if(start > 0){
			count--;
			DEBUG_PRINT(F("- campo: "));
			DEBUG_PRINT("\""+fieldArr[i] + "\":\"");
			DEBUG_PRINTLN(F("TROVATO"));
			
			start += (fieldArr[i]).length() + 4;
			for(ends=start+1; ends < start + valueLen && srcJSONStr.charAt(ends)!='"' && ends < srcJSONStr.length(); ends++);
			//calcola la fine del valore 
			//estrae il campo e lo converte in intero e aggiorna gli ingressi dello switchf X
			destIntArr[i-first] = (srcJSONStr.substring(start, ends)).toInt(); 
			/*if(destIntArr[i-first] == 0){ //se è 1 è sicuramente un num, ma se è 0 è un num o stringa?
				DEBUG_PRINT(F("\n:Stringa to num "));
				DEBUG_PRINTLN(srcJSONStr.charAt(start));
				if(srcJSONStr.charAt(start) != '0'){ // allora è una stringa
					destIntArr[i-first] = 1; //allora è un messaggio di configurazione, flag ON
				}
			}*/		
			DEBUG_PRINT(F("- flagname: "));
			DEBUG_PRINT(p[i]->parname);	
			p[i]->doaction();
			DEBUG_PRINT(srcJSONStr.substring(start, ends));
			DEBUG_PRINT(F(" - start: "));
			DEBUG_PRINT(start);
			DEBUG_PRINT(F(" - end: "));
			DEBUG_PRINT(ends);
			DEBUG_PRINT(F(" - "));
		}else{
			DEBUG_PRINT(F("- campo: "));
			DEBUG_PRINT("\""+fieldArr[i] + "\":\"");
			DEBUG_PRINTLN(F(" non trovato o non necessario"));
		}
	}  
	return (count > 0);
}
