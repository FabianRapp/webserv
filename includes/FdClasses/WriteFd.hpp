#pragma once

#include "BaseFd.hpp"
#include  <functional>

class Response;
class WriteFd: public BaseFd {
public:
	WriteFd(DataManager& data, Response& response, const std::string_view& src, int fd, Client& client,
			std::function<void()> completion_callback);


	~WriteFd(void);

	void	execute(void) override;
	Client*	get_client(void);
	Server*	get_server(void);

protected:
	const std::string_view					src;
	size_t								pos;
	std::function<void()>				completion_callback;
	Client*								client;
	Server*								server;
	Response&							response;
};

