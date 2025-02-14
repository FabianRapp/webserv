#ifndef DEFAULT_ERROR_PAGES_HPP
#define DEFAULT_ERROR_PAGES_HPP

#include <map>
#include <iostream>

class DefaultErrorPagesError: public std::runtime_error{
public:
	DefaultErrorPagesError(const std::string& msg): std::runtime_error(msg){}
};

class DefaultErrorPages {
private:
	std::map<int, std::string> _error_pages;

public:
	DefaultErrorPages();
	DefaultErrorPages(const DefaultErrorPages& old): _error_pages(old._error_pages){}

	//getter
	const std::string getErrorPageLink(int err_code) const;
	//setter
	void setErrorPageLink(int err_code, std::string link);

	void printErrorPages() const;
};

#endif
