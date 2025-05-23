#include "../includes/Manager.hpp"
#include "../includes/ConfigParser/ServerConfigFile.hpp"


//maksim delete
// DataManager::DataManager(void): config_parser(nullptr), _total_entrys(0), _count(0),
// 	cgi_lifetimes(std::chrono::seconds(3)),
// 	_consecutive_poll_fails(0)
// {}

DataManager::DataManager(void):
		config_parser(nullptr),
		cgi_lifetimes(std::chrono::seconds(3)),
		_total_entrys(0),
		_count(0),
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
		LOG(FT_ANSI_RED_BOLD "Error creating server: " << err.what() << "\n" FT_ANSI_RESET);
		return ;
	}
	_add_entry(reinterpret_cast<BaseFd*>(server), server->poll_events);
}

void	DataManager::new_client(Server* server) {
 	Client* client = new Client(*this, server);
	_add_entry(reinterpret_cast<BaseFd*>(client), client->poll_events);
}

ReadFd*	DataManager::new_read_fd(Response& response, std::string& target_buffer, int fd, Client& client,
			ssize_t byte_count, const std::string& fd_name, std::function<void()> callback) {
	ReadFd*	reader = new ReadFd(*this, response, target_buffer, fd, client, byte_count,
						callback);
	reader->name = fd_name;
	_add_entry(reinterpret_cast<BaseFd*>(reader), reader->poll_events);
	return (reader);
}

WriteFd*	DataManager::new_write_fd(Response& response, int fd, const std::string_view& input_data, Client& client,
				const std::string& fd_name, std::function<void()> callback) {
	WriteFd*	writer = new WriteFd(*this, response, input_data, fd, client, callback);
	writer->name = fd_name;
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

// marks everything of that server for cleanup
void	DataManager::_handle_server_panic(Server* serv) {
	serv->panic = true;
	std::cerr << FT_ANSI_RED << "Error: server panic!"
		<< "Restarting server with configs:\n";
	for (const auto& conf : serv->configs) {
		conf.printServer();
	}
	std::cerr << FT_ANSI_RESET;
	for (size_t idx = 0; idx < _count; idx++) {
		Client*		client = dynamic_cast<Client*>(_fd_users[idx]);
		ReadFd*		read_fd = dynamic_cast<ReadFd*>(_fd_users[idx]);
		WriteFd*	write_fd = dynamic_cast<WriteFd*>(_fd_users[idx]);
		if (client && client->server == serv) {
			set_close(idx);
		} else if (read_fd && read_fd->get_server() == serv) {
			set_close(idx);
		} else if (write_fd && write_fd->get_server() == serv) {
			set_close(idx);
		}
	}
}

void	DataManager::process_closures() {
	size_t idx = 0;

	while (idx < _count) {
		if (_close_later[idx]) {
			Server*	serv = dynamic_cast<Server*>(_fd_users[idx]);
			if (serv && !serv->panic) {
				serv->panic = true;
				_handle_server_panic(serv);
				idx = 0;
				continue ;
			}
			_fd_close(idx);
		} else {
			idx++;
		}
	}
}

bool	DataManager::is_ready(size_t idx, short event) {
	return (_pollfds[idx].revents & event);
}

int	DataManager::get_fd(size_t idx) {
	return (_pollfds[idx].fd);
}


size_t	DataManager::get_count(void) const {
	return (_count);
}

void	DataManager::run_poll() {
	if (poll(&_pollfds[0], static_cast<nfds_t>(_count), 0) < 0) {
		LOG("Error: poll: " << strerror(errno) << "\n");
		FT_ASSERT(errno != EINVAL && errno != EFAULT && errno != EBADF);// would indicate a bug
		if (errno != EINTR && ++_consecutive_poll_fails > 1000) {
			throw (std::ios_base::failure(std::string("poll: ")
				+ strerror(errno) + "failed more than 1000 times in a row"));
		}
		for (auto& pollfd : _pollfds) {
			pollfd.revents = 0;
		}
		return ;
	}
	_consecutive_poll_fails = 0;
}

void	DataManager::execute_all(void) {
	size_t	count = _count;

	// fabi: 07/02: canged loop para from _count to count:
	// i think _count would just check the same + new obj where none of the
	// new can possibly be ready(since they were not polled yet)
	for (size_t i = 0; i < count; i++) {
		BaseFd* user = _fd_users[i];
		if (user->name != "Server") {
			LOG_FABIAN3("Manager: execec " << user->name << "\n");
		}
		if (exit_) {
			break ;
		}
		user->execute();
	}
}

void	DataManager::_add_entry(BaseFd *entry, short poll_events) {
	_total_entrys++;
	entry->data_idx = _count++;
	LOG(FT_ANSI_BLUE "Added new fd object: " << entry->name
		<< "; Current fd object count: " << _count
		<< "; Total count: " << _total_entrys << "\n" FT_ANSI_RESET);
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
	LOG(FT_ANSI_BLUE "Closing fd-object " << _fd_users[idx]->name
		<< "\n" FT_ANSI_RESET);
	delete _fd_users[idx];
	if (_pollfds[idx].fd > 0) {
		ft_close(_pollfds[idx].fd);
	}
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

	LOG(FT_ANSI_BLUE "fd-object count: " << _count << FT_ANSI_RESET << std::endl);
}

size_t	DataManager::get_current_count(void) const {
	return (_fd_users.size());
}

size_t		DataManager::get_total_count(void) const {
	return (_total_entrys);
}
