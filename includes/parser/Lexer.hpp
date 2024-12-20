#pragma once

#include <Token.hpp>
#include <cstdbool>
#include <iostream>
#include <cstdint>

/* Todo: implement throws for erros. */
class Lexer {
public:
						Lexer(void) = delete;
						Lexer(const std::string & raw_http_request);
						~Lexer(void);
	Token				next(bool expected_body, bool chunked_body, size_t expected_body_size);
	Token				next(void);
	void				reset(void); /* Resets the lexer for the next
										request. */
private:
	const std::string	&_input;
	size_t				_pos;
	char				_cur;

	void				_advance(void);
	void				_advance(size_t count);

	bool				_is_eof(void); /* Has no extract (when there is no
											more input. */
	bool				_is_method(void);
	Token				_extract_method(void);
	bool				_is_uri(void);
	Token				_extract_uri(void);
	bool				_is_version(void);
	Token				_extract_version(void);
	bool				_is_header(void);
	Token				_extract_header(void);
	bool				_is_header_termination(void);
	Token				_extract_header_termination(void);
	bool				_is_body(bool expected_body, size_t expected_body_size);
	Token				_extract_body(bool chunked, size_t expected_body_size);
	bool				_is_body_termination(void);
	Token				_extract_body_termination(void);
	bool				_is_unfinished(void);
	Token				_extract_unfinished(void);
};

