#pragma once

#include <ClientConnections.hpp>
#include <utils.hpp>
#include <thread>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <vector>

class NewClientListener {
public:
	NewClientListener(ClientConnections& connections);
	NewClientListener(void) = delete;
	NewClientListener(const NewClientListener& old) = delete;
	NewClientListener(const NewClientListener&& old) = delete;
	~NewClientListener(void);
	/* takes ownership of the fd and will close it on destruction */
	void	set_server_fd(int fd);
	void	run(void);
	void	join_thread(void); /* will wait forever if 'exit' is not set */

private:
	void	_loop(void);
	std::thread			_thread;
	t_fd				_server_fd;

	//std::vector<t_fd>&	_server_sockets;
	ClientConnections&	_connections;
};
