#ifndef ASSERT_H
#define ASSERT_H

#ifdef NDEBUG
#define assert(expr) (0)
#else
#define assert(expr) ((expr) ? 0 : __assert(#expr, __FILE__, __LINE__))
#endif

extern int  __assert(const char *msg, const char *file, int line);

#endif
