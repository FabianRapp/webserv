#include <enums.hpp>

std::string	to_string(HeaderType val) {
	switch (val) {
		case HeaderType::ACCEPT: return ("ACCEPT");
		case HeaderType::ACCEPT_CHARSET: return ("ACCEPT_CHARSET");
		case HeaderType::ACCEPT_ENCODING: return ("ACCEPT_ENCODING");
		case HeaderType::ACCEPT_LANGUAGE: return ("ACCEPT_LANGUAGE");
		case HeaderType::AUTHORIZATION: return ("AUTHORIZATION");
		case HeaderType::EXPECT: return ("EXPECT");
		case HeaderType::FROM: return ("FROM");
		case HeaderType::HOST: return ("HOST");
		case HeaderType::IF_MATCH: return ("IF_MATCH");
		case HeaderType::IF_MODIFIED_SINCE: return ("IF_MODIFIED_SINCE");
		case HeaderType::IF_NONE_MATCH: return ("IF_NONE_MATCH");
		case HeaderType::IF_RANGE: return ("IF_RANGE");
		case HeaderType::IF_UNMODIFIED_SINCE: return ("IF_UNMODIFIED_SINCE");
		case HeaderType::MAX_FORWARDS: return ("MAX_FORWARDS");
		case HeaderType::PROXY_AUTHORIZATION: return ("PROXY_AUTHORIZATION");
		case HeaderType::RANGE: return ("RANGE");
		case HeaderType::REFERER: return ("REFERER");
		case HeaderType::TE: return ("TE");
		case HeaderType::USER_AGENT: return ("USER_AGENT");
		default:
			std::cerr << "Error: to_string: unsupported HeaderType value" << std::endl;
			FT_ASSERT(0);
			return (nullptr);
	}
}

std::string	to_string(TokenType val) {
	switch (val) {
		case TokenType::METHOD: return ("METHOD");
		case TokenType::URI: return ("URI");
		case TokenType::VERSION: return ("VERSION");
		case TokenType::HEADER: return ("HEADER");
		case TokenType::HEADER_TERMINATION: return ("HEADER_TERMINATION");
		case TokenType::BODY: return ("BODY");
		case TokenType::BODY_TERMINATION: return ("BODY_TERMINATION");
		case TokenType::EOF_TOKEN: return ("EOF_TOKEN");
		case TokenType::UNFINISHED: return ("UNFINISHED");
		default:
			std::cerr << "Error: to_string: unsupported TokenType value" << std::endl;
			FT_ASSERT(0);
			return (nullptr);
	}
}

std::string	to_string(MethodType val) {
	switch (val) {
		case MethodType::OPTIONS: return ("OPTIONS");
		case MethodType::GET: return ("GET");
		case MethodType::HEAD: return ("HEAD");
		case MethodType::POST: return ("POST");
		case MethodType::PUT: return ("PUT");
		case MethodType::DELETE: return ("DELETE");
		case MethodType::TRACE: return ("TRACE");
		case MethodType::CONNECT: return ("CONNECT");
		case MethodType::INVALID: return ("INVALID");
		default:
			std::cerr << "Error: to_string: unsupported MethodType" << std::endl;
			FT_ASSERT(0);
			return (nullptr);
	}
}
