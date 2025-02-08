#include "../../includes/ConfigParser/ServerConfigFile.hpp"
#include "../../includes/parser/StringArray.hpp"
#include <unistd.h>
#include <stdexcept>

ServerConfigFile::ServerConfigFile() {}

std::string	toggle_path(std::string path) {
	bool	pwd_on = true;

	if (pwd_on) {
		return (std::string(std::getenv("PWD")) + path);
	} else {
		return (path);
	}
}

void ServerConfigFile::setPort(int port) {
	_port = port;
}

// void ServerConfigFile::setServerNames(const std::string& server_names) {
// 	Line	names(server_names, " ");
// 	_server_names = names;

// 	for (auto& name : _server_names) {
// 		std::transform(name.begin(), name.end(), name.begin(),
// 			[](unsigned char c) {
// 				return (std::tolower(c));
// 			}
// 		);
// 	}
// }

void ServerConfigFile::addServerName(const std::string& serverName) {
	_server_names.push_back(serverName);
}

void ServerConfigFile::setMethods(bool get, bool post, bool del) {
	_get_header = get;
	_post_header = post;
	_delete_header = del;

	// validateMethods();
}

void ServerConfigFile::setAutoIndex(bool autoindex) {
	_autoindex = autoindex;
}

void ServerConfigFile::setRoot(const std::string& root) {
	_root = toggle_path(root);
}

void ServerConfigFile::setIndexFile(const std::string index_file) {
	_index_file = index_file;
}

// void ServerConfigFile::addErrorPage(int error_code, const std::string& path) {
// 	_error_pages[error_code] = path;
// }

void ServerConfigFile::addErrorPage(int err_code, const std::string& path) {
	// Check if the file exists
	//todo: need to add full path to the file.
	if (std::filesystem::exists(path)) {
		_error_pages.setErrorPageLink(err_code, path); // Replace default error page link
		std::cout << "Replaced default error page for code " << err_code << " with " << path << "\n";
	} else {
		std::cerr << "Error: File does not exist at path: " << path << "\n";
	}
}

void ServerConfigFile::addLocation(const LocationConfigFile& location) {
	_locations.push_back(location);
}

void ServerConfigFile::setRequestBodySize(int size) {
	_request_body_size = size;
}

void ServerConfigFile::addCgiExtension(const std::string& ext, const std::string& path_to_binary) {
	// Validate extension format
	if (ext.empty() || ext[0] != '.') {
		throw std::invalid_argument("CGI extension must start with '.'");
	}

	// Validate allowed characters in extension
	const std::string allowed_chars = "abcdefghijklmnopqrstuvwxyz_";
	if (ext.substr(1).find_first_not_of(allowed_chars) != std::string::npos) {
		throw std::invalid_argument("Invalid CGI extension: " + ext + " !");
	}

	// Validate binary path format
	if (path_to_binary.empty() || path_to_binary[0] != '/' ||
		path_to_binary.find("..") != std::string::npos) {
		throw std::invalid_argument("Invalid CGI binary path: " + path_to_binary);
	}

	// Validate binary is executable
	if (access(path_to_binary.c_str(), X_OK) != 0) {
		throw std::runtime_error("CGI binary not executable: " + path_to_binary);
	}

	_cgi_map[ext] = path_to_binary;
}


// Getters
int ServerConfigFile::getPort() const {
	return _port;
}

// const std::vector<std::string>& ServerConfigFile::getServerNames() const {
// 	return _server_names;
// }

const LocationConfigFile&	ServerConfigFile::getDefaultLocation(void) const {
	return _defaultLocation;
}

const std::vector<std::string>& ServerConfigFile::getServerNames() const {
	return _server_names;
}

bool ServerConfigFile::isGetAllowed() const {
	return _get_header;
}

bool ServerConfigFile::isPostAllowed() const {
	return _post_header;
}

bool ServerConfigFile::isDeleteAllowed() const {
	return _delete_header;
}

bool ServerConfigFile::getAutoIndex() const {
	return _autoindex;
}

const std::string& ServerConfigFile::getRoot() const {
	return _root;
}

// const std::map<int, std::string>& ServerConfigFile::getErrorPages() const {
// 	return _error_pages;
// }

const DefaultErrorPages& ServerConfigFile::getErrorPages() const {
	return _error_pages;
}

const std::vector<LocationConfigFile>& ServerConfigFile::getLocations() const {
	return _locations;
}

int ServerConfigFile::getRequestBodySize() const {
	// std::cout << "ServerConfigFile::getRequestBodySize(): " <<  _request_body_size << std::endl;
	return _request_body_size;
}

const std::map<std::string, std::string>& ServerConfigFile::getCgiExtensions() const {
	return _cgi_map;
}

LocationConfigFile& ServerConfigFile::setDefaultLocation() {
	return _defaultLocation;
}

// Debug
void ServerConfigFile::printServer() const {
	std::cout << "********************************" << std::endl;
	std::cout << "Server Port: " << _port << "\n";
	std::cout << "Server Names:\n";
	for (const auto& name : _server_names) {
		std::cout << "\t" << name << "\n";
	}
	std::cout << "Root Directory: " << _root << "\n";

	// Print error pages
	// std::cout << "Error Pages:\n";
	// _error_pages.printErrorPages();

	// Print locations
	std::cout << "********************************" << std::endl;
	if (!_locations.empty()) {
		std::cout << FT_ANSI_GREEN "Locations:\n" FT_ANSI_RESET FT_ANSI_ITALIC;
		for (const auto& location : _locations) {
			location.printLocation();
			std::cout << "\n";
			std::cout << FT_ANSI_RESET;
			std::cout << "********************************" << std::endl;
		}
	}
	// std::cout << "********************************" << std::endl;
	std::cout << FT_ANSI_GREEN "default location:\n" FT_ANSI_RESET FT_ANSI_ITALIC;
	_defaultLocation.printLocation();
	std::cout << FT_ANSI_RESET;
	std::cout << "********************************" << std::endl;
}

