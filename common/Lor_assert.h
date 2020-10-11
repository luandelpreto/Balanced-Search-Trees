/* C Header file:
 *              Lor_assert.h
 *
 */
#ifndef LOR_ASSERT_H
#define LOR_ASSERT_H 1

#include <stdio.h>
#include <stdlib.h>

#ifdef NDEBUG
#define Lor_assert(e, reason) ((void)0)
#else
extern void Lor_assert(int e, const char *reason);
#define LOR_ABORT(reason) Lor_assert_abort((reason), __FILE__, __LINE__+0UL)
#define Lor_assert(e, reason) ((void)((e)||(LOR_ABORT(reason),0)))

inline void Lor_assert_abort(const char *restrict reason, const char *restrict file, unsigned long line)
{
    if (file && line) {
        fprintf(stderr, "Assertion failed at %s: %lu: ", file, line);
    }

    if (reason) {
        fprintf(stderr, "%s\n", reason);
    }

    fprintf(stderr, "aborting...\n");
    fflush(stderr);
    abort();
}
#endif

#endif
