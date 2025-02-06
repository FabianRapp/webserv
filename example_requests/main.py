import socket
from delete_dir import http_request as delete_dir
from delete_file import http_request as delete_file
from post_file import http_request as post_file

PORT = 8080


with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect(("127.0.0.1", PORT))

    #http_request = delete_dir
    #http_request = delete_file
    http_request = post_file

    print(f"Sending this request:\n{http_request}")

    s.sendall(http_request.encode())
    response = b""

    while True:
        data = s.recv(1024)
        if not data:
            break
        response += data

print(f"Got response:\n{response.decode()}")
