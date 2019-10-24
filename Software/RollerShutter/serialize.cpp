#include "serialize.h"


void parseJsonFieldArrayToStr(String srcJSONStr, Par*pa[], int valueLen, int arrLen, int first, char delim, String op){
	int start, ends=0;
	short count = 0;
	
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
	
	//check deviceid if exists
	start = srcJSONStr.indexOf("\""+pa[p(MQTTID)]->getStrJsonName() + "\":\"");
	if(start > 0){
		DEBUG2_PRINT(F("- campo: "));
		DEBUG2_PRINT("\""+pa[p(MQTTID)]->getStrJsonName() + "\":\"");
		DEBUG2_PRINT(F("TROVATO"));
				
		start += (pa[p(MQTTID)]->getStrJsonName()).length() + 4;
		for(ends=start+1; ends < start + valueLen && srcJSONStr.charAt(ends)!='"' && ends < srcJSONStr.length(); ends++);	
		
		String rcvdid = srcJSONStr.substring(start, ends); 
		
		//if received deviceid equals local deviceid		
		if(strcmp(static_cast<ParStr32*>(pa[p(MQTTID)])->val, rcvdid.c_str())!=0 && strcmp(rcvdid.c_str(), BROADCASTID)!=0){
			count = -1;
		}else{
			count--;
		}
		
	}else{
		count--;
		DEBUG2_PRINT(F("- campo: "));
		DEBUG2_PRINT("\""+pa[p(MQTTID)]->getStrJsonName() + "\":\"");
		DEBUG2_PRINTLN(F(" non trovato o non necessario"));
	}
	
	DEBUG1_PRINT(F("- count: "));
	DEBUG1_PRINTLN(count);
	//load all other fields
	for(int i=first; i<arrLen && count > 0; i++){
		///calcola l'inizio del valore
		start = srcJSONStr.indexOf("\""+ pa[i]->getStrJsonName() + "\":\"");
		if(start > 0){
			count--;
			DEBUG2_PRINT(F("- campo: "));
			DEBUG2_PRINT("\""+pa[i]->getStrJsonName() + "\":\"");
			DEBUG2_PRINT(F("TROVATO"));
					
			start += (pa[i]->getStrJsonName()).length() + 4;
			for(ends=start+1; ends < start + valueLen && srcJSONStr.charAt(ends)!='"' && ends < srcJSONStr.length(); ends++);
			app = srcJSONStr.substring(start, ends);
			
			DEBUG2_PRINT(app);
			DEBUG2_PRINT(F(" - start: "));
			DEBUG2_PRINT(start);
			DEBUG2_PRINT(F(" - end: "));
			DEBUG2_PRINT(ends);
			DEBUG2_PRINT(F(" - "));
			
			if(pa[i-first]->getType() == 'j'){
				static_cast<ParUint8*>(pa[i-first])->loadFromStr(srcJSONStr.substring(start, ends)); 
				DEBUG2_PRINT(F("val: "));
				DEBUG2_PRINTLN(((ParUint8*)pa[i-first])->val);
			}else if(pa[i-first]->getType() == 'p'){
				if(app[1] != delim){//default append
					pa[i-first]->loadFromStr(app);
				}else{
					if(app[0] == 'a'){//append
						app1 = pa[i-first]->getStrVal();
						app1 += " " + op + " "+ app.substring(2);
						pa[i-first]->loadFromStr(app1);
					}else if(app[0] == 'w'){//overwrite
						pa[i-first]->loadFromStr(app.substring(2));
					}else{//default append
						app1 = pa[i-first]->getStrVal();
						app1 += " " + op + " "+ app.substring(2);
						pa[i-first]->loadFromStr(app1);
						DEBUG2_PRINT(F("\val: "));
						DEBUG2_PRINTLN(((ParUint8*)pa[i])->val);
					}
				}
			}
			DEBUG2_PRINT(F("- flagname: "));
			DEBUG2_PRINT(pa[i]->formname);	
			DEBUG2_PRINT(F("- val received: "));
			DEBUG2_PRINT(srcJSONStr.substring(start, ends));
			DEBUG2_PRINT(F(" - start: "));
			DEBUG2_PRINT(start);
			DEBUG2_PRINT(F(" - end: "));
			DEBUG2_PRINT(ends);
			DEBUG2_PRINT(F(" - "));

			pa[i-first]->doaction(1);
			
		}else{
			DEBUG2_PRINT(F("- campo: "));
			DEBUG2_PRINT("\""+pa[i-first]->getStrJsonName() + "\":\"");
			DEBUG2_PRINTLN(F(" non trovato o non necessario"));
		}
	}
}
/*
bool parseJsonFieldArrayToInt(String srcJSONStr, Par*pa[], int valueLen, int arrLen, int first=0){
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
		start = srcJSONStr.indexOf("\""+ pa[i]->getStrJsonName() + "\":\"");
		if(start > 0){
			count--;
			DEBUG2_PRINT(F("- campo: "));
			DEBUG2_PRINT("\"" + pa[i]->getStrJsonName() + "\":\"");
			DEBUG2_PRINTLN(F("TROVATO"));
			
			start += (pa[i]->getStrJsonName()).length() + 4;
			for(ends=start+1; ends < start + valueLen && srcJSONStr.charAt(ends)!='"' && ends < srcJSONStr.length(); ends++);
			//calcola la fine del valore 
			//estrae il campo e lo converte in intero e aggiorna gli ingressi dello switchf X
			//pa[i-first]->loadFromStr(srcJSONStr.substring(start, ends)); 
			static_cast<ParUint8*>(pa[i-first])->loadFromStr(srcJSONStr.substring(start, ends)); 
			DEBUG2_PRINT(F("val: "));
			DEBUG2_PRINTLN(((ParUint8*)pa[i-first])->val);
			/*if(destIntArr[i-first] == 0){ //se è 1 è sicuramente un num, ma se è 0 è un num o stringa?
				DEBUG2_PRINT(F("\n:Stringa to num "));
				DEBUG2_PRINTLN(srcJSONStr.charAt(start));
				if(srcJSONStr.charAt(start) != '0'){ // allora è una stringa
					destIntArr[i-first] = 1; //allora è un messaggio di configurazione, flag ON
				}
			}*/		
/*			DEBUG2_PRINT(F("- flagname: "));
			DEBUG2_PRINT(pa[i]->formname);	
			pa[i-first]->doaction();
			DEBUG2_PRINT(F("- val received: "));
			DEBUG2_PRINT(srcJSONStr.substring(start, ends));
			DEBUG2_PRINT(F(" - start: "));
			DEBUG2_PRINT(start);
			DEBUG2_PRINT(F(" - end: "));
			DEBUG2_PRINT(ends);
			DEBUG2_PRINT(F(" - "));
		}else{
			DEBUG2_PRINT(F("- campo: "));
			DEBUG2_PRINT("\"" + pa[i]->getStrJsonName() + "\":\"");
			DEBUG2_PRINTLN(F(" non trovato o non necessario"));
		}
	}  
	return (count > 0);
}
*/
