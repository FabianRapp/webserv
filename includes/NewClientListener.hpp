#pragma once

#include <ClientConnections.hpp>
#include <utils.hpp>
#include <thread>
#include <sys/socket.h>
#include <arpa/inet.h>

class NewClientListener {
public:
	NewClientListener(void) = delete;
	NewClientListener(const NewClientListener& old) = delete;
	NewClientListener(const NewClientListener&& old) = delete;
	NewClientListener(std::atomic<bool>& exit, ClientConnections& connections);
	~NewClientListener(void);
	void	set_server_fd(int fd);
	void	run(void);
	void	join_thread(void); /* will wait forever if 'exit' is not set */

private:
	void	_loop(void);
	std::thread			_thread;
	int					_server_fd;
	std::atomic<bool>&	_exit;
	ClientConnections&	_connections;
};
