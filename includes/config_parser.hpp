#pragma once
#include "types.hpp"

struct server_config	get_server_config(struct total_config total_config, size_t idx);
struct total_config		parse_config(char *config_file);

