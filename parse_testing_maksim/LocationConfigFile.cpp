#include "LocationConfigFile.hpp"

LocationConfigFile::LocationConfigFile() {}

// Setters
void LocationConfigFile::setPath(const std::string& path) {
	_path = path;
}

void LocationConfigFile::setMethods(bool get, bool post, bool del) {
	_get_header = get;
	_post_header = post;
	_delete_header = del;
}

void LocationConfigFile::setAutoIndex(bool autoindex) {
	_autoindex = autoindex;
}

void LocationConfigFile::setUploadDir(const std::string upload_dir) {
	_upload_dir = upload_dir;
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

const std::string& LocationConfigFile::getUploadDir() const{
	return _upload_dir;
}

const std::string& LocationConfigFile::getIndexFile() const {
	return _index_file;
}

// Debugging utility
void LocationConfigFile::printLocation() const {
	std::cout << "Location Path: " << _path << "\n";
	std::cout << "Allowed Methods: "
				<< (_get_header ? "GET " : "")
				<< (_post_header ? "POST " : "")
				<< (_delete_header ? "DELETE" : "") << "\n";
	std::cout << "Autoindex: " << (_autoindex ? "on" : "off") << "\n";
	if (!_upload_dir.empty()) {
		std::cout << "Upload Directory: " << _upload_dir << "\n";
	}
	std::cout << "Index File: " << _index_file << "\n";
}
