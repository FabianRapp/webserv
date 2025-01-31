#include "../includes/Manager.hpp"
#include "../includes/ConfigParser/ServerConfigFile.hpp"

DataManager::DataManager(void): config_parser(nullptr), _total_entrys(0), _count(0),
	_consecutive_poll_fails(0)
{}

DataManager::~DataManager(void) {
	delete config_parser;
	while (_count) {
		_fd_close(0);
	}
}

void	DataManager::new_server(std::vector<ServerConfigFile>& configs) {
	Server*	server;
	try {
		server = new Server(*this, configs);
	} catch (const Server::ServerError& err) {
		std::cerr << "Error creating server: " << err.what() << "\n";
		return ;
	}
	_add_entry(reinterpret_cast<BaseFd*>(server), server->poll_events);
}

void	DataManager::new_client(Server* server) {
 	Client* client = new Client(*this, server);
	_add_entry(reinterpret_cast<BaseFd*>(client), client->poll_events);
}

ReadFd*	DataManager::new_read_fd(std::string& target_buffer, int fd,
			ssize_t byte_count, std::function<void()> callback) {
	ReadFd*	reader = new ReadFd(*this, target_buffer, fd, byte_count, callback);
	_add_entry(reinterpret_cast<BaseFd*>(reader), reader->poll_events);
	return (reader);
}

WriteFd*	DataManager::new_write_fd(int fd, std::string_view& input_data, std::function<void()> callback) {
	WriteFd*	writer = new WriteFd(*this, input_data, fd, callback);
	_add_entry(reinterpret_cast<WriteFd*>(writer), writer->poll_events);
	return (writer);
}

void	DataManager::set_close(size_t idx) {
	_close_later[idx] = true;
	_pollfds[idx].revents = 0;
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
		std::cerr << "Error: poll: " << strerror(errno) << "\n";
		_consecutive_poll_fails++;
		/*todo:
		if (_consecutive_poll_fails > some value) {
			throw ( error to indicate exit);
		}
		*/
		for (auto& pollfd : _pollfds) {
			pollfd.revents = 0;
		}
		return ;
	}
	_consecutive_poll_fails = 0;
}

void	DataManager::execute_all(void) {
	size_t	count = _count;
	for (size_t i = 0; i < _count; i++) {
		BaseFd* user = _fd_users[i];
		if (user->name != "Server") {
			std::cout << "Manager: execec " << user->name << "\n";
		}
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
	std::cout << "added fd " << _fd_users[_count - 1]->name << " with idx " << _count -1 << "\n";
}

void	DataManager::_fd_close(size_t idx) {
	std::cout << "closing fd " << _fd_users[idx]->name << " with idx " << idx << "\n";
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

