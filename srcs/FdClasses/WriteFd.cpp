#include "../../includes/FdClasses/WriteFd.hpp"
#include "../../includes/Manager.hpp"

WriteFd::WriteFd(DataManager& data, std::string_view& src, int fd,
		std::function<void()> completion_callback):
	BaseFd(data, POLLOUT), src(src),
	completion_callback(std::move(completion_callback)),
	pos(0)
{
	this->fd = fd;
	_set_non_blocking();
}

WriteFd::~WriteFd(void) {
}

void	WriteFd::execute(void) {
	if (!is_ready(POLLOUT)) {
		return ;
	}
	std::cout << "exec write fd\n";
	ssize_t write_ret = write(fd, src.data() + pos, src.size() - pos);
	assert(write_ret >= 0);
	pos += static_cast<size_t>(write_ret);
	if (pos == src.size()) {
		//data.set_close(data_idx);
		completion_callback();
		return ;
	}
}
