<?php
    // Store the HTML content in a variable
    $htmlContent = '
		<!DOCTYPE html>
		<html lang="en">
		<head>
			<meta charset="UTF-8">
			<meta name="viewport" content="width=device-width, initial-scale=1.0">
			<title>Hello Page</title>
			<style>
				body {
					font-family: Arial, sans-serif;
					background-color: #f4f4f4;
					color: #333;
					display: flex;
					justify-content: center;
					align-items: center;
					height: 100vh;
					margin: 0;
				}
				h1 {
					color: #007bff;
					font-size: 13rem;
				}
			</style>
		</head>
		<body>
			<h1>Hello</h1>
		</body>
		</html>
    ';

    // Calculate the length of the HTML content in bytes
    $contentLength = strlen($htmlContent);

    // Output the headers manually
    echo "HTTP/1.1 200 OK\n";
    echo "Content-Type: text/html\n";
    echo "Content-Length: $contentLength\n\n"; // Two newlines to end headers

    // Output the HTML content
    echo $htmlContent;
?>
