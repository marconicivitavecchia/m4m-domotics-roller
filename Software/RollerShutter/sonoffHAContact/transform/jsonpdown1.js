(function(json) {
	var data = JSON.parse(json);
	return data.down1 === "1" ? "OPEN" : "CLOSED";
})(input);