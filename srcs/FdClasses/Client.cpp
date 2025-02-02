#include "../../includes/FdClasses/Client.hpp"
#include "../../includes/FdClasses/Server.hpp"
#include "../../includes/Manager.hpp"
#include "../../includes/Response.hpp"
#include "../../includes/macros.h"

//todo: constructor err handling
Client::Client(DataManager& data, Server* parent_server):
	BaseFd(data, POLLIN | POLLOUT, "Client"),
	mode(ClientMode::RECEIVING),
	_send_data({"", 0}),
	_parser(input),
	_last_availability(std::chrono::high_resolution_clock::now()),
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
	//todo:
	// this delets the response which then leads to sefaults with Read/WriteFd that are still running
	delete _response;
}

void	Client::_receive_request(void) {
	if (!is_ready(POLLIN)) {
		std::cout << "not ready\n";
		auto now = std::chrono::high_resolution_clock::now();
		auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - _last_availability).count();
		if (elapsed_ms > 2000) {
			std::cerr << "client " << data_idx << " timed out..\n";
			set_close();
		}
		return ;
	}
	_last_availability = std::chrono::high_resolution_clock::now();
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
		//std::cout << "Request STATUS = " << _parser.is_finished() << std::endl;
		if (_parser.is_finished() == true) {
			_request = _parser.get_request();
			mode = ClientMode::BUILD_RESPONSE;
			_send_data.pos = 0;
			_send_data.response = "";
		}
	} catch (const SendClientError& err) {
		//_default_err_response(connection, err);
	}
}

/*
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
	_write_fd(ClientMode::SENDING, fd, true);
	//mode = ClientMode::SENDING;
}
*/

ServerConfigFile&	Client::_select_config(
	std::vector<ServerConfigFile>& server_configs, Request& request)
{
	if (request._headers.find(HeaderType::HOST) == request._headers.end()) {
		return (server_configs[0]);
	}
	std::string	to_match = request._headers[HeaderType::HOST];
	//not sure which one
	//std::string	to_match = request._headers[HeaderType::AUTHORIZATION];

	std::transform(to_match.begin(), to_match.end(), to_match.begin(),
		[](unsigned char c) { return (std::tolower(c));});

	for (ServerConfigFile& config : server_configs) {
		const std::vector<std::string>&	names = config.getServerNames();
		for (const auto& name : names) {
			if (name == to_match) {
				return (config);
			}
		}
	}
	return (server_configs[0]);
}

void	Client::execute(void) {
	if (is_ready(POLLHUP)) {
		std::cout << FT_ANSI_YELLOW << name << "(idx " << data_idx
			 << ") has disconnected by itself\n" FT_ANSI_RESET;
		set_close();
		return ;
	}
	if (is_ready(POLLERR)) {
		//todo: handle err
		std::cout << FT_ANSI_RED << name << "(idx " << data_idx
			 << ") had a poll exception, idk what that means..\n" FT_ANSI_RESET;
		set_close();
		return ;
	}
	std::cout << FT_ANSI_GREEN "Client " << this->data_idx << ": ";
	std::cout << FT_ANSI_RESET;
	switch (this->mode) {
		case (ClientMode::RECEIVING): {
			std::cout << "receiving\n";
			_receive_request();
			break ;
		}
		case (ClientMode::BUILD_RESPONSE): {
			std::cout << "exec\n";
			if (_response == nullptr) {
				_response = new Response(_select_config(server->configs, _request), _request, *this, mode);
			}
			_response->execute();
			if (mode == ClientMode::SENDING) {
				_send_data.response = _response->get_str_response();
				_send_data.pos = 0;
				delete _response;
				_response = nullptr;
			}
			break ;
		}
		case (ClientMode::SENDING): {
			std::cout << "sending\n";
			_send_response();
			break ;
		}
		case (ClientMode::READING_FD):
			std::cout << "read fd\n";
			// do nothing
			break ;
		case (ClientMode::WRITING_FD): {
			std::cout << "write fd\n";
			// do nothing
			break ;
		}
		case (ClientMode::TESTING_MODE): {
			std::cout << "testing mode\n";
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
		std::cout << "not rdy\n";
		FT_ASSERT(this->poll_events & POLLOUT);
		auto now = std::chrono::high_resolution_clock::now();
		auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - _last_availability).count();
		if (elapsed_ms > 2000) {
			std::cerr << "client " << data_idx << " timed out..\n";
			set_close();
		}
		return ;
	}
	_last_availability = std::chrono::high_resolution_clock::now();
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
			int dbg_fd= open("response.txt", O_WRONLY | O_TRUNC | O_CREAT, 0777);
			std::cout << getenv("PWD");
			write(dbg_fd, _send_data.response.c_str(), _send_data.response.size());
			close(dbg_fd);
			if (errno) {
				std::cout << strerror(errno);
			}
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
		set_close();
	}
}

ClientMode&	Client::get_mode(void) {
	return (mode);
 }
