#include "../includes/enums.hpp"

std::string to_string(HeaderType val) {
	switch (val) {
		// Request Headers
		case HeaderType::ACCEPT: return "ACCEPT";
		case HeaderType::ACCEPT_CHARSET: return "ACCEPT_CHARSET";
		case HeaderType::ACCEPT_ENCODING: return "ACCEPT_ENCODING";
		case HeaderType::ACCEPT_LANGUAGE: return "ACCEPT_LANGUAGE";
		case HeaderType::AUTHORIZATION: return "AUTHORIZATION";
		case HeaderType::EXPECT: return "EXPECT";
		case HeaderType::FROM: return "FROM";
		case HeaderType::HOST: return "HOST";
		case HeaderType::IF_MATCH: return "IF_MATCH";
		case HeaderType::IF_MODIFIED_SINCE: return "IF_MODIFIED_SINCE";
		case HeaderType::IF_NONE_MATCH: return "IF_NONE_MATCH";
		case HeaderType::IF_RANGE: return "IF_RANGE";
		case HeaderType::IF_UNMODIFIED_SINCE: return "IF_UNMODIFIED_SINCE";
		case HeaderType::MAX_FORWARDS: return "MAX_FORWARDS";
		case HeaderType::PROXY_AUTHORIZATION: return "PROXY_AUTHORIZATION";
		case HeaderType::RANGE: return "RANGE";
		case HeaderType::REFERER: return "REFERER";
		case HeaderType::TE: return "TE";
		case HeaderType::USER_AGENT: return "USER_AGENT";

		// General Headers
		case HeaderType::CACHE_CONTROL: return "CACHE_CONTROL";
		case HeaderType::CONNECTION: return "CONNECTION";
		case HeaderType::DATE: return "DATE";
		case HeaderType::PRAGMA: return "PRAGMA";
		case HeaderType::TRAILER: return "TRAILER";
		case HeaderType::TRANSFER_ENCODING: return "TRANSFER_ENCODING";
		case HeaderType::UPGRADE: return "UPGRADE";
		case HeaderType::VIA: return "VIA";
		case HeaderType::WARNING: return "WARNING";

		// Entity Headers
		case HeaderType::ALLOW: return "ALLOW";
		case HeaderType::CONTENT_ENCODING: return "CONTENT_ENCODING";
		case HeaderType::CONTENT_LANGUAGE: return "CONTENT_LANGUAGE";
		case HeaderType::CONTENT_LENGTH: return "CONTENT_LENGTH";
		case HeaderType::CONTENT_LOCATION: return "CONTENT_LOCATION";
		case HeaderType::CONTENT_MD5: return "CONTENT_MD5";
		case HeaderType::CONTENT_RANGE: return "CONTENT_RANGE";
		case HeaderType::CONTENT_TYPE: return "CONTENT_TYPE";
		case HeaderType::EXPIRES: return "EXPIRES";
		case HeaderType::LAST_MODIFIED: return "LAST_MODIFIED";

		// Extension Headers
		case HeaderType::COOKIE: return "COOKIE";
		case HeaderType::SET_COOKIE: return "SET_COOKIE";
		case HeaderType::DNT: return "DNT";
		case HeaderType::ORIGIN: return "ORIGIN";
		case HeaderType::UPGRADE_INSECURE_REQUESTS: return "UPGRADE_INSECURE_REQUESTS";

		// Additional Optional Headers
		case HeaderType::ACCEPT_DATETIME: return "ACCEPT_DATETIME";
		case HeaderType::EARLY_DATA: return "EARLY_DATA";

		// Invalid case
		case HeaderType::INVALID: return "INVALID";

		default:
			std::cerr << "Error: to_string: unsupported HeaderType value" << std::endl;
			FT_ASSERT(0);
			return nullptr;
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
