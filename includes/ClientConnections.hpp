#pragma once
#include "ClientConnection.hpp"
#include <poll.h>
#include "macros.h"

/* NewClientListenter is only allowed to call add_client() and get_count() */
class ClientConnections {
public:
	/* do not use these within the class */
	ClientConnections(void);
	~ClientConnections(void);
	ClientConnection *		add_client(t_fd fd);
	nfds_t					get_count(void);
	void					set_and_poll(short int events);
	void					close_client_connection(ClientConnection *connection);
	class PollIterator {
	public:
		PollIterator(ClientConnection **connections, int connection_count, struct pollfd *poll_fds, short int event);
		PollIterator(const PollIterator &right);
		~PollIterator(void) = default;
		void				set_begin(void);
		void				set_end(void);
		PollIterator&		operator=(const PollIterator &old);
		PollIterator&		operator++(void);
		PollIterator		operator++(int);
		PollIterator&		operator--(void);
		PollIterator		operator--(int);
		bool				operator==(const PollIterator & right);
		bool				operator!=(const PollIterator & right);
		bool				operator<=(const PollIterator & right);
		bool				operator>=(const PollIterator & right);
		bool				operator<(const PollIterator & right);
		bool				operator>(const PollIterator & right);
		ClientConnection&	operator*(void);
	private:
		ClientConnection	**_connections;
		struct pollfd		*_poll_fds;
		int					_begin_idx;
		int					_end_idx;
		short int			_event;
		int					_idx;
	};
	PollIterator			begin(short int event);
	PollIterator			end(short int event);
private:
	std::mutex				_mutex;
	ClientConnection *		_client_connections[MAX_CLIENTS];
	nfds_t					_active_client_count;
	struct pollfd			_client_fds[MAX_CLIENTS];
};

