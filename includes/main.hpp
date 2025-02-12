#pragma once

#include <signal.h>

extern volatile sig_atomic_t	exit_;

#ifndef PRINT_REQUEST
#define PRINT_REQUEST 0
#endif //PRINT_REQUEST

#ifndef LOGGING
#define LOGGING (1)
#endif //LOGGING

#ifndef LOGGING_PARSING
#define LOGGING_PARSING (0 && LOGGING)
#endif //LOGGING_PARSING


#ifndef LOGGING_MAKSIM
#define LOGGING_MAKSIM (0 && LOGGING)
#endif //LOGGING_MAKSIM

#ifndef LOGGING_ADRIAN
#define LOGGING_ADRIAN (0 && LOGGING)
#endif //LOGGING_ADRIAN

#ifndef LOGGING_FABIAN
#define LOGGING_FABIAN (0 && LOGGING)
#endif //LOGGING_FABIAN

//for more logging, mostly disabled
#ifndef LOGGING_FABIAN3
#define LOGGING_FABIAN3 (0 && LOGGING_FABIAN)
#endif //LOGGING_FABIAN

#ifndef LOG
#define LOG(content) \
	do {\
		if (LOGGING) { \
			std::cerr << content; \
		} \
	} while(0)
#endif //LOG

#ifndef LOG_PARSER
#define LOG_PARSER(content) \
	do {\
		if (LOGGING_PARSING) { \
			std::cerr << content; \
		} \
	} while(0)
#endif //LOG_PARSER

#ifndef LOG_MAKSIM
#define LOG_MAKSIM(content) \
	do {\
		if (LOGGING_MAKSIM) { \
			std::cerr << content; \
		} \
	} while(0)
#endif //LOG_MAKSIM
// LOG_MAKSIM("HELLO\n");

#ifndef LOG_ADRIAN
#define LOG_ADRIAN(content) \
	do {\
		if (LOGGING_ADRIAN) { \
			std::cerr << content; \
		} \
	} while(0)
#endif //LOG_ADRIAN

#ifndef LOG_FABIAN
#define LOG_FABIAN(content) \
	do {\
		if (LOGGING_FABIAN) { \
			std::cerr << content; \
		} \
	} while(0)
#endif //LOG_FABIAN


#ifndef LOG_FABIAN3
#define LOG_FABIAN3(content) \
	do {\
		if (LOGGING_FABIAN3) { \
			std::cerr << content; \
		} \
	} while(0)
#endif //LOG_FABIAN
