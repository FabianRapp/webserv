#pragma once

#include <macros.h>
#include <iostream>
#include <string>

/**************************** For http request ********************************/

/* for me the 'HOST' type is missing but as i read the docs these current
 * types align more with the docs?*/
enum class TokenType {
	METHOD,
	URI,
	VERSION,
	HEADER,
	HEADER_TERMINATION,
	BODY,
	BODY_TERMINATION,
	EOF_TOKEN,
};

enum class HeaderType {
	ACCEPT,
	ACCEPT_CHARSET,
	ACCEPT_ENCODING,
	ACCEPT_LANGUAGE,
	AUTHORIZATION,
	EXPECT,
	FROM,
	HOST,
	IF_MATCH,
	IF_MODIFIED_SINCE,
	IF_NONE_MATCH,
	IF_RANGE,
	IF_UNMODIFIED_SINCE,
	MAX_FORWARDS,
	PROXY_AUTHORIZATION,
	RANGE,
	REFERER,
	TE,
	USER_AGENT,
};

enum class MethodType {
	OPTIONS,
	GET,
	HEAD,
	POST,
	PUT,
	DELETE,
	TRACE,
	CONNECT,
};

/**************************** to_string prototypes ****************************/
std::string	to_string(HeaderType val);
std::string	to_string(TokenType val);
std::string	to_string(MethodType val);
