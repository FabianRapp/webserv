#pragma once

#include <variant>

#include <fcntl.h>
#include <sys/stat.h>
#include "Manager.hpp"

class DataManager;

//placeholder
class Config {
public:
	int	port;
private:
};

class BaseFd {
public:
	BaseFd(DataManager& data, short poll_events):
		poll_events(poll_events),
		data(data),
		fd(-1),
		data_idx(std::numeric_limits<size_t>::max())
	{
	}

	virtual
	~BaseFd(void) {
		data.set_close(data_idx);
	}

	bool	is_ready(short event) {
		return (data.is_ready(data_idx, event));
	}

	virtual
	void	execute(void) = 0;

	const short		poll_events;
	DataManager&	data;
	int				fd;
	size_t			data_idx;
private:
};

class Server: public BaseFd {
public:
	Server(DataManager& data, Config& config)
	:	BaseFd(data, POLLIN)
	{
		this->config = config;
		//server init here
	}

	~Server(void) {
		//set all clients to close
		//don't call 'close()' here
	}

	void	execute(void) {
		// accept new clients
	}

	Config	config;

private:
};

class ReadFile: public BaseFd {
public:
	ReadFile(DataManager& data, std::string path):
		BaseFd(data, POLLIN)
	{
		struct stat		stats;
		std::memset(&stats, 0, sizeof stats);
		if (stat(path.c_str(), &stats) < 0) {
			assert(0);
		}
		left_over_bytes = stats.st_size;

		int read_flags = 0; /* todo: what flags */
		fd = open(path.c_str(), O_RDONLY, read_flags);
		assert(fd > 0);
	}
	
	~ReadFile(void) {
	}

	void	execute(void) {
		if (!is_ready(POLLIN)) {
			return ;
		}

		ssize_t read_ret = read(fd, buffer, sizeof buffer);
		assert(read_ret >= 0);


	}
	char								buffer[1024];
	ssize_t								left_over_bytes;
private:

};

class FileIo: public BaseFd {
public:
	typedef ssize_t (*t_read_fn)(int, void *, size_t);
	typedef ssize_t (*t_write_fn)(int, const void *, size_t);

	FileIo(DataManager& data, std::string path, IoMode mode): FileIo(data, path, mode, "") {}

	FileIo(DataManager& data, std::string path, IoMode mode, std::string&& input):
		BaseFd(data, mode == IoMode::READ ? POLLIN : POLLOUT)
	{
		this->input = std::move(input);

		if (mode == IoMode::READ) {
			struct stat		stats;
			std::memset(&stats, 0, sizeof stats);
			if (stat(path.c_str(), &stats) < 0) {
				assert(0);
			}
			left_over_bytes = stats.st_size;

			read_write = read;
			int read_flags = 0; /* todo: what flags */
			fd = open(path.c_str(), O_RDONLY, read_flags);
		} else if (mode == IoMode::WRITE) {
			left_over_bytes = static_cast<ssize_t>(this->input.size());
	
			read_write = write;
			int	write_flags = 0; /* todo: what flags */
			fd = open(path.c_str(), O_WRONLY, write_flags);
		} else {
			assert(0);
		}
		assert(fd > 0);
	}

	void	execute(void) {
		read_write(fd, buffer, left_over_bytes);
	}

	std::string	input;
	ssize_t		left_over_bytes;
	std::variant<t_read_fn, t_write_fn>	read_write;
private:

};
