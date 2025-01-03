#include <Webserv.hpp>

static struct sockaddr_in	make_addr(unsigned short port) {
	struct sockaddr_in	addr;

	memset(&addr, 0, sizeof(struct sockaddr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	return (addr);
}

Webserv::Webserv(void):
	_server_addr_len(static_cast<socklen_t>(sizeof _server_addr)),
	_client_addr_len(static_cast<socklen_t>(sizeof _client_addr)),
	_server_addr_ptr(reinterpret_cast<struct sockaddr *>(&_server_addr)),
	_client_addr_ptr(reinterpret_cast<struct sockaddr *>(&_client_addr)),
	_active_client_count(0)
{
	memset(_client_connections, 0, sizeof _client_connections);
	for (auto & poll_fd : _client_fds) {
		poll_fd.fd = -1;
		poll_fd.events = 0;
		poll_fd.revents = 0;
	}
	/* AF_INET : ipv4
	 * AF_INET6: ipv6 */
	_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_server_fd < 0) {
		std::cerr << "Error: server: socket: " << strerror(errno) << '\n';
		exit(errno);
	}
	_server_fd = set_fd_non_block(_server_fd);
	if (_server_fd < 0) {
		exit(errno);
	}
	_server_addr = make_addr(8080);
	if (bind(_server_fd, _server_addr_ptr, _server_addr_len) < 0) {
		close(_server_fd);
		std::cerr << "Error: " << strerror(errno) << '\n';
		exit(errno);
	}
	if (listen(_server_fd, REQUEST_QUE_SIZE) < 0) {
		close(_server_fd);
		std::cerr << "Error: " << strerror(errno) << '\n';
		exit(errno);
	}
	std::cout << "Started server on port 8080...\n";
}

/* todo: parse config file */
Webserv::Webserv(const char * config_file_path):
	_server_addr_len(static_cast<socklen_t>(sizeof _server_addr)),
	_client_addr_len(static_cast<socklen_t>(sizeof _client_addr)),
	_server_addr_ptr(reinterpret_cast<struct sockaddr *>(&_server_addr)),
	_client_addr_ptr(reinterpret_cast<struct sockaddr *>(&_client_addr)),
	_active_client_count(0)
{
	FT_ASSERT(0 && "not implemented");
	(void)config_file_path;
}

Webserv::~Webserv(void) {
	for (auto & poll_fd : _client_fds) {
		if (poll_fd.fd > 0) {
			close(poll_fd.fd);
		}
	}
}

void	Webserv::_accept_clients(void) {
	FT_ASSERT(_active_client_count <= MAX_CLIENTS);
	if (_active_client_count == MAX_CLIENTS) {
		return ;
	}
	int	old_err = errno;
	errno = 0;

	int	new_client_fd;
	while (_active_client_count < MAX_CLIENTS)
	{
		struct pollfd	poll_fd = {
			.fd = _server_fd,
			.events = POLLIN,
			.revents = 0,
		};
		if (poll(&poll_fd, 1, 0) < 0) {
			FT_ASSERT(0 && "poll failed");
		}
		if (!(poll_fd.revents & POLLIN)) {
			return ;
		}
		new_client_fd = accept(_server_fd, _client_addr_ptr, &_client_addr_len);
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			FT_ASSERT(0 && "Should have been handled by poll");
		}
		new_client_fd = set_fd_non_block(new_client_fd);
		if (new_client_fd < 0) {
			break ;
		}
		ClientConnection * new_connection = _add_client(new_client_fd);
		(void)new_connection; /* is this ptr needed here? */

		std::cout << "Connection accepted from address("
			<< inet_ntoa(_client_addr.sin_addr) << "): PORT("
			<< ntohs(_client_addr.sin_port) << ")\n";
	}
	errno = old_err;
}

[[noreturn]]
void	Webserv::run(void) {
	while (1) {
		_accept_clients();

		if (_active_client_count == 0) {
			continue ;
		}

		_set_client_poll_events(POLLPRI | POLLIN);
		for (size_t client_idx = 0; client_idx < MAX_CLIENTS; client_idx++) {
			auto & client = _client_fds[client_idx];
			if (client.fd == -1) {
				continue ;
			}
			if (!(client.revents & (POLLIN | POLLPRI))) {
				/* fd is not ready */
				continue ;
			}

			ClientConnection	*connection = _client_connections[client_idx];
			char		buffer[4096];
			int			recv_flags = 0;//MSG_ERRQUEUE <- has smth to do with error checks
			long int	bytes_read = recv(client.fd, buffer, sizeof buffer - 1, recv_flags);
			if (bytes_read < 0) {
				std::cerr << "Error: read failed\n";
				FT_ASSERT(0);
			}
			buffer[bytes_read] = 0;
			std::cout << "Read:\n" << buffer << '\n';
			connection->input += buffer;
			//for (size_t i = 0; i < strlen(buffer); i++) {
			//	printf("%x\n", buffer[i]);
			//}

			/* todo: check for earlyer chunks of the msg etc.. */
			connection->parse();
			if (connection->completed_request()) {
				t_http_request	request = connection->get_request();
				_execute_request(request, client_idx);
			} /* else if (something that has to be done without the full
					request, example: the client expectes:CONTINUE)
			{
					...
			} */
			else {
				std::cout << FT_ANSI_YELLOW
					"Warning: not completed requst(bug or long request?)\n"
					FT_ANSI_RESET;
			}

			//write(client_fd, http_response, strlen(http_response));
		}
	}
}

/* assumes space for the given client */
ClientConnection *	Webserv::_add_client(t_fd fd) {
	FT_ASSERT(_active_client_count < MAX_CLIENTS);
	FT_ASSERT(fd > 0);
	_active_client_count++;

	for (size_t i = 0; i < MAX_CLIENTS; i++) {
		if (_client_connections[i] == nullptr) {
			_client_fds[i].fd = fd;
			_client_connections[i] = new ClientConnection(_client_fds[i].fd);
			return (_client_connections[i]);
		}
	}
	FT_ASSERT(0);
}

void	Webserv::_set_client_poll_events(short int events) {
	for (auto & client : _client_fds) {
		client.events = events;
	}
	if (poll(_client_fds, _active_client_count, 0) < 0) {
		std::cerr << "Error: poll: " << strerror(errno) << '(' << errno << ")\n";
		FT_ASSERT(0);
	}
}

/* todo: */
void	Webserv::_execute_request(t_http_request request, size_t client_idx) {
	switch (request.type) {
		case (MethodType::GET): {
			break ;
		}
		case (MethodType::POST): {
			break ;
		}
		case (MethodType::DELETE): {
			break ;
		}
		default: {
			std::cerr << "Error: Unsupported request type: "
				<< to_string(request.type) << "\n";
			/* todo: send error response */
			_close_client_connection(client_idx); // placeholder
			return ;
		}
	}
	/* todo: first handle error not checked in parser/lexer then execute */
}

/* TODO: Update for new ClientConnection */
void	Webserv::_close_client_connection(size_t client_idx) {
	auto &	poll_fd = _client_fds[client_idx];
	FT_ASSERT(poll_fd.fd > 0);

	delete _client_connections[client_idx];
	_client_connections[client_idx] = nullptr;

	/* fd allready closed in destructor of ClientConnection */
	poll_fd.events = 0;
	poll_fd.revents = 0;
	poll_fd.fd = -1;
	_active_client_count--;
}
