#include "CGIManager.hpp"
#include <iostream>
#include <unordered_map>
#include <vector>

using HeadersMap = std::unordered_map<std::string, std::string>;

enum class MethodType {
	OPTIONS,
	GET,
	HEAD,
	POST,
	PUT,
	DELETE,
	TRACE,
	CONNECT,
	INVALID
};

struct Request {
	MethodType _type;           // HTTP method
	std::string _uri;           // Requested URI
	std::string _version;       // HTTP version
	HeadersMap _headers;        // Map of headers
	std::string _body;          // Request body
};

MethodType parseMethod(const std::string& method) {
	if (method == "GET") return MethodType::GET;
	if (method == "POST") return MethodType::POST;
	if (method == "DELETE") return MethodType::DELETE;
	return MethodType::INVALID;
}

int main() {
	Request req;

	req._type = parseMethod("POST");
	// req._uri = "./hello.php"; // test with php
	req._uri = "./hello1.py"; // test with py
	req._version = "HTTP/1.1";
	req._headers["Host"] = "localhost";
	req._headers["Content-Length"] = "13";
	req._headers["Content-Type"] = "application/x-www-form-urlencoded";
	req._body = "username=John";

	// todo: here we should use get methods to get all the data from Request class.
	std::vector<char*> envCGI = {
		const_cast<char*>("REQUEST_METHOD=POST"),
		const_cast<char*>(("CONTENT_LENGTH=" + std::to_string(req._body.size())).c_str()),
		const_cast<char*>("CONTENT_TYPE=application/x-www-form-urlencoded"),
		const_cast<char*>(("SCRIPT_NAME=" + req._uri).c_str()),
		nullptr // Null-terminate the array
	};

	try {
		// Pass the necessary data to CGIManager
		CGIManager cgiManager(req._uri, req._body, envCGI);
		std::string _cgiOutput = cgiManager.execute();

		// Construct and display the HTTP response
		std::string httpResponse = "HTTP/1.1 200 OK\r\n" + _cgiOutput;
		// print response
		std::cout << "\nFull HTTP Response:\n" << httpResponse << std::endl;
	} catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
