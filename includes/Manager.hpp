#pragma once

#include "general_includes.hpp"
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
#include "CgiTimeouts.hpp"

#include "FdClasses/BaseFd.hpp"
// #include <types.hpp>


//todo: needs panic functionality which is like bad alloc i guess
class DataManager {
public:
	DataManager(void);
	~DataManager(void);

	void		new_server(std::vector<ServerConfigFile>& configs);
	void		new_client(Server* server);
	ReadFd*		new_read_fd(std::string& target_buffer, int fd, Client& client,
					ssize_t byte_count, std::function<void()> callback);
	WriteFd*	new_write_fd(int fd, const std::string_view& input_data, Client& client,
					   std::function<void()> callback);
	CgiTimeouts	cgi_lifetimes;


	void	set_close(size_t idx);

	bool	closing(size_t idx) const;
	void	process_closures();

	bool	is_ready(size_t idx, short event);

	int		get_fd(size_t idx);

	void	run_poll();
	size_t	get_count(void) const;

	void	execute_all(void);
	bool	in_panic(void) const;

	ConfigParser	*config_parser;
	size_t		get_total_count(void) const;
private:
	size_t	_total_entrys;
	void	_add_entry(BaseFd *entry, short poll_events);

	void	_fd_close(size_t idx);

	void	_handle_server_panic(Server* server);

	size_t							_count;
	std::vector<struct pollfd>		_pollfds;
	std::vector<bool>				_close_later;
	std::vector<BaseFd*>			_fd_users;
	size_t							_consecutive_poll_fails;
	bool							_panic;
};

