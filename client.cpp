
#include <iostream>
#include <cstring>
#include <string>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <cerrno>
#include <iostream>
#include <string>

#include <poll.h>
#include <netdb.h>
#include <fcntl.h>

int	main(void) {
	const char				*hostname = "localhost";
	const int16_t			port = 9090;
	//const char				*hostname = "google.com";
	//const int16_t			port = 80;
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
		.sin_addr = {0},
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
	if (send(fd, request.c_str(), request.length(), 0) < 0) {
		std::cerr << "Error: send error: " << strerror(errno) << '\n';
		return (1);
	}
	std::cout << "requst send..\n";
	int received;

	/* why does this block if the server does not close the connection? */
	while ((received = recv(fd, buffer, sizeof(buffer) - 1, 0)) > 0) {
		buffer[received] = '\0';
		std::cout << "received " << received << "bytes\n";
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

/*
#include <sstream>
#include <iomanip>
#include <unistd.h>
std::string toHex(int decimal) {
    std::stringstream stream;
    stream << std::hex << decimal;  // Convert the decimal number to hexadecimal
    return stream.str();

}

//Unchunked main
int main() {
    const char *hostname = "localhost"; // Change to "google.com" for external requests
    const int16_t port = 8080;          // Change to 80 for standard HTTP

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
    struct sockaddr_in server_addr;{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    std::memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    // Connect to the server
    if (connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Error: Connection failed: " << strerror(errno) << '\n';
        close(socket_fd);
        return 1;
    }

    // Prepare the HTTP request
    std::string hostnameReq = "example.com"; // Replace with your hostname
    std::string body1 = "im string 1 \r\n";
    std::string body2 = "im string 2 \r\n";
    std::string body3 = "im string 3\r\n";
    size_t contentLength = body1.length() + body2.length() + body3.length();

    std::string headersPart1 =
        "GET / HTTP/1.1\r\n"
        "Host: " + hostnameReq + "\r\n";

    std::string headersPart2 =
        "Connection: close\r\n"
        "Content-Length: " + std::to_string(contentLength) + "\r\n\r\n";

    // Send the first part of headers
    if (send(socket_fd, headersPart1.c_str(), headersPart1.length(), 0) < 0) {
        std::cerr << "Error: Failed to send headers part 1: " << strerror(errno) << '\n';
        close(socket_fd);
        return 1;
    }
    sleep(2); // Simulate delay before sending the second part

    // Send the second part of headers
    if (send(socket_fd, headersPart2.c_str(), headersPart2.length(), 0) < 0) {
        std::cerr << "Error: Failed to send headers part 2: " << strerror(errno) << '\n';
        close(socket_fd);
        return 1;
    }

    // Send body parts with delays
    if (send(socket_fd, body1.c_str(), body1.length(), 0) < 0) {
        std::cerr << "Error: Failed to send body1: " << strerror(errno) << '\n';
        close(socket_fd);
        return 1;
    }
    sleep(5);

    if (send(socket_fd, body2.c_str(), body2.length(), 0) < 0) {
        std::cerr << "Error: Failed to send body2: " << strerror(errno) << '\n';
        close(socket_fd);
        return 1;
    }
    sleep(5);

    if (send(socket_fd, body3.c_str(), body3.length(), 0) < 0) {
        std::cerr << "Error: Failed to send body3: " << strerror(errno) << '\n';
        close(socket_fd);
        return 1;
    }

    std::cout << "Request sent successfully in multiple parts.\n";

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
*/
// Chunked main
/*
int main() {
    const char *hostname = "localhost"; // Change to "google.com" for external requests
    const int16_t port = 8080;          // Change to 80 for standard HTTP

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
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    std::memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    // Connect to the server
    if (connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Error: Connection failed: " << strerror(errno) << '\n';
        close(socket_fd);
        return 1;
    }

    // Prepare the HTTP request
    std::string hostnameReq = "example.com"; // Replace with your hostname
    // std::string chunk1 = "im string 1\r\n";
    // std::string chunk2 = "im string 2\r\n";
    // std::string chunk3 = "im string 3\r\n";
    std::string chunk1 = "im string 1 ";
    std::string chunk2 = "im string 2 ";
    std::string chunk3 = "im string 3";
    std::string headers =
        "GET / HTTP/1.1\r\n"
        "Host: " + hostnameReq + "\r\n"
        "Connection: close\r\n"
        "Transfer-Encoding: chunked\r\n\r\n";

    // Send headers
    if (send(socket_fd, headers.c_str(), headers.length(), 0) < 0) {
        std::cerr << "Error: Failed to send headers: " << strerror(errno) << '\n';
        close(socket_fd);
        return 1;
    }

    // Send chunk 1
    std::string chunk1Size = toHex(chunk1.length()) + "\r\n";
    if (send(socket_fd, chunk1Size.c_str(), chunk1Size.length(), 0) < 0 ||
        send(socket_fd, chunk1.c_str(), chunk1.length(), 0) < 0 ||
        send(socket_fd, "\r\n", 2, 0) < 0) {
        std::cerr << "Error: Failed to send chunk1: " << strerror(errno) << '\n';
        close(socket_fd);
        return 1;
    }
    sleep(5);

    // Send chunk 2
    std::string chunk2Size = toHex(chunk2.length()) + "\r\n";
    if (send(socket_fd, chunk2Size.c_str(), chunk2Size.length(), 0) < 0 ||
        send(socket_fd, chunk2.c_str(), chunk2.length(), 0) < 0 ||
        send(socket_fd, "\r\n", 2, 0) < 0) {
        std::cerr << "Error: Failed to send chunk2: " << strerror(errno) << '\n';
        close(socket_fd);
        return 1;
    }
    sleep(5);

    // Send chunk 3
    std::string chunk3Size = toHex(chunk3.length()) + "\r\n";
    if (send(socket_fd, chunk3Size.c_str(), chunk3Size.length(), 0) < 0 ||
        send(socket_fd, chunk3.c_str(), chunk3.length(), 0) < 0 ||
        send(socket_fd, "\r\n", 2, 0) < 0) {
        std::cerr << "Error: Failed to send chunk3: " << strerror(errno) << '\n';
        close(socket_fd);
        return 1;
    }

    // Send final chunk (0-length)
    if (send(socket_fd, "0\r\n\r\n", 5, 0) < 0) {
        std::cerr << "Error: Failed to send final chunk: " << strerror(errno) << '\n';
        close(socket_fd);
        return 1;
    }

    std::cout << "Chunked request sent successfully in multiple parts.\n";

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
*/
