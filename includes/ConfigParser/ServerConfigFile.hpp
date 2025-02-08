#ifndef SERVER_CONFIG_FILE_HPP
#define SERVER_CONFIG_FILE_HPP

#include <map>
#include <vector>
#include <algorithm>
#include <filesystem>
#include "LocationConfigFile.hpp"
#include "DefaultErrorPages.hpp"
#include <cstdlib>

class ServerConfigFile {
private:
	int _port = 0;
	std::vector<std::string> _server_names;
	std::string _root;
	bool _get_header = false;
	bool _post_header = false;
	bool _delete_header = false;
	bool _autoindex = false;
	int _request_body_size = -1;
	// std::map<int, std::string> _error_pages;
	std::string _index_file = "index.html";
	DefaultErrorPages _error_pages;
	std::vector<LocationConfigFile> _locations;
	LocationConfigFile				_defaultLocation;

public:
	ServerConfigFile();

	// setters
	void setPort(int port);
	// void setServerNames(const std::string& server_names);
	void addServerName(const std::string& _server_names);
	void setRoot(const std::string& root);
	void setIndexFile(const std::string index_file);
	void setMethods(bool get, bool post, bool del);
	void setAutoIndex(bool autoindex);
	// void addErrorPage(int err_code, const std::string& path);
	void addErrorPage(int err_code, const std::string& path);
	void addLocation(const LocationConfigFile& location);
	void setRequestBodySize(int size);

	int getPort() const;
	const LocationConfigFile&	getDefaultLocation(void) const;

	// In ServerConfigFile class declaration (header)

	// LocationConfigFile& getDefaultLocation() { return _defaultLocation; }
	// const LocationConfigFile& getDefaultLocation() const { return _defaultLocation; }

	// const std::vector<std::string>& getServerNames() const;
	const std::vector<std::string>& getServerNames() const;
	bool isGetAllowed() const;
	bool isPostAllowed() const;
	bool isDeleteAllowed() const;
	const std::string& getRoot() const;
	// const std::map<int, std::string>& getErrorPages() const;
	bool getAutoIndex() const;
	const DefaultErrorPages& getErrorPages() const;
	const std::vector<LocationConfigFile>& getLocations() const;
	int getRequestBodySize() const;
	LocationConfigFile& setDefaultLocation();

	void printServer() const;

};

#endif
