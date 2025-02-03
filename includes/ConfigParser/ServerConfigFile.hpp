#ifndef SERVER_CONFIG_FILE_HPP
#define SERVER_CONFIG_FILE_HPP

#include <map>
#include <vector>
#include <filesystem>
#include "LocationConfigFile.hpp"
#include "DefaultErrorPages.hpp"
#include <cstdlib>

class ServerConfigFile {
private:
	int _port = 0;
	std::vector<std::string>	_server_names;
	std::string _root;
	bool _get_header = true;
	bool _post_header = true;
	bool _delete_header = true;
	int _client_body_size = -1;
	// std::map<int, std::string> _error_pages;
	std::string _index_file = "index.html";
	DefaultErrorPages _error_pages;
	std::vector<LocationConfigFile> _locations;

public:
	ServerConfigFile();

	// setters
	void setPort(int port);
	void setServerNames(const std::string& server_names);
	void setRoot(const std::string& root);
	void setIndexFile(const std::string index_file);
	// void addErrorPage(int err_code, const std::string& path);
	void addErrorPage(int err_code, const std::string& path);
	void addLocation(const LocationConfigFile& location);
	void setClientBodySize(int size);

	int getPort() const;
	const std::vector<std::string>& getServerNames() const;
	const std::string& getRoot() const;
	// const std::map<int, std::string>& getErrorPages() const;
	const DefaultErrorPages& getErrorPages() const;
	const std::vector<LocationConfigFile>& getLocations() const;
	int getClientBodySize() const;

	void printServer() const;

};

#endif
