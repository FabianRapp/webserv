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
		_add_client(new_client_fd);

		std::cout << "Connection accepted from address("
			<< inet_ntoa(_client_addr.sin_addr) << "): PORT("
			<< ntohs(_client_addr.sin_port) << ")\n";
	}
	errno = old_err;
}

/* main loop */
[[noreturn]]
void	Webserv::run(void) {
	while (1) {
		_accept_clients();

		if (_active_client_count == 0) {
			continue ;
		}

		_set_client_poll_events(POLLIN);

		for (auto & client : _client_fds) {
			if (client.fd == -1) {
				continue ;
			}
			if (!(client.revents & POLLIN)) {
				/* fd is not ready */
				continue ;
			}
			char	buffer[4096];
			long int bytes_read = read(client.fd, buffer, sizeof buffer - 1);
			if (bytes_read < 0) {
				std::cerr << "Error: read failed\n";
				FT_ASSERT(0);
			} else if (bytes_read == sizeof buffer -1) {
			/* todo: Handle longer than buffer messages.
			 * This should pause this fd execution and continue with non
			 * blocking one. On malloc error (size the massage will need to be
			 * allocaed) the connection should be droped with the right error
			 * response.
			 * For now not important, just keep in mind.
			 */
			}


			buffer[bytes_read] = 0;
			printf("Received Request:\n%s\n", buffer);
			char	*full_msg;

			/* todo: check for earlyer chunks of the msg etc.. */
			full_msg = buffer;
			t_http_request	request = _parse(full_msg);
			_execute_request(request, client);


			//write(client_fd, http_response, strlen(http_response));
		}
	}
}

/* assumes space for the given client */
void	Webserv::_add_client(t_fd fd) {
	FT_ASSERT(_active_client_count < MAX_CLIENTS);
	FT_ASSERT(fd > 0);
	_active_client_count++;
	for (auto & poll_fd : _client_fds) {
		if (poll_fd.fd == -1) {
			poll_fd.fd = fd;
			return ;
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

/* wraper for Parser, I think leaving it like this should be the simplest */
t_http_request	Webserv::_parse(std::string raw_input) {
	Parser	parser(raw_input);
	return (parser.parse());
}

/* todo: */
void	Webserv::_execute_request(t_http_request request, struct pollfd & client) {
	_close_client_connection(client); // placeholder
	(void)request;
}

void	Webserv::_close_client_connection(struct pollfd & client) {
	close(client.fd);
	client.events = 0;
	client.revents = 0;
	client.fd = -1;
	_active_client_count--;
}
