#include <Parser.hpp>

/* todo: */
t_http_request	Parser::parse(void) {
	t_http_request	msg;

	Token token = _lexer.next();
	while (token.type != EOF_TOKEN) {
		switch (token.type) {
			default:
				std::cerr << "Unknown tyoken type: " << token.type << '\n';
				FT_ASSERT(0);
		}
	}

	return (msg);
}

