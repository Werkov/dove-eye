#ifndef DOVE_EYE_LOGGING_H_
#define DOVE_EYE_LOGGING_H_

#include <stdio.h>

#ifdef WIN32
#define __func__ __FUNCTION__
#endif

//TODO Replace "\n" with platform independent constant
#ifndef NDEBUG
#define DEBUG(...) do {              \
  fprintf(stderr , __VA_ARGS__);     \
  fprintf(stderr, "\n");             \
} while(false)
#else
#define DEBUG(...) /* empty */
#endif

#define ERROR(...) do {              \
  fprintf(stderr , __VA_ARGS__);     \
  fprintf(stderr, "\n");             \
} while(false)

#endif // DOVE_EYE_LOGGING_H_

