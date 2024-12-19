#pragma once

#include <Token.hpp>
#include <msg.hpp>
#include <Parser.hpp>
#include <utils.hpp>
#include <types.hpp>
//#include <ClientConnection.hpp>

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

#ifndef MAX_CLIENTS
# define MAX_CLIENTS 3
#endif

/* to handle small client requests statically and large ones dynamically
 * (ensures operation for most reqeusts on low memory without droping all
 * requests) */
/* todo: implementation */
class RawClientMsg {
public:
					RawClientMsg(void);
					~RawClientMsg(void);
					RawClientMsg(const RawClientMsg & old);
	RawClientMsg &	operator=(const RawClientMsg & right);
	char			*buf; /* points to the correct buffer */
	size_t			full_size;
	bool			finished;
private:
	char	static_buf[1024];/* To operate in OOM situations */
	char	*dynamic_buf; /* For long messages, tho might fail if OOM */
};

class Webserv {
public:
					Webserv(void);
					Webserv(const char * config_file_path);
					~Webserv(void);
	[[noreturn]]
	void			run(void);
private:
	t_http_request	_parse(std::string raw_input);
	void			_execute_request(t_http_request request,
						 struct pollfd & client);

private:
	int						_server_fd;

	struct sockaddr_in		_server_addr;
	const socklen_t			_server_addr_len;
	struct sockaddr_in		_client_addr;
	socklen_t				_client_addr_len;
	/* to avoid pointer casts every where */
	struct sockaddr	*const _server_addr_ptr;
	struct sockaddr	*const _client_addr_ptr;


	/* client connection managment */
	struct pollfd			_client_fds[MAX_CLIENTS];
	nfds_t					_active_client_count;
	void					_accept_clients(void);
	void					_add_client(t_fd fd);
	void					_set_client_poll_events(short int event);

	void					_close_client_connection(struct pollfd & client);
};
