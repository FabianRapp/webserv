#include <Parser.hpp>

/* todo: */
t_http_request	Parser::parse(void) {
	t_http_request	msg;

	Token token = _lexer.next();
	while (token.type != TokenType::EOF_TOKEN) {
		switch (token.type) {
			default:
				std::cerr << "Parser Error: Unknown token type \n";
				FT_ASSERT(0);
		}
		token = _lexer.next();
	}

	return (msg);
}

