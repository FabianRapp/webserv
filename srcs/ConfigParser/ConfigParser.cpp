#include "../../includes/ConfigParser/ConfigParser.hpp"
#include <iostream>
#include <stdexcept>

//validateMethods
#include <set>
#include <sstream>
// #include <algorithm>

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

void ConfigParser::parseFile(const std::string& config_file) {
	std::ifstream file(config_file);
	if (!file.is_open()) {
		throw std::runtime_error("Cannot open configuration file.");
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
				throw std::runtime_error("Nested server blocks are not allowed.");
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
			throw std::runtime_error("Unmatched closing bracket.");
		}
	}

	// Debug: Final bracket count
	std::cout << "Final Bracket Count: " << bracket_count << "\n";

	if (bracket_count != 0) {
		throw std::runtime_error("Mismatched brackets in configuration file.");
	}
}

// void ConfigParser::parseServerBlock(std::ifstream& file, ServerConfigFile& current_server, int& bracket_count) {
// 	std::string line;

// 	while (std::getline(file, line)) {
// 		line = trimWhiteSpace(line);

// 		if (line.empty() || line[0] == '#') {
// 			continue; // Skip empty lines or comments
// 		}

// 		// Debugging output for tracking brackets
// 		// std::cout << "Parsing server block line: " << line << " | Bracket count: " << bracket_count << "\n";

// 		// Handle location block start
// 		if (line.find("location") == 0 && line.back() == '{') {
// 			//todo: delete, just for testing
// 			std::cout << "line here: " << "|" << line << "|" << std::endl;
// 			bracket_count++; // Increment for location block start
// 			LocationConfigFile current_location;
// 			current_location.setPath(line.substr(9, line.size() - 10)); // Extract path from "location /path {"
// 			//todo: delete this, just for testing
// 			std::cout << "Parsed path: " << "|" << current_location.getPath() << "|" << std::endl;
// 			parseLocationBlock(file, current_location, bracket_count);
// 			current_server.addLocation(current_location);
// 			continue;
// 		}

// 		// Handle end of server block
// 		if (line == "}") {
// 			bracket_count--; // Decrement for closing bracket
// 			return; // End parsing this server block
// 		}

// 		// Parse key-value pairs inside the server block
// 		if (!line.empty() && line.back() == ';') {
// 				line.pop_back(); // Remove trailing semicolon
// 			}
// 		if (line.find("listen ") == 0) {
// 			current_server.setPort(std::stoi(line.substr(7)));
// 		} else if (line.find("server_name ") == 0) {
// 			current_server.setServerNames(line.substr(12));
// 		} else if (line.find("root ") == 0) {
// 			current_server.setRoot(line.substr(5));
// 		} else if (line.find("error_page ") == 0) {
// 			int code = std::stoi(line.substr(11, 3)); // Extract error code
// 			std::string path = trimWhiteSpace(line.substr(15)); // Extract path
// 			current_server.addErrorPage(code, path);
// 		} else if (line.find("index ") == 0) { // Handle index directive
// 			std::string index_file = trimWhiteSpace(line.substr(6)); // Extract index file
// 			if (!index_file.empty() && index_file.back() == ';') {
// 				index_file.pop_back(); // Remove trailing semicolon
// 			}
// 			// Optional logic to store index file in the ServerConfigFile object.
// 		} else {
// 			throw std::runtime_error("Invalid directive inside server block: " + line);
// 		}
// 	}
// }

bool ConfigParser::isValidLocationPath(const std::string& path) const {
	// Check if the path starts with '/'
	if (path.empty() || path[0] != '/') {
		return false;
	}

	// Check if the path ends with '/'
	if (path.size() > 1 && path.back() == '/') {
		return false;
	}

	// Check if the path contains only allowed characters
	for (char ch : path) {
		if (!std::isalnum(ch) && ch != '/' && ch != '_' && ch != '-') {
			return false;
		}
	}

	// Ensure there are no more then one slashes (e.g., "//")
	if (path.find("//") != std::string::npos) {
		return false;
	}

	return true;
}

void ConfigParser::parseServerBlock(std::ifstream& file, ServerConfigFile& current_server, int& bracket_count) {
	std::string line;

	while (std::getline(file, line)) {
		line = trimWhiteSpace(line);

		if (line.empty() || line[0] == '#') {
			continue; // Skip empty lines or comments
		}

		// Handle location block start
		if (line.find("location") == 0 && line.back() == '{') {
			size_t location_start = 8; // "location" is 8 characters long
			size_t location_end = line.find_last_of('{') - 1; // Exclude the '{' and space before it
			std::string path = trimWhiteSpace(line.substr(location_start, location_end - location_start));

			// Validate the path
			if (!isValidLocationPath(path)) {
				throw std::runtime_error("Invalid location path: " + path);
			}

			// Debug: Print the parsed path
			std::cout << "Parsed path: " << "|" << path << "|" << std::endl;

			bracket_count++; // Increment for location block start
			LocationConfigFile current_location;
			current_location.setPath(path);
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
		} else if (line.find("index ") == 0) {
			std::string index_file = trimWhiteSpace(line.substr(6)); // Extract index file
			if (!index_file.empty() && index_file.back() == ';') {
				index_file.pop_back(); // Remove trailing semicolon
			}
		} else {
			throw std::runtime_error("Invalid directive inside server block: " + line);
		}
	}
}

void ConfigParser::validateAutoIndex(const std::string& value, LocationConfigFile& current_location) {
	if (value == "on") {
		current_location.setAutoIndex(true);
	} else if (value == "off") {
		current_location.setAutoIndex(false);
	} else {
		throw std::runtime_error("Invalid value for autoindex: " + value + ". Only 'on' or 'off'.");
	}
}


void ConfigParser::validateMethods(const std::string& methods_str, LocationConfigFile& current_location) {
	// Define the valid methods
	const std::set<std::string> valid_methods = {"GET", "POST", "DELETE"};

	// Tokenize the methods string
	std::istringstream iss(methods_str);
	std::string method;
	bool get = false, post = false, del = false;

	while (iss >> method) {
		// Check if the method is strictly in the valid set
		if (valid_methods.find(method) == valid_methods.end()) {
			throw std::runtime_error("Invalid method in allowed_methods: " + method);
		}

		if (method == "GET") get = true;
		else if (method == "POST") post = true;
		else if (method == "DELETE") del = true;
	}

	// Ensure at least one valid method is set
	if (!get && !post && !del) {
		throw std::runtime_error("allowed_methods must contain at least one valid method");
	}

	// Set the validated methods in the LocationConfigFile object
	current_location.setMethods(get, post, del);
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
			std::string methods_str = line.substr(15); // Remove "allowed_methods "
			methods_str = methods_str.substr(0, methods_str.find(';')); // Remove trailing semicolon

			validateMethods(methods_str, current_location);

		// std::cout << "Parsed path: " << "|" << path << "|" << std::endl;
		} else if (line.find("autoindex ") == 0) {
			// Extract and clean the value after "autoindex "
			std::string value = trimWhiteSpace(line.substr(10));
			
			// Remove trailing semicolon if present
			if (!value.empty() && value.back() == ';') {
				value.pop_back();
			}

			// Validate and set autoindex using the existing function
			validateAutoIndex(value, current_location);
		} else if (line.find("upload_dir ") == 0) {
			current_location.setUploadDir(line.substr(11));
		} else if (line.find("index ") == 0) {
			current_location.setIndexFile(line.substr(6));
		} else {
			throw std::runtime_error("Invalid directive inside location block: " + line);
		}
	}
}


const std::vector<ServerConfigFile> ConfigParser::getServers() const {
	return _servers;
}
