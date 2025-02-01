#include "CGIManager.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <vector>

CGIManager::CGIManager(const std::string& uri, const std::string& body, const std::unordered_map<std::string, std::string>& envVars)
	: uri(uri), body(body), envVars(envVars) {}

std::string CGIManager::execute() {
	if (!isCGI(uri)) {
		throw std::runtime_error("Not CGI");
	}

	std::string interpreter = getInterpreter(uri);
	if (interpreter.empty()) {
		throw std::runtime_error("Unsupported CGI type");
	}

	int inputPipe[2];
	int outputPipe[2];
	if (pipe(inputPipe) == -1 || pipe(outputPipe) == -1) {
		throw std::runtime_error("Failed to create pipes");
	}

	// Dynamically construct environment variables
	std::vector<std::string> envStrings;
	for (const auto& [key, value] : envVars) {
		envStrings.push_back(key + "=" + value);
	}

	// Add SCRIPT_NAME to the environment
	envStrings.push_back("SCRIPT_NAME=" + uri);

	// Convert envStrings to a char* array for execve
	std::vector<char*> env;
	for (auto& str : envStrings) {
		env.push_back(const_cast<char*>(str.c_str()));
	}
	env.push_back(nullptr); // Null-terminate the array

	pid_t pid = fork();
	if (pid == -1) {
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

		execve(args[0], args, env.data());
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
	if (last_dot == std::string::npos) return false;
	std::string extension = uri.substr(last_dot);
	return (extension == ".py" || extension == ".php");
}

std::string CGIManager::getInterpreter(const std::string& uri) {
	size_t last_dot = uri.find_last_of('.');
	if (last_dot != std::string::npos) {
		std::string extension = uri.substr(last_dot);
		if (extension == ".py") return "/usr/bin/python3";
		if (extension == ".php") return "/usr/local/bin/php";
	}
	return "";
}