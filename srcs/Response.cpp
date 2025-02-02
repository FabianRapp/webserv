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
#include "../includes/CGIManager.hpp"

Response::Response(const ServerConfigFile& configFile, const Request& request, Client& client,
		ClientMode& client_mode):
	_config(configFile),
	_client_mode(client_mode),
	_client(&client),
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
	_is_cgi = CGIManager::isCGI(_path);
}

Response::~Response(void) {
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
void	Response::write_fd(int write_fd, bool close_fd) {
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


	FT_ASSERT(stat(_path.c_str(), &stats) != -1);
	int	file_fd = open(_path.c_str(), O_RDONLY);
	FT_ASSERT(file_fd >0);
	read_fd(file_fd, stats.st_size, true);

	_mode = ResponseMode::FINISH_UP;
}

void	Response::_handle_get_moved(void) {
	std::string new_location = _request._uri + "/";
	_response_str =
		"HTTP/1.1 301 Moved Permanently\r\n"
		"Location: " + new_location + "\r\n"
	;
	_load_status_code(301);
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
			_is_cgi = CGIManager::isCGI(_path);
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
	_response_str =
		std::string("HTTP/1.1 200 OK\r\n")
		+ "Content-Type: text/html\r\n"
	;
	if (_is_cgi) {
		_cgi_manager = new CGIManager(_client, this, _path, _request);
		//_body = _cgi_manager->execute();
		//
		//_response_str +=
		//	"Connection: close\r\n"
		//	"Content-Length: " + std::to_string(_body.length()) + "\r\n"
		//	"\r\n"
		//	+ _body
		//;
		//_client_mode = ClientMode::SENDING;
		delete _cgi_manager;
	} else {
		_handle_get_file();
	}
}

void	Response::_handle_post(void) {
	/*
	if (!is_dir(_path)) {
		err;
	}
	if(_is_cgi) {
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
void	Response::_load_status_code(int code) {
	std::string stat_code_path = "default/error_pages/" + std::to_string(code) + ".html"; //todo
	_response_str += "Content-Type: text/html\r\n";
	struct stat stats;
	FT_ASSERT(stat(stat_code_path.c_str(), &stats) != -1);
	int	file_fd = open(stat_code_path.c_str(), O_RDONLY);
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
		_load_status_code(204);
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

//todo: throw / catch fd errors
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
				//todo: not sure if this code/string is correct:
				_response_str = "HTTP/1.1 405 Method Not Allowed\r\n";
				_load_status_code(405);
				break ;
			}
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

void	Response::set_fd_write_data(std::string_view data) {
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

std::string	Response::getExpandedTarget(void) {
	//return (std::string(getenv("PWD")) + "/" + "hello_world.html");//to test get file
	return (std::string(getenv("PWD")) + "/" + "hello.php");//to test get file
	return (std::string(getenv("PWD")) + "/" + "hello.py");//to test get file
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

WriteFd*&	Response::get_writer(void) {
	return (_writer);
}
