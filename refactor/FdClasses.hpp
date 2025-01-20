#pragma once

#include <Webserv.hpp>

#include <fcntl.h>
#include <sys/stat.h>
#include <Manager.hpp>

class DataManager;

//placeholder
class Config {
public:
	int	port;
private:
};

class BaseFd {
public:
	BaseFd(DataManager& data, short poll_events);

	virtual
	~BaseFd(void);

	bool	is_ready(short event) const;

	virtual
	void	execute(void) = 0;

	const short		poll_events;
	DataManager&	data;
	int				fd;
	size_t			data_idx;
private:
};

class Server: public BaseFd {
public:
	Server(DataManager& data, Config& config);
	~Server(void);

	void	execute(void);

	Config	config;

private:
	std::unordered_map<unsigned long, std::string>	_codes;
};

class ReadFd: public BaseFd {
public:
	ReadFd(DataManager& data, std::string& target_buffer, int fd,
			ssize_t byte_count, std::function<void()> completion_callback);

	~ReadFd(void);

	void	execute(void);

private:
	std::string&						target_buf;
	char								buffer[1024];
	ssize_t								left_over_bytes;
	std::function<void()>				completion_callback;
};



class Client: public BaseFd {
public:
	enum class ClientMode {
		RECEIVING,
		SENDING,
		READING_FILE,
		WRITING_FILE,
		READING_PIPE,
		WRITING_PIPE,
	}	mode;
	Client(DataManager& data, Server* parent_server);

	~Client(void);

	void	execute(void);

	void	parse(void);
	Server*	server;
	std::string	input;
private:
	Parser			_parser;
};
