#pragma once

#include <fcntl.h>
#include <cerrno>
#include <iostream>
#include <cstring>

int	set_fd_non_block(int fd);
