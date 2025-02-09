#include "../../includes/FdClasses/WriteFd.hpp"
#include "../../includes/Manager.hpp"
#include <Response.hpp>

WriteFd::WriteFd(DataManager& data, Response& response, const std::string_view& src, int fd, Client& client,
		std::function<void()> completion_callback):
	BaseFd(data, POLLOUT, "WriteFd"),
	src(src),
	completion_callback(std::move(completion_callback)),
	pos(0),
	client(&client),
	server(client.server),
	response(response)
{
	std::cout << "writer constructor: src size: " << src.size() << std::endl;
	this->fd = fd;
	_set_non_blocking();
}

WriteFd::~WriteFd(void) {
}

void	WriteFd::execute(void) {
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
		std::cerr << FT_ANSI_RED "Error: fd: " << name
			<< ": write failed\n" FT_ANSI_RESET;
		data.set_close(data_idx);
		completion_callback();
		response.load_status_code_response(500, "Internal Server Error");
		return ;
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
