#include <Manager.hpp>


DataManager::DataManager(void): _total_entrys(0), _count(0) {}

DataManager::~DataManager(void) {
}

Server*	DataManager::new_server(Config config) {
 	Server* server = new Server(*this, config);
	_add_entry(reinterpret_cast<BaseFd*>(server), server->poll_events);
	return (server);
}

Client*	DataManager::new_client(Server* server) {
 	Client* client = new Client(*this, server);
	_add_entry(reinterpret_cast<BaseFd*>(client), client->poll_events);
	return (client);
}

ReadFd*	DataManager::new_read_fd(std::string& target_buffer, int fd,
			ssize_t byte_count, std::function<void()> callback) {
	ReadFd*	reader = new ReadFd(*this, target_buffer, fd, byte_count, callback);
	_add_entry(reinterpret_cast<BaseFd*>(reader), reader->poll_events);
	return (reader);
}

void	DataManager::set_close(size_t idx) {
	_close_later[idx] = true;
}

bool	DataManager::closing(size_t idx) const {
	return (_close_later[idx]);
}

void	DataManager::process_closures() {
	for (size_t idx = 0; idx < _count; idx++) {
		if (_close_later[idx]) {
			_fd_close(idx);
		}
	}
}

bool	DataManager::is_ready(size_t idx, short event) {
	return (_pollfds[idx].revents & event);
}

int	DataManager::get_fd(size_t idx) {
	return (_pollfds[idx].fd);
}

void	DataManager::run_poll() {
	if (poll(&_pollfds[0], static_cast<nfds_t>(_count), 0) < 0) {
		std::cerr << "Error: poll: " << strerror(errno) << '(' << errno << ")\n";
		assert(0);
	}
}

void	DataManager::execute_all(void) {
	for (auto & user : _fd_users) {
		user->execute();
	}
}

void	DataManager::_add_entry(BaseFd *entry, short poll_events) {
	_total_entrys++;
	entry->data_idx = _count++;
	std::cout << "_add_entry: count: " << _count << " ; total count: " << _total_entrys << "\n";
	assert(entry && entry->fd != -1);
	struct pollfd	poll_fd = {
		.fd = entry->fd,
		.events = poll_events,
		.revents = 0,
	};
	_pollfds.push_back(poll_fd);
	_fd_users.push_back(entry);
	_close_later.push_back(false);
}

void	DataManager::_fd_close(size_t idx) {
	delete _fd_users[idx];
	close(_pollfds[idx].fd);
	_count--;
	if (idx < _count) {
		_pollfds[idx] = *(_pollfds.end() - 1);
		_close_later[idx] = *(_close_later.end() - 1);
		_fd_users[idx] = *(_fd_users.end() - 1);
		_fd_users[idx]->data_idx = idx;
	}
	_pollfds.pop_back();
	_close_later.pop_back();
	_fd_users.pop_back();
}

