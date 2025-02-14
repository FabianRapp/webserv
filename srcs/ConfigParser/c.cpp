#include "../../includes/ConfigParser/ConfigParser.hpp"
#include <iostream>
#include <stdexcept>
#include <unistd.h>
#include <colors.h>

ConfigParser::ConfigParser() {}

void ConfigParser::checkAndSetServerOptionDubs(const std::string& option) {
	if (dubs_serv_set.find(option) != dubs_serv_set.end()) {
		throw ConfigParseError("ERROR: Duplicate option in server block'"
			+ option + "' found!");
	}
	dubs_serv_set.insert(option);
}

void ConfigParser::checkAndSetLocationOptionDubs(const std::string& option) {
	if (dubs_loc_set.find(option) != dubs_loc_set.end()) {
		throw ConfigParseError("ERROR: Duplicate option in location block'"
			+ option + "' found!");
	}
	dubs_loc_set.insert(option);
}

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

std::string ConfigParser::trimWhiteSpaceEdges(const std::string& str) const {
	const std::string whitespace_chars = " \t\n\r\f\v";

	size_t leading_pos = str.find_first_not_of(whitespace_chars);
	size_t trailing_pos = str.find_last_not_of(whitespace_chars);

	if (leading_pos == std::string::npos) {
		return "";
	}
	return str.substr(leading_pos, trailing_pos - leading_pos + 1);
}

std::string ConfigParser::sanitizeLine(const std::string& line) const {
	if (!line.empty() && line.back() == '{') {
		return trimWhiteSpaceEdges(line);
	}

	if (line == "}") {
		return line;
	}

	size_t semicolon_pos = line.find(';');
	if (semicolon_pos == std::string::npos) {
		throw (ConfigParseError("ERROR: Missing semicolon in the line: " + line));
	}

	// remove everything after semicolon
	std::string sanitized = line.substr(0, semicolon_pos);

	return trimWhiteSpaceEdges(sanitized);
}

std::vector<std::string> ConfigParser::splitByWhitespace(const std::string& str) const {
	std::istringstream iss(str);
	std::vector<std::string> tokens;
	std::string token;

	while (iss >> token) {
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
		throw (ConfigParseError("ERROR: server_name '" + name
			+ "' must start/end with alphanumeric character!"));
	}

	bool previous_is_dot = false;
	for (size_t i = 0; i < name.size(); ++i) {
		const char c = name[i];

		if (!std::isalnum(c) && c != '-' && c != '.' && c != '_') {
			throw (ConfigParseError("ERROR: server_name '" + name
				+ "' contains invalid character: '" + std::string(1, c) + "'!"));
		}
		if (c == '.') {
			if (previous_is_dot) {
				throw (ConfigParseError("ERROR: server_name '"
					+ name + "' contains consecutive dots!"));
			}
			previous_is_dot = true;
		} else {
			previous_is_dot = false;
		}
	}
}

void ConfigParser::validatePort(const std::string& line, ServerConfigFile& current_server) {

	size_t space_pos = line.find(' ');
	if (space_pos != std::string::npos) {
		throw ConfigParseError("ERROR: Invalid listen option: '" + line
			+ "'. Port must not contain spaces or additional words.");
	}

	if (line.find_first_not_of("0123456789") != std::string::npos) {
		throw ConfigParseError("ERROR: Invalid listen option: '"+ line +
			"'. Port must be numeric.");
	}
	int port = std::stoi(line);

	if (port < 2 || port > 65535) {
		throw ConfigParseError("ERROR: Invalid port number: "
			+ std::to_string(port) + ". Port must be between 2 and 65535.");
	}

	current_server.setPort(port);
}

void ConfigParser::validateRoot(const std::string& value, bool is_server_block) {
	// Check if root is empty in server block
	if (is_server_block && value.empty()) {
		throw ConfigParseError("ERROR: Invalid root configuration:"
			"Root cannot be empty in server block.");
	}

	// For server block, ensure it starts with "/www/"
	if (is_server_block && value.substr(0, 5) != "/www/") {
		throw ConfigParseError("ERROR: Invalid root configuration: '" + value
			+ "'. Root in server block must start with '/www/'.");
	}

	// Check if the path starts with '/'
	if (!value.empty() && value[0] != '/') {
		throw ConfigParseError("ERROR: Invalid root configuration: '" + value
			+ "'. Path must start with '/'.");
	}

	// Check if the path ends with '/' (but allow just "/")
	if (value.size() > 1 && value.back() == '/') {
		throw ConfigParseError("ERROR: Invalid root configuration: '" + value
			+ "'. Path must not end with '/'.");
	}

	// Check for invalid characters and ensure no dots
	for (char ch : value) {
		if (!std::isalnum(ch) && ch != '/' && ch != '_' && ch != '-') {
			throw ConfigParseError("ERROR: Invalid root configuration: '"
				+ value + "'. Path contains invalid characters.");
		}
		if (ch == '.') {
			throw ConfigParseError("ERROR: Invalid root configuration: '"
				+ value + "'. Path must not contain dots.");
		}
	}

	// Check for spaces or multiple words
	if (value.find(' ') != std::string::npos) {
		throw ConfigParseError("ERROR: Invalid root configuration: '" + value
			+ "'. Path must not contain spaces or multiple words.");
	}
}

bool ConfigParser::isValidLocationPath(const std::string& path) const {
	if (path.empty() || path[0] != '/') {
		return false;
	}

	if (path.size() > 1 && path.back() == '/') {
		return false;
	}

	for (char ch : path) {
		if (!std::isalnum(ch) && ch != '/' && ch != '_' && ch != '-') {
			return false;
		}
	}

	if (path.find("//") != std::string::npos) {
		return false;
	}

	return true;
}

void ConfigParser::errorPageValidation(const std::string& values, ServerConfigFile& current_server) {
	// std::string error_option = line.substr(11);
	// std::cout << "error_option: " << error_option << std::endl;
	std::vector<std::string> tokens = splitByWhitespace(values);

	if (tokens.size() != 2) {
		throw ConfigParseError("ERROR: Invalid error_page directive."
			"Valid example: 'error_page 404 /404.html'. Your line: 'error_page " + values + "'");
	}

	std::string path = tokens.back();
	tokens.pop_back();

	if (path.empty() || path[0] != '/') {
		throw ConfigParseError("ERROR: Invalid error_page path '" + path
			+ "'. Path must start with '/'.");
	}
	std::cout << "Path: " << path << std::endl;

	for (size_t i = 1; i < path.size(); ++i) {
		char ch = path[i];
		if (!std::isalnum(ch) && ch != '/' && ch != '-' && ch != '_' && ch != '.') {
			throw ConfigParseError("ERROR: Invalid error_page path '" + path
				+ "'. Path can only contain letters, numbers, '-', '_', and '/'.");
		}
	}

	if (path.find("..") != std::string::npos) {
		throw ConfigParseError("ERROR: Invalid error_page path '" + path
			+ "'. Path must not contain '..'.");
	} else if (path.find("//") != std::string::npos) {
		throw ConfigParseError("ERROR: Invalid error_page path '" + path
			+ "'. Path must not contain '//'.");
	}

	std::string code_str = tokens[0];

	if (code_str.empty() || code_str.find_first_not_of("0123456789") != std::string::npos) {
		throw ConfigParseError("ERROR: Invalid error_page error code '" + code_str + "'. Value must be numeric!");
	}

	int code;
	try {
		code = std::stoi(code_str);
	} catch (const std::invalid_argument& e) {
		throw ConfigParseError("ERROR: Invalid error_page error code '" + code_str + "'. Value must be a valid integer.");
	} catch (const std::out_of_range& e) {
		throw ConfigParseError("ERROR: Invalid error_page error code '" + code_str + "'. Value is out of range.");
	}

	if (code < 100 || code > 599) {
		throw ConfigParseError("ERROR: Error code " + code_str + " is out of range (100-599)");
	}


	std::string root = current_server.getRoot();
	if (root.empty()) {
		throw ConfigParseError("Root option has to be set before error_code!");
	}

	// make a full path to error page
	std::string full_path;
	if (root.back() == '/') {
		full_path = root + path.substr(1);
	} else {
		full_path = root + path;
	}

	for (const std::string& code_str : tokens) {
		int code;
		try {
			code = std::stoi(code_str);
		} catch (const std::invalid_argument& e) {
			throw ConfigParseError("ERROR: Invalid error_page error code '" + code_str + "'. Value must be a valid integer.");
		} catch (const std::out_of_range& e) {
			throw ConfigParseError("ERROR: Invalid error_page error code '" + code_str + "'. Value is out of range.");
		}
		current_server.addErrorPage(code, full_path);
	}

	// Debug: Print error pages
	current_server.getErrorPages().printErrorPages();
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
	std::cout << "path: " << path << std::endl;
	if (access(path.c_str(), X_OK) != 0) {
		throw ConfigParseError("ERROR: CGI binary not executable: " + path);
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
		line = trimWhiteSpaceEdges(line);

		if (line.empty() || line[0] == '#') {
			continue ;
		}
		// server block starts here
		else if (line == "server {") {
			if (in_server_block) {
				throw (ConfigParseError("ERROR: Nested server blocks!"));
			}

			in_server_block = true;
			current_server = ServerConfigFile();
			bracket_count++;

			parseServerBlock(file, current_server, bracket_count);

			_servers.push_back(current_server);
			in_server_block = false;
			continue;
		} else if (line == "}") {
			throw (ConfigParseError("ERROR: Something is wrong with brackets!"));
		} else {
			throw (ConfigParseError("ERROR: Code outside of server block!"));
		}
	}
	if (bracket_count != 0) {
		throw (ConfigParseError("ERROR: Something is wrong with brackets!"));
	}
}

void ConfigParser::parseServerBlock(std::ifstream& file, ServerConfigFile& current_server, int& bracket_count) {
	dubs_serv_set.clear();
	location_paths.clear();
	std::string line;

	while (std::getline(file, line)) {
		line = trimWhiteSpaceEdges(line);

		if (line.empty() || line[0] == '#') {
			continue;
		}

		line = sanitizeLine(line);

		if (line.find("location") == 0 && line.back() == '{') {
			size_t location_start = 8;
			size_t location_end = line.find_last_of('{') - 1;
			std::string path = trimWhiteSpaceEdges(line.substr(location_start, location_end - location_start));

			if (!isValidLocationPath(path)) {
				throw ConfigParseError("ERROR: Invalid location path: " + path);
			}
			// check for dup location blocks
			else if (location_paths.find(path) != location_paths.end()) {
				throw ConfigParseError("ERROR: Duplicate location path found: " + path);
			}
			location_paths.insert(path);
			// Debug: Print the parsed path
			// std::cout << "Parsed path: " << "|" << path << "|" << std::endl;

			bracket_count++; // increment before location block starts
			LocationConfigFile current_location;
			current_location.setPath(path);
			parseLocationBlock(file, current_location, bracket_count);
			current_server.addLocation(current_location);
			continue;
		}
		else if (line == "}") {
			bracket_count--;
			validateServerConfig(current_server);
			return; // ending parsing for this server block
		}

		// Parse key-value pairs inside the server block
		if (!line.empty() && line.back() == ';') {
			line.pop_back(); // Remove trailing semicolon
		}

		size_t	sep = line.find(" ");
		if (sep == std::string::npos) {
			throw ConfigParseError("ERROR: No space after option");
		}
		std::string	option = line.substr(0, sep);
		if ("error_page" != option && "cgi_path" != option) {
			checkAndSetServerOptionDubs(option);
		}
		std::string	single_value = trimWhiteSpaceEdges(line.substr(sep));
		if (line.find("listen ") == 0) {
			//checkAndSetServerOptionDubs("listen");
			// std::string port_value = trimWhiteSpaceEdges(line.substr(7));

			if (!single_value.empty() && single_value.back() == ';') {
				single_value.pop_back();
			}

			validatePort(single_value, current_server);
		} else if (line.find("server_name ") == 0) {
			//checkAndSetServerOptionDubs("server_name");
			// std::string server_names_line = trimWhiteSpaceEdges(line.substr(12));

			if (single_value.empty()) {
				throw ConfigParseError("ERROR: server_name cannot be empty!");
			}

			std::vector<std::string> server_names = splitByWhitespace(single_value);

			for (const auto& name : server_names) {
				validateServerName(name);
				current_server.addServerName(name);
			}
		} else if (line.find("cgi_path ") == 0) {
			// checkAndSetServerOptionDubs("cgi_path");
			handleCgiPath(line, current_server);
			handleCgiPath(line, current_server.setDefaultLocation());
		} else if (line.find("allowed_methods ") == 0) {
			//checkAndSetServerOptionDubs("allowed_methods");
			//todo: fix this shit later
			// std::string methods_str = trimWhiteSpaceEdges(line.substr(15));

			if (!single_value.empty() && single_value.back() == ';') {
				single_value.pop_back();
			}

			// Validate and set allowed methods for the server block
			validateMethods(single_value, current_server);
			validateMethods(single_value, current_server.setDefaultLocation());

		} else if (line.find("root ") == 0) {
			//checkAndSetServerOptionDubs("root");
			// std::string root_value = trimWhiteSpaceEdges(line.substr(5)); // Extract the value after "root "

			if (!single_value.empty() && single_value.back() == ';') {
				single_value.pop_back();
			}

			validateRoot(single_value, true); // true for server block
			current_server.setRoot(single_value);
			current_server.setDefaultLocation().setRoot("/");

		} else if (line.find("error_page ") == 0) {

				errorPageValidation(single_value, current_server);


		} else if (line.find("request_body_size ") == 0) {
			//checkAndSetServerOptionDubs("request_body_size");
			// std::string size_value = trimWhiteSpaceEdges(line.substr(17));

			if (!single_value.empty() && single_value.back() == ';') {
				single_value.pop_back();
			}

			validateClientBodySize(single_value, current_server);
			validateClientBodySize(single_value, current_server.setDefaultLocation());

		} else if (line.find("index ") == 0) {
			//checkAndSetServerOptionDubs("index");
			// std::string index_value = trimWhiteSpaceEdges(line.substr(6));

			if (!single_value.empty() && single_value.back() == ';') {
				single_value.pop_back();
			}

			validateIndex(single_value, current_server);
			validateIndex(single_value, current_server.setDefaultLocation());

		} else if (line.find("autoindex ") == 0) {
			//checkAndSetServerOptionDubs("autoindex");
			// std::string autoindex_value = trimWhiteSpaceEdges(line.substr(10));

			if (!single_value.empty() && single_value.back() == ';') {
				single_value.pop_back();
			}

			validateAutoIndex(single_value, current_server);
			validateAutoIndex(single_value, current_server.setDefaultLocation());
		} else {
			throw ConfigParseError("ERROR: Invalid line inside server block: " + line);
		}
	}
}

void ConfigParser::parseLocationBlock(std::ifstream& file, LocationConfigFile& current_location, int& bracket_count) {
	dubs_loc_set.clear();
	std::string line;

	while (std::getline(file, line)) {
		// line = trimWhiteSpaceEdges(line);

		if (line.empty() || line[0] == '#') {
			continue;
		}
		size_t	sep = line.find(" ");
		if (sep == std::string::npos) {
			throw ConfigParseError("ERROR: No space after option");
		}
		std::string	option = line.substr(0, sep);
		if ("error_page" != option && "cgi_path" != option) {
			checkAndSetServerOptionDubs(option);
		}
		std::string	single_value = trimWhiteSpaceEdges(line.substr(sep));

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
			checkAndSetLocationOptionDubs("allowed_methods");
			std::string methods_str = trimWhiteSpaceEdges(line.substr(15));

			if (!methods_str.empty() && methods_str.back() == ';') {
				methods_str.pop_back();
			}

			// Validate and set allowed methods for the location block
			validateMethods(methods_str, current_location);

		} else if (line.find("cgi_path ") == 0) {
			// checkAndSetLocationOptionDubs("cgi_path");
			handleCgiPath(line, current_location);

		} else if (line.find("redirection ") == 0) {
			checkAndSetLocationOptionDubs("redirection");
			std::string redir_str = trimWhiteSpaceEdges(line.substr(12));

			if (!redir_str.empty() && redir_str.back() == ';') {
				redir_str.pop_back();
			}
			current_location.setIsRedir(true);
			current_location.setRedirection(redir_str);
		} else if (line.find("autoindex ") == 0) {
			checkAndSetLocationOptionDubs("autoindex");
			std::string autoindex_value = trimWhiteSpaceEdges(line.substr(10));

			if (!autoindex_value.empty() && autoindex_value.back() == ';') {
				autoindex_value.pop_back();
			}

			validateAutoIndex(autoindex_value, current_location);

		} else if (line.find("root ") == 0) {
			checkAndSetLocationOptionDubs("root");
			std::string root_value = trimWhiteSpaceEdges(line.substr(5));

			if (!root_value.empty() && root_value.back() == ';') {
				root_value.pop_back();
			}

			validateRoot(root_value, false); // false for location block
			current_location.setRoot(root_value);
		} else if (line.find("request_body_size ") == 0) {
			checkAndSetLocationOptionDubs("request_body_size");
			std::string size_value = trimWhiteSpaceEdges(line.substr(17));

			if (!size_value.empty() && size_value.back() == ';') {
				size_value.pop_back();
			}

			validateClientBodySize(size_value, current_location);

		} else if (line.find("index ") == 0) {
			checkAndSetLocationOptionDubs("index");
			std::string index_value = trimWhiteSpaceEdges(line.substr(6));

			if (!index_value.empty() && index_value.back() == ';') {
				index_value.pop_back();
			}

			validateIndex(index_value, current_location);

		} else {
			throw (ConfigParseError("ERROR: Invalid option inside location block: " + line));
		}
	}
}

const std::vector<ServerConfigFile> ConfigParser::getServers() const {
	return _servers;
}


