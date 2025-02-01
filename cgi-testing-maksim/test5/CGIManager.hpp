#ifndef CGI_MANAGER_HPP
#define CGI_MANAGER_HPP

#include <string>
#include <unordered_map>
#include <stdexcept>

class CGIManager {
private:
	std::string uri;
	std::string body;
	std::unordered_map<std::string, std::string> envVars;

	bool isCGI(const std::string& uri);
	std::string getInterpreter(const std::string& uri);
	
public:
	CGIManager(const std::string& uri, const std::string& body, const std::unordered_map<std::string, std::string>& envVars);
	std::string execute();


};

#endif // CGI_MANAGER_HPP