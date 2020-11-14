/* C file:
 *          Lor_assert.c
 */
#include "Lor_assert.h"

extern void (Lor_assert)(int, const char *restrict, const char *restrict);

_Noreturn void Lor_assert_abort(const char *restrict reason, const char *restrict file,
                                const char *restrict func, unsigned long line)
{
    fprintf(stderr, "Assertion failed");
    if (file && func && line) {
        fprintf(stderr, " at %s: %s: %lu: ", file, func, line);
    }

    if (reason) {
        fprintf(stderr, "%s", reason);
    }

    fprintf(stderr, "\naborting...\n");
    fflush(stderr);
    abort();
}
