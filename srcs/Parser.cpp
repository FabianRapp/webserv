#include <Parser.hpp>

Parser::Parser(std::string & http_input)
:	_lexer(http_input)
{}

Parser::~Parser(void) {
}

void	Parser::_handle_header(Token & token) {
	std::pair<HeaderType, std::string>	header = token.get_header();
	switch (header.first) {
		case (HeaderType::ACCEPT): {
			/* todo: */
			break ;
		}
		case (HeaderType::ACCEPT_CHARSET): {
			/* todo: */
			break ;
		}
		case (HeaderType::ACCEPT_ENCODING): {
			/* todo: */
			break ;
		}
		case (HeaderType::ACCEPT_LANGUAGE): {
			/* todo: */
			break ;
		}
		case (HeaderType::AUTHORIZATION): {
			/* todo: */
			break ;
		}
		case (HeaderType::EXPECT): {
			/* todo: */
			break ;
		}
		case (HeaderType::FROM): {
			/* todo: */
			break ;
		}
		case (HeaderType::HOST): {
			/* todo: */
			break ;
		}
		case (HeaderType::IF_MATCH): {
			/* todo: */
			break ;
		}
		case (HeaderType::IF_MODIFIED_SINCE): {
			/* todo: */
			break ;
		}
		case (HeaderType::IF_NONE_MATCH): {
			/* todo: */
			break ;
		}
		case (HeaderType::IF_RANGE): {
			/* todo: */
			break ;
		}
		case (HeaderType::IF_UNMODIFIED_SINCE): {
			/* todo: */
			break ;
		}
		case (HeaderType::MAX_FORWARDS): {
			/* todo: */
			break ;
		}
		case (HeaderType::PROXY_AUTHORIZATION): {
			/* todo: */
			break ;
		}
		case (HeaderType::RANGE): {
			/* todo: */
			break ;
		}
		case (HeaderType::REFERER): {
			/* todo: */
			break ;
		}
		case (HeaderType::TE): {
			/* todo: */
			break ;
		}
		case (HeaderType::USER_AGENT): {
			/* todo: */
			break ;
		}
		case (HeaderType::CACHE_CONTROL): {
			/* todo: */
			break ;
		}
		case (HeaderType::CONNECTION): {
			/* todo: */
			break ;
		}
		case (HeaderType::DATE): {
			/* todo: */
			break ;
		}
		case (HeaderType::PRAGMA): {
			/* todo: */
			break ;
		}
		case (HeaderType::TRAILER): {
			/* todo: */
			break ;
		}
		case (HeaderType::TRANSFER_ENCODING): {
			/* todo: */
			break ;
		}
		case (HeaderType::UPGRADE): {
			/* todo: */
			break ;
		}
		case (HeaderType::VIA): {
			/* todo: */
			break ;
		}
		case (HeaderType::WARNING): {
			/* todo: */
			break ;
		}
		case (HeaderType::ALLOW): {
			/* todo: */
			break ;
		}
		case (HeaderType::CONTENT_ENCODING): {
			/* todo: */
			break ;
		}
		case (HeaderType::CONTENT_LANGUAGE): {
			/* todo: */
			break ;
		}
		case (HeaderType::CONTENT_LENGTH): {
			/* todo: verify no illegel char that atoi does not throw for */
			try {
				_request.body_len = static_cast<size_t>(
					std::stoi(header.second));
			} catch (const std::invalid_argument & e) {
				/* todo: */
			} catch (const std::out_of_range & e) {
				/* todo: */
			}
			break ;
		}
		case (HeaderType::CONTENT_LOCATION): {
			/* todo: */
			break ;
		}
		case (HeaderType::CONTENT_MD5): {
			/* todo: */
			break ;
		}
		case (HeaderType::CONTENT_RANGE): {
			/* todo: */
			break ;
		}
		case (HeaderType::CONTENT_TYPE): {
			/* todo: */
			break ;
		}
		case (HeaderType::EXPIRES): {
			/* todo: */
			break ;
		}
		case (HeaderType::LAST_MODIFIED): {
			/* todo: */
			break ;
		}
		case (HeaderType::COOKIE): {
			/* todo: */
			break ;
		}
		case (HeaderType::SET_COOKIE): {
			/* todo: */
			break ;
		}
		case (HeaderType::DNT): {
			/* todo: */
			break ;
		}
		case (HeaderType::ORIGIN): {
			/* todo: */
			break ;
		}
		case (HeaderType::UPGRADE_INSECURE_REQUESTS): {
			/* todo: */
			break ;
		}
		case (HeaderType::ACCEPT_DATETIME): {
			/* todo: */
			break ;
		}
		case (HeaderType::EARLY_DATA): {
			/* todo: */
			break ;
		}
	}
}

void	Parser::parse(void) {
	while (1) {
		Token token = _lexer.next();
		std::cout << "got token: " << token.full_string() << "\n";
		if (_request.finished && token.type != TokenType::EOF_TOKEN) {
			FT_ASSERT(0
				&& "Likly piplined request miss handling");
		}
		switch (token.type) {
			case (TokenType::METHOD): {
				_request.type = token.get_method();
				break ;
			}
			case (TokenType::URI): {
				_request.uri = s_uri(token.get_string());
				break ;
			}
			case (TokenType::VERSION): {
				_request.version = s_version(token.get_string());
				break ;
			}
			case (TokenType::HEADER): {
				_handle_header(token);
				break ;
			}
			case (TokenType::HEADER_TERMINATION): {
				break ;
			}
			case (TokenType::BODY): {
				_request.body = token.get_string();
				break ;
			}
			case (TokenType::BODY_TERMINATION): {
				_request.finished = true;
				break ;
			}
			case (TokenType::EOF_TOKEN): {
				/* place holder, should not set finished when other stuff
				 * works */
				_request.finished = true;
				return ;
			}
			case (TokenType::UNFINISHED): {
				return ;
			}
		}
	}
}

/* to check wether the read request is complete or needs to be read further */
bool	Parser::is_finished(void) const {
	return (_request.finished);
}

t_http_request	Parser::get_request(void) const {
	return (_request);
}

/* once is_finished is true this can be called.
 * Resets the parser/lexer for the next client request with the same
 	connection.
 * todo: think of way to handle persistant connection (andvance input and reuse
	it
*/
void	Parser::_reset(void) {
	_lexer.reset();
	
/* once is_finished is true this can be called.
 * Resets the parser/lexer for the next client request with the same
 	connection.
*/_request.finished = false;
	/* todo: reset every field in _request */
}
