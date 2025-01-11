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
	_listener(_exit, _connections),
	_exit(false),
	_server_addr_len(static_cast<socklen_t>(sizeof _server_addr)),
	_client_addr_len(static_cast<socklen_t>(sizeof _client_addr)),
	_server_addr_ptr(reinterpret_cast<struct sockaddr *>(&_server_addr)),
	_client_addr_ptr(reinterpret_cast<struct sockaddr *>(&_client_addr))
{
	/* AF_INET : ipv4
	 * AF_INET6: ipv6 */
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0) {
		std::cerr << "Error: server: socket: " << strerror(errno) << '\n';
		exit(errno);
	}
	server_fd = set_fd_non_block(server_fd);
	if (server_fd < 0) {
		exit(errno);
	}
	_server_addr = make_addr(8080);
	if (bind(server_fd, _server_addr_ptr, _server_addr_len) < 0) {
		close(server_fd);
		std::cerr << "Error: " << strerror(errno) << '\n';
		exit(errno);
	}
	if (listen(server_fd, REQUEST_QUE_SIZE) < 0) {
		close(server_fd);
		std::cerr << "Error: " << strerror(errno) << '\n';
		exit(errno);
	}
	_listener.set_server_fd(server_fd);
	std::cout << "Started server on port 8080...\n";
}

/* todo: parse config file */
Webserv::Webserv(const char * config_file_path):
	_listener(_exit, _connections),
	_exit(false),
	_server_addr_len(static_cast<socklen_t>(sizeof _server_addr)),
	_client_addr_len(static_cast<socklen_t>(sizeof _client_addr)),
	_server_addr_ptr(reinterpret_cast<struct sockaddr *>(&_server_addr)),
	_client_addr_ptr(reinterpret_cast<struct sockaddr *>(&_client_addr))
{
	FT_ASSERT(0 && "not implemented");
	(void)config_file_path;
}

Webserv::~Webserv(void) {
	_listener.join_thread();
}

//void	Webserv::_listener(void) {
//	FT_ASSERT(_connections.get_count() <= MAX_CLIENTS);
//	if (_connections.get_count() == MAX_CLIENTS) {
//		/*todo: send basic error response */
//		return ;
//	}
//	int	old_err = errno;
//	errno = 0;
//
//	int	new_client_fd;
//	while (_connections.get_count() < MAX_CLIENTS)
//	{
//		struct pollfd	poll_fd = {
//			.fd = _server_fd,
//			.events = POLLIN,
//			.revents = 0,
//		};
//		if (poll(&poll_fd, 1, 0) < 0) {
//			FT_ASSERT(0 && "poll failed");
//		}
//		if (!(poll_fd.revents & POLLIN)) {
//			return ;
//		}
//		new_client_fd = accept(_server_fd, _client_addr_ptr, &_client_addr_len);
//		if (errno == EAGAIN || errno == EWOULDBLOCK) {
//			FT_ASSERT(0 && "Should have been handled by poll");
//		}
//		new_client_fd = set_fd_non_block(new_client_fd);
//		if (new_client_fd < 0) {
//			break ;
//		}
//		_connections.add_client(new_client_fd);
//
//		std::cout << "Connection accepted from address("
//			<< inet_ntoa(_client_addr.sin_addr) << "): PORT("
//			<< ntohs(_client_addr.sin_port) << ")\n";
//	}
//	errno = old_err;
//}

[[noreturn]]
void	Webserv::run(void) {
	_listener.run();
	while (1) {
		//_accept_clients();

		if (_connections.get_count() == 0) {
			continue ;
		}
		_connections.set_and_poll(POLLPRI | POLLIN);
		ClientConnections::PollIterator	it = _connections.begin(POLLPRI | POLLIN);
		ClientConnections::PollIterator	end = _connections.end(POLLPRI | POLLIN);
		while (it < end) {
			ClientConnection&	connection = *it;
			char		buffer[4096];
			int			recv_flags = 0;//MSG_ERRQUEUE <- has smth to do with error checks
			long int	bytes_read = recv(connection.fd, buffer, sizeof buffer - 1, recv_flags);
			if (bytes_read < 0) {
				std::cerr << "Error: read failed\n";
				FT_ASSERT(0);
			}
			buffer[bytes_read] = 0;
			std::cout << "Read:\n" << buffer << '\n';
			connection.input += buffer;
			//for (size_t i = 0; i < strlen(buffer); i++) {
			//	printf("%x\n", buffer[i]);
			//}

			/* todo: check for earlyer chunks of the msg etc.. */
			connection.parse();
			bool testing_response = true;
			if (testing_response || connection.completed_request()) {
				t_http_request	request = connection.get_request();
				_execute_request(request, connection);
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
			++it;
		}
	}
}

std::string	Webserv::_build_response(t_http_request request, bool & close_connection) {
	std::string	response = "HTTP/1.1 ";

	close_connection = true; /* default for now == true */
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
			response += "405 Method Not Allowed\r\n";
			//response += "\r\n\r\n";
			close_connection = true;
		}
	}
	return (response);
}

/* todo: */
void	Webserv::_execute_request(t_http_request request, ClientConnection & connection) {
	std::cout << "Executiing request..\n";
	/* todo: idk when the client does not expect a response yet */

	bool				close_connection;
	std::string			response = _build_response(request, close_connection);

	if (send(connection.fd, response.c_str(), response.length(), 0) < 0) {
		std::cerr << "Error: send error: " << strerror(errno) << '\n';
		exit(1);
	}
	if (close_connection) {
		_connections.close_client_connection(&connection); // placeholder
	}
}
