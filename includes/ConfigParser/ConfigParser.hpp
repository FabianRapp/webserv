#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <vector>
#include <string>
#include <fstream>
#include <set>
#include <unordered_set>
#include <sstream>
#include "ServerConfigFile.hpp"
#include "LocationConfigFile.hpp"
#include <colors.h>

#ifdef __linux__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#include <regex>
#pragma GCC diagnostic pop
#else
#include <regex>
#endif //__linux__

class ConfigParseError: public std::runtime_error{
public:
	ConfigParseError(const std::string& msg): std::runtime_error(msg){}
};

class ConfigParser {
private:
	std::unordered_set<std::string> dubs_serv_set;
	std::unordered_set<std::string> dubs_loc_set;
	std::unordered_set<std::string> location_paths;
	void checkAndSetServerOptionDubs(const std::string& option);
	void checkAndSetLocationOptionDubs(const std::string& option);
	std::vector<ServerConfigFile> _servers;
	void validateServerConfig(const ServerConfigFile& server) const;
	std::string trimWhiteSpaceEdges(const std::string& str) const;
	std::string sanitizeLine(const std::string& line) const;
	std::vector<std::string> splitByWhitespace(const std::string& str) const;
	void validateServerName(const std::string& value, ServerConfigFile& server);
	void validatePort(const std::string& line, ServerConfigFile& current_server);
	void validateRoot(const std::string& value, bool is_server_block);
	bool isValidLocationPath(const std::string& path) const;
	void errorPageValidation(const std::string& line, ServerConfigFile& current_server);
	void validateCgiExtension(const std::string& ext);
	void validateExecutablePath(const std::string& path);
	void parseFile(const std::string& config_file);
	void parseServerBlock(std::ifstream& file, ServerConfigFile& current_server, int& bracket_count);
	void handleServerOptions(const std::string& option, const std::string& value, ServerConfigFile& current_server);
	void parseLocationBlock(std::ifstream& file, LocationConfigFile& current_location, int& bracket_count);
	void handleLocationOption(const std::string& line, const std::string& single_value, LocationConfigFile& current_location);

	template <typename T>
	void validateMethods(const std::string& methods_str, T& config_object);

	template <typename T>
	void validateCgiPath(const std::string& values, T& config);

	template <typename T>
	void validateIndex(const std::string& value, T& config_object);

	template <typename T>
	void validateClientBodySize(const std::string& value, T& config_object);

	template <typename T>
	void validateAutoIndex(const std::string& value, T& config_object);

public:
	ConfigParser();
	explicit ConfigParser(const std::string& config_file);

	const std::vector<ServerConfigFile> getServers() const;
};

template <typename T>
void ConfigParser::validateMethods(const std::string& methods_str, T& config_object) {
	const std::set<std::string> valid_methods = {"GET", "POST", "DELETE", "PUT"};

	std::istringstream iss(methods_str);
	std::string method;
	bool get = false, post = false, del = false, put = false;

	while (iss >> method) {
		if (valid_methods.find(method) == valid_methods.end()) {
			throw (ConfigParseError("ERROR: Invalid method in allowed_methods: " + method));
		}

		if (method == "GET") get = true;
		else if (method == "POST") post = true;
		else if (method == "DELETE") del = true;
		else if (method == "PUT") put = true;
	}

	if (!get && !post && !del && !put) {
		throw (ConfigParseError("ERROR: allowed_methods must contain at least one valid method!"));
	}

	config_object.setMethods(get, post, del, put);
}

template <typename T>
void ConfigParser::validateCgiPath(const std::string& values, T& config_object) {
	std::vector<std::string> tokens = splitByWhitespace(values);

	if (tokens.size() != 2) {
		throw (ConfigParseError("Invalid cgi_pass format: " + values));
	}

	const std::string& ext = tokens[0];
	const std::string& path = tokens[1];

	validateCgiExtension(ext);
	validateExecutablePath(path);

	config_object.addCgiExtension(ext, path);
}

template <typename T>
void ConfigParser::validateIndex(const std::string& value, T& config_object) {
	std::regex index_regex(R"(^[a-zA-Z0-9_\-]+(\.[a-zA-Z0-9_\-]+)?$)");

	if (value.empty() || value.find(' ') != std::string::npos) {
		throw ConfigParseError("ERROR: Invalid index file name: " + value + ". File name must not contain spaces.");
	}

	if (!std::regex_match(value, index_regex)) {
		throw ConfigParseError("ERROR: Invalid index file name: " + value
			+ ". File name must contain only letters, numbers, '_', '-', and optionally a single '.'");
	}

	config_object.setIndexFile(value);
}

template <typename T>
void ConfigParser::validateClientBodySize(const std::string& value, T& config_object) {
	//todo: check if value is empty
	if (!value.empty() && value[0] == '-') {
		throw (ConfigParseError("ERROR: Invalid request_body_size value: " + value + ". Negative values are not allowed."));
	}

	for (char ch : value) {
		if (!std::isdigit(ch)) {
			throw (ConfigParseError("ERROR: Invalid request_body_size value: " + value + ". Must be a non-negative integer."));
		}
	}

	int size;
	try {
		size = std::stoi(value);
	} catch (const std::invalid_argument& ) {
		throw(ConfigParseError("ERROR: Invalid request_body_size value: " + value + ". Must be a non-negative integer."));
	} catch (const std::out_of_range& ) {
		throw (ConfigParseError("ERROR: Invalid request_body_size value: " + value + ". Must be a non-negative integer."));
	}

	const int MAX_SIZE = 1024 * 1024 * 1024;

	if (size > MAX_SIZE) {
		throw (ConfigParseError("ERROR: Invalid request_body_size value: " + value
			+ ". Maximum allowed is " + std::to_string(MAX_SIZE) + " bytes."));
	}
	config_object.setRequestBodySize(size);
}

template<typename T>
void ConfigParser::validateAutoIndex(const std::string& value, T& config_object) {
	if (value == "on") {
		config_object.setAutoIndex(true);
	} else if (value == "off") {
		config_object.setAutoIndex(false);
	} else {
		throw ConfigParseError("ERROR: autoindex invalid value. Must be 'on' or 'off'");
	}
}

#endif // CONFIG_PARSER_HPP
