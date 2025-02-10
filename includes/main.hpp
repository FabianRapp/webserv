#pragma once

#include <signal.h>

extern volatile sig_atomic_t	exit_;
#ifndef LOGGING
#define LOGGING 1
#endif //LOGGING

#ifndef LOGGING_MAKSIM
#define LOGGING_MAKSIM (1 && LOGGING)
#endif //LOGGING_MAKSIM

#ifndef LOGGING_ADRIAN
#define LOGGING_ADRIAN (1 && LOGGING)
#endif //LOGGING_ADRIAN

#ifndef LOGGING_FABIAN
#define LOGGING_FABIAN (1 && LOGGING)
#endif //LOGGING_FABIAN

#ifndef LOG
#define LOG(content) \
	do {\
		if (!exit_ && LOGGING) { \
			std::cerr << content; \
		} \
	} while(0)
#endif //LOG

#ifndef LOG_MAKSIM
#define LOG_MAKSIM(content) \
	do {\
		if (!exit_ && LOGGING_MAKSIM) { \
			std::cerr << content; \
		} \
	} while(0)
#endif //LOG_MAKSIM
// LOG_MAKSIM("HELLO\n");

#ifndef LOG_ADRIAN
#define LOG_ADRIAN(content) \
	do {\
		if (!exit_ && LOGGING_ADRIAN) { \
			std::cerr << content; \
		} \
	} while(0)
#endif //LOG_ADRIAN

#ifndef LOG_FABIAN
#define LOG_FABIAN(content) \
	do {\
		if (!exit_ && LOGGING_FABIAN) { \
			std::cerr << content; \
		} \
	} while(0)
#endif //LOG_FABIAN
