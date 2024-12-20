#include <Parser.hpp>

Parser::Parser(const std::string & http_input)
:	_lexer(http_input)
{}

Parser::~Parser(void) {
}

void	Parser::parse(void) {
	Token	token = _lexer.next();
	while (token.type != TokenType::EOF_TOKEN) {
		token = _lexer.next();
		/* todo: main parsing logic */
	}
}

/* to check wether the read request is complete or needs to be read further */
bool	Parser::is_finished(void) const {
	return (true); /* place holder */
}

/* once is_finished is true this can be called.
 * Resets the parser/lexer for the next client request with the same
 	connection. */
t_http_request	Parser::get_request(void) const {
	return (_request);
}
void	Parser::_reset(void) {
	_lexer.reset();
	/* todo: reset every field in _request */
}
