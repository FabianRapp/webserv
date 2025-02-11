#include "../../includes/ConfigParser/ConfigParser.hpp"
#include <iostream>
#include <stdexcept>
#include  <unistd.h>
//validateMethods
// #include <set>


///remove me:
#include <colors.h>
// #include <algorithm>

// Default constructor
ConfigParser::ConfigParser() {}

// Constructor with file parsing
ConfigParser::ConfigParser(const std::string& config_file) {
	parseFile(config_file);
}


void ConfigParser::validateServerConfig(const ServerConfigFile& server) const {
	if (server.getPort() == 0) {
		throw (ConfigParseError("ERROR: Mandatory 'listen' option is not set in the server block!"));
	}
	if (server.getRoot().empty()) {
		throw (ConfigParseError("ERROR: Mandatory 'root' option is not set in the server block!"));
	}
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
		throw (ConfigParseError("ERROR: 'server_name' cannot be empty!"));
	}

	// Check first and last characters are alphanumeric
	if (!std::isalnum(name.front()) || !std::isalnum(name.back())) {
		throw (ConfigParseError("ERROR: server_name '" + name + "' must start/end with alphanumeric character!"));
	}

	bool previous_is_dot = false;
	for (size_t i = 0; i < name.size(); ++i) {
		const char c = name[i];

		if (!std::isalnum(c) && c != '-' && c != '.' && c != '_') {
			throw (ConfigParseError("ERROR: server_name '" + name + "' contains invalid character: '" + std::string(1, c) + "'!"));
		}
		if (c == '.') {
			if (previous_is_dot) {
				throw (ConfigParseError("ERROR: server_name '" + name + "' contains consecutive dots!"));
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
		throw (ConfigParseError("ERROR: Invalid configuration file!"));
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
				throw (ConfigParseError("ERROR: Nested server blocks!"));
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
			throw (ConfigParseError("ERROR: Unmatched closing bracket!"));
		}
	}

	// Debug: Final bracket count
	// std::cout << "Final Bracket Count: " << bracket_count << "\n";

	if (bracket_count != 0) {
		throw (ConfigParseError("ERROR: Mismatched brackets in configuration file!"));
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
		throw (ConfigParseError("ERROR: Missing semicolon in the line: " + line));
	}

	// Remove everything after the semicolon
	std::string sanitized = line.substr(0, semicolon_pos);

	// Trim whitespace from the sanitized line
	return trimWhiteSpace(sanitized);
}

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







void ConfigParser::validateCgiExtension(const std::string& ext) {
	if (ext.empty() || ext[0] != '.' || ext.find_first_of(" \t\n") != std::string::npos) {
		throw ConfigParseError("ERROR: Invalid CGI extension: " + ext);
	}
}

void ConfigParser::validateExecutablePath(const std::string& path) {
	if (path.empty() || path[0] != '/' || path.find("..") != std::string::npos) {
		throw ConfigParseError("ERROR: Invalid CGI executable path: " + path);
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
					throw ConfigParseError("ERROR: Invalid location path: " + path);
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
			validateServerConfig(current_server);
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
				throw ConfigParseError("ERROR: server_name cannot be empty!");
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

		} else if (line.find("root ") == 0) {
			std::string root_value = trimWhiteSpace(line.substr(5)); // Extract the value after "root "

			// Remove trailing semicolon if present
			if (!root_value.empty() && root_value.back() == ';') {
				root_value.pop_back();
			}

			// Validate and set the root path
			validateRoot(root_value, "root", true); // true indicates this is a server block
			current_server.setRoot(root_value);
			current_server.setDefaultLocation().setRoot("/");

		} else if (line.find("error_page ") == 0) {
			// Extract the part after "error_page "
			std::string error_directive = line.substr(11);
			std::vector<std::string> tokens = splitByWhitespace(error_directive);

			if (tokens.size() != 2) {
				throw ConfigParseError("ERROR: error_line invalid. Valid example: 'error_line 404 /404.html'. Your line: 'error_line " + error_directive + "'");
			}

			// The last token is the path
			std::string path = tokens.back();
			tokens.pop_back(); // Now tokens contains only the error code

			// Validate error code (only one allowed)
			for (const std::string& code_str : tokens) {
				// Numeric check
				if (code_str.empty() || code_str.find_first_not_of("0123456789") != std::string::npos) {
					throw ConfigParseError("ERROR: Invalid error_page error code '" + code_str + "'. Value must be numeric!");
				}

				int code = std::stoi(code_str);
				if (code < 100 || code > 599) {
					throw ConfigParseError("ERROR: Error code " + code_str + " is out of range (100-599)");
				}

				// Path processing and error page addition
				if (!path.empty() && path.front() == '/') {
					path = path.substr(1);
				}

				std::string root = current_server.getRoot();
				std::string full_path = root.back() == '/' ? root + path : root + "/" + path;

				current_server.addErrorPage(code, full_path);
				current_server.getErrorPages().printErrorPages();
			}

		} else if (line.find("request_body_size ") == 0) {
			std::string size_value = trimWhiteSpace(line.substr(17)); // Extract the value after "request_body_size "
			// std::cout << "REAL SIZE HERE: " << size_value << std::endl;
			// Remove trailing semicolon if present
			if (!size_value.empty() && size_value.back() == ';') {
				size_value.pop_back();
			}


			// Validate and set client body size
			validateClientBodySize(size_value, current_server);
			validateClientBodySize(size_value, current_server.setDefaultLocation());

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

			if (!autoindex_value.empty() && autoindex_value.back() == ';') {
				autoindex_value.pop_back();
			}

			validateAutoIndex(autoindex_value, current_server);
			validateAutoIndex(autoindex_value, current_server.setDefaultLocation());
		} else {
			throw ConfigParseError("ERROR: Invalid line inside server block: " + line);
		}
	}
}



void ConfigParser::validatePort(const std::string& line, ServerConfigFile& current_server) {
	// Ensure no spaces or second words
	size_t space_pos = line.find(' ');
	if (space_pos != std::string::npos) {
		throw ConfigParseError("ERROR: Invalid listen directive: '" + line + "'. Port must not contain spaces or additional words.");
	}

	// Convert to integer and validate range
	int port = std::stoi(line); //todo: unsanfe here?
	if (port < 1 || port > 65535) {
		throw ConfigParseError("ERROR: Invalid port number: " + std::to_string(port) + ". Port must be between 1 and 65535.");
	}

	// Set the validated port in the server configuration
	current_server.setPort(port);
}

void ConfigParser::validateRoot(const std::string& value, const std::string& directive_name, bool is_server_block) {
	// Check if root is empty in server block
	if (is_server_block && value.empty()) {
		throw ConfigParseError("ERROR: Invalid " + directive_name + " directive: Root cannot be empty in server block.");
	}

	// For server block, ensure it starts with "/www/"
	if (is_server_block && value.substr(0, 5) != "/www/") {
		throw ConfigParseError("ERROR: Invalid " + directive_name + " directive: '" + value + "'. Root in server block must start with '/www/'.");
	}

	// Check if the path starts with '/'
	if (!value.empty() && value[0] != '/') {
		throw ConfigParseError("ERROR: Invalid " + directive_name + " directive: '" + value + "'. Path must start with '/'.");
	}

	// Check if the path ends with '/' (but allow just "/")
	if (value.size() > 1 && value.back() == '/') {
		throw ConfigParseError("ERROR: Invalid " + directive_name + " directive: '" + value + "'. Path must not end with '/'.");
	}

	// Check for invalid characters and ensure no dots
	for (char ch : value) {
		if (!std::isalnum(ch) && ch != '/' && ch != '_' && ch != '-') {
			throw ConfigParseError("ERROR: Invalid " + directive_name + " directive: '" + value + "'. Path contains invalid characters.");
		}
		if (ch == '.') {
			throw ConfigParseError("ERROR: Invalid " + directive_name + " directive: '" + value +"'. Path must not contain dots.");
		}
	}

	// Check for spaces or multiple words
	if (value.find(' ') != std::string::npos) {
		throw ConfigParseError("ERROR: Invalid " + directive_name + " directive: '" + value + "'. Path must not contain spaces or multiple words.");
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
		} else if (line.find("redirection ") == 0) {
			std::string redir_str = trimWhiteSpace(line.substr(12)); // Extract the value after "redirection "

			if (!redir_str.empty() && redir_str.back() == ';') {
				redir_str.pop_back();
			}
			current_location.setIsRedir(true);
			current_location.setRedirection(redir_str);
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
		} else if (line.find("request_body_size ") == 0) {
			std::string size_value = trimWhiteSpace(line.substr(17)); // Extract the value after "request_body_size "
			// std::cout << "REAL SIZE HERE: " << size_value << std::endl;
			// Remove trailing semicolon if present
			if (!size_value.empty() && size_value.back() == ';') {
				size_value.pop_back();
			}

			// Validate and set client body size
			validateClientBodySize(size_value, current_location);

		} else if (line.find("index ") == 0) {
			std::string index_value = trimWhiteSpace(line.substr(6)); // Extract index file name

			// Remove trailing semicolon if present
			if (!index_value.empty() && index_value.back() == ';') {
				index_value.pop_back();
			}

			validateIndex(index_value, current_location);

		} else {
			throw (ConfigParseError("ERROR: Invalid directive inside location block: " + line));
		}
	}
}






const std::vector<ServerConfigFile> ConfigParser::getServers() const {
	return _servers;
}


