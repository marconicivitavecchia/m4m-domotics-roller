(function(json) {
	var data = JSON.parse(json);
	return data.up1 === "1" ? "ON" : "OFF";
})(input);