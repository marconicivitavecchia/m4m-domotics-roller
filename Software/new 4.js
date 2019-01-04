//potenze in ordine crescente
var doff = msg.payload.doff;
var onid = msg.payload.deviceid;
var p = msg.payload.priority;
var delta = msg.payload.delta;
var info = msg.payload.info;
var s = msg.payload.s;

if(info[0]===null && s <delta){
    for(k=0;  k<info.length && info[k].devicewatt < delta; k++);
    for(j=k; j>=0 && s < delta; j--){
    	s+=info[j].devicewatt;
    	doff.push(info[j]);
    }
}
var newMsg = {};
if(s>delta){
	newMsg.payload = {deviceid:onid, offlist: doff, okon:true, s:s, delta:delta, priority:p};
}else{
	//acceso KO;
	delta=delta-s;
	newMsg.payload = {deviceid:onid, offlistdoff: doff, okon:false, s:s, delta:delta, priority:p};
}
return newMsg;