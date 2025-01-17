
#include <iostream>
#include <cstring>
#include <string>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <cerrno>

int main() {
	const char *hostname = "localhost"; // Change to "google.com" for external requests
	const int16_t port = 8080;		  // Change to 80 for standard HTTP

	// Resolve hostname to IP address
	struct hostent *server = gethostbyname(hostname);
	if (!server) {
		std::cerr << "Error: Server not found.\n";
		return 1;
	}

	// Create a socket
	int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd < 0) {
		std::cerr << "Error: Unable to create socket: " << strerror(errno) << '\n';
		return 1;
	}

	// Configure server address
	struct sockaddr_in server_addr; {};
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	std::memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);

	// Connect to the server
	if (connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		std::cerr << "Error: Connection failed: " << strerror(errno) << '\n';
		close(socket_fd);
		return 1;
	}

	// Prepare HTTP GET request
	std::string request =
		"GET / HTTP/1.1\r\n"
		"Host: " + std::string(hostname) + "\r\n"
		"Connection: close\r\n\r\n"
		"heloo i would be" + "\r\n"
		"the body" + "\r\n";

	// Send the request
	if (send(socket_fd, request.c_str(), request.length(), 0) < 0) {
		std::cerr << "Error: Failed to send request: " << strerror(errno) << '\n';
		close(socket_fd);
		return 1;
	}
	std::cout << "Request sent successfully.\n";

	// Receive the response
	char buffer[1024];
	int bytes_received;
	while ((bytes_received = recv(socket_fd, buffer, sizeof(buffer) - 1, 0)) > 0) {
		buffer[bytes_received] = '\0'; // Null-terminate the received data
		std::cout << buffer;
	}

	if (bytes_received < 0) {
		std::cerr << "Error: Failed to receive response: " << strerror(errno) << '\n';
	}

	// Clean up
	close(socket_fd);
	return 0;
}









// #include <unistd.h>
// #include <cstring>
// #include <errno.h>
// #include <sys/socket.h>
// #include <arpa/inet.h>
// #include <iostream>
// #include <string>
// #include <poll.h>
// #include <netdb.h>
// #include <fcntl.h>

// int	main(void) {
// 	const char				*hostname = "localhost";
// 	const int16_t			port = 8080;
// 	//const char				*hostname = "google.com";
// 	//const int16_t			port = 80;
// 	const struct hostent	*server = gethostbyname(hostname);
// 	if (server == NULL) {
// 		std::cerr << "Error: Server does not exist\n";
// 		return (1);
// 	}
// 	int	fd = socket(AF_INET, SOCK_STREAM, 0);
// 	if (fd < 0) {
// 		std::cerr << "Error: Socket error: " << strerror(errno) << '\n';
// 		return (1);
// 	}
// 	struct sockaddr_in	server_addr = {
// 		.sin_len = 0,
// 		.sin_family = AF_INET,
// 		.sin_port = htons(port),
// 		.sin_addr.s_addr = 0,
// 		.sin_zero = {0},
// 	};
// 	memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);
// 	char				buffer[100000];
// 	if (connect(fd, (struct sockaddr *)&server_addr, sizeof server_addr) < 0) {
// 		std::cerr << "Error: connect error: " << strerror(errno) << '\n';
// 		return (1);
// 	}


// 	std::string	request =
// 		std::string("GET / HTTP/1.1\r\n")
// 		+ "Host: " + hostname + "\r\n"
// 		+ "Connection: close\r\n\r\n"
// 	;
// 	if (send(fd, request.c_str(), request.length(), 0) < 0) {
// 		std::cerr << "Error: send error: " << strerror(errno) << '\n';
// 		return (1);
// 	}
// 	std::cout << "request sended..\n";
// 	int received;

// 	/* why does this block if the server does not close the connection? */
// 	while ((received = recv(fd, buffer, sizeof(buffer) - 1, 0)) > 0) {
// 		buffer[received] = '\0';
// 		std::cout << "received " << received << "bytes\n";
// 		std::cout << buffer;
// 	}
// 	if (received < 0) {
// 		perror("Error: Could not receive response");
// 		return (1);
// 	}
// 	if (close(fd) < 0) {
// 		perror("Error: Could not close socket");
// 		return (1);
// 	}
// 	return (0);
// }