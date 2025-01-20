#ifndef DEFAULT_ERROR_PAGES_HPP
#define DEFAULT_ERROR_PAGES_HPP

#include <map>

class DefaultErrorPages {
private:
	std::map<int, std::string> _error_pages;

public:
	DefaultErrorPages();

	void addErrorPage(int err_code, const std::string& path);

};

#endif
