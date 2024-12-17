#pragma once

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

# ifndef FT_ASSERT
#  define FT_ASSERT(cond) \
   do { \
   	if (!(cond)) {\
   		assert(cond); \
   	} \
   } while(0)
# endif //FT_ASSERT


#endif //NDEBUG
