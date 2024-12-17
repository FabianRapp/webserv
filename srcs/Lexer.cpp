#include <Lexer.hpp>

Lexer::Lexer(std::string raw_http_request)
: _input(raw_http_request), _pos(0)
{
	std::cout << "Called lexer with input: " << raw_http_request << '\n';
}

/*todo: */
Token	Lexer::next(void) {
	return (Token(EOF_TOKEN, ""));
}
