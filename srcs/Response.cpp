/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adrherna <adrianhdt.2001@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 13:09:21 by adrherna          #+#    #+#             */
/*   Updated: 2025/01/31 13:41:50 by adrherna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Response.hpp"
#include "../includes/FdClasses/Client.hpp"

Response::Response(const ServerConfigFile& configFile, const Request& request, Client& client,
		ClientMode& client_mode):
	_config(configFile),
	_client_mode(client_mode),
	_request(request),
	_reader(nullptr),
	_writer(nullptr),
	_is_cgi(false),
	_mode(ResponseMode::NORMAL)
{
	//later expansions have to be applied if upload_dir is present
	_target = request._uri;
	_body = "";
	_server = client.server;
	_path = getExpandedTarget();
	// todo: _is_cgi = 
}

Response::~Response(void) {
	if (this->_reader) {
		this->_reader->set_close();
	}
	if (this->_writer) {
		this->_writer->set_close();
	}
}

// Use this to read from from a pipe or a file.
// Appends to the body.
// Switched int ClientMode::WRITING_FD, thus:
// After calling this function the caller should return straight to Client::execute
// without any more actions besides the following:
// Uses dup() on the given fd, if the fd is not needed anywher else simply close
// the fd after calling this.
// Assumes the given fd to be valid.
void	Response::_read_fd(int read_fd, ssize_t byte_count, bool close_fd) {
	//_fd_error.error = false;
	FT_ASSERT(read_fd > 0);
	ClientMode	next_mode = _client_mode;
	_client_mode = ClientMode::READING_FD;
	_reader = _server->data.new_read_fd(
		_body,
		read_fd,
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
void	Response::_write_fd(int write_fd, bool close_fd) {
	//_fd_error.error = false;
	FT_ASSERT(write_fd > 0);
	ClientMode	next_mode = _client_mode;
	_client_mode = ClientMode::WRITING_FD;
	
	_writer = _server->data.new_write_fd(
		write_fd,
		_fd_write_data,
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

	_response_str =
		std::string("HTTP/1.1 200 OK\r\n")
		+ "Content-Type: text/html\r\n"
	;
	FT_ASSERT(stat(_path.c_str(), &stats) != -1);
	int	file_fd = open(_path.c_str(), O_RDONLY);
	FT_ASSERT(file_fd >0);
	_read_fd(file_fd, stats.st_size, true);

	_mode = ResponseMode::FINISH_UP;
}

void	Response::_handle_get_moved(void) {
	std::string new_location = _request._uri + "/";

	_response_str =
		"HTTP/1.1 301 Moved Permanently\r\n"
		"Location: " + new_location + "\r\n"
	;

	std::string	err_file_placeholder = "default/error_pages/301.html"; //todo

	struct stat stats;
	FT_ASSERT(stat(err_file_placeholder.c_str(), &stats) != -1);
	int	file_fd = open(err_file_placeholder.c_str(), O_RDONLY);
	FT_ASSERT(file_fd >0);
	_read_fd(file_fd, stats.st_size, true);

	_mode = ResponseMode::FINISH_UP;
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
			_is_cgi = is_cgi(index_file);
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
	/*
	if (is_cgi(_path, config)) {
		_handle_cgi(_path, config);
	} else */{
		_handle_get_file();
	}
}

void	Response::_handle_post(void) {
}

void	Response::_handle_delete(void) {
}

void	Response::execute(void) {
	if (_mode == ResponseMode::NORMAL) {
		switch (_request._type) {
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
				std::cerr << "Error: Unsupported request type: "
					<< to_string(_request._type) << "\n";
				//todo: 405 err
				FT_ASSERT(0);
			}
		}
	} else if (_mode == ResponseMode::FINISH_UP) {
		if (!_is_cgi) {
			_response_str += 
				"Connection: close\r\n"
				"Content-Length: " + std::to_string(_body.length()) + "\r\n"
					"\r\n"
				+ _body
			;
		}
		_client_mode = ClientMode::SENDING;
	} else {
		FT_ASSERT(0);
	}
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

std::string	Response::getExpandedTarget(void) {
	//return (std::string(getenv("PWD")) + "/" + "hello_world.html");//to test get file
	return (std::string(getenv("PWD")) + "/"); // to test auto index
	/*
	std::vector<LocationConfigFile> locations = _config.getLocations();
	std::string expandedTarget = _target;

	for (auto location: locations)
	{
		location.printLocation();
	}

	std::cout << "ENTERING EXPANDER\n";

	for (auto location: locations)
	{
		std::cout << "_target: |" << _target << "|\n" << " Location _path: |" << location.get_path() << "|\n" << " Upload dir: |" << location.getUploadDir() << "|" << std::endl;

		if (_target == location.get_path() && location.getUploadDir() != "")
		{
			expandedTarget = _config.getRoot() + location.getUploadDir();
			std::cout << "NEW TARGET: " << _target << std::endl;
		}
	}
	
	std::cout << "EXITING EXPANDER\n";
	return expandedTarget;

	*/
}

std::string&&	Response::get_str_response(void) {
	return (std::move(_response_str));
}

