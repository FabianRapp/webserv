#include <Webserv.hpp>
#include <ClientConnections.hpp>
#include <config_parser.hpp>
#include <vector>
#include <Exceptions.hpp>




void	init(void) {
}


int	main(int ac, char *av[]) {
	init();
	
	std::vector<std::pair<std::thread, Webserv*>>	servers;
	

	struct total_config		total_config;
	struct server_config	server_config;

	std::pair<std::thread, Webserv*>	cur;
	try {
		if (ac == 1) {
			memset(&server_config, 0, sizeof server_config);
			cur.second = new Webserv(server_config);
			cur.first = std::thread(&Webserv::run, cur.second);
			servers.push_back(std::move(cur));
		} else if (ac == 2) {
			/*todo: try catch errors.. */
			total_config = parse_config(av[1]);
			for (size_t server_idx = 0; server_idx < total_config.server_count;
				server_idx++)
			{
				server_config = get_server_config(total_config, server_idx);
				cur.second = new Webserv(server_config);
				cur.first = std::thread(&Webserv::run, cur.second);
				servers.push_back(std::move(cur));
			}
		} else {
			std::cerr << "Error: Expected exactly 0(no config file) or "
				"1(with config file) arguments!\n";
			return (1);
		}
		for (auto & server : servers) {
			server.first.join();
		}
	} catch (const ForceFullExit& e) {
		for (auto & server : servers) {
			server.second->set_exit();
		}
		for (auto & server : servers) {
			server.first.join();
		}
	}
	FT_ASSERT(0 && "Should never reach return from main");
	return (0);
}
