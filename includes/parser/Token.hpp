#pragma once

#include <enums.hpp>
#include <macros.h>

#include <string>
#include <variant>


class Token {
public:
	TokenType	type;

	std::variant<
		std::monostate, /* EOF_TOKEN, BODY_TERMINATION, HEADER_TERMINATION */
		MethodType, /* METHOD */
		std::string, /* VERSION, URI, BODY */
		std::pair<HeaderType, std::string> /* HEADER_TOKEN */
	>			value;

	Token(
		TokenType type,
		std::variant<
			std::monostate,
			MethodType,
			std::string,
			std::pair<HeaderType, std::string>
		> value = {}
	);
	Token(const Token &old);
	Token &	operator=(const Token & right);
	MethodType &							get_method(void);
	std::pair<HeaderType, std::string> &	get_header(void);
	void									get_no_value(void);
	std::string &							get_string(void);
	std::string								full_string(void) const;

private:
};

