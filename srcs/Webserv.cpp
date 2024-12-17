#include <Webserv.hpp>

Webserv::Webserv(void) {
}

Webserv::~Webserv(void) {
}

t_http_request	Webserv::parse(std::string raw_input) {
	Parser	parser(raw_input);
	return (parser.parse());
}
