#include "../../includes/FdClasses/Server.hpp"
#include "../../includes/Manager.hpp"

Server::Server(DataManager& data, std::vector<ServerConfigFile>& configs):
	BaseFd(data, POLLIN, "Server"),
	total_unique_clients(0),
	configs(configs),
	start_panic(false),
	panic(false)
{
	struct sockaddr_in		server_addr;
	const socklen_t			server_addr_len = static_cast<socklen_t>(sizeof server_addr);
	struct sockaddr	*const server_addr_ptr = reinterpret_cast<struct sockaddr *>(&server_addr);

	/* AF_INET : ipv4
	 * AF_INET6: ipv6 */
	init_status_codes(_codes);
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		throw(ServerError("server: socket: " + std::string(strerror(errno))));
		return ;
	}

	int	option_val = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &option_val, sizeof option_val) < 0) {
		ft_close(fd);
		throw(ServerError("server: setsockopt: " + std::string(strerror(errno))));
	}
	_set_non_blocking();
	if (fd < 0) {
		throw(ServerError("server: set non blocking: " + std::string(strerror(errno))));
		return ;
	}


	memset(&server_addr, 0, sizeof(struct sockaddr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(static_cast<uint16_t>(configs[0].getPort()));

	if (bind(fd, server_addr_ptr, server_addr_len) < 0) {
		ft_close(fd);
		throw(ServerError("server: bind: " + std::string(strerror(errno))));
		return ;
	}
	if (listen(fd, REQUEST_QUE_SIZE) < 0) {
		ft_close(fd);
		throw(ServerError("server: listen: " + std::string(strerror(errno))));
		return ;
	}
	std::cout << "Started server on port " << configs[0].getPort() << "...\n";
}

Server::~Server(void) {
	//set all clients to close
	//don't call 'close()' here
}

void	Server::execute(void) {
	//todo: implement a very basic rejection response if there are too many clients
	if (!is_ready(POLLIN) || data.get_count() > 500) {
		return ;
	}
	data.new_client(this);
	this->total_unique_clients++;
	std::cout << "Added client numner " << this->total_unique_clients << "`\n";
}
