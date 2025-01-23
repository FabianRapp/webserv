#include "../includes/Exceptions.hpp"

SendClientError::SendClientError(unsigned err_code, std::string title,
	std::string msg, bool close_connection)
:	title(title),
	msg(msg),
	err_code(err_code),
	close_connection(close_connection)
{
}
