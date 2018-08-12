(function(json) {
	var data = JSON.parse(json);
	return data.down2 === "1" ? "OPEN" : "CLOSED";
})(input);


