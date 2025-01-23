#include "../includes/ClientConnections.hpp"

ClientConnections::ClientConnections(void)
:	_active_client_count(0)
{
	std::lock_guard<std::mutex>	lock(_mutex);
	memset(_client_connections, 0, sizeof _client_connections);
	for (auto & poll_fd : _client_fds) {
		poll_fd.fd = -1;
		poll_fd.events = 0;
		poll_fd.revents = 0;
	}
}

ClientConnections::~ClientConnections(void) {
	std::lock_guard<std::mutex>	lock(_mutex);
	for (auto & poll_fd : _client_fds) {
		if (poll_fd.fd > 0) {
			close(poll_fd.fd);
		}
	}
}

void	ClientConnections::close_client_connection(ClientConnection *connection) {
	std::lock_guard<std::mutex>	lock(_mutex);
	int	idx = connection->client_idx;
	auto &	poll_fd = _client_fds[idx];
	FT_ASSERT(poll_fd.fd > 0);
	FT_ASSERT(_client_connections[idx] == connection);
	delete connection;
	_active_client_count--;
	while (idx < static_cast<int>(_active_client_count)) {
		_client_connections[idx] = _client_connections[idx + 1];
		_client_fds[idx] = _client_fds[idx + 1];
	}
	_client_connections[idx] = nullptr;
	_client_fds[idx].events = 0;
	_client_fds[idx].revents = 0;
	_client_fds[idx].fd = -1;
}

ClientConnections::PollIterator::PollIterator(ClientConnection **connections,
	int connection_count, struct pollfd *poll_fds, short int event)
:	_connections(connections),
	_poll_fds(poll_fds),
	_event(event),
	_idx(0)
{
	while (_idx < connection_count) {
		if (_poll_fds[_idx].revents & _event) {
			_begin_idx = _idx;
			break ;
		}
		_idx++;
	}
	if (_idx == connection_count) {
		_begin_idx = connection_count;
		_end_idx = _begin_idx;
		return ;
	}
	while (_idx < connection_count) {
		if (_poll_fds[_idx].revents & _event) {
			_end_idx = _idx + 1;
		}
		_idx++;
	}
	_idx = _begin_idx;
}

ClientConnections::PollIterator::PollIterator(const ClientConnections::PollIterator &old)
:	_connections(old._connections),
	_poll_fds(old._poll_fds),
	_begin_idx(old._begin_idx),
	_end_idx(old._end_idx),
	_event(old._event),
	_idx(old._idx)
{}

void	ClientConnections::PollIterator::set_begin(void) {
	this->_idx = this->_begin_idx;
}

void	ClientConnections::PollIterator::set_end(void) {
	this->_idx = this->_end_idx;
}

ClientConnections::PollIterator&	ClientConnections::PollIterator::operator=(const PollIterator &old) {
	if (this == &old) {
		return (*this);
	}
	this->_begin_idx = old._begin_idx;
	this->_end_idx = old._end_idx;
	this->_connections = old._connections;
	this->_poll_fds = old._poll_fds;
	this->_event = old._event;
	this->_idx = old._idx;
	return (*this);
}

ClientConnections::PollIterator&	ClientConnections::PollIterator::operator++(void) {
	if (_idx >= _end_idx) {
		return (*this);
	}
	while (++_idx < _end_idx) {
		if (_poll_fds[_idx].revents & _event) {
			return (*this);
		}
		_idx++;
	}
	return (*this);
}

ClientConnections::PollIterator	ClientConnections::PollIterator::operator++(int) {
	if (_idx >= _end_idx) {
		return (*this);
	}
	ClientConnections::PollIterator	tmp = *this;
	while (++_idx < _end_idx) {
		if (_poll_fds[_idx].revents & _event) {
			return (tmp);
		}
		_idx++;
	}
	return (tmp);
}

ClientConnections::PollIterator&	ClientConnections::PollIterator::operator--(void) {
	if (_idx <= _begin_idx) {
		return (*this);
	}
	while (--_idx > _begin_idx) {
		if (_poll_fds[_idx].revents & _event) {
			return (*this);
		}
		_idx++;
	}
	return (*this);
}

ClientConnections::PollIterator	ClientConnections::PollIterator::operator--(int) {
	if (_idx >= _begin_idx) {
		return (*this);
	}
	ClientConnections::PollIterator	tmp = *this;
	while (++_idx < _begin_idx) {
		if (_poll_fds[_idx].revents & _event) {
			return (tmp);
		}
		_idx++;
	}
	return (tmp);
}
ClientConnection&	ClientConnections::PollIterator::operator*(void) {
	FT_ASSERT(_idx >= _begin_idx);
	FT_ASSERT(_idx < _end_idx);
	return (*_connections[_idx]);
}

/* assumes right to be of the same container */
bool	ClientConnections::PollIterator::operator==(const PollIterator & right) {
	return (this->_idx == right._idx);
}

/* assumes right to be of the same container */
bool	ClientConnections::PollIterator::operator!=(const PollIterator & right) {
	return (this->_idx != right._idx);
}

/* assumes right to be of the same container */
bool	ClientConnections::PollIterator::operator<=(const PollIterator & right) {
	return (this->_idx <= right._idx);
}

/* assumes right to be of the same container */
bool	ClientConnections::PollIterator::operator>=(const PollIterator & right) {
	return (this->_idx >= right._idx);
}

/* assumes right to be of the same container */
bool	ClientConnections::PollIterator::operator<(const PollIterator & right) {
	return (this->_idx < right._idx);
}

/* assumes right to be of the same container */
bool	ClientConnections::PollIterator::operator>(const PollIterator & right) {
	return (this->_idx > right._idx);
}

void	ClientConnections::set_and_poll(short int events) {
	std::lock_guard<std::mutex>	lock(_mutex);
	for (auto & client : _client_fds) {
		client.events = events;
	}
	if (poll(_client_fds, _active_client_count, 0) < 0) {
		std::cerr << "Error: poll: " << strerror(errno) << '(' << errno << ")\n";
		FT_ASSERT(0);
	}
}

nfds_t	ClientConnections::get_count(void) {
	std::lock_guard<std::mutex>	lock(_mutex);
	return (_active_client_count);
}

ClientConnections::PollIterator	ClientConnections::begin(short int event) {
	PollIterator	it(_client_connections, static_cast<int>(_active_client_count), (struct pollfd *)_client_fds, event);
	it.set_begin();
	return (it);
}

ClientConnections::PollIterator	ClientConnections::end(short int event) {
	PollIterator	it(_client_connections, static_cast<int>(_active_client_count), (struct pollfd *)_client_fds, event);
	it.set_end();
	return (it);
}

ClientConnection*	ClientConnections::add_client(t_fd fd) {
	std::lock_guard<std::mutex>	lock(_mutex);
	FT_ASSERT(_active_client_count < MAX_CLIENTS);
	FT_ASSERT(fd > 0);
	_active_client_count++;

	for (size_t i = 0; i < MAX_CLIENTS; i++) {
		if (_client_connections[i] == nullptr) {
			_client_fds[i].fd = fd;
			std::cout << "fd: " << fd << "\ni: " << i << "\n";
			_client_connections[i] = new ClientConnection(_client_fds[i].fd, static_cast<int>(i));
			return (_client_connections[i]);
		}
	}
	FT_ASSERT(0);
}
