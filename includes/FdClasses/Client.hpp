#pragma once

#include <general_includes.hpp>
#include <BaseFd.hpp>
#include <msg.hpp>
#include <Parser.hpp>

#include <sys/socket.h>
#include <poll.h>
#include <sys/poll.h>
#include <sys/stat.h>

class Client: public BaseFd {
public:
	enum class ClientMode {
		RECEIVING,
		BUILD_RESPONSE,
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
	t_http_request	_request;
	struct {
		std::string		body;
		ReadFd			*reader;
	}				_response_builder;

	void			_send_response(void);
	struct send_data {
		std::string	response;
		size_t		pos;
		bool		close_after_send;
	}	_send_data;
	Parser			_parser;
};
