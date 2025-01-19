#pragma once

#include <cstdlib>
#include <vector>
#include <sys/poll.h>
#include <unistd.h>
#include <cassert>
#include <cerrno>
#include <iostream>

#include "FdClasses.hpp"

class BaseFd;

class DataManager {
public:
	DataManager(void): _count(0) {}
	~DataManager(void);

	Server*	new_server(Config config) {
	 	Server* server = new Server(*this, config);
		_add_entry(server, server->poll_events);
		return (server);
	}

	void	set_close(size_t idx) {
		_close_later[idx] = true;
	}

	void	process_closures() {
		for (size_t idx = 0; idx < _count; idx++) {
			if (_close_later[idx]) {
				_fd_close(idx);
			}
		}
	}

	bool	is_ready(size_t idx, short event) {
		return (_pollfds[idx].revents & event);
	}
	
	int	get_fd(size_t idx) {
		return (_pollfds[idx].fd);
	}

	void	run_poll() {
		if (poll(&_pollfds[0], static_cast<nfds_t>(_count), 0) < 0) {
			std::cerr << "Error: poll: " << strerror(errno) << '(' << errno << ")\n";
			assert(0);
		}
	}

	void	execute_all(void) {
		for (auto & user : _fd_users) {
			user->execute();
		}
	}


private:
	void	_add_entry(BaseFd *entry, short poll_events) {
		entry->data_idx = _count++;
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

	void	_fd_close(size_t idx) {
		delete _fd_users[idx];
		close(_pollfds[idx].fd);
		_pollfds[idx] = *(_pollfds.end() - 1);
		_close_later[idx] = *(_close_later.end() - 1);
		_fd_users[idx] = *(_fd_users.end() - 1);
		_fd_users[idx]->data_idx = idx;
		_pollfds.pop_back();
		_close_later.pop_back();
		_fd_users.pop_back();
	}

	size_t							_count;
	std::vector<struct pollfd>		_pollfds;
	std::vector<bool>				_close_later;
	std::vector<BaseFd*>			_fd_users;
};

