<html>
	<head><title>Doorlock Setting Page</title></head>
	<body>
	
	<form method="post">
		<h1>Set password</h1><br>
		<input type="text" name="name" id="name" maxlength="4">
		<input type="submit" id="passbtn" value = "set"/>
	</form>
	
	<h1>Get temp password</h1><br>
	<form method="post" id="tempform">
		<input type="hidden" name="temp" id="temp">
		<input type="submit" id="tempbtn" value = "set"/>
	</form>

<?
function setPass(){
	$fd = fopen("./pass", "w");
	
	$pass = $_POST['name'];
	fwrite($fd, $pass);

	fclose($fd);
}

function setTemp(){
	$fd = fopen("./temp", "w");

	$pass = sprintf("%04d", rand(0000,9999));
	$pass = "$pass";
	fwrite($fd, $pass);

	fclose($fd);

	echo "<script>";
	echo "var node = document.createElement(\"p\");";
	echo "var mention = document.createTextNode(\"Your Password is $pass \");";
	echo "node.appendChild(mention);";
	echo "document.getElementById(\"tempform\").appendChild(node);";
	echo "</script>";
}


if(array_key_exists('name', $_POST )){
	setPass();
}
else if(array_key_exists('temp', $_POST)){
	setTemp();
}
?>

	</body>
</html>
