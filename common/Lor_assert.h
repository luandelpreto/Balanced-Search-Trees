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
_Noreturn void Lor_assert_abort(const char *restrict reason, const char *restrict file,
                                const char *restrict func, unsigned long line);
#define LOR_ABORT(func, reason) Lor_assert_abort((reason), __FILE__, func, __LINE__+0UL)
#define Lor_assert(e, func, reason) ((void)((e)||(LOR_ABORT(func, reason),0)))

inline void (Lor_assert)(int e, const char *restrict func, const char *restrict reason)
{
    Lor_assert(e, func, reason);
}

#endif

#endif
