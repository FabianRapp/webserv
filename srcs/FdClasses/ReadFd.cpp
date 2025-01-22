#include <ReadFd.hpp>
#include <Manager.hpp>

ReadFd::ReadFd(DataManager& data, std::string& target_buffer, int fd,
		ssize_t byte_count, std::function<void()> completion_callback):
	BaseFd(data, POLLIN), target_buf(target_buffer),
	completion_callback(std::move(completion_callback))
{
	left_over_bytes = byte_count;
	this->fd = fd;
}

ReadFd::~ReadFd(void) {
}

void	ReadFd::execute(void) {
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
