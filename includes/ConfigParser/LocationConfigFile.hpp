#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <iostream>
#include <string>
#include <map>

class LocationConfigFile {

private:
	std::string _path = "/";
	bool _get_header = false;
	bool _post_header = false;
	bool _delete_header = false;
	bool _put_header = false;
	bool _autoindex = false;
	int _request_body_size = -1;
	bool _isRedir = false;
	std::map<std::string, std::string> _cgi_map;
	std::string _root = "";
	std::string _index_file = "index.html";
	std::string _redireciton = "";

public:
	LocationConfigFile();
    // Copy constructor
    LocationConfigFile(const LocationConfigFile& old)
        : _path(old._path),
          _get_header(old._get_header),
          _post_header(old._post_header),
          _delete_header(old._delete_header),
          _put_header(old._put_header),
          _autoindex(old._autoindex),
          _request_body_size(old._request_body_size),
          _isRedir(old._isRedir),
          _cgi_map(old._cgi_map),
          _root(old._root),
          _index_file(old._index_file),
          _redireciton(old._redireciton) {}

    // Assignment operator
    LocationConfigFile& operator=(const LocationConfigFile& old) {
        if (this == &old) {
            return *this;
        }

        _path = old._path;
        _get_header = old._get_header;
        _post_header = old._post_header;
        _delete_header = old._delete_header;
        _put_header = old._put_header;
        _autoindex = old._autoindex;
        _request_body_size = old._request_body_size;
        _isRedir = old._isRedir;
        _cgi_map = old._cgi_map;
        _root = old._root;
        _index_file = old._index_file;
        _redireciton = old._redireciton;

        return *this;
    }
	void setPath(const std::string& path);
	void setMethods(bool get, bool post, bool del, bool put);
	void setAutoIndex(bool autoindex);
	void setIsRedir(bool isredir);
	void setRoot(const std::string root);
	void setIndexFile(const std::string index_file);
	void setRedirection(const std::string redirection);
	void setRequestBodySize(int size);
	void addCgiExtension(const std::string& ext, const std::string& path_to_binary);

	const std::string& getPath() const;
	bool isGetAllowed() const;
	bool isPostAllowed() const;
	bool isDeleteAllowed() const;
	bool isPutAllowed() const;
	bool getAutoIndex() const;
	bool getIsRedir() const;
	const std::string& getRoot() const;
	const std::string& getIndexFile() const;
	const std::string& getRedirection() const;
	int getRequestBodySize() const;
	const std::map<std::string, std::string>& getCgiExtensions() const;

	void printLocation() const;

};

#endif
