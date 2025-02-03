#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <vector>
#include <string>
#include <fstream>
#include "ServerConfigFile.hpp"
#include "LocationConfigFile.hpp"

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
	void validateMethods(const std::string& methods_str, LocationConfigFile& current_location);
	void validateAutoIndex(const std::string& value, LocationConfigFile& current_location);
public:
	ConfigParser();
	explicit ConfigParser(const std::string& config_file);

	const std::vector<ServerConfigFile> getServers() const;
};

#endif // CONFIG_PARSER_HPP
