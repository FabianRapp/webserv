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
#include <enums.hpp>

CGIManager::CGIManager(Client* client, const LocationConfigFile& location_config,
		Response* response, std::string path, const Request& request):
	path(path),
	request_body(request._body),
	_client(client),
	_response(response),
	_main_manager(client->data),
	_mode(CGI_MODE::INIT_WRITING),
	_location_cofig(location_config)
{
	int	 old_errno = errno;
	inputPipe[0] = -1;
	inputPipe[1] = -1;
	outputPipe[0] = -1;
	outputPipe[1] = -1;
	errno = 0;
	envCGI_storage = {
		//"REQUEST_METHOD=" + to_string(request._type),
		//"CONTENT_LENGTH=" + std::to_string(request._body.size()),
		//"CONTENT_TYPE=application/x-www-form-urlencoded",
		"SCRIPT_NAME=" + path,
	};
	std::string	env_var;
	env_var = "REQUEST_METHOD=" + to_string(request._type);
	envCGI_storage.push_back(env_var);
	envCGI_storage.push_back(std::string("SERVER_PROTOCOL=HTTP/1.1"));
	envCGI_storage.push_back(std::string("PATH_INFO=") + "");
	for (const auto& [type, value] : request._headers) {
		if (type == HeaderType::CONTENT_LENGTH
			|| type == HeaderType::CONTENT_TYPE
		){
			env_var = to_string(type) + "=" + value;
		} else {
			env_var = "HTTP_" + to_string(type) + "=" + value;
		}
		envCGI_storage.push_back(env_var);
	}

// envCGI_storage = {
// 	// The HTTP method used for the request (e.g., GET, POST, PUT, DELETE).
// 	static_cast<const char*>(("REQUEST_METHOD=" + to_string(request._type)).c_str()),


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
				envCGI_storage.push_back(var);
			//}
		}
	}



	for (std::string& var : envCGI_storage) {
		envCGI.push_back(var.c_str());
	}
	envCGI.push_back(nullptr);


	if (!isCGI(path, _location_cofig)) {
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

void	CGIManager::_init_writing(void) {
	int	fd_to_write = inputPipe[1];
	inputPipe[1] = -1;

	if (request_body.empty()) {
		ft_close(fd_to_write);
		_mode = CGI_MODE::INIT_READING;
		//std::cout << "no request body in cgi init writing\n";
		return ;
	}
	//std::cout << "has request body in cgi init writing\n";
	_response->set_fd_write_data(request_body);
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
	if (debug) {
		std::cout << "cgi::execute: " << std::endl;;

	}
	switch (_mode) {
		case (CGI_MODE::PASS):
			if (debug) {
				std::cout << "mode: PASS: ";
			}
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

bool CGIManager::isCGI(const std::string& path, const LocationConfigFile& location_config) {
	size_t last_dot = path.find_last_of('.');
	if (last_dot == std::string::npos)
		return false;

	std::string extension = path.substr(last_dot);

	// Get configured CGI extensions from location config
	const auto& cgi_extensions = location_config.getCgiExtensions();

	// Check if extension exists in CGI map
	return cgi_extensions.find(extension) != cgi_extensions.end();
}

std::string CGIManager::getInterpreter(const std::string& path) {
	size_t last_dot = path.find_last_of('.');
	if (last_dot == std::string::npos) {
		return "";
	}

	std::string extension = path.substr(last_dot);
	const auto& cgi_map = _location_cofig.getCgiExtensions();

	if (auto it = cgi_map.find(extension); it != cgi_map.end()) {
		const std::string& interpreter = it->second;

		// Runtime executable check
		if (access(interpreter.c_str(), X_OK) != 0) {
			return "";
		}
			printCgiRunning();
		return interpreter;
	}
	return "";
}


void CGIManager::printCgiRunning() {
std::cout << FT_ANSI_RED_BOLD << R"(
  /$$$$$$   /$$$$$$  /$$$$$$       /$$$$$$$  /$$   /$$ /$$   /$$ /$$   /$$ /$$$$$$ /$$   /$$  /$$$$$$
 /$$__  $$ /$$__  $$|_  $$_/      | $$__  $$| $$  | $$| $$$ | $$| $$$ | $$|_  $$_/| $$$ | $$ /$$__  $$
| $$  \__/| $$  \__/  | $$        | $$  \ $$| $$  | $$| $$$$| $$| $$$$| $$  | $$  | $$$$| $$| $$  \__/
| $$      | $$ /$$$$  | $$        | $$$$$$$/| $$  | $$| $$ $$ $$| $$ $$ $$  | $$  | $$ $$ $$| $$ /$$$$
| $$      | $$|_  $$  | $$        | $$__  $$| $$  | $$| $$  $$$$| $$  $$$$  | $$  | $$  $$$$| $$|_  $$
| $$    $$| $$  \ $$  | $$        | $$  \ $$| $$  | $$| $$\  $$$| $$\  $$$  | $$  | $$\  $$$| $$  \ $$
|  $$$$$$/|  $$$$$$/ /$$$$$$      | $$  | $$|  $$$$$$/| $$ \  $$| $$ \  $$ /$$$$$$| $$ \  $$|  $$$$$$/
 \______/  \______/ |______/      |__/  |__/ \______/ |__/  \__/|__/  \__/|______/|__/  \__/ \______/
)" << FT_ANSI_RESET << std::endl;
}
