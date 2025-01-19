#pragma once

#include <Webserv.hpp>

#include <fcntl.h>
#include <sys/stat.h>
#include "Manager.hpp"

class DataManager;

//placeholder
class Config {
public:
	int	port;
private:
};

class BaseFd {
public:
	BaseFd(DataManager& data, short poll_events):
		poll_events(poll_events),
		data(data),
		fd(-1),
		data_idx(std::numeric_limits<size_t>::max())
	{
	}

	virtual
	~BaseFd(void) {
		data.set_close(data_idx);
	}

	bool	is_ready(short event) {
		return (data.is_ready(data_idx, event));
	}

	virtual
	void	execute(void) = 0;

	const short		poll_events;
	DataManager&	data;
	int				fd;
	size_t			data_idx;
private:
};

class Server: public BaseFd {
public:
	Server(DataManager& data, Config& config):
		BaseFd(data, POLLIN),
		config(config)
	{
		struct sockaddr_in		server_addr;
		const socklen_t			server_addr_len = static_cast<socklen_t>(sizeof server_addr);
		struct sockaddr	*const server_addr_ptr = reinterpret_cast<struct sockaddr *>(&server_addr);

		/* AF_INET : ipv4
		 * AF_INET6: ipv6 */
		init_status_codes(_codes);
		int server_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (server_fd < 0) {
			std::cerr << "Error: server: socket: " << strerror(errno) << '\n';
			exit(errno);
		}
		server_fd = set_fd_non_block(server_fd);
		if (server_fd < 0) {
			exit(errno);
		}

		memset(&server_addr, 0, sizeof(struct sockaddr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = INADDR_ANY;
		server_addr.sin_port = htons(config.port);

		if (bind(server_fd, server_addr_ptr, server_addr_len) < 0) {
			close(server_fd);
			std::cerr << "Error: " << strerror(errno) << '\n';
			exit(errno);
		}
		if (listen(server_fd, REQUEST_QUE_SIZE) < 0) {
			close(server_fd);
			std::cerr << "Error: " << strerror(errno) << '\n';
			exit(errno);
		}
		//_listener.set_server_fd(server_fd);
		std::cout << "Started server on port " << config.port << "...\n";
	}

	~Server(void) {
		//set all clients to close
		//don't call 'close()' here
	}

	void	execute(void) {
		/* to avoid pointer casts every where */
		// accept new clients
	}

	Config	config;

private:
	std::unordered_map<unsigned long, std::string>	_codes;
};

class ReadFd: public BaseFd {
public:
	ReadFd(DataManager& data, std::string& target_buffer, int fd,
			ssize_t byte_count, std::function<void()> completion_callback):
		BaseFd(data, POLLIN), target_buf(target_buffer),
		completion_callback(std::move(completion_callback))
	{
		left_over_bytes = byte_count;
		this->fd = fd;
	}

	~ReadFd(void) {
	}

	void	execute(void) {
		if (!is_ready(POLLIN)) {
			return ;
		}
		size_t	read_size = sizeof buffer - 1 < static_cast<size_t>(left_over_bytes)
			? sizeof buffer - 1: static_cast<size_t>(left_over_bytes);
		ssize_t read_ret = read(fd, buffer, read_size);
		assert(read_ret >= 0);
		buffer[read_ret] = 0;
		left_over_bytes -= read_ret;
		target_buf += buffer;
		if (left_over_bytes == 0) {
			data.set_close(data_idx);
			completion_callback();
			return ;
		}
	}

private:
	std::string&						target_buf;
	char								buffer[1024];
	ssize_t								left_over_bytes;
	std::function<void()>				completion_callback;
};

class Client: public BaseFd {
public:
	Client(DataManager& data, Server* parent_server):
		BaseFd(data, POLLIN | POLLOUT)
	{
		this->server = parent_server;
		assert(server->is_ready(POLLIN));
		errno = 0;
		struct sockaddr_in		addr;
		socklen_t				addr_len = static_cast<socklen_t>(sizeof addr);
		struct sockaddr			*const addr_ptr =
			reinterpret_cast<struct sockaddr *>(&addr);
		memset(&addr, 0, sizeof addr);// might not be needed
		fd = accept(server->fd, addr_ptr, &addr_len);
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			FT_ASSERT(0 && "Should have been handled by poll");
		}
		//fd = set_fd_non_block(fd);
		//if (new_client_fd < 0) {
		//	FT_ASSERT(0);
		//}
		//_connections.add_client(new_client_fd);
		std::cout << "Connection accepted from address("
			<< inet_ntoa(addr.sin_addr) << "): PORT("
			<< ntohs(addr.sin_port) << ")\n";
	}

	~Client(void) {
	}

	void	execute(void) {
	}

	Server*	server;
private:
};
