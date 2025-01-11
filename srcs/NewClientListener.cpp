#include <NewClientListener.hpp>

NewClientListener::NewClientListener(std::atomic<bool>& exit,
	ClientConnections& connections)
	:_exit(exit),
	_connections(connections)
{
	_server_fd = -1;
}

NewClientListener::~NewClientListener(void) {
	if (_server_fd > 0) {
		close(_server_fd);
		_server_fd = -1;
	}
}

void	NewClientListener::set_server_fd(int fd) {
	_server_fd = fd;
}

void	NewClientListener::run(void) {
	_thread = std::thread(&NewClientListener::_loop, this);
}

void	NewClientListener::join_thread(void) {
	_thread.join();
}

void	NewClientListener::_loop(void) {
	size_t	total_client_count = 0;
	errno = 0;
	struct sockaddr_in		addr;
	socklen_t				addr_len = static_cast<socklen_t>(sizeof addr);
	struct sockaddr			*const addr_ptr =
		reinterpret_cast<struct sockaddr *>(&addr);
	std::cout << "entering listerner loop..\n";
	while (!_exit) {
		FT_ASSERT(_connections.get_count() <= MAX_CLIENTS);
		if (_connections.get_count() == MAX_CLIENTS) {
			/*todo: send basic error response */
			continue ;
		}

		int	new_client_fd;
		while (_connections.get_count() < MAX_CLIENTS)
		{
			struct pollfd	poll_fd = {
				.fd = _server_fd,
				.events = POLLIN,
				.revents = 0,
			};
			if (poll(&poll_fd, 1, 0) < 0) {
				FT_ASSERT(0 && "server_fd poll failed");
			}
			if (!(poll_fd.revents & POLLIN)) {
				continue ;
			}
			memset(&addr, 0, sizeof addr);// might not be needed
			new_client_fd = accept(_server_fd, addr_ptr, &addr_len);
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				FT_ASSERT(0 && "Should have been handled by poll");
			}
			new_client_fd = set_fd_non_block(new_client_fd);
			if (new_client_fd < 0) {
				FT_ASSERT(0);
			}
			_connections.add_client(new_client_fd);
			total_client_count++;
			std::cout << "Connection accepted from address("
				<< inet_ntoa(addr.sin_addr) << "): PORT("
				<< ntohs(addr.sin_port) << ")\n";
			std::cout << "In total " << total_client_count << " clients "
				"connected to the server\n";
		}
	}
}
