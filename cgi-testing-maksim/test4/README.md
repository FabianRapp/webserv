c++ -Wall -Wextra -Werror -std=c++17 main.cpp -o main



(venv) ➜  cgi-testing-maksim git:(feat/CGI_maksim) ✗ ./cgi_tester 
cgi: no REQUEST_METHOD in environment
(venv) ➜  cgi-testing-maksim git:(feat/CGI_maksim) ✗ export REQUEST_METHOD="POST"
(venv) ➜  cgi-testing-maksim git:(feat/CGI_maksim) ✗ ./cgi_tester 
cgi: invalid SERVER_PROTOCOL version
(venv) ➜  cgi-testing-maksim git:(feat/CGI_maksim) ✗ export SERVER_PROTOCOL="HTTP/1.1"
(venv) ➜  cgi-testing-maksim git:(feat/CGI_maksim) ✗ 

(venv) ➜  cgi-testing-maksim git:(feat/CGI_maksim) ✗ ./cgi_tester                     
Status: 500 Internal Server Error
Content-Type: text/html; charset=utf-8

PATH_INFO not found%                                                                                                                              
(venv) ➜  cgi-testing-maksim git:(feat/CGI_maksim) ✗ 


| Variable Name     | Description                                                                 |
|-------------------|-----------------------------------------------------------------------------|
| `REQUEST_METHOD`  | HTTP method used for the request (e.g., `GET`, `POST`).                     |
| `CONTENT_LENGTH`  | Length of the request body (only for `POST` requests).                      |
| `CONTENT_TYPE`    | MIME type of the request body (e.g., `application/x-www-form-urlencoded`).  |
| `QUERY_STRING`    | Query parameters from a `GET` request (e.g., `name=JohnDoe`).               |
| `SCRIPT_NAME`     | Name of the script being executed (e.g., `/path/to/script.py`).             |
| `SERVER_PROTOCOL` | Version of HTTP being used (e.g., `HTTP/1.1`).                              |
| `SERVER_NAME`     | Hostname or IP address of the server handling the request.                  |
| `SERVER_PORT`     | Port number on which the server is listening (e.g., `80` or `8080`).        |