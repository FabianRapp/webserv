import socket

HOST = "has_delte.com"
PORT = 8080

TYPE = "DELETE"
URI = "/here_is_delete_allowed/delete_me.txt"

http_request = """\
{type} {uri} HTTP/1.1\r
Host: {host}\r
User-Agent: CustomClient/1.0\r
Connection: close\r
\r
""".format(type=TYPE, uri=URI, host=HOST)

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
	s.connect(("127.0.0.1", PORT))
	s.sendall(http_request.encode())
	response = b""
	
	while True:
		data = s.recv(1024)
		if not data:
			break
		response += data

print(response.decode())
