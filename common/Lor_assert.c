/* C file:
 *          Lor_assert.c
 */
#include "Lor_assert.h"

void Lor_assert_abort(const char *restrict, const char *restrict, unsigned long);

void (Lor_assert)(int e, const char *restrict reason)
{
    Lor_assert(e, reason);
}
