#pragma once

#include <Manager.hpp>

#include <fcntl.h>
#include <sys/stat.h>


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
	void	set_close(void);

	virtual
	void	execute(void) = 0;

	const short		poll_events;
	DataManager&	data;
	int				fd;
	size_t			data_idx;
protected:
	void	_set_non_blocking(void);
private:
};

class Server: public BaseFd {
public:
	Server(DataManager& data, Config& config);
	~Server(void);

	void	execute(void);
	size_t	total_unique_clients;
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

	std::string		_build_response(t_http_request request, bool & close_connection);
	void			_receive_request(void);

	void			_send_response(void);
	struct send_data {
		std::string	response;
		size_t		pos;
		bool		close_after_send;
	}	_send_data;
	Parser			_parser;
};
