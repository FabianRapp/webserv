#pragma once

#include "../general_includes.hpp"
#include "BaseFd.hpp"
#include "WriteFd.hpp"
#include "../parser/Parser.hpp"
#include <algorithm>
#include <sys/socket.h>
#include <poll.h>
#include <sys/poll.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <chrono>

#include "../../includes/ConfigParser/ServerConfigFile.hpp"
#include "../Response.hpp"

class Response;

enum class ClientMode {
	RECEIVING,
	BUILD_RESPONSE,
	SENDING,
	READING_FD,
	WRITING_FD,
};

class Client: public BaseFd {
public:
	Client(DataManager& data, Server* parent_server);

	~Client(void);

	void	execute(void) override;

	void	parse(void);
	ClientMode&	get_mode(void);

	void	set_close(void) override;

	Server*	server;
	std::string	input;
private:

	void			_receive_request(void);

	ServerConfigFile&	_select_config(
		std::vector<ServerConfigFile>& server_configs, Request& request);
	Request			_request;
	Response*		_response;
	ClientMode		mode;

	void			_send_response(void);
	struct send_data {
		std::string	response;
		size_t		pos;
	}	_send_data;
	Parser			_parser;

	std::chrono::steady_clock::time_point
					_last_availability;
};
