#include "../includes/Manager.hpp"
#include <vector>
#include <csignal>

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

bool	port_conflict(std::vector<ServerConfigFile>& matching_ports, int ac, char **av) {
	if (matching_ports.size() > 1) {
		LOG(FT_ANSI_RED_BOLD_UNDERLINE "Warning: Multiple serverconfigs on "
			"the same port!\n" FT_ANSI_RESET);
		LOG(FT_ANSI_RED "Do you want to continue running multiple servers on the same"
			" port[y/Y] or do you want to exit (any other input)?\n"
			FT_ANSI_RESET);
		LOG(FT_ANSI_WHITE_BOLD "Input: " FT_ANSI_RESET);
		if (ac >= 3 && std::string(av[2]) == "-y") {
			LOG("y (auto yes)\n");
			return (false);
		}
		std::string	line;
		std::getline(std::cin, line);
		if (line != "y" && line != "Y") {
			LOG(FT_ANSI_RED "Error: Servers in config have the same ports!\n");
			return (true);
		}
		LOG("For automatic 'y' run the webserv with './webserv <config> -y'.\n");
	}
	return (false);
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
	LOG("Config count: " << all_configs.size() << std::endl);
	if (all_configs.size() == 0) {
		LOG("No config, exiting..\n");
		return ;
	}
	std::vector<ServerConfigFile>	matching_ports;
	while (all_configs.size()) {
		int	cur_port = all_configs[0].getPort();
		for (size_t i = 0; i < all_configs.size(); ) {
			if (all_configs[i].getPort() == cur_port) {
				//all_configs[i].printServer();
				matching_ports.push_back(all_configs[i]);
				all_configs.erase(all_configs.begin() + static_cast<ssize_t>(i));
			} else {
				i++;
			}
		}
		if (port_conflict(matching_ports, ac, av)) {
			return ;
		}
		manager.new_server(matching_ports);
		matching_ports.clear();
	}

	delete manager.config_parser;
	manager.config_parser = nullptr;

	if (!manager.get_current_count()) {
		LOG(FT_ANSI_RED_BOLD "Warning: Every server launch failed\n" FT_ANSI_RESET);
		return ;
	}
	LOG(FT_ANSI_GREEN "Entering main loop with " FT_ANSI_RESET
		FT_ANSI_GREEN_UNDERLINE << manager.get_current_count() << FT_ANSI_RESET
		FT_ANSI_GREEN " socket(s) for the server(s).\n" FT_ANSI_RESET);
	while (!exit_) {
		//usleep(100000);
		manager.run_poll();
		manager.execute_all();
		manager.process_closures();
		manager.cgi_lifetimes.handle_timeouts();
	}
	if (std::cerr.fail()) {
		std::cerr.clear();
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
		std::cerr << FT_ANSI_RED_BOLD << err.what() << FT_ANSI_RESET << std::endl;
	} catch (const DefaultErrorPagesError& err) {
		std::cerr << FT_ANSI_RED_BOLD << err.what() << FT_ANSI_RESET << std::endl;
	} catch (const std::bad_alloc&) {
		std::cerr << FT_ANSI_RED_BOLD << "Bad alloc!\nRestarting servers.." << FT_ANSI_RESET << std::endl;
		goto start;
	} catch (const std::ios_base::failure& e) {
		std::cerr << FT_ANSI_RED_BOLD << "Error: " << e.what() << FT_ANSI_RESET << std::endl;
		std::cerr << FT_ANSI_YELLOW_BOLD << "Restarting servers.." << FT_ANSI_RESET << std::endl;
		goto start;
	} catch (const ChildError&) {
		return (1);
	}
	LOG("exiting..\n");
	return (0);
}
