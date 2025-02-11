import os

body="<html><body><h1>Hello, World from Python CGI!</h1></body></html>"
print("HTTP/1.1 200 OK\r")
print("Content-Type: text/html\r")
print("Set-Cookie: theme=dark; Max-Age=30\r")
print("Connection: close\r")
print(f'Content-Length: {len(body)}\r\n\r')
print(body)

