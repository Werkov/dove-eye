#ifndef DOVE_EYE_LOGGING_H_
#define	DOVE_EYE_LOGGING_H_

#include <stdio.h>

#ifndef NDEBUG
#define DEBUG(...) fprintf(stderr, __VA_ARGS__)
#else
#define DEBUG(...) /* empty */
#endif

#define ERROR(...) fprintf(stderr, __VA_ARGS__)

#endif	/* DOVE_EYE_LOGGING_H_ */

