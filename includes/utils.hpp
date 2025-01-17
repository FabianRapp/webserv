#pragma once

#include <string>
#include <unordered_map>
#include <fcntl.h>
#include <cerrno>
#include <iostream>
#include <cstring>

extern
std::atomic<bool>	exit_;

int	set_fd_non_block(int fd);

std::string	default_err_response(int err_code);
void	init_status_codes(std::unordered_map<unsigned long, std::string> &codes);

