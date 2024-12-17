#include <Webserv.hpp>


int	main(int ac, char *av[]) {
	if (ac == 1) {
		Webserv	serv;
		serv.run();
	} else if (ac == 2) {
		Webserv	serv(av[1]);
		serv.run();
	} else {
		std::cerr << "Error: Expected exactly 0(no config file) or "
			"1(with config file) arguments!\n";
		return (1);
	}
	FT_ASSERT(0 && "Should never reach return from main");
	return (0);
}
