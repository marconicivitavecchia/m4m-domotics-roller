(function(value) {
	var v = (value === "OPEN") ? "1" : "0";
	return "{\"gpioSLed\":\""+ v + "\"}";
})(input);