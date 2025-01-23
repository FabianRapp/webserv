#include "../includes/parser/Lexer.hpp"

Lexer::Lexer(std::string  & raw_http_request)
:	_input(raw_http_request),
	_pos(0),
	_cur(0),
	_last_header_term(false),
	_body_size(0)
{
	std::cout << "Lexer constructor\n";
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
	_body_size = 0;
}

Token	Lexer::next(void) {
	std::cout << "Lexer: trying to match \"" << _input.substr(_pos, 10) << "...\":\n";

	Token	token(TokenType::UNFINISHED);
	if (_is_eof()) { /* first check */
		/* does not need to advance or have any value so simply returns. */
		token = Token(TokenType::EOF_TOKEN);
	} else if (_is_body()) { /* second check */
		token = _extract_body();
	} else if (_last_header_term) {
		/* body is not complete, waiting for more data,
		 * reutrn UNFINISHED token */
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
	/* todo: check if the input can not possible be any valid field even when
	 * more input is read (to prevent reading forever and waiting for the input
	 * to for example match a header field when it should be an error */
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
	/* placeholder until lexer does more stuff */
	//return (true);
	return (false);
}

std::pair<std::string, MethodType>	Lexer::_match_method(void) {
	/* strcmp to all methods */
	/* Termination: a sinlge SP (0x20) */
	const std::pair<std::string, MethodType>	patterns[] = {
		{"OPTIONS\x20", MethodType::OPTIONS},
		{"GET\x20", MethodType::GET},
		{"HEAD\x20", MethodType::HEAD},
		{"POST\x20", MethodType::POST},
		{"PUT\x20", MethodType::PUT},
		{"DELETE\x20", MethodType::DELETE},
		{"TRACE\x20", MethodType::TRACE},
		{"CONNECT\x20", MethodType::CONNECT},
	};

	for (auto & pattern : patterns) {
		if (0 == _input.compare(_pos, pattern.first.size(), pattern.first)) {
			return (pattern);
		}
	}
	return (std::pair<std::string, MethodType>({"", MethodType::INVALID}));
}

/* todo: Placeholder */
bool	Lexer::_is_method(void) {
	if (_match_method().second == MethodType::INVALID) {
		return (false);
	}
	return (true);
}

/* todo: Placeholder */
Token	Lexer::_extract_method(void) {
	std::pair<std::string, MethodType>	method = _match_method();
	FT_ASSERT(method.second != MethodType::INVALID);
	_advance(method.first.size());
	return (Token(TokenType::METHOD, method.second));
}

//std::pair<std::string, std::string>	Lexer::_match_uri(void) {
//	/* Termination: a sinlge SP (0x20) */
//	const static std::regex	patterns[] = {
//		std::regex("*\x20"),
//	};
//	for (auto & pattern : patterns) {
//		std::smatch	match;
//		if (std::regex_search(_input.begin() + static_cast<long>(_pos),
//			_input.end(), pattern)) {
//		}
//	}
//}

/* todo: Placeholder */
/*  Request-URI    = "*" | absoluteURI | abs_path | authority */
bool	Lexer::_is_uri(void) {


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

	/* todo: keep in mind:
	 * for the CONTENT_LENGTH header the length has to be stored in _body_len
	 * for what ever header determines chunked encoding the _body_len has to
	 *		be set to smth, tho idk what yet.
	 */
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

void	Lexer::set_body_size(size_t size) {
	_body_size = size;
}
