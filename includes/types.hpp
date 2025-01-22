#pragma once
#include <cstdlib>

typedef int	t_fd;

class Config;
class DataManager;
class BaseFd;
class Server;
class Client;
class ReadFd;


struct total_config {
	size_t	server_count;
};

struct server_config {
	unsigned short	port;
};


