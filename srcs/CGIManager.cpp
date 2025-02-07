#include "CGIManager.hpp"
#include "StringArray.hpp"
#include "Client.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <Request.hpp>
#include <Manager.hpp>
#include <colors.h>
#include <WriteFd.hpp>
#include <ReadFd.hpp>

CGIManager::CGIManager(Client* client, Response* response, std::string path, const Request& request):
	path(path),
	request_body(request._body),
	_client(client),
	_response(response),
	_main_manager(client->data),
	_mode(CGI_MODE::INIT_WRITING)
{
	int	 old_errno = errno;
	inputPipe[0] = -1;
	inputPipe[1] = -1;
	outputPipe[0] = -1;
	outputPipe[1] = -1;
	errno = 0;
	envCGI = {
		static_cast<const char*>(("REQUEST_METHOD=" + to_string(request._type)).c_str()),
		static_cast<const char*>(("CONTENT_LENGTH=" + std::to_string(request._body.size())).c_str()),
		static_cast<const char*>("CONTENT_TYPE=application/x-www-form-urlencoded"),
		static_cast<const char*>((("SCRIPT_NAME=" + path).c_str())),
	};

// envCGI = {
// 	// The HTTP method used for the request (e.g., GET, POST, PUT, DELETE).
// 	static_cast<const char*>(("REQUEST_METHOD=" + to_string(request._type)).c_str()),

// 	// The length of the request body in bytes (used for POST and PUT requests).
// 	static_cast<const char*>(("CONTENT_LENGTH=" + std::to_string(request._body.size())).c_str()),

// 	// The MIME type of the request body (e.g., application/x-www-form-urlencoded).
// 	static_cast<const char*>("CONTENT_TYPE=application/x-www-form-urlencoded"),

// 	// The virtual path to the script being executed.
// 	// static_cast<const char*>("SCRIPT_NAME=/cgi-bin/script.cgi"),
// 	static_cast<const char*>((("SCRIPT_NAME=" + path).c_str())),

// 	// The extra path information provided in the URL after the script name.
// 	static_cast<const char*>("PATH_INFO=/extra/path/info"),

// 	// The physical file system path corresponding to PATH_INFO.
// 	static_cast<const char*>("PATH_TRANSLATED=/var/www/html/extra/path/info"),

// 	// The full URI of the current request, including query strings.
// 	static_cast<const char*>("REQUEST_URI=/cgi-bin/script.cgi?name=value"),

// 	// The query string part of the URL (everything after the `?` in the URL).
// 	static_cast<const char*>("QUERY_STRING=name=value"),

// 	// The hostname or IP address of the server handling the request.
// 	static_cast<const char*>("SERVER_NAME=localhost"),

// 	// The IP address of the server handling the request.
// 	static_cast<const char*>("SERVER_ADDR=127.0.0.1"),

// 	// The port number on which the server is listening for requests.
// 	static_cast<const char*>("SERVER_PORT=80"),

// 	// The protocol and version used for the request (e.g., HTTP/1.1).
// 	static_cast<const char*>("SERVER_PROTOCOL=HTTP/1.1"),

// 	// The name and version of your web server software.
// 	static_cast<const char*>("SERVER_SOFTWARE=CustomWebServer/1.0"),

// 	// The CGI specification version supported by the server (e.g., CGI/1.1).
// 	static_cast<const char*>("GATEWAY_INTERFACE=CGI/1.1"),

// 	// The IP address of the client making the request.
// 	static_cast<const char*>("REMOTE_ADDR=192.168.1.100"),

// 	// The hostname of the client making the request (if available).
// 	static_cast<const char*>("REMOTE_HOST=client.localdomain"),

// 	// The port number from which the client made the request.
// 	static_cast<const char*>("REMOTE_PORT=54321"),

// 	// If authentication is required, this specifies the authentication method used (e.g., Basic).
// 	static_cast<const char*>("AUTH_TYPE=Basic"),

// 	// If authentication is required, this specifies the authenticated username.
// 	static_cast<const char*>("REMOTE_USER=username"),

// 	// If RFC 931 identification is supported, this specifies the remote username for logging purposes.
// 	static_cast<const char*>("REMOTE_IDENT=user_identification"),

// 	// All HTTP headers sent by the client, prefixed with `HTTP_` and transformed to uppercase.
// 	static_cast<const char*>("ALL_HTTP=HTTP_ACCEPT:text/html; HTTP_USER_AGENT:Mozilla/5.0; HTTP_HOST:localhost;"),

// 	// All raw HTTP headers sent by the client without transformation.
// 	static_cast<const char*>("ALL_RAW=Accept: text/html\r\nUser-Agent: Mozilla/5.0\r\nHost: localhost\r\n"),

// 	// A list of MIME types that the client can accept, separated by commas (from HTTP headers).
// 	static_cast<const char*>("HTTP_ACCEPT=text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8"),

// 	// Information about the client's browser or user agent (from HTTP headers).
// 	static_cast<const char*>("HTTP_USER_AGENT=Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 Chrome/114 Safari/537.36"),

// 	// If SSL is enabled, this indicates whether HTTPS is being used ("on" or "off").
// 	static_cast<const char*>("HTTPS=off"),  // Use "on" if HTTPS is enabled.

// 	// If SSL is enabled, this specifies the protocol version used (e.g., TLSv1.3).
// 	static_cast<const char*>("SSL_PROTOCOL=TLSv1.3"),  // Only relevant if HTTPS is on.

// 	// If SSL is enabled, this specifies the cipher used for encryption.
// 	static_cast<const char*>("SSL_CIPHER=AES256-GCM-SHA384"),  // Only relevant if HTTPS is on.

// 	// Absolute path to your web server's document root directory.
// 	static_cast<const char*>("DOCUMENT_ROOT=/var/www/html"),

// 	// Full URL to access this script or resource requested by a client
// 	static_cast <const char*> ("SCRIPT_URI=http://localhost/cgi-bin/script.cgi")
// };


	if (request._headers.find(HeaderType::COOKIE) != request._headers.end()) {

		//std::cout << std::string(request._headers[HeaderType::HOST]);
		std::string	request_cookies_val = "name=value; name2=value2; name3=value3";
		Line	key_vals(request_cookies_val, "; ");
		for (auto var : key_vals) {
			/*todo: verification
			if (cookie_data_structure.find(var)) {
			*/
				envCGI.push_back(var.c_str());
			//}
		}
	}
	envCGI.push_back(nullptr);

	if (!isCGI(path)) {
		_client->response->load_status_code_response(500, "Internal Server Error");
		_mode = CGI_MODE::FINISHED;
		return ;
	}

	// File existence checks moved HERE (before execution)
	if (access(path.c_str(), F_OK) == -1) {
		_client->response->load_status_code_response(500, "Internal Server Error");
		_mode = CGI_MODE::FINISHED;
		return ;
	}
	if (access(path.c_str(), R_OK) == -1) {
		_client->response->load_status_code_response(500, "Internal Server Error");
		_mode = CGI_MODE::FINISHED;
		return ;
	}

	std::string interpreter = getInterpreter(path);
	if (interpreter.empty()) {
		_client->response->load_status_code_response(500, "Internal Server Error");
		_mode = CGI_MODE::FINISHED;
		return ;
	}

	if (pipe(inputPipe) == -1 || pipe(outputPipe) == -1) {
		_client->response->load_status_code_response(500, "Internal Server Error");
		_mode = CGI_MODE::FINISHED;
		return ;
	}
	_pid = fork();
	if (_pid == -1) {
		_client->response->load_status_code_response(500, "Internal Server Error");
		_mode = CGI_MODE::FINISHED;
		return ;
	}

	if (_pid == 0) { // Child process
		std::cout << "cgi child\n\n";
		//sleep(10);


		char *args[] = {
			const_cast<char *>(interpreter.c_str()),
			const_cast<char *>(path.c_str()),
			nullptr
		};
		for (int i = 0; char *arg = args[i]; i++) {
			std::cout << "cgi_args[" << i << "] == " << arg << "\n";
		}

		ft_close(inputPipe[1]);
		inputPipe[1] = -1;

		ft_close(outputPipe[0]);
		outputPipe[0] = -1;

		if (dup2(inputPipe[0], STDIN_FILENO) < 0) {
			_child_dup_fail();
		}
		ft_close(inputPipe[0]);
		inputPipe[0] = -1;

		if (dup2(outputPipe[1], STDOUT_FILENO) < 0) {
			_child_dup_fail();
		}
		ft_close(outputPipe[1]);
		outputPipe[1] = -1;

		execve(args[0], args, (char**)(envCGI.data()));
		_child_exec_fail();
	} else { // Parent process
		_main_manager.cgi_lifetimes.add(_pid);
		ft_close(inputPipe[0]);
		inputPipe[0] = -1;

		ft_close(outputPipe[1]);
		outputPipe[1] = -1;

	}
	errno = old_errno;
}

void	CGIManager::_child_dup_fail(void) {
	exit(1);
}

void	CGIManager::_child_exec_fail(void) {
	//todo: execve(cgi to read 500 code);
	exit(1);
}

void	CGIManager::_init_reading(void) {
	int	fd_to_read = outputPipe[0];
	outputPipe[0] = -1;
	//read_fd will make sure the cgi->execute does not get called unitil the data is read
	_response->read_fd(fd_to_read, -1);
	_mode = CGI_MODE::FINISHED;
}

//todo: this is a place holder body since I'm too lazy to make a client that sends a request with body
const std::string test_body = "\ntest body, raplace this later(" __FILE__ " line " + std::to_string(__LINE__) + ")\n";
void	CGIManager::_init_writing(void) {
	//request_body <--

	int	fd_to_write = inputPipe[1];
	inputPipe[1] = -1;

	if (test_body.empty()) {
		ft_close(fd_to_write);
		return ;
	}

	_response->set_fd_write_data(test_body);
	//write_fd will make sure the cgi->execute does not get called unitil the data is written
	_response->write_fd(fd_to_write);
	_mode = CGI_MODE::INIT_READING;
}

CGIManager::~CGIManager(void) {
	std::cout << "cgi manager destructor\n";

	if (outputPipe[0] != -1) {
		ft_close(outputPipe[0]);
	}
	if (outputPipe[0] != -1) {
		ft_close(outputPipe[0]);
	}
	if (inputPipe[0] != -1) {
		ft_close(inputPipe[0]);
	}
	if (inputPipe[1] != -1) {
		ft_close(inputPipe[1]);
	}
}

// returns true when done
bool	CGIManager::execute() {
	bool	debug = true;
	if (debug) std::cout << "cgi::execute: ";
	switch (_mode) {
		case (CGI_MODE::PASS):
			break ;
		case (CGI_MODE::INIT_WRITING):
			if (debug) std::cout << "init_writing\n";
			_init_writing();
			break ;
		case (CGI_MODE::INIT_READING):
			if (debug) std::cout << "init_reading\n";
			_init_reading();
			break ;
		case (CGI_MODE::FINISHED):
			if (debug) std::cout << "finished\n";
			return (true);
	}
	return (false);
}

bool CGIManager::isCGI(const std::string& path) {
	size_t last_dot = path.find_last_of('.');
	if (last_dot == std::string::npos)
		return false;
	std::string extension = path.substr(last_dot);
	return (extension == ".py" || extension == ".php");
}

std::string CGIManager::getInterpreter(const std::string& path) {
	size_t last_dot = path.find_last_of('.');
	if (last_dot != std::string::npos) {
		std::string extension = path.substr(last_dot);
		std::string interpreter;

		if (extension == ".py") {
			interpreter = "/usr/bin/python3";
		} else if (extension == ".php") {
			//todo: try to check if the interpreter is available, because it might be in different location on different machines
			interpreter = "/usr/bin/php";
			// interpreter = "/usr/local/bin/php";
		} else {
			// "Unsupported CGI type: " + extension
			_client->response->load_status_code_response(500, "Internal Server Error");
			_mode = CGI_MODE::FINISHED;
			return "";
		}
		if (access(interpreter.c_str(), X_OK) == -1) {
			//"Interpreter not found or not executable: " + interpreter
			_client->response->load_status_code_response(500, "Internal Server Error");
			_mode = CGI_MODE::FINISHED;
			return "";
		}
		return interpreter;
	}

	//"Internal error: No file extension found in path"
	_client->response->load_status_code_response(500, "Internal Server Error");
	_mode = CGI_MODE::FINISHED;
	return "";
}

