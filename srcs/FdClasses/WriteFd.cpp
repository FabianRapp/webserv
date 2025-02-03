#include "../../includes/FdClasses/WriteFd.hpp"
#include "../../includes/Manager.hpp"

WriteFd::WriteFd(DataManager& data, const std::string_view& src, int fd, bool close_fd, Client& client,
		std::function<void()> completion_callback):
	BaseFd(data, POLLOUT, "Writer"),
	src(src),
	completion_callback(std::move(completion_callback)),
	pos(0),
	client(&client),
	server(client.server)
{
	if (close_fd) {
		this->fd = fd;
	} else {
		this->fd = dup(fd);
		//todo: verify fd >0
	}
	_set_non_blocking();
}

WriteFd::~WriteFd(void) {
}

void	WriteFd::execute(void) {
	//todo: check if there is a poll value for closed pipes
	if (!is_ready(POLLOUT)) {
		return ;
	}
	std::cout << "exec write fd\n";
	ssize_t write_ret = write(fd, src.data() + pos, src.size() - pos);
	if (write_ret < 0) {
		//todo: internal server error: 500
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
