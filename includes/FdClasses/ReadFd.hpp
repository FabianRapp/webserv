#pragma once

#include "BaseFd.hpp"

// appends the dat to the given target_buffer
class ReadFd: public BaseFd {
public:
	ReadFd(DataManager& data, std::string& target_buffer, int fd, bool close_fd,
			ssize_t byte_count, std::function<void()> completion_callback);

	~ReadFd(void);

	void	execute(void);

protected:
	std::string&						target_buf;
	char								buffer[1024];
	ssize_t								left_over_bytes;
	std::function<void()>				completion_callback;
};

