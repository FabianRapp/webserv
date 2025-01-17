#include <Webserv.hpp>

static struct sockaddr_in	make_addr(unsigned short port) {
	struct sockaddr_in	addr;

	memset(&addr, 0, sizeof(struct sockaddr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	return (addr);
}

Webserv::Webserv(struct server_config server_config):
	_listener(_connections),
	_server_addr_len(static_cast<socklen_t>(sizeof _server_addr)),
	_server_addr_ptr(reinterpret_cast<struct sockaddr *>(&_server_addr))
{
	/* AF_INET : ipv4
	 * AF_INET6: ipv6 */
	init_status_codes(_codes);
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0) {
		std::cerr << "Error: server: socket: " << strerror(errno) << '\n';
		exit(errno);
	}
	server_fd = set_fd_non_block(server_fd);
	if (server_fd < 0) {
		exit(errno);
	}
	_server_addr = make_addr(server_config.port);
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
	std::cout << "Started server on port " << server_config.port << "...\n";
}

Webserv::~Webserv(void) {
	_listener.join_thread();
}

[[noreturn]]
void	Webserv::run(void) {
	_listener.run();
	while (1) {
		if (_connections.get_count() == 0) {
			/* todo: implement some wake up condition that the listener thread
			 * can set to avoid this dead loop */
			continue ;
		}
		_connections.set_and_poll(POLLPRI | POLLIN | POLLOUT);
		{
			ClientConnections::PollIterator	it = _connections.begin(POLLPRI | POLLIN);
			ClientConnections::PollIterator	end = _connections.end(POLLPRI | POLLIN);
			for (; it < end; ++it) {
				ClientConnection&	connection = *it;
				if (connection.current_mode != ConnectionMode::RESEIVING) {
					continue ;
				}
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

				/* todo: check for earlyer chunks of the msg etc.. */
				try {
					/* todo: this throw is just for testing */
					throw (SendClientError(404, _codes[404], "testing", true));

					connection.parse();
					bool testing_response = true;
					if (testing_response || connection.completed_request()) {
						connection.current_mode = ConnectionMode::RESEIVING;
						t_http_request	request = connection.get_request();
						bool	placeholder_close_connection;
						std::string	response = _build_response(request, placeholder_close_connection);
						connection.set_response(std::move(response), placeholder_close_connection);
					}
				} catch (const SendClientError& err) {
					_default_err_response(connection, err);
				}
			}
		}
		{
			ClientConnections::PollIterator	it = _connections.begin(POLLOUT);
			ClientConnections::PollIterator	end = _connections.end(POLLOUT);
			for (; it < end; ++it) {
				ClientConnection&	connection = *it;
				if (connection.current_mode != ConnectionMode::SENDING) {
					continue ;
				}
				//try {
					connection.send_response();
					if (connection.finished_sending()) {
						if (connection.should_close_after_send()) {
							connection.close_after_loop = true;
							// todo: connections habe to be closed at the end of outer loop to not break iterators
							_connections.close_client_connection(&connection);
						} else {
							connection.current_mode = ConnectionMode::RESEIVING;
						}
					}
				//} catch (const & err) {
					// todo: send/malloc fail handling
				//}
			}
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

void	Webserv::_default_err_response(ClientConnection& connection,
		const SendClientError& err)
{
	const std::string code_str = std::to_string(err.err_code);
	std::string	response = "HTTP/1.1 " + code_str + " " + err.title + "\r\n";
	std::string	body =
		"<!DOCTYPE html>"
		"<html>"
		"<head><title>" + code_str + " " + err.title + "</title></head>"
		"<body><h1>" + err.title + "</h1><p>" + err.msg + "</p></body>"
		"</html>";
	;
	const std::string	content_len = "Content-Length: "
		+ std::to_string(body.size())
		+ "\r\n";
	response += "Content-Type: text/html\r\n";
	response += content_len;
	response += "\r\n";
	response += body;
	//response += "0\r\n\r\n";//todo: this is only for chunked mode right?

	connection.set_response(std::move(response), err.close_connection);
}

