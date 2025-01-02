#include <Webserv.hpp>
#include <StatusCodes.hpp>

std::unordered_map<unsigned long, std::string>	StatusCodes::codes;
void	init_status_codes(void) {
	/* append section for info:
	 * https://datatracker.ietf.org/doc/html/rfc2616#section- */
	std::pair<unsigned long, std::string>	defaults[] = {
		{100, "Continue"}, /* ; Section 10.1.1 */
		{101, "Switching Protocols"}, /* ; Section 10.1.2 */
		{200, "OK"}, /* ; Section 10.2.1 */
		{201, "Created"}, /* ; Section 10.2.2 */
		{202, "Accepted"}, /* ; Section 10.2.3 */
		{203, "Non-Authoritative Information"}, /* ; Section 10.2.4 */
		{204, "No Content"}, /* ; Section 10.2.5 */
		{205, "Reset Content"}, /* ; Section 10.2.6 */
		{206, "Partial Content"}, /* ; Section 10.2.7 */
		{300, "Multiple Choices"}, /* ; Section 10.3.1 */
		{301, "Moved Permanently"}, /* ; Section 10.3.2 */
		{302, "Found"}, /* ; Section 10.3.3 */
		{303, "See Other"}, /* ; Section 10.3.4 */
		{304, "Not Modified"}, /* ; Section 10.3.5 */
		{305, "Use Proxy"}, /* ; Section 10.3.6 */
		{307, "Temporary Redirect"}, /* ; Section 10.3.8 */
		{400, "Bad Request"}, /* ; Section 10.4.1 */
		{401, "Unauthorized"}, /* ; Section 10.4.2 */
		{402, "Payment Required"}, /* ; Section 10.4.3 */
		{403, "Forbidden"}, /* ; Section 10.4.4 */
		{404, "Not Found"}, /* ; Section 10.4.5 */
		{405, "Method Not Allowed"}, /* ; Section 10.4.6 */
		{406, "Not Acceptable"}, /* ; Section 10.4.7 */
		{407, "Proxy Authentication Required"}, /* ; Section 10.4.8 */
		{408, "Request Time-out"}, /* ; Section 10.4.9 */
		{409, "Conflict"}, /* ; Section 10.4.10 */
		{410, "Gone"}, /* ; Section 10.4.11 */
		{411, "Length Required"}, /* ; Section 10.4.12 */
		{412, "Precondition Failed"}, /* ; Section 10.4.13 */
		{413, "Request Entity Too Large"}, /* ; Section 10.4.14 */
		{414, "Request-URI Too Large"}, /* ; Section 10.4.15 */
		{415, "Unsupported Media Type"}, /* ; Section 10.4.16 */
		{416, "Requested range not satisfiable"}, /* ; Section 10.4.17 */
		{417, "Expectation Failed"}, /* ; Section 10.4.18 */
		{500, "Internal Server Error"}, /* ; Section 10.5.1 */
		{501, "Not Implemented"}, /* ; Section 10.5.2 */
		{502, "Bad Gateway"}, /* ; Section 10.5.3 */
		{503, "Service Unavailable"}, /* ; Section 10.5.4 */
		{504, "Gateway Time-out"}, /* ; Section 10.5.5 */
		{505, "HTTP Version not supported"}, /* ; Section 10.5.6 */
	};
	for (auto & status : defaults) {
		StatusCodes::codes[(status.first)] = status.second;
	}
}

void	init(void) {
	init_status_codes();
}

int	main(int ac, char *av[]) {
	init();
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
