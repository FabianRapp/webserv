#include "../includes/Manager.hpp"
#include <vector>
#include <csignal>
#include "../includes/Exceptions.hpp"

//remove this, just for testing
#include "../includes/ConfigParser/LocationConfigFile.hpp"
#include "../includes/ConfigParser/ConfigParser.hpp"

volatile
sig_atomic_t	exit_ = 0;


void printHelloWebServer() {
std::cout << FT_ANSI_CYAN_BOLD << R"(
  (`\ .-') /`   ('-. .-. .-')          .-')      ('-.  _  .-')        (`-.      ('-.  _  .-')
   `.( OO ),' _(  OO)\  ( OO )        ( OO ).  _(  OO)( \( -O )     _(OO  )_  _(  OO)( \( -O )
,--./  .--.  (,------.;-----.\       (_)---\_)(,------.,------. ,--(_/   ,. \(,------.,------.
|      |  |   |  .---'| .-.  |       /    _ |  |  .---'|   /`. '\   \   /(__/ |  .---'|   /`. '
|  |   |  |,  |  |    | '-' /_)      \  :` `.  |  |    |  /  | | \   \ /   /  |  |    |  /  | |
|  |.'.|  |_)(|  '--. | .-. `.        '..`''.)(|  '--. |  |_.' |  \   '   /, (|  '--. |  |_.' |
|         |   |  .--' | |  \  |      .-._)   \ |  .--' |  .  '.'   \     /__) |  .--' |  .  '.'
|   ,'.   |   |  `---.| '--'  /      \       / |  `---.|  |\  \     \   /     |  `---.|  |\  \
'--'   '--'   `------'`------'        `-----'  `------'`--' '--'     `-'      `------'`--' '--'
)" << FT_ANSI_RESET << std::endl;
}

void	sig_int(int) {
	exit_ = 1;
}

void	webserv(int ac, char **av) {
	DataManager		manager;

	std::vector<ServerConfigFile>	all_configs;
	if (ac == 1) {
		manager.config_parser = new ConfigParser("config/m.conf");
	} else {
		manager.config_parser = new ConfigParser(av[1]);
	}
	all_configs = manager.config_parser->getServers();
	LOG("config count: " << all_configs.size() << std::endl);

	std::vector<ServerConfigFile>	matching_ports;
	while (all_configs.size()) {
		int	cur_port = all_configs[0].getPort();
		for (size_t i = 0; i < all_configs.size(); ) {
			if (all_configs[i].getPort() == cur_port) {
				all_configs[i].printServer();
				matching_ports.push_back(all_configs[i]);
				all_configs.erase(all_configs.begin() + static_cast<ssize_t>(i));
			} else {
				i++;
			}
		}
		manager.new_server(matching_ports);
		matching_ports.clear();
	}

	delete manager.config_parser;
	manager.config_parser = nullptr;
	LOG("entering main loop\n");
	while (!exit_) {
		//usleep(100000);
		manager.run_poll();
		manager.execute_all();
		manager.process_closures();
		manager.cgi_lifetimes.handle_timeouts();
	}
}

void	init(void) {
	printHelloWebServer();
	signal(SIGINT, sig_int);
	signal(SIGTSTP, sig_int);
}

int	main(int ac, char *av[]) {
	init();

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
