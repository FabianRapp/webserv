#include "../../includes/FdClasses/ReadFd.hpp"
#include "../../includes/Manager.hpp"
#include <Response.hpp>

//todo: remove debug fd

ReadFd::ReadFd(DataManager& data, Response& response, std::string& target_buffer, int fd, Client& client,
		ssize_t byte_count, std::function<void()> completion_callback):
	BaseFd(data, POLLIN, "ReadFd"),
	target_buf(target_buffer),
	completion_callback(std::move(completion_callback)),
	client(&client),
	server(client.server),
	response(response),
	debug_fd(open("read_data_debug", O_CREAT | O_TRUNC | O_WRONLY, 0777))
{
	FT_ASSERT(debug_fd >= 0);
	this->fd = fd;
	_set_non_blocking();
	left_over_bytes = byte_count;
}

ReadFd::~ReadFd(void) {
	close(debug_fd);
}

void	ReadFd::execute(void) {
	static
	char	buffer[1024 * 1024 * 100]; // 100mb read buffer

	if (is_ready(POLLHUP) && !is_ready(POLLIN)) {
		LOG_FABIAN(FT_ANSI_RED "readfd POLLHUP\n" FT_ANSI_RESET);
		data.set_close(data_idx);
		completion_callback();
		return ;
	}
	if (!is_ready(POLLIN)) {
		return ;
	}
	LOG_FABIAN3("exec read fd\n");
	size_t	read_size;
	if (left_over_bytes > 0 ) {
		read_size = sizeof buffer < static_cast<size_t>(left_over_bytes)
		? sizeof buffer : static_cast<size_t>(left_over_bytes);
	} else {
		read_size = sizeof buffer;
	}
	ssize_t read_ret = read(fd, buffer, read_size);
	if (read_ret < 0) {
		data.set_close(data_idx);
		completion_callback();
		response.load_status_code_response(500, "Internal Server Error");
		return ;
	}
	if (read_ret) {
		size_t	max_loged_chars = 10;
		std::string	log_chars(buffer, std::min(static_cast<size_t>(read_ret), max_loged_chars));
		LOG(FT_ANSI_BLUE "Read by " << name << ": " << log_chars << "..\n" FT_ANSI_RESET);
	}
	left_over_bytes -= read_ret;
	target_buf.append(buffer, static_cast<size_t>(read_ret));
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
