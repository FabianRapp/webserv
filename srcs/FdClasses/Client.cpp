#include "../../includes/FdClasses/Client.hpp"
#include "../../includes/FdClasses/Server.hpp"
#include "../../includes/Manager.hpp"
#include "../../includes/macros.h"

//todo: constructor err handling
Client::Client(DataManager& data, Server* parent_server):
	BaseFd(data, POLLIN | POLLOUT, "Client"),
	mode(ClientMode::RECEIVING),
	_response_builder({""}),
	_send_data({"", 0, false}),
	_parser(input),
	_writer(nullptr),
	_last_availability(std::chrono::high_resolution_clock::now())
{
	this->server = parent_server;
	assert(server->is_ready(POLLIN));
	errno = 0;
	struct sockaddr_in		addr;
	socklen_t				addr_len = static_cast<socklen_t>(sizeof addr);
	struct sockaddr			*const addr_ptr =
		reinterpret_cast<struct sockaddr *>(&addr);
	memset(&addr, 0, sizeof addr);// might not be needed
	fd = accept(server->fd, addr_ptr, &addr_len);

	// todo: this if statement is only for debugging and should not stay for
	// submission
	if (errno == EAGAIN || errno == EWOULDBLOCK) {
		FT_ASSERT(0 && "Should have been handled by poll");
	}

	_set_non_blocking();
	std::cout << "Connection accepted from address("
		<< inet_ntoa(addr.sin_addr) << "): PORT("
		<< ntohs(addr.sin_port) << ")\n";
}

Client::~Client(void) {
}

void	Client::_receive_request(void) {
	if (!is_ready(POLLIN)) {
		std::cout << "not ready\n";
		auto now = std::chrono::high_resolution_clock::now();
		auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - _last_availability).count();
		if (elapsed_ms > 2000) {
			std::cerr << "client " << data_idx << " timed out..\n";
			set_close();
		}
		return ;
	}
	_last_availability = std::chrono::high_resolution_clock::now();
	char		buffer[4096];
	int			recv_flags = 0;//MSG_ERRQUEUE <- has smth to do with error checks
	long int	bytes_read = recv(this->fd, buffer, sizeof buffer - 1, recv_flags);
	if (bytes_read < 0) {
		std::cerr << "Error: read failed\n";
		set_close();
		return ;
	}
	buffer[bytes_read] = 0;
	std::cout << "Read:\n" << buffer << '\n';
	this->input += buffer;

	/* todo: check for earlyer chunks of the msg etc.. */
	try {
		/* todo: this throw is just for testing */
		//throw (SendClientError(404, _codes[404], "testing", true));

		this->parse();
		//std::cout << "Request STATUS = " << _parser.is_finished() << std::endl;
		if (_parser.is_finished() == true) {
			_request = _parser.get_request();
			mode = ClientMode::BUILD_RESPONSE;
			_send_data.pos = 0;
			_send_data.response = "";
			_send_data.close_after_send = true; /* placeholder */
		}
	} catch (const SendClientError& err) {
		//_default_err_response(connection, err);
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
void	Client::_read_fd(ClientMode next_mode, int read_fd, ssize_t byte_count, bool close_fd) {
	_fd_error.error = false;
	FT_ASSERT(read_fd > 0);
	mode = ClientMode::READING_FD;
	_reader = data.new_read_fd(
		_response_builder.body,
		read_fd,
		byte_count,
		close_fd,
		[this, next_mode] () {
			this->mode = next_mode;
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
void	Client::_write_fd(ClientMode next_mode, int write_fd, bool close_fd) {
	_fd_error.error = false;
	FT_ASSERT(write_fd > 0);
	mode = ClientMode::WRITING_FD;
	_writer = data.new_write_fd(
		write_fd,
		_fd_write_data,
		close_fd,
		[this, next_mode] () {
			this->mode = next_mode;
			this->_writer = nullptr;
			//todo: maybe clear <this->_fd_write_data> here
		}
	);
}

ServerConfigFile&	select_config(std::vector<ServerConfigFile>& server_configs,
						Request& request)
{
	if (request._headers.find(HeaderType::HOST) == request._headers.end()) {
		return (server_configs[0]);
	}
	std::string	to_match = request._headers[HeaderType::HOST];
	//not sure which one
	//std::string	to_match = request._headers[HeaderType::AUTHORIZATION];

	std::transform(to_match.begin(), to_match.end(), to_match.begin(),
		[](unsigned char c) { return (std::tolower(c));});

	for (ServerConfigFile& config : server_configs) {
		const std::vector<std::string>&	names = config.getServerNames();
		for (const auto& name : names) {
			if (name == to_match) {
				return (config);
			}
		}
	}
	return (server_configs[0]);
}

// assumes dir to be a valid directory
// check errno for potential errors
std::vector<std::string>	get_dir(std::string dir_path) {
	DIR*	dir = opendir(dir_path.c_str()); // todo: has to be part of some class for err handling
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

std::string	auto_index_body(Request& request, std::vector<std::string>& files) {
	std::string body =
		"<!DOCTYPE HTML>"
		"<html>"
		" <head>"
		"  <title>Index of " + request._uri + "</title>"
		" </head>"
		" <body>"
		"  <h1>Index of " + request._uri + "</h1>"
		"  <hr>";
	for (auto& file : files) {
			body += "  <a href=\"" + request._uri + file + "\">" + file + "</a><br>\n";
	}
	body +=
		"  </pre>"
		"  <hr>"
		" </body>"
		"</html>"
	;
	return (body);
}

// called if path is a directory
void	Client::_handle_auto_index(std::string& path,
			std::vector<std::string>&files, ServerConfigFile& config) {

	std::string	body = auto_index_body(_request, files);
	std::string& response = _send_data.response;
	response =
		std::string("HTTP/1.1 200 OK\r\n")
		+ "Content-Type: text/html\r\n"
		"Content-Length: " + std::to_string(body.size()) + "\r\n"
		"Connection: close\r\n"
		"\r\n"
		+ body
	;
	mode = ClientMode::SENDING;
}

// path is a file that is not CGI
// todo: err handling
void	Client::_handle_get_file(const std::string& path, ServerConfigFile& config) {
	std::string&	body = _response_builder.body;
	if (body.empty()) {
		struct stat stats;
		FT_ASSERT(stat(path.c_str(), &stats) != -1);
		int	file_fd = open(path.c_str(), O_RDONLY);
		FT_ASSERT(file_fd >0);
		_read_fd(mode, file_fd, stats.st_size, true);
		return ;
	}
	std::string&	response = _send_data.response;
	_send_data.pos = 0;
	_send_data.close_after_send = true;
	response =
		std::string("HTTP/1.1 200 OK\r\n")
		+ "Content-Length: " + std::to_string(body.size()) + "\r\n"
		"Connection: close\r\n"
		"\r\r"
		+ body
	;
	body = "";
	mode = ClientMode::SENDING;
}

//todo: commented lines
void	Client::_handle_get(std::string& path, ServerConfigFile& config) {
	if (std::filesystem::is_directory(path)) {
		if (_request._uri.back() != '/') {
			std::string new_location = _request._uri + "/";
			std::string redirect_response =
				"HTTP/1.1 301 Moved Permanently\r\n"
				"Location: " + new_location + "\r\n"
				"Connection: close\r\n"
				//"Content-Length: " + len(301 err html file) + "\r\n"
				"\r\n";
				//+ 301 err html file
			mode = ClientMode::SENDING;
			return ;
		}
		std::vector<std::string>	files = get_dir(path);
		/*
		std::string					index_file;
		if (has_index(files, config, index_file)) {
			path = index_file;
		} else if (enabled_auto_index(path, config)) {
		*/
			_handle_auto_index(path, files, config);
			return ;
		/*
		} else {
			handle invlaid request
			return ;
		}
		*/
	}
	/*
	if (does not exist(path)) {
	}
	*/
	FT_ASSERT(!std::filesystem::is_directory(path));
	/*
	if (is_cgi(path, config)) {
		_handle_cgi(path, config);
	} else */{
		_handle_get_file(path, config);
	}
}

void	Client::_handle_post(std::string& path, ServerConfigFile& config) {
}

void	Client::_handle_delete(std::string& path, ServerConfigFile& config) {
}

void	Client::_execute_response(void) {
	_send_data.pos = 0;
	_send_data.close_after_send = true;

	ServerConfigFile&	config = select_config(server->configs, _request);
	std::string	path /*= expand_uri(_request.uri, config)*/;
	path = getenv("PWD");//placeholder

	switch (_request._type) {
		case (MethodType::GET): {
			_handle_get(path, config);
			break ;
		} case (MethodType::POST): {
			_handle_post(path, config);
			break ;
		} case (MethodType::DELETE): {
			_handle_delete(path, config);
			break ;
		} default: {
			std::cerr << "Error: Unsupported request type: "
				<< to_string(_request._type) << "\n";
			//todo: 405 err
			FT_ASSERT(0);
		}
	}
}

// diff test_file.txt test_file_cmp.txt
// -> should not show any diff
// can be used as an intermediate mode for this->mode before entering SENDING mode
void	Client::_test_write_fd() {
	int fd = open("test_file.txt", O_WRONLY | O_APPEND | O_NONBLOCK | O_CREAT | O_TRUNC, 0666);
	int fd2 = open("test_file_cmp.txt", O_WRONLY | O_APPEND | O_CREAT | O_TRUNC, 0666);
	FT_ASSERT(fd > 0);
	_fd_write_data = std::string_view(_send_data.response.c_str(), _send_data.response.size());
	write(fd2, _fd_write_data.data(), _fd_write_data.size());
	close(fd2);
	_write_fd(ClientMode::SENDING, fd, true);
	//mode = ClientMode::SENDING;
}

void	Client::execute(void) {
	std::cout << FT_ANSI_GREEN "Client " << this->data_idx << ": ";
	std::cout << FT_ANSI_RESET;
	switch (this->mode) {
		case (ClientMode::RECEIVING): {
			std::cout << "receiving\n";
			_receive_request();
			break ;
		}
		case (ClientMode::BUILD_RESPONSE): {
			std::cout << "exec\n";
			_execute_response();
			break ;
		}
		case (ClientMode::SENDING): {
			std::cout << "sending\n";
			_send_response();
			break ;
		}
		case (ClientMode::READING_FD):
		case (ClientMode::WRITING_FD): {
			std::cout << "read/write fd\n";
			// do nothing
			break ;
		}
		case (ClientMode::TESTING_MODE): {
			std::cout << "testing mode\n";
			//_test_write_fd();
			break ;
		}
	}
}

void	Client::parse() {
	_parser.parse();
}

void	Client::_send_response(void) {
	if (!this->is_ready(POLLOUT)) {
		std::cout << "not rdy\n";
		FT_ASSERT(this->poll_events & POLLOUT);
		auto now = std::chrono::high_resolution_clock::now();
		auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - _last_availability).count();
		if (elapsed_ms > 2000) {
			std::cerr << "client " << data_idx << " timed out..\n";
			set_close();
		}
		return ;
	}
	_last_availability = std::chrono::high_resolution_clock::now();
	{
		/* todo: poll to catch potential issues: remove later */
		struct pollfd	test_poll = {fd, POLLOUT, 0};
		poll(&test_poll, 1, 0);
		FT_ASSERT(test_poll.revents & POLLOUT);
	}

	const int send_flags = 0;
	//std::cout << "sending:\n" << _send_data.response << "\n";
	ssize_t send_bytes = send(
		fd,
		_send_data.response.c_str() + _send_data.pos,
		_send_data.response.size() - _send_data.pos,
		send_flags
	);
	if (send_bytes <= 0) {
		std::cerr << "Error: send: closing connection now\n";
		//todo: the line below has to removed before submission according to subject
		std::cerr << "err: " << strerror(errno) << '\n';
		set_close();
		return ;
	}
	_send_data.pos += static_cast<size_t>(send_bytes);
	if (_send_data.pos == _send_data.response.size()) {
		mode = ClientMode::RECEIVING;
		_send_data.response = "";
		_send_data.pos = 0;
		if (_send_data.close_after_send) {
			set_close();
		}
	}
}
