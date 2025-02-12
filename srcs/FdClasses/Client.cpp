#include "../../includes/FdClasses/Client.hpp"
#include "../../includes/FdClasses/Server.hpp"
#include "../../includes/Manager.hpp"
#include "../../includes/Response.hpp"
#include "../../includes/macros.h"

Client::Client(DataManager& data, Server* parent_server):
	BaseFd(data, POLLIN | POLLOUT, "ClientHandler"),
		response(nullptr),
	mode(ClientMode::RECEIVING),

	_send_data({"", 0}),
	_parser(input, parent_server->configs),
	_last_availability(std::chrono::steady_clock::now())

{
	this->server = parent_server;
	assert(server->is_ready(POLLIN));
	errno = 0;

	socklen_t				addr_len = static_cast<socklen_t>(sizeof addr);
	struct sockaddr			*const addr_ptr =
		reinterpret_cast<struct sockaddr *>(&addr);
	memset(&addr, 0, sizeof addr);// might not be needed
	fd = accept(server->fd, addr_ptr, &addr_len);
	if (fd < 0) {
		LOG("Error: " << name << "could not be accepted\n");
		set_close();
		return ;
	}
	name = name + ": " + std::string(inet_ntoa(addr.sin_addr)) + "::" +
		std::to_string(ntohs(addr.sin_port));
	_set_non_blocking();
}

Client::~Client(void) {
	delete response;
}

void	Client::_receive_request(void) {
	if (!is_ready(POLLIN)) {
		LOG_FABIAN3("not ready\n");
		auto now = std::chrono::steady_clock::now();
		auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - _last_availability).count();
		if (elapsed_ms > 2000) {
			LOG(FT_ANSI_YELLOW << name <<": timed out..\n" FT_ANSI_RESET);
			set_close();
		}
		return ;
	}
	_last_availability = std::chrono::steady_clock::now();
	static
	char		buffer[1024 * 1024 * 100]; //100 mb
	int			recv_flags = MSG_DONTWAIT;
	long int	bytes_read = recv(this->fd, buffer, sizeof buffer - 1, recv_flags);

	if (bytes_read <= 0) {
		LOG(FT_ANSI_RED "Error: recv failed\n" FT_ANSI_RESET);
		set_close();
		return ;
	}
	size_t	max_loged_chars = 10;
	std::string	log_chars(buffer, std::min(static_cast<size_t>(bytes_read), max_loged_chars));
	LOG(FT_ANSI_BLUE "Received by " << name << ": " << log_chars << "..\n" FT_ANSI_RESET);
	this->input.append(buffer, static_cast<size_t>(bytes_read));

	this->parse();
	if (_parser.is_finished() == true)
	{
		{
			//todo: remove this later
			int	debug_fd = open("request.txt", O_WRONLY | O_TRUNC | O_APPEND | O_CREAT, 0644);
			FT_ASSERT(debug_fd > 0);
			write(debug_fd, input.c_str(), input.size());
			close(debug_fd);
		}
		_request = _parser.move_request();
		mode = ClientMode::BUILD_RESPONSE;
		_send_data.pos = 0;
		_send_data.response = "";
	}
}

void	Client::execute(void) {
	if (is_ready(POLLHUP)) {
		LOG(FT_ANSI_YELLOW << name << "(idx " << data_idx
			 << ") has disconnected by itself\n" FT_ANSI_RESET);
		set_close();
		return ;
	}
	if (is_ready(POLLERR)) {
		LOG(FT_ANSI_RED << name << "(idx " << data_idx
			 << ") had a poll exception\n" FT_ANSI_RESET);
		set_close();
		return ;
	}
	switch (this->mode) {
		case (ClientMode::RECEIVING): {
			LOG_FABIAN3("client: receiving\n");
			_receive_request();
			break ;
		}
		case (ClientMode::BUILD_RESPONSE): {
			LOG_FABIAN3("client: exec\n");
			if (response == nullptr) {
				response = new Response(_parser.get_config(),
						_parser.get_location_config(), _request, *this, mode);
			}
			response->execute();
			if (mode == ClientMode::SENDING) {
				_send_data.response = response->move_str_response();
				_send_data.pos = 0;
				delete response;
				response = nullptr;
			}
			break ;
		}
		case (ClientMode::SENDING): {
			LOG_FABIAN3("sending\n");
			_send_response();
			break ;
		}
		case (ClientMode::READING_FD):
			// do nothing
			break ;
		case (ClientMode::WRITING_FD): {
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
		FT_ASSERT(this->poll_events & POLLOUT);
		auto now = std::chrono::steady_clock::now();
		auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - _last_availability).count();
		if (elapsed_ms > 2000) {
			LOG(FT_ANSI_YELLOW << name <<": timed out..\n" FT_ANSI_RESET);
			set_close();
		}
		return ;
	}
	_last_availability = std::chrono::steady_clock::now();

	const int send_flags = MSG_DONTWAIT;
	ssize_t send_bytes = send(
		fd,
		_send_data.response.c_str() + _send_data.pos,
		_send_data.response.size() - _send_data.pos,
		send_flags
	);
		{
			//remove this later
			int dbg_fd= open("response.txt", O_WRONLY |O_CLOEXEC| O_TRUNC | O_CREAT, 0777);
			write(dbg_fd, _send_data.response.c_str(), _send_data.response.size());
			ft_close(dbg_fd);
			if (errno) {
				LOG(strerror(errno) << std::endl);
			}
		}

	if (send_bytes <= 0) {
		LOG(FT_ANSI_RED "Error: " << name << ": send: closing connection now\n" FT_ANSI_RESET);
		set_close();
		return ;
	}
	LOG(FT_ANSI_BLUE "Send by " << name << ": " << _send_data.response.substr(_send_data.pos, 10) << "..\n" FT_ANSI_RESET);
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
