#ifndef CGI_MANAGER_HPP
#define CGI_MANAGER_HPP

#include <string>
#include <vector>
#include <stdexcept>

class Request;
class Response;
class Client;
class DataManager;

class CGIManager {
private:
	std::string path;
	const std::string& request_body;
	std::vector<const char*> envCGI; // Use vector of char* for environment variables
	Client*				_client;
	Response*			_response;
	DataManager&		_main_manager;
	std::string_view	_input;

	std::string getInterpreter(const std::string& path);
	//void	_write_fd(int write_fd, bool close_fd);
	std::string	cgiOutput;
	int inputPipe[2];
	int outputPipe[2];
public:
	static
	bool isCGI(const std::string& path);

	CGIManager(Client* client, Response* response, std::string path, const Request& request);
	~CGIManager(void);

	std::string execute();
};

#endif // CGI_MANAGER_HPP
