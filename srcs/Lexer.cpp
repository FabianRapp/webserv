#include <Lexer.hpp>

Lexer::Lexer(const std::string  & raw_http_request)
: _input(raw_http_request), _pos(0)
{
	std::cout << "Called lexer\n";
}

/*todo: */
Token	Lexer::next(void) {
	return (Token(EOF_TOKEN, ""));
}
