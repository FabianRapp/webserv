#include "../../includes/FdClasses/WriteFd.hpp"
#include "../../includes/Manager.hpp"

WriteFd::WriteFd(DataManager& data, const std::string_view& src, int fd, Client& client,
		std::function<void()> completion_callback):
	BaseFd(data, POLLOUT, "Writer"),
	src(src),
	completion_callback(std::move(completion_callback)),
	pos(0),
	client(&client),
	server(client.server)
{
	std::cout << "writer constructor: src size: " << src.size() << std::endl;
	this->fd = fd;
	_set_non_blocking();
}

WriteFd::~WriteFd(void) {
}

void	WriteFd::execute(void) {
	//todo: idk about this POLLHUP for write fd
	if (is_ready(POLLHUP) && !is_ready(POLLIN)) {
		std::cout << FT_ANSI_RED "write POLLHUP\n" FT_ANSI_RESET;
		data.set_close(data_idx);
		completion_callback();
		return ;
	}
	if (!is_ready(POLLOUT)) {
		return ;
	}
	std::cout << "exec write fd\n";
	ssize_t write_ret = write(fd, src.data() + pos, src.size() - pos);
	if (write_ret < 0) {
		//todo: internal server error: 500
		std::cout << "fd: " << fd << std::endl;
		FT_ASSERT(0);
	}
	//std::cout << "written: |" << src.substr(0, static_cast<size_t>(write_ret)) << "|\n";
	pos += static_cast<size_t>(write_ret);
	if (pos == src.size() || write_ret == 0) {
		data.set_close(data_idx);
		completion_callback();
		//std::cout << "writer finished\n";
		return ;
	}
}

Client*	WriteFd::get_client(void) {
	return (client);
}

Server*	WriteFd::get_server(void) {
	return (server);
}
