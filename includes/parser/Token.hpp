#pragma once

#include <macros.h>
#include <string>
#include <variant>

/*
 * Header token types:
Accept,
Accept_Charset,
Accept_Encoding,
Accept_Language,
Authorization,
Expect,
From,
Host,
If_Match,
If_Modified_Since,
If_None_Match,
If_Range,
If_Unmodified_Since,
Max_Forwards,
Proxy_Authorization,
Range,
Referer,
TE,
User_Agent,
*/


/* for me the 'HOST' type is missing but as i read the docs these current
 * types align more with the docs?*/
enum class TokenType {
	METHOD_TYPE,
	URI,
	VERSION,
	HEADER,
	HEADER_TERMINATION,
	BODY,
	BODY_TERMINATION,
	EOF_TOKEN,
};

enum class MethodType {
	OPTIONS,
	GET,
	HEAD,
	POST,
	PUT,
	DELETE,
	TRACE,
	CONNECT,
};

class Token {
public:
	TokenType type;

	std::variant<
		std::monostate, /* EOF_TOKEN, BODY_TERMINATION, HEADER_TERMINATION */
		MethodType, /* METHOD_TYPE */
		std::string, /* VERSION, URI, BODY */
		std::pair<std::string, std::string> /* HEADER_TOKEN */
	>	value;

	Token(TokenType type,
		  std::variant<
			std::monostate,
			MethodType,
			std::string,
			std::pair<std::string, std::string>
		  > value = {})
		: type(type), value(std::move(value)) {}

	std::string	to_string() const {
		switch (type) {
			case TokenType::METHOD_TYPE: {
				return ("Method: "
					+ Token::_method_to_string(std::get<MethodType>(value)));
			} case TokenType::URI: {
				return ("URI: " + std::get<std::string>(value));
			} case TokenType::VERSION: {
				return ("Version: " + std::get<std::string>(value));
			} case TokenType::HEADER: {
				const auto & header =
					std::get<std::pair<std::string, std::string>>(value);
				return ("Header: " + header.first + ": " + header.second);
			} case TokenType::BODY: {
				return ("Body: " + std::get<std::string>(value));
			} case TokenType::HEADER_TERMINATION: {
				return ("HEADER_TERMINATION");
			} case TokenType::BODY_TERMINATION: {
				return ("BODY_TERMINATION");
			} case TokenType::EOF_TOKEN: {
				return ("EOF_TOKEN");
			} default: {
				FT_ASSERT(0 && "unknown token type");
			}
		}
	}

private:
	static std::string _method_to_string(MethodType method) {
		switch (method) {
			case MethodType::GET: return ("GET");
			case MethodType::POST: return ("POST");
			case MethodType::PUT: return ("PUT");
			case MethodType::DELETE: return ("DELETE");
			default: return ("UNKNOWN");
		}
	}
};

