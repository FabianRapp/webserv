#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <iostream>
#include <string>

class LocationConfigFile {

private:
	std::string _path = "/";
	bool _get_header = false;
	bool _post_header = false;
	bool _delete_header = false;
	bool _autoindex = false;
	std::string _root = "";
	std::string _index_file = "index.html";

public:
	LocationConfigFile();

	void setPath(const std::string& path);
	void setMethods(bool get, bool post, bool del);
	void setAutoIndex(bool autoindex);
	void setRoot(const std::string root);
	void setIndexFile(const std::string index_file);

	const std::string& getPath() const;
	bool isGetAllowed() const;
	bool isPostAllowed() const;
	bool isDeleteAllowed() const;
	bool getAutoIndex() const;
	const std::string& getRoot() const;
	const std::string& getIndexFile() const;

	// void validateMethods() const;

	//todo: might need to delete this before eval.
	void printLocation() const;

};

#endif
