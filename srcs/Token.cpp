#include <Token.hpp>

Token::Token(token_type type, const std::string & value)
	: type(type), val(value) {}

Token::~Token(void) {}

Token::Token(const Token & old): type(old.type), val(old.val) {}

Token &	Token::operator=(const Token & right) {
	if (this == &right) {
		return (*this);
	}
	this->type = right.type;
	this->val = right.val;
	return (*this);
}
