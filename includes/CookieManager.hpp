#pragma once

#include <unordered_map>
#include <Request.hpp>
#include <string>
#include <chrono>
#include <iostream>


struct Cookie {
	Cookie(void) {}
	Cookie(const Request& request);
	Cookie(const Cookie& old);
	Cookie&	operator=(const Cookie& old);

	~Cookie(void);

	std::string								value;
	std::chrono::system_clock::time_point	expiration_time;
	std::string								server_name;
	std::string								uri;
	bool									secure;//not used
};

std::ostream	&operator<<(std::ostream &output, const Cookie &cookie);

class CookieManager {
public:
	CookieManager(void);
	~CookieManager(void);

	void			add_cookies(const std::string& cgi_output, const Request& request);

	bool			valid_cookie(const std::string& name, const Request& request);

	void			remove_expired_cookies(void);
	void			print_cookies(void);
private:
	std::unordered_map<std::string, struct Cookie>	_cookies;

	void	_add_cookie(const std::string& cookie_para_str, const Request& request);
};
