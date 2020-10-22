/* C Header file:
 *              Lor_mem_pool_def.h
 *
 * This header is for exclusive use of the implementation and should
 * not be exposed.
 */
#ifndef LOR_MEM_POOL_DEF_H
#define LOR_MEM_POOL_DEF_H 1

#include "Lor_mem_pool.h"
#include <setjmp.h>
#include <limits.h>
#include <inttypes.h>

#ifndef FLEX_ARRAY
/* Check if the compiler is known to support flexible array members */
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) && (!defined(__SUNPRO_C) || (__SUNPRO_C > 0x580))
#  define FLEX_ARRAY  /* empty */
#elif defined(__GNUC__)
#  if (__GNUC__ >= 3)
#    define FLEX_ARRAY  /* empty */
#  else
#    define FLEX_ARRAY 0  /* older GNU extension */
#  endif
#endif
/* Otherwise, default to safer but a bit wasteful traditional style */
#ifndef FLEX_ARRAY
#  define FLEX_ARRAY 1
#endif
#endif

typedef struct mem_pool_block {
    struct mem_pool_block *nextblock;
    char *nextfree;
    char *end;
    uintmax_t space[FLEX_ARRAY];  /* more */
} mem_pool_block;

#define BLOCK_GROWTH_SIZE 1024*1024 - sizeof(mem_pool_block);

struct _Lor_mem_pool {
    mem_pool_block *mpblock;
    size_t poolalloc;  /* Total amount of memory allocated by the pool. */
    size_t blockalloc; /* Amount of available memory to grow the  pool  */
};                     /* by. This size does not include the  overhead  */
                       /* for the mpblock.                              */

/*********************************************
 * Overflow check
 *********************************************/
#define BITSIZEOF(x) (CHAR_BIT * sizeof(x))

#define MAX_UNSIGNED_VALUE_OF_TYPE(a)      \
    (UINTMAX_MAX >> (BITSIZEOF(uintmax_t) - BITSIZEOF(a)))

#define UNSIGNED_ADD_OVERFLOWS(a, b)       \
    ((b) > MAX_UNSIGNED_VALUE_OF_TYPE(a) - (a))

/* To avoid evaluate 'a' twice, it's not checked against zero division */
#define UNSIGNED_MULT_OVERFLOWS(a, b)      \
    ((b) > MAX_UNSIGNED_VALUE_OF_TYPE(a) / (a))

static inline size_t __st_add(jmp_buf env, size_t a, size_t b)
{
    if (UNSIGNED_ADD_OVERFLOWS(a, b)) {
        longjmp(env, LOR_MP_USIZE_OVERFLOW_ERR);
    }

    return a + b;
}

static inline size_t __st_mult(jmp_buf env, size_t a, size_t b)
{
    if (UNSIGNED_MULT_OVERFLOWS(a, b)) {
        longjmp(env, LOR_MP_USIZE_OVERFLOW_ERR);
    }

    return a * b;
}
/**********************************************/
#endif
