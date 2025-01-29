#include "../../includes/FdClasses/Client.hpp"
#include "../../includes/FdClasses/Server.hpp"
#include "../../includes/Manager.hpp"
#include "../../includes/Response.hpp"
#include "../../includes/macros.h"

//todo: constructor err handling
Client::Client(DataManager& data, Server* parent_server):
	BaseFd(data, POLLIN | POLLOUT, "Client"),
	mode(ClientMode::RECEIVING),
	_response_builder({""}),
	_send_data({"", 0, false}),
	_parser(input),
	_writer(nullptr),
	_response(nullptr)
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

	// todo: this if statement is only for debugging and should not stay for
	// submission
	if (errno == EAGAIN || errno == EWOULDBLOCK) {
		FT_ASSERT(0 && "Should have been handled by poll");
	}

	_set_non_blocking();
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
		set_close();
		return ;
	}
	buffer[bytes_read] = 0;
	std::cout << "Read:\n" << buffer << '\n';
	this->input += buffer;

	/* todo: check for earlyer chunks of the msg etc.. */
	try {
		/* todo: this throw is just for testing */
		//throw (SendClientError(404, _codes[404], "testing", true));

		this->parse();
		std::cout << "Request STATUS = " << _parser.is_finished() << std::endl;
		if (_parser.is_finished() == true) {
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

// Use this to read from from a pipe or a file.
// Clears _fd_read_data and then writes the data to it.
// Switched int ClientMode::WRITING_FD, thus:
// After calling this function the caller should return straight to Client::execute
// without any more actions besides the following:
// Uses dup() on the given fd, if the fd is not needed anywher else simply close
// the fd after calling this.
// Assumes the given fd to be valid.
void	Client::_read_fd(ClientMode next_mode, int read_fd, ssize_t byte_count) {
	_fd_error.error = false;
	_fd_read_data = "";
	FT_ASSERT(read_fd > 0);
	read_fd = dup(read_fd);
	FT_ASSERT(read_fd > 0);
	mode = ClientMode::READING_FD;
	_reader = data.new_read_fd(
		_fd_read_data,
		read_fd,
		byte_count,
		[this, next_mode] () {
			this->mode = next_mode;
			this->_reader = nullptr;
		}
	);
}

// Use this to write to a pipe or a file.
// The input data has to be in Client::_fd_write_data
// Switched int ClientMode::WRITING_FD, thus:
// After calling this function the caller should return straight to Client::execute
// without any more actions besides the following:
// Uses dup() on the given fd, if the fd is not needed anywher else simply close
// the fd after calling this.
// Assumes the given fd to be valid.
void	Client::_write_fd(ClientMode next_mode, int write_fd) {
	_fd_error.error = false;
	FT_ASSERT(write_fd > 0);
	write_fd = dup(write_fd);
	mode = ClientMode::WRITING_FD;
	_writer = data.new_write_fd(
		write_fd,
		_fd_write_data,
		[this, next_mode] () {
			this->mode = next_mode;
			this->_writer = nullptr;
			//todo: maybe clear <this->_fd_write_data> here
		}
	);
}

ServerConfigFile&	select_config(std::vector<ServerConfigFile>& server_configs,
						Request& request)
{
	//std::string	host = request._headers[HeaderType::HOST];
	//i think authorization is the one to look at
	std::string	requested_name = request._headers[HeaderType::AUTHORIZATION];

	std::transform(requested_name.begin(), requested_name.end(), requested_name.begin(),
		[](unsigned char c) { return (std::tolower(c));});

	for (ServerConfigFile& config : server_configs) {
		const std::vector<std::string>&	names = config.getServerNames();
		for (const auto& name : names) {
			if (name == requested_name) {
				return (config);
			}
		}
	}
	return (server_configs[0]);
}

/* todo: should not return value */
void	Client::_execute_response(bool & close_connection) {
	ServerConfigFile&	config = select_config(server->configs, _request);

	if (_response == nullptr)
		_response = new Response(config, _request, *this);

	if (_response->getBody().length() == 0)
	{
		_response->appendToBody("HTTP/1.1 200 OK\r\n");
	}

	if (!_response->getReadContent().length() && !_fd_read_data.size()) {
		std::string	path = "hello_world.html";
		struct stat	stats;
		stat(path.c_str(), &stats);
		int	file_fd = open(path.c_str(), O_RDONLY);
		FT_ASSERT(file_fd > 0);
		_read_fd(ClientMode::BUILD_RESPONSE, file_fd, stats.st_size);
		close(file_fd);
		return ;
	} else if (!_response->getReadContent().length()) {
		_response->appendToRead( _fd_read_data);
	}

	close_connection = true; /* default for now == true */
	switch (_request._type) {
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
				<< to_string(_request._type) << "\n";
			_response->appendToBody("405 Method Not Allowed\r\n");
			//response += "\r\n\r\n";
			close_connection = true;
			/*
			 *todo:
			 set_err(405);
			 return;
			*/
		}
	}
	_response->appendToBody("Content-Length: ");
	_response->appendToBody(std::to_string(_response_builder.body.length()));
	_response->appendToBody("\r\n");
	_response->appendToBody("Content-Type: text/html; charset=UTF-8");
	_response->appendToBody("\r\n");
	_response->appendToBody("\r\n");
	_response->appendToBody(_response->getReadContent());
	this->mode = ClientMode::SENDING;
	std::cout << "HERE COMES BODY:\n" << _response->getBody() << "HERE ENDS BODY:\n";
	return;
}

// diff test_file.txt test_file_cmp.txt
// -> should not show any diff
// can be used as an intermediate mode for this->mode before entering SENDING mode
void	Client::_test_write_fd() {
	int fd = open("test_file.txt", O_WRONLY | O_APPEND | O_NONBLOCK | O_CREAT | O_TRUNC, 0666);
	int fd2 = open("test_file_cmp.txt", O_WRONLY | O_APPEND | O_CREAT | O_TRUNC, 0666);
	FT_ASSERT(fd > 0);
	_fd_write_data = std::string_view(_send_data.response.c_str(), _send_data.response.size());
	write(fd2, _fd_write_data.data(), _fd_write_data.size());
	close(fd2);
	_write_fd(ClientMode::SENDING, fd);
	//mode = ClientMode::SENDING;
	close(fd);
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
			_execute_response(placeholder_close_connection);
			break ;
		}
		case (ClientMode::SENDING): {
			_send_response();
			return ;
		}
		case (ClientMode::READING_FD):
		case (ClientMode::WRITING_FD): {
			// do nothing
			break ;
		}
		case (ClientMode::TESTING_MODE): {
			//_test_write_fd();
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
	//std::cout << "sending:\n" << _send_data.response << "\n";
	ssize_t send_bytes = send(
		fd,
		_send_data.response.c_str() + _send_data.pos,
		_send_data.response.size() - _send_data.pos,
		send_flags
	);
	if (send_bytes <= 0) {
		std::cerr << "Error: send: closing connection now\n";
		//todo: the line below has to removed before submission according to subject
		std::cerr << "err: " << strerror(errno) << '\n';
		set_close();
		return ;
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

// void	Client::_send_response(void) {
// 	if (!this->is_ready(POLLOUT)) {
// 		return ;
// 	}

// 	{
// 		/* todo: poll to catch potential issues: remove later */
// 		struct pollfd	test_poll = {fd, POLLOUT, 0};
// 		poll(&test_poll, 1, 0);
// 		FT_ASSERT(test_poll.revents & POLLOUT);
// 	}

// 	const int send_flags = 0;
// 	//std::cout << "sending:\n" << _send_data.response << "\n";
// 	ssize_t send_bytes = send(
// 		fd,
// 		_response->getBody().c_str() + _send_data.pos,
// 		_response->getBody().size() - _send_data.pos,
// 		send_flags
// 	);
// 	if (send_bytes <= 0) {
// 		std::cerr << "Error: send: closing connection now\n";
// 		//todo: the line below has to removed before submission according to subject
// 		std::cerr << "err: " << strerror(errno) << '\n';
// 		set_close();
// 		return ;
// 	}
// 	_send_data.pos += static_cast<size_t>(send_bytes);
// 	if (_send_data.pos == _response->getBody().size()) {
// 		mode = ClientMode::RECEIVING;
// 		_send_data.response = "";
// 		_send_data.pos = 0;
// 		if (_send_data.close_after_send) {
// 			set_close();
// 			_send_data.close_after_send = false;
// 		}
// 	}
// }
