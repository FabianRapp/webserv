#pragma once

#include "Token.hpp"
#include <cstdbool>
#include <iostream>
#include <cstdint>
#include <regex>

/* Todo: implement throws for erros. */
class Lexer {
public:
						Lexer(void) = delete;
						Lexer(std::string & raw_http_request);
						Lexer(const Lexer & old);
						Lexer &	operator=(const Lexer & right);
						~Lexer(void);
	Token				next(void);
	void				reset(void); /* Resets the lexer for the next
										request. */

	void	set_body_size(size_t size);
private:
	std::string			&_input;
	size_t				_pos;
	char				_cur;
	bool				_last_header_term;
	size_t				_body_size;

	void				_advance(void);
	void				_advance(size_t count);

	bool				_is_eof(void); /* Has no extract (when there is no
											more input. */
	std::pair<std::string, MethodType>
						_match_method(void);
	bool				_is_method(void);
	Token				_extract_method(void);
	std::pair<std::string, std::string>
						_match_uri(void);
	bool				_is_uri(void);
	Token				_extract_uri(void);
	bool				_is_version(void);
	Token				_extract_version(void);
	bool				_is_header(void);
	Token				_extract_header(void);
	bool				_is_header_termination(void);
	Token				_extract_header_termination(void);
	bool				_is_body(void);
	Token				_extract_body(void);
	bool				_is_body_termination(void);
	Token				_extract_body_termination(void);
};

