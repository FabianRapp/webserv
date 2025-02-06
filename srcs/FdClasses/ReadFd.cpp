#include "../../includes/FdClasses/ReadFd.hpp"
#include "../../includes/Manager.hpp"

ReadFd::ReadFd(DataManager& data, std::string& target_buffer, int fd, Client& client,
		ssize_t byte_count, std::function<void()> completion_callback):
	BaseFd(data, POLLIN, "ReadFd"),
	target_buf(target_buffer),
	completion_callback(std::move(completion_callback)),
	client(&client),
	server(client.server),
	debug_fd(open("read_data_debug", O_CREAT | O_TRUNC | O_WRONLY, 0777))
{
	FT_ASSERT(debug_fd >= 0);
	this->fd = fd;
	_set_non_blocking();
	left_over_bytes = byte_count;
	std::cout << byte_count << "=byte count\n";
}

ReadFd::~ReadFd(void) {
	close(debug_fd);
}

void	ReadFd::execute(void) {
	if (is_ready(POLLHUP) && !is_ready(POLLIN)) {
		std::cout << FT_ANSI_RED "readfd POLLHUP\n" FT_ANSI_RESET;
		data.set_close(data_idx);
		completion_callback();
		return ;
	}
	if (!is_ready(POLLIN)) {
		return ;
	}
	//std::cout << "exec read fd\n";
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
	buffer[read_ret] = 0;//not needed, only for debugging

	left_over_bytes -= read_ret;
	target_buf.append(buffer, static_cast<size_t>(read_ret));
	//std::cout << "buffer: " << target_buf << "\n";
	//std::cout << "read_size: " << read_size << "\n";
	//std::cout << "read_ret: " << read_ret << "\n";
	//std::cout << "left_over_bytes: " << left_over_bytes << "\n";
	if (left_over_bytes == 0 || read_ret == 0) {
		write(debug_fd, target_buf.c_str(), static_cast<size_t>(target_buf.size()));
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
