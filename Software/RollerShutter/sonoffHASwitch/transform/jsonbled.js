(function(value) {
	var v = (value === "ON") ? "1" : "0";
	return "{\"gpioSLed\":\""+ v + "\"}";
})(input);