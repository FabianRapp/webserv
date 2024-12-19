#include <Lexer.hpp>

Lexer::Lexer(const std::string  & raw_http_request)
: _input(raw_http_request), _pos(0)
{
	std::cout << "Called lexer\n";
}

/*todo: */
Token	Lexer::next(void) {
	/* placeholder */
	TokenType	type = TokenType::EOF_TOKEN;
	return (Token(type));
}
