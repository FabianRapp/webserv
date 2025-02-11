#include <CookieManager.hpp>
#include <main.hpp>
#include <StringArray.hpp>
#include <ctime>


Cookie::Cookie(const Request& request):
		value(""),
		expiration_time(std::chrono::system_clock::now() + std::chrono::hours(100000000)),
		server_name(""),
		secure(false)
{
	if (request._headers.find(HeaderType::HOST) != request._headers.end()) {
		server_name = request._headers.at(HeaderType::HOST);
	}
	uri = request._uri;
}

Cookie::Cookie(const Cookie& old):
		value(old.value),
		expiration_time(old.expiration_time),
		server_name(old.server_name),
		uri(old.uri),
		secure(old.secure)
{
}

Cookie&	Cookie::operator=(const Cookie& old) {
	if (this == &old) {
		return (*this);
	}
	value = old.value;
	expiration_time = old.expiration_time;
	server_name = old.server_name;
	uri = old.uri;
	secure = old.secure;
	return (*this);
}

Cookie::~Cookie(void) {
}

std::ostream	&operator<<(std::ostream &output, const Cookie &cookie) {
	std::time_t	extime = std::chrono::system_clock::to_time_t(cookie.expiration_time);
	output << "value: " << cookie.value
		<< "; server_name: " << cookie.server_name
		<< "; expiration_time: " << std::ctime(&extime)
		//<< "; uri: " << cookie.uri
		//<< "; secure: " << (cookie.secure ? "true" : "false")
	;
	return (output);
}

CookieManager::CookieManager(void)
{
}

CookieManager::~CookieManager(void) {
}


void	CookieManager::add_cookies(const std::string& cgi_output,
			const Request& request)
{
	const std::string	header = "\r\nSet-Cookie: ";
	size_t	pos = 0;
	size_t	body_start = cgi_output.find("\r\n\r\r");

	while (1) {
		size_t	set_pos = cgi_output.find(header, pos);
		if (set_pos == std::string::npos) {
			LOG_FABIAN("No more set cookie header found\n");
			return ;
		}
		if (body_start < set_pos) {
			LOG_FABIAN("Set cookie header found in body?\n");
			return ;
		}
		size_t	set_end = cgi_output.find("\r\n", set_pos + header.length());
		if (set_end == std::string::npos) {
			return ;
		}
		set_pos += + header.length();
		_add_cookie(cgi_output.substr(set_pos, set_end - set_pos), request);
		pos = set_end;
	}
}

bool	CookieManager::valid_cookie(const std::string& name,
					const Request& request)
{
	std::string	host;
	if (request._headers.find(HeaderType::HOST) != request._headers.end()) {
		host = request._headers.at(HeaderType::HOST);
	}
	auto it = _cookies.find(name);
	if (it != _cookies.end()
		&& request._uri.find(it->second.uri) == 0
		&& host == it->second.server_name
	){
		return (true);
	}
	return (false);
}

void	CookieManager::remove_expired_cookies(void) {
	std::chrono::system_clock::time_point	cur_time =
		std::chrono::system_clock::now();

	for (auto it = _cookies.begin(); it != _cookies.end(); ) {
		if (cur_time > it->second.expiration_time) {
			LOG(FT_ANSI_MAGENTA_BOLD_UNDERLINE 
				"Cookie: Removed {" << it->first << "}: "
				<< it->second << FT_ANSI_RESET);
			it = _cookies.erase(it);
		} else {
			++it;
		}
	}
}

void	CookieManager::_add_cookie(const std::string& cookie_para_str,
			const Request& request)
{
	LOG("header_val: |" << cookie_para_str << "|\n");
	Line	cookie_paras(cookie_para_str, ";");
	if (cookie_paras.empty()) {
		return ;
	}
	const std::string	keywords[] = {
		"Expires=",
		"Max-Age=",
		"Domain=",
		"Path=",
		"Secure",
		"HttpOnly",
		"SameSite=",
	};
	struct Cookie	cookie(request);
	std::string		key;

	int i = 0;//i is to tix a bug in Line class
	for (auto& para : cookie_paras) {
		if (i++ && !para.empty()) {
			para = para.substr(1, para.size() - 1);
		}
		bool	is_name = true;
		for (const auto& keyword : keywords) {
			if (para.find(keyword) == 0) {
				LOG_FABIAN3("matched set cookie keyword: " << keyword << std::endl);
				std::string	value = para.substr(keyword.length(), para.length() - keyword.length());
				is_name = false;
				if (keyword == "Expires=") {
				} else if (keyword == "Max-Age=") {
					try {
						int	seconds = std::stoi(value);
						cookie.expiration_time = std::chrono::system_clock::now()
							+ std::chrono::seconds(seconds);
					} catch (const std::invalid_argument&) {
						LOG("invalid 'Max-Age' from 'SET-COOKIE' header");
						return ;
					} catch (const std::out_of_range&) {
						LOG("invalid 'Max-Age' from 'SET-COOKIE' header");
						return ;
					}
				} else if (keyword == "Domain=") {
					cookie.server_name = value;
				} else if (keyword == "Path=") {
					cookie.uri = value;
				} else if (keyword == "Secure") {
					cookie.secure = true;
				} else if (keyword == "HttpOnly") {
				} else if (keyword == "SameSite=") {
				}
				break ;
			}
		}
		if (!is_name) {
			continue ;
		}
		if (!key.empty()) {
			LOG("invalid SET-COOKIE header(multiple names?)\n");
			return ;
		}
		size_t	eq_pos = para.find("=");
		if (eq_pos == std::string::npos) {
			LOG("invalid SET-COOKIE header(name has no '=')\n");
			return ;
		}
		key = para.substr(0, eq_pos);
		cookie.value = para.substr(eq_pos + 1, para.size() - eq_pos + 1);
	}
	_cookies[key] = cookie;
	LOG(FT_ANSI_MAGENTA_BOLD_UNDERLINE "Cookie: Added {" << key << "}: "
		<< cookie << FT_ANSI_RESET);
}

void	CookieManager::print_cookies(void) {
	for (const auto& [key, cookie] : _cookies) {
		std::cerr << "cookie{" << key << "}: " << cookie << std::endl;
	}
}
