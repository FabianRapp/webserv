#pragma once

#include <string>
#include <enums.hpp>


/* todo: */
typedef struct {
	std::string	str;
}	t_version;

/*todo: this is only a rough basic idea */
typedef struct http_request {
	MethodType	type;
	std::string	uri;
	t_version	version;
	/* todo: headers? */
	std::string	body;
}	t_http_request;

