#include <ClientConnection.hpp>

ClientConnection::ClientConnection(const t_fd & fd, int idx)
:	current_mode(ConnectionMode::RESEIVING),
	fd(fd),
	client_idx(idx),
	close_after_loop(false),
	_parser(input)
{}

ClientConnection::~ClientConnection(void) {
	close(fd);
}

bool	ClientConnection::completed_request(void) {
	return (_parser.is_finished());
}

void	ClientConnection::parse(void) {
	_parser.parse();
}

/* only call this if completed_request returned true */
t_http_request	ClientConnection::get_request(void) const {
	//FT_ASSERT(_parser.is_finished());
	return (_parser.get_request());
}

/* sets response and enters ConnectionMode::SENDING */
void	ClientConnection::set_response(std::string&& output,
			bool close_after_send) {
	_send_data.response = std::move(output);
	_send_data.pos = 0;
	_send_data.close_after_send = close_after_send;
	current_mode = ConnectionMode::SENDING;
}

void	ClientConnection::send_response(void) {
	const size_t	rest_size = _send_data.response.size() - _send_data.pos;
	ssize_t	send_ret = send(fd, _send_data.response.c_str() + _send_data.pos,
		rest_size, 0);
	if (send_ret < 0) {
		/* todo: */
		std::cerr << "Error: send error: " << strerror(errno) << '\n';
		exit(1);
	}
	_send_data.pos += static_cast<size_t>(send_ret);
	FT_ASSERT(send_ret > 0);
}

bool	ClientConnection::finished_sending(void) const {
	return (_send_data.pos == _send_data.response.size());
}

bool	ClientConnection::should_close_after_send(void) const {
	return (_send_data.close_after_send);
}
