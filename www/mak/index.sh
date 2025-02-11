#!/bin/bash

# CGI script to display all environment variables

# if [ "$ID" == "" ]; then

# fi

if [ "$theme" != "dark" ]; then
	theme="bright"
	aaa="green"
fi

if [ "$theme" == "dark" ]; then
	aaa="red"
fi

body=$(cat <<EOF
<html>
<head>
    <title>Environment Variables</title>
    <style>
        body { font-family: monospace; margin: 2em; background-color: $aaa;}
        h1 { color: #333; }
        pre { background: #f5f5f5; padding: 1em; border-radius: 5px; }
    </style>
</head>
<body>
    <h1>Environment Variables</h1>
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
echo -e "Set-Cookie: theme=dark; Max-Age=30\r"

echo -e "\r\n\r"

echo -e "$body"

#echo $theme
# Print required HTTP headers
#echo

# HTML output with environment variables
