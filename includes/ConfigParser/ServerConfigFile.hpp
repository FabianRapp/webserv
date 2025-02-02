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
	// std::map<int, std::string> _error_pages;
	DefaultErrorPages _error_pages;
	std::vector<LocationConfigFile> _locations;

public:
	ServerConfigFile();

	// setters
	void setPort(int port);
	void setServerNames(const std::string& server_names);
	void setRoot(const std::string& root);
	// void addErrorPage(int err_code, const std::string& path);
	void addErrorPage(int err_code, const std::string& path);
	void addLocation(const LocationConfigFile& location);

	int getPort() const;
	const std::vector<std::string>& getServerNames() const;
	const std::string& getRoot() const;
	// const std::map<int, std::string>& getErrorPages() const;
	const DefaultErrorPages& getErrorPages() const;
	const std::vector<LocationConfigFile>& getLocations() const;

	void printServer() const;

};

#endif
