#include <Webserv.hpp>

Webserv::Webserv(void) {
}

/* todo: parse config file */
Webserv::Webserv(const char * config_file_path) {
	(void)config_file_path;
}

Webserv::~Webserv(void) {
}

/* main loop */
noreturn
void	Webserv::run(void) {
	while (1) {
	}
}

/* wraper for Parser, I think leaving it like this should be the simplest */
t_http_request	Webserv::_parse(std::string raw_input) {
	Parser	parser(raw_input);
	return (parser.parse());
}

/* todo: */
void	Webserv::_send(t_http_response msg) {
	(void)msg;
}
