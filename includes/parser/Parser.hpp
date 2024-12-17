#pragma once

#include <msg.hpp>
#include <Token.hpp>
#include <Lexer.hpp>

#include <macros.h>
#include <string>
#include <iostream>


class Parser {
public:
					Parser(std::string http_input): _lexer(http_input) {}
	t_http_request	parse();
private:
	Lexer			_lexer;
};
