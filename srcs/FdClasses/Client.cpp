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
	_last_availability(std::chrono::steady_clock::now()),
	_parser(input),
	response(nullptr)
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
	if (fd < 0) {
		//todo: out of fds: manager panic
	}
	_set_non_blocking();
	std::cout << "Connection accepted from address("
		<< inet_ntoa(addr.sin_addr) << "): PORT("
		<< ntohs(addr.sin_port) << ")\n";
}

Client::~Client(void) {
	delete response;
}

void	Client::_receive_request(void) {
	if (!is_ready(POLLIN)) {
		std::cout << "not ready\n";
		auto now = std::chrono::steady_clock::now();
		auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - _last_availability).count();
		if (elapsed_ms > 2000) {
			std::cerr << "client " << data_idx << " timed out..\n";
			set_close();
		}
		return ;
	}
	_last_availability = std::chrono::steady_clock::now();
	char		buffer[4096];
	int			recv_flags = MSG_DONTWAIT;
	long int	bytes_read = recv(this->fd, buffer, sizeof buffer - 1, recv_flags);
	if (bytes_read < 0) {
		std::cerr << "Error: read failed\n";
		set_close();
		return ;
	}
	buffer[bytes_read] = 0;//not needed, only for debugging
	std::cout << "Read:\n" << buffer << '\n';
	this->input.append(buffer, static_cast<size_t>(bytes_read));

	this->parse();
	//std::cout << "Request STATUS = " << _parser.is_finished() << std::endl;
	if (_parser.is_finished() == true)
	{
		int errorCode;
		if ((errorCode = _parser.getErrorCode()))
		{
			std::cout << "Parser had Error but handling is not yet implemented\n";
			// build also response based on the error code 
		}
		else
		{
			_request = _parser.get_request();
			////to avoid issues later on removes the last '/'
			//if (_request._uri.length() > 1 && _request._uri.back() == '/') {
			//	_request._uri.pop_back();
			//}
		}
		mode = ClientMode::BUILD_RESPONSE;
		_send_data.pos = 0;
		_send_data.response = "";
	}
}

ServerConfigFile&	Client::_select_config(
	std::vector<ServerConfigFile>& server_configs, Request& request)
{
	if (request._headers.find(HeaderType::HOST) == request._headers.end()) {
		return (server_configs[0]);
	}
	std::string	to_match = request._headers[HeaderType::HOST];

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
		std::cout << FT_ANSI_RED << name << "(idx " << data_idx
			 << ") had a poll exception\n" FT_ANSI_RESET;
		set_close();
		return ;
	}
	std::cout << FT_ANSI_GREEN << name << " (idx " << this->data_idx << "): ";
	std::cout << FT_ANSI_RESET;
	switch (this->mode) {
		case (ClientMode::RECEIVING): {
			std::cout << "receiving\n";
			_receive_request();
			break ;
		}
		case (ClientMode::BUILD_RESPONSE): {
			std::cout << "exec\n";
			if (response == nullptr) {
				response = new Response(_select_config(server->configs, _request), _request, *this, mode);
			}
			response->execute();
			if (mode == ClientMode::SENDING) {
				_send_data.response = response->get_str_response();
				_send_data.pos = 0;
				delete response;
				response = nullptr;
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
	}
}

void	Client::parse() {
	_parser.parse();
}

void	Client::_send_response(void) {
	if (!this->is_ready(POLLOUT)) {
		std::cout << "not rdy\n";
		FT_ASSERT(this->poll_events & POLLOUT);
		auto now = std::chrono::steady_clock::now();
		auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - _last_availability).count();
		if (elapsed_ms > 2000) {
			std::cerr << "client " << data_idx << " timed out..\n";
			set_close();
		}
		return ;
	}
	_last_availability = std::chrono::steady_clock::now();

	const int send_flags = 0;//todo: research flags
	//std::cout << "sending:\n" << _send_data.response << "\n";
	ssize_t send_bytes = send(
		fd,
		_send_data.response.c_str() + _send_data.pos,
		_send_data.response.size() - _send_data.pos,
		send_flags
	);
		{
			//remove this later
			int dbg_fd= open("response.txt", O_WRONLY |O_CLOEXEC| O_TRUNC | O_CREAT, 0777);
			std::cout << getenv("PWD");
			write(dbg_fd, _send_data.response.c_str(), _send_data.response.size());
			ft_close(dbg_fd);
			if (errno) {
				std::cout << strerror(errno);
			}
		}

	if (send_bytes <= 0) {
		std::cerr << "Error: " << name << ": send: closing connection now\n";
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

void	Client::set_close(void) {
	if (response) {
		response->close_io_fds();
	}
	Client::BaseFd::set_close();
}
