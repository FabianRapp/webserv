#include <Manager.hpp>
#include <config_parser.hpp>
#include <vector>
#include <Exceptions.hpp>
#include <Manager.hpp>
#include <ConfigParser.hpp>

bool	exit_ = false;

int	main(int ac, char *av[]) {
	DataManager		manager;

	ConfigParser	*parser;
	if (ac == 1) {
		parser = new ConfigParser("config/default.conf");
	} else {
		parser = new ConfigParser(av[1]);
	}
	std::vector<ServerConfigFile> all_configs = parser->getServers();

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
