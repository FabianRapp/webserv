#pragma once

#include <colors.h>
#include <Token.hpp>
#include <msg.hpp>
#include <Parser.hpp>
#include <utils.hpp>
#include <types.hpp>
#include <ClientConnection.hpp>
#include <ClientConnections.hpp>
#include <NewClientListener.hpp>
#include <Exceptions.hpp>

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

class Server {
public:
	t_fd&	socket;
	Server(struct server_config);
	Server(const Server& old) = delete;
	Server&	operator=(const Server& old) = delete;
	~Server(void);


private:
	ClientConnections		_connections;
};


class Socket {
public:
	Socket(struct pollfd& pollfd, unsigned short port);
	Socket(const Socket& old) = delete;
	Socket(const Socket&& old);
	~Socket(void);
	Socket&	operator=(const Socket& old) = delete;

	struct pollfd&	poll_fd;
	t_fd			fd;
	unsigned short	port;
};


class Webserv {
public:
					Webserv(struct server_config);
					~Webserv(void);
	[[noreturn]]
	void			run(void);
private:
	void			_execute_request(t_http_request request, ClientConnection & connection);

	void			_default_err_response(ClientConnection& connection,
						const SendClientError& err);

private:
	NewClientListener		_listener;

	struct sockaddr_in		_server_addr;
	const socklen_t			_server_addr_len;
	/* to avoid pointer casts every where */
	struct sockaddr	*const _server_addr_ptr;

	ClientConnections		_connections;

	std::string				_build_response(t_http_request request, bool & close_connection);
	std::unordered_map<unsigned long, std::string>	_codes;

};
