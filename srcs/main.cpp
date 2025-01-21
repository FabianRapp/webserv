#include <Manager.hpp>
#include <config_parser.hpp>
#include <vector>
#include <Exceptions.hpp>
#include <Manager.hpp>
#include <FdClasses.hpp>

bool	exit_ = false;

void	init(void) {

}


int	main(int ac, char *av[]) {
	init();

	DataManager		manager;
	
	Config	config;
	config.port = 8080;

	Server *serv = manager.new_server(config);
	while (!exit_) {
		manager.run_poll();
		manager.execute_all();
		manager.process_closures();
	}
//	std::vector<std::pair<std::thread, Webserv*>>	servers;
//	
//
//	struct total_config		total_config;
//	struct server_config	server_config;
//
//	std::pair<std::thread, Webserv*>	cur;
//	try {
//		if (ac == 1) {
//			memset(&server_config, 0, sizeof server_config);
//			server_config.port = 8080;
//			cur.second = new Webserv(server_config);
//			cur.first = std::thread(&Webserv::run, cur.second);
//			servers.push_back(std::move(cur));
//
//			server_config.port = 80;
//			cur.second = new Webserv(server_config);
//			cur.first = std::thread(&Webserv::run, cur.second);
//			servers.push_back(std::move(cur));
//		} else if (ac == 2) {
//			/*todo: try catch errors.. */
//			total_config = parse_config(av[1]);
//			for (size_t server_idx = 0; server_idx < total_config.server_count;
//				server_idx++)
//			{
//				server_config = get_server_config(total_config, server_idx);
//				cur.second = new Webserv(server_config);
//				cur.first = std::thread(&Webserv::run, cur.second);
//				servers.push_back(std::move(cur));
//			}
//		} else {
//			std::cerr << "Error: Expected exactly 0(no config file) or "
//				"1(with config file) arguments!\n";
//			return (1);
//		}
//		for (auto & server : servers) {
//			server.first.join();
//		}
//	} catch (const ForceFullExit& e) {
//		exit_.store(true);
//		for (auto & server : servers) {
//			server.first.join();
//		}
//	}
	//FT_ASSERT(0 && "Should never reach return from main");
	return (0);
}
