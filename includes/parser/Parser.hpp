#pragma once

#include "../msg.hpp"
#include "Token.hpp"
#include "Lexer.hpp"
#include "../Request.hpp"

#include "../macros.h"
#include <string>
#include <iostream>
#include <vector>

// /* todo: parse(): main logic */
// /* assumes one parser object per client connection */
// class Parser {
// public:
// 					Parser(std::string & http_input);
// 					~Parser(void);
// 	void			parse(void);
// 	bool			is_finished(void) const; /* to check wether the read request is
// 									complete or needs to be read further */
// 	t_http_request	get_request(void) const; /* once is_finished is true this can be
// 	called. Resets the parser/lexer for the next client request with the same
// 	connection. */
// private:
// 	void			_reset(void);
// 	void			_handle_header(Token & token);
// 	t_http_request	_request;
// 	Lexer			_lexer;
// };

// class Parser {
// 	public:
// 		Parser(std::string & http_input);
// 		~Parser(void);
// 		void			parse(void);
// 		bool			is_finished(void) const; /* to check wether the read request is complete or needs to be read further */
// 		t_http_request	get_request(void) const; /* once is_finished is true this can be
// 		called. Resets the parser/lexer for the next client request with the same
// 		connection. */

// 	private:
// 		void			_reset(void);
// 		void			_handle_header(Token & token);
// 		t_http_request	_request;
// 		Lexer			_lexer;
// };

using vector = std::vector<std::string>;

class Parser {
	private:
		Request		_request;
		std::string	_request_content;

	public:
		Parser() = default;
		~Parser() = default;

		void		parse(std::string input);
		void		parse_first_line(std::string input);
		static bool	is_header_present(const std::string& str);
		std::string extract_first_line(const std::string& request);
		std::vector<std::string> split(const std::string& str, char delimiter);


		bool		ends_with(const std::string& str, const std::string& suffix);
		bool		is_finished(void) const;
		Request		get_request(void) const;
};