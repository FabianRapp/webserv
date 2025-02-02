#include <CgiTimeouts.hpp>

CgiTimeouts::CgiTimeouts(std::chrono::milliseconds timeout_time):
	_timeout_time(timeout_time)
{}

CgiTimeouts::~CgiTimeouts(void) {
	while (!_queue.empty()) {
		struct process	cur = _queue.front();
		_queue.pop();
		int stat;
		if (waitpid(cur.pid, &stat, WNOHANG) > 0) {
			continue ;
		}
		std::cerr << FT_ANSI_YELLOW "Warning: killing cgi process with pid "
			<< cur.pid << FT_ANSI_RESET "\n";
		kill(cur.pid, SIGKILL);
	}
}

void	CgiTimeouts::add(pid_t pid) {
	struct process new_process = {
		.start_time = std::chrono::steady_clock::now(),
		.pid = pid,
	};
	_queue.push(new_process);
}

void	CgiTimeouts::handle_timeouts(void) {
	while (!_queue.empty()) {
		struct process	cur = _queue.front();
		int stat;
		if (waitpid(cur.pid, &stat, WNOHANG) > 0) {
			_queue.pop();
			continue ;
		}
		auto now = std::chrono::steady_clock::now();
		auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - cur.start_time);
		if (std::chrono::milliseconds(elapsed_ms) >= _timeout_time) {
			std::cerr << FT_ANSI_YELLOW "Warning: timing out cgi process with pid "
				<< cur.pid << FT_ANSI_RESET "\n";
			_queue.pop();
			kill(cur.pid, SIGKILL);
		}
		return ;
	}
}
