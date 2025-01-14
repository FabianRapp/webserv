#pragma once

#include "./parser/Parser.hpp"
#include "types.hpp"
#include <unistd.h>
#include "Request.hpp"

// class ClientConnection {
// 	public:
// 		ClientConnection(const t_fd & fd, int idx);
// 		~ClientConnection(void);

// 		const t_fd		&fd;
// 		std::string		input;
// 		void			parse(void);
// 		bool			completed_request(void);
// 		/* only call this if completed_request returned true */
// 		t_http_request	get_request(void) const;
// 		int				client_idx;

// 	private:
// 		Parser			_parser;
// };

class ClientConnection {
	public:
		ClientConnection(const t_fd & fd, int idx);
		~ClientConnection(void);

		const t_fd		&fd;
		std::string		input;
		void			parse(void);
		bool			completed_request(void);
		/* only call this if completed_request returned true */
		Request			get_request(void) const;
		int				client_idx;

	private:
		Parser			_parser;
};
