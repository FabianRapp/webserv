#pragma once

#include "general_includes.hpp"
#include "parser/Token.hpp"
#include "msg.hpp"
#include "parser/Parser.hpp"
#include "utils.hpp"
#include "types.hpp"
#include "Exceptions.hpp"

// #include <types.hpp>

#include "FdClasses/Server.hpp"
#include "FdClasses/Client.hpp"
#include "FdClasses/ReadFd.hpp"

#include <thread>
#include <unistd.h>
#include <cstring>
#include <errno.h>
#include <iostream>
#include <string>

#include <cstdlib>
#include <vector>
#include <unistd.h>
#include <cassert>
#include <cerrno>
#include <iostream>
#include "../includes/ConfigParser/ConfigParser.hpp"


#include "FdClasses/BaseFd.hpp"
// #include <types.hpp>


class DataManager {
public:
	DataManager(void);
	~DataManager(void);

	void	new_server(std::vector<ServerConfigFile>& configs);
	void	new_client(Server* server);
	ReadFd*	new_read_fd(std::string& target_buffer, int fd,
			ssize_t byte_count, std::function<void()> callback);

	void	set_close(size_t idx);

	bool	closing(size_t idx) const;
	void	process_closures();

	bool	is_ready(size_t idx, short event);

	int		get_fd(size_t idx);

	void	run_poll();

	void	execute_all(void);

	ConfigParser	*config_parser;
private:
	size_t	_total_entrys;
	void	_add_entry(BaseFd *entry, short poll_events);

	void	_fd_close(size_t idx);

	size_t							_count;
	std::vector<struct pollfd>		_pollfds;
	std::vector<bool>				_close_later;
	std::vector<BaseFd*>			_fd_users;
	size_t							_consecutive_poll_fails;
};

