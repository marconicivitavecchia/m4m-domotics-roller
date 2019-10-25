<!-- const char HTTP_FORM_HEAD[] PROGMEM = -->

<!-- Remove " (work on Visual Studio Code) -->
<!-- search: ^([ \t]*)"(.*)$ -->
<!-- search: $1$2 -->

<!-- Remove "; at EOF -->

<head><meta charset=\"UTF-8\"><title>Document</title>
<!-- {TC} -->
<meta name='viewport' content='width=device-width, initial-scale=1.0'>
<style>
* {
	box-sizing: border-box;
}
[class*='col-']{
	padding: 0 15px;
	margin:0;
}
html, body, div{
	font-family: "arial\, helvetica, serif";
	font-size: 1.1rem;
	color: #fff;
	background-color: #333;
}
.header {
	background-color: #333;
	color: #ffffff;
	padding: 15px;
	text-align: center;
}
#logo {
	text-align: center;
}
label{
	margin-left: 5%;
	color: #0099cc;
}
#form {
	margin-top: 20px;
	justify-content: center;
	/*border 5px solid yellow;*/
}
input, textarea, select{
	width: 100%;
	min-height: 2.4rem;
	border-radius: 25px;
	margin: 20px 0;
	border: 1px solid #0099cc;
	font-size: 1.1rem;
}
meter {
	width: 100%;
	height:10px;
}
.boxed {
	border-radius: 25px;
	border: 1px solid gray;
	margin: 25px 0;
	padding: 3% 3%;
}
input[type='submit'],input[type='button']{
	background-color: #333;
	color: #fff;
	max-width: 100%;
	min-width: 50%;
	/*"border-radius: 25px;
	"margin: 20px 0;*/
	font-size: 1.5rem;
	padding-left: 0px;
	min-height: 2.8rem;
	background-color: #00ccff;
	border: 3px solid #0099cc;
	opacity: 0.6;
	transition: 0.3s;
}
.asidetop, .asidebottom{
	height: 10%;
	margin-top: auto;
	margin-bottom: auto;
	background-color: #333;
}
.aside{
	margin: 18% 3%;
	background-color: #333;
	padding: 0 2%;
	text-align: center;
}

#temp{
	font-size: 3rem;
	background-color: #333;
	text-align: center;
}
#time{
	font-size: 2.6rem;
	background-color: #333;
	text-align: center;
}
#date{
	font-size: 2rem;
	background-color: #333;
	text-align: center;
}

.footer {
	background-color: #333;
	color: #ffffff;
	text-align: center;
	font-size: 12px;
	padding: 15px;
}

.grid-container {
	/* For mini: */
	display: grid;
	grid-template-columns: 1fr;
	grid-column-gap: 2%;
}
@media only screen and (min-width: 600px){
	/* For tablets: */
	.grid-container {
	  grid-template-columns: 1fr 1fr 1fr 1fr 1fr 1fr 1fr 1fr 1fr 1fr 1fr 1fr;
	}
	.col-s-1 {grid-column:   span 1;}
	.col-s-2 {grid-column:   span 2;}
	.col-s-3 {grid-column:   span 3;}
	.col-s-4 {grid-column:   span 4;}
	.col-s-5 {grid-column:   span 5;}
	.col-s-6 {grid-column:   span 6;}
	.col-s-7 {grid-column:   span 7;}
	.col-s-8 {grid-column:   span 8;}
	.col-s-9 {grid-column:   span 9;}
	.col-s-10 {grid-column:  span 10;}
	.col-s-11 {grid-column:  span 11;}
	.col-s-12 {grid-column:  span 12;}
}
@media only screen and (min-width: 768px){
	/* For desktop: */
	.grid-container {
	  grid-template-columns: 1fr 1fr 1fr 1fr 1fr 1fr 1fr 1fr 1fr 1fr 1fr 1fr;
	}
	.col-1 {grid-column:   span 1;}
	.col-2 {grid-column:   span 2;}
	.col-3 {grid-column:   span 3;}
	.col-4 {grid-column:   span 4;}
	.col-5 {grid-column:   span 5;}
	.col-6 {grid-column:   span 6;}
	.col-7 {grid-column:   span 7;}
	.col-8 {grid-column:   span 8;}
	.col-9 {grid-column:   span 9;}
	.col-10 {grid-column:  span 10;}
	.col-11 {grid-column:  span 11;}
	.col-12 {grid-column:  span 12;}
}
</style>
<script>
	function pushopacity(bid){
		var btn=document.getElementById(bid);
		btn.onmousedown='btn.style.opacity=\"1\"';
		btn.onmouseup='btn.style.opacity=\"0.6\"';
		btn.ontouchstart='btn.style.opacity=\"1\"';
		btn.ontouchend='btn.style.opacity=\"0.6\"';
	}
</script>
</head>

