#pragma once

#include <string>
#include <enums.hpp>


/* todo: this is just a place holder */
 struct s_version {
	std::string	str;

	s_version(void)
	:	str("")
	{
	}
	s_version(const std::string & version_str)
	:	str(version_str)
	{
		/* todo: parse version */
	}

	s_version(const struct s_version & old)
	:	str(old.str)
	{
	}
	struct s_version &	operator=(const struct s_version & right) {
		if (this == &right) {
			return (*this);
		}
		str = right.str;
		return (*this);
	}
};

/* todo: this is just a place holder */
/* todo: uri refactore to a simple pair made by lexer */
struct s_uri {
	std::string	str;

	s_uri(void)
	:	str("")
	{
	}
	s_uri(const std::string & uri_str)
	:	str(uri_str)
	{
		/* todo: parse uri */
	}

	s_uri(const struct s_uri & old)
	:	str(old.str)
	{
	}
	struct s_uri &	operator=(const struct s_uri & right) {
		if (this == &right) {
			return (*this);
		}
		str = right.str;
		return (*this);
	}
};

/*todo: this is only a rough basic idea */
typedef struct http_request {
	bool				finished = false;
	MethodType			type = MethodType::INVALID;
	struct s_uri		uri;
	struct s_version	version;
	size_t				body_len = 0;
	/* todo: all header kinds */
	std::string			body = "";
}	t_http_request;

