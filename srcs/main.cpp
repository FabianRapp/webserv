#include <Manager.hpp>
#include <config_parser.hpp>
#include <vector>
#include <Exceptions.hpp>
#include <Manager.hpp>

bool	exit_ = false;

int	main(int ac, char *av[]) {

	DataManager		manager;
	
	Config	config;
	config.port = 8080;

	manager.new_server(config);
	while (!exit_) {
		manager.run_poll();
		manager.execute_all();
		manager.process_closures();
	}
	return (0);
}
