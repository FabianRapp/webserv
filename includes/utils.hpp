#pragma once

#include <string>
#include <unordered_map>
#include <fcntl.h>
#include <cerrno>
#include <iostream>
#include <cstring>
#include <csignal>
#include <unistd.h>

extern volatile sig_atomic_t	exit_;


std::string	default_err_response(int err_code);
void	init_status_codes(std::unordered_map<unsigned long, std::string> &codes);
void	ft_close(int fd);
