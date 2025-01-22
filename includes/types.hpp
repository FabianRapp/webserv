#pragma once
#include <cstdlib>

#include <string>
#include <vector>
#include <map>

typedef int	t_fd;

class DataManager;
class BaseFd;
class Server;
class Client;
class ReadFd;


struct total_config {
	size_t	server_count;
	// std::vector<server_config> servers;
};

struct location_config {
	std::string path;						// Path of the route (e.g., "/assets")
	bool	get_header;
	bool	post_header;
	bool	delete_header;
	// std::vector<std::string> methods;		// Allowed HTTP methods (e.g., GET, POST)
	bool	autoindex = false;					// Enable/disable directory listing (default: off)
	std::string upload_dir = "";			// Directory for uploads (optional)
	std::string index_file = "index.html"; // Default index file
};

struct server_config {
	unsigned short	port;
			  			// e.g. 8080
	std::string server_name = ""; 			// e.g. example.com
	std::string root;			 			// e.g. root directory for files
	std::map<int, std::string> error_pages;	// Custom error pages (e.g., {404: "/errors/404.html"})
	std::vector<location_config> locations;	// List of locations
};
