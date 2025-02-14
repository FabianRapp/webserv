#include "../../includes/ConfigParser/DefaultErrorPages.hpp"
#include <colors.h>

DefaultErrorPages::DefaultErrorPages() {
	// 1xx Informational Responses
	_error_pages[100] = "./default/error_pages/100.html"; // Continue
	_error_pages[101] = "./default/error_pages/101.html"; // Switching Protocols
	_error_pages[102] = "./default/error_pages/102.html"; // Processing (WebDAV)
	_error_pages[103] = "./default/error_pages/103.html"; // Early Hints

	// 2xx Success Responses
	_error_pages[200] = "./default/error_pages/200.html"; // OK
	_error_pages[201] = "./default/error_pages/201.html"; // Created
	_error_pages[202] = "./default/error_pages/202.html"; // Accepted
	_error_pages[203] = "./default/error_pages/203.html"; // Non-Authoritative Information
	_error_pages[204] = "./default/error_pages/204.html"; // No Content
	_error_pages[205] = "./default/error_pages/205.html"; // Reset Content
	_error_pages[206] = "./default/error_pages/206.html"; // Partial Content
	_error_pages[207] = "./default/error_pages/207.html"; // Multi-Status (WebDAV)
	_error_pages[208] = "./default/error_pages/208.html"; // Already Reported (WebDAV)
	_error_pages[226] = "./default/error_pages/226.html"; // IM Used

	// 3xx Redirection Responses
	_error_pages[300] = "./default/error_pages/300.html"; // Multiple Choices
	_error_pages[301] = "./default/error_pages/301.html"; // Moved Permanently
	_error_pages[302] = "./default/error_pages/302.html"; // Found (Previously "Moved Temporarily")
	_error_pages[303] = "./default/error_pages/303.html"; // See Other
	_error_pages[304] = "./default/error_pages/304.html"; // Not Modified
	_error_pages[305] = "./default/error_pages/305.html"; // Use Proxy
	_error_pages[307] = "./default/error_pages/307.html"; // Temporary Redirect
	_error_pages[308] = "./default/error_pages/308.html"; // Permanent Redirect

	// 4xx Client Errors
	_error_pages[400] = "./default/error_pages/400.html"; // Bad Request
	_error_pages[401] = "./default/error_pages/401.html"; // Unauthorized
	_error_pages[402] = "./default/error_pages/402.html"; // Payment Required
	_error_pages[403] = "./default/error_pages/403.html"; // Forbidden
	_error_pages[404] = "./default/error_pages/404.html"; // Not Found
	_error_pages[405] = "./default/error_pages/405.html"; // Method Not Allowed
	_error_pages[406] = "./default/error_pages/406.html"; // Not Acceptable
	_error_pages[407] = "./default/error_pages/407.html"; // Proxy Authentication Required
	_error_pages[408] = "./default/error_pages/408.html"; // Request Timeout
	_error_pages[409] = "./default/error_pages/409.html"; // Conflict
	_error_pages[410] = "./default/error_pages/410.html"; // Gone
	_error_pages[411] = "./default/error_pages/411.html"; // Length Required
	_error_pages[412] = "./default/error_pages/412.html"; // Precondition Failed
	_error_pages[413] = "./default/error_pages/413.html"; // Payload Too Large
	_error_pages[414] = "./default/error_pages/414.html"; // URI Too Long
	_error_pages[415] = "./default/error_pages/415.html"; // Unsupported Media Type
	_error_pages[416] = "./default/error_pages/416.html"; // Range Not Satisfiable
	_error_pages[417] = "./default/error_pages/417.html"; // Expectation Failed
	_error_pages[418] = "./default/error_pages/418.html"; // I'm a teapot (RFC 2324)
	_error_pages[421] = "./default/error_pages/421.html"; // Misdirected Request
	_error_pages[422] = "./default/error_pages/422.html"; // Unprocessable Content (WebDAV)
	_error_pages[423] = "./default/error_pages/423.html"; // Locked (WebDAV)
	_error_pages[424] = "./default/error_pages/424.html"; // Failed Dependency (WebDAV)
	_error_pages[425] = "./default/error_pages/425.html"; // Too Early (RFC 8470)
	_error_pages[426] = "./default/error_pages/426.html"; // Upgrade Required
	_error_pages[428] = "./default/error_pages/428.html"; // Precondition Required (RFC 6585)
	_error_pages[429] = "./default/error_pages/429.html"; // Too Many Requests (RFC 6585)
	_error_pages[431] = "./default/error_pages/431.html"; // Request Header Fields Too Large (RFC 6585)
	_error_pages[451] = "./default/error_pages/451.html"; // Unavailable For Legal Reasons (RFC 7725)
	_error_pages[495] = "./default/error_pages/495.html"; // SSL Certificate Error
	_error_pages[496] = "./default/error_pages/496.html"; // SSL Certificate Required
	_error_pages[497] = "./default/error_pages/497.html"; // HTTP Request Sent to HTTPS Port
	_error_pages[498] = "./default/error_pages/498.html"; // Token expired/invalid
	_error_pages[499] = "./default/error_pages/499.html"; // Client Closed Request

	// 5xx Server Errors
	_error_pages[500] = "./default/error_pages/500.html"; // Internal Server Error
	_error_pages[501] = "./default/error_pages/501.html"; // Not Implemented
	_error_pages[502] = "./default/error_pages/502.html"; // Bad Gateway
	_error_pages[503] = "./default/error_pages/503.html"; // Service Unavailable
	_error_pages[504] = "./default/error_pages/504.html"; // Gateway Timeout
	_error_pages[505] = "./default/error_pages/505.html"; // HTTP Version Not Supported
	_error_pages[506] = "./default/error_pages/506.html"; // Variant Also Negotiates (RFC 2295)
	_error_pages[507] = "./default/error_pages/507.html"; // Insufficient Storage (WebDAV)
	_error_pages[508] = "./default/error_pages/508.html"; // Loop Detected (WebDAV)
	_error_pages[510] = "./default/error_pages/510.html"; // Not Extended (RFC 2774)
	_error_pages[511] = "./default/error_pages/511.html"; // Network Authentication Required (RFC 6585)
}

DefaultErrorPages::DefaultErrorPages(const DefaultErrorPages& old) 
	: _error_pages(old._error_pages) {}

// Copy assignment operator
DefaultErrorPages& DefaultErrorPages::operator=(const DefaultErrorPages& old) {
	if (this != &old) {
		_error_pages = old._error_pages;
	}
	return *this;
}

const std::string DefaultErrorPages::getErrorPageLink(int err_code) const {
	auto it = _error_pages.find(err_code);

	if(it != _error_pages.end()) {
		return it->second;
	} else {
		throw (std::ios_base::failure("Status page for code("
			+ std::to_string(err_code) + ") not found"));
	}
	return 0;
}

void DefaultErrorPages::setErrorPageLink(int err_code, std::string link) {
	auto it = _error_pages.find(err_code);

	if(it != _error_pages.end()) {
		it->second = link;
	} else {
		throw (DefaultErrorPagesError("Status page for code("+ std::to_string(err_code) + ") not found"));
	}

}

void DefaultErrorPages::printErrorPages() const {
	for (const auto& [code, path] : _error_pages) {
		if( code == 404) {
		std::cout << FT_ANSI_YELLOW << "Error Code " << code << ": " << path << FT_ANSI_RESET << "\n";
		}
	}
}
