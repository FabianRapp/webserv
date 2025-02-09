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
	std::map<std::string, std::string> _cgi_map;
	std::string _root = "";
	std::string _index_file = "index.html";

public:
	LocationConfigFile();

	void setPath(const std::string& path);
	void setMethods(bool get, bool post, bool del, bool put);
	void setAutoIndex(bool autoindex);
	void setRoot(const std::string root);
	void setIndexFile(const std::string index_file);
	void addCgiExtension(const std::string& ext, const std::string& path_to_binary);

	const std::string& getPath() const;
	bool isGetAllowed() const;
	bool isPostAllowed() const;
	bool isDeleteAllowed() const;
	bool isPutAllowed() const;
	bool getAutoIndex() const;
	const std::string& getRoot() const;
	const std::string& getIndexFile() const;
	const std::map<std::string, std::string>& getCgiExtensions() const;

	// void validateMethods() const;

	//todo: might need to delete this before eval.
	void printLocation() const;

};

#endif
