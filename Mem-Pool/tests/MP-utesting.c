/* C file:
 *       utesting.c
 *
 * Testing for Lorena library memory pool
 */
#include "Lor_mem_pool_def.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <math.h>
#include <cmocka.h>
#include <assert.h>
#include <string.h>

typedef struct {
    int d;
    float f;
    char s[101];
} UserType;

static void TEST_MEM_POOL_ALLOC(void **state);
static void TEST_MEM_POOL_CONTAINS(void **state);
static void TEST_MEM_POOL_STRDUP(void **state);
static void TEST_MEM_POOL_COMBINE(void **state);

static int setup(void **state);
static int tear_down(void **state);

#define EPSILON 0.000000001

static void TEST_MEM_POOL_ALLOC(void **state)
{
    Lor_mem_pool *pool = Lor_mem_pool_create();
    assert_non_null(pool);

    assert_int_equal(Lor_mem_pool_init(pool, 4094*4096), LOR_SUCCESS);

    for (size_t i = 0; i < 50; i++) {
        UserType *ut = Lor_mem_pool_alloc(pool, sizeof *ut);
        assert_non_null(ut);
        /* Fill test struct */
        *ut = (UserType){
            .d = INT_MAX,
            .f = 10e6,
        };
        strcpy(ut->s, "TEST STRING");
        /* */
        assert_int_equal(ut->d, INT_MAX);
        assert(fabs(ut->f - 10e6) < EPSILON);
        assert_string_equal(ut->s, "TEST STRING");
    }
    assert_null(Lor_mem_pool_alloc(pool, SIZE_MAX));

    assert_int_equal(Lor_mem_pool_discard(pool, false), LOR_SUCCESS);
    assert_int_equal(Lor_mem_pool_destroy(&pool), LOR_SUCCESS);
}

static void TEST_MEM_POOL_CONTAINS(void **state)
{
    Lor_mem_pool *pool = Lor_mem_pool_create();
    assert_non_null(pool);
    assert_int_equal(Lor_mem_pool_init(pool, 256), LOR_SUCCESS);


    UserType *ut = Lor_mem_pool_alloc(pool, sizeof *ut);
    assert_non_null(ut);
    assert_true(Lor_mem_pool_contains(pool, (void *) ut));

    void *mem = malloc(64);
    assert_non_null(mem);
    assert_false(Lor_mem_pool_contains(pool, mem));
    free(mem);

    assert_int_equal(Lor_mem_pool_discard(pool, false), LOR_SUCCESS);
    assert_int_equal(Lor_mem_pool_destroy(&pool), LOR_SUCCESS);
}

static void TEST_MEM_POOL_STRDUP(void **state)
{
    Lor_mem_pool *pool = Lor_mem_pool_create();
    assert_non_null(pool);

    char *strs[100] = {
        [0] = "WHAB BABA UMBA MA",
        [1] = "ONE TEST IS NOT EVEN CLOSE OF SUFFICIENT",
        [2] = "NEITHER THIS MANY",
        [3] = "THIS IS NOT BY ANY STRECH OF IMAGINATION RIGOROUS",
        [4] = "BOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOM"
    };
    assert_int_equal(Lor_mem_pool_init(pool, 5*101), LOR_SUCCESS);

    for (size_t i = 0; i < 5; i++) {
        char *ptr = Lor_mem_pool_strdup(pool, strs[i]);
        assert_non_null(ptr);
        assert_string_equal(ptr, strs[i]);
    }

    for (size_t i = 0; i < 5; i++) {
        char *ptr = Lor_mem_pool_strndup(pool, strs[i], 10);
        assert_non_null(ptr);
        char *sptr = strndup(strs[i], 10);
        assert_string_equal(ptr, sptr);
        free(sptr);
    }

    assert_int_equal(Lor_mem_pool_discard(pool, false), LOR_SUCCESS);
    assert_int_equal(Lor_mem_pool_destroy(&pool), LOR_SUCCESS);
}

static void TEST_MEM_POOL_COMBINE(void **state)
{
    Lor_mem_pool *pool1 = Lor_mem_pool_create();
    assert_non_null(pool1);
    assert_int_equal(Lor_mem_pool_init(pool1, 256), LOR_SUCCESS);

    Lor_mem_pool *pool2 = Lor_mem_pool_create();
    assert_non_null(pool2);
    assert_int_equal(Lor_mem_pool_init(pool2, 1024), LOR_SUCCESS);

    size_t szsum = pool1->poolalloc + pool2->poolalloc;
    void *mem = (void *) pool2->mpblock->space;
    assert_int_equal(Lor_mem_pool_combine(pool1, pool2), LOR_SUCCESS);
    assert_int_equal(Lor_mem_pool_destroy(&pool2), LOR_SUCCESS);

    assert_int_equal(szsum, pool1->poolalloc);
    assert_true(Lor_mem_pool_contains(pool1, mem));

    assert_int_equal(Lor_mem_pool_discard(pool1, false), LOR_SUCCESS);
    assert_int_equal(Lor_mem_pool_destroy(&pool1), LOR_SUCCESS);
}


static int setup(void **state)
{
    return EXIT_SUCCESS;
}

static int tear_down(void **state)
{
    return EXIT_SUCCESS;
}


int main(int argc, char *argv[])
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(TEST_MEM_POOL_ALLOC),
        cmocka_unit_test(TEST_MEM_POOL_CONTAINS),
        cmocka_unit_test(TEST_MEM_POOL_STRDUP),
        cmocka_unit_test(TEST_MEM_POOL_COMBINE),
    };

    return cmocka_run_group_tests(tests, setup, tear_down);
}
