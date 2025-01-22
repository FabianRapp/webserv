#pragma once

#include <BaseFd.hpp>
#include <types.hpp>
#include <ServerConfigFile.hpp>

#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include <sys/poll.h>
#include <arpa/inet.h>
#include <unordered_map>

#ifndef REQUEST_QUE_SIZE
# define REQUEST_QUE_SIZE 10
#endif


class Server: public BaseFd {
public:
	Server(DataManager& data, ServerConfigFile& config);
	~Server(void);

	void	execute(void);
	size_t	total_unique_clients;
	ServerConfigFile	config;
private:
	std::unordered_map<unsigned long, std::string>	_codes;
};
