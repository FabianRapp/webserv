#include "ServerConfigFile.hpp"

ServerConfigFile::ServerConfigFile() {}

void ServerConfigFile::setPort(int port) {
	_port = port;
}

void ServerConfigFile::setServerName(const std::string& server_name) {
	_server_name = server_name;
}

void ServerConfigFile::setRoot(const std::string& root) {
	_root = root;
}

void ServerConfigFile::addErrorPage(int error_code, const std::string& path) {
	_error_pages[error_code] = path;
}

void ServerConfigFile::addLocation(const LocationConfigFile& location) {
	_locations.push_back(location);
}

// Getters
int ServerConfigFile::getPort() const {
	return _port;
}

const std::string& ServerConfigFile::getServerName() const {
	return _server_name;
}

const std::string& ServerConfigFile::getRoot() const {
	return _root;
}

const std::map<int, std::string>& ServerConfigFile::getErrorPages() const {
	return _error_pages;
}

const std::vector<LocationConfigFile>& ServerConfigFile::getLocations() const {
	return _locations;
}

// Debugging utility
void ServerConfigFile::printServer() const {
	std::cout << "Server Port: " << _port << "\n";
	if (!_server_name.empty()) {
		std::cout << "Server Name: " << _server_name << "\n";
	}
	std::cout << "Root Directory: " << _root << "\n";

	if (!_error_pages.empty()) {
		std::cout << "Error Pages:\n";
		for (const auto& [code, path] : _error_pages) {
			std::cout << "  Error " << code << ": " << path << "\n";
		}
	}

	if (!_locations.empty()) {
		std::cout << "Locations:\n";
		for (const auto& location : _locations) {
			location.printLocation();
			std::cout << "\n";
		}
	}
}
