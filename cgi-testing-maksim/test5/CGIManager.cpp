#include "CGIManager.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <vector>
#include <unistd.h>

CGIManager::CGIManager(const std::string& uri, const std::string& body,
	const std::vector<char*>& envCGI)
	: uri(uri), body(body), envCGI(envCGI) {}

std::string CGIManager::execute() {
	if (!isCGI(uri)) {
		throw std::runtime_error("Not CGI");
	}

	// File existence checks moved HERE (before execution)
	if (access(uri.c_str(), F_OK) == -1) {
		throw std::runtime_error("Script file not found: " + uri);
	}
	if (access(uri.c_str(), R_OK) == -1) {
		throw std::runtime_error("Script file not readable: " + uri);
	}

	std::string interpreter = getInterpreter(uri);
	if (interpreter.empty()) {
		throw std::runtime_error("Unsupported CGI type");
	}

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
			const_cast<char *>(uri.c_str()),
			nullptr
		};

		execve(args[0], args, envCGI.data());

		exit(1);
	} else { // Parent process
		close(inputPipe[0]);
		close(outputPipe[1]);

		if (!body.empty()) {
			write(inputPipe[1], body.c_str(), body.size());
		}
		close(inputPipe[1]);

		char buffer[1024];
		ssize_t bytesRead;
		std::string cgiOutput;

		while ((bytesRead = read(outputPipe[0], buffer, sizeof(buffer) - 1)) > 0) {
			buffer[bytesRead] = '\0';
			cgiOutput += buffer;
		}

		close(outputPipe[0]);
		waitpid(pid, NULL, 0);

		return cgiOutput;
	}
}

bool CGIManager::isCGI(const std::string& uri) {
	size_t last_dot = uri.find_last_of('.');
	if (last_dot == std::string::npos)
		return false;
	std::string extension = uri.substr(last_dot);
	return (extension == ".py" || extension == ".php");
}

std::string CGIManager::getInterpreter(const std::string& uri) {
	size_t last_dot = uri.find_last_of('.');
	if (last_dot != std::string::npos) {
		std::string extension = uri.substr(last_dot);
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
	throw std::runtime_error("Internal error: No file extension found in URI");
}

