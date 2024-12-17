#pragma once

#include <Token.hpp>
#include <cstdbool>
#include <iostream>

class Lexer {
public:
						Lexer(std::string raw_http_request);
	Token				next(void);
private:
	std::string			_input;
	size_t				_pos;
						Lexer(void);
	/* Each token type should have 2 lexing function:
		1 to decide if the current position (_input[_pos]) is of that type,
		it should not change _pos.
		And 1 for extracting that string and setting _pos to right after the
		extracted string.
	 * examples for lexing a function pair prototypes:
	//would indicate a VERSION_TOKEN
	bool				isVesion(void);
	// would extract the relevant string for a VERSION_TOKEN
	std::string			extractVersion(void);
	...and other http token types
	*/
};

