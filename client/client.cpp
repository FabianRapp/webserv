
#include <iostream>
#include <cstring>
#include <string>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <cerrno>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <unistd.h>
std::string toHex(int decimal) {
    std::stringstream stream;
    stream << std::hex << decimal;  // Convert the decimal number to hexadecimal
    return stream.str();
}


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

		// Example of unchunked request
			// std::string hostnameReq = "example.com";  // Replace with your hostname
			// std::string body = "heloo i would be\r\nthe body\r\n";
			// size_t contentLength = body.length();

			// std::string request =
			// 	"GET / HTTP/1.1\r\n"
			// 	"Host: " + hostnameReq + "\r\n"
			// 	"Connection: close\r\n"
			// 	"Content-Length: " + std::to_string(contentLength) + "\r\n\r\n"
			// 	+ body;


			std::string hostnameReq = "example.com";  // Replace with your hostname
			std::string body = "heloo i would be\r\nthe body\r\n";

			// Split the body into chunks
			std::string chunk1 = "heloo i would be ";
			std::string chunk2 = "the body";

			// Calculate chunk sizes in hexadecimal
			std::string chunk1Size = toHex(chunk1.size());  // Get the size of chunk1 and convert to hex
			std::string chunk2Size = toHex(chunk2.size());  // Get the size of chunk2 and convert to hex

			// Construct the chunked body
			std::string chunkedBody =
				chunk1Size + "\r\n" + chunk1 + "\r\n" +
				chunk2Size + "\r\n" + chunk2 + "\r\n" +
				"0\r\n\r\n";  // "0\r\n\r\n" indicates the end of the chunked body

			// Construct the full HTTP request with chunked encoding
			std::string request =
				"GET / HTTP/1.1\r\n"
				"Host: " + hostnameReq + "\r\n"
				"Connection: close\r\n"
				"Transfer-Encoding: chunked\r\n\r\n"  // Use chunked transfer encoding
				+ chunkedBody;

	// Send the request
	std::string m1 = request.substr(0, request.size() / 2 + 20);
	std::string m2 = request.substr(request.size() / 2 + 20, request.size());
	if (send(socket_fd, m1.c_str(), m1.length(), 0) < 0) {
		std::cerr << "Error: Failed to send request: " << strerror(errno) << '\n';
		close(socket_fd);
		return 1;
	}
	sleep(10);
	if (send(socket_fd, m2.c_str(), m2.length(), 0) < 0) {
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