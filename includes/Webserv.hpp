#pragma once

#include "colors.h"
#include "./parser/Token.hpp"
#include "msg.hpp"
#include "./parser/Parser.hpp"
#include "utils.hpp"
#include "types.hpp"
#include "ClientConnection.hpp"
#include "ClientConnections.hpp"
#include "NewClientListener.hpp"

#include <thread>
#include <unistd.h>
#include <cstring>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <poll.h>

#ifndef REQUEST_QUE_SIZE
# define REQUEST_QUE_SIZE 10
#endif

class Webserv {
public:
					Webserv(struct server_config);
					~Webserv(void);
	[[noreturn]]
	void			run(void);
private:
	void			_execute_request(t_http_request request, ClientConnection & connection);

private:
	NewClientListener		_listener;
	std::atomic<bool>		_exit;

	struct sockaddr_in		_server_addr;
	const socklen_t			_server_addr_len;
	/* to avoid pointer casts every where */
	struct sockaddr	*const _server_addr_ptr;

	ClientConnections		_connections;

	std::string				_build_response(t_http_request request, bool & close_connection);
};
