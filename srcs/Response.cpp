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

// Use this to read froserverm from a pipe or a file.
// Appends to the body.
// Switched int ClientMode::WRITING_FD, thus:
// After calling this function the caller should return straight to Client::execute
// without any more actions besides the following:
// Uses dup() on the given fd, if the fd is not needed anywher else simply close
// the fd after calling this.
// Assumes the given fd to be valid.
void	Response::read_fd(int read_fd, ssize_t byte_count, bool close_fd) {
	//_fd_error.error = false;
	FT_ASSERT(read_fd > 0);
	//char buff[1024];
	//ssize_t val;
	//std::cout << "read_fd\n";
	//val = read(read_fd, buff, 1023);
	//while (val > 0) {
	//	assert(val > 0);
	//	buff[val] = 0;
	//	std::cout << "buff: " << buff << "\n";
	//	_body += buff;
	//	val = read(read_fd, buff, 1023);
	//}
	//printf("val: %ld\n", val);
	//return ;
	ClientMode	next_mode = _client_mode;
	_client_mode = ClientMode::READING_FD;
	_reader = _server->data.new_read_fd(
		_body,
		read_fd,
		*_client,
		byte_count,
		close_fd,
		[this, next_mode] () {
			this->_client_mode = next_mode;
			this->_reader = nullptr;
		}
	);
}

// Use this to write to a pipe or a file.
// The input data has to be in Client::_fd_write_data
// Switched int ClientMode::WRITING_FD, thus:
// After calling this function the caller should return straight to Client::execute
// without any more actions besides the following:
// Uses dup() on the given fd, if the fd is not needed anywher else simply close
// the fd after calling this.
// Assumes the given fd to be valid.
void	Response::write_fd(int write_fd, bool close_fd) {
	//_fd_error.error = false;
	FT_ASSERT(write_fd > 0);
	ClientMode	next_mode = _client_mode;
	_client_mode = ClientMode::WRITING_FD;
	_writer = _server->data.new_write_fd(
		write_fd,
		_fd_write_data,
		*_client,
		close_fd,
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

	FT_ASSERT(stat(_path.c_str(), &stats) != -1);
	int	file_fd = open(_path.c_str(), O_CLOEXEC | O_RDONLY);
	FT_ASSERT(file_fd >0);
	read_fd(file_fd, stats.st_size, true);

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
	load_status_code(301);
}

//todo: commented lines
void	Response::_handle_get(void) {
	if (std::filesystem::is_directory(_path)) {
		if (_request._uri.back() != '/') {
			_handle_get_moved();
			return ;
		}
		std::vector<std::string>	files = _get_dir();
		/*
		std::string					index_file;
		if (has_index(files, config, index_file)) {
			_path = index_file;
		} else if (enabled_auto_index(_path, config)) {
		*/
			_handle_auto_index(files);
			return ;
		/*
		} else {
			handle invlaid request
			return ;
		}
		*/
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

//todo: needs to work with config not hard coded paths
void	Response::load_status_code(int code) {
	std::string stat_code_path = "default/error_pages/" + std::to_string(code) + ".html"; //todo
	_response_str += "Content-Type: text/html\r\n";
	struct stat stats;
	FT_ASSERT(stat(stat_code_path.c_str(), &stats) != -1);
	int	file_fd = open(stat_code_path.c_str(), O_CLOEXEC | O_RDONLY);
	FT_ASSERT(file_fd >0);
	read_fd(file_fd, stats.st_size, true);
	_mode = ResponseMode::FINISH_UP;
}

void	Response::_handle_delete(void) {
	std::cout << "Would" FT_ANSI_RED " DELETE " FT_ANSI_RESET << _path << "\n";
	if (1 /* std::remove(_path.c_str()) == 0 */) {
		//success
		_response_str =
			"HTTP/1.1 204 No Content\r\n";
		load_status_code(204);
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
					break ;
				}
			}
		}
		else
		{
			std::cout << "ERROR IN EXECUTE\n";
			_response_str = "HTTP/1.1 405 Method Not Allowed\r\n";
			load_status_code(405);
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

LocationConfigFile* Response::getLocationConfig() {

	std::vector<LocationConfigFile> locationsFiles;

	for (auto& locationFile : locationsFiles)
	{
		if (_request._uri == locationFile.getPath())
		{
			std::cout << "URI = |" << _request._uri << " LOC = |" << locationFile.getPath() << "|\n";
			return &locationFile;
		}
	}
	std::cout << "NO LOCATION CONFIG FOUND\n";
	return nullptr;
}

void Response::setAllowedMethods() {

	if (_locationConfig == nullptr) {
		_allowedMethods.push_back(MethodType::GET);
		_allowedMethods.push_back(MethodType::POST);
		_allowedMethods.push_back(MethodType::DELETE);
		std::cout << "NO LOCATION PRESENT, default all methods allowed\n";
		return ;
	}
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
