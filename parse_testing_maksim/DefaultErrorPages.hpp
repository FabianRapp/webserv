#ifndef DEFAULT_ERROR_PAGES_HPP
#define DEFAULT_ERROR_PAGES_HPP

#include <map>
#include <iostream>

class DefaultErrorPages {
private:
	std::map<int, std::string> _error_pages;

public:
	DefaultErrorPages();

	//setter
	// void addErrorPage(int err_code, const std::string& path);

	// //getter
	// const std::string& getErrorPage(int err_code) const;

	// //debug
	// void printErrorPages() const;


	//getter
	const std::string getErrorPageLink(int err_code) const;
	//setter
	void setErrorPageLink(int err_code, std::string link);

	void printErrorPages() const;
};

#endif
