import os

print("HTTP/1.1 200 OK\r")
print("Content-Type: text/html\r")
print("Connection: close\r")
print("Content-Length: 11\r\n\r")
print("<html><body>")
print("<h1>Hello, World from Python CGI!</h1>")
print("</body></html>")

