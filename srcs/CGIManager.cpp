#include "CGIManager.hpp"
#include "StringArray.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <Request.hpp>
#include <colors.h>

CGIManager::CGIManager(std::string path, const Request& request):
	path(path),
	request_body(request._body)
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
}
#include <fcntl.h>
std::string CGIManager::execute() {
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

	//todo: later: think of which fds for destructor
	int inputPipe[2];
	int outputPipe[2];
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
		dup2(inputPipe[0], STDIN_FILENO);
		close(inputPipe[1]);

		dup2(outputPipe[1], STDOUT_FILENO);
		close(outputPipe[0]);

		char *args[] = {
			const_cast<char *>(interpreter.c_str()),
			const_cast<char *>(path.c_str()),
			nullptr
		};

		execve(args[0], args, (char**)(envCGI.data()));

		exit(1);
	} else { // Parent process
		close(inputPipe[0]);
		close(outputPipe[1]);

		if (!request_body.empty()) {
			write(inputPipe[1], request_body.c_str(), request_body.size());
		}
		close(inputPipe[1]);

		char buffer[1024];
		ssize_t bytesRead;
		std::string cgiOutput;

		while ((bytesRead = read(outputPipe[0], buffer, sizeof(buffer) - 1)) > 0) {
			buffer[bytesRead] = '\0';
			cgiOutput += buffer;
		}
		
		"SET-COOK: "

		"\r\n"
		"\r\n\r\n";


		close(outputPipe[0]);
		waitpid(pid, NULL, 0);

		return cgiOutput;
	}
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

