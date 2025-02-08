#include "../includes/Manager.hpp"
#include <vector>
#include <csignal>
#include "../includes/Exceptions.hpp"

//remove this, just for testing
#include "../includes/ConfigParser/LocationConfigFile.hpp"
#include "../includes/ConfigParser/ConfigParser.hpp"

volatile
sig_atomic_t	exit_ = 0;

void	sig_int(int) {
	exit_ = 1;
}

void	webserv(int ac, char **av) {
	DataManager		manager;

	std::vector<ServerConfigFile>	all_configs;
	if (ac == 1) {
		manager.config_parser = new ConfigParser("config/default1.conf");
	} else {
		manager.config_parser = new ConfigParser(av[1]);
	}
	all_configs = manager.config_parser->getServers();
	//todo: dont use std::sort
	sort(all_configs.begin(), all_configs.end(),
		[](ServerConfigFile&a, ServerConfigFile&b) {
			return (a.getPort() - b.getPort());
		}
	);

	std::vector<ServerConfigFile>	matching_ports;
	for (auto & config : all_configs) {
		config.printServer();
		config.getDefaultLocation().printLocation();
		if (matching_ports.size() == 0 || matching_ports[0].getPort() == config.getPort()) {
			matching_ports.push_back(config);
			continue ;
		}
		manager.new_server(matching_ports);
		matching_ports.clear();
		matching_ports.push_back(config);
	}
	if (matching_ports.size()) {
		manager.new_server(matching_ports);
	}

	delete manager.config_parser;
	manager.config_parser = nullptr;

	while (!exit_) {
		//usleep(100000);
		manager.run_poll();
		manager.execute_all();
		manager.process_closures();
		manager.cgi_lifetimes.handle_timeouts();
	}
}

int	main(int ac, char *av[]) {
	signal(SIGINT, sig_int);
	signal(SIGTSTP, sig_int);

	//todo: remove this, just for testing

start:
	try {
		errno = 0;
		webserv(ac, av);
	} catch (const ConfigParseError& err) {
		std::cerr << err.what() << "\n";
	} catch (const std::bad_alloc&) {
		std::cerr << "Bad alloc!\nRestarting servers..\n";
		goto start;
	} catch (const std::ios_base::failure& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		std::cerr << "Restarting servers..\n";
		goto start;
	}
	return (0);
}
