#include "serialize.h"


void parseJsonFieldArrayToStr(String srcJSONStr, Par*p[], int valueLen, int arrLen, int first, char delim, String op){
	int start, ends=0;
	byte count = 0;
	
	valueLen+=3;
	if(first < 0 || first > arrLen)
		first=0;
	String app, app1;
	
	DEBUG2_PRINT(F("\nParser configurazioni, MSG: "));
	DEBUG2_PRINTLN(srcJSONStr);
	
	//field counter
	for(ends = 0; ends < srcJSONStr.length(); ends++)
		if(srcJSONStr.charAt(ends) == ',') count++;
	count++;
	
	for(int i=first; i<arrLen && count > 0; i++){
		///calcola l'inizio del valore
		start = srcJSONStr.indexOf("\""+ p[i]->getStrJsonName() + "\":\"");
		if(start > 0){
			count--;
			DEBUG2_PRINT(F("- campo: "));
			DEBUG2_PRINT("\""+p[i]->getStrJsonName() + "\":\"");
			DEBUG2_PRINT(F("TROVATO"));
					
			start += (p[i]->getStrJsonName()).length() + 4;
			for(ends=start+1; ends < start + valueLen && srcJSONStr.charAt(ends)!='"' && ends < srcJSONStr.length(); ends++);
			app = srcJSONStr.substring(start, ends);
			
			DEBUG2_PRINT(app);
			DEBUG2_PRINT(F(" - start: "));
			DEBUG2_PRINT(start);
			DEBUG2_PRINT(F(" - end: "));
			DEBUG2_PRINT(ends);
			DEBUG2_PRINT(F(" - "));
			
			if(p[i-first]->getType() == 'j'){
				static_cast<ParUint8*>(p[i-first])->loadFromStr(srcJSONStr.substring(start, ends)); 
				DEBUG2_PRINT(F("val: "));
				DEBUG2_PRINTLN(((ParUint8*)p[i-first])->val);
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
						DEBUG2_PRINT(F("\val: "));
						DEBUG2_PRINTLN(((ParUint8*)p[i])->val);
					}
				}
			}
			DEBUG2_PRINT(F("- flagname: "));
			DEBUG2_PRINT(p[i]->formname);	
			DEBUG2_PRINT(F("- val received: "));
			DEBUG2_PRINT(srcJSONStr.substring(start, ends));
			DEBUG2_PRINT(F(" - start: "));
			DEBUG2_PRINT(start);
			DEBUG2_PRINT(F(" - end: "));
			DEBUG2_PRINT(ends);
			DEBUG2_PRINT(F(" - "));

			p[i-first]->doaction(true);
			
		}else{
			DEBUG2_PRINT(F("- campo: "));
			DEBUG2_PRINT("\""+p[i-first]->getStrJsonName() + "\":\"");
			DEBUG2_PRINTLN(F(" non trovato o non necessario"));
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

	DEBUG2_PRINT(F("\nParser comandi:, MSG: "));
	DEBUG2_PRINTLN(srcJSONStr);
	
	//field counter
	for(ends = 0; ends < srcJSONStr.length(); ends++)
		if(srcJSONStr.charAt(ends) == ',') count++;
	count++;

	for(int i=first; i<arrLen && count > 0; i++){
		start = srcJSONStr.indexOf("\""+ p[i]->getStrJsonName() + "\":\"");
		if(start > 0){
			count--;
			DEBUG2_PRINT(F("- campo: "));
			DEBUG2_PRINT("\"" + p[i]->getStrJsonName() + "\":\"");
			DEBUG2_PRINTLN(F("TROVATO"));
			
			start += (p[i]->getStrJsonName()).length() + 4;
			for(ends=start+1; ends < start + valueLen && srcJSONStr.charAt(ends)!='"' && ends < srcJSONStr.length(); ends++);
			//calcola la fine del valore 
			//estrae il campo e lo converte in intero e aggiorna gli ingressi dello switchf X
			//p[i-first]->loadFromStr(srcJSONStr.substring(start, ends)); 
			static_cast<ParUint8*>(p[i-first])->loadFromStr(srcJSONStr.substring(start, ends)); 
			DEBUG2_PRINT(F("val: "));
			DEBUG2_PRINTLN(((ParUint8*)p[i-first])->val);
			/*if(destIntArr[i-first] == 0){ //se è 1 è sicuramente un num, ma se è 0 è un num o stringa?
				DEBUG2_PRINT(F("\n:Stringa to num "));
				DEBUG2_PRINTLN(srcJSONStr.charAt(start));
				if(srcJSONStr.charAt(start) != '0'){ // allora è una stringa
					destIntArr[i-first] = 1; //allora è un messaggio di configurazione, flag ON
				}
			}*/		
/*			DEBUG2_PRINT(F("- flagname: "));
			DEBUG2_PRINT(p[i]->formname);	
			p[i-first]->doaction();
			DEBUG2_PRINT(F("- val received: "));
			DEBUG2_PRINT(srcJSONStr.substring(start, ends));
			DEBUG2_PRINT(F(" - start: "));
			DEBUG2_PRINT(start);
			DEBUG2_PRINT(F(" - end: "));
			DEBUG2_PRINT(ends);
			DEBUG2_PRINT(F(" - "));
		}else{
			DEBUG2_PRINT(F("- campo: "));
			DEBUG2_PRINT("\"" + p[i]->getStrJsonName() + "\":\"");
			DEBUG2_PRINTLN(F(" non trovato o non necessario"));
		}
	}  
	return (count > 0);
}
*/
