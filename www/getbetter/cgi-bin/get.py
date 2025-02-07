#!/usr/bin/python3

# Import modules for CGI handling
import cgitb
cgitb.enable()  # Enable debugging

# Output HTTP headers and a simple HTML response
# print("Content-Type: text/html\r\n\r\n")
print("<html>")
print("<head><title>Simple GET Request Example</title></head>")
print("<body>")
print("<h1>GET Request Received</h1>")
print("<p>This is a simple response without any input parameters.</p>")
print("</body>")
print("</html>")
