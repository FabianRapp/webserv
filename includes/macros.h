#pragma once

#if defined(__linux__)
    #define LINUX
#elif defined(__APPLE__) && defined(__MACH__)
    #define MACOS
#else
    static_assert(0, "unsupported platform or linux/macos not detected");
#endif


#ifdef NDEBUG


# ifndef FT_ASSERT
#  define FT_ASSERT(cond) \
   do { \
   	if(!(cond)) {\
   		__builtin_unreachable(); \
   	}\
   } while(0)
# endif //FT_ASSERT

# else //NDEBUG

#  include <stdio.h>
#  include <assert.h>
#  include <string.h>

# ifndef FT_ASSERT
#  define FT_ASSERT(cond) \
   do { \
   	if (!(cond)) {\
		if (errno) { \
			printf("%s\n", strerror(errno));\
		}\
   		assert(cond); \
   	} \
   } while(0)
# endif //FT_ASSERT


#endif //NDEBUG
