#pragma once

#include <msg.hpp>
#include <Token.hpp>
#include <Lexer.hpp>

#include <macros.h>
#include <string>
#include <iostream>

/* todo: parse(): main logic */
/* assumes one parser object per client connection */
class Parser {
public:
					Parser(const std::string & http_input);
					~Parser(void);
	void			parse(void);
	bool			is_finished(void) const; /* to check wether the read request is
									complete or needs to be read further */
	t_http_request	get_request(void) const; /* once is_finished is true this can be
	called. Resets the parser/lexer for the next client request with the same
	connection. */
private:
	void			_reset(void);
	t_http_request	_request;
	Lexer			_lexer;
};
