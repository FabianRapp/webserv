#pragma once

#include <fcntl.h>
#include "../general_includes.hpp"

class BaseFd {
public:
	BaseFd(DataManager& data, short poll_events, const std::string& name);

	virtual
	~BaseFd(void);

	bool	is_ready(short event) const;

	//only call this from the manager
	virtual
	void	execute(void) = 0;

	const short			poll_events;
	DataManager&		data;
	int					fd;
	size_t				data_idx;
	std::string			name;//for debugging

	//be very carefull with calling this outside of the object itself
	virtual
	void	set_close(void);
protected:
	void	_set_non_blocking(void);
private:
};


