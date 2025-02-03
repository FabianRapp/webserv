#include <iostream>
#include <string>
#include <unordered_map>
#include <unistd.h>
#include <sys/wait.h>

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
	if (method == "OPTIONS") return MethodType::OPTIONS;
	if (method == "GET") return MethodType::GET;
	if (method == "HEAD") return MethodType::HEAD;
	if (method == "POST") return MethodType::POST;
	if (method == "PUT") return MethodType::PUT;
	if (method == "DELETE") return MethodType::DELETE;
	if (method == "TRACE") return MethodType::TRACE;
	if (method == "CONNECT") return MethodType::CONNECT;
	return MethodType::INVALID;
}

bool isCGI(const std::string& uri){
	return uri.size() >= 3 && uri.substr(uri.size() - 3) == ".py";
}

int main()
{
	Request req;

	req._type = parseMethod("POST");
	req._uri = "./hello.py";
	// req._uri = "./hello.php";
	req._version = "HTTP/1.1";

	//headers
	req._headers["Host"] = "localhost";
	req._headers["Content-Length"] = "13";
	req._headers["Content-Type"] = "application/x-www-form-urlencoded";

	//body
	req._body = "username=John";

	// ------------------

	// if(isCGI(req._uri)){
	// 	std::cout << "Yes" << std::endl;
	// } else {
	// 	std::cout << "No" << std::endl;
	// }

	// check if we need to run CGI script or no
	if (!isCGI(req._uri)) {
		// std::cerr << "Not a CGI request.\n";
		std::cout << "Not CGI" << std::endl;
		return 1;
	}

	// -----------
	// Create a pipes for request body if we have one and response
	int inputPipe[2];
	int outputPipe[2];
	if(pipe(inputPipe) == -1 || pipe(outputPipe) == -1){
		perror("pipe");
		exit(1);
	}

	char *env[] = {
		const_cast<char *>("REQUEST_METHOD=POST"),
		const_cast<char *>("CONTENT_LENGTH=13"),
		const_cast<char *>("CONTENT_TYPE=application/x-www-form-urlencoded"),
		const_cast<char *>("SCRIPT_NAME=./hello.py"),
		NULL
	};

	pid_t pid = fork();
	if (pid == -1) {
		perror("pipe");
		exit(1);
	}

	if (pid == 0) { //if child process
		dup2(inputPipe[0], STDIN_FILENO); // stdin to inputPipe[0]
		ft_close(inputPipe[1]); // closing write end, cuz we gonna write inside the parent

		dup2(outputPipe[1], STDOUT_FILENO); //stdout to outputPipe[0]
		ft_close(outputPipe[0]); // close read end of the pipe, cuz we gonna read inside the parent

		char *args[] = {
			const_cast<char *>("/usr/bin/python3"),
			const_cast<char *>("./hello.py"),
			NULL
		};

		// run CGI
		execve(args[0], args, env);
		exit(1);
	}

	//parent
	else if (pid > 0)  {
		ft_close(inputPipe[0]);
		ft_close(outputPipe[1]);

		if(!req._body.empty()) {
			write(inputPipe[1], req._body.c_str(), req._body.size());
		}
		ft_close(inputPipe[1]);

		char buffer[1024];
		ssize_t bytesRead;
		std::string _cgiOutput;

		while ((bytesRead = read(outputPipe[0], buffer, sizeof(buffer) - 1)) > 0) {
			buffer[bytesRead] = '\0';
			_cgiOutput += buffer;
		}

		ft_close(outputPipe[0]);
		waitpid(pid, NULL, 0);

		// std::cout << "\nCaptured CGI Output:\n" << _cgiOutput << std::endl;

		std::string httpResponse = "HTTP/1.1 200 OK\r\n" + _cgiOutput;
		std::cout << "\nFull HTTP Response:\n" << httpResponse << std::endl;

		return 0;

	} else {
		perror("fork failed");
		return 1;
	}

}
