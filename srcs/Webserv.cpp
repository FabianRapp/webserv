#include "../includes/Webserv.hpp"

static struct sockaddr_in	make_addr(unsigned short port) {
	struct sockaddr_in	addr;

	memset(&addr, 0, sizeof(struct sockaddr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	return (addr);
}

Webserv::Webserv(struct server_config):
	_listener(_exit, _connections),
	_exit(false),
	_server_addr_len(static_cast<socklen_t>(sizeof _server_addr)),
	_server_addr_ptr(reinterpret_cast<struct sockaddr *>(&_server_addr))
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

Webserv::~Webserv(void) {
	_listener.join_thread();
}
// enum class PostParseAction {
// 	WAIT,
// 	SEND_CONTINUE,
// 	EXECUTE_RESPONSE,
// 	ERROR,
// };

[[noreturn]]
void	Webserv::run(void) {
	_listener.run();
	while (1) {
		//_accept_clients();
		// std::cout << "looped\n" << std::endl;
		if (_connections.get_count() == 0) {
			continue ;
		}
		_connections.set_and_poll(POLLPRI | POLLIN | POLLOUT);
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
			if (Parser::is_header_present(connection.input) == true)
			{
				connection.parse();
				bool testing_response = true;
				if (testing_response || connection.completed_request()) {
					Request	request = connection.get_request();
					// _execute_request(request, connection);
				}
			} /* else if (something that has to be done without the full
					request, example: the client expectes:CONTINUE)
			{
					...
			} */
			else {
				std::cout << FT_ANSI_YELLOW
					"Warning: headers ending sequence not detected (bug or uncomplete request?)\n"
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
