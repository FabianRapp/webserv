#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>

struct location_config {
	std::string path;						// Path of the route (e.g., "/assets")
	bool	get_header;
	bool	post_header;
	bool	delete_header;
	bool	autoindex = false;					// Enable/disable directory listing (default: off)
	std::string upload_dir = "";			// Directory for uploads (optional)
	std::string index_file = "index.html"; // Default index file
};

struct server_config {
	int port = 0;				  			// e.g. 8080
	std::string server_name = ""; 			// e.g. example.com
	std::string root;			 			// e.g. root directory for files
	std::map<int, std::string> error_pages;	// Custom error pages (e.g., {404: "/errors/404.html"})
	std::vector<location_config> locations;	// List of locations
};

struct total_config {
	size_t server_count;
	std::vector<server_config> servers;
};

// std::string::npos - It represents a value that means "not found" or "no position" when working with string operations.

std::string trim_white_space_around(const std::string& str) {
	size_t start = str.find_first_not_of(" \t\n\r");
	size_t end = str.find_last_not_of(" \t\n\r");

	if (start == std::string::npos) {
		// If no non-whitespace character is found, return an empty string
		return "";
	} else {
		// Extract and return the substring from start to end
		return str.substr(start, end - start + 1);
	}
}


struct total_config parse_config(char *config_file) {
	struct total_config total_config;
	total_config.server_count = 0;

	std::ifstream file(config_file);
	if(!file.is_open() ){
		std::cout << "ERRROR: Cannot open the FILE" << std::endl;
	}
	std::cout << "File opened successfully:\n" << std::endl;

	bool in_server_block = false;
	bool in_location_block = false;

	server_config current_server; // temp object for current server block
	location_config current_location; // temp object for current location block

	int bracket_count = 0;
	std::string line;

	while(std::getline(file, line)) {
		line = trim_white_space_around(line);

		// skip any empty lines or comment lines
		if(line.empty() || line[0] == '#')
			continue;

		// check if "{" starts before "}"
		for(char c : line) {
			if(c == '{') {
				bracket_count++;
			} else if (c == '}') {
				bracket_count--;
			}
			if(bracket_count < 0) {
				std::cout << "ERROR: Closing brackets unmatched!" << std::endl;
				return total_config;
			}
		}

		if (line == "server {") {
			if (in_server_block) {
				std::cout << "ERROR: Nested server blocks are not allowed." << std::endl;
				continue;
			}
			in_server_block = true;
			current_server = server_config(); // Reset current server object
			continue;
		}

		// Detect end of a block (server or location)
		if (line == "}") {
			if (in_location_block) {
				current_server.locations.push_back(current_location); // Add completed location to server
				in_location_block = false;
			} else if (in_server_block) {
				total_config.servers.push_back(current_server); // Add completed server to total_config
				total_config.server_count++;
				in_server_block = false;
			}
			continue;
		}

		// Detect start of a location block within a server block
		if (line.find("location") == 0 && line.back() == '{') {
			if (!in_server_block) {
				std::cout << "ERROR: Location block outside of a server block.\n" << std::endl;
				continue;
			}
			in_location_block = true;
			current_location = location_config(); // Reset current location object

			// Extract path from "location /path {"
			current_location.path = line.substr(9, line.size() - 10);
			continue;
		}

		// Parse key-value pairs inside a location block
		if (in_location_block) {
			if (line.find("allowed_methods ") == 0) {
				current_location.get_header = (line.find("GET") != std::string::npos);
				current_location.post_header = (line.find("POST") != std::string::npos);
				current_location.delete_header = (line.find("DELETE") != std::string::npos);
			} else if (line.find("autoindex ") == 0) {
				current_location.autoindex = (line.substr(10) == "on");
			} else if (line.find("upload_dir ") == 0) {
				current_location.upload_dir = line.substr(11);
			} else if (line.find("index ") == 0) {
				current_location.index_file = line.substr(6);
			} else {
				//todo: EXIT
				std::cout << "ERROR: config parser: line " << __LINE__
					<< " file " << __FILE__ <<
					"\nInvalid data: " << line << std::endl;
				exit(1);
			}

			continue;
		}

		// Parse key-value pairs inside a server block
		if (in_server_block) {
			if (line.find("listen ") == 0) {
				current_server.port = std::stoi(line.substr(7));
			} else if (line.find("server_name ") == 0) {
				current_server.server_name = line.substr(12);
			} else if (line.find("root ") == 0) {
				current_server.root = line.substr(5);
			} else if (line.find("error_page ") == 0) {
				int code = std::stoi(line.substr(11, 3));
				current_server.error_pages[code] = line.substr(15);
			} else {
				//todo: EXIT
				std::cout << "ERROR: config parser: line " << __LINE__
					<< " file " << __FILE__ <<
					"\nInvalid data: " << line << std::endl;
				exit(1);
			}
		}
	}


		// std::cout << line << std::endl;
	// }
	// printf("bracket_count: ");
	// printf("%d", bracket_count);

	file.close();

	if(bracket_count != 0) {
		std::cout << "ERROR: Opening brackets unmatched!" << std::endl;
	}


	return (total_config);
}

#include <sys/stat.h>

int main(int ac, char **av)
{

	struct total_config total_config;
	total_config.server_count = 0;

	std::string	path = "main.cpp";

	struct stat	stats;

	if (stat(path.c_str(), &stats) == -1) {
		std::cerr << "Error: file: " << path << ": " << strerror(errno) << "\n";
	}

	if (ac < 2) {
		std::cerr << "Usage: ./program <config_file>" << std::endl;
		return EXIT_FAILURE;
	}

	struct total_config config = parse_config(av[1]);

	std::cout << "----\n";

	// Print parsed data for debugging purposes:
	std::cout << "Parsed servers: " << config.server_count << "\n";
	for(size_t i=0; i<config.servers.size(); i++) {
		std::cout<<"Server "<<i+1<<": port "<<config.servers[i].port<< std::endl;
		std::cout<<"Server "<<i+1<<": root "<<config.servers[i].root<< std::endl;
	}

	return EXIT_SUCCESS;
}

