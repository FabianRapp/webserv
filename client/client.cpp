#include <unistd.h>
#include <cstring>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <poll.h>
#include <netdb.h>

int	main(void) {
	const char				*hostname = "google.com";
	const int16_t			port = 80;
	const struct hostent	*server = gethostbyname(hostname);
	if (server == NULL) {
		std::cerr << "Error: Server does not exist\n";
		return (1);
	}
	int	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		std::cerr << "Error: Socket error: " << strerror(errno) << '\n';
		return (1);
	}
	struct sockaddr_in	server_addr = {
		.sin_len = 0,
		.sin_family = AF_INET,
		.sin_port = htons(port),
		.sin_addr.s_addr = 0,
		.sin_zero = {0},
	};
	memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);
	char				buffer[100000];
	if (connect(fd, (struct sockaddr *)&server_addr, sizeof server_addr) < 0) {
		std::cerr << "Error: connect error: " << strerror(errno) << '\n';
		return (1);
	}
	

	std::string	request =
		std::string("GET / HTTP/1.1\r\n")
		+ "Host: " + hostname + "\r\n"
		+ "Connection: close\r\n\r\n"
	;
	if (send(fd, request.c_str(), request.length(), 0) < 0) {
		std::cerr << "Error: send error: " << strerror(errno) << '\n';
		return (1);
	}

	int received;

	while ((received = recv(fd, buffer, sizeof(buffer) - 1, 0)) > 0) {
		buffer[received] = '\0';
		std::cout << buffer;
	}
	if (received < 0) {
		perror("Error: Could not receive response");
		return (1);
	}
	if (close(fd) < 0) {
		perror("Error: Could not close socket");
		return (1);
	}
	return (0);
}
