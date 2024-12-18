#pragma once

#include <Token.hpp>
#include <msg.hpp>
#include <Parser.hpp>

#include <unistd.h>
#include <cstring>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdnoreturn.h>
#include <iostream>
#include <string>

#ifndef REQUEST_QUE_SIZE
# define REQUEST_QUE_SIZE 10
#endif

class Webserv {
public:
					Webserv(void);
					Webserv(const char * config_file_path);
					~Webserv(void);
	[[noreturn]]
	void			run(void);
private:
	t_http_request	_parse(std::string raw_input);
	void			_send(t_http_response msg);
	/* todo: structure to go form 't_http_request' to 't_http_response' */

private:
	int					_server_fd;
	struct sockaddr_in	_server_addr;
	struct sockaddr_in	_client_addr;
	socklen_t			_addr_len;
};
