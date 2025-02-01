#ifndef CGI_MANAGER_HPP
#define CGI_MANAGER_HPP

#include <string>
#include <vector>
#include <stdexcept>

class Request;

class CGIManager {
private:
	std::string path;
	const std::string& request_body;
	std::vector<const char*> envCGI; // Use vector of char* for environment variables

	std::string getInterpreter(const std::string& path);

public:
	static
	bool isCGI(const std::string& path);

	CGIManager(std::string path, const Request& request);
	std::string execute();
};

#endif // CGI_MANAGER_HPP
