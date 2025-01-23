#pragma once

#include "macros.h"
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
	EOF_TOKEN, /* not actually part of the request, inserted by lexer/parser. */
	UNFINISHED, /* Input is finished but token is not complete */
};

/* we most likely don't need all of these */
enum class HeaderType {
	// Request Headers
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

	/* todo: from here onwards header types by chatgpt, needs verification */
	// General Headers
	CACHE_CONTROL,
	CONNECTION, /* example "close" */
	DATE,
	PRAGMA,
	TRAILER,
	TRANSFER_ENCODING,
	UPGRADE,
	VIA,
	WARNING,

	// Entity Headers
	ALLOW,
	CONTENT_ENCODING,
	CONTENT_LANGUAGE,
	CONTENT_LENGTH,
	CONTENT_LOCATION,
	CONTENT_MD5, // Deprecated
	CONTENT_RANGE,
	CONTENT_TYPE,
	EXPIRES,
	LAST_MODIFIED,

	// Extension Headers
	COOKIE,			   // Common custom header
	SET_COOKIE,		   // Often used in requests for session handling
	DNT,				  // "Do Not Track" header
	ORIGIN,			   // Cross-Origin Resource Sharing (CORS)
	UPGRADE_INSECURE_REQUESTS, // Browser security preference

	// Additional Optional Headers
	ACCEPT_DATETIME,	  // Rare, for datetime-specific requests
	EARLY_DATA,			// Introduced in HTTP/2 extensions
	INVALID
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
	INVALID,
};

/**************************** to_string prototypes ****************************/
std::string	to_string(HeaderType val);
std::string	to_string(TokenType val);
std::string	to_string(MethodType val);
