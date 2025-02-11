#pragma once

#include "BaseFd.hpp"
#include "../types.hpp"
#include "../ConfigParser/ServerConfigFile.hpp"

#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include <sys/poll.h>
#include <arpa/inet.h>
#include <unordered_map>
#include <CookieManager.hpp>

#ifndef REQUEST_QUE_SIZE
# define REQUEST_QUE_SIZE 10
#endif

class Server: public BaseFd {
public:
	Server(DataManager& data, std::vector<ServerConfigFile>& configs);
	~Server(void);

	void	execute(void) override;
	size_t	total_unique_clients;
	std::vector<ServerConfigFile>	configs;
	class ServerError: public std::runtime_error {
	public:
		 ServerError(const std::string& str): std::runtime_error(str) {}
	};
	bool	start_panic;
	//only set panic from manger(when detecting a server closing it sets it to panic)
	//use start_panic instead
	bool	panic;
	CookieManager	cookie_manager;
private:
	std::unordered_map<unsigned long, std::string>	_codes;
};
