#!/bin/bash

# CGI script to display all environment variables

# Print required HTTP headers
echo "Content-Type: text/html"
echo

# HTML output with environment variables
cat <<EOF
<html>
<head>
    <title>Environment Variables</title>
    <style>
        body { font-family: monospace; margin: 2em; }
        h1 { color: #333; }
        pre { background: #f5f5f5; padding: 1em; border-radius: 5px; }
    </style>
</head>
<body>
    <h1>Environment Variables</h1>
    <pre>
EOF

# List all environment variables
printenv

cat <<EOF
    </pre>
</body>
</html>
EOF
