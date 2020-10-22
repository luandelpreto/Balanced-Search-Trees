/* C file:
 *       Lor_mem_pool.c
 *
 * Lorena library memory pool implementation.
 */
#include "Lor_mem_pool_def.h"
#include <Lor_assert.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#define USIZE_OVERFLOW_MSG(a, b)                                                                             \
    fprintf(stderr, "[%s:%lu]: [OVERFLOW]: size_t overflow encountered at: %"PRIuMAX" + %"PRIuMAX"\n", \
                __FILE__, __LINE__+0UL, (uintmax_t) (a), (uintmax_t) (b));


static size_t __st_add(jmp_buf, size_t, size_t);
static size_t __st_mult(jmp_buf, size_t, size_t);

/**********************************************************
 * Allocate a  new mem_pool_block  and insert it after  the
 * block specified  in  'insertafter'.  If 'insertafter' is
 * NULL, then insert the block  at the head of  the  linked
 * list.
 **********************************************************/
static mem_pool_block *__mem_pool_alloc_block(Lor_mem_pool *pool, size_t blockalloc,
                                              mem_pool_block *insertafter, bool *overflow);

static mem_pool_block *__mem_pool_alloc_block(Lor_mem_pool *pool, size_t blockalloc,
                                              mem_pool_block *insertafter, bool *overflow)
{
    size_t oldpoolalloc = pool->poolalloc;
    pool->poolalloc += sizeof(*pool->mpblock) + blockalloc;

    *overflow = false;
    /* Handling size_t overflow */
    jmp_buf env;
    int val = setjmp(env);
    if (val == LOR_MP_USIZE_OVERFLOW_ERR) {
        USIZE_OVERFLOW_MSG(sizeof(mem_pool_block), blockalloc);
        *overflow = true;
        pool->poolalloc = oldpoolalloc;
        return NULL;
    }
    size_t szsum =  __st_add(env, sizeof(mem_pool_block), blockalloc);

    mem_pool_block *p = malloc(szsum);
    if (!p) {
        perror("malloc failed");
        errno = 0;
        pool->poolalloc = oldpoolalloc;
        return NULL;
    }

    p->nextfree = (char *) p->space;
    p->end = p->nextfree + blockalloc;

    if (insertafter) {
        p->nextblock = insertafter->nextblock;
        insertafter->nextblock = p;
    }
    else {
        p->nextblock = pool->mpblock;
        pool->mpblock = p;
    }

    return p;
}

Lor_mem_pool *Lor_mem_pool_create(void)
{
    Lor_mem_pool *pool = malloc(sizeof *pool);
    if (!pool) {
        perror("malloc failed");
        errno = 0;
        return NULL;
    }
    return pool;
}

int Lor_mem_pool_init(Lor_mem_pool *pool, size_t size)
{
    Lor_assert(pool, "argument 'pool' must be non-NULL");

    memset(pool, 0, sizeof *pool);
    pool->blockalloc = 0;
    pool->poolalloc = 0;

    if (size > 0) {
        bool overflow;
        mem_pool_block *tmp = __mem_pool_alloc_block(pool, size, NULL, &overflow);
        if (overflow) {
            return LOR_MP_USIZE_OVERFLOW_ERR;
        }
        else if (!tmp) {
            return LOR_MP_ALLOC_FAIL_ERR;
        }
    }
    else {
        return LOR_MP_ZERO_SIZE_ALLOC_ERR;
    }

    pool->blockalloc = BLOCK_GROWTH_SIZE;
    return LOR_MP_SUCCESS;
}

int Lor_mem_pool_destroy(Lor_mem_pool **pool)
{
    Lor_assert(*pool, "address of pointer 'pool' must be non-NULL");

    if ((*pool)->mpblock) {
        return LOR_MP_POSSIBLE_MEMLEAK_WARN;
    }
    free(*pool);

    return LOR_MP_SUCCESS;
}

int Lor_mem_pool_discard(Lor_mem_pool *pool, bool invalidate_mem)
{
    Lor_assert(pool, "argument 'pool' must be non-NULL");

    if (!pool->mpblock) {
        return LOR_MP_FREE_NULLPTR_WARN;
    }

    mem_pool_block *block = pool->mpblock;
    while (block) {
        mem_pool_block *blocktofree = block;
        block = block->nextblock;
        if (invalidate_mem) {
            memset(blocktofree->space, 0xDD, ((char *) blocktofree->end) - ((char *) blocktofree->space));
        }
        free(blocktofree);
    }
    pool->mpblock = NULL;

    return LOR_MP_SUCCESS;
}

void *Lor_mem_pool_alloc(Lor_mem_pool *pool, size_t len)
{
    Lor_assert(pool, "argument 'pool' must be non-NULL");

    /* Check for size_t overflow and round up to a uintmax_t alignment */
    size_t alignment = sizeof(uintmax_t) - (len & (sizeof(uintmax_t) - 1));
    if (UNSIGNED_ADD_OVERFLOWS(len, alignment)) {
        USIZE_OVERFLOW_MSG(len, alignment);
        return NULL;
    }
    else if (len & (sizeof(uintmax_t) - 1)) {
        len += alignment;
    }

    mem_pool_block *p = NULL;
    if (pool->mpblock && pool->mpblock->end - pool->mpblock->nextfree >= len) {
        p = pool->mpblock;
    }

    bool overflow;
    if (!p) {
        if (len >= (pool->blockalloc / 2)) {
            mem_pool_block *tmp =  __mem_pool_alloc_block(pool, len, pool->mpblock, &overflow);
            if (overflow) return NULL;
            return tmp;
        }
        p = __mem_pool_alloc_block(pool, pool->blockalloc, NULL, &overflow);
        if (overflow) return NULL;
    }
    if (!p) {
        return NULL;
    }

    void *retblock = p->nextfree;
    p->nextfree += len;
    return retblock;
}

void *Lor_mem_pool_calloc(Lor_mem_pool *pool, size_t count, size_t size)
{
    Lor_assert(pool, "argument 'pool' must be non-NULL");

    if (!count || !size) {
        return NULL;
    }

    jmp_buf env;
    int val = setjmp(env);
    if (val == LOR_MP_USIZE_OVERFLOW_ERR) {
        return NULL;
    }

    size_t len = __st_mult(env, count, size);
    void *retblock = Lor_mem_pool_alloc(pool, len);
    memset(retblock, 0, len);
    return retblock;
}

char *Lor_mem_pool_strdup(Lor_mem_pool *pool, const char *str)
{
    Lor_assert(pool, "argument 'pool' must be non-NULL");

    size_t len = strlen(str) + 1;
    char *retstr = Lor_mem_pool_alloc(pool, len);
    if (!retstr) {
        return NULL;
    }
    return memcpy(retstr, str, len);
}

char *Lor_mem_pool_strndup(Lor_mem_pool *pool, const char *str, size_t len)
{
    Lor_assert(pool, "argument 'pool' must be non-NULL");

    char *p = memchr(str, '\0', len);
    size_t actuallen = (p ? p - str : len);
    char *retstr = Lor_mem_pool_alloc(pool, actuallen+1);
    if (!retstr) {
        return NULL;
    }
    retstr[actuallen] = '\0';
    return memcpy(retstr, str, actuallen);
}

bool Lor_mem_pool_contains(Lor_mem_pool *pool, void *mem)
{
    Lor_assert(pool, "argument 'pool' must be non-NULL");

    /* Check if memory is allocated in any block. */
    for (mem_pool_block *p = pool->mpblock; p; p = p->nextblock) {
        if ((((void *) p->space) <= mem) && (mem < ((void *) p->end))) {
            return true;
        }
    }
    return false;
}

int Lor_mem_pool_combine(Lor_mem_pool *dst, Lor_mem_pool *src)
{
    Lor_assert(dst && src, "arguments 'dst' and 'src' must be non-NULL");

    /* Handling size_t overflow */
    jmp_buf env;
    int val = setjmp(env);
    size_t allocsum = 0;
    if (val == LOR_MP_USIZE_OVERFLOW_ERR) {
        return LOR_MP_USIZE_OVERFLOW_ERR;
    }
    allocsum = __st_add(env, dst->poolalloc, src->poolalloc);

    /* Append the blocks from 'src' to 'dst' */
    if (dst->mpblock && src->mpblock) {
        /* 'src' and 'dst' have blocks, append blocks from
         * 'src' to 'dst' */
        mem_pool_block *p = dst->mpblock;
        while (p->nextblock) {
            p = p->nextblock;
        }
        p->nextblock = src->mpblock;
    }
    else if (src->mpblock) {
        /* 'dst' is empty */
        dst->mpblock = src->mpblock;
    }
    else {
        return LOR_MP_SRC_EMPTY_WARN;
    }

    dst->poolalloc = allocsum;
    src->poolalloc = 0;
    src->mpblock = NULL;
    return LOR_MP_SUCCESS;
}

/* End of File */
