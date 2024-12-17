#pragma once

#include <Token.hpp>
#include <msg.hpp>
#include <Parser.hpp>

#include <stdnoreturn.h>
#include <iostream>
#include <string>

class Webserv {
public:
					Webserv(void);
					Webserv(const char * config_file_path);
					~Webserv(void);
	noreturn
	void	run(void);
private:
	t_http_request	_parse(std::string raw_input);
	void			_send(t_http_response msg);
	/* todo: structure to go form 't_http_request' to 't_http_response' */

private:
};
