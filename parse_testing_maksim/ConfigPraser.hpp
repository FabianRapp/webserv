#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP



#include <vector>
#include <fstream>
//todo: should it be only in the deeper one? #include <iostream>
#include <iostream>
#include "ServerConfigFile.hpp"
#include "LocationConfigFile.hpp"

class ConfigParser {
private:
	//todo: do we need size or we can check _servers.size()?
	std::vector<ServerConfigFile> _servers;

	std::string trimWhiteSpace(const std::string& str) const;
    void parseLine(const std::string& line, bool& in_server_block, bool& in_location_block, ServerConfigFile& current_server, LocationConfigFile& current_location, int& bracket_count);
};

#endif
