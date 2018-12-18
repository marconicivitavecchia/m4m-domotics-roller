//potenze in ordine crescente
for(i=5, s=0; i>p && s<delta; i--){
	for(k=0; k<v.length && v[k]<delta; k++);
	for(j=k, s=0; j>=0 && s<delta; j--){
		//spegni v[j];
		s+=v[j];
	}
	if(s<delta){
		delta=delta-s;
		i++;
		//carica lista a priorità maggiore
	}
}
if(s>delta){
	//acceso OK;
}else{
	//acceso KO;
}
//---------------------------------------------------------------
for(i=5, s=0; i<p && s<delta; i++){
	for(j=v.length, s=0; j>=0 && s<delta; j++){
			spegni v[j];
			s+=v[j];
		}
}

if((s>delta){
	acceso OK;
}else{
	acceso KO;
}
//------------------------------------------
var myArray = flow.get('myArray');
if (!myArray) {
    myArray = [];
}

// do something with myArray

// save it back to context
flow.set('myArray',myArray);
//--------------------------------
var count = context.get('count') || 0;
count++;
context.set('count',count);
//-------------------------------