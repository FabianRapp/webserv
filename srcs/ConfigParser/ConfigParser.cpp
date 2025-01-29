#include "../../includes/ConfigParser/ConfigParser.hpp"
#include <iostream>
#include <stdexcept>

// Default constructor
ConfigParser::ConfigParser() {}

// Constructor with file parsing
ConfigParser::ConfigParser(const std::string& config_file) {
	parseFile(config_file);
}

std::string ConfigParser::trimWhiteSpace(const std::string& str) const {
	size_t start = str.find_first_not_of(" \t\n\r");
	size_t end = str.find_last_not_of(" \t\n\r");

	return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

// void ConfigParser::parseFile(const std::string& config_file) {
// 	std::ifstream file(config_file);
// 	if (!file.is_open()) {
// 		throw ConfigParseError("Cannot open configuration file.");
// 	}

// 	bool in_server_block = false;
// 	int bracket_count = 0;
// 	ServerConfigFile current_server;

// 	std::string line;
// 	while (std::getline(file, line)) {
// 		line = trimWhiteSpace(line);

// 		// Skip empty lines or comments
// 		if (line.empty() || line[0] == '#') {
// 			continue;
// 		}

// 		// Handle server block start
// 		// if (line == "server {") {
// 		// 	if (in_server_block) {
// 		// 		throw ConfigParseError("Nested server blocks are not allowed.");
// 		// 	}
// 		// 	in_server_block = true;
// 		// 	current_server = ServerConfigFile();
// 		// 	bracket_count++;
// 		// 	continue;
// 		// }

// 		// // Handle end of server block
// 		// if (line == "}") {
// 		// 	if (in_server_block) {
// 		// 		_servers.push_back(current_server); // Add completed server to the list
// 		// 		in_server_block = false;
// 		// 		bracket_count--;
// 		// 		continue;
// 		// 	} else {
// 		// 		throw ConfigParseError("Unmatched closing bracket.");
// 		// 	}
// 		// }

// 	if (line == "server {") {
// 		if (in_server_block) {
// 			throw ConfigParseError("Nested server blocks are not allowed.");
// 		}
// 		in_server_block = true;
// 		current_server = ServerConfigFile();
// 		bracket_count++;
// 		continue;
// 	}

// 	if (line == "}") {
// 		if (in_server_block) {
// 			_servers.push_back(current_server); // Add completed server to the list
// 			in_server_block = false; // Reset flag
// 			bracket_count--;
// 			continue;
// 		} else {
// 			throw ConfigParseError("Unmatched closing bracket.");
// 		}
// 	}


// 		// Delegate to appropriate block parser
// 		if (in_server_block) {
// 			parseServerBlock(file, current_server, bracket_count);
// 		}
// 	}

// 	if (bracket_count != 0) {
// 		throw ConfigParseError("Mismatched brackets in configuration file.");
// 	}
// }

void ConfigParser::parseFile(const std::string& config_file) {
	std::ifstream file(config_file);
	if (!file.is_open()) {
		throw ConfigParseError("Cannot open configuration file.");
	}

	bool in_server_block = false;
	int bracket_count = 0;
	ServerConfigFile current_server;

	std::string line;
	while (std::getline(file, line)) {
		line = trimWhiteSpace(line);

		// Skip empty lines or comments
		if (line.empty() || line[0] == '#') {
			continue;
		}

		// Debug: Print the current line and bracket count
		// std::cout << "Parsing line: " << line << " | Bracket count: " << bracket_count << "\n";

		// Handle start of server block
		if (line == "server {") {
			if (in_server_block) {
				throw ConfigParseError("Nested server blocks are not allowed.");
			}
			in_server_block = true;
			current_server = ServerConfigFile();
			bracket_count++;

			// Delegate parsing to parseServerBlock
			parseServerBlock(file, current_server, bracket_count);

			// Add completed server to the list
			_servers.push_back(current_server);
			in_server_block = false; // Reset flag after completing the block
			continue;
		}

		// Handle unmatched closing brackets
		if (line == "}") {
			throw ConfigParseError("Unmatched closing bracket.");
		}
	}

	// Debug: Final bracket count
	std::cout << "Final Bracket Count: " << bracket_count << "\n";

	if (bracket_count != 0) {
		throw ConfigParseError("Mismatched brackets in configuration file.");
	}
}


// void ConfigParser::parseServerBlock(std::ifstream& file, ServerConfigFile& current_server, int& bracket_count) {
// 	std::string line;

// 	while (std::getline(file, line)) {
// 		line = trimWhiteSpace(line);

// 		if (line.empty() || line[0] == '#') {
// 			continue; // Skip empty lines or comments
// 		}

// 		// Handle location block start
// 		if (line.find("location") == 0 && line.back() == '{') {
// 			LocationConfigFile current_location;
// 			current_location.setPath(line.substr(9, line.size() - 10)); // Extract path from "location /path {"
// 			parseLocationBlock(file, current_location, bracket_count);
// 			current_server.addLocation(current_location);
// 			continue;
// 		}

// 		// Handle end of server block
// 		if (line == "}") {
// 			bracket_count--;
// 			return; // End parsing this server block
// 		}

// 		// Parse key-value pairs inside the server block
// 		if (line.find("listen ") == 0) {
// 			current_server.setPort(std::stoi(line.substr(7)));
// 		} else if (line.find("server_name ") == 0) {
// 			current_server.setServerName(line.substr(12));
// 		} else if (line.find("root ") == 0) {
// 			current_server.setRoot(line.substr(5));
// 		} else if (line.find("error_page ") == 0) {
// 			int code = std::stoi(line.substr(11, 3)); // Extract error code
// 			std::string path = trimWhiteSpace(line.substr(15)); // Extract path
// 			if (!path.empty() && path.back() == ';') {
// 				path.pop_back(); // Remove trailing semicolon
// 			}
// 			current_server.addErrorPage(code, path);
// 		} else if (line.find("index ") == 0) { // Handle index directive
// 			std::string index_file = trimWhiteSpace(line.substr(6)); // Extract index file
// 			if (!index_file.empty() && index_file.back() == ';') {
// 				index_file.pop_back(); // Remove trailing semicolon
// 			}
// 			// current_server.addErrorPage(200, index_file); // Optional: Store as "default" for successful requests
// 		} else {
// 			throw ConfigParseError("Invalid directive inside server block: " + line);
// 		}
// 	}
// }


void ConfigParser::parseServerBlock(std::ifstream& file, ServerConfigFile& current_server, int& bracket_count) {
	std::string line;

	while (std::getline(file, line)) {
		line = trimWhiteSpace(line);

		if (line.empty() || line[0] == '#') {
			continue; // Skip empty lines or comments
		}

		// Debugging output for tracking brackets
		// std::cout << "Parsing server block line: " << line << " | Bracket count: " << bracket_count << "\n";

		// Handle location block start
		if (line.find("location") == 0 && line.back() == '{') {
			bracket_count++; // Increment for location block start
			LocationConfigFile current_location;
			current_location.setPath(line.substr(9, line.size() - 10)); // Extract path from "location /path {"
			parseLocationBlock(file, current_location, bracket_count);
			current_server.addLocation(current_location);
			continue;
		}

		// Handle end of server block
		if (line == "}") {
			bracket_count--; // Decrement for closing bracket
			return; // End parsing this server block
		}

		// Parse key-value pairs inside the server block
		if (!line.empty() && line.back() == ';') {
				line.pop_back(); // Remove trailing semicolon
			}
		if (line.find("listen ") == 0) {
			current_server.setPort(std::stoi(line.substr(7)));
		} else if (line.find("server_name ") == 0) {
			current_server.setServerNames(line.substr(12));
		} else if (line.find("root ") == 0) {
			current_server.setRoot(line.substr(5));
		} else if (line.find("error_page ") == 0) {
			int code = std::stoi(line.substr(11, 3)); // Extract error code
			std::string path = trimWhiteSpace(line.substr(15)); // Extract path
			current_server.addErrorPage(code, path);
		} else if (line.find("index ") == 0) { // Handle index directive
			std::string index_file = trimWhiteSpace(line.substr(6)); // Extract index file
			if (!index_file.empty() && index_file.back() == ';') {
				index_file.pop_back(); // Remove trailing semicolon
			}
			// Optional logic to store index file in the ServerConfigFile object.
		} else {
			throw ConfigParseError("Invalid directive inside server block: " + line);
		}
	}
}




void ConfigParser::parseLocationBlock(std::ifstream& file, LocationConfigFile& current_location, int& bracket_count) {
	std::string line;

	while (std::getline(file, line)) {
		line = trimWhiteSpace(line);

		if (line.empty() || line[0] == '#') {
			continue; // Skip empty lines or comments
		}

		// Debugging output for tracking brackets
		// std::cout << "Parsing location block line: " << line << " | Bracket count: " << bracket_count << "\n";

		// Handle end of location block
		if (line == "}") {
			bracket_count--; // Decrement for closing bracket of location block
			return; // End parsing this location block
		}

		// Parse key-value pairs inside the location block
		if (line.find("allowed_methods ") == 0) {
			current_location.setMethods(
				line.find("GET") != std::string::npos,
				line.find("POST") != std::string::npos,
				line.find("DELETE") != std::string::npos
			);
		} else if (line.find("autoindex ") == 0) {
			current_location.setAutoIndex(line.substr(10) == "on");
		} else if (line.find("upload_dir ") == 0) {
			current_location.setUploadDir(line.substr(11));
		} else if (line.find("index ") == 0) {
			current_location.setIndexFile(line.substr(6));
		} else {
			throw ConfigParseError("Invalid directive inside location block: " + line);
		}
	}
}


const std::vector<ServerConfigFile> ConfigParser::getServers() const {
	return _servers;
}
