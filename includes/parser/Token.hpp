#pragma once

#include <string>

/* todo */
typedef enum token_type {
	VERSION_TOKEN,
	BODY_TOKEN,
	EOF_TOKEN,
}	t_token_type;

class Token {
public:
	token_type	type;
	std::string	val;
				Token(void) = delete;
				~Token(void);
				Token(token_type type, const std::string & value);
				Token(const Token & old);
	Token &		operator=(const Token & right);
};
