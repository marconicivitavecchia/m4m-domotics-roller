(function(json) {
	var data = JSON.parse(json);
	return data.gpioSLed === "1" ? "ON" : "OFF";
})(input);