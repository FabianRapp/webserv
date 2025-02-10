#!/bin/bash
echo "SET-COOKIE: abc=xyz;\r"
echo $theme
# Set the content type to HTML
echo "Content-type: text/html\r"
echo "";

# Output the HTML content
echo "<html>"
echo "<head><title>Hello, World!</title></head>"
echo "<body>"
echo "<h1>Hello, World FROM BASHHHHHHHs!</h1>"
echo "</body>"
echo "</html>"
