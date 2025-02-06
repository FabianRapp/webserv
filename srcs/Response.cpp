/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adrherna <adrianhdt.2001@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 13:09:21 by adrherna          #+#    #+#             */
/*   Updated: 2025/02/05 13:20:58 by adrherna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Response.hpp"
#include "../includes/FdClasses/Client.hpp"
#include "../includes/CGIManager.hpp"
#include <stdexcept>
#include <vector>

Response::Response(const ServerConfigFile& configFile, const Request& request, Client& client,
		ClientMode& client_mode):
	_response_str(),
	_request(request),
	_client_mode(client_mode),
	_body(""),
	_config(configFile),
	_target(request._uri),
	_server(client.server),
	_client(&client),
	_writer(nullptr),
	_reader(nullptr),
	_mode(ResponseMode::NORMAL),
	_cgi_manager(nullptr)
{
	//later expansions have to be applied if upload_dir is present

	_locationConfig = getLocationConfig();
	setAllowedMethods();
	_path = getExpandedTarget();

}

Response::~Response(void) {
	//if (this->_reader) {
	//	this->_reader->set_close();
	//}
	//if (this->_writer) {
	//	this->_writer->set_close();
	//}
}

// call this from client in case of early destruction
void	Response::close_io_fds(void) {
	if (this->_reader) {
		this->_reader->set_close();
	}
	if (this->_writer) {
		this->_writer->set_close();
	}
}

void	Response::set_mode(ResponseMode mode) {
	_mode = mode;
}

// Use this to read from a pipe or a file.
// Appends to the body.
// Switched into ClientMode::WRITING_FD, thus:
// Don't close the fd after calling this.
// After calling this function the caller should return straight to Client::execute
// without any more actions besides the following:
// Assumes the given fd to be valid.
void	Response::read_fd(int read_fd, ssize_t byte_count) {
	FT_ASSERT(read_fd > 0);
	ClientMode	next_mode = _client_mode;
	_client_mode = ClientMode::READING_FD;
	_reader = _server->data.new_read_fd(
		_body,
		read_fd,
		*_client,
		byte_count,
		[this, next_mode] () {
			this->_client_mode = next_mode;
			this->_reader = nullptr;
		}
	);
}

// Use this to write to a pipe or a file.
// The input data has to be in Client::_fd_write_data
// Switched into ClientMode::WRITING_FD, thus:
// After calling this function the caller should return straight to Client::execute
// without any more actions besides the following:
// Don't close the fd after calling this.
// Assumes the given fd to be valid.
void	Response::write_fd(int write_fd) {
	FT_ASSERT(write_fd > 0);
	ClientMode	next_mode = _client_mode;
	_client_mode = ClientMode::WRITING_FD;
	_writer = _server->data.new_write_fd(
		write_fd,
		_fd_write_data,
		*_client,
		[this, next_mode] () {
			this->_client_mode = next_mode;
			this->_writer = nullptr;
		}
	);
}

// assumes dqir to be a valid directory
// check errno for potential errors
std::vector<std::string>	Response::_get_dir(void) {
	DIR*	dir = opendir(_path.c_str()); // todo: has to be part of some class for err handling
	if (!dir) {
		if (errno == ENOMEM) {
			throw (std::bad_alloc());
		}
		//todo: other errors
		//500 or 403?
		std::cerr << "err: opendir: " << strerror(errno) << "\n";
		errno = 0;
		return (std::vector<std::string>());
	}
	std::vector<std::string>	files;
	struct dirent	*dir_data = readdir(dir);
	while (dir_data != NULL) {
		std::string	name = dir_data->d_name;
		files.push_back(name);
		dir_data = readdir(dir);
	}
	//todo: check for errrors of readdir
	//todo: check if auto index is enabled for the given directory
	closedir(dir);
	return (files);
}

std::string	Response::_auto_index_body(std::vector<std::string>& files)
{
	std::string body =
		"<!DOCTYPE HTML>"
		"<html>"
		" <head>"
		"  <title>Index of " + _request._uri + "</title>"
		" </head>"
		" <body>"
		"  <h1>Index of " + _request._uri + "</h1>"
		"  <hr>";
	for (auto& file : files) {
			body += "  <a href=\"" + _request._uri + file + "\">" + file + "</a><br>\n";
	}
	body +=
		"  </pre>"
		"  <hr>"
		" </body>"
		"</html>"
	;
	return (body);
}

// called if _path is a directory
void	Response::_handle_auto_index(std::vector<std::string>&files) {
	std::string	body = _auto_index_body(files);
	_response_str =
		std::string("HTTP/1.1 200 OK\r\n")
		+ "Content-Type: text/html\r\n"
		"Content-Length: " + std::to_string(body.size()) + "\r\n"
		"Connection: close\r\n"
		"\r\n"
		+ body
	;
	_client_mode = ClientMode::SENDING;
}

// _path is a file that is not CGI
// todo: err handling
void	Response::_handle_get_file(void) {
	struct stat stats;

	std::cout << "B: " << _path << "\n";
	FT_ASSERT(stat(_path.c_str(), &stats) != -1);
	int	file_fd = open(_path.c_str(), O_CLOEXEC | O_RDONLY);
	FT_ASSERT(file_fd >0);
	read_fd(file_fd, stats.st_size);

	_mode = ResponseMode::FINISH_UP;
	_response_str =
		std::string("HTTP/1.1 200 OK\r\n")
		+ "Content-Type: text/html\r\n"
	;
}

void	Response::_handle_get_moved(void) {
	std::string new_location = _request._uri + "/";
	_response_str =
		"HTTP/1.1 301 Moved Permanently\r\n"
		"Location: " + new_location + "\r\n"
	;
	load_status_code_body(301);
}

//if index file is found returns true and puts it's path in index_file
bool	Response::_has_index(std::vector<std::string>& files, std::string& index_file) {
	_config;//const ServerConfigFile&
	_locationConfig;//LocationConfigFile*
	//default return
	return (false);
}

//todo: commented lines
void	Response::_handle_get(void) {
	if (std::filesystem::is_directory(_path)) {
		if (_request._uri.back() != '/') {
			std::cout << "MOVED\n";
			sleep(5);
			_handle_get_moved();
			return ;
		}
		std::vector<std::string>	files = _get_dir();
		std::string					index_file;
		if (_has_index(files, index_file)) {
			_path = index_file;
		} else if (_locationConfig->getAutoIndex()) {
			_handle_auto_index(files);
			return ;
		/*
		} else {
			handle invlaid request
			return ;
		*/
		}
	}
	/*
	if (does not exist(_path)) {
	}
	*/
	FT_ASSERT(!std::filesystem::is_directory(_path));

	if (CGIManager::isCGI(_path)) {
		if (!_cgi_manager) {
			_cgi_manager = new CGIManager(_client, this, _path, _request);
		}
		if (_cgi_manager->execute()) {
			_response_str =
				std::string("HTTP/1.1 200 OK\r\n")
				+ "Content-Type: text/html\r\n"
			;
			delete _cgi_manager;
			_cgi_manager = nullptr;
			_mode = ResponseMode::FINISH_UP;
		}
	} else {
		_handle_get_file();
	}
}

void	Response::_handle_post(void) {
	/*
	if (!is_dir(_path)) {
		err;
	}
	if(CGIManager::isCGI(_path)) {
		_cgi_manager = new CGIManager(_path, _request);
		_response_str = _cgi_manager->execute();
		_client_mode = ClientMode::SENDING;
		delete _cgi_manager;
	} else {
		_handle_post_file();
	}
	*/
}

//don't use this if the response needs any custom data besides the status
//this response the respose
void	Response::load_status_code_response(int code, const std::string& status) {
	_client_mode = ClientMode::BUILD_RESPONSE; // in case this was called from other call back
	_response_str = std::string("HTTP/1.1 ") + std::to_string(code) + status + "\r\n"
		+ "Content-Type: text/html\r\n";
	std::string stat_code_path = _config.getErrorPages().getErrorPageLink(code);
	
	struct stat stats;
	FT_ASSERT(stat(stat_code_path.c_str(), &stats) != -1);
	int	file_fd = open(stat_code_path.c_str(), O_CLOEXEC | O_RDONLY);
	FT_ASSERT(file_fd >0);
	read_fd(file_fd, stats.st_size);
	_mode = ResponseMode::FINISH_UP;
}

void	Response::load_status_code_body(int code) {
	std::string stat_code_path = _config.getErrorPages().getErrorPageLink(code);
	_response_str += "Content-Type: text/html\r\n";
	struct stat stats;
	FT_ASSERT(stat(stat_code_path.c_str(), &stats) != -1);
	int	file_fd = open(stat_code_path.c_str(), O_CLOEXEC | O_RDONLY);
	FT_ASSERT(file_fd >0);
	read_fd(file_fd, stats.st_size);
	_mode = ResponseMode::FINISH_UP;
}

void	Response::_handle_delete(void) {
	std::cout << "Would" FT_ANSI_RED " DELETE " FT_ANSI_RESET << _path << "\n";
	if (1 /* std::remove(_path.c_str()) == 0 */) {
		//success
		_response_str =
			"HTTP/1.1 204 No Content\r\n";
		load_status_code_body(204);
		return ;
	} else {
		//error
		switch (errno) {
			case (ENOMEM):
				errno = 0;
				throw(std::bad_alloc()); break ;
			default:
			//todo
			break ;
		}
		errno = 0;
	}
}

bool	Response::isMethodAllowed(MethodType method) {
	return std::find(_allowedMethods.begin(), _allowedMethods.end(), method) != _allowedMethods.end();
}

void	Response::execute(void) {
	if (_mode == ResponseMode::NORMAL) {
		if (isMethodAllowed(_request._type))
		{
			switch (_request._type)
			{
				case (MethodType::GET): {
					_handle_get();
					break ;
				} case (MethodType::POST): {
					_handle_post();
					break ;
				} case (MethodType::DELETE): {
					_handle_delete();
					break ;
				} default: {
					_response_str = "HTTP/1.1 405 Method Not Allowed\r\n";
					load_status_code_body(405);
					return ;
				}
			}
		}
		else
		{
			std::cout << "ERROR IN EXECUTE\n";
			_response_str = "HTTP/1.1 405 Method Not Allowed\r\n";
			load_status_code_body(405);
			return ;
		}
	} else if (_mode == ResponseMode::FINISH_UP) {
		/* for potential file reads:
		 * can not be done in same function call as initial if statement!
		*/
		_response_str +=
			"Connection: close\r\n"
			"Content-Length: " + std::to_string(_body.length()) + "\r\n"
				"\r\n"
			+ _body
		;
		_client_mode = ClientMode::SENDING;
	} else {
		FT_ASSERT(0);
	}
}

void	Response::reset_body(void) {
	_body = "";
}

void	Response::set_fd_write_data(const std::string_view data) {
	_fd_write_data = data;
}

std::string& Response::getBody() {
	return(_body);
}

std::string& Response::getTarget() {
	return (_target);
}

const ServerConfigFile& Response::getConfig() const {
	return (_config);
}

void	Response::appendToBody(std::string content) {
	_body += content;
}

const LocationConfigFile* Response::getLocationConfig() {
	const std::vector<LocationConfigFile>& locationsFiles = _config.getLocations();

	for (auto& locationFile : locationsFiles)
	{
		//todo: if uri has a file type ending remove the the file name from uri for the match checking
		if (_request._uri == locationFile.getPath() + '/'
			|| (_request._uri.length() == 1 && _request._uri == locationFile.getPath()))
		{
			std::cout << "URI = |" << _request._uri << " LOC = |" << locationFile.getPath() << "|\n";
			return &locationFile;
		}
	}
	std::cout << "NO LOCATION CONFIG FOUND... using default\n";
	return &_config.getDefaultLocation();
}

void Response::setAllowedMethods() {

	if (_locationConfig == nullptr) {
		_allowedMethods.push_back(MethodType::GET);
		_allowedMethods.push_back(MethodType::POST);
		_allowedMethods.push_back(MethodType::DELETE);
		std::cout << "NO LOCATION PRESENT, default all methods allowed\n";
		return ;
	}
	//todo: for testing since LocationFile has unreliable data
	if (_locationConfig->isGetAllowed())
	{
		_allowedMethods.push_back(MethodType::GET);
	}
	if (_locationConfig->isPostAllowed())
	{
		_allowedMethods.push_back(MethodType::POST);
	}
	if (_locationConfig->isPostAllowed())
	{
		_allowedMethods.push_back(MethodType::DELETE);
	}
}

std::string	Response::getExpandedTarget(void) {
	std::string expandedPath;

//
	if (_locationConfig != nullptr)
		expandedPath = _config.getRoot() + _locationConfig->getRoot();
	else {
		expandedPath = _config.getRoot() + _request._uri;
	}

	std::cout << "RESPONSE PATH SETTED TO |" << expandedPath << "|\n";

	if (std::filesystem::exists(expandedPath)) {
		std::cout << "File exists: " << expandedPath << std::endl;
	} else {
		std::cout << "File does not exist: " << expandedPath << std::endl;
		return (_config.getRoot() + "/404.html");
		// throw std::runtime_error("File does not exist: " + expandedPath);
	}
\
	return (expandedPath);
}

std::string&&	Response::get_str_response(void) {
	return (std::move(_response_str));
}

CGIManager*&	Response::get_cgi_manger(void) {
	return (_cgi_manager);
}

WriteFd*&	Response::get_writer(void) {
	return (_writer);
}
