#include <Token.hpp>

Token::Token(TokenType type,
	std::variant<
		std::monostate,
		MethodType,
		std::string,
		std::pair<HeaderType, std::string>
	> value)
	: type(type), value(std::move(value)) {}

Token::Token(const Token &old) :type(old.type), value(old.value) {}

Token &	Token::operator=(const Token & right) {
	if (this == &right) {
		return (*this);
	}
	this->type = right.type;
	this->value = right.value;
	return (*this);
}
MethodType &	Token::get_method(void) {
	FT_ASSERT(type == TokenType::METHOD);
	return (std::get<MethodType>(value));
}

std::pair<HeaderType, std::string> &	Token::get_header(void) {
	FT_ASSERT(type == TokenType::HEADER);
	return (std::get<std::pair<HeaderType, std::string>>(value));
}

void	Token::get_no_value(void) {
	FT_ASSERT(
		type == TokenType::EOF_TOKEN
		|| type == TokenType::BODY_TERMINATION
		|| type == TokenType::HEADER_TERMINATION
	);
	return ;
}

std::string &	Token::get_string(void) {
	FT_ASSERT(
		type == TokenType::VERSION
		|| type == TokenType::URI
		|| type == TokenType::BODY
	);
	return (std::get<std::string>(value));
}

std::string	Token::full_string() const {
	std::string	str = to_string(type) + ": ";
	switch (type) {
		case TokenType::METHOD: {
			return (str + to_string(std::get<MethodType>(value)));
		} case TokenType::URI: {
			return (str + std::get<std::string>(value));
		} case TokenType::VERSION: {
			return (str + std::get<std::string>(value));
		} case TokenType::HEADER: {
			const auto &	header =
				std::get<std::pair<HeaderType, std::string>>(value);
			return (str + to_string(header.first) + ": " + header.second);
		} case TokenType::BODY: {
			return (str + std::get<std::string>(value));
		} case TokenType::HEADER_TERMINATION: {
			return (to_string(type));
		} case TokenType::BODY_TERMINATION: {
			return (to_string(type));
		} case TokenType::EOF_TOKEN: {
			return (to_string(type));
		} default: {
			FT_ASSERT(0 && "unknown token type");
		}
	}
}
