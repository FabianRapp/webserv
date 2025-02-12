/* ************************************************************************** */
/*																			*/
/*														:::	  ::::::::   */
/*   Response.cpp									   :+:	  :+:	:+:   */
/*													+:+ +:+		 +:+	 */
/*   By: adrherna <adrianhdt.2001@gmail.com>		+#+  +:+	   +#+		*/
/*												+#+#+#+#+#+   +#+		   */
/*   Created: 2025/01/28 13:09:21 by adrherna		  #+#	#+#			 */
/*   Updated: 2025/02/06 13:55:33 by adrherna		 ###   ########.fr	   */
/*																			*/
/* ************************************************************************** */

#include "../includes/Response.hpp"
#include "../includes/FdClasses/Client.hpp"
#include "../includes/CGIManager.hpp"

#include <vector>

Response::Response(const ServerConfigFile& configFile, const LocationConfigFile& locationConfig,
		const Request& request, Client& client, ClientMode& client_mode):
	_response_str(),
	_request(request),
	_client_mode(client_mode),
	_body(""),
	_config(configFile),
	_location_config(locationConfig),
	_target(request._uri),
	_server(client.server),
	_client(&client),
	_writer(nullptr),
	_reader(nullptr),
	_mode(ResponseMode::NORMAL),
	_cgi_manager(nullptr),
	_first_iter(true),
	_dir(nullptr),
	_in_error_handling(false)
{
	setAllowedMethods();
}

Response::~Response(void) {
	if (_dir) {
		closedir(_dir);
	}
	delete _cgi_manager;
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
void	Response::read_fd(int read_fd, ssize_t byte_count, bool cgi_output) {
	FT_ASSERT(read_fd > 0);
	ClientMode	next_mode = _client_mode;
	_client_mode = ClientMode::READING_FD;
	if (!cgi_output) {
		_reader = _server->data.new_read_fd(
			*this,
			_body,
			read_fd,
			*_client,
			byte_count,
			[this, next_mode] () {
				this->_client_mode = next_mode;
				this->_reader = nullptr;
			}
		);
	} else {
		_response_str = "";
		_reader = _server->data.new_read_fd(
			*this,
			_response_str,
			read_fd,
			*_client,
			byte_count,
			[this, next_mode] () {
				this->_client_mode = next_mode;
				this->_reader = nullptr;
			}
		);
	}
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
		*this,
		write_fd,
		_fd_write_data,
		*_client,
		[this, next_mode] () {
			this->_client_mode = next_mode;
			this->_writer = nullptr;
		}
	);
}

std::vector<std::string>	Response::_get_dir(void) {
	_dir = opendir(_path.c_str());
	if (!_dir) {
		std::cerr << FT_ANSI_RED " ERROR " FT_ANSI_RESET << "opendir: " << _path << " failed: " << strerror(errno) << "\n";
		switch (errno) {
			case ENOMEM:
			case EMFILE:  // Process limit reached for open files
			case ENFILE:  // System limit reached for open files
				errno = 0;
				throw std::bad_alloc();
			case EACCES:
			case ELOOP:
			case ENAMETOOLONG:
			case ENOENT:
			case ENOTDIR:
				load_status_code_response(404, "Not Found");
				break ;
			case EIO:
			case EBADF:
			case EFAULT:
			default:
				load_status_code_response(500, "Internal Server Error");
				break ;
		}
		errno = 0;
		return (std::vector<std::string>());
	}
	std::vector<std::string>	files;
	errno = 0;
	struct dirent	*dir_data = readdir(_dir);
	while (dir_data != NULL) {
		std::string	name = dir_data->d_name;
		files.push_back(name);
		dir_data = readdir(_dir);
	}
	if (errno) {
		std::cerr << FT_ANSI_RED " ERROR " FT_ANSI_RESET << "readdir: " << strerror(errno) << "\n";
		closedir(_dir);
		_dir = nullptr;
		errno = 0;
		load_status_code_response(500, "Internal Server Error");
		return (std::vector<std::string>());
	}
	closedir(_dir);
	_dir = nullptr;
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
void	Response::_handle_get_file(void) {
	struct stat stats;

	if (stat(_path.c_str(), &stats) == -1) {
		load_status_code_response(500, "Internal Server Error");
		return ;
	}
	int	file_fd = open(_path.c_str(), O_CLOEXEC | O_RDONLY);
	if (file_fd < 0) {
		load_status_code_response(500, "Internal Server Error");
		return ;
	}
	read_fd(file_fd, stats.st_size, false);

	_mode = ResponseMode::FINISH_UP;
	_response_str = std::string("HTTP/1.1 200 OK\r\n");
	_append_content_type(_path);
}

//status has to be either 301 (Moved Permanently) or 302(302 Moved Temporarily)
void	Response::_handle_get_moved(const std::string& new_loc, int status) {
	if (status == 301) {
		_response_str =
			"HTTP/1.1 301 Moved Permanently\r\n"
			"Location: " + new_loc + "\r\n"
		;
		load_status_code_body(301);
	} else if (status == 302) {
		_response_str =
			"HTTP/1.1 302 Moved Temporarily\r\n"
			"Location: " + new_loc + "\r\n"
		;
		load_status_code_body(302);
	} else {
		FT_ASSERT(0);
	}
}

bool	Response::_has_index(std::vector<std::string>& files, std::string& index_file) {
	const std::string&	index_name = _location_config.getIndexFile();
	for (const auto& file : files) {
		if (file == index_name) {
			index_file = _path + index_name;
			return (true);
		}
	}
	return (false);
}

void	Response::_handle_get(void) {
	if (std::filesystem::is_directory(_path)) {
	 	if (_request._uri.back() != '/') {
			_handle_get_moved(_request._uri + "/", 301);
			return ;
		}
		std::vector<std::string>	files = _get_dir();
		std::string					index_file;
		if (_has_index(files, index_file)) {
			_path = index_file;
		} else if (_location_config.getAutoIndex()) {
			_handle_auto_index(files);
			return ;
		} else {
			load_status_code_response(404, "Not Found");
			return ;
		}
	}
	if (access(_path.c_str(), F_OK) == -1) {
		load_status_code_response(404, "Not Found");
		return ;
	}
	if (access(_path.c_str(), R_OK) == -1) {
		load_status_code_response(404, "Not Found");
		return ;
	}

	FT_ASSERT(!std::filesystem::is_directory(_path));

	if (CGIManager::isCGI(_path, _location_config)) {
		if (!_cgi_manager) {
			_cgi_manager = new CGIManager(_client, _location_config, this, _path, _request);
		}
		if (_cgi_manager->execute()) {
			delete _cgi_manager;
			_cgi_manager = nullptr;
			//indicate client can delete resposne obj and send response
			_client_mode = ClientMode::SENDING;
		}
	} else {
		_handle_get_file();
	}
}

void	Response::_append_content_type(const std::string& path) {
	_response_str += "Content-Type: ";
	size_t pos = path.find_last_of(".");
	if (pos == std::string::npos) {
		_response_str += "application/octet-stream\r\n";
		return ;
	}

	std::string ext = path.substr(pos + 1);
	std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

	if (ext == "html" || ext == "htm") {
		_response_str += "text/html\r\n";
	} else if (ext == "css") {
		_response_str += "text/css\r\n";
	} else if (ext == "js") {
		_response_str += "application/javascript\r\n";
	} else if (ext == "jpg" || ext == "jpeg") {
		_response_str += "image/jpeg\r\n";
	} else if (ext == "png") {
		_response_str += "image/png\r\n";
	} else {
		_response_str += "application/octet-stream";
	}
}

void	Response::_handle_put_file(bool post) {
	int	flags;
	int	put_status = 204;
	if (!access(_path.c_str(), F_OK)) {
		put_status = 201;
	}
	if (post) {
		flags = O_WRONLY | O_CREAT | O_APPEND | O_EXCL;
	} else {
		flags = O_WRONLY | O_CREAT | O_APPEND | O_TRUNC;
	}
	int	post_fd = open(_path.c_str(), flags, 0644);
	if (post_fd < 0) {
		std::cout << "Error: POST: failed open(): " << strerror(errno) << std::endl;
		switch (errno) {
			case ENOMEM:
			case EMFILE:  // Process limit reached for open files
			case ENFILE:  // System limit reached for open files
				errno = 0;
				throw std::bad_alloc();
				break ;
			case EACCES:
			case EFAULT:
			case ENOENT:
			case ENOTDIR:
				load_status_code_response(404, "Not Found");
				break ;
			case EEXIST:
				load_status_code_response(409, "Conflict");
				break ;
			case ENOSPC:
				load_status_code_response(507, "Insufficient Storage");
				break ;
			case EROFS:
			case EPERM:
			case EBUSY:
			case EISDIR:
			case EINVAL:
			case ENAMETOOLONG:
			case ELOOP:
			case EIO:
			default:
				load_status_code_response(500, "Internal Server Error");
				break ;
		}
		errno = 0;
		return ;
	}
	if (put_status == 204) {
		_response_str = std::string("HTTP/1.1 204 Created\r\n");
	} else if (put_status == 201) {
		_response_str = std::string("HTTP/1.1 201 No Content\r\n");
	} else {
		FT_ASSERT(0 && "put_stus unknown value");
	}
	std::cout << "request body size: " << _request._body.size() << std::endl ;
	_fd_write_data = std::string_view(_request._body);
	write_fd(post_fd);
	_mode = ResponseMode::FINISH_UP;
}

void	Response::_handle_post(void) {
	std::cout << "handle_post\n";
	if (std::filesystem::is_directory(_path)) {
	 	if (_request._uri.back() != '/') {
			_handle_get_moved(_request._uri + "/", 301);
			return ;
		}
		std::vector<std::string>	files = _get_dir();
		std::string					index_file;
		if (_has_index(files, index_file) && CGIManager::isCGI(index_file, _location_config)) {
			_path = index_file;
		} else {
			load_status_code_response(403, "Forbidden");
			return ;
		}
	}
	if(CGIManager::isCGI(_path, _location_config)) {
		if (!_cgi_manager) {
			_cgi_manager = new CGIManager(_client, _location_config, this, _path, _request);
		}
		if (_cgi_manager->execute()) {
			delete _cgi_manager;
			_cgi_manager = nullptr;
			//indicate client can delete resposne obj and send response
			_client_mode = ClientMode::SENDING;
		}
	} else {
		//todo: do we handle it as put or give an error?
		_handle_put_file(true);
	}
}

void	Response::_handle_put(void) {
	std::cout << "handle_post\n";
	if (CGIManager::isCGI(_request._uri, _location_config)) {
		load_status_code_response(403, "Forbidden");
		return ;
	}
	if (std::filesystem::is_directory(_path)) {
		load_status_code_response(403, "Forbidden");
		return ;
	}
	_handle_put_file(false);
}

//don't use this if the response needs any custom data besides the status
//this response the respose
void	Response::load_status_code_response(int code, const std::string& status,
			std::vector<std::string> extra_headers)
{
	if (_in_error_handling) {
		_response_str = std::string("HTTP/1.1 500 Internal Server Error\r\n"
			"Content-Type: text/html\r\n");
		FT_ASSERT(code == 500);
		_body =
			"<!DOCTYPE html>"
			"<head>"
			"    <title>500 Internal Server Error</title>"
			"</head>"
			"<body>"
			"    <h1>500 Internal Server Error</h1>"
			"</body>"
			"</html>";
		_mode = ResponseMode::FINISH_UP;
		return ;
	}
	_response_str = std::string("HTTP/1.1 ") + std::to_string(code) + " " + status + "\r\n"
		+ "Content-Type: text/html\r\n";
	if (code == 500) {
		_in_error_handling = true;
	}
	for (const auto& header: extra_headers) {
		_response_str += header;
	}
	_client_mode = ClientMode::BUILD_RESPONSE; // in case this was called from other call back like cgi manager
	std::string stat_code_path = _config.getErrorPages().getErrorPageLink(code);

	struct stat stats;
	if (stat(stat_code_path.c_str(), &stats) == -1) {
		load_status_code_response(500, "Internal Server Error");
		return ;
	}
	int	file_fd = open(stat_code_path.c_str(), O_CLOEXEC | O_RDONLY);
	if (file_fd < 0) {
		load_status_code_response(500, "Internal Server Error");
		return ;
	}
	read_fd(file_fd, stats.st_size, false);
	_mode = ResponseMode::FINISH_UP;

}

//don't use this if the response needs any custom data besides the status
//this response the respose
void	Response::load_status_code_response(int code, const std::string& status) {
	if (_in_error_handling) {
		_response_str = std::string("HTTP/1.1 500 Internal Server Error\r\n"
			"Content-Type: text/html\r\n");
		FT_ASSERT(code == 500);
		_body =
			"<!DOCTYPE html>"
			"<head>"
			"    <title>500 Internal Server Error</title>"
			"</head>"
			"<body>"
			"    <h1>500 Internal Server Error</h1>"
			"</body>"
			"</html>";
		_mode = ResponseMode::FINISH_UP;
		return ;
	}
	_response_str = std::string("HTTP/1.1 ") + std::to_string(code) + " " + status + "\r\n"
		+ "Content-Type: text/html\r\n";
	if (code == 500) {
		_in_error_handling = true;
	}
	_client_mode = ClientMode::BUILD_RESPONSE; // in case this was called from other call back like cgi manager
	std::string stat_code_path = _config.getErrorPages().getErrorPageLink(code);

	struct stat stats;
	if (stat(stat_code_path.c_str(), &stats) == -1) {
		LOG("stat on " << stat_code_path << " failed!\n");
		load_status_code_response(500, "Internal Server Error");
		return ;
	}
	int	file_fd = open(stat_code_path.c_str(), O_CLOEXEC | O_RDONLY);
	if (file_fd < 0) {
		LOG("open on " << stat_code_path << " failed!\n");
		load_status_code_response(500, "Internal Server Error");
		return ;
	}
	read_fd(file_fd, stats.st_size, false);
	_mode = ResponseMode::FINISH_UP;
}

void	Response::load_status_code_body(int code) {
	if (_in_error_handling) {
		load_status_code_response(500, "Internal Server Error");
		return ;
	}
	if (code == 500) {
		_in_error_handling = true;
	}
	std::string stat_code_path = _config.getErrorPages().getErrorPageLink(code);
	_response_str += "Content-Type: text/html\r\n";
	struct stat stats;
	if (stat(stat_code_path.c_str(), &stats) == -1) {
		load_status_code_response(500,"Internal Server Error");
		return ;
	}
	int	file_fd = open(stat_code_path.c_str(), O_CLOEXEC | O_RDONLY);
	if (file_fd < 0) {
		load_status_code_response(500,"Internal Server Error");
		return ;
	}
	read_fd(file_fd, stats.st_size, false);
	_mode = ResponseMode::FINISH_UP;
}

//change to std::filesystem::remove_all(_path) to remove directories recursivly
//currently it gives a 409 for not empty directories
void	Response::_handle_delete(void) {
	if (std::remove(_path.c_str()) == 0) {
		//success
		std::cout << FT_ANSI_RED " DELETE " FT_ANSI_RESET << _path << "\n";
		load_status_code_response(204, "No Content");
		return ;
	} else {
		std::cout << FT_ANSI_RED " DELETE " FT_ANSI_RESET << _path << " failed:\n";
		std::cout << strerror(errno) << std::endl;
		switch (errno) {
			case (ENOMEM):
				errno = 0;
				throw(std::bad_alloc());
				break ;
			case (EACCES):
			case (EFAULT):
			case (ENOENT):
			case (ENOTDIR):
				load_status_code_response(404, "Not Found");
				break;
			case (ENOTEMPTY):
				load_status_code_response(409, "Conflict");
				break ;
			case (EBADF):
			case (EROFS):
			case (EPERM):
			case (ENAMETOOLONG):
			case (ELOOP):
			case (EBUSY):
			case (EISDIR):
			case (EIO):
			case (EINVAL):
			default:
				load_status_code_response(500, "Internal Server Error");
				break ;
		}
		errno = 0;
	}
}

bool	Response::isMethodAllowed(MethodType method) {
	return std::find(_allowedMethods.begin(), _allowedMethods.end(), method) != _allowedMethods.end();
}

// method not allowed
void	Response::_handle405(void) {
	_response_str = "HTTP/1.1 405 Method Not Allowed\r\n";
	_response_str += "Allow:";
	if (!_allowedMethods.empty()) {
		_response_str += " ";
		for (auto it = _allowedMethods.cbegin(); it != _allowedMethods.cend(); it++) {
			_response_str += to_string(*it);
			if (it != _allowedMethods.cend() - 1) {
				_response_str += ", ";
			}
		}
	}
	_response_str += "\r\n";
	load_status_code_body(405);
}

void	Response::_finish_up(void) {
	_response_str +=
		"Connection: close\r\n"
		"Content-Length: " + std::to_string(_body.length()) + "\r\n"
			"\r\n"
		+ _body
	;
	_client_mode = ClientMode::SENDING;
	//for debugging: saves the body as file
	int debug_body_fd = open("body", O_WRONLY | O_CREAT | O_TRUNC, 0777);
	FT_ASSERT(debug_body_fd >  0);
	write(debug_body_fd, _body.c_str(), _body.length());
	close(debug_body_fd);
}

void	Response::overwrite_response_str(const std::string& new_response) {
	_response_str = new_response;
}

void	Response::execute(void) {
	// check if the status was changed from 200 by something outside the Response class
	int	status_code = _request._status_code.first;
	const std::string&	status_str = _request._status_code.second;
	if (status_code != 200) {
		load_status_code_response(status_code, status_str, _request.additional_response_headers);
		return ;
	}
	if (_first_iter) {
		_first_iter = false;
		_path = getExpandedTarget();
		//return if a status code file was requested
		if (_mode != ResponseMode::NORMAL) {
			return ;
		}
	}

	if (_mode == ResponseMode::NORMAL) {
		if (_location_config.getIsRedir()) {
			_handle_get_moved(_path, 301);
			_mode = ResponseMode::FINISH_UP;
			return ;
		} else if (isMethodAllowed(_request._type)) {
			switch (_request._type) {
				case (MethodType::GET): {
					_handle_get();
					break ;
				} case (MethodType::POST): {
					_handle_post();
					break ;
				} case (MethodType::PUT): {
					_handle_put();
					break ;
				} case (MethodType::DELETE): {
					_handle_delete();
					break ;
				} default: {
					_handle405();
					return ;
				}
			}
		} else {
			_handle405();
			return ;
		}
	} else if (_mode == ResponseMode::FINISH_UP) {
		/* for potential file reads:
		 * can not be done in same function call as initial if statement!
		*/
		_finish_up();
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

void Response::setAllowedMethods() {
	if (_location_config.isGetAllowed()) {
		_allowedMethods.push_back(MethodType::GET);
	}
	if (_location_config.isPostAllowed()) {
		_allowedMethods.push_back(MethodType::POST);
	}
	if (_location_config.isPutAllowed()) {
		_allowedMethods.push_back(MethodType::PUT);
	}
	if (_location_config.isPostAllowed()) {
		_allowedMethods.push_back(MethodType::DELETE);
	}
}

std::string	Response::getExpandedTarget(void) {
	std::string expandedPath;
	_location_config.printLocation();
	std::cout << "\n";

	std::cout << "--- _config.getRoot() " << _config.getRoot() + "\n";
	std::cout << "--- _location_config.getRoot() " << _location_config.getRoot() + "\n";
	std::cout << "--- _location_config.getPath() " << _location_config.getPath() + "\n";
	std::cout << "--- _location_config.getRedirection()" << _location_config.getRedirection() + "\n";
	std::cout << "--- request._uri " << _request._uri + "\n";

	std::cout << "\n";

	size_t	loc_path_len = _location_config.getPath().length();
	if (!_location_config.getIsRedir()) {
		expandedPath = _config.getRoot() + _location_config.getRoot();
		expandedPath += _request._uri.substr(loc_path_len, _request._uri.length() - loc_path_len);
	} else {
		expandedPath = _location_config.getRedirection()
			+ _request._uri.substr(
				loc_path_len, _request._uri.length() - loc_path_len);
		return (expandedPath);
	}

	std::cout << "RESPONSE PATH SETTED TO |" << expandedPath << "|\n";
	if (std::filesystem::exists(expandedPath)) {
		std::cout << "File exists: " << expandedPath << std::endl;
	} else if (_request.getMethod() == MethodType::GET) {
		std::cout << "File does not exist: " << expandedPath << std::endl;
		load_status_code_response(404, "Not Found");
		return ("");
	}
	return (expandedPath);
}

std::string&&	Response::move_str_response(void) {
	return (std::move(_response_str));
}

const std::string&	Response::get_str_response(void) const {
	return (_response_str);
}

CGIManager*&	Response::get_cgi_manger(void) {
	return (_cgi_manager);
}

WriteFd*&	Response::get_writer(void) {
	return (_writer);
}
