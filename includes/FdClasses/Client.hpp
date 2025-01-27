#pragma once

#include "../general_includes.hpp"
#include "BaseFd.hpp"
#include "WriteFd.hpp"
#include "../msg.hpp"
#include "../parser/Parser.hpp"
#include <algorithm>
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
		WRITING_FD,
		READING_PIPE,
		TESTING_MODE,
	}	mode;
	Client(DataManager& data, Server* parent_server);

	~Client(void);

	void	execute(void);

	void	parse(void);
	Server*	server;
	std::string	input;
private:

	std::string		_execute_response(bool & close_connection);
	void			_receive_request(void);
	Request			_request;
	struct {
		std::string		body;
		ReadFd			*reader;
	}				_response_builder;
	WriteFd			*_writer;

	/* interace for file/pipe IO */
	void				_write_fd(ClientMode next_mode, int fd);
	std::string			_fd_read_data;
	std::string_view	_fd_write_data;

	void			_send_response(void);
	struct send_data {
		std::string	response;
		size_t		pos;
		bool		close_after_send;
	}	_send_data;
	Parser			_parser;



	void			_test_write_fd();
};
