#include "../../includes/FdClasses/BaseFd.hpp"
#include "../../includes/Manager.hpp"

BaseFd::BaseFd(DataManager& data, short poll_events, const std::string& name):
	poll_events(poll_events),
	data(data),
	fd(-1),
	data_idx(std::numeric_limits<size_t>::max()),
	name(name)
{
}

BaseFd::~BaseFd(void) {
	//data.set_close(data_idx);
}

//be very carefull with calling this outside of the object itself
void	BaseFd::set_close(void) {
	data.set_close(data_idx);
}

bool	BaseFd::is_ready(short event) const {
	return (!data.closing(data_idx) && data.is_ready(data_idx, event));
}

void	BaseFd::_set_non_blocking(void) {
	int	old_err = errno;
	errno = 0;
	int	flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	if (errno) {
		std::cerr << "Error: fcntl: set non blocking: " << strerror(errno) << '\n';
		set_close();
	}
	errno = old_err;
}
