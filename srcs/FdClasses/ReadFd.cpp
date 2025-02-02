#include "../../includes/FdClasses/ReadFd.hpp"
#include "../../includes/Manager.hpp"

ReadFd::ReadFd(DataManager& data, std::string& target_buffer, int fd, bool close_fd, Client& client,
		ssize_t byte_count, std::function<void()> completion_callback):
	BaseFd(data, POLLIN, "ReadFd"),
	target_buf(target_buffer),
	completion_callback(std::move(completion_callback)),
	client(&client),
	server(client.server)
{
	if (close_fd) {
		this->fd = fd;
	} else {
		this->fd = dup(fd);
		// todo: verify fd > 0
	}
	_set_non_blocking();
	left_over_bytes = byte_count;
	std::cout << byte_count << "=byte count\n";
}

ReadFd::~ReadFd(void) {
}

void	ReadFd::execute(void) {
	if (!is_ready(POLLIN)) {
		return ;
	}
	std::cout << "exec read fd\n";
	size_t	read_size;
	if (left_over_bytes > 0 ) {
		read_size = sizeof buffer - 1 < static_cast<size_t>(left_over_bytes)
		? sizeof buffer - 1: static_cast<size_t>(left_over_bytes);
	} else {
		read_size = sizeof buffer;
	}
	ssize_t read_ret = read(fd, buffer, read_size);
	if (read_ret < 0) {
		//todo: err
		FT_ASSERT(0);
	}
	buffer[read_ret] = 0;

	left_over_bytes -= read_ret;
	target_buf += buffer;
	//std::cout << "buffer: " << target_buf << "\n";
	//std::cout << "read_size: " << read_size << "\n";
	//std::cout << "read_ret: " << read_ret << "\n";
	//std::cout << "left_over_bytes: " << left_over_bytes << "\n";
	if (left_over_bytes == 0 || read_ret == 0) {
		data.set_close(data_idx);
		completion_callback();
		return ;
	}
}

Client*	ReadFd::get_client(void) {
	return (client);
}

Server*	ReadFd::get_server(void) {
	return (server);
}
