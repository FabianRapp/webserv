#include <string>
#include <unordered_map>

class StatusCodes {
public:
	static std::unordered_map<unsigned long, std::string>	codes;

	StatusCodes(void) = delete;
	~StatusCodes(void) = delete;
};


void	init_status_codes(void);
