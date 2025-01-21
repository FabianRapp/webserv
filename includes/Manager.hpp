#pragma once
#include <colors.h>
#include <Token.hpp>
#include <msg.hpp>
#include <Parser.hpp>
#include <utils.hpp>
#include <types.hpp>
#include <Exceptions.hpp>

#include <thread>
#include <unistd.h>
#include <cstring>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <poll.h>

#include <cstdlib>
#include <vector>
#include <sys/poll.h>
#include <unistd.h>
#include <cassert>
#include <cerrno>
#include <iostream>

#ifndef REQUEST_QUE_SIZE
# define REQUEST_QUE_SIZE 10
#endif


#include <FdClasses.hpp>

class BaseFd;
class Server;
class Client;
class Config;

class DataManager {
public:
	DataManager(void);
	~DataManager(void);

	Server*	new_server(Config config);
	Client*	new_client(Server* server);

	void	set_close(size_t idx);

	void	process_closures();

	bool	is_ready(size_t idx, short event);
	
	int		get_fd(size_t idx);

	void	run_poll();

	void	execute_all(void);


private:
	void	_add_entry(BaseFd *entry, short poll_events);

	void	_fd_close(size_t idx);

	size_t							_count;
	std::vector<struct pollfd>		_pollfds;
	std::vector<bool>				_close_later;
	std::vector<BaseFd*>			_fd_users;
};

