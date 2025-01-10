#include <ClientConnection.hpp>

ClientConnection::ClientConnection(const t_fd & fd, int idx)
:	fd(fd),
	client_idx(idx),
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

