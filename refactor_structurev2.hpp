#pragma once

#include <cstdlib>
#include <vector>
#include <sys/poll.h>
#include <unistd.h>
#include <cassert>
#include <cerrno>
#include <iostream>

class BaseFd;

class DataManager {
public:
	DataManager(void): count(0) {}
	~DataManager(void);
	void	add_entry(BaseFd *entry, short poll_events);
	void	remove_entry(BaseFd *entry);

	void	process_closures() {
		for (size_t idx = 0; idx < count; idx++) {
			if (close_later[idx]) {
				fd_close(idx);
			}
		}
	}

	void	fd_close(size_t idx) {
		close(pollfds[idx].fd);
		pollfds[idx] = *(pollfds.end() - 1);
		close_later[idx] = *(close_later.end() - 1);
		fd_users[idx] = *(fd_users.end() - 1);
		fd_users[idx]->data_idx = idx;
		pollfds.pop_back();
		close_later.pop_back();
		fd_users.pop_back();
	}

	bool	is_ready(size_t idx, short event) {
		return (pollfds[idx].revents & event);
	}
	
	int	get_fd(size_t idx) {
		return (pollfds[idx].fd);
	}

	void	run_poll() {
		if (poll(&pollfds[0], static_cast<nfds_t>(count), 0) < 0) {
			std::cerr << "Error: poll: " << strerror(errno) << '(' << errno << ")\n";
			assert(0);
		}
	}

	void	execute_all(void) {
		for (auto & user : fd_users) {
			user->execute();
		}
	}

	size_t							count;
	std::vector<struct pollfd>		pollfds;
	std::vector<bool>				close_later;
	std::vector<BaseFd*>			fd_users;
private:
};

class BaseFd {
public:
	BaseFd(DataManager& data, int fd, short poll_events)
	:	data(data),
		fd(fd)
	{
		data.add_entry(this, poll_events);
	}

	virtual
	~BaseFd(void) {
		data.remove_entry(this);
	}

	bool	is_ready(short event) {
		data.is_ready(data_idx, event);
	}

	virtual
	void	execute(void) = 0;


	DataManager&	data;
	int				fd;
	size_t			data_idx;
private:
};

class Server: public BaseFd {
public:
	Server(Config config)
	:	BaseFd()
	{
	}
	Config	config;
private:
};

class FileIo: public BaseFd {
     ssize_t	(*read_write)(int fildes, void *buf, size_t nbyte);


};
