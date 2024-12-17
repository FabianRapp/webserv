#pragma once

#include <Token.hpp>
#include <msg.hpp>
#include <Parser.hpp>

#include <iostream>
#include <string>

class Webserv {
public:
					Webserv(void);
					Webserv(char * config_file);
					~Webserv(void);
	t_http_request	parse(std::string raw_input);
	void			send(t_http_response msg);
	/*todo: structure to go form 't_http_request' to 't_http_response' */
private:
};
