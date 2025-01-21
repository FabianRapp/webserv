
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>

// #include "LocationConfigFile.hpp"
// #include "ServerConfigFile.hpp"
// #include "DefaultErrorPages.hpp"
#include "ConfigParser.hpp"

int main(int argc, char** argv) {
	// Create a Location object
	// LocationConfigFile loc1;
	// loc1.setPath("/");
	// loc1.setMethods(true, true, false); // Allow GET and POST only
	// loc1.setAutoIndex(false);
	// loc1.setIndexFile("index.html");

	// LocationConfigFile loc2;
	// loc2.setPath("/assets");
	// loc2.setMethods(true, true, true); // Allow GET, POST, DELETE
	// loc2.setAutoIndex(true);

	// // Create a Server object
	// ServerConfigFile server1;

	// server1.setPort(8080);
	// server1.setServerName("example.com");
	// server1.setRoot("/www/default");

	// server1.addErrorPage(404, "/errors/404.html");

	// server1.addLocation(loc1);
	// server1.addLocation(loc2);

	// // Print the server configuration
	// std::cout << "Testing Server Class:\n";

	// server1.printServer();

	// DefaultErrorPages err_object;
	// err_object.setErrorPageLink(999, "randomLink");
	// std::cout << "Error 404: " << err_object.getErrorPageLink(404) << std::endl;

	try {
		// Ensure a configuration file is provided as an argument
		if (argc < 2) {
			throw std::invalid_argument("Usage: ./program <config_file>");
		}

		// Create a ConfigParser object and parse the provided configuration file
		ConfigParser parser(argv[1]);

		// Retrieve the parsed servers
		const auto& servers = parser.getServers();

		// Print the parsed server configurations
		std::cout << "Parsed Servers:\n";
		for (size_t i = 0; i < servers.size(); ++i) {
			std::cout << "\nServer " << i + 1 << ":\n";
			//print all error codes + links
			servers[i].printServer();
		}

	} catch (const std::exception& e) {
		// Handle any exceptions thrown during parsing
		std::cerr << "Error: " << e.what() << '\n';
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
