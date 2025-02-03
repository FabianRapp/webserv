#include "../../includes/ConfigParser/LocationConfigFile.hpp"

LocationConfigFile::LocationConfigFile() {}



// Setters
void LocationConfigFile::setPath(const std::string& path) {
	_path = path;
}

void LocationConfigFile::setMethods(bool get, bool post, bool del) {
	_get_header = get;
	_post_header = post;
	_delete_header = del;

	// validateMethods();
}

void LocationConfigFile::setAutoIndex(bool autoindex) {
	_autoindex = autoindex;
}

void LocationConfigFile::setRoot(const std::string root) {
	_root = root;
}

void LocationConfigFile::setIndexFile(const std::string index_file) {
	_index_file = index_file;
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

const std::string& LocationConfigFile::getRoot() const{
	return _root;
}

const std::string& LocationConfigFile::getIndexFile() const {
	return _index_file;
}

// void LocationConfigFile::validateMethods() const {
// 	if (!_get_header && !_post_header && !_delete_header) {
// 		throw std::runtime_error("Invalid or no methods provided.");
// 	}
// }


//todo: delete this later. Debugging utility
void LocationConfigFile::printLocation() const {
	std::cout << "Location Path: " << _path << "\n";
	std::cout << "Allowed Methods: "
				<< (_get_header ? "GET " : "")
				<< (_post_header ? "POST " : "")
				<< (_delete_header ? "DELETE" : "") << "\n";
	std::cout << "Autoindex: " << (_autoindex ? "on" : "off") << "\n";
	if (!_root.empty()) {
		std::cout << "Upload Directory: " << _root << "\n";
	}
	std::cout << "Index File: " << _index_file << "\n";
}
