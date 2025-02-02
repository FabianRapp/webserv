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

CGIManager::CGIManager(Client* client, Response* response, std::string path, const Request& request):
	path(path),
	request_body(request._body),
	_client(client),
	_response(response),
	_main_manager(client->data)
{
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
		throw std::runtime_error("Not CGI");
	}

	// File existence checks moved HERE (before execution)
	if (access(path.c_str(), F_OK) == -1) {
		throw std::runtime_error("Script file not found: " + path);
	}
	if (access(path.c_str(), R_OK) == -1) {
		throw std::runtime_error("Script file not readable: " + path);
	}

	std::string interpreter = getInterpreter(path);
	if (interpreter.empty()) {
		throw std::runtime_error("Unsupported CGI type");
	}

	if (pipe(inputPipe) == -1 || pipe(outputPipe) == -1) {
		//todo: how are we handeling errors?
		throw std::runtime_error("Failed to create pipes");
	}

	pid_t pid = fork();
	if (pid == -1) {
		//todo: how are we handeling errors?
		throw std::runtime_error("Failed to fork");
	}

	if (pid == 0) { // Child process
		close(inputPipe[1]);
		inputPipe[1] = -1;
	
		close(outputPipe[0]);
		outputPipe[0] = -1;

		dup2(inputPipe[0], STDIN_FILENO);
		close(inputPipe[0]);
		inputPipe[0] = -1;

		dup2(outputPipe[1], STDOUT_FILENO);
		close(outputPipe[1]);
		outputPipe[1] = -1;

		char *args[] = {
			const_cast<char *>(interpreter.c_str()),
			const_cast<char *>(path.c_str()),
			nullptr
		};
		execve(args[0], args, (char**)(envCGI.data()));
		//todo: err
		exit(1);
	} else { // Parent process
		close(inputPipe[0]);
		inputPipe[0] = -1;

		close(outputPipe[1]);
		outputPipe[1] = -1;


	
		int	fd_to_read = outputPipe[0];
		outputPipe[0] = -1;
		_response->read_fd(fd_to_read, -1, true);

		/*
		"SET-COOK: "
		"\r\n"
		"\r\n\r\n";
		*/


		if (!request_body.empty()) {
			//int	fd_to_write = inputPipe[1];
			//inputPipe[1] = -1;
			//_response->set_fd_write_data(request_body);
			//_response->write_fd(fd_to_write, true);
			write(inputPipe[1], request_body.c_str(), request_body.size());
		}
		close(inputPipe[1]);
		inputPipe[1] = -1;
	
		waitpid(pid, NULL, 0);
		_response->set_mode(Response::ResponseMode::FINISH_UP);
	}
}

CGIManager::~CGIManager(void) {
	if (outputPipe[0] != -1) {
		close(outputPipe[0]);
	}
	if (outputPipe[0] != -1) {
		close(outputPipe[0]);
	}
	if (inputPipe[0] != -1) {
		close(inputPipe[0]);
	}
	if (inputPipe[1] != -1) {
		close(inputPipe[1]);
	}
}

std::string CGIManager::execute() {

		return cgiOutput;
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
			throw std::runtime_error("Unsupported CGI type: " + extension);
		}

		if (access(interpreter.c_str(), X_OK) == -1) {
			throw std::runtime_error("Interpreter not found or not executable: " + interpreter);
		}

		return interpreter;
	}

	// This ensures that all paths return a value or throw an exception
	throw std::runtime_error("Internal error: No file extension found in path");
}

