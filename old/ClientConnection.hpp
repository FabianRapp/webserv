// #pragma once

// #include "./parser/Parser.hpp"
// #include "types.hpp"
// #include <unistd.h>
// #include <sys/socket.h>

// enum class ConnectionMode {
// 	RESEIVING,
// 	SENDING,
// };

// class ClientConnection {
// public:
// 					ClientConnection(const t_fd & fd, int idx);
// 					~ClientConnection(void);
// 	ConnectionMode	current_mode;
// 	const t_fd		&fd;
// 	std::string		input;
// 	void			parse(void);
// 	bool			completed_request(void);
// 	/* only call this if completed_request returned true */
// 	t_http_request	get_request(void) const;
// 	int				client_idx;
// 	bool			close_after_loop;

// 	void			set_response(std::string&& output, bool close_after_send);
// 	void			send_response(void);
// 	bool			finished_sending(void) const;
// 	bool			should_close_after_send(void) const;

// private:
// 	struct send_data {
// 		std::string	response;
// 		size_t		pos;
// 		bool		close_after_send;
// 	}	_send_data;
// 	Parser			_parser;
// #include "Request.hpp"

// // class ClientConnection {
// // 	public:
// // 		ClientConnection(const t_fd & fd, int idx);
// // 		~ClientConnection(void);

// // 		const t_fd		&fd;
// // 		std::string		input;
// // 		void			parse(void);
// // 		bool			completed_request(void);
// // 		/* only call this if completed_request returned true */
// // 		t_http_request	get_request(void) const;
// // 		int				client_idx;

// // 	private:
// // 		Parser			_parser;
// // };

// class ClientConnection {
// 	public:
// 		ClientConnection(const t_fd & fd, int idx);
// 		~ClientConnection(void);

// 		const t_fd		&fd;
// 		std::string		input;
// 		void			parse(void);
// 		bool			completed_request(void);
// 		/* only call this if completed_request returned true */
// 		Request			get_request(void) const;
// 		int				client_idx;

// 	private:
// 		Parser			_parser;
// };
