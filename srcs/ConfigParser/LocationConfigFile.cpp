#include "../../includes/ConfigParser/LocationConfigFile.hpp"
#include <unistd.h>
#include <stdexcept>

LocationConfigFile::LocationConfigFile() {}

// Setters
void LocationConfigFile::setPath(const std::string& path) {
	_path = path;
}

void LocationConfigFile::setMethods(bool get, bool post, bool del, bool put) {
	_get_header = get;
	_post_header = post;
	_delete_header = del;
	_put_header = put;
}

void LocationConfigFile::setAutoIndex(bool autoindex) {
	_autoindex = autoindex;
}

void LocationConfigFile::setIsRedir(bool isredir) {
	_isRedir = isredir;
}

void LocationConfigFile::setRoot(const std::string root) {
	_root = root;
}

void LocationConfigFile::setIndexFile(const std::string index_file) {
	_index_file = index_file;
}

void LocationConfigFile::setRedirection(const std::string redirection) {
	_redireciton = redirection;
}

void LocationConfigFile::setRequestBodySize(int size) {
	_request_body_size = size;
}

void LocationConfigFile::addCgiExtension(const std::string& ext, const std::string& path_to_binary) {
	_cgi_map[ext] = path_to_binary;
}

// Getters
const std::string& LocationConfigFile::getPath() const {
	return _path;
}

bool LocationConfigFile::isGetAllowed() const {
	return _get_header;
}

bool LocationConfigFile::isPostAllowed() const {
	return _post_header;
}

bool LocationConfigFile::isDeleteAllowed() const {
	return _delete_header;
}

bool LocationConfigFile::isPutAllowed() const {
	return _put_header;
}

bool LocationConfigFile::getAutoIndex() const {
	return _autoindex;
}

bool LocationConfigFile::getIsRedir() const {
	return _isRedir;
}

const std::string& LocationConfigFile::getRoot() const{
	return _root;
}

const std::string& LocationConfigFile::getIndexFile() const {
	return _index_file;
}

const std::string& LocationConfigFile::getRedirection() const {
	return _redireciton;
}

int LocationConfigFile::getRequestBodySize() const {
	return _request_body_size;
}

const std::map<std::string, std::string>& LocationConfigFile::getCgiExtensions() const {
	return _cgi_map;
}


void LocationConfigFile::printLocation() const {
	std::cout << "Location Path: " << _path << "\n";

	std::cout << "Allowed Methods: "
				<< (_get_header ? "GET " : "")
				<< (_post_header ? "POST " : "")
				<< (_delete_header ? "DELETE " : "")
				<< (_put_header ? "PUT" : "") << "\n";

	std::cout << "Autoindex: " << (_autoindex ? "on" : "off") << "\n";

	if (!_root.empty()) {
		std::cout << "Root: " << _root << "\n";
	}
	std::cout << "Redirection: " << _redireciton << "\n";
	std::cout << "IS REDIRECTION FLAG ON: " << _isRedir << "\n";
	std::cout << "Index File: " << _index_file << "\n";

	if (_request_body_size != -1) {
		std::cout << "Request Body Size: " << _request_body_size << " bytes\n";
	} else {
		std::cout << "Request Body Size: Not set\n";
	}

	if (!_cgi_map.empty()) {
		std::cout << "CGI Extensions:\n";
		for (const auto& [ext, path] : _cgi_map) {
			std::cout << "\tExtension: " << ext << ", Path to Binary: " << path << "\n";
		}
	} else {
		std::cout << "CGI Extensions: None\n";
	}
}

