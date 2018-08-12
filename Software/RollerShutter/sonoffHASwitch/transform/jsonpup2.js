(function(json) {
	var data = JSON.parse(json);
	return data.up2 === "1" ? "ON" : "OFF";
})(input);