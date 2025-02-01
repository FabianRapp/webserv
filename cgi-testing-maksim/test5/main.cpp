#include "CGIManager.hpp"
#include <iostream>
#include <unordered_map>

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
	req._uri = "./hello.py";
	// req._uri = "./hello.php";
	req._version = "HTTP/1.1";
	req._headers["Host"] = "localhost";
	req._headers["Content-Length"] = "13";
	req._headers["Content-Type"] = "application/x-www-form-urlencoded";
	req._body = "username=John";

	// Prepare environment variables
	std::unordered_map<std::string, std::string> envVars = {
		{"REQUEST_METHOD", "POST"},
		{"CONTENT_LENGTH", std::to_string(req._body.size())},
		{"CONTENT_TYPE", "application/x-www-form-urlencoded"}
	};

	try {
		// Pass the necessary data to CGIManager
		CGIManager cgiManager(req._uri, req._body, envVars);
		std::string cgiOutput = cgiManager.execute();

		// Construct and display the HTTP response
		std::string httpResponse = "HTTP/1.1 200 OK\r\n" + cgiOutput;
		std::cout << "\nFull HTTP Response:\n" << httpResponse << std::endl;
	} catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}