#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <vector>
#include <string>
#include <fstream>
#include "ServerConfigFile.hpp"
#include "LocationConfigFile.hpp"

#include <regex>

class ConfigParseError: public std::runtime_error{
public:
	ConfigParseError(const std::string& msg): std::runtime_error(msg){}
};

class ConfigParser {
private:
	std::vector<ServerConfigFile> _servers; // List of parsed servers

	// Helper methods
	std::string trimWhiteSpace(const std::string& str) const;
	bool isValidLocationPath(const std::string& path) const;
	void parseFile(const std::string& config_file);
	void parseServerBlock(std::ifstream& file, ServerConfigFile& current_server, int& bracket_count);
	void parseLocationBlock(std::ifstream& file, LocationConfigFile& current_location, int& bracket_count);
	// void validateMethods(const std::string& methods_str, LocationConfigFile& current_location);
	void validateAutoIndex(const std::string& value, LocationConfigFile& current_location);

	//testing templates for validation. Need to recheck todo
	template <typename T>
	void validateMethods(const std::string& methods_str, T& config_object);
	template <typename T>
	void validateIndex(const std::string& value, T& config_object);

	template <typename T>
	void handleCgiPath(const std::string& line, T& config);
	void validateCgiExtension(const std::string& ext);
	void validateExecutablePath(const std::string& path);

	void validatePort(const std::string& line, ServerConfigFile& current_server);

	// void validateRoot(const std::string& value, const std::string& directive_name);
	void validateRoot(const std::string& value, const std::string& directive_name, bool is_server_block);

	void validateClientBodySize(const std::string& value);

	std::string sanitizeLine(const std::string& line) const;

	std::vector<std::string> splitByWhitespace(const std::string& str) const;

public:
	ConfigParser();
	explicit ConfigParser(const std::string& config_file);

	const std::vector<ServerConfigFile> getServers() const;
};

#endif // CONFIG_PARSER_HPP
