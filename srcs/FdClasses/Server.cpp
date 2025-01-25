#include "../../includes/FdClasses/Server.hpp"
#include "../../includes/Manager.hpp"

Server::Server(DataManager& data, ServerConfigFile& config):
	BaseFd(data, POLLIN),
	total_unique_clients(0),
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
	_set_non_blocking();
	if (fd < 0) {
		exit(errno);
	}

	memset(&server_addr, 0, sizeof(struct sockaddr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(config.getPort());

	if (bind(fd, server_addr_ptr, server_addr_len) < 0) {
		close(fd);
		std::cerr << "Error: bind:" << strerror(errno) << '\n';
		exit(errno);
	}
	if (listen(fd, REQUEST_QUE_SIZE) < 0) {
		close(fd);
		std::cerr << "Error: listen:" << strerror(errno) << '\n';
		exit(errno);
	}
	_set_non_blocking();
	//_listener.set_server_fd(server_fd);
	std::cout << "Started server on port " << config.getPort() << "...\n";
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
	this->total_unique_clients++;
	std::cout << "Added client numner " << this->total_unique_clients << "`\n";
}
