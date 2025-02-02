#pragma once

#include <queue>
#include <chrono>
#include <sys/wait.h>
#include <signal.h>
#include <colors.h>
#include <iostream>

class CgiTimeouts {
public:
	CgiTimeouts(std::chrono::milliseconds timeout_time);
	~CgiTimeouts();
	void	add(pid_t pid);
	void	handle_timeouts(void);
private:
	const std::chrono::milliseconds	_timeout_time;
	struct process {
		std::chrono::time_point<
			std::chrono::steady_clock, std::chrono::nanoseconds>
									start_time;
		pid_t						pid;
	};
	std::queue<struct process>	_queue;
};


