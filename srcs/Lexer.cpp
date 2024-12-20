#include <Lexer.hpp>

Lexer::Lexer(const std::string  & raw_http_request)
:	_input(raw_http_request),
	_pos(0),
	_cur(0)
{
	std::cout << "Called lexer\n";
	if (_input.size()) {
		_cur = _input[0];
	} else {
		_cur = 0;
	}
}

Lexer::~Lexer(void) {}


void	Lexer::_advance(size_t count) {
	_pos += count;
	FT_ASSERT(_pos <= _input.size());
	if (_pos < _input.size()) {
		_cur = _input[_pos];
	} else {
		_cur = 0;
	}
}

void	Lexer::_advance(void) {
	_advance(1);
}

/* todo: */
/* For when a request is finsihed and the connection is held for more
	requests. */
void	Lexer::reset(void) {
}

/* only call this if the last token was not HEADER_TERMINATION */
Token	Lexer::next(void) {
	return (next(false, false, 0));
}

/* call this directtly when the body is to be expected
 * (last token was HEADER_TERMINATION) */
Token	Lexer::next(bool expected_body, bool chunked_body, size_t expected_body_size) {
	std::cout << "called Lexer::next with input:\n"
		<< _input.substr(_pos, _input.size()) << '\n';
	/* todo: is this too stupid? xd */

	/* order might change if it makes sense idk */
	if (_is_eof()) {
		/* does not need to advance or have any value so simply returns. */
		return (Token(TokenType::EOF_TOKEN));
	} else if (_is_body(expected_body, expected_body_size)) {
		return (_extract_body(chunked_body, expected_body_size));
	} else if (_is_method()) {
		return (_extract_method());
	} else if (_is_uri()) {
		return (_extract_uri());
	} else if (_is_version()) {
		return (_extract_version());
	} else if (_is_header()) {
		return (_extract_header());
	} else if (_is_header_termination()) {
		return (_extract_header_termination());
	} else if (_is_body_termination()) {
		return (_extract_body_termination());
	} else if (_is_unfinished()) {
		return (_extract_unfinished());
	}

	/* Placeholder,
		usually this would indicate an error when all is implemented */
	return (Token(TokenType::EOF_TOKEN));
}

/*****************************************************************************/
/* actual lexing functions: (some rough ideas what they are supposed to do
 * in some comments, tho when implementing it should be double checked
 * with the docs */

bool	Lexer::_is_eof(void) {
	if (_pos == _input.size()) {
		return (true);
	}
	return (false);
}

/* todo: Placeholder */
bool	Lexer::_is_method(void) {
	
	/* strcmp to all methods */
	/* Termination: a sinlge SP (0x20) */
	return (false);
}

/* todo: Placeholder */
Token	Lexer::_extract_method(void) {
	MethodType	placeholder_method_type = MethodType::GET;
	return (Token(TokenType::METHOD, placeholder_method_type));
}

/* todo: Placeholder */
bool	Lexer::_is_uri(void) {

	/* Termination: a sinlge SP (0x20) */
	return (false);
}

/* todo: Placeholder */
Token	Lexer::_extract_uri(void) {
	std::string	placeholder_uri = "placeholder uri";
	return (Token(TokenType::URI, placeholder_uri));
}

/* todo: Placeholder */
bool	Lexer::_is_version(void) {
	/* Termination: "\r\n" */
	return (false);
}

/* todo: Placeholder */
Token	Lexer::_extract_version(void) {

	std::string	placeholder_version = "placeholder version";
	return (Token(TokenType::VERSION, placeholder_version));
}

/* todo: Placeholder */
bool	Lexer::_is_header(void) {
	/* something like (not sure need to ckeck docs) */
	/* <key: prefix>:<string content> "\r\n" */
	return (false);
}

/* todo: Placeholder */
Token	Lexer::_extract_header(void) {
	std::pair<HeaderType, std::string>	placeholder_header;
	placeholder_header.first = HeaderType::ACCEPT;
	placeholder_header.second = "placeholder header content";

	/* advance past the line termination "\r\n" */
	return (Token(TokenType::HEADER, placeholder_header));
}

bool	Lexer::_is_header_termination(void) {
	if (_input.compare(_pos, 4, "\r\n\r\n")) {
		return (false);
	}
	return (true);
}

Token	Lexer::_extract_header_termination(void) {
	_advance(4);
	return (Token(TokenType::HEADER_TERMINATION));
}

/* assumes that the token before was the header terimination
	(state should be passed from parser into lexer to handle this context). */
bool	Lexer::_is_body(bool expected_body, size_t expected_body_size) {
	if (!expected_body) {
		return (false);
	}
	if (_input.size() - _pos < expected_body_size) {
		return (false); /* wait for more input */
	}
	return (true);
}

/* might change: i don't fully understand the diffrent situations of:
 * 1. a body length header existing && no chunked header
 * 2. no body length header && chunkded header
 * 3. both
 * 4. none with no body
 * 5. none with a body
 * This is mostly a problem of the point of reading from the socket with
 * some context from the paser/lexer. IDK YET
 */
Token	Lexer::_extract_body(bool chunked, size_t expected_body_size) {
	(void)chunked; /* todo */
	FT_ASSERT(_input.size() >= _pos + expected_body_size);
	std::string	body_content = _input.substr(_pos, expected_body_size);
	_advance(expected_body_size);
	return (Token(TokenType::BODY, body_content));
}

/* todo: Placeholder */
bool	Lexer::_is_body_termination(void) {
	const char	*body_termination = "0\r\n\r\n";
	(void) body_termination;
	return (false);
}

Token	Lexer::_extract_body_termination(void) {
	const uint8_t	body_termination_size = 5;
	
	/* todo: some throw if _input.size() > _pos + body_termination_size to
		indicate some kinds of error? */
	FT_ASSERT(_input.size() >= _pos + body_termination_size);
	_advance(body_termination_size);
	return (Token(TokenType::BODY_TERMINATION));
}

/* todo: Placeholder */
bool	Lexer::_is_unfinished(void) {
	return (false);
}

/* todo: Placeholder */
Token	Lexer::_extract_unfinished(void) {
	return (Token(TokenType::UNFINISHED));
}
