#include <Client.hpp>
#include <Server.hpp>
#include <Manager.hpp>

Client::Client(DataManager& data, Server* parent_server):
	BaseFd(data, POLLIN | POLLOUT),
	mode(ClientMode::RECEIVING),
	_response_builder({"", nullptr}),
	_send_data({"", 0, false}),
	_parser(input)
{
	this->server = parent_server;
	assert(server->is_ready(POLLIN));
	errno = 0;
	struct sockaddr_in		addr;
	socklen_t				addr_len = static_cast<socklen_t>(sizeof addr);
	struct sockaddr			*const addr_ptr =
		reinterpret_cast<struct sockaddr *>(&addr);
	memset(&addr, 0, sizeof addr);// might not be needed
	fd = accept(server->fd, addr_ptr, &addr_len);
	if (errno == EAGAIN || errno == EWOULDBLOCK) {
		FT_ASSERT(0 && "Should have been handled by poll");
	}
	_set_non_blocking();
	//if (new_client_fd < 0) {
	//	FT_ASSERT(0);
	//}
	//_connections.add_client(new_client_fd);
	std::cout << "Connection accepted from address("
		<< inet_ntoa(addr.sin_addr) << "): PORT("
		<< ntohs(addr.sin_port) << ")\n";
}

Client::~Client(void) {
}

void	Client::_receive_request(void) {
	if (!is_ready(POLLIN)) {
		return ;
	}
	std::cout << "clinet exec\n";
	char		buffer[4096];
	int			recv_flags = 0;//MSG_ERRQUEUE <- has smth to do with error checks
	long int	bytes_read = recv(this->fd, buffer, sizeof buffer - 1, recv_flags);
	if (bytes_read < 0) {
		std::cerr << "Error: read failed\n";
		FT_ASSERT(0);
	}
	buffer[bytes_read] = 0;
	std::cout << "Read:\n" << buffer << '\n';
	this->input += buffer;

	/* todo: check for earlyer chunks of the msg etc.. */
	try {
		/* todo: this throw is just for testing */
		//throw (SendClientError(404, _codes[404], "testing", true));

		this->parse();
		bool testing_response = true;
		if (testing_response || _parser.is_finished()) {
			_request = _parser.get_request();
			mode = ClientMode::BUILD_RESPONSE;
			_send_data.pos = 0;
			_send_data.response = "";
			_send_data.close_after_send = true; /* placeholder */
		}
	} catch (const SendClientError& err) {
		//_default_err_response(connection, err);
	}
}

/* todo: should not return value */
std::string	Client::_build_response(t_http_request request, bool & close_connection) {
	std::string	&response = _send_data.response;

	if (response.size() == 0) {
		// first call here
		response = "HTTP/1.1 200 OK";
		response += "\r\n";
		_response_builder.body = "";
	}

	if (!_response_builder.body.length()) {
		/* testing to load a file */
		std::string	path = "hello_world.html";
		struct stat	stats;
		stat(path.c_str(), &stats);
		int	file_fd = open(path.c_str(), O_RDONLY);
		FT_ASSERT(file_fd > 0);

		mode = ClientMode::READING_FILE;
		_response_builder.reader = data.new_read_fd(
			_response_builder.body,
			file_fd, stats.st_size,
			[this] () {
				this->mode = ClientMode::BUILD_RESPONSE;

			}
		);
		return (response);
	}
	if (_response_builder.reader != nullptr) {
		_response_builder.reader->set_close();
		_response_builder.reader = nullptr;
	}

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
	response += "Content-Length: ";
	response += std::to_string(_response_builder.body.length());
	response += "\r\n";
	response += "Content-Type: text/html; charset=UTF-8";
	response += "\r\n";
	response += "\r\n";
	response += _response_builder.body;
	this->mode = ClientMode::SENDING;
	return (response);
}

void	Client::execute(void) {
	switch (this->mode) {
		case (ClientMode::RECEIVING): {
			_receive_request();
			if (this->mode != ClientMode::SENDING) {
				break ;
			}
		}
		case (ClientMode::BUILD_RESPONSE): {
			bool	placeholder_close_connection;
			_send_data.response = _build_response(_request, placeholder_close_connection);
			break ;
		}
		case (ClientMode::SENDING): {
			_send_response();
			return ;
		}
		case (ClientMode::READING_FILE): {
			break ;
		}
		case (ClientMode::WRITING_FILE): {
			break ;
		}
		case (ClientMode::READING_PIPE): {
			break ;
		}
		case (ClientMode::WRITING_PIPE): {
			break ;
		}
	}

}

void	Client::parse() {
	_parser.parse();
}

void	Client::_send_response(void) {
	if (!this->is_ready(POLLOUT)) {
		return ;
	}

	{
		/* todo: poll to catch potential issues: remove later */
		struct pollfd	test_poll = {fd, POLLOUT, 0};
		poll(&test_poll, 1, 0);
		FT_ASSERT(test_poll.revents & POLLOUT);
	}

	const int send_flags = 0;
	std::cout << "sending:\n" << _send_data.response << "\n";
	ssize_t send_bytes = send(
		fd,
		_send_data.response.c_str() + _send_data.pos,
		_send_data.response.size() - _send_data.pos,
		send_flags
	);
	if (send_bytes <= 0) {
		//todo:
		std::cerr << "send_bytes: " << send_bytes << '\n';
		std::cerr << "err: " << strerror(errno) << '\n';
		assert(0);
	}
	_send_data.pos += static_cast<size_t>(send_bytes);
	if (_send_data.pos == _send_data.response.size()) {
		mode = ClientMode::RECEIVING;
		_send_data.response = "";
		_send_data.pos = 0;
		if (_send_data.close_after_send) {
			set_close();
			_send_data.close_after_send = false;
		}
	}
}
