#include <Manager.hpp>
#include <config_parser.hpp>

#include <fstream>

struct total_config	parse_config(char *config_file) {
	struct total_config	total_config;

	std::ifstream file(config_file);
	if (!file.is_open() )
	{
		throw std::runtime_error("Failed to open config file");
	}

	std::cout << "File opened succesfully" << std::endl;

	file.close();

	// memset(&total_config, 0, sizeof total_config);
	/*todo: parsing */
	return (total_config);
}

struct server_config	get_server_config(struct total_config total_config,
	size_t idx)
{
	FT_ASSERT(idx < total_config.server_count);

	struct server_config	server_config;

	memset(&server_config, 0, sizeof server_config);

	/*todo: fill out struct with all the data relevant to the specific server */

	return (server_config);
}
