To run:
docker-compose up -d --build
docker-compose up -d

open server/client:
docker exec -it docker_testing_server_1 bash
docker exec -it docker_testing_client_1 bash

Then on the server run 'make && ./webserv'.

to send http requests to the server(example):
curl http://server:8080
curl -X POST -d "Hello world!" http://server:8080

The server will write out the content and save the full message with headers to 'http_message.bin'.

example of 'http_message.bin':
server: cat http_message.bin 
POST / HTTP/1.0
Host: server:8080
User-Agent: curl/7.74.0
Accept: */*
Content-Length: 12
Content-Type: application/x-www-form-urlencoded

Hello world!
