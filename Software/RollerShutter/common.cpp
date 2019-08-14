#include "common.h"
#define serverp (*serveru)

ESP8266WebServer *serveru; 
String  *mqttJsonp;
String  *confJsonp;
String  *confcmdp;

Par **parsp;

int EEaddress = FIXEDPARAMSLEN;
int varStrOfst[VARCONFDIM+1];
//bool rollmode[2] = {true, true};

inline void eepromBegin(){
	int i, len;
	for(i=0,len=0;i<VARCONFDIM;++i){
		len += confcmdp[i].length();
	}
	EEPROM.begin(FIXEDPARAMSLEN + len + i);
}

void initCommon(ESP8266WebServer *serveri, Par **parsi, String  *mqttJsoni, String  *confJsoni, String  *confcmdi){
	serveru=serveri; 
	mqttJsonp=mqttJsoni;
	confJsonp=confJsoni;
	confcmdp=confcmdi;
	parsp=parsi;
	for(int i=0; i<VARCONFDIM+1; ++i){
		varStrOfst[i] = 0;
	}
}


/*******************************************************************************
 * Copyright (c) 2013, 2016 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 *******************************************************************************/
 
/*const char MQTT_CLIENT[] PROGMEM =
"<script>(function(p,s){'object'===typeof exports&&'object'===typeof module?module.exports=s():'function'===typeof define&&define.amd?define(s):'object'===typeof exports?exports=s():('undefined'===typeof p.Paho&&(p.Paho={}),p.Paho.MQTT=s())})(this,function(){return function(p){function s(a,b,c){b[c++]=a>>8;b[c++]=a%256;return c}function u(a,b,c,k){k=s(b,c,k);D(a,c,k);return k+b}function n(a){for(var b=0,c=0;c<a.length;c++){var k=a.charCodeAt(c);2047<k?(55296<=k&&56319>=k&&(c++,b++),b+=3):127<k?b+=2:b++}return b}"
"function D(a,b,c){for(var k=0;k<a.length;k++){var e=a.charCodeAt(k);if(55296<=e&&56319>=e){var g=a.charCodeAt(++k);if(isNaN(g))throw Error(f(h.MALFORMED_UNICODE,[e,g]));e=(e-55296<<10)+(g-56320)+65536}127>=e?b[c++]=e:(2047>=e?b[c++]=e>>6&31|192:(65535>=e?b[c++]=e>>12&15|224:(b[c++]=e>>18&7|240,b[c++]=e>>12&63|128),b[c++]=e>>6&63|128),b[c++]=e&63|128)}return b}function E(a,b,c){for(var k='',e,g=b;g<b+c;){e=a[g++];if(!(128>e)){var m=a[g++]-128;if(0>m)throw Error(f(h.MALFORMED_UTF,[e.toString(16),m.toString(16),"
"'']));if(224>e)e=64*(e-192)+m;else{var d=a[g++]-128;if(0>d)throw Error(f(h.MALFORMED_UTF,[e.toString(16),m.toString(16),d.toString(16)]));if(240>e)e=4096*(e-224)+64*m+d;else{var l=a[g++]-128;if(0>l)throw Error(f(h.MALFORMED_UTF,[e.toString(16),m.toString(16),d.toString(16),l.toString(16)]));if(248>e)e=262144*(e-240)+4096*m+64*d+l;else throw Error(f(h.MALFORMED_UTF,[e.toString(16),m.toString(16),d.toString(16),l.toString(16)]));}}}65535<e&&(e-=65536,k+=String.fromCharCode(55296+(e>>10)),e=56320+(e&"
"1023));k+=String.fromCharCode(e)}return k}var z=function(a,b){for(var c in a)if(a.hasOwnProperty(c))if(b.hasOwnProperty(c)){if(typeof a[c]!==b[c])throw Error(f(h.INVALID_TYPE,[typeof a[c],c]));}else{c='Unknown property, '+c+'. Valid properties are:';for(var k in b)b.hasOwnProperty(k)&&(c=c+' '+k);throw Error(c);}},v=function(a,b){return function(){return a.apply(b,arguments)}},h={OK:{code:0,text:'AMQJSC0000I OK.'},CONNECT_TIMEOUT:{code:1,text:'AMQJSC0001E Connect timed out.'},SUBSCRIBE_TIMEOUT:{code:2,"
"text:'AMQJS0002E Subscribe timed out.'},UNSUBSCRIBE_TIMEOUT:{code:3,text:'AMQJS0003E Unsubscribe timed out.'},PING_TIMEOUT:{code:4,text:'AMQJS0004E Ping timed out.'},INTERNAL_ERROR:{code:5,text:'AMQJS0005E Internal error. Error Message: {0}, Stack trace: {1}'},CONNACK_RETURNCODE:{code:6,text:'AMQJS0006E Bad Connack return code:{0} {1}.'},SOCKET_ERROR:{code:7,text:'AMQJS0007E Socket error:{0}.'},SOCKET_CLOSE:{code:8,text:'AMQJS0008I Socket closed.'},MALFORMED_UTF:{code:9,text:'AMQJS0009E Malformed UTF data:{0} {1} {2}.'},"
"UNSUPPORTED:{code:10,text:'AMQJS0010E {0} is not supported by this browser.'},INVALID_STATE:{code:11,text:'AMQJS0011E Invalid state {0}.'},INVALID_TYPE:{code:12,text:'AMQJS0012E Invalid type {0} for {1}.'},INVALID_ARGUMENT:{code:13,text:'AMQJS0013E Invalid argument {0} for {1}.'},UNSUPPORTED_OPERATION:{code:14,text:'AMQJS0014E Unsupported operation.'},INVALID_STORED_DATA:{code:15,text:'AMQJS0015E Invalid data in local storage key\x3d{0} value\x3d{1}.'},INVALID_MQTT_MESSAGE_TYPE:{code:16,text:'AMQJS0016E Invalid MQTT message type {0}.'},"
"MALFORMED_UNICODE:{code:17,text:'AMQJS0017E Malformed Unicode string:{0} {1}.'},BUFFER_FULL:{code:18,text:'AMQJS0018E Message buffer is full, maximum buffer size: {0}.'}},H={0:'Connection Accepted',1:'Connection Refused: unacceptable protocol version',2:'Connection Refused: identifier rejected',3:'Connection Refused: server unavailable',4:'Connection Refused: bad user name or password',5:'Connection Refused: not authorized'},f=function(a,b){var c=a.text;if(b)for(var k,e,g=0;g<b.length;g++)if(k='{'+"
"g+'}',e=c.indexOf(k),0<e)var h=c.substring(0,e),c=c.substring(e+k.length),c=h+b[g]+c;return c},A=[0,6,77,81,73,115,100,112,3],B=[0,4,77,81,84,84,4],q=function(a,b){this.type=a;for(var c in b)b.hasOwnProperty(c)&&(this[c]=b[c])};q.prototype.encode=function(){var a=(this.type&15)<<4,b=0,c=[],k=0,e;void 0!==this.messageIdentifier&&(b+=2);switch(this.type){case 1:switch(this.mqttVersion){case 3:b+=A.length+3;break;case 4:b+=B.length+3}b+=n(this.clientId)+2;void 0!==this.willMessage&&(b+=n(this.willMessage.destinationName)+"
"2,e=this.willMessage.payloadBytes,e instanceof Uint8Array||(e=new Uint8Array(h)),b+=e.byteLength+2);void 0!==this.userName&&(b+=n(this.userName)+2);void 0!==this.password&&(b+=n(this.password)+2);break;case 8:for(var a=a|2,g=0;g<this.topics.length;g++)c[g]=n(this.topics[g]),b+=c[g]+2;b+=this.requestedQos.length;break;case 10:a|=2;for(g=0;g<this.topics.length;g++)c[g]=n(this.topics[g]),b+=c[g]+2;break;case 6:a|=2;break;case 3:this.payloadMessage.duplicate&&(a|=8);a=a|=this.payloadMessage.qos<<1;this.payloadMessage.retained&&"
"(a|=1);var k=n(this.payloadMessage.destinationName),h=this.payloadMessage.payloadBytes,b=b+(k+2)+h.byteLength;h instanceof ArrayBuffer?h=new Uint8Array(h):h instanceof Uint8Array||(h=new Uint8Array(h.buffer))}var f=b,g=Array(1),d=0;do{var t=f%128,f=f>>7;0<f&&(t|=128);g[d++]=t}while(0<f&&4>d);f=g.length+1;b=new ArrayBuffer(b+f);d=new Uint8Array(b);d[0]=a;d.set(g,1);if(3==this.type)f=u(this.payloadMessage.destinationName,k,d,f);else if(1==this.type){switch(this.mqttVersion){case 3:d.set(A,f);f+=A.length;"
"break;case 4:d.set(B,f),f+=B.length}a=0;this.cleanSession&&(a=2);void 0!==this.willMessage&&(a=a|4|this.willMessage.qos<<3,this.willMessage.retained&&(a|=32));void 0!==this.userName&&(a|=128);void 0!==this.password&&(a|=64);d[f++]=a;f=s(this.keepAliveInterval,d,f)}void 0!==this.messageIdentifier&&(f=s(this.messageIdentifier,d,f));switch(this.type){case 1:f=u(this.clientId,n(this.clientId),d,f);void 0!==this.willMessage&&(f=u(this.willMessage.destinationName,n(this.willMessage.destinationName),d,f),"
"f=s(e.byteLength,d,f),d.set(e,f),f+=e.byteLength);void 0!==this.userName&&(f=u(this.userName,n(this.userName),d,f));void 0!==this.password&&u(this.password,n(this.password),d,f);break;case 3:d.set(h,f);break;case 8:for(g=0;g<this.topics.length;g++)f=u(this.topics[g],c[g],d,f),d[f++]=this.requestedQos[g];break;case 10:for(g=0;g<this.topics.length;g++)f=u(this.topics[g],c[g],d,f)}return b};var F=function(a,b,c){this._client=a;this._window=b;this._keepAliveInterval=1E3*c;this.isReset=!1;var k=(new q(12)).encode(),"
"e=function(a){return function(){return g.apply(a)}},g=function(){this.isReset?(this.isReset=!1,this._client._trace('Pinger.doPing','send PINGREQ'),this._client.socket.send(k),this.timeout=this._window.setTimeout(e(this),this._keepAliveInterval)):(this._client._trace('Pinger.doPing','Timed out'),this._client._disconnected(h.PING_TIMEOUT.code,f(h.PING_TIMEOUT)))};this.reset=function(){this.isReset=!0;this._window.clearTimeout(this.timeout);0<this._keepAliveInterval&&(this.timeout=setTimeout(e(this),"
"this._keepAliveInterval))};this.cancel=function(){this._window.clearTimeout(this.timeout)}},w=function(a,b,c,f,e){this._window=b;c||(c=30);this.timeout=setTimeout(function(a,b,c){return function(){return a.apply(b,c)}}(f,a,e),1E3*c);this.cancel=function(){this._window.clearTimeout(this.timeout)}},d=function(a,b,c,d,e){if(!('WebSocket'in p&&null!==p.WebSocket))throw Error(f(h.UNSUPPORTED,['WebSocket']));if(!('localStorage'in p&&null!==p.localStorage))throw Error(f(h.UNSUPPORTED,['localStorage']));"
"if(!('ArrayBuffer'in p&&null!==p.ArrayBuffer))throw Error(f(h.UNSUPPORTED,['ArrayBuffer']));this._trace('Paho.MQTT.Client',a,b,c,d,e);this.host=b;this.port=c;this.path=d;this.uri=a;this.clientId=e;this._wsuri=null;this._localKey=b+':'+c+('/mqtt'!=d?':'+d:'')+':'+e+':';this._msg_queue=[];this._buffered_msg_queue=[];this._sentMessages={};this._receivedMessages={};this._notify_msg_sent={};this._message_identifier=1;this._sequence=0;for(var g in localStorage)0!==g.indexOf('Sent:'+this._localKey)&&0!=="
"g.indexOf('Received:'+this._localKey)||this.restore(g)};d.prototype.host=null;d.prototype.port=null;d.prototype.path=null;d.prototype.uri=null;d.prototype.clientId=null;d.prototype.socket=null;d.prototype.connected=!1;d.prototype.maxMessageIdentifier=65536;d.prototype.connectOptions=null;d.prototype.hostIndex=null;d.prototype.onConnected=null;d.prototype.onConnectionLost=null;d.prototype.onMessageDelivered=null;d.prototype.onMessageArrived=null;d.prototype.traceFunction=null;d.prototype._msg_queue="
"null;d.prototype._buffered_msg_queue=null;d.prototype._connectTimeout=null;d.prototype.sendPinger=null;d.prototype.receivePinger=null;d.prototype._reconnectInterval=1;d.prototype._reconnecting=!1;d.prototype._reconnectTimeout=null;d.prototype.disconnectedPublishing=!1;d.prototype.disconnectedBufferSize=5E3;d.prototype.receiveBuffer=null;d.prototype._traceBuffer=null;d.prototype._MAX_TRACE_ENTRIES=100;d.prototype.connect=function(a){var b=this._traceMask(a,'password');this._trace('Client.connect',"
"b,this.socket,this.connected);if(this.connected)throw Error(f(h.INVALID_STATE,['already connected']));if(this.socket)throw Error(f(h.INVALID_STATE,['already connected']));this._reconnecting&&(this._reconnectTimeout.cancel(),this._reconnectTimeout=null,this._reconnecting=!1);this.connectOptions=a;this._reconnectInterval=1;this._reconnecting=!1;a.uris?(this.hostIndex=0,this._doConnect(a.uris[0])):this._doConnect(this.uri)};d.prototype.subscribe=function(a,b){this._trace('Client.subscribe',a,b);if(!this.connected)throw Error(f(h.INVALID_STATE,"
"['not connected']));var c=new q(8);c.topics=[a];c.requestedQos=void 0!==b.qos?[b.qos]:[0];b.onSuccess&&(c.onSuccess=function(a){b.onSuccess({invocationContext:b.invocationContext,grantedQos:a})});b.onFailure&&(c.onFailure=function(a){b.onFailure({invocationContext:b.invocationContext,errorCode:a,errorMessage:f(a)})});b.timeout&&(c.timeOut=new w(this,window,b.timeout,b.onFailure,[{invocationContext:b.invocationContext,errorCode:h.SUBSCRIBE_TIMEOUT.code,errorMessage:f(h.SUBSCRIBE_TIMEOUT)}]));this._requires_ack(c);"
"this._schedule_message(c)};d.prototype.unsubscribe=function(a,b){this._trace('Client.unsubscribe',a,b);if(!this.connected)throw Error(f(h.INVALID_STATE,['not connected']));var c=new q(10);c.topics=[a];b.onSuccess&&(c.callback=function(){b.onSuccess({invocationContext:b.invocationContext})});b.timeout&&(c.timeOut=new w(this,window,b.timeout,b.onFailure,[{invocationContext:b.invocationContext,errorCode:h.UNSUBSCRIBE_TIMEOUT.code,errorMessage:f(h.UNSUBSCRIBE_TIMEOUT)}]));this._requires_ack(c);this._schedule_message(c)};"
"d.prototype.send=function(a){this._trace('Client.send',a);wireMessage=new q(3);wireMessage.payloadMessage=a;if(this.connected)0<a.qos?this._requires_ack(wireMessage):this.onMessageDelivered&&(this._notify_msg_sent[wireMessage]=this.onMessageDelivered(wireMessage.payloadMessage)),this._schedule_message(wireMessage);else if(this._reconnecting&&this.disconnectedPublishing){if(Object.keys(this._sentMessages).length+this._buffered_msg_queue.length>this.disconnectedBufferSize)throw Error(f(h.BUFFER_FULL,"
"[this.disconnectedBufferSize]));0<a.qos?this._requires_ack(wireMessage):(wireMessage.sequence=++this._sequence,this._buffered_msg_queue.push(wireMessage))}else throw Error(f(h.INVALID_STATE,['not connected']));};d.prototype.disconnect=function(){this._trace('Client.disconnect');this._reconnecting&&(this._reconnectTimeout.cancel(),this._reconnectTimeout=null,this._reconnecting=!1);if(!this.socket)throw Error(f(h.INVALID_STATE,['not connecting or connected']));wireMessage=new q(14);this._notify_msg_sent[wireMessage]="
"v(this._disconnected,this);this._schedule_message(wireMessage)};d.prototype.getTraceLog=function(){if(null!==this._traceBuffer){this._trace('Client.getTraceLog',new Date);this._trace('Client.getTraceLog in flight messages',this._sentMessages.length);for(var a in this._sentMessages)this._trace('_sentMessages ',a,this._sentMessages[a]);for(a in this._receivedMessages)this._trace('_receivedMessages ',a,this._receivedMessages[a]);return this._traceBuffer}};d.prototype.startTrace=function(){null===this._traceBuffer&&"
"(this._traceBuffer=[]);this._trace('Client.startTrace',new Date,'1.0.3')};d.prototype.stopTrace=function(){delete this._traceBuffer};d.prototype._doConnect=function(a){this.connectOptions.useSSL&&(a=a.split(':'),a[0]='wss',a=a.join(':'));this._wsuri=a;this.connected=!1;this.socket=4>this.connectOptions.mqttVersion?new WebSocket(a,['mqttv3.1']):new WebSocket(a,['mqtt']);this.socket.binaryType='arraybuffer';this.socket.onopen=v(this._on_socket_open,this);this.socket.onmessage=v(this._on_socket_message,"
"this);this.socket.onerror=v(this._on_socket_error,this);this.socket.onclose=v(this._on_socket_close,this);this.sendPinger=new F(this,window,this.connectOptions.keepAliveInterval);this.receivePinger=new F(this,window,this.connectOptions.keepAliveInterval);this._connectTimeout&&(this._connectTimeout.cancel(),this._connectTimeout=null);this._connectTimeout=new w(this,window,this.connectOptions.timeout,this._disconnected,[h.CONNECT_TIMEOUT.code,f(h.CONNECT_TIMEOUT)])};d.prototype._schedule_message=function(a){this._msg_queue.push(a);"
"this.connected&&this._process_queue()};d.prototype.store=function(a,b){var c={type:b.type,messageIdentifier:b.messageIdentifier,version:1};switch(b.type){case 3:b.pubRecReceived&&(c.pubRecReceived=!0);c.payloadMessage={};for(var d='',e=b.payloadMessage.payloadBytes,g=0;g<e.length;g++)d=15>=e[g]?d+'0'+e[g].toString(16):d+e[g].toString(16);c.payloadMessage.payloadHex=d;c.payloadMessage.qos=b.payloadMessage.qos;c.payloadMessage.destinationName=b.payloadMessage.destinationName;b.payloadMessage.duplicate&&"
"(c.payloadMessage.duplicate=!0);b.payloadMessage.retained&&(c.payloadMessage.retained=!0);0===a.indexOf('Sent:')&&(void 0===b.sequence&&(b.sequence=++this._sequence),c.sequence=b.sequence);break;default:throw Error(f(h.INVALID_STORED_DATA,[key,c]));}localStorage.setItem(a+this._localKey+b.messageIdentifier,JSON.stringify(c))};d.prototype.restore=function(a){var b=localStorage.getItem(a),c=JSON.parse(b),d=new q(c.type,c);switch(c.type){case 3:for(var b=c.payloadMessage.payloadHex,e=new ArrayBuffer(b.length/"
"2),e=new Uint8Array(e),g=0;2<=b.length;){var m=parseInt(b.substring(0,2),16),b=b.substring(2,b.length);e[g++]=m}b=new Paho.MQTT.Message(e);b.qos=c.payloadMessage.qos;b.destinationName=c.payloadMessage.destinationName;c.payloadMessage.duplicate&&(b.duplicate=!0);c.payloadMessage.retained&&(b.retained=!0);d.payloadMessage=b;break;default:throw Error(f(h.INVALID_STORED_DATA,[a,b]));}0===a.indexOf('Sent:'+this._localKey)?(d.payloadMessage.duplicate=!0,this._sentMessages[d.messageIdentifier]=d):0===a.indexOf('Received:'+"
"this._localKey)&&(this._receivedMessages[d.messageIdentifier]=d)};d.prototype._process_queue=function(){for(var a=null,b=this._msg_queue.reverse();a=b.pop();)this._socket_send(a),this._notify_msg_sent[a]&&(this._notify_msg_sent[a](),delete this._notify_msg_sent[a])};d.prototype._requires_ack=function(a){var b=Object.keys(this._sentMessages).length;if(b>this.maxMessageIdentifier)throw Error('Too many messages:'+b);for(;void 0!==this._sentMessages[this._message_identifier];)this._message_identifier++;"
"a.messageIdentifier=this._message_identifier;this._sentMessages[a.messageIdentifier]=a;3===a.type&&this.store('Sent:',a);this._message_identifier===this.maxMessageIdentifier&&(this._message_identifier=1)};d.prototype._on_socket_open=function(){var a=new q(1,this.connectOptions);a.clientId=this.clientId;this._socket_send(a)};d.prototype._on_socket_message=function(a){this._trace('Client._on_socket_message',a.data);a=this._deframeMessages(a.data);for(var b=0;b<a.length;b+=1)this._handleMessage(a[b])};"
"d.prototype._deframeMessages=function(a){a=new Uint8Array(a);var b=[];if(this.receiveBuffer){var c=new Uint8Array(this.receiveBuffer.length+a.length);c.set(this.receiveBuffer);c.set(a,this.receiveBuffer.length);a=c;delete this.receiveBuffer}try{for(c=0;c<a.length;){var d;a:{var e=a,g=c,m=g,n=e[g],l=n>>4,t=n&15,g=g+1,x=void 0,C=0,p=1;do{if(g==e.length){d=[null,m];break a}x=e[g++];C+=(x&127)*p;p*=128}while(0!==(x&128));x=g+C;if(x>e.length)d=[null,m];else{var y=new q(l);switch(l){case 2:e[g++]&1&&(y.sessionPresent="
"!0);y.returnCode=e[g++];break;case 3:var m=t>>1&3,s=256*e[g]+e[g+1],g=g+2,u=E(e,g,s),g=g+s;0<m&&(y.messageIdentifier=256*e[g]+e[g+1],g+=2);var r=new Paho.MQTT.Message(e.subarray(g,x));1==(t&1)&&(r.retained=!0);8==(t&8)&&(r.duplicate=!0);r.qos=m;r.destinationName=u;y.payloadMessage=r;break;case 4:case 5:case 6:case 7:case 11:y.messageIdentifier=256*e[g]+e[g+1];break;case 9:y.messageIdentifier=256*e[g]+e[g+1],g+=2,y.returnCode=e.subarray(g,x)}d=[y,x]}}var v=d[0],c=d[1];if(null!==v)b.push(v);else break}c<"
"a.length&&(this.receiveBuffer=a.subarray(c))}catch(w){d='undefined'==w.hasOwnProperty('stack')?w.stack.toString():'No Error Stack Available';this._disconnected(h.INTERNAL_ERROR.code,f(h.INTERNAL_ERROR,[w.message,d]));return}return b};d.prototype._handleMessage=function(a){this._trace('Client._handleMessage',a);try{switch(a.type){case 2:this._connectTimeout.cancel();this._reconnectTimeout&&this._reconnectTimeout.cancel();if(this.connectOptions.cleanSession){for(var b in this._sentMessages){var c=this._sentMessages[b];"
"localStorage.removeItem('Sent:'+this._localKey+c.messageIdentifier)}this._sentMessages={};for(b in this._receivedMessages){var d=this._receivedMessages[b];localStorage.removeItem('Received:'+this._localKey+d.messageIdentifier)}this._receivedMessages={}}if(0===a.returnCode)this.connected=!0,this.connectOptions.uris&&(this.hostIndex=this.connectOptions.uris.length);else{this._disconnected(h.CONNACK_RETURNCODE.code,f(h.CONNACK_RETURNCODE,[a.returnCode,H[a.returnCode]]));break}a=[];for(var e in this._sentMessages)this._sentMessages.hasOwnProperty(e)&&"
"a.push(this._sentMessages[e]);if(0<this._buffered_msg_queue.length){e=null;for(var g=this._buffered_msg_queue.reverse();e=g.pop();)a.push(e),this.onMessageDelivered&&(this._notify_msg_sent[e]=this.onMessageDelivered(e.payloadMessage))}a=a.sort(function(a,b){return a.sequence-b.sequence});for(var g=0,m=a.length;g<m;g++)if(c=a[g],3==c.type&&c.pubRecReceived){var n=new q(6,{messageIdentifier:c.messageIdentifier});this._schedule_message(n)}else this._schedule_message(c);if(this.connectOptions.onSuccess)this.connectOptions.onSuccess({invocationContext:this.connectOptions.invocationContext});"
"c=!1;this._reconnecting&&(c=!0,this._reconnectInterval=1,this._reconnecting=!1);this._connected(c,this._wsuri);this._process_queue();break;case 3:this._receivePublish(a);break;case 4:if(c=this._sentMessages[a.messageIdentifier])if(delete this._sentMessages[a.messageIdentifier],localStorage.removeItem('Sent:'+this._localKey+a.messageIdentifier),this.onMessageDelivered)this.onMessageDelivered(c.payloadMessage);break;case 5:if(c=this._sentMessages[a.messageIdentifier])c.pubRecReceived=!0,n=new q(6,{messageIdentifier:a.messageIdentifier}),"
"this.store('Sent:',c),this._schedule_message(n);break;case 6:d=this._receivedMessages[a.messageIdentifier];localStorage.removeItem('Received:'+this._localKey+a.messageIdentifier);d&&(this._receiveMessage(d),delete this._receivedMessages[a.messageIdentifier]);var l=new q(7,{messageIdentifier:a.messageIdentifier});this._schedule_message(l);break;case 7:c=this._sentMessages[a.messageIdentifier];delete this._sentMessages[a.messageIdentifier];localStorage.removeItem('Sent:'+this._localKey+a.messageIdentifier);"
"if(this.onMessageDelivered)this.onMessageDelivered(c.payloadMessage);break;case 9:if(c=this._sentMessages[a.messageIdentifier]){c.timeOut&&c.timeOut.cancel();if(128===a.returnCode[0]){if(c.onFailure)c.onFailure(a.returnCode)}else if(c.onSuccess)c.onSuccess(a.returnCode);delete this._sentMessages[a.messageIdentifier]}break;case 11:if(c=this._sentMessages[a.messageIdentifier])c.timeOut&&c.timeOut.cancel(),c.callback&&c.callback(),delete this._sentMessages[a.messageIdentifier];break;case 13:this.sendPinger.reset();"
"break;case 14:this._disconnected(h.INVALID_MQTT_MESSAGE_TYPE.code,f(h.INVALID_MQTT_MESSAGE_TYPE,[a.type]));break;default:this._disconnected(h.INVALID_MQTT_MESSAGE_TYPE.code,f(h.INVALID_MQTT_MESSAGE_TYPE,[a.type]))}}catch(t){c='undefined'==t.hasOwnProperty('stack')?t.stack.toString():'No Error Stack Available',this._disconnected(h.INTERNAL_ERROR.code,f(h.INTERNAL_ERROR,[t.message,c]))}};d.prototype._on_socket_error=function(a){this._reconnecting||this._disconnected(h.SOCKET_ERROR.code,f(h.SOCKET_ERROR,"
"[a.data]))};d.prototype._on_socket_close=function(){this._reconnecting||this._disconnected(h.SOCKET_CLOSE.code,f(h.SOCKET_CLOSE))};d.prototype._socket_send=function(a){if(1==a.type){var b=this._traceMask(a,'password');this._trace('Client._socket_send',b)}else this._trace('Client._socket_send',a);this.socket.send(a.encode());this.sendPinger.reset()};d.prototype._receivePublish=function(a){switch(a.payloadMessage.qos){case 'undefined':case 0:this._receiveMessage(a);break;case 1:var b=new q(4,{messageIdentifier:a.messageIdentifier});"
"this._schedule_message(b);this._receiveMessage(a);break;case 2:this._receivedMessages[a.messageIdentifier]=a;this.store('Received:',a);a=new q(5,{messageIdentifier:a.messageIdentifier});this._schedule_message(a);break;default:throw Error('Invaild qos\x3d'+wireMmessage.payloadMessage.qos);}};d.prototype._receiveMessage=function(a){if(this.onMessageArrived)this.onMessageArrived(a.payloadMessage)};d.prototype._connected=function(a,b){if(this.onConnected)this.onConnected(a,b)};d.prototype._reconnect="
"function(){this._trace('Client._reconnect');this.connected||(this._reconnecting=!0,this.sendPinger.cancel(),this.receivePinger.cancel(),128>this._reconnectInterval&&(this._reconnectInterval*=2),this.connectOptions.uris?(this.hostIndex=0,this._doConnect(this.connectOptions.uris[0])):this._doConnect(this.uri))};d.prototype._disconnected=function(a,b){this._trace('Client._disconnected',a,b);if(void 0!==a&&this._reconnecting)this._reconnectTimeout=new w(this,window,this._reconnectInterval,this._reconnect);"
"else if(this.sendPinger.cancel(),this.receivePinger.cancel(),this._connectTimeout&&(this._connectTimeout.cancel(),this._connectTimeout=null),this._msg_queue=[],this._buffered_msg_queue=[],this._notify_msg_sent={},this.socket&&(this.socket.onopen=null,this.socket.onmessage=null,this.socket.onerror=null,this.socket.onclose=null,1===this.socket.readyState&&this.socket.close(),delete this.socket),this.connectOptions.uris&&this.hostIndex<this.connectOptions.uris.length-1)this.hostIndex++,this._doConnect(this.connectOptions.uris[this.hostIndex]);"
"else if(void 0===a&&(a=h.OK.code,b=f(h.OK)),this.connected){this.connected=!1;if(this.onConnectionLost)this.onConnectionLost({errorCode:a,errorMessage:b,reconnect:this.connectOptions.reconnect,uri:this._wsuri});a!==h.OK.code&&this.connectOptions.reconnect&&(this._reconnectInterval=1,this._reconnect())}else if(4===this.connectOptions.mqttVersion&&!1===this.connectOptions.mqttVersionExplicit)this._trace('Failed to connect V4, dropping back to V3'),this.connectOptions.mqttVersion=3,this.connectOptions.uris?"
"(this.hostIndex=0,this._doConnect(this.connectOptions.uris[0])):this._doConnect(this.uri);else if(this.connectOptions.onFailure)this.connectOptions.onFailure({invocationContext:this.connectOptions.invocationContext,errorCode:a,errorMessage:b})};d.prototype._trace=function(){if(this.traceFunction){for(var a in arguments)'undefined'!==typeof arguments[a]&&arguments.splice(a,1,JSON.stringify(arguments[a]));a=Array.prototype.slice.call(arguments).join('');this.traceFunction({severity:'Debug',message:a})}if(null!=="
"this._traceBuffer){a=0;for(var b=arguments.length;a<b;a++)this._traceBuffer.length==this._MAX_TRACE_ENTRIES&&this._traceBuffer.shift(),0===a?this._traceBuffer.push(arguments[a]):'undefined'===typeof arguments[a]?this._traceBuffer.push(arguments[a]):this._traceBuffer.push('  '+JSON.stringify(arguments[a]))}};d.prototype._traceMask=function(a,b){var c={},d;for(d in a)a.hasOwnProperty(d)&&(c[d]=d==b?'******':a[d]);return c};var G=function(a,b,c,k){var e;if('string'!==typeof a)throw Error(f(h.INVALID_TYPE,"
"[typeof a,'host']));if(2==arguments.length){k=b;e=a;var g=e.match(/^(wss?):\/\/((\[(.+)\])|([^\/]+?))(:(\d+))?(\/.*)$/);if(g)a=g[4]||g[2],b=parseInt(g[7]),c=g[8];else throw Error(f(h.INVALID_ARGUMENT,[a,'host']));}else{3==arguments.length&&(k=c,c='/mqtt');if('number'!==typeof b||0>b)throw Error(f(h.INVALID_TYPE,[typeof b,'port']));if('string'!==typeof c)throw Error(f(h.INVALID_TYPE,[typeof c,'path']));e='ws://'+(-1!==a.indexOf(':')&&'['!==a.slice(0,1)&&']'!==a.slice(-1)?'['+a+']':a)+':'+b+c}for(var m="
"g=0;m<k.length;m++){var n=k.charCodeAt(m);55296<=n&&56319>=n&&m++;g++}if('string'!==typeof k||65535<g)throw Error(f(h.INVALID_ARGUMENT,[k,'clientId']));var l=new d(e,a,b,c,k);this._getHost=function(){return a};this._setHost=function(){throw Error(f(h.UNSUPPORTED_OPERATION));};this._getPort=function(){return b};this._setPort=function(){throw Error(f(h.UNSUPPORTED_OPERATION));};this._getPath=function(){return c};this._setPath=function(){throw Error(f(h.UNSUPPORTED_OPERATION));};this._getURI=function(){return e};"
"this._setURI=function(){throw Error(f(h.UNSUPPORTED_OPERATION));};this._getClientId=function(){return l.clientId};this._setClientId=function(){throw Error(f(h.UNSUPPORTED_OPERATION));};this._getOnConnected=function(){return l.onConnected};this._setOnConnected=function(a){if('function'===typeof a)l.onConnected=a;else throw Error(f(h.INVALID_TYPE,[typeof a,'onConnected']));};this._getDisconnectedPublishing=function(){return l.disconnectedPublishing};this._setDisconnectedPublishing=function(a){l.disconnectedPublishing="
"a};this._getDisconnectedBufferSize=function(){return l.disconnectedBufferSize};this._setDisconnectedBufferSize=function(a){l.disconnectedBufferSize=a};this._getOnConnectionLost=function(){return l.onConnectionLost};this._setOnConnectionLost=function(a){if('function'===typeof a)l.onConnectionLost=a;else throw Error(f(h.INVALID_TYPE,[typeof a,'onConnectionLost']));};this._getOnMessageDelivered=function(){return l.onMessageDelivered};this._setOnMessageDelivered=function(a){if('function'===typeof a)l.onMessageDelivered="
"a;else throw Error(f(h.INVALID_TYPE,[typeof a,'onMessageDelivered']));};this._getOnMessageArrived=function(){return l.onMessageArrived};this._setOnMessageArrived=function(a){if('function'===typeof a)l.onMessageArrived=a;else throw Error(f(h.INVALID_TYPE,[typeof a,'onMessageArrived']));};this._getTrace=function(){return l.traceFunction};this._setTrace=function(a){if('function'===typeof a)l.traceFunction=a;else throw Error(f(h.INVALID_TYPE,[typeof a,'onTrace']));};this.connect=function(a){a=a||{};z(a,"
"{timeout:'number',userName:'string',password:'string',willMessage:'object',keepAliveInterval:'number',cleanSession:'boolean',useSSL:'boolean',invocationContext:'object',onSuccess:'function',onFailure:'function',hosts:'object',ports:'object',reconnect:'boolean',mqttVersion:'number',mqttVersionExplicit:'boolean',uris:'object'});void 0===a.keepAliveInterval&&(a.keepAliveInterval=60);if(4<a.mqttVersion||3>a.mqttVersion)throw Error(f(h.INVALID_ARGUMENT,[a.mqttVersion,'connectOptions.mqttVersion']));void 0==="
"a.mqttVersion?(a.mqttVersionExplicit=!1,a.mqttVersion=4):a.mqttVersionExplicit=!0;if(void 0!==a.password&&void 0===a.userName)throw Error(f(h.INVALID_ARGUMENT,[a.password,'connectOptions.password']));if(a.willMessage){if(!(a.willMessage instanceof r))throw Error(f(h.INVALID_TYPE,[a.willMessage,'connectOptions.willMessage']));a.willMessage.stringPayload=null;if('undefined'===typeof a.willMessage.destinationName)throw Error(f(h.INVALID_TYPE,[typeof a.willMessage.destinationName,'connectOptions.willMessage.destinationName']));"
"}'undefined'===typeof a.cleanSession&&(a.cleanSession=!0);if(a.hosts){if(!(a.hosts instanceof Array))throw Error(f(h.INVALID_ARGUMENT,[a.hosts,'connectOptions.hosts']));if(1>a.hosts.length)throw Error(f(h.INVALID_ARGUMENT,[a.hosts,'connectOptions.hosts']));for(var b=!1,d=0;d<a.hosts.length;d++){if('string'!==typeof a.hosts[d])throw Error(f(h.INVALID_TYPE,[typeof a.hosts[d],'connectOptions.hosts['+d+']']));if(/^(wss?):\/\/((\[(.+)\])|([^\/]+?))(:(\d+))?(\/.*)$/.test(a.hosts[d]))if(0===d)b=!0;else{if(!b)throw Error(f(h.INVALID_ARGUMENT,"
"[a.hosts[d],'connectOptions.hosts['+d+']']));}else if(b)throw Error(f(h.INVALID_ARGUMENT,[a.hosts[d],'connectOptions.hosts['+d+']']));}if(b)a.uris=a.hosts;else{if(!a.ports)throw Error(f(h.INVALID_ARGUMENT,[a.ports,'connectOptions.ports']));if(!(a.ports instanceof Array))throw Error(f(h.INVALID_ARGUMENT,[a.ports,'connectOptions.ports']));if(a.hosts.length!==a.ports.length)throw Error(f(h.INVALID_ARGUMENT,[a.ports,'connectOptions.ports']));a.uris=[];for(d=0;d<a.hosts.length;d++){if('number'!==typeof a.ports[d]||"
"0>a.ports[d])throw Error(f(h.INVALID_TYPE,[typeof a.ports[d],'connectOptions.ports['+d+']']));var b=a.hosts[d],g=a.ports[d];e='ws://'+(-1!==b.indexOf(':')?'['+b+']':b)+':'+g+c;a.uris.push(e)}}}l.connect(a)};this.subscribe=function(a,b){if('string'!==typeof a)throw Error('Invalid argument:'+a);b=b||{};z(b,{qos:'number',invocationContext:'object',onSuccess:'function',onFailure:'function',timeout:'number'});if(b.timeout&&!b.onFailure)throw Error('subscribeOptions.timeout specified with no onFailure callback.');"
"if('undefined'!==typeof b.qos&&0!==b.qos&&1!==b.qos&&2!==b.qos)throw Error(f(h.INVALID_ARGUMENT,[b.qos,'subscribeOptions.qos']));l.subscribe(a,b)};this.unsubscribe=function(a,b){if('string'!==typeof a)throw Error('Invalid argument:'+a);b=b||{};z(b,{invocationContext:'object',onSuccess:'function',onFailure:'function',timeout:'number'});if(b.timeout&&!b.onFailure)throw Error('unsubscribeOptions.timeout specified with no onFailure callback.');l.unsubscribe(a,b)};this.send=function(a,b,c,d){var e;if(0==="
"arguments.length)throw Error('Invalid argument.length');if(1==arguments.length){if(!(a instanceof r)&&'string'!==typeof a)throw Error('Invalid argument:'+typeof a);e=a;if('undefined'===typeof e.destinationName)throw Error(f(h.INVALID_ARGUMENT,[e.destinationName,'Message.destinationName']));}else e=new r(b),e.destinationName=a,3<=arguments.length&&(e.qos=c),4<=arguments.length&&(e.retained=d);l.send(e)};this.publish=function(a,b,c,d){console.log('Publising message to: ',a);var e;if(0===arguments.length)throw Error('Invalid argument.length');"
"if(1==arguments.length){if(!(a instanceof r)&&'string'!==typeof a)throw Error('Invalid argument:'+typeof a);e=a;if('undefined'===typeof e.destinationName)throw Error(f(h.INVALID_ARGUMENT,[e.destinationName,'Message.destinationName']));}else e=new r(b),e.destinationName=a,3<=arguments.length&&(e.qos=c),4<=arguments.length&&(e.retained=d);l.send(e)};this.disconnect=function(){l.disconnect()};this.getTraceLog=function(){return l.getTraceLog()};this.startTrace=function(){l.startTrace()};this.stopTrace="
"function(){l.stopTrace()};this.isConnected=function(){return l.connected}};G.prototype={get host(){return this._getHost()},set host(a){this._setHost(a)},get port(){return this._getPort()},set port(a){this._setPort(a)},get path(){return this._getPath()},set path(a){this._setPath(a)},get clientId(){return this._getClientId()},set clientId(a){this._setClientId(a)},get onConnected(){return this._getOnConnected()},set onConnected(a){this._setOnConnected(a)},get disconnectedPublishing(){return this._getDisconnectedPublishing()},"
"set disconnectedPublishing(a){this._setDisconnectedPublishing(a)},get disconnectedBufferSize(){return this._getDisconnectedBufferSize()},set disconnectedBufferSize(a){this._setDisconnectedBufferSize(a)},get onConnectionLost(){return this._getOnConnectionLost()},set onConnectionLost(a){this._setOnConnectionLost(a)},get onMessageDelivered(){return this._getOnMessageDelivered()},set onMessageDelivered(a){this._setOnMessageDelivered(a)},get onMessageArrived(){return this._getOnMessageArrived()},set onMessageArrived(a){this._setOnMessageArrived(a)},"
"get trace(){return this._getTrace()},set trace(a){this._setTrace(a)}};var r=function(a){var b;if('string'===typeof a||a instanceof ArrayBuffer||a instanceof Int8Array||a instanceof Uint8Array||a instanceof Int16Array||a instanceof Uint16Array||a instanceof Int32Array||a instanceof Uint32Array||a instanceof Float32Array||a instanceof Float64Array)b=a;else throw f(h.INVALID_ARGUMENT,[a,'newPayload']);this._getPayloadString=function(){return'string'===typeof b?b:E(b,0,b.length)};this._getPayloadBytes="
"function(){if('string'===typeof b){var a=new ArrayBuffer(n(b)),a=new Uint8Array(a);D(b,a,0);return a}return b};var c;this._getDestinationName=function(){return c};this._setDestinationName=function(a){if('string'===typeof a)c=a;else throw Error(f(h.INVALID_ARGUMENT,[a,'newDestinationName']));};var d=0;this._getQos=function(){return d};this._setQos=function(a){if(0===a||1===a||2===a)d=a;else throw Error('Invalid argument:'+a);};var e=!1;this._getRetained=function(){return e};this._setRetained=function(a){if('boolean'==="
"typeof a)e=a;else throw Error(f(h.INVALID_ARGUMENT,[a,'newRetained']));};var g=!1;this._getDuplicate=function(){return g};this._setDuplicate=function(a){g=a}};r.prototype={get payloadString(){return this._getPayloadString()},get payloadBytes(){return this._getPayloadBytes()},get destinationName(){return this._getDestinationName()},set destinationName(a){this._setDestinationName(a)},get topic(){return this._getDestinationName()},set topic(a){this._setDestinationName(a)},get qos(){return this._getQos()},"
"set qos(a){this._setQos(a)},get retained(){return this._getRetained()},set retained(a){this._setRetained(a)},get duplicate(){return this._getDuplicate()},set duplicate(a){this._setDuplicate(a)}};return{Client:G,Message:r}}(window)});</script>";
*/

const char HTTP_FORM_ROOT[] PROGMEM =
	"<html>{HD}<body>"
	"<div class='header' 'id='logo'><h1>MyTapparella</h1></div>"
		"<form action='login' method='POST'>"
			"<div class='grid-container'>"
				"<div class='col-6 col-s-12'>"
					"<label for='webusr'>Configuration web client webusr:</label>"
					"<input type='text' name='webusr' value='{WU}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='webpsw'>Configuration web client webpsw:</label>"
					 "<input type='webpsw' name='webpsw' placeholder='**********'>"
				"</div>"
			"</div>"
			"<div class='footer'>"
				"<input type='submit' value='Login'>"
			"</div>"
		"</form>"
	"</div></body></html>";

const char HTTP_FORM_WIFI[] PROGMEM =	
	"<html>{HD}<body>"
	"<div class='header' 'id='logo'><h1>MyTapparella</h1></div>"
	"<div id='form'>"
        "<form action='/login' method='POST'>"
			"<div class='grid-container'>"
				"<div class='col-6 col-s-12'><label for='clntssid1'>Wifi instructure 1 SSID:</label>"
					 "<input type='text' name='clntssid1' value='{S1}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='clntpsw1'>Wifi instructure 1 webpsw:</label>"
					 "<input type='webpsw' name='clntpsw1' value='{P1}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='clntssid2'>Wifi instructure 2 SSID:</label>"
					 "<input type='text' name='clntssid2' value='{S2}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='clntpsw2'>Wifi instructure 2 webpsw:</label>"
					 "<input type='webpsw' name='clntpsw2' value='{P2}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='appssid'>Wifi local AP SSID:</label>"
					 "<input type='text' name='appssid' value='{AS}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='apppsw'>Wifi local AP webpsw:</label>"
					 "<input type='webpsw' name='apppsw' value='{AP}'>"
				"</div>"
				"<div class='col-2'></div>"
				"<div class='col-2 col-s-12'>"
					"<input type='submit' name='svwifi' value='Save' formaction='/modify' formmethod='post'>"
				"</div>"
				"<div class='col-4'></div>"
				"<div class='col-2 col-s-12'>"
					"<input type='submit' value='Back'>"
				"</div>"
			"</div>"
        "</form>"
	"</div></div></body></html>";
	
const char HTTP_FORM_SYSTEM[] PROGMEM =	
	"<html>{HD}<body onload='loadTimeZoneList();showLoaded();'>"
	"<div class='header' 'id='logo'><h1>MyTapparella</h1></div>"
	"<div id='form'>"
        "<form action='/login' method='POST'>"
			"<div class='grid-container'>"
				"<div class='col-6 col-s-12'>"
					"<label for='webusr'>Configuration web client webusr:</label>"
					 "<input type='text' name='webusr' value='{WU}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='webpsw'>Configuration web client webpsw:</label>"
					 "<input type='webpsw' name='webpsw' value='{WP}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='ntpaddr1'>NTP server 1:</label>"
					 "<input type='text' name='ntpaddr1' value='{N1}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='ntpaddr2'>NTP server 2:</label>"
					 "<input type='text' name='ntpaddr2' value='{N2}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='utcsync'>NTP sync interval (sec):</label>"
					 "<input type='text' name='utcsync' value='{N3}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='utcadj'>NTP error adjust (msec):</label>"
					 "<input type='text' name='utcadj' value='{N4}'>"
				"</div>"	
				"<div class='col-6 col-s-12'><label for='utczone'>NTP SDT time zones (hour):</label>"
					 "<select id='utczone' name='utczone'  onchange='showSelected()'></select><br>"
				"</div>"	
				"<div class='col-6 col-s-12'><label for='report'>Selected time zone</label>"
					"<input type='text' id='report' name='report' disabled='disabled' />"
				"</div>"
				"<div class='col-6 col-s-12'><label for='utcsdt'>Set daylight save (legal time)</label>"
					 "<input type='checkbox' name='utcsdt' value='y' {N6}>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='reboot'>Reboot the system with default config</label>"
					 "<input type='checkbox' name='rebootd' value='y'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='reboot'>Reboot the system</label>"
					 "<input type='checkbox' name='reboot' value='y'>"
				"</div>"
#if (!AUTOCAL) 
				"<div class='col-6 col-s-12'>"
					"<label for=\stdel1'>Start delay on click of group 1:</label>"
					 "<input type='text' name='stdel1' value='{S1}'>"
				"</div>"
				"<div class='col-6 col-s-12'>"
					"<label for=\stdel2'>Start delay on click of group 2:</label>"
					 "<input type='text' name='stdel2' value='{S2}'>"
				"</div>"
#endif		
				"<div class='col-2'></div>"
					"<div class='col-2 col-s-12'>"
						"<input type='submit' name='svsystem' value='Save' formaction='/modify' formmethod='post'>"
					"</div>"
					"<div class='col-4'></div>"
					"<div class='col-2 col-s-12'>"
						"<input type='submit' value='Back'>"
				"</div>"
			"</div>"
        "</form>"
	"</div></div>"
	"<script>"
		"function loadTimeZoneList(){" 
			"var d = new Date();"
			"var n = -d.getTimezoneOffset()/60;"
			"var select = document.getElementById('utczone');"
			"select.innerHTML = '';" 
			"for(i=-11; i<13; i++){"
				"option = document.createElement('option');"      
				"option.textContent = '(GMT '+(i<0?'':'+') + i +':00)';"  
				"option.value = i;"
				"if (n == i){"
					"option.selected = true;"
				"}"
				"select.appendChild(option);"
			"}"
			"showSelected();"
		"};"
		"function showSelected(){"
			"document.getElementById('report').value=document.getElementById('utczone').value;"
		"}"
		"function showLoaded(){"
			"document.getElementById('report').value={N5};"
		"}"
	"</script>"
	"</body></html>";
	
const char HTTP_FORM_MQTT[] PROGMEM =	
	"<html>{HD}<body>"
	"<div class='header' 'id='logo'><h1>MyTapparella</h1></div>"
	"<div id='form'>"
		"<form action='/login' method='POST'>"
			"<div class='grid-container'>"
				"<div class='col-6 col-s-12'><label for='mqttaddr'>MQTT server:</label>"
					 "<input type='text' name='mqttaddr' value='{MA}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='mqttid'>MQTT Client ID:</label>"
					 "<input type='text' name='mqttid' value='{MI}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='mqttport'>MQTT port:</label>"
					 "<input type='text' name='mqttport' value='{MT}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='wsport'>WS port:</label>"
					 "<input type='text' name='wsport' value='{WT}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='mqttproto'>MQTT path:</label>"
					 "<input type='text' name='mqttproto' value='{PP}'>"
				"</div>"
				"<div class='col-6 col-s-12'>"
				"</div>"
				/*"<div class='col-6 col-s-12'><label for='mqttproto'>MQTT protocol:</label>"
					 "<select id='mqttproto' name='mqttproto'>"
						  "<option value='ws' {P1}>ws</option>"
						  "<option value='tcp' {P2}>tcp</option>"
					 "</select>"
				"</div>"*/
				"<div class='col-6 col-s-12'><label for='mqttusr'>MQTT user name:</label>"
					 "<input type='text' name='mqttusr' value='{MU}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='mqttpsw'>MQTT user webpsw:</label>"
					 "<input type='text' name='mqttpsw' value='{MP}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='mqttintopic'>In topic:</label>"
					 "<input type='text' name='mqttintopic' value='{QI}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='mqttouttopic'>Out topic:</label>"
					 "<input type='text' name='mqttouttopic' value='{MO}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='btnup1'>MQTT message button 1 UP:</label>"
					 "<input type='text' name='btnup1' value='{J1}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='btndown1'>MQTT message button 1 DOWN:</label>"
					 "<input type='text' name='btndown1' value='{J2}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='btnup2'>MQTT message button 2 UP:</label>"
					 "<input type='text' name='btnup2' value='{J3}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='btndown2'>MQTT message button 2 DOWN:</label>"
					 "<input type='text' name='btndown2' value='{J4}'>"
				"</div>"
				/*
				"<div class='col-6 col-s-12'><label for='btnmeanpwr'>Mean power request button:</label>"
					 "<input type='text' name='btnmeanpwr' value='{J6}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='btnpeakpwr'>Peak power request button:</label>"
					 "<input type='text' name='btnpeakpwr' value='{J7}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='btnall'>All states request button:</label>"
					 "<input type='text' name='btnall' value='{J8}'>"
				"</div>"*/				
				"<div class='col-2'></div>"
				"<div class='col-2 col-s-12'>"
					"<input type='submit' name='svmqtt' value='Save' formaction='/modify' formmethod='post'>"
				"</div>"
				"<div class='col-4'></div>"
				"<div class='col-2 col-s-12'>"
					"<input type='submit' value='Back'>"
				"</div>"
			"</div>"
		"</form>"
	"</div></body></html>";
	
const char HTTP_FORM_LOGIC[] PROGMEM =	
	"<html>{HD}<body>"
	"<div class='header' 'id='logo'><h1>MyTapparella</h1></div>"
	"<div id='form'>"
        "<form action='/login' method='POST'>"
			"<div class='grid-container'>"
				"<div class='col-6 col-s-12'><label for='swroll1'>Mode switch 1 and 2 (Switch if checked, Roller Shutter if unchecked)</label>"
					 "<input type='checkbox' name='swroll1' value='1' {H1}>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='swroll2'>Mode switch 3 and 4 (Switch if checked, Roller Shutter if unchecked)</label>"
					 "<input type='checkbox' name='swroll2' value='1' {H2}>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='thalt1'>End-of-stroke time switch 1:</label>"
					 "<input type='text' name='thalt1' value='{TU}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='thalt2'>End-of-stroke time switch 2:</label>"
					 "<input type='text' name='thalt2' value='{TD}'  {H3}>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='thalt3'>End-of-stroke time switch 3:</label>"
					 "<input type='text' name='thalt3' value='{DU}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='thalt4'>End-of-stroke time switch 4:</label>"
					 "<input type='text' name='thalt4' value='{DD}'  {H4}>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='tlength'>Rollershutter excursion:</label>"
					 "<input type='text' name='tlength' value='{TL}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='barrelrad'>Barrel radius:</label>"
					 "<input type='text' name='barrelrad' value='{BR}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='thickness'>Roller shutter thickness</label>"
					 "<input type='text' name='thickness' value='{TN}'>"
				"</div>"
				"<div class='col-6 col-s-12'><label for='slatsratio'>Roller shutter slatsratio</label>"
					 "<input type='text' name='slatsratio' value='{SR}'>"
				"</div>"
				"<div class='col-2'></div>"
				"<div class='col-2 col-s-12'>"
					"<input type='submit' name='svlogic' value='Save' formaction='/modify' formmethod='post'>"
				"</div>"
				"<div class='col-4'></div>"
				"<div class='col-2 col-s-12'>"
					"<input type='submit' value='Back'>"
				"</div>"
			"</div>"   
        "</form>"
	"</div></body></html>";
		
	//EVENT MANAGEMENT CONFIG
	const char HTTP_FORM_EVENT[] PROGMEM =	
	"<html>{HD}<body>"
	"<div class='header' 'id='logo'><h1>MyTapparella</h1></div>"
	"<div id='form'>"
		"<form action='/login' method='POST'>"
			"<div class='grid-container'>"
				"<div class='col-6 col-s-12 boxed'>"
					"<label for='c1'>Switch 1 condition:</label>"
					"<textarea id='c1' name='c1' cols='100' rows='4' {H3}>{C1}</textarea>"
					"<div id='sw1' {V1}>"
						"<label for='a1'>Switch 1 action</label>"
						 "<select id='a1' name='a1'>"
							  "<option value='0'>SetReset</option>"
							  "<option value='1'>No action</option>"
							  "<option value='2'>Monostable normally open</option>"
							  "<option value='3'>Monostable normally close</option>"
						 "</select><br>"
						 "<label for='haltdelay1'>Delay for timer SW 1</label>"
						 "<input type='text' id='haltdelay1' name='haltdelay1' value='{D1}'>"
					 "</div>"
					 "<label for='smplt1'>Sample time SW 1</label>"
					 "<input type='text' id='smplt1' name='smplt1' value='{S1}'>"
					 "<label for='oe1'>Output enable SW 1</label>"
					 "<input type='checkbox' id='oe1' name='oe1' value='0' {O1}>"
				"</div>"
				"<div class='col-6 col-s-12 boxed'>"
					"<label for='c2'>Switch 2 condition:</label>"
					"<textarea id='c2' name='c2' cols='100' rows='4' {H3}>{C2}</textarea>"
					"<div id='sw2' {V1}>"
						"<label for='a2'>Switch 2 action</label>"
						"<select id='a2' name='a2'>"
							  "<option value='0'>SetReset</option>"
							  "<option value='1'>No action</option>"
							  "<option value='2'>Monostable normally open</option>"
							  "<option value='3'>Monostable normally close</option>"
						"</select><br>"
						"<label for='haltdelay2'>Delay for timer SW 2</label>"
						"<input type='text' id='haltdelay2' name='haltdelay2' value='{D2}'>"
					"</div>"
					"<label for='smplt2'>Sample time SW 2</label>"
					"<input type='text' id='smplt2' name='smplt2' value='{S2}'>"
					"<label for='oe2'>Output enable SW 2</label>"
					"<input type='checkbox' id='oe2' name='oe2' value='0' {O2}>"
				"</div>"
				"<div class='col-6 col-s-12 boxed'>"
					"<label for='c3'>Switch 3 condition:</label>"
					"<textarea id='c3' name='c3' cols='100' rows='4' {H4}>{C3}</textarea>"
					"<div id='sw3' {V2}>"
						"<label for='a3'>Switch 3 action</label>"
						"<select id='a3' name='a3'>"
							  "<option value='0'>SetReset</option>"
							  "<option value='1'>No action</option>"
							  "<option value='2'>Monostable normally open</option>"
							  "<option value='3'>Monostable normally close</option>"
						"</select><br>"
						"<label for='haltdelay3'>Delay for timer SW 3</label>"
						"<input type='text' id='haltdelay3' name='haltdelay3' value='{D3}'>"
					"</div>"
					"<label for='smplt3'>Sample time SW 3</label>"
					"<input type='text' id='smplt3' name='smplt3' value='{S3}'>"
					"<label for='oe3'>Output enable SW 3</label>"
					"<input type='checkbox' id='oe3' name='oe3' value='0' {O3}>"
				"</div>"
				"<div class='col-6 col-s-12 boxed'>"
					"<label for='c4'>Switch 4 condition:</label>"
					"<textarea id='c4' name='c4' cols='100' rows='4' {H4}>{C4}</textarea>"
					"<div id='sw4' {V2}>"
						"<label for='a4'>Switch 4 action</label>"
						"<select id='a4' name='a4'>"
							  "<option value='0'>SetReset</option>"
							  "<option value='1'>No action</option>"
							  "<option value='2'>Monostable normally open</option>"
							  "<option value='3'>Monostable normally close</option>"
						"</select><br>"
						"<label for='haltdelay4'>Delay for timer SW 4</label>"
						"<input type='text' id='haltdelay4' name='haltdelay4' value='{D4}'>"
					"</div>"
					"<label for='smplt4'>Sample time SW 4</label>"
					"<input type='text' id='smplt4' name='smplt4' value='{S4}'>"
					"<label for='oe2'>Output enable SW 4</label>"
					"<input type='checkbox' id='oe4' name='oe4' value='0' {O4}>"
				"</div>"
				"<div class='col-12 col-s-12 boxed'>"
					"<label for='act'>Switches action commands:</label>"
					"<textarea id='act' name='act' cols='100' rows='4' {H4}>{AC}</textarea>"
				"</div>"
				"<div class='col-12 col-s-12 boxed'>"
					"<label for='call'>General action commands:</label>"
					"<textarea id='call' name='call' cols='100' rows='4' {H5}>{AD}</textarea>"
				"</div>"
				"<div class='col-12 col-s-12'>"
					"<input type='submit' value='Back'>"
				"</div>"
			"</div>"
		"</form>"
	"</div>"	
	"<script>"
		//client side dynamic event management
		"{SH}"
		"var oe1=document.getElementById('oe1');"
		"var oe2=document.getElementById('oe2');"
		"var oe3=document.getElementById('oe3');"
		"var oe4=document.getElementById('oe4');"
		"oe1.addEventListener('change', function(){condsend(0,'c1')}, false);"
		"oe2.addEventListener('change', function(){condsend(1,'c2')}, false);"
		"oe3.addEventListener('change', function(){condsend(2,'c3')}, false);"
		"oe4.addEventListener('change', function(){condsend(3,'c4')}, false);"
		"var act=document.getElementById('act');"
		"act.addEventListener('change', function(){actsend(act.value);}, false);"
		"call.addEventListener('change', function(){actsend(call.value);}, false);"
		"function setfield(str,field,val){"
			"if(str.search(field)>=0){"
				"var digit = '=[0-9]+';"
				"var regex = new RegExp(field+digit, 'g');"
				"str = str.replace(regex,field+'='+val);"
			"}else{"
				"str = str+'|('+field+'='+val+')';"
			"}"
			"console.log('digit:'+ digit);"
			"console.log('field: '+field);"
			"console.log('val: '+field+'='+val);"
			"console.log('setfield: '+str);"
			"return str;"
		"}"
		"function addCheck(chkstr,act){"
			"var chk=document.getElementById(chkstr);"
			"chk.addEventListener('change', function (event) {"
				"if(chk.checked){"
					"act.value=setfield(act.value,chkstr,'1');"
				"}else{"
					"act.value=setfield(act.value,chkstr,'0');"
				"}"
				"actsend(act.value);"
			"});"
		"}"
		"function readSelect(selel,n,act){"
			//0: toggleLogic
			//1: condition output disabled
			//2: normalmente aperto, chiuso per un haltdelay alla pressione
			//3: normalmente chiuso, aperto per un haltdelay alla pressione
			"var sel=document.getElementById(selel);"
			//"var s=sel.options[sel.selectedIndex].value;"
			"sel.addEventListener('change', function (event) {"
				"console.log('selectedIndex: '+(sel.selectedIndex).toString());"
				"act.value=setfield(act.value,'ma'+n,(sel.selectedIndex).toString());"
				//"actsend(act.value);"
			"});"
		"}"
		"function readInput(inel,n,cmd,act){"
			"var inp=document.getElementById(inel);"
			//"var s=sel.options[sel.selectedIndex].value;"
			"inp.addEventListener('change', function (event) {"
				"console.log('selectedIndex: '+inp.value);"
				"act.value=setfield(act.value,cmd+n,inp.value);"
				//"actsend(act.value);"
			"});"
		"}"
		"function onRcv(d) {"
			//"document.getElementById('p').innerHTML = f.data;\n"
			"var obj = JSON.parse(d);"
			"for(x in obj){"
				"if(x=='mode1'){"
					"var el1 = document.getElementById('sw1');"
					"var el2 = document.getElementById('sw2');"
					"if(obj[x] == \"1\"){"
						"el1.style.display = 'none';"
						"el2.style.display = 'none';"
					"}else{"
						"el1.style.display = 'block';"
						"el2.style.display = 'block';"
					"}"
				"}"
				"if(x=='mode2'){"
					"var el3 = document.getElementById('sw3');"
					"var el4 = document.getElementById('sw4');"
					"if(obj[x] == \"1\"){"
						"el3.style.display = 'none';"
						"el4.style.display = 'none';"
					"}else{"
						"el3.style.display = 'block';"
						"el4.style.display = 'block';"
					"}"
				"}"
			"}"
		"}"
		"addCheck('oe1',act);"
		"addCheck('oe2',act);"
		"addCheck('oe3',act);"
		"addCheck('oe4',act);"
		"readSelect('a1',1,act);"
		"readSelect('a2',2,act);"
		"readSelect('a3',3,act);"
		"readSelect('a4',4,act);"
		"readInput('haltdelay1',1,'td',act);"
		"readInput('haltdelay2',2,'td',act);"
		"readInput('haltdelay3',3,'td',act);"
		"readInput('haltdelay4',4,'td',act);"
		"readInput('smplt1',1,'tsmpl',act);"
		"readInput('smplt2',2,'tsmpl',act);"
		"readInput('smplt3',3,'tsmpl',act);"
		"readInput('smplt4',4,'tsmpl',act);"
	"</script>"
	"</body></html>";
	//END EVENT MANAGEMENT CONFIG*/

/*const char [] PROGMEM =		
	"<label for='action'>Action {I} delay</label>"
	"<input id='d{I}' type='text' name='d{I}' value='D{I}'>"
	"<input id='sw{I}' type='button' value='Switch {I} config' onclick='confsend({I},'d{I}','c{I}')'>";*/
	
const char HTTP_FORM_LOGIN[] PROGMEM =	
	"<html>{HD}<body>"
	"<div class='header' 'id='logo'><h1>MyTapparella</h1></div>"
	"<div class='grid-container'>"
		"<div class='col-3 col-s-12'></div>"
		"<div id='form' class='col-6 col-s-12'>"
			"<form action='/login' method='POST'>"
				"<input type='submit' formaction='systconf' formmethod='post' value='System configuration'>"
				"<input type='submit' formaction='/wificonf' formmethod='post' value='WiFi configuration'>"
				"<br><input type='submit' formaction='mqttconf' formmethod='post' value='MQTT configuration'>"
				"<br><input type='submit' formaction='logicconf' formmethod='post' value='Logic configuration'>"
				"<br><input type='submit' formaction='eventconf' formmethod='post' value='Events configuration'>"
				"<br><input type='submit' value='Exit' name='disconnect'>"
			"</form>"
		"</div>"
		"<div class='col-3 col-s-12'></div>"
	"</div></body></html>";
	
const char HTTP_FORM_SUCCESS[] PROGMEM =	
	"<html>{HD}<body>"
	"<div class='header' 'id='logo'><h1>MyTapparella</h1></div>"
	"<div class='grid-container'>"
	"<div id='form' class='col-12 col-s-12'>"
		"<form action='/login' method='POST'>"
			"<div class='col-1-1'>"
                "<label><h1><p>Modifica avvenuta con successo.</p></label>"
            "</div>"
			"<input type='submit' value='Back'>"
		"</form>"
	"</div></div></body></html>";
	
const char HTTP_FORM_CMD[] PROGMEM =	
	//{"OUTSLED":"0","up1":"1","down1":"0","up2":"50","down2":"0"}
	"<html>{HD}<body>{SP}"
	"<div class='header' 'id='logo'><h1>MyTapparella</h1></div>"
	"<div class='grid-container'>"
		"<div class='col-4 col-s-12'>"
			"<div class='asidetop'></div>"
			"<div class='aside'>"
				"<span id='temp'></span>"
			"</div>"
			"<div class='aside'>"
				"<span id='time'></span>"
			"</div>"
			"<div class='aside'>"
				"<span id='date'></span>"
			"</div>"
			"<div class='asidebottom'></div>"
		"</div>"
		"<div class='col-6 col-s-12'>"
			"<div id='form'>"
				"<form>"
					"<input id='up1' type='button' value='Button 1 UP' onclick='press(vls[0])' onmousedown='this.style.opacity='1'' onmouseup='this.style.opacity='0.6'' ontouchstart='this.style.opacity=\"1\"' ontouchend='this.style.opacity=\"0.6\"'>"
					"<br>"
					"<input id='down1' type='button' value='Button 1 DOWN' onclick='press(vls[1])' onmousedown='this.style.opacity=\"1\"' onmouseup='this.style.opacity=\"0.6\"' ontouchstart='this.style.opacity=\"1\"' ontouchend='this.style.opacity=\"0.6\"'>"
					"<br><br><br>"
					"<input id='up2' type='button' value='Button 2 UP' onclick='press(vls[2])' onmouseup='this.style.opacity=\"0.6\"' ontouchstart='this.style.opacity=\"1\"' ontouchend='this.style.opacity=\"0.6\"'>"
					"<br>"
					"<input id='down2' type='button' value='Button 2 DOWN' onclick='press(vls[3])' onmouseup='this.style.opacity=\"0.6\"' ontouchstart='this.style.opacity=\"1\"' ontouchend='this.style.opacity=\"0.6\"'>"
				"</form>"
				//"<p id='p'></p>"
			"</div>"
		"</div>"
		"<div class='col-2 col-s-12'>"
		"</div>"
	"</div>"
	"<div class='footer'>"
		"<div class='sldcont1' style='width:90%; clear:both; margin:0 auto'>"
			"<p>Group 1: <span id='val1'></span></p>"
			"<meter id='pr1' low='{PD}' min='0' max='100' ></meter>"
			"<input type='range' min='0' max='100' value='50' list='tickmarks' class='slider' id='rng1'>"
			"<datalist id='tickmarks'>"
				"<option>0</option>"
				"<option>{PD}</option>"
				"<option id='mid'></option>"
				"<option>100</option>"
			"</datalist>"
		"</div>"
		"<div class='sldcont2' style='width:90%; margin:0 auto'>"
			"<p>Group 2: <span id='val2'></span></p>"
			"<meter id='pr2' low='{PD}' min='0' max='100'></meter>"
			"<input type='range' min='0' max='100' value='50' list='tickmarks' class='slider' id='rng2'>"
		"</div>"
	"</div>"	
	"<script>"
		"{SH}"
		"var a=[0,0];"
		"var updt=[0,0];"
		"var p=[0,0];"
		"var tr=[0,0];"
		"(document.getElementById('mid')).innerHTML=50-{PD};"
		//"var c=[0,0];"
		"var dir=[0,0];"
		"var ie=[0,0,0,0];"
		"var sld1 = document.getElementById('rng1');"
		"var o1 = document.getElementById('val1');"
		"var tmp = document.getElementById('temp');"
		"var dt = document.getElementById('date');"
		"var tm = document.getElementById('time');"
		"o1.innerHTML = sld1.value;"
		"sld1.ontouchend = function() {"
			"o1.innerHTML = this.value;"
			"var pp=Number(sld1.value)+110;"
			"var vl=vlsp[0].replace('N', pp.toString());"
			"console.log(vl);"
			"press(vl);"
		"};"
		"sld1.onmouseup=sld1.ontouchend;"
		
		"var sld2 = document.getElementById('rng2');"
		"var o2 = document.getElementById('val2');"
		"o2.innerHTML = sld2.value;"
		"sld2.ontouchend = function() {"
			"o2.innerHTML = this.value;"
			"var pp=Number(sld2.value)+110;"
			"var vl=vlsp[1].replace('N', pp.toString());"
			"console.log(vl);"
			"press(vl);"
		"};"
		"sld2.onmouseup=sld2.ontouchend;"
		"function onRcv(d) {"
			//"document.getElementById('p').innerHTML = f.data;\n"
			"var obj = JSON.parse(d);"
			"for(x in obj){"
				"var el = document.getElementById(x);"
				"if(el){"  //controlla se il campo esiste nel DOM della pagina
					//"console.log(x);"
					"if(x=='up1'){"
						"if(obj[x] == \"1\"){"
							"ie[0]=1;"
						"}else{"
							"ie[0]=0;"
						"}"
					"}"
					"if(x=='down1'){"
						"if(obj[x] == \"1\"){"
							"ie[1]=1;"
						"}else{"
							"ie[1]=0;"
						"}"
					"}"
					"if(x=='up2'){"
						"if(obj[x] == \"1\"){"
							"ie[2]=1;"
						"}else{"
							"ie[2]=0;"
						"}"
					"}"
					"if(x=='down2'){"
						"if(obj[x] == \"1\"){"
							"ie[3]=1;"
						"}else{"
							"ie[3]=0;"
						"}"
					"}"
					"if(x=='up1' || x=='down1' || x=='up2' || x=='down2'){"
						"if(obj[x] == \"1\"){"
							"el.style.backgroundColor = \"#b30000\";"
						"}else{"
							"el.style.backgroundColor = \"#00ccff\";"
						"}"
					"}"
					"dir[0]=ie[0]+ie[1]*-1;"
					"dir[1]=ie[2]+ie[3]*-1;"
					//"console.log('a[0]:'+a[0]);" 
					//"console.log('x:'+x);" 
					"if(x=='pr1' && a[0]==0){"
						"a[0]=1;"
						"pr1.value=obj[x];"
						//"startPrgrBar(pr1,p[0],100,0);"
					"}"
					"if(x=='pr2' && a[1]==0){"
						"a[1]=1;"
						"pr2.value=obj[x];"
						//"startPrgrBar(pr2,p[1],100,1);"
					"}"
					"if(x=='{TP}'){"
						"tmp.innerHTML=obj[x]+' &#176;'+'C';"
						"tmp.style.backgroundColor = \"#333\";"
					"}"
					"if(x=='{DT}'){"
						"var res = obj[x].split('-');"
						"dt.innerHTML=res[0];"
						"tm.innerHTML=res[1];"
						"console.log('date:'+res[0]+' Time:'+res[1]);" 
						"dt.style.backgroundColor = \"#333\";"
						"tm.style.backgroundColor = \"#333\";"
					"}"
				"}else{"	
					"if(x=='sp1')"
						"p[0]=Number(obj[x]);"						
					"if(x=='sp2')"
						"p[1]=Number(obj[x]);"
					"if(x=='tr1')"
						"tr[0]=Number(obj[x]);"						
					"if(x=='tr2')"
						"tr[1]=Number(obj[x]);"
				"};"
			"}"
			"console.log('pr1:'+pr1.value+' p[0]:'+p[0]);" 
			//"console.log('x:'+x);" 
			"startPrgrBar(pr1,tr[0],p[0],100,5,0);"
			"startPrgrBar(pr2,tr[1],p[1],100,5,1);"
		"}"	
		"function startPrgrBar(p,ti,l,k,delay,n) {"
			"var t=ti;"
			"if(t>0){"
				//"l+=delay;"
				//"console.log('------------------------------------');"
				"var e=(dir[n]*l)/k;"
				//"console.log('dir[n]:'+dir[n]);"
				//"console.log('e:'+e);"
				//"console.log('----------------------------------');"
				"clearInterval(updt[n]);"
				"var aa=Math.round(calcLen(t,l));"
				"if(isNaN(aa) || !isFinite(aa))p.value=0; else p.value=aa;"
				"console.log('aa: '+aa);"
				"console.log('t:'+t+' p.value:'+p.value);"
				"updt[n]=setInterval(function(){"
					"if(dir[n]!=0){"
						"t=t+e;"
						"aa=Math.round(calcLen(t-delay*dir[n],l));"
						"console.log('aa: '+aa);"
						"if(isNaN(aa) || !isFinite(aa))p.value=0; else p.value=aa;"
						"console.log('++++++++++++++');"
						"console.log('t:'+(t-delay*dir[n])+' p.value:'+p.value+' dir:'+dir[n]);"
					"}else{"
						"a[n]=0;"
					"};"
				"}, l/k);"
			"};"
		"};"
		/*"function calct(v){"   
			"var tl={TL};"
			"var br={BR};"
			"var tn={TN};"
			"var nmax={NM};"
			"console.log('tl:'+tl+' br:'+br+' tn:'+tn+' nmax:'+nmax);"
			"var nv;"
			"nv = (Math.sqrt((v*tl*tn)/Math.PI/100 + br*br) - br) / tn;"	
			"nv =  nv / nmax * 100;"
			"return  nv;"
		"}"*/
		"function calcLen(t,maxt){"
			"var tl={TL};"
			"var br={BR};"
			"var tn={TN};"
			"var nmax={NM};"
			"var app = t/maxt*nmax;"
			"return  Math.PI*app*(app*tn + 2*br)*100/tl;"
		"}"
		"document.onload='function(){"
			"dir[0]=1;"
			"dir[1]=1;"
			"pr1.value=10;"
			"pr2.value=20;"
			//"startPrgrBar(pr1,10000,100,0);"
			//"startPrgrBar(pr2,10000,100,1);"
		"}';"
		//"{TC}"
	"</script>"
	"</body></html>";

const char HTTP_FORM_HEAD[] PROGMEM =
"<head><meta charset=\"UTF-8\"><title>Document</title>"
//"{TC}"
"<meta name='viewport' content='width=device-width, initial-scale=1.0'>"

"<style>"
"* {"
	"box-sizing: border-box;"
"}"

"[class*='col-']{"
	"padding: 0 15px;"
	"margin:0;"
"}"

"html, body, div{"
	"font-family: \"arial\", \"helvetica\", serif;"
	"font-size: 1.1rem;"
	"color: #fff;"
	"background-color: #333;"
"}"

".header {"
	"background-color: #333;"
	"color: #ffffff;"
	"padding: 15px;"
	"text-align: center;"
"}"

"#logo {"
	"text-align: center;"
"}"

"label{"
	"margin-left: 5%;"
	"color: #0099cc;"
"}"

"#form {"
	"margin-top: 20px;"
	"justify-content: center;"
	//"border 5px solid yellow;"
"}"

"input, textarea, select{"
	"width: 100%;"
	"min-height: 2.4rem;"
	"border-radius: 25px;"
	"margin: 20px 0;"
	"border: 1px solid #0099cc;"
	"font-size: 1.1rem;"
"}"

"meter {"
	"width: 100%;"
	"height:10px;"
"}"

".boxed {"
	"border-radius: 25px;"
	"border: 1px solid gray;"
	"margin: 25px 0;"
	"padding: 3% 3%;"
"}"

"input[type='submit'],input[type='button']{"
	"background-color: #333;"
	"color: #fff;"
	"max-width: 100%;"
	"min-width: 50%;"
	//"border-radius: 25px;"
	//"margin: 20px 0;"
	"font-size: 1.5rem;"
	"padding-left: 0px;"
	"min-height: 2.8rem;"
	"background-color: #00ccff;"
	"border: 3px solid #0099cc;"
	"opacity: 0.6;"
	"transition: 0.3s;"
"}"

".asidetop, .asidebottom{"
	"height: 10%;"
	"margin-top: auto;"
	"margin-bottom: auto;"
	"background-color: #333;"
"}"

".aside{"
	//"display:inline-block;"
	"margin: 18% 3%;"
	"background-color: #333;"
	"padding: 0 2%;"
	"text-align: center;"
"}"

"#temp{"
	"font-size: 3rem;"
	"background-color: #333;"
	"text-align: center;"
"}"
//
"#time{"
	"font-size: 2.6rem;"
	"background-color: #333;"
	"text-align: center;"
"}"

"#date{"
	"font-size: 2rem;"
	"background-color: #333;"
	"text-align: center;"
"}"

".footer {"
	"background-color: #333;"
	"color: #ffffff;"
	"text-align: center;"
	"font-size: 12px;"
	"padding: 15px;"
"}"

".grid-container {"
	"/* For mini: */"
	"display: grid;"
	"grid-template-columns: 1fr;"
	"grid-column-gap: 2%;"
"}"

"@media only screen and (min-width: 600px){"
	"/* For tablets: */"
	".grid-container {"
	  "grid-template-columns: 1fr 1fr 1fr 1fr 1fr 1fr 1fr 1fr 1fr 1fr 1fr 1fr;"
	"}"
	".col-s-1 {grid-column:   span 1;}"
	".col-s-2 {grid-column:   span 2;}"
	".col-s-3 {grid-column:   span 3;}"
	".col-s-4 {grid-column:   span 4;}"
	".col-s-5 {grid-column:   span 5;}"
	".col-s-6 {grid-column:   span 6;}"
	".col-s-7 {grid-column:   span 7;}"
	".col-s-8 {grid-column:   span 8;}"
	".col-s-9 {grid-column:   span 9;}"
	".col-s-10 {grid-column:  span 10;}"
	".col-s-11 {grid-column:  span 11;}"
	".col-s-12 {grid-column:  span 12;}"
"}"

"@media only screen and (min-width: 768px){"
	"/* For desktop: */"
	".grid-container {"
	  "grid-template-columns: 1fr 1fr 1fr 1fr 1fr 1fr 1fr 1fr 1fr 1fr 1fr 1fr;"
	"}"
	".col-1 {grid-column:   span 1;}"
	".col-2 {grid-column:   span 2;}"
	".col-3 {grid-column:   span 3;}"
	".col-4 {grid-column:   span 4;}"
	".col-5 {grid-column:   span 5;}"
	".col-6 {grid-column:   span 6;}"
	".col-7 {grid-column:   span 7;}"
	".col-8 {grid-column:   span 8;}"
	".col-9 {grid-column:   span 9;}"
	".col-10 {grid-column:  span 10;}"
	".col-11 {grid-column:  span 11;}"
	".col-12 {grid-column:  span 12;}"
"}"

"<script>"
	"function pushopacity(bid){"
		"var btn=document.getElementById(bid);"
		"btn.onmousedown='btn.style.opacity=\"1\"';"
		"btn.onmouseup='btn.style.opacity=\"0.6\"';"
		"btn.ontouchstart='btn.style.opacity=\"1\"';"
		"btn.ontouchend='btn.style.opacity=\"0.6\"';"
	"}"
"</script>"
"</style>"
"</head>";

const char PAHO_SRC[] PROGMEM = "<script src='https://cdnjs.cloudflare.com/ajax/libs/paho-mqtt/1.0.1/mqttws31.js' type='text/javascript'></script>";

const char HTTP_MQTT[] PROGMEM =
		// Create a client instance
		"var vls = ['{\"up1\":\"255\"}','{\"down1\":\"255\"}','{\"up2\":\"255\"}','{\"down2\":\"255\"}'];"
		"var vlsp = ['{\"up1\":\"N\"}','{\"up2\":\"N\"}'];"
		"var action = '{\"onaction\":\"D\"}';"
		"var cond = ['{\"oncond1\":\"C\"}','{\"oncond2\":\"C\"}','{\"oncond3\":\"C\"}','{\"oncond4\":\"C\"}'];"
		// Generate a random client ID
		"var clientID = '{MI}_' + parseInt(Math.random() * 100);"
		"var conn = new Paho.MQTT.Client('{MA}', Number('{WT}'), '/{PP}', clientID);"
		//"var conn = new Paho.MQTT.Client('{MA}', Number('{MT}'), clientID);"
		"console.log(conn);"
		"console.log('broker:{MA}, port:{WT}, path:{PP}, id:'+clientID);"
		// connect the client
		"conn.connect({onSuccess:onConnect});"
		"function onConnect() {"
			//"conn.send('Connect ' + new Date());"
			"console.log('onConnect');"
			"conn.subscribe('{MO}');"
		"};"
		"conn.onMessageArrived = function (e) {"
			"console.log('onMessageArrived:'+e.payloadString);"
			"onRcv(e.payloadString);"
		"};"
		"conn.onConnectionLost  = function (responseObject) {"
			"if (responseObject.errorCode !== 0) {"
				"console.log('onConnectionLost:'+responseObject.errorMessage);"
			"}"
		"};"
		"function send(str){"
		    "console.log('send: '+str);"
			"var msg = new Paho.MQTT.Message(str);"
			"msg.destinationName = '{QI}';"
			"conn.send(msg);"
		"};"
		"function actsend(str){"
			//"var btn=document.getElementById(eid);"
			"console.log('actsend: '+str);"
			"var v=action.replace('D',str);"
			"send(v);"
		"};"
		"function condsend(i,eid){"
			"var el=document.getElementById(eid);"
			"var c=cond[i].replace('C',el.value);"
			"send(c);"
			"console.log('condsend: '+c);"
		"};"
		"function confsend(i,actid,cnfid){"
			"actsend(i,actid);"
			"condsend(i,cnfid);"
		"};"
		"function press(s){"
			"send(s);"
		"};";

const char HTTP_WEBSOCKET[] PROGMEM =
		"var vls = ['{\"up1\":\"255\"}','{\"down1\":\"255\"}','{\"up2\":\"255\"}','{\"down2\":\"255\"}'];"
		"var vlsp = ['{\"up1\":\"N\"}','{\"up2\":\"N\"}'];"
		"var action = '{\"onaction\":\"D\"}';"
		"var cond = ['{\"oncond1\":\"C\"}','{\"oncond2\":\"C\"}','{\"oncond3\":\"C\"}','{\"oncond4\":\"C\"}'];"
		"var conn = new WebSocket('ws://{WS}:81/', ['arduino']);"
		"conn.onopen = function () {"
			//"conn.send('Connect ' + new Date());"
		"};"
		"conn.onerror = function (error) {"
			"console.log('WebSocket Error ', error);"
			"conn.close();"
			"conn = new WebSocket('ws://{WS}:81/', ['arduino']);"
		"};"
		"conn.onmessage = function (e) {"
			"console.log('Server: ', e.data);"
			"onRcv(e.data);"
		"};"
		"conn.onclose = function () {"
			"console.log('WebSocket connection closed');"
			"conn = new WebSocket('ws://{WS}:81/', ['arduino']);"
		"};"
		"function actsend(str){"
			//"var btn=document.getElementById(eid);"
			"console.log('actsend: '+str);"
			"var v=action.replace('D',str);"
			"conn.send(v);"
		"};"
		"function condsend(i,eid){"
			"var el=document.getElementById(eid);"
			"var c=cond[i].replace('C',el.value);"
			"conn.send(c);"
			"console.log('condsend: '+c);"
		"};"
		"function confsend(i,actid,cnfid){"
			"actsend(i,actid);"
			"condsend(i,cnfid);"
		"};"
		"function press(s){"
			"conn.send(s);"
		"};";

	//-------------------------------------------------------------------
//fine variabili globali

void handleNotFound(){
	DEBUG_PRINTLN(F("Enter handleNotFound"));
	is_authentified(serverp);
}
	
void handleRoot() {   // When URI / is requested, send a web page with a button to toggle the LED
	
	String page = FPSTR(HTTP_FORM_ROOT);
	page.replace(F("{HD}"),  FPSTR(HTTP_FORM_HEAD) );
	page.replace(F("{WU}"),  confcmdp[WEBUSR] ); 
	DEBUG_PRINTLN(F("Enter handleRoot"));
	//String header;
	if (!is_authentified(serverp)) {
	serverp.send(200, "text/html", page);
		
	}else{
		serverp.sendHeader("Location", "/login");
		serverp.sendHeader("Cache-Control", "no-cache");
		serverp.sendHeader("Set-Cookie", "ESPSESSIONID=0");
		serverp.send(301);
		
	}
}

void handleLogin() {  // If a POST request is made to URI /login
  DEBUG_PRINTLN(F("Enter handleLogin"));
  //I parametri NON devono essere modificati
  bool ok=false;

  if(serverp.hasArg("disconnect")) {
	DEBUG_PRINTLN(F("Disconnection"));
	//like head command of PHP
    serverp.sendHeader("Location", "/");
    serverp.sendHeader("Cache-Control", "no-cache");
    serverp.sendHeader("Set-Cookie", "ESPSESSIONID=0");
    serverp.send(301);
	return;
  }
  else if(is_authentified(serverp)) 
  {
#if (DEBUG)	
	DEBUG_PRINT(F("handleLogin: found cookie: "));
    String cookie = serverp.header("Cookie");
    DEBUG_PRINTLN(cookie);
#endif
	ok=true;
  }
  else if(!serverp.hasArg("webusr")||!serverp.hasArg("webpsw")||serverp.arg("webusr")==NULL||serverp.arg("webpsw")==NULL)
  { // If the POST request doesn't have webusr and webpsw data
     DEBUG_PRINTLN(F("Userneme "));
	 DEBUG_PRINTLN(serverp.arg("webusr"));
	 DEBUG_PRINTLN(F(" o webpsw "));
	 DEBUG_PRINTLN(serverp.arg("webpsw"));
	 DEBUG_PRINTLN(F(" assenti "));
	 ok=false;
  }
  else if(serverp.arg("webusr") == confcmdp[WEBUSR] && serverp.arg("webpsw") == confcmdp[WEBPSW])
  {
	DEBUG_PRINTLN(F("Login di "));
	DEBUG_PRINTLN(serverp.arg("webusr"));
	DEBUG_PRINTLN(F(" effettuato con successo "));
	ok=true;
  }else{
	DEBUG_PRINTLN(F("Userneme "));
	DEBUG_PRINTLN(serverp.arg("webusr"));
	DEBUG_PRINTLN(F(" o webpsw "));
	DEBUG_PRINTLN(serverp.arg("webusr"));
	DEBUG_PRINTLN(F(" scorretti"));
	ok=false;  
  }
  
  String page = FPSTR(HTTP_FORM_LOGIN);
	
  if(ok) { 
		// If both the webusr and the webpsw are correct
		//Head placeholders
		page.replace(F("{HD}"), FPSTR(HTTP_FORM_HEAD));
		//set cookies OK
		//DEBUG_PRINTLN(F("Scrittura cookie login "));
		//DEBUG_PRINTLN(page);
		serverp.sendHeader("Cache-Control", "no-cache");
		serverp.sendHeader("Set-Cookie", "ESPSESSIONID=1");
		serverp.send(200, "text/html", page);
  } else {   
		DEBUG_PRINTLN(F("Login non consentito "));
		serverp.sendHeader("Location", "/");
		serverp.sendHeader("Cache-Control", "no-cache");
		serverp.send(301);
  }
}

void handleWifiConf() {  // If a POST request is made to URI /login
  //I parametri NON devono essere modificati
  bool ok=false;

  DEBUG_PRINTLN(F("Enter handleWifiConf"));
  if(is_authentified(serverp)) 
  {
#if (DEBUG)	
	DEBUG_PRINT(F("handleWifiConf: found cookie: "));
    String cookie = serverp.header("Cookie");
    DEBUG_PRINTLN(cookie);
#endif
	ok=true;
  }
    
  String page = FPSTR(HTTP_FORM_WIFI);
	
  if(ok) { 
		//Head placeholders
		page.replace(F("{HD}"), FPSTR(HTTP_FORM_HEAD) );
		//Body placeholders
		page.replace(F("{S1}"), confcmdp[CLNTSSID1] );
		page.replace(F("{P1}"), confcmdp[CLNTPSW1] );
		page.replace(F("{S2}"), confcmdp[CLNTSSID2] );
		page.replace(F("{P2}"), confcmdp[CLNTPSW2] );
		page.replace(F("{AS}"), confcmdp[APPSSID] );
		page.replace(F("{AP}"), confcmdp[APPPSW] );
		//set cookies OK
		//DEBUG_PRINTLN(page);
		//DEBUG_PRINTLN(F("Scrittura cookie handleMQTTConf "));
		serverp.sendHeader("Cache-Control", "no-cache");
		serverp.sendHeader("Set-Cookie", "ESPSESSIONID=1");
		serverp.send(200, "text/html", page);
  } else {   
		DEBUG_PRINTLN(F("Login non consentito "));
		serverp.sendHeader("Location", "/");
		serverp.sendHeader("Cache-Control", "no-cache");
		serverp.send(301);
  }
}

void handleSystemConf() {  // If a POST request is made to URI /login
  //I parametri NON devono essere modificati
  bool ok=false;

  DEBUG_PRINTLN(F("Enter handleSystem"));
  if(is_authentified(serverp)) 
  {
#if (DEBUG)	
	DEBUG_PRINT(F("handleSystem: found cookie: "));
    String cookie = serverp.header("Cookie");
    DEBUG_PRINTLN(cookie);
#endif
	ok=true;
  }
    
  String page = FPSTR(HTTP_FORM_SYSTEM);
	
  if(ok) { 
		//Head placeholders
		page.replace(F("{HD}"), FPSTR(HTTP_FORM_HEAD) );
		//Body placeholders
		page.replace(F("{WU}"), confcmdp[WEBUSR]) ;
		page.replace(F("{WP}"), confcmdp[WEBPSW]);
		page.replace(F("{N1}"), confcmdp[NTPADDR1]);
		page.replace(F("{N2}"), confcmdp[NTPADDR2]);
		page.replace(F("{N3}"), confcmdp[UTCSYNC]);
		page.replace(F("{N4}"), confcmdp[UTCADJ]);
		page.replace(F("{N5}"), confcmdp[UTCZONE]);
		page.replace(F("{N6}"), ((confcmdp[UTCSDT]).toInt()==1)?"checked":"");
#if (!AUTOCAL) 
		page.replace(F("{S1}"), confcmdp[STDEL1]);
		page.replace(F("{S2}"), confcmdp[STDEL2]);
#endif
		//set cookies OK
		//DEBUG_PRINTLN(page);
		//DEBUG_PRINTLN(F("Scrittura cookie handleMQTTConf "));
		serverp.sendHeader("Cache-Control", "no-cache");
		serverp.sendHeader("Set-Cookie", "ESPSESSIONID=1");
		serverp.send(200, "text/html", page);
  } else {   
		DEBUG_PRINTLN(F("Login non consentito "));
		serverp.sendHeader("Location", "/");
		serverp.sendHeader("Cache-Control", "no-cache");
		serverp.send(301);
  }
}

void handleMQTTConf() {  // If a POST request is made to URI /login
  //I parametri NON devono essere modificati
  bool ok=false;

  DEBUG_PRINTLN(F("Enter handleMQTTConf"));
  if(is_authentified(serverp)) 
  {
#if (DEBUG)	
	DEBUG_PRINT(F("handleMQTTConf: found cookie: "));
    String cookie = serverp.header("Cookie");
    DEBUG_PRINTLN(cookie);
#endif
	ok=true;
  }
    
  String page = FPSTR(HTTP_FORM_MQTT);
	
  if(ok) { 
		//Head placeholders
		page.replace(F("{HD}"), FPSTR(HTTP_FORM_HEAD));
		//Body placeholders
		page.replace(F("{MA}"), confcmdp[MQTTADDR]);
		page.replace(F("{MT}"), confcmdp[MQTTPORT]);
		page.replace(F("{WT}"), confcmdp[WSPORT]);
		page.replace(F("{MU}"), confcmdp[MQTTUSR]);
		page.replace(F("{MP}"), confcmdp[MQTTPSW]);
		page.replace(F("{MI}"), confcmdp[MQTTID]);
		page.replace(F("{MO}"), confcmdp[MQTTOUTTOPIC]);
		page.replace(F("{QI}"), confcmdp[MQTTINTOPIC]);
		page.replace(F("{J1}"), mqttJsonp[MQTTUP1]);
		page.replace(F("{J2}"), mqttJsonp[MQTTDOWN1]);
		page.replace(F("{J3}"), mqttJsonp[MQTTUP2]);
		page.replace(F("{J4}"), mqttJsonp[MQTTDOWN2]);
		page.replace(F("{PP}"), confcmdp[MQTTPROTO]);
		//page.replace(F("{P1}"), (confcmdp[MQTTPROTO]=="ws")?"selected":"");
		//page.replace(F("{P2}"), (confcmdp[MQTTPROTO]=="tcp")?"selected":"");
		//page.replace(F("{J5}"), mqttJsonp[MQTTTEMP]);
		//page.replace(F("{J6}"), mqttJsonp[MQTTMEANPWR]);
		//page.replace(F("{J7}"), mqttJsonp[MQTTPEAKPWR]);
		//page.replace(F("{J8}"), mqttJsonp[MQTTALL]);
		//set cookies OK
		//DEBUG_PRINTLN(page);
		//DEBUG_PRINTLN(F("Scrittura cookie handleMQTTConf "));
		serverp.sendHeader("Cache-Control", "no-cache");
		serverp.sendHeader("Set-Cookie", "ESPSESSIONID=1");
		serverp.send(200, "text/html", page);
  } else {   
		DEBUG_PRINTLN(F("Login non consentito "));
		serverp.sendHeader("Location", "/");
		serverp.sendHeader("Cache-Control", "no-cache");
		serverp.send(301);
  }
}

void handleLogicConf() {  // If a POST request is made to URI /login
  //I parametri NON devono essere modificati
  bool ok=false;

  DEBUG_PRINTLN(F("Enter handleLogicConf"));
  if(is_authentified(serverp)) 
  {
#if (DEBUG)	
	DEBUG_PRINT(F("handleLogicConf: found cookie: "));
    String cookie = serverp.header("Cookie");
    DEBUG_PRINTLN(cookie);
#endif
	ok=true;
  }
    
  String page = FPSTR(HTTP_FORM_LOGIC);
  
  if(ok) { 
		//Head placeholders
		page.replace(F("{HD}"), FPSTR(HTTP_FORM_HEAD) );
		//Body placeholders
		page.replace(F("{TU}"), confcmdp[THALT1]);
		page.replace(F("{TD}"), confcmdp[THALT2]);
		page.replace(F("{DU}"), confcmdp[THALT3]);
		page.replace(F("{DD}"), confcmdp[THALT4]);
		if(confcmdp[SWROLL1]=="0"){
			page.replace(F("{H3}"), "disabled");
		}else{
			page.replace(F("{H3}"), "");
		}
		if(confcmdp[SWROLL2]=="0"){
			page.replace(F("{H4}"), "disabled");
		}else{
			page.replace(F("{H4}"), "");
		}
		page.replace(F("{TL}"), confcmdp[TLENGTH]);
		page.replace(F("{BR}"), confcmdp[BARRELRAD]);
		page.replace(F("{TN}"), confcmdp[THICKNESS]);
		page.replace(F("{SR}"), confcmdp[SLATSRATIO]);
		page.replace(F("{H1}"), ((confcmdp[SWROLL1]).toInt()==0)?"checked":"");
		page.replace(F("{H2}"), ((confcmdp[SWROLL2]).toInt()==0)?"checked":"");
		//set cookies OK
		//DEBUG_PRINTLN(page);
		//DEBUG_PRINTLN(F("Scrittura cookie handleMQTTConf "));
		serverp.sendHeader("Cache-Control", "no-cache");
		serverp.sendHeader("Set-Cookie", "ESPSESSIONID=1");
		serverp.send(200, "text/html", page);
  } else {   
		DEBUG_PRINTLN(F("Login non consentito "));
		serverp.sendHeader("Location", "/");
		serverp.sendHeader("Cache-Control", "no-cache");
		serverp.send(301);
  }
}

//EVENT MANAGEMENT
void handleEventConf() {  // If a POST request is made to URI /login
  //I parametri NON devono essere modificati
  bool ok=false;

  DEBUG_PRINTLN(F("Enter handleEventConf"));
  if(is_authentified(serverp)) 
  {
#if (DEBUG)	
	DEBUG_PRINT(F("handleLogicConf: found cookie: "));
    String cookie = serverp.header("Cookie");
    DEBUG_PRINTLN(cookie);
#endif
	ok=true;
  }
    
  String page = FPSTR(HTTP_FORM_EVENT);
  String act = "";
  
  if(ok) { 
		//Head placeholders
		page.replace(F("{HD}"), FPSTR(HTTP_FORM_HEAD));
		page.replace(F("{SH}"), FPSTR(HTTP_WEBSOCKET) );
		page.replace(F("{WS}"), confcmdp[LOCALIP]);
		//Body placeholders
		//page.replace(F("{A1}"), confcmdp[SWACTION1]);
		//page.replace(F("{A2}"), confcmdp[SWACTION2]);
		//page.replace(F("{A3}"), confcmdp[SWACTION3]);
		//page.replace(F("{A4}"), confcmdp[SWACTION4]);
		page.replace(F("{C1}"), confcmdp[ONCOND1]);
		page.replace(F("{C2}"), confcmdp[ONCOND2]);
		page.replace(F("{C3}"), confcmdp[ONCOND3]);
		page.replace(F("{C4}"), confcmdp[ONCOND4]);
		page.replace(F("{AC}"), confcmdp[ACTIONEVAL]);
		page.replace(F("{AD}"), confcmdp[ONCOND5]);
		page.replace(F("{D1}"), confcmdp[THALT1]);
		page.replace(F("{D2}"), confcmdp[THALT2]);
		page.replace(F("{D3}"), confcmdp[THALT3]);
		page.replace(F("{D4}"), confcmdp[THALT4]);
		page.replace(F("{S1}"), String(getCntValue(1)));
		page.replace(F("{S2}"), String(getCntValue(2)));
		page.replace(F("{S3}"), String(getCntValue(3)));
		page.replace(F("{S4}"), String(getCntValue(4)));
		if(confcmdp[SWROLL1]=="0"){
			page.replace(F("{V1}"), "style=\"display:block\"");
		}else{
			page.replace(F("{V1}"), "style=\"display:none\"");
		}
		if(confcmdp[SWROLL2]=="0"){
			page.replace(F("{V2}"), "style=\"display:block\"");
		}else{
			page.replace(F("{V2}"), "style=\"display:none\"");
		}
		/*if(rollmode[0]==1){
			page.replace(F("{H3}"), "disabled");
		}else{
			page.replace(F("{H3}"), "");
		}
		if(rollmode[1]==1){
			page.replace(F("{H4}"), "disabled");
		}else{
			page.replace(F("{H4}"), "");
		}*/
		//set cookies OK
		//DEBUG_PRINTLN(page);
		//DEBUG_PRINTLN(F("Scrittura cookie handleMQTTConf "));
		serverp.sendHeader("Cache-Control", "no-cache");
		serverp.sendHeader("Set-Cookie", "ESPSESSIONID=1");
		serverp.send(200, "text/html", page);
  } else {   
		DEBUG_PRINTLN(F("Login non consentito "));
		serverp.sendHeader("Location", "/");
		serverp.sendHeader("Cache-Control", "no-cache");
		serverp.send(301);
  }
}
//END EVENT MANAGEMENT*/

void handleCmd() {  // If a POST request is made to URI /login

	String page = FPSTR(HTTP_FORM_CMD);
	//Head placeholders
	page.replace(F("{HD}"), FPSTR(HTTP_FORM_HEAD));
	page.replace(F("{SH}"), FPSTR(HTTP_WEBSOCKET));
	page.replace(F("{WS}"), confcmdp[LOCALIP]);
	page.replace(F("{TL}"), String(getTaplen()));
	page.replace(F("{BR}"), confcmdp[BARRELRAD]);
	page.replace(F("{TN}"), confcmdp[THICKNESS]);
	page.replace(F("{SR}"), confcmdp[SLATSRATIO]);
	page.replace(F("{NM}"), String(getNmax()));
	page.replace(F("{PD}"), String(round(getPosdelta())));
	page.replace(F("{TP}"), mqttJsonp[MQTTTEMP]);
	page.replace(F("{DT}"), mqttJsonp[MQTTDATE]);
	//Body placeholders
	//DEBUG_PRINTLN(page);
	//DEBUG_PRINTLN(F("Scrittura cookie handleMQTTConf "));
	serverp.sendHeader("Cache-Control", "no-cache");
	serverp.sendHeader("Set-Cookie", "ESPSESSIONID=0");
	serverp.send(200, "text/html", page);
}

void handleMqttCmd() {  // If a POST request is made to URI /login

	String page = FPSTR(HTTP_FORM_CMD);
	//Head placeholders
	page.replace(F("{HD}"), FPSTR(HTTP_FORM_HEAD));
	page.replace(F("{SH}"), FPSTR(HTTP_MQTT));
	//page.replace(F("{TC}"), FPSTR(MQTT_CLIENT));PAHO_SRC
	page.replace(F("{SP}"), FPSTR(PAHO_SRC));
	
	page.replace(F("{MA}"), confcmdp[MQTTADDR]);
	page.replace(F("{MT}"), confcmdp[MQTTPORT]);
	page.replace(F("{WT}"), confcmdp[WSPORT]);
	page.replace(F("{MU}"), confcmdp[MQTTUSR]);
	page.replace(F("{MP}"), confcmdp[MQTTPSW]);
	page.replace(F("{MI}"), confcmdp[MQTTID]);
	page.replace(F("{MO}"), confcmdp[MQTTOUTTOPIC]);
	page.replace(F("{QI}"), confcmdp[MQTTINTOPIC]);
	page.replace(F("{PP}"), confcmdp[MQTTPROTO]);

	page.replace(F("{TL}"), String(getTaplen()));
	page.replace(F("{BR}"), confcmdp[BARRELRAD]);
	page.replace(F("{TN}"), confcmdp[THICKNESS]);
	page.replace(F("{SR}"), confcmdp[SLATSRATIO]);
	page.replace(F("{NM}"), String(getNmax()));
	page.replace(F("{PD}"), String(round(getPosdelta())));
	page.replace(F("{TP}"), mqttJsonp[MQTTTEMP]);
	page.replace(F("{DT}"), mqttJsonp[MQTTDATE]);

	//Body placeholders
	//DEBUG_PRINTLN(page);
	//DEBUG_PRINTLN(F("Scrittura cookie handleMQTTConf "));
	serverp.sendHeader("Cache-Control", "no-cache");
	serverp.sendHeader("Set-Cookie", "ESPSESSIONID=0");
	serverp.sendHeader("Connection", "close");
	serverp.send(200, "text/html", page);
}

inline void savegroup(byte fields[], byte len){
	for(int i=0; i<len; i++){
		saveParamFromForm(fields[i]);
	}
}

void handleModify(){
  
  if (!is_authentified(serverp)) {
		serverp.sendHeader("Location", "/");
		serverp.sendHeader("Cache-Control", "no-cache");
		serverp.send(301);
		return;
  }
  
  eepromBegin();
  if(serverp.hasArg("svwifi")){
	  DEBUG_PRINTLN(F("savegroup svwifi"));
	  byte fields[6] ={p(APPSSID), p(APPPSW), p(CLNTSSID1), p(CLNTPSW1), p(CLNTSSID2), p(CLNTPSW2)};
	  savegroup(fields, 6);
  }else if(serverp.hasArg("svmqtt")){
	  DEBUG_PRINTLN(F("savegroup svmqtt"));
	  byte fields[9] ={p(MQTTADDR), p(MQTTID), p(MQTTOUTTOPIC), p(MQTTINTOPIC), p(MQTTUSR), p(MQTTPSW), p(MQTTPORT), p(WSPORT), p(MQTTPROTO)};
	  savegroup(fields, 9);
  }else if(serverp.hasArg("svsystem")){
	  DEBUG_PRINTLN(F("savegroup svsystem"));
	  byte fields[6] ={p(WEBUSR), p(WEBPSW), p(UTCSYNC), p(UTCADJ), p(UTCSDT), p(UTCZONE)};
	  savegroup(fields, 6);
	  
	  if( serverp.hasArg("rebootd") && String("y") == serverp.arg("rebootd") ){
		rebootSystem();
	  }
	  if( serverp.hasArg("reboot") && String("y") == serverp.arg("reboot") ){
		DEBUG_PRINTLN(F("Rebooting ESP"));
		ESP.restart();
	  }  
	  if( serverp.hasArg("utcsdt") && String("y") == serverp.arg("utcsdt") ){
		setSDT(true);
		updtConf(UTCSDT, String(1));
	  }else{
		setSDT(false);
		updtConf(UTCSDT, String(0));
	  }
  }else if(serverp.hasArg("svlogic")){
	  DEBUG_PRINTLN(F("savegroup svlogic"));
	  byte fields[11] ={p(THALT1), p(THALT2), p(THALT3), p(THALT4), p(STDEL1), p(STDEL2), p(VALWEIGHT), p(TLENGTH), p(BARRELRAD), p(THICKNESS), p(SLATSRATIO)};
	  savegroup(fields, 11);
	  
	  if( serverp.hasArg("swroll1") && String("1") == serverp.arg("swroll1") ){
		//writeSWMode(0,0); 
		updtConf(SWROLL1OFST, String(0));
		setSWMode(0,0); 
	  }else{
		//writeSWMode(1,0);
		updtConf(SWROLL1OFST, String(1));
		setSWMode(1,0);	
	  }
	  if( serverp.hasArg("swroll2") && String("1") == serverp.arg("swroll2") ){
		//writeSWMode(0,1);
		setSWMode(0,1);
		updtConf(SWROLL1OFST+1, String(1));
	  }else{
		//writeSWMode(1,1);
		updtConf(SWROLL1OFST+1, String(0));
		setSWMode(1,1);	
	  }
  }
  EEPROM.end();
  
  DEBUG_PRINTLN(F("Disconnection"));
  
  serverp.sendHeader("Cache-Control", "no-cache");
  serverp.sendHeader("Set-Cookie", "ESPSESSIONID=1");
  
  String page = FPSTR(HTTP_FORM_SUCCESS);
  page.replace(F("{HD}"), FPSTR(HTTP_FORM_HEAD) );
  serverp.send(200, "text/html", page);
		
  if(confcmdp[TIMINGCHANGED]=="true"){
	confcmdp[TIMINGCHANGED]=="false";
	initIiming(false);
  } 
}

void loadConfig() {
	char buf[250]="";
	int i;
    EEPROM.begin(THALT1OFST);
	delay(10);
	//DEBUG_PRINTln();
	DEBUG_PRINTLN();
	DEBUG_PRINT(F("Startup string: "));
	bool cfg = validateEEPROM();
	EEPROMReadStr(0,buf,2);
	varStrOfst[0] = FIXEDPARAMSLEN;  		 		//l'offset del primo variabile parte dalla fine dell'ultimo fisso
	varStrOfst[5] = EEPROMReadInt(EEPROMLENOFST);//la fine dell'ultimo variabile (offset+lunghezza campo) è conservato nella posizione EEPROMLENOFST 
	EEPROM.end();
	DEBUG_PRINTLN(buf);
	delay(1000);

	if(!cfg) {
		DEBUG_PRINTLN(F("EEPROM read error! EEPROM will be initialized, you must load a new valid system configuration."));
		EEPROM.begin(FIXEDPARAMSLEN);
		initEEPROM(FIXEDPARAMSLEN); 
		EEPROM.end();
		varStrOfst[0] = varStrOfst[1] = varStrOfst[2] = varStrOfst[3] = varStrOfst[4] = varStrOfst[5] = FIXEDPARAMSLEN;
		DEBUG_PRINT(F("FIXEDPARAMSLEN "));
		DEBUG_PRINTLN(FIXEDPARAMSLEN);
		saveOnEEPROM(FIXEDPARAMSLEN);
		confcmdp[CONFLOADED]="false";
		delay(1000);
	} else {
		EEPROM.begin(THALT1OFST);//the next after EEPROMLENOFST
		int eepromlen = EEPROMReadInt(EEPROMLENOFST);
		EEPROM.end();
		EEPROM.begin(eepromlen+1);
		DEBUG_PRINT(F("eepromlen "));
		DEBUG_PRINTLN(eepromlen);
		
		DEBUG_PRINTLN(F("Reading all fixed params... "));
		for(int i=0; i<PARAMSDIM; i++){
			loadConf(i);
		}
		
		DEBUG_PRINT(F("Reading all variables params... "));
		varStrOfst[0] = FIXEDPARAMSLEN;
		for(i=0; i<VARCONFDIM; ++i){
				varStrOfst[i+1] = EEPROMReadStr(varStrOfst[i], buf) + varStrOfst[i];
				confcmdp[i]=buf;
				DEBUG_PRINT(F("sensors CONFEXPR "));
				DEBUG_PRINT(i);
				DEBUG_PRINT(F(": "));
				DEBUG_PRINTLN(confcmdp[i]);
		}
			
		confcmdp[CONFLOADED]="true";
		EEPROM.end();
		DEBUG_PRINTLN(F("EEPROM configuration readed"));
	}
}		
		
//Check if header is present and correct
bool is_authentified(ESP8266WebServer &server){
#if (DEBUG)	 
  DEBUG_PRINTLN(F("Enter is_authentified"));  
  String message = "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  DEBUG_PRINTLN(message);
  
  message = "HEADERS: ";
  message += server.headers();
  message += "\n";
  for (uint8_t i = 0; i < server.headers(); i++) {
    message += " " + server.headerName(i) + ": " + server.header(i) + "\n";
  }
  DEBUG_PRINTLN(message); 
#endif
  
  if (server.hasHeader("Cookie")) {
    DEBUG_PRINTLN(F("Found cookie: "));
    DEBUG_PRINTLN(server.header("Cookie"));
    if ((server.header("Cookie")).indexOf("ESPSESSIONID=1") != -1) {
      DEBUG_PRINTLN(F("Authentification Successful"));
      return true;
    }
  }
  DEBUG_PRINTLN(F("Authentification Failed"));
  return false;
}

void saveOnEEPROM(int len){
	int i;
	char eprom, form;
	
	//I parametri devono poter essere modificati
	DEBUG_PRINTLN(F("Saving configuration...."));
  
	EEPROM.begin(len);	
	for(int i=0; i<PARAMSDIM; i++){
		saveConf(i);
	}
	EEPROM.end();
	
	DEBUG_PRINTLN(F("Fixed length params saved...."));
	writeOnOffConditions();
	DEBUG_PRINTLN(F("Variable length params saved...."));
}
	
void printConfig(){
		int i;
		
		DEBUG_PRINT(F("\nPrinting EEPROM configuration...."));
		
		for(i=0; i<VARCONFDIM; ++i){
			DEBUG_PRINT(F("\nsensors CONFEXPR "));
			DEBUG_PRINT(i);
			DEBUG_PRINT(F(": "));
			DEBUG_PRINTLN(confcmdp[i]);
			DEBUG_PRINT(F("- len "));
			DEBUG_PRINT(i);
			DEBUG_PRINT(F(": "));
			DEBUG_PRINT(varStrOfst[i]);
		}
		
		for(int i=0; i<PARAMSDIM; i++){
			printFixedParam(i);
		}

		DEBUG_PRINT(F("\neeprom EEPROMLENOFST: "));
		DEBUG_PRINT(varStrOfst[VARCONFDIM]);
}		

void saveSingleParam(unsigned paramofst){
	eepromBegin();
	saveConf(paramofst);
	EEPROM.end();
}

void updtConf(unsigned paramofst, String val){
	char partype;
	unsigned eepromofst;
	char* param;
	
	DEBUG_PRINT(F("paramofst: "));
	DEBUG_PRINTLN(paramofst);
	if(parsp[paramofst] != NULL){
		param = parsp[paramofst]->parname;
		DEBUG_PRINT(F("param: "));
		DEBUG_PRINTLN(param);
		eepromofst = parsp[paramofst]->eprom;
		DEBUG_PRINT(F("eepromofst: "));
		DEBUG_PRINTLN(eepromofst);
		partype = parsp[paramofst]->partype;
		DEBUG_PRINT(F("partype: "));
		DEBUG_PRINTLN(partype);
		unsigned confofst = getConfofstFromParamofst(paramofst);
		DEBUG_PRINT(F("confofst: "));
		DEBUG_PRINTLN(confofst);
		
		if(partype == 'p'){
			confcmdp[confofst] = val;
			parsp[paramofst]->writeParam(confcmdp[confofst]);
			
			DEBUG_PRINT(F("Modified param: "));
			DEBUG_PRINT(param);
			DEBUG_PRINT(F(": "));
			DEBUG_PRINTLN(confcmdp[paramofst]);	
		}else if(partype == 'j'){
			mqttJsonp[confofst] = val; 
			EEPROMWriteStr(eepromofst,(mqttJsonp[confofst]).c_str(),32);
			
			DEBUG_PRINT(F("Modified json: "));
			DEBUG_PRINT(param);
			DEBUG_PRINT(F(": "));
			DEBUG_PRINTLN(mqttJsonp[confofst]);	
		}
		DEBUG_PRINTLN(F("---------------------------------"));
	}
}

void saveConf(unsigned paramofst){
	char partype;
	unsigned eepromofst;
	char* param;
	
	DEBUG_PRINT(F("paramofst: "));
	DEBUG_PRINTLN(paramofst);
	if(parsp[paramofst] != NULL){
		param = parsp[paramofst]->parname;
		DEBUG_PRINT(F("param: "));
		DEBUG_PRINTLN(param);
		eepromofst = parsp[paramofst]->eprom;
		DEBUG_PRINT(F("eepromofst: "));
		DEBUG_PRINTLN(eepromofst);
		partype = parsp[paramofst]->partype;
		DEBUG_PRINT(F("partype: "));
		DEBUG_PRINTLN(partype);
		unsigned confofst = getConfofstFromParamofst(paramofst);
		DEBUG_PRINT(F("confofst: "));
		DEBUG_PRINTLN(confofst);
		
		if(partype == 'p'){
			parsp[paramofst]->writeParam(confcmdp[confofst]);
			
			DEBUG_PRINT(F("Modified param: "));
			DEBUG_PRINT(param);
			DEBUG_PRINT(F(": "));
			DEBUG_PRINTLN(confcmdp[confofst]);	
		}else if(partype == 'j'){	
			EEPROMWriteStr(eepromofst,(mqttJsonp[confofst]).c_str(),32);
			
			DEBUG_PRINT(F("Modified json: "));
			DEBUG_PRINT(param);
			DEBUG_PRINT(F(": "));
			DEBUG_PRINTLN(mqttJsonp[confofst]);	
		}
		DEBUG_PRINTLN(F("---------------------------------"));
	}
}

void loadConf(unsigned paramofst){
	char partype;
	unsigned eepromofst;
	char buf[50];
	char* param;
	
	DEBUG_PRINT(F("paramofst: "));
	DEBUG_PRINTLN(paramofst);
	if(parsp[paramofst] != NULL){
		param = parsp[paramofst]->parname;
		DEBUG_PRINT(F("param: "));
		DEBUG_PRINTLN(param);
		eepromofst = parsp[paramofst]->eprom;
		DEBUG_PRINT(F("eepromofst: "));
		DEBUG_PRINTLN(eepromofst);
		partype = parsp[paramofst]->partype;
		DEBUG_PRINT(F("partype: "));
		DEBUG_PRINTLN(partype);
		unsigned confofst = getConfofstFromParamofst(paramofst);
		DEBUG_PRINT(F("confofst: "));
		DEBUG_PRINTLN(confofst);
		
		if(partype == 'p'){
			confcmdp[confofst] = parsp[paramofst]->getParam();
			
			DEBUG_PRINT(F("Load param: "));
			DEBUG_PRINT(param);
			DEBUG_PRINT(F(": "));
			DEBUG_PRINTLN(confcmdp[confofst]);
		}else if(partype == 'j'){	
			EEPROMReadStr(eepromofst,buf);
			mqttJsonp[confofst] = buf;	
				
			DEBUG_PRINT(F("Load json: "));
			DEBUG_PRINT(param);
			DEBUG_PRINT(F(": "));
			DEBUG_PRINTLN(mqttJsonp[confofst]);	
		}
		DEBUG_PRINTLN(F("---------------------------------"));
	}
}

void printFixedParam(unsigned paramofst){
	char eprom, partype;
	unsigned eepromofst;
	char* param;
	
	DEBUG_PRINT(F("paramofst: "));
	DEBUG_PRINTLN(paramofst);
	if(parsp[paramofst] != NULL){
		param = parsp[paramofst]->parname;
		DEBUG_PRINT(F("param: "));
		DEBUG_PRINTLN(param);
		eepromofst = parsp[paramofst]->eprom;
		DEBUG_PRINT(F("eepromofst: "));
		DEBUG_PRINTLN(eepromofst);
		partype = parsp[paramofst]->partype;
		DEBUG_PRINT(F("partype: "));
		DEBUG_PRINTLN(partype);
		unsigned confofst = getConfofstFromParamofst(paramofst);
		DEBUG_PRINT(F("confofst: "));
		DEBUG_PRINTLN(confofst);
		
		if(partype == 'p'){
			if(eprom != 'v'){
				DEBUG_PRINT(F("\nsensors CONFEXPR "));
				DEBUG_PRINT(param);
				DEBUG_PRINT(F(": "));
				DEBUG_PRINTLN(confcmdp[confofst]);
			}	
		}else if(partype == 'j'){	
			DEBUG_PRINT(F("json: "));
			DEBUG_PRINTLN(param);
			DEBUG_PRINT(F(" : "));
			DEBUG_PRINTLN(mqttJsonp[confofst]);	
		}
		DEBUG_PRINTLN(F("---------------------------------"));
	}
}

void saveParamFromForm(unsigned paramofst){
	char buf[50];
	char partype;
	char intype;
	char* param;
	unsigned eepromofst;
	
	DEBUG_PRINT(F("paramofst: "));
	DEBUG_PRINTLN(paramofst);
	if(parsp[paramofst] != NULL){
		param = parsp[paramofst]->parname;
		DEBUG_PRINT(F("param: "));
		DEBUG_PRINTLN(param);
		eepromofst = parsp[paramofst]->eprom;
		DEBUG_PRINT(F("eepromofst: "));
		DEBUG_PRINTLN(eepromofst);
		partype = parsp[paramofst]->partype;
		DEBUG_PRINT(F("partype: "));
		DEBUG_PRINTLN(partype);
		unsigned confofst = getConfofstFromParamofst(paramofst);
		DEBUG_PRINT(F("confofst: "));
		DEBUG_PRINTLN(confofst);
		intype = parsp[paramofst]->formfield;
		DEBUG_PRINT(F("formfield: "));
		DEBUG_PRINTLN(intype);
		
		if(intype == 'i'){//form element input or text area
			if(partype == 'p'){
				if(serverp.hasArg(param) && (confcmdp[confofst] != String(serverp.arg(param))) ){
					confcmdp[confofst] = serverp.arg(param);
					parsp[paramofst]->writeParam(confcmdp[confofst]);
					
					DEBUG_PRINT(F("Updated param: "));
					DEBUG_PRINT(param);
					DEBUG_PRINT(F(" : "));
					DEBUG_PRINTLN(confcmdp[confofst]);
				}				
			}else if(partype == 'j'){
				if(serverp.hasArg(param) && (mqttJsonp[confofst] != String(serverp.arg(param))) ){
					mqttJsonp[confofst] = serverp.arg(param);
					parsp[paramofst]->writeParam(mqttJsonp[confofst]);
					
					DEBUG_PRINT(F("Updated param: "));
					DEBUG_PRINT(param);
					DEBUG_PRINT(F(" : "));
					DEBUG_PRINTLN(mqttJsonp[confofst]);
				}
			}
		}//form element input or text area		
	}
	DEBUG_PRINTLN(F("---------------------------------"));
}

//conservata in eeprom, acqu,isita in form, sia nome che valore campo MQTT
Par::Par(const char* pname, unsigned epromofst, char ptype, char ffield){
	parname = (char *) pname;
	eprom = epromofst;
	formfield = ffield;
	partype = ptype;
}

String Par::getParam(){
	return  "";
}

void Par::writeParam(String str){}

String ParByte::getParam(){
	return  String((byte)EEPROM.read(eprom));
}

void ParByte::writeParam(String str){
	byte app = str.toInt();
	EEPROM.write(eprom, app);
}

String ParInt::getParam(){
	return  String(EEPROMReadInt(eprom));
}

void ParInt::writeParam(String str){
	EEPROMWriteInt(eprom, str.toInt());
}

String ParLong::getParam(){
	return  String(EEPROMReadLong(eprom));
}

void ParLong::writeParam(String str){
	EEPROMWriteLong(eprom,strtoul(str.c_str(), NULL, 10));
}

String ParFloat::getParam(){
	return  String(EEPROMReadFloat(eprom));
}

void ParFloat::writeParam(String str){
	EEPROMWriteFloat(eprom,str.toFloat());
}

String ParStr32::getParam(){
	char buf[32];
	EEPROMReadStr(eprom, buf,32);
	return  String(buf);
}

void ParStr32::writeParam(String str){
	EEPROMWriteStr(eprom,str.c_str(),32);
}

String ParStr64::getParam(){
	char buf[64];
	EEPROMReadStr(eprom, buf,64);
	return  String(buf);
}

void ParStr64::writeParam(String str){
	EEPROMWriteStr(eprom,str.c_str(),64);
}

void writeOnOffConditions(){
	int i;
	
	DEBUG_PRINTLN(F("writeOnOffConditions"));
	eepromBegin();
	varStrOfst[0] = FIXEDPARAMSLEN;
	for(i=0; i<VARCONFDIM; ++i){
		varStrOfst[i+1] = EEPROMWriteStr(varStrOfst[i], (confcmdp[i]).c_str()) + varStrOfst[i];
		DEBUG_PRINT(F("Modified sensors CONFEXPR "));
		DEBUG_PRINT(i);
		DEBUG_PRINT(F(": "));
		DEBUG_PRINTLN(confcmdp[i]);
	}
	EEPROMWriteInt(EEPROMLENOFST,varStrOfst[VARCONFDIM]);
	DEBUG_PRINT(F("Modified sensors EEPROMLENOFST: "));
	DEBUG_PRINTLN(varStrOfst[i]);
	EEPROM.end();
}

byte saveByteConf(unsigned confofst){
	saveConf(confofst + USRMODIFICABLEFLAGS);
	return (byte) (confcmdp[confofst]).toInt();
}

int saveIntConf(unsigned confofst){
	saveConf(confofst + USRMODIFICABLEFLAGS);
	return (confcmdp[confofst]).toInt();
}

long saveLongConf( unsigned confofst){
	saveConf(confofst + USRMODIFICABLEFLAGS);
	return strtoul((confcmdp[confofst]).c_str(), NULL, 10);
}

float saveFloatConf(unsigned confofst){
	saveConf(confofst + USRMODIFICABLEFLAGS);
	return (confcmdp[confofst]).toFloat();
}

