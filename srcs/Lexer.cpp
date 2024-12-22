#include <Lexer.hpp>

Lexer::Lexer(std::string  & raw_http_request)
:	_input(raw_http_request),
	_pos(0),
	_cur(0),
	_last_header_term(false),
	_body_size(0)
{
	std::cout << "Called lexer\n";
	if (_input.size()) {
		_cur = _input[0];
	} else {
		_cur = 0;
	}
}

Lexer::~Lexer(void) {}

Lexer::Lexer(const Lexer & old)
:	_input(old._input),
	_pos(old._pos),
	_cur(old._cur),
	_last_header_term(old._last_header_term),
	_body_size(old._body_size)
{
}

Lexer &	Lexer::operator=(const Lexer & right) {
	if (this == &right) {
		return (*this);
	}
	if (&_input != &right._input) {
		_input = right._input;
	}
	_pos = right._pos;
	_cur = right._cur;
	_last_header_term = right._last_header_term;
	_body_size = right._body_size;
	return (*this);
}

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

/* call this directtly when the body is to be expected
 * (last token was HEADER_TERMINATION) */
Token	Lexer::next(void) {
	std::cout << "called Lexer::next with input:\n"
		<< _input.substr(_pos, _input.size()) << '\n';

	/* todo: is this too stupid? xd */
	/* order might change if it makes sense idk */
	Token	token(TokenType::UNFINISHED);
	if (_is_eof()) {
		/* does not need to advance or have any value so simply returns. */
		token = Token(TokenType::EOF_TOKEN);
	} else if (_is_body()) {
		token = _extract_body();
	} else if (_is_method()) {
		token = _extract_method();
	} else if (_is_uri()) {
		token = _extract_uri();
	} else if (_is_version()) {
		token = _extract_version();
	} else if (_is_header()) {
		token = _extract_header();
	} else if (_is_header_termination()) {
		token = _extract_header_termination();
	} else if (_is_body_termination()) {
		token = _extract_body_termination();
	}

	return (token);
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
	
	/* todo:
	 * for the CONTENT_LENGTH header the length has to be stored in _body_len */
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
	_last_header_term = true;
	return (Token(TokenType::HEADER_TERMINATION));
}

/* assumes that the token before was the header terimination
	(state should be passed from parser into lexer to handle this context). */
bool	Lexer::_is_body(void) {
	if (!_last_header_term) {
		return (false);
	}
	if (_input.size() - _pos < _body_size) {
		return (false); /* wait for more input */
	}
	_last_header_term = false;
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
Token	Lexer::_extract_body(void) {
	FT_ASSERT(_input.size() >= _pos + _body_size);
	std::string	body_content = _input.substr(_pos, _body_size);
	_advance(_body_size);
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
