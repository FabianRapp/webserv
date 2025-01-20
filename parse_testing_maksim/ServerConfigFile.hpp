#ifndef SERVER_CONFIG_FILE_HPP
#define SERVER_CONFIG_FILE_HPP

#include <map>
#include <vector>
#include "LocationConfigFile.hpp"

class ServerConfigFile {
private:
	int _port = 0;
	std::string _server_name = "";
	std::string _root;
	std::map<int, std::string> _error_pages;
	std::vector<LocationConfigFile> _locations;

public:
	ServerConfigFile();

	// setters
	void setPort(int port);
	void setServerName(const std::string& server_name);
	void setRoot(const std::string& root);
	void addErrorPage(int err_code, const std::string& path);
	void addLocation(const LocationConfigFile& location);

	int getPort() const;
	const std::string& getServerName() const;
	const std::string& getRoot() const;
	const std::map<int, std::string>& getErrorPages() const;
	const std::vector<LocationConfigFile>& getLocations() const;

	void printServer() const;

};

#endif
