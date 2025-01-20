#include <FdClasses.hpp>

BaseFd::BaseFd(DataManager& data, short poll_events):
	poll_events(poll_events),
	data(data),
	fd(-1),
	data_idx(std::numeric_limits<size_t>::max())
{
}

BaseFd::~BaseFd(void) {
	data.set_close(data_idx);
}

bool	BaseFd::is_ready(short event) const {
	return (data.is_ready(data_idx, event));
}

Server::Server(DataManager& data, Config& config):
	BaseFd(data, POLLIN),
	config(config)
{
	struct sockaddr_in		server_addr;
	const socklen_t			server_addr_len = static_cast<socklen_t>(sizeof server_addr);
	struct sockaddr	*const server_addr_ptr = reinterpret_cast<struct sockaddr *>(&server_addr);

	/* AF_INET : ipv4
	 * AF_INET6: ipv6 */
	init_status_codes(_codes);
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		std::cerr << "Error: server: socket: " << strerror(errno) << '\n';
		exit(errno);
	}
	fd = set_fd_non_block(fd);
	if (fd < 0) {
		exit(errno);
	}

	memset(&server_addr, 0, sizeof(struct sockaddr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(config.port);

	if (bind(fd, server_addr_ptr, server_addr_len) < 0) {
		close(fd);
		std::cerr << "Error: " << strerror(errno) << '\n';
		exit(errno);
	}
	if (listen(fd, REQUEST_QUE_SIZE) < 0) {
		close(fd);
		std::cerr << "Error: " << strerror(errno) << '\n';
		exit(errno);
	}
	//_listener.set_server_fd(server_fd);
	std::cout << "Started server on port " << config.port << "...\n";
}

Server::~Server(void) {
	//set all clients to close
	//don't call 'close()' here
}

void	Server::execute(void) {
	if (!is_ready(POLLIN)) {
		return ;
	}
	data.new_client(this);
}

ReadFd::ReadFd(DataManager& data, std::string& target_buffer, int fd,
		ssize_t byte_count, std::function<void()> completion_callback):
	BaseFd(data, POLLIN), target_buf(target_buffer),
	completion_callback(std::move(completion_callback))
{
	left_over_bytes = byte_count;
	this->fd = fd;
}

ReadFd::~ReadFd(void) {
}

void	ReadFd::execute(void) {
	if (!is_ready(POLLIN)) {
		return ;
	}
	size_t	read_size = sizeof buffer - 1 < static_cast<size_t>(left_over_bytes)
		? sizeof buffer - 1: static_cast<size_t>(left_over_bytes);
	ssize_t read_ret = read(fd, buffer, read_size);
	assert(read_ret >= 0);
	buffer[read_ret] = 0;
	left_over_bytes -= read_ret;
	target_buf += buffer;
	if (left_over_bytes == 0) {
		data.set_close(data_idx);
		completion_callback();
		return ;
	}
}

Client::Client(DataManager& data, Server* parent_server):
	BaseFd(data, POLLIN | POLLOUT),
	mode(ClientMode::RECEIVING),
	_parser(input)
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
	if (errno == EAGAIN || errno == EWOULDBLOCK) {
		FT_ASSERT(0 && "Should have been handled by poll");
	}
	//fd = set_fd_non_block(fd);
	//if (new_client_fd < 0) {
	//	FT_ASSERT(0);
	//}
	//_connections.add_client(new_client_fd);
	std::cout << "Connection accepted from address("
		<< inet_ntoa(addr.sin_addr) << "): PORT("
		<< ntohs(addr.sin_port) << ")\n";
}

Client::~Client(void) {
}

void	Client::execute(void) {
	switch (this->mode) {
		case (ClientMode::RECEIVING): {
			if (!is_ready(POLLIN)) {
				return ;
			}
			std::cout << "clinet exec\n";
			char		buffer[4096];
			int			recv_flags = 0;//MSG_ERRQUEUE <- has smth to do with error checks
			long int	bytes_read = recv(this->fd, buffer, sizeof buffer - 1, recv_flags);
			if (bytes_read < 0) {
				std::cerr << "Error: read failed\n";
				FT_ASSERT(0);
			}
			buffer[bytes_read] = 0;
			std::cout << "Read:\n" << buffer << '\n';
			this->input += buffer;

			/* todo: check for earlyer chunks of the msg etc.. */
			try {
				/* todo: this throw is just for testing */
				//throw (SendClientError(404, _codes[404], "testing", true));

				this->parse();
				bool testing_response = true;
				//if (testing_response || connection.completed_request()) {
				//	connection.current_mode = ClientMode::RECEIVING;
				//	t_http_request	request = connection.get_request();
				//	bool	placeholder_close_connection;
				//	std::string	response = _build_response(request, placeholder_close_connection);
				//	connection.set_response(std::move(response), placeholder_close_connection);
				//}
			} catch (const SendClientError& err) {
				//_default_err_response(connection, err);
			}

		}
		case (ClientMode::SENDING): {
		}
		case (ClientMode::READING_FILE): {
		}
		case (ClientMode::WRITING_FILE): {
		}
		case (ClientMode::READING_PIPE): {
		}
		case (ClientMode::WRITING_PIPE): {
		}
	}
}

void	Client::parse() {
}
