#ifndef CGI_MANAGER_HPP
#define CGI_MANAGER_HPP

#include <string>
#include <vector>
#include <stdexcept>

class CGIManager {
private:
	std::string uri;
	std::string body;
	std::vector<char*> envCGI; // Use vector of char* for environment variables

	bool isCGI(const std::string& uri);
	std::string getInterpreter(const std::string& uri);

public:
	CGIManager(const std::string& uri, const std::string& body,
		const std::vector<char*>& envCGI);
	std::string execute();
};

#endif // CGI_MANAGER_HPP
