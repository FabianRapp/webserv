#!/bin/bash

custom_message="Hello from BASH CGI!"

body=$(cat <<EOF
<html>
<head>
	<title>Environment Variables</title>
	<style>
		body { font-family: monospace; margin: 2em; background-color: #101112;}
		h1 { color: #FF0000; }
		p { color: #FF0000; }
		pre { color: #FF0000; background: #f5f5f5; padding: 1em; border-radius: 5px; }
	</style>
</head>
<body>
	<h1>Environment Variables</h1>
	<p>$custom_message</p>
	<pre>
$(printenv)
	</pre>
</body>
</html>
EOF
)

echo -e "HTTP/1.1 200 OK\r"
echo -e "Content-Type: text/html\r"
echo -e "Content-Length: ${#body}"
# echo -e "Set-Cookie: theme=dark; Max-Age=30\r"

echo -e "\r\n\r"

echo -e "$body"
