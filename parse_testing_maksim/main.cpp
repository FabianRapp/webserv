
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
	// Ensure a configuration file is provided as an argument
	if (argc < 2) {
		std::cerr << "Usage: ./program <config_file>" << std::endl;
		return EXIT_FAILURE;
	}

	// Parse the configuration file
	ConfigParser parser(argv[1]);

	// Retrieve servers as a vector
	std::vector<ServerConfigFile> servers = parser.getServers();

	// Print all parsed information step by step
	std::cout << "Parsed Servers:\n";

	for (size_t i = 0; i < servers.size(); ++i) {
		ServerConfigFile server = servers[i];
		std::cout << "\nServer " << i + 1 << ":\n";

		// Print server details
		std::cout << "Port: " << server.getPort() << "\n";
		std::cout << "Server Name: " << server.getServerName() << "\n";
		std::cout << "Root Directory: " << server.getRoot() << "\n";

		// Print error pages
		// std::cout << "Error Pages:\n";
		// const std::map<int, std::string>& error_pages = server.getErrorPages();
		// for (const auto& error_page : error_pages) {
		// 	int code = error_page.first;
		// 	std::string path = error_page.second;
		// 	std::cout << "  Error Code " << code << ": " << path << "\n";
		// }

		// Print locations
		std::cout << "Locations:\n";
		const std::vector<LocationConfigFile>& locations = server.getLocations();
		for (size_t j = 0; j < locations.size(); ++j) {
			LocationConfigFile location = locations[j];
			std::cout << "  Location Path: " << location.getPath() << "\n";
			std::cout << "  Allowed Methods: ";
			if (location.isGetAllowed()) std::cout << "GET ";
			if (location.isPostAllowed()) std::cout << "POST ";
			if (location.isDeleteAllowed()) std::cout << "DELETE ";
			std::cout << "\n";
			std::cout << "  Index File: " << location.getIndexFile() << "\n";
			if (!location.getUploadDir().empty()) {
				std::cout << "  Upload Directory: " << location.getUploadDir() << "\n";
			}
		}
	}

	return EXIT_SUCCESS;
}