#include "../../includes/ConfigParser/ConfigParser.hpp"
#include <iostream>
#include <stdexcept>
#include  <unistd.h>
//validateMethods
#include <set>
#include <sstream>
// #include <algorithm>
template void ConfigParser::validateMethods(const std::string&, ServerConfigFile&);
template void ConfigParser::validateMethods(const std::string&, LocationConfigFile&);
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

std::vector<std::string> ConfigParser::splitByWhitespace(const std::string& str) const {
	std::istringstream iss(str);
	std::vector<std::string> tokens;
	std::string token;
	while (iss >> token) { // Automatically handles multiple spaces/tabs
		tokens.push_back(token);
	}
	return tokens;
}

void ConfigParser::validateServerName(const std::string& name) {
	if (name.empty()) {
		throw std::runtime_error("Invalid server_name: Name cannot be empty.");
	}

	// Check first and last characters are alphanumeric
	if (!std::isalnum(name.front()) || !std::isalnum(name.back())) {
		throw std::runtime_error("Invalid server_name '" + name +
			"': Must start/end with alphanumeric character");
	}

	bool previous_is_dot = false;
	for (size_t i = 0; i < name.size(); ++i) {
		const char c = name[i];

		if (!std::isalnum(c) && c != '-' && c != '.') {
			throw std::runtime_error("Invalid server_name '" + name +
				"': Contains invalid character '" + std::string(1, c) + "'");
		}
		if (c == '.') {
			if (previous_is_dot) {
				throw std::runtime_error("Invalid server_name '" + name +
					"': Consecutive dots detected");
			}
			previous_is_dot = true;
		} else {
			previous_is_dot = false;
		}
	}
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
	// std::cout << "Final Bracket Count: " << bracket_count << "\n";

	if (bracket_count != 0) {
		throw std::runtime_error("Mismatched brackets in configuration file.");
	}
}

std::string ConfigParser::sanitizeLine(const std::string& line) const {
	// Check if the line ends with '{' (indicating a block start)
	if (!line.empty() && line.back() == '{') {
		return trimWhiteSpace(line);
	}

	if (line == "}") {
		return line;
	}
	// Find the position of the semicolon
	size_t semicolon_pos = line.find(';');
	if (semicolon_pos == std::string::npos) {
		throw std::runtime_error("Missing semicolon in the line: " + line);
	}

	// Remove everything after the semicolon
	std::string sanitized = line.substr(0, semicolon_pos);

	// Trim whitespace from the sanitized line
	return trimWhiteSpace(sanitized);
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



template <typename T>
void ConfigParser::validateMethods(const std::string& methods_str, T& config_object) {
	const std::set<std::string> valid_methods = {"GET", "POST", "DELETE", "PUT"};

	std::istringstream iss(methods_str);
	std::string method;
	bool get = false, post = false, del = false, put = false;

	while (iss >> method) {
		if (valid_methods.find(method) == valid_methods.end()) {
			throw std::runtime_error("Invalid method in allowed_methods: " + method);
		}

		if (method == "GET") get = true;
		else if (method == "POST") post = true;
		else if (method == "DELETE") del = true;
		else if (method == "PUT") put = true;
	}

	if (!get && !post && !del && !put) {
		throw std::runtime_error("allowed_methods must contain at least one valid method");
	}

	config_object.setMethods(get, post, del, put);
}

template <typename T>
void ConfigParser::handleCgiPath(const std::string& line, T& config_object) {
	std::vector<std::string> tokens = splitByWhitespace(line);

	if (tokens.size() != 3 || tokens[0] != "cgi_path") {
		throw ConfigParseError("Invalid cgi_pass format: " + line);
	}

	const std::string& ext = tokens[1];
	const std::string& path = tokens[2];

	validateCgiExtension(ext);
	validateExecutablePath(path);

	config_object.addCgiExtension(ext, path);
}

void ConfigParser::validateCgiExtension(const std::string& ext) {
	if (ext.empty() || ext[0] != '.' || ext.find_first_of(" \t\n") != std::string::npos) {
		throw ConfigParseError("Invalid CGI extension: " + ext);
	}
}

void ConfigParser::validateExecutablePath(const std::string& path) {
	if (path.empty() || path[0] != '/' || path.find("..") != std::string::npos) {
		throw ConfigParseError("Invalid CGI executable path: " + path);
	}
}

void ConfigParser::parseServerBlock(std::ifstream& file, ServerConfigFile& current_server, int& bracket_count) {
	std::string line;

	while (std::getline(file, line)) {
		line = trimWhiteSpace(line);

		if (line.empty() || line[0] == '#') {
			continue; // Skip empty lines or comments
		}

		line = sanitizeLine(line);

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
			// std::cout << "Parsed path: " << "|" << path << "|" << std::endl;

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
			std::string port_value = trimWhiteSpace(line.substr(7)); // Extract the value after "listen "

			// Remove trailing semicolon if present
			if (!port_value.empty() && port_value.back() == ';') {
				port_value.pop_back();
			}

			// Validate and set the port
			validatePort(port_value, current_server);
		} else if (line.find("server_name ") == 0) {
			// Extract everything after "server_name " and trim it
			std::string server_names_line = trimWhiteSpace(line.substr(12));

			// Validate that the line is not empty
			if (server_names_line.empty()) {
				throw std::runtime_error("Invalid directive: 'server_name' must be followed by one or more names.");
			}

			// Split the line into individual server names
			std::vector<std::string> server_names = splitByWhitespace(server_names_line);



			// Add each server name to the ServerConfigFile object
			for (const auto& name : server_names) {
				std::cout << "SERVER NAME : " << "|" << name << "|" << std::endl;
				validateServerName(name);
				current_server.addServerName(name); // Use addServerName from ServerConfigFile
			}
		} else if (line.find("cgi_path ") == 0) {
			handleCgiPath(line, current_server);
			handleCgiPath(line, current_server.setDefaultLocation());
		} else if (line.find("allowed_methods ") == 0) {
			//todo: fix this shit later
			std::string methods_str = trimWhiteSpace(line.substr(15)); // Extract the value after "allowed_methods "

			// Remove trailing semicolon if present
			if (!methods_str.empty() && methods_str.back() == ';') {
				methods_str.pop_back();
			}

			// Validate and set allowed methods for the server block
			validateMethods(methods_str, current_server);
			validateMethods(methods_str, current_server.setDefaultLocation());

			std::cout << "METHODSDSDS: " << methods_str << std::endl;

		} else if (line.find("root ") == 0) {
			std::string root_value = trimWhiteSpace(line.substr(5)); // Extract the value after "root "


			// Remove trailing semicolon if present
			if (!root_value.empty() && root_value.back() == ';') {
				root_value.pop_back();
			}

			//testing:
			// std::cout << "Parsed server root: " << "|" << root_value << "|" << std::endl;

			// Validate and set the root path
			validateRoot(root_value, "root", true); // true indicates this is a server block
			current_server.setRoot(root_value);
			current_server.setDefaultLocation().setRoot("/");
		} else if (line.find("error_page ") == 0) {
			int code = std::stoi(line.substr(11, 3)); // Extract error code
			std::string path = trimWhiteSpace(line.substr(15)); // Extract path
			current_server.addErrorPage(code, path);
		} else if (line.find("request_body_size ") == 0) {
			std::string size_value = trimWhiteSpace(line.substr(17)); // Extract the value after "request_body_size "
			// std::cout << "REAL SIZE HERE: " << size_value << std::endl;
			// Remove trailing semicolon if present
			if (!size_value.empty() && size_value.back() == ';') {
				size_value.pop_back();
			}


			// Validate and set client body size
			validateClientBodySize(size_value);
			current_server.setRequestBodySize(std::stoi(size_value));

		} else if (line.find("index ") == 0) {
			std::string index_value = trimWhiteSpace(line.substr(6)); // Extract index file name

			// Remove trailing semicolon if present
			if (!index_value.empty() && index_value.back() == ';') {
				index_value.pop_back();
			}

			// Call the shared validation method
			validateIndex(index_value, current_server);
			validateIndex(index_value, current_server.setDefaultLocation());

		} else if (line.find("autoindex ") == 0) {

			std::string autoindex_value = trimWhiteSpace(line.substr(10));

			// Remove trailing semicolon if present
			if (!autoindex_value.empty() && autoindex_value.back() == ';') {
				autoindex_value.pop_back();
			}

			// Validate and set for server
			if (autoindex_value == "on") {
				current_server.setAutoIndex(true);
				current_server.setDefaultLocation().setAutoIndex(true);
			} else if (autoindex_value == "off") {
				current_server.setAutoIndex(false);
				current_server.setDefaultLocation().setAutoIndex(false);
			} else {
				throw std::runtime_error("Invalid autoindex value: " + autoindex_value + ". Must be 'on' or 'off'");
			}
				std::cout << "DEFAULT_LOCATION_AUTOINDEX: " << "|" << current_server.getDefaultLocation().getAutoIndex() << "|" << std::endl;
				std::cout << "DEFAULT_LOCATION_AUTOINDEX: " << "|" << current_server.getDefaultLocation().getAutoIndex() << "|" << std::endl;
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

// void ConfigParser::validateMethods(const std::string& methods_str, LocationConfigFile& current_location) {
// 	// Define the valid methods
// 	const std::set<std::string> valid_methods = {"GET", "POST", "DELETE"};

// 	// Tokenize the methods string
// 	std::istringstream iss(methods_str);
// 	std::string method;
// 	bool get = false, post = false, del = false;

// 	while (iss >> method) {
// 		// Check if the method is strictly in the valid set
// 		if (valid_methods.find(method) == valid_methods.end()) {
// 			throw std::runtime_error("Invalid method in allowed_methods: " + method);
// 		}

// 		if (method == "GET") get = true;
// 		else if (method == "POST") post = true;
// 		else if (method == "DELETE") del = true;
// 	}

// 	// Ensure at least one valid method is set
// 	if (!get && !post && !del) {
// 		throw std::runtime_error("allowed_methods must contain at least one valid method");
// 	}

// 	// Set the validated methods in the LocationConfigFile object
// 	current_location.setMethods(get, post, del);
// }





template <typename T>
void ConfigParser::validateIndex(const std::string& value, T& config_object) {
	// Define a regex pattern for valid filenames (with or without an extension)
	std::regex index_regex(R"(^[a-zA-Z0-9_\-]+(\.[a-zA-Z0-9_\-]+)?$)");

	// Check for spaces or empty values
	if (value.empty() || value.find(' ') != std::string::npos) {
		throw std::runtime_error("Invalid index file name: " + value + ". File name must not contain spaces.");
	}

	// Validate the file name against the regex
	if (!std::regex_match(value, index_regex)) {
		throw std::runtime_error("Invalid index file name: " + value + ". File name must contain only letters, numbers, '_', '-', and optionally a single '.'");
	}

	// Set the validated index file in the appropriate config object
	config_object.setIndexFile(value);
}

void ConfigParser::validatePort(const std::string& line, ServerConfigFile& current_server) {
	// Ensure no spaces or second words
	size_t space_pos = line.find(' ');
	if (space_pos != std::string::npos) {
		throw std::runtime_error("Invalid listen directive: '" + line + "'. Port must not contain spaces or additional words.");
	}

	// Convert to integer and validate range
	int port = std::stoi(line);
	if (port < 1 || port > 65535) {
		throw std::runtime_error("Invalid port number: " + std::to_string(port) + ". Port must be between 1 and 65535.");
	}

	// Set the validated port in the server configuration
	current_server.setPort(port);
}

void ConfigParser::validateRoot(const std::string& value, const std::string& directive_name, bool is_server_block) {
	// Check if root is empty in server block
	if (is_server_block && value.empty()) {
		throw std::runtime_error("Invalid " + directive_name + " directive: Root cannot be empty in server block.");
	}

	// For server block, ensure it starts with "/www/"
	if (is_server_block && value.substr(0, 5) != "/www/") {
		throw std::runtime_error("Invalid " + directive_name + " directive: '" + value +
			"'. Root in server block must start with '/www/'.");
	}

	// Check if the path starts with '/'
	if (!value.empty() && value[0] != '/') {
		throw std::runtime_error("Invalid " + directive_name + " directive: '" + value +
			"'. Path must start with '/'.");
	}

	// Check if the path ends with '/' (but allow just "/")
	if (value.size() > 1 && value.back() == '/') {
		throw std::runtime_error("Invalid " + directive_name + " directive: '" + value +
			"'. Path must not end with '/'.");
	}

	// Check for invalid characters and ensure no dots
	for (char ch : value) {
		if (!std::isalnum(ch) && ch != '/' && ch != '_' && ch != '-') {
			throw std::runtime_error("Invalid " + directive_name + " directive: '" + value +
				"'. Path contains invalid characters.");
		}
		if (ch == '.') {
			throw std::runtime_error("Invalid " + directive_name + " directive: '" + value +
				"'. Path must not contain dots.");
		}
	}

	// Check for spaces or multiple words
	if (value.find(' ') != std::string::npos) {
		throw std::runtime_error("Invalid " + directive_name + " directive: '" + value +
			"'. Path must not contain spaces or multiple words.");
	}
}



void ConfigParser::parseLocationBlock(std::ifstream& file, LocationConfigFile& current_location, int& bracket_count) {
	std::string line;

	while (std::getline(file, line)) {
		line = trimWhiteSpace(line);

		if (line.empty() || line[0] == '#') {
			continue; // Skip empty lines or comments
		}

		line = sanitizeLine(line);

		// Handle end of location block
		if (line == "}") {
			bracket_count--; // Decrement for closing bracket of location block

			// If no root is set, default it to the location's path
			if (current_location.getRoot().empty()) { // Assuming getUploadDir() is equivalent to root
				current_location.setRoot(current_location.getPath());
			}
			// std::cout << "Location root: " << "|" << current_location.getRoot() << "|" << std::endl;
			return; // End parsing this location block
		}

		// Parse key-value pairs inside the location block
		if (line.find("allowed_methods ") == 0) {
			std::string methods_str = trimWhiteSpace(line.substr(15)); // Extract the value after "allowed_methods "

			// Remove trailing semicolon if present
			if (!methods_str.empty() && methods_str.back() == ';') {
				methods_str.pop_back();
			}

			// Validate and set allowed methods for the location block
			validateMethods(methods_str, current_location);

		} else if (line.find("cgi_path ") == 0) {
			handleCgiPath(line, current_location);
		} else if (line.find("autoindex ") == 0) {
			std::string value = trimWhiteSpace(line.substr(10)); // Extract value after "autoindex "

			// Remove trailing semicolon if present
			if (!value.empty() && value.back() == ';') {
				value.pop_back();
			}

			validateAutoIndex(value, current_location);

		} else if (line.find("root ") == 0) {
			std::string root_value = trimWhiteSpace(line.substr(5)); // Extract value after "root "

			// Remove trailing semicolon if present
			if (!root_value.empty() && root_value.back() == ';') {
				root_value.pop_back();
			}

			validateRoot(root_value, "root", false); // false indicates this is a location block
			current_location.setRoot(root_value);

		} else if (line.find("index ") == 0) {
			std::string index_value = trimWhiteSpace(line.substr(6)); // Extract index file name

			// Remove trailing semicolon if present
			if (!index_value.empty() && index_value.back() == ';') {
				index_value.pop_back();
			}

			validateIndex(index_value, current_location);

		} else {
			throw std::runtime_error("Invalid directive inside location block: " + line);
		}
	}
}

void ConfigParser::validateClientBodySize(const std::string& value) {
	// Debug: Print the raw value
	// std::cout << "Raw request_body_size value: " << value << std::endl;

	// Check if the value starts with a '-' (negative number)
	if (!value.empty() && value[0] == '-') {
		throw std::runtime_error("Invalid request_body_size value: " + value + ". Negative values are not allowed.");
	}

	// Ensure the value is numeric
	for (char ch : value) {
		if (!std::isdigit(ch)) {
			throw std::runtime_error("Invalid request_body_size value: " + value + ". Must be a non-negative integer.");
		}
	}

	// Convert to integer and check range
	int size = std::stoi(value);

	// Check for maximum allowed size (e.g., 1 GB = 1073741824 bytes)
	const int MAX_SIZE = 1073741824; // 1 GB
	if (size > MAX_SIZE) {
		throw std::runtime_error("Invalid request_body_size value: " + value + ". Maximum allowed is " +
			std::to_string(MAX_SIZE) + " bytes.");
	}
}




const std::vector<ServerConfigFile> ConfigParser::getServers() const {
	return _servers;
}


