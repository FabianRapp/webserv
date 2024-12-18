#include <Webserv.hpp>

static struct sockaddr_in	make_addr(unsigned short port) {
	struct sockaddr_in	addr;

	memset(&addr, 0, sizeof(struct sockaddr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	return (addr);
}

Webserv::Webserv(void) {
	/* AF_INET : ipv4
	 * AF_INET6: ipv6 */
	_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_server_fd < 0) {
		std::cerr << "Error: " << strerror(errno) << '\n';
		exit(errno);
	}
	_server_addr = make_addr(8080);
	if (bind(_server_fd, reinterpret_cast<struct sockaddr *>(&_server_addr), sizeof(_server_addr)) < 0) {
		close(_server_fd);
		std::cerr << "Error: " << strerror(errno) << '\n';
		exit(errno);
	}
	_addr_len = static_cast<socklen_t>(sizeof _server_addr);
	if (listen(_server_fd, REQUEST_QUE_SIZE) < 0) {
		close(_server_fd);
		std::cerr << "Error: " << strerror(errno) << '\n';
		exit(errno);
	}
	std::cout << "Started server on port 8080...\n";
}

/* todo: parse config file */
Webserv::Webserv(const char * config_file_path) {
	FT_ASSERT(0 && "not implemented");
	(void)config_file_path;
}

Webserv::~Webserv(void) {
}

/* main loop */
[[noreturn]]
void	Webserv::run(void) {
	while (1) {
		int	client_fd = accept(_server_fd,
			reinterpret_cast<struct sockaddr *>(&_client_addr), &_addr_len);
		if (client_fd < 0) {
			std::cerr << "Error: " << strerror(errno) << '\n';
			continue ;
		}
		std::cout << "Connection accepted from address("
			<< inet_ntoa(_client_addr.sin_addr) << "): PORT("
			<< ntohs(_client_addr.sin_port) << ")\n";

		char	buffer[4096];
		long int bytes_read = read(client_fd, buffer, sizeof buffer - 1);
		buffer[bytes_read] = 0;
		printf("Received Request:\n%s\n", buffer);

		//write(client_fd, http_response, strlen(http_response));
		//printf("Response sent to client.\n");

		close(client_fd);
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
