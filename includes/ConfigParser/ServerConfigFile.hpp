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
	std::vector<std::string> _server_names;
	std::string _root;
	bool _get_header = false;
	bool _post_header = false;
	bool _delete_header = false;
	bool _put_header = false;
	bool _autoindex = false;
	int _request_body_size = -1;
	std::map<std::string, std::string> _cgi_map;
	std::string _index_file = "index.html";
	DefaultErrorPages _error_pages;
	std::vector<LocationConfigFile> _locations;
	LocationConfigFile				_defaultLocation;

public:
	ServerConfigFile();
	ServerConfigFile(const ServerConfigFile& old)
		: _port(old._port),
			_server_names(old._server_names),
			_root(old._root),
			_get_header(old._get_header),
			_post_header(old._post_header),
			_delete_header(old._delete_header),
			_put_header(old._put_header),
			_autoindex(old._autoindex),
			_request_body_size(old._request_body_size),
			_cgi_map(old._cgi_map),
			_index_file(old._index_file),
			_error_pages(old._error_pages),
			_locations(old._locations),
			_defaultLocation(old._defaultLocation) {}

	ServerConfigFile& operator=(const ServerConfigFile& old) {
		if (this == &old) {
			return *this;
		}

		_port = old._port;
		_server_names = old._server_names;
		_root = old._root;
		_get_header = old._get_header;
		_post_header = old._post_header;
		_delete_header = old._delete_header;
		_put_header = old._put_header;
		_autoindex = old._autoindex;
		_request_body_size = old._request_body_size;
		_cgi_map = old._cgi_map;
		_index_file = old._index_file;
		_error_pages = old._error_pages;
		_locations = old._locations;
		_defaultLocation = old._defaultLocation;

		return *this;
	}
	// setters
	void setPort(int port);
	void addServerName(const std::string& _server_names);
	void setMethods(bool get, bool post, bool del, bool put);
	void setAutoIndex(bool autoindex);
	void setRoot(const std::string& root);
	void setIndexFile(const std::string index_file);
	void addErrorPage(int err_code, const std::string& path);
	void addLocation(const LocationConfigFile& location);
	void setRequestBodySize(int size);
	void addCgiExtension(const std::string& ext, const std::string& path_to_binary);
	const LocationConfigFile&	getDefaultLocation(void) const;

	// getters
	int getPort() const;
	const std::vector<std::string>& getServerNames() const;
	bool isGetAllowed() const;
	bool isPostAllowed() const;
	bool isDeleteAllowed() const;
	bool isPutAllowed() const;
	const std::string& getRoot() const;
	bool getAutoIndex() const;
	const DefaultErrorPages& getErrorPages() const;
	const std::vector<LocationConfigFile>& getLocations() const;
	int getRequestBodySize() const;
	const std::map<std::string, std::string>& getCgiExtensions() const;

	void printServer() const;

	LocationConfigFile& setDefaultLocation();
};

#endif
