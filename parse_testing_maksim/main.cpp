
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>

#include "LocationConfigFile.hpp"
#include "ServerConfigFile.hpp"


int main() {
	// Create a Location object
	LocationConfigFile loc1;
	loc1.setPath("/");
	loc1.setMethods(true, true, false); // Allow GET and POST only
	loc1.setAutoIndex(false);
	loc1.setIndexFile("index.html");

	LocationConfigFile loc2;
	loc2.setPath("/assets");
	loc2.setMethods(true, true, true); // Allow GET, POST, DELETE
	loc2.setAutoIndex(true);

	// Create a Server object
	ServerConfigFile server1;

	server1.setPort(8080);
	server1.setServerName("example.com");
	server1.setRoot("/www/default");

	server1.addErrorPage(404, "/errors/404.html");

	server1.addLocation(loc1);
	server1.addLocation(loc2);

	// Print the server configuration
	std::cout << "Testing Server Class:\n";

	server1.printServer();

	return 0;
}
