#pragma once

#include "BaseFd.hpp"

class WriteFd: public BaseFd {
public:
	WriteFd(DataManager& data, const std::string_view& src, int fd, bool close_fd,
			std::function<void()> completion_callback);

	~WriteFd(void);

	void	execute(void);

protected:
	const std::string_view					src;
	size_t								pos;
	std::function<void()>				completion_callback;
};

