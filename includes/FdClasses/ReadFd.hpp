#pragma once

#include "BaseFd.hpp"
#include <algorithm>
#include  <functional>

class Response;

// appends the dat to the given target_buffer
class ReadFd: public BaseFd {
public:
	ReadFd(DataManager& data, Response& response, std::string& target_buffer, int fd, Client& client,
			ssize_t byte_count, std::function<void()> completion_callback);

	~ReadFd(void);

	void	execute(void) override;
	Client*	get_client(void);
	Server*	get_server(void);

protected:
	std::string&						target_buf;
	//char								buffer[1024];
	ssize_t								left_over_bytes;
	std::function<void()>				completion_callback;
	Client*								client;
	Server*								server;
	Response&							response;


	//debug: remove later
	int									debug_fd;
};

