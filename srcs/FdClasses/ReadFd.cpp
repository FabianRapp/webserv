#include <ReadFd.hpp>
#include <Manager.hpp>

ReadFd::ReadFd(DataManager& data, std::string& target_buffer, int fd,
		ssize_t byte_count, std::function<void()> completion_callback):
	BaseFd(data, POLLIN), target_buf(target_buffer),
	completion_callback(std::move(completion_callback))
{
	this->fd = fd;
	_set_non_blocking();
	left_over_bytes = byte_count;
}

ReadFd::~ReadFd(void) {
}

void	ReadFd::execute(void) {
	if (!is_ready(POLLIN)) {
		return ;
	}
	std::cout << "exec read fd\n";
	size_t	read_size = sizeof buffer - 1 < static_cast<size_t>(left_over_bytes)
		? sizeof buffer - 1: static_cast<size_t>(left_over_bytes);
	ssize_t read_ret = read(fd, buffer, read_size);
	assert(read_ret >= 0);
	buffer[read_ret] = 0;
	left_over_bytes -= read_ret;
	target_buf += buffer;
	//std::cout << "buffer: " << target_buf << "\n";
	//std::cout << "read_size: " << read_size << "\n";
	//std::cout << "read_ret: " << read_ret << "\n";
	//std::cout << "left_over_bytes: " << left_over_bytes << "\n";
	if (left_over_bytes == 0) {
		//data.set_close(data_idx);
		completion_callback();
		return ;
	}
}
