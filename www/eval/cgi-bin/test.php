<?php
	$allEnvVars = getenv();

	$htmlContent = '
		<!DOCTYPE html>
		<html lang="en">
		<head>
			<meta charset="UTF-8">
			<meta name="viewport" content="width=device-width, initial-scale=1.0">
			<title>PHP CGI</title>
			<style>
				body {
					font-family: "Segoe UI", Tahoma, Geneva, Verdana, sans-serif;
					background: linear-gradient(135deg, #2d2d2d, #1a1a1a);
					color: #333;
					padding: 20px;
				}
				h1 {
					color: #007bff;
				}
				pre {
					background-color: #fff;
					padding: 15px;
					border-radius: 5px;
					border: 1px solid #ddd;
				}
			</style>
		</head>
		<body>
			<h1>Environment Variables - PHP CGI</h1>
			<pre>';

	foreach ($allEnvVars as $key => $value) {
		$htmlContent .= "$key: $value\n";
	}

	$htmlContent .= '
			</pre>
		</body>
		</html>';

	$contentLength = strlen($htmlContent);

	// headers
	echo "HTTP/1.1 200 OK\n";
	echo "Content-Type: text/html\n";
	echo "Content-Length: $contentLength\n\n";

	echo $htmlContent;
?>
