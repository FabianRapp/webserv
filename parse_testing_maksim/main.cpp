#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>


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
	int port = 0;				  			// e.g. 8080
	std::string server_name = ""; 			// e.g. example.com
	std::string root;			 			// e.g. root directory for files
	std::map<int, std::string> error_pages;	// Custom error pages (e.g., {404: "/errors/404.html"})
	std::vector<location_config> locations;	// List of locations
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

	std::ifstream file(config_file);

	if(!file.is_open() ){
		std::cout << "ERRROR: Cannot open the FILE" << std::endl;
	}
	std::cout << "File opened successfully:\n" << std::endl;

	// std::string line;
	// while (std::getline(file, line)) {
	// 	std::cout << line << std::endl;
	// }

	// std::string randomLine;
	// std::getline(file, randomLine);
	// trim_white_space_around(randomLine); // does not work
	// std::cout << randomLine << std::endl;

	// std::string randomLine2;
	// std::getline(file, randomLine2);
	// trim_white_space_around(randomLine2); // does not work
	// std::cout << randomLine2 << std::endl;

	// std::string randomLine3;
	// std::getline(file, randomLine3);

	// std::cout << trim_white_space_around(randomLine3) << std::endl; //works here like this.


	std::string line;
	while(std::getline(file, line)) {
		line = trim_white_space_around(line);

		if (line.empty() || line[0] == '#')
			continue;
		std::cout << line << std::endl;
	}

	file.close();

	return (total_config);
}


int main(int ac, char **av)
{
	std::cout << "ac: " << ac << std::endl;
	// parse_config(av[1]);
	parse_config(av[1]);

	std::cout << "----" << std::endl;




	return (0);
}

