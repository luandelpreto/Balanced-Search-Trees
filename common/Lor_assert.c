/* C file:
 *          Lor_assert.c
 */
#include "Lor_assert.h"

extern void (Lor_assert)(int, const char *restrict);

_Noreturn void Lor_assert_abort(const char *restrict reason, const char *restrict file, unsigned long line)
{
    fprintf(stderr, "Assertion failed");
    if (file && line) {
        fprintf(stderr, " at %s: %lu: ", file, line);
    }

    if (reason) {
        fprintf(stderr, "%s", reason);
    }

    fprintf(stderr, "\naborting...\n");
    fflush(stderr);
    abort();
}
