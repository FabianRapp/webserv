#pragma once

#include <Parser.hpp>
#include <types.hpp>
#include <unistd.h>

class ClientConnection {
public:
					ClientConnection(const t_fd & fd);
					~ClientConnection(void);
	const t_fd		&fd;
	std::string		input;
	void			parse(void);
	bool			completed_request(void);
	/* only call this if completed_request returned true */
	t_http_request	get_request(void) const;

private:
	Parser			_parser;
};
