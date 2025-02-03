#pragma once

#include <exception>
#include <string>

/* only throw main thread. For example when config file is wrong. */
class ForceFullExit: public std::exception {
public:
};

/* internal exception from within a server */
class ForceClientClosure: public std::exception {
public:
};

/* out of memory, handle later...*/
class OOM: public std::exception {
public:
};
