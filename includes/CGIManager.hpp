#ifndef CGI_MANAGER_HPP
#define CGI_MANAGER_HPP

#include <string>
#include <vector>
#include <stdexcept>
#include <LocationConfigFile.hpp>
class Request;
class Response;
class Client;
class DataManager;
class WriteFd;
class ReadFd;

class CGIManager {
private:
	std::string path;
	const std::string& request_body;
	std::vector<std::string> envCGI_storage;
	std::vector<const char*> envCGI; // Use vector of char* for environment variables
	Client*				_client;
	Response*			_response;
	DataManager&		_main_manager;
	const std::string_view	_input;
	// WriteFd*			_writer;
	// ReadFd*				_reader;
	pid_t				_pid;
	const LocationConfigFile&	_location_cofig;
	enum class CGI_MODE {
		PASS,
		INIT_READING,
		INIT_WRITING,
		FINISHED,
	}	_mode;

	std::string getInterpreter(const std::string& path);
	void		_init_reading(void);
	void		_init_writing(void);
	void		_child_dup_fail(void);
	void		_child_exec_fail(void);

	//void	_write_fd(int write_fd, bool close_fd);
	std::string	_cgiOutput;
	int inputPipe[2];
	int outputPipe[2];

public:
	static
	bool isCGI(const std::string& path, const LocationConfigFile& location_config);

	CGIManager(void) = delete;
	CGIManager(Client* client, const LocationConfigFile& location_config, Response* response, std::string path, const Request& request);
	void printCgiRunning();
	~CGIManager(void);

	bool	execute();
};

#endif // CGI_MANAGER_HPP
