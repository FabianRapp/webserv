#include "../includes/Manager.hpp"
#include "../includes/ConfigParser/ConfigParser.hpp"
#include <vector>
#include <csignal>
#include "../includes/Exceptions.hpp"

volatile
sig_atomic_t	exit_ = 0;

void	sig_int(int) {
	exit_ = 1;
}

int	main(int ac, char *av[]) {
	signal(SIGINT, sig_int);
	DataManager		manager;

	ConfigParser					*parser;
	std::vector<ServerConfigFile>	all_configs;
	try {
		if (ac == 1) {
			parser = new ConfigParser("config/default.conf");
		} else {
			parser = new ConfigParser(av[1]);
		}
		all_configs = parser->getServers();
	} catch (const ConfigParseError& err) {
		std::cerr << "Config parse error: " << err.what() << "\n";
		exit(1);
	}

	for (auto & config : all_configs) {
		manager.new_server(config);
	}
	delete parser;
	while (!exit_) {
		manager.run_poll();
		manager.execute_all();
		manager.process_closures();
	}
	return (0);
}
