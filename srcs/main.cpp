#include "../includes/Manager.hpp"
#include <vector>
#include <csignal>
#include "../includes/Exceptions.hpp"

volatile
sig_atomic_t	exit_ = 0;

void	sig_int(int) {
	exit_ = 1;
}

void	webserv(int ac, char **av) {
	DataManager		manager;

	std::vector<ServerConfigFile>	all_configs;
	try {
		if (ac == 1) {
			manager.config_parser = new ConfigParser("config/default.conf");
		} else {
			manager.config_parser = new ConfigParser(av[1]);
		}
		all_configs = manager.config_parser->getServers();
	} catch (const ConfigParseError& err) {
		std::cerr << "Config parse error: " << err.what() << "\n";
		return ;
	}

	for (auto & config : all_configs) {
		manager.new_server(config);
	}
	delete manager.config_parser;
	manager.config_parser = nullptr;
	while (!exit_) {
		manager.run_poll();
		manager.execute_all();
		manager.process_closures();
	}
}

int	main(int ac, char *av[]) {
	signal(SIGINT, sig_int);

start:
	try {
		webserv(ac, av);
	} catch (std::bad_alloc) {
		std::cerr << "Bad alloc!\nRestarting servers..\n";
		goto start;
	}
	return (0);
}
