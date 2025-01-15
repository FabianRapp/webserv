#pragma once

#include <exception>
#include <string>

/* only throw main thread. For example when config file is wrong. */
class ForceFullExit: public std::exception {
public:
};

/* internal exception from within a server */
class ForceClientClosure: public std::exception {
public:
};

/* internal exception from within a server */
class SendClientError: public std::exception {
public:
	SendClientError(unsigned err_code, std::string title, std::string msg,
		bool close_connection);
	std::string	title;
	std::string	msg;
	unsigned	err_code;
	bool		close_connection;
};

/* out of memory, handle later...*/
class OOM: public std::exception {
public:
};
