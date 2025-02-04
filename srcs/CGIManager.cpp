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
			interpreter = "/usr/bin/php";
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

