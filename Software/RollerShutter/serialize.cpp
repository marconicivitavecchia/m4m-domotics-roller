#include "serialize.h"


void parseJsonFieldArrayToStr(String srcJSONStr, Par*p[], int valueLen, int arrLen, int first, char delim, String op){
	int start, ends=0;
	byte count = 0;
	
	valueLen+=3;
	if(first < 0 || first > arrLen)
		first=0;
	String app, app1;
	
	DEBUG_PRINT(F("\nParser configurazioni, MSG: "));
	DEBUG_PRINTLN(srcJSONStr);
	
	//field counter
	for(ends = 0; ends < srcJSONStr.length(); ends++)
		if(srcJSONStr.charAt(ends) == ',') count++;
	count++;
	
	for(int i=first; i<arrLen && count > 0; i++){
		///calcola l'inizio del valore
		start = srcJSONStr.indexOf("\""+ p[i]->getStrJsonName() + "\":\"");
		if(start > 0){
			count--;
			DEBUG_PRINT(F("- campo: "));
			DEBUG_PRINT("\""+p[i]->getStrJsonName() + "\":\"");
			DEBUG_PRINT(F("TROVATO"));
					
			start += (p[i]->getStrJsonName()).length() + 4;
			for(ends=start+1; ends < start + valueLen && srcJSONStr.charAt(ends)!='"' && ends < srcJSONStr.length(); ends++);
			app = srcJSONStr.substring(start, ends);
			
			DEBUG_PRINT(app);
			DEBUG_PRINT(F(" - start: "));
			DEBUG_PRINT(start);
			DEBUG_PRINT(F(" - end: "));
			DEBUG_PRINT(ends);
			DEBUG_PRINT(F(" - "));
			
			if(p[i-first]->getType() == 'j'){
				static_cast<ParByte*>(p[i-first])->loadFromStr(srcJSONStr.substring(start, ends)); 
				DEBUG_PRINT(F("val: "));
				DEBUG_PRINTLN(((ParByte*)p[i-first])->val);
			}else if(p[i-first]->getType() == 'p'){
				if(app[1] != delim){//default append
					p[i-first]->loadFromStr(app);
				}else{
					if(app[0] == 'a'){//append
						app1 = p[i-first]->getStrVal();
						app1 += " " + op + " "+ app.substring(2);
						p[i-first]->loadFromStr(app1);
					}else if(app[0] == 'w'){//overwrite
						p[i-first]->loadFromStr(app.substring(2));
					}else{//default append
						app1 = p[i-first]->getStrVal();
						app1 += " " + op + " "+ app.substring(2);
						p[i-first]->loadFromStr(app1);
						DEBUG_PRINT(F("\val: "));
						DEBUG_PRINTLN(((ParByte*)p[i])->val);
					}
				}
			}
			DEBUG_PRINT(F("- flagname: "));
			DEBUG_PRINT(p[i]->formname);	
			DEBUG_PRINT(F("- val received: "));
			DEBUG_PRINT(srcJSONStr.substring(start, ends));
			DEBUG_PRINT(F(" - start: "));
			DEBUG_PRINT(start);
			DEBUG_PRINT(F(" - end: "));
			DEBUG_PRINT(ends);
			DEBUG_PRINT(F(" - "));

			p[i-first]->doaction();
			
		}else{
			DEBUG_PRINT(F("- campo: "));
			DEBUG_PRINT("\""+p[i-first]->getStrJsonName() + "\":\"");
			DEBUG_PRINTLN(F(" non trovato o non necessario"));
		}
	}
}
/*
bool parseJsonFieldArrayToInt(String srcJSONStr, Par*p[], int valueLen, int arrLen, int first=0){
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
		start = srcJSONStr.indexOf("\""+ p[i]->getStrJsonName() + "\":\"");
		if(start > 0){
			count--;
			DEBUG_PRINT(F("- campo: "));
			DEBUG_PRINT("\"" + p[i]->getStrJsonName() + "\":\"");
			DEBUG_PRINTLN(F("TROVATO"));
			
			start += (p[i]->getStrJsonName()).length() + 4;
			for(ends=start+1; ends < start + valueLen && srcJSONStr.charAt(ends)!='"' && ends < srcJSONStr.length(); ends++);
			//calcola la fine del valore 
			//estrae il campo e lo converte in intero e aggiorna gli ingressi dello switchf X
			//p[i-first]->loadFromStr(srcJSONStr.substring(start, ends)); 
			static_cast<ParByte*>(p[i-first])->loadFromStr(srcJSONStr.substring(start, ends)); 
			DEBUG_PRINT(F("val: "));
			DEBUG_PRINTLN(((ParByte*)p[i-first])->val);
			/*if(destIntArr[i-first] == 0){ //se è 1 è sicuramente un num, ma se è 0 è un num o stringa?
				DEBUG_PRINT(F("\n:Stringa to num "));
				DEBUG_PRINTLN(srcJSONStr.charAt(start));
				if(srcJSONStr.charAt(start) != '0'){ // allora è una stringa
					destIntArr[i-first] = 1; //allora è un messaggio di configurazione, flag ON
				}
			}*/		
/*			DEBUG_PRINT(F("- flagname: "));
			DEBUG_PRINT(p[i]->formname);	
			p[i-first]->doaction();
			DEBUG_PRINT(F("- val received: "));
			DEBUG_PRINT(srcJSONStr.substring(start, ends));
			DEBUG_PRINT(F(" - start: "));
			DEBUG_PRINT(start);
			DEBUG_PRINT(F(" - end: "));
			DEBUG_PRINT(ends);
			DEBUG_PRINT(F(" - "));
		}else{
			DEBUG_PRINT(F("- campo: "));
			DEBUG_PRINT("\"" + p[i]->getStrJsonName() + "\":\"");
			DEBUG_PRINTLN(F(" non trovato o non necessario"));
		}
	}  
	return (count > 0);
}
*/
