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
#include "../Response.hpp"

class Client: public BaseFd {
public:
	enum class ClientMode {
		RECEIVING,
		BUILD_RESPONSE,
		SENDING,
		READING_FD,
		WRITING_FD,
		TESTING_MODE,
	}	mode;
	Client(DataManager& data, Server* parent_server);

	~Client(void);

	void	execute(void);

	void	parse(void);
	Server*	server;
	std::string	input;
private:

	void		_execute_response(bool & close_connection);
	void			_receive_request(void);
	Request			_request;
	Response*		_response;
	struct {
		std::string		body;
	}				_response_builder;

	/********************** interace for file/pipe IO ************************/
	//todo: make this a class?
	void				_write_fd(ClientMode next_mode, int fd);
	void				_read_fd(ClientMode next_mode, int fd, ssize_t byte_count);
	std::string			_fd_read_data;
	std::string_view	_fd_write_data;
	struct {
		bool				error;//check this to check if there is any error
		//todo: add data for error handling
	}					_fd_error;
	WriteFd*			_writer;
	ReadFd*				_reader;
	/*************************************************************************/

	void			_send_response(void);
	struct send_data {
		std::string	response;
		size_t		pos;
		bool		close_after_send;
	}	_send_data;
	Parser			_parser;



	void			_test_write_fd();
};
