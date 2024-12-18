#include <utils.hpp>

/* return -1 on error, oterwise the fd */
/* todo: idk if this can fail with valid given fd's, if so how to handle that? */
int	set_fd_non_block(int fd) {
	int	old_err = errno;
	errno = 0;
	int	flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	if (errno) {
		std::cerr << "Error: fcntl: set non blocking" << strerror(errno) << '\n';
		return (-1);
	}
	errno = old_err;
	return (fd);
}
