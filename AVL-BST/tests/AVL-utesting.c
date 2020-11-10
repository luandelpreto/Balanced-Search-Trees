/* C file:
 *        utesting.c
 * Simple unit testing for AVL_bst implementation
 */
#include "Lor_AVLbstdef.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <math.h>
#include <cmocka.h>
#include <assert.h>
#include <string.h>

typedef struct UserTest_ UserTest;

static void *alloc(size_t nbytes);
static int32_t compare_int(const void *, const void *);
static int32_t compare_str(const void *, const void *);

static void _AVL_insert_increasing_order(Lor_AVL_bst *tree);
static void _AVL_insert_unordered(Lor_AVL_bst *tree, size_t n, const int ints[n]);

static void TEST_INT_AVL_insert_increasing_order(void **state);
static void TEST_INT_AVL_insert_decreasing_order(void **state);
static void TEST_INT_AVL_insert_unordered0(void **state);
static void TEST_INT_AVL_insert_unordered1(void **state);
static void TEST_INT_AVL_find(void **state);
static void TEST_INT_AVL_interval_find(void **state);
static void TEST_INT_AVL_delete(void **state);
static void TEST_USER_DEF_TYPE_insert(void **state);
static void TEST_USER_DEF_TYPE_interval_find(void **state);
static void TEST_USER_DEF_TYPE_delete(void **state);

static int setup(void **state);
static int tear_down(void **state);

static void *alloc(size_t nbytes)
{
    void *block = malloc(nbytes);
    if (!block) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    return block;
}

static inline int32_t compare_int(const void *key1, const void *key2)
{
    int k1 = *((int *) key1);
    int k2 = *((int *) key2);
    if (k1 < k2) {
        return -1;
    }
    else if (k1 > k2) {
        return 1;
    }
    return 0;
}

static inline int32_t compare_str(const void *str1, const void *str2)
{
    return strcmp((char *) str1, (char *) str2);
}

static void TEST_INT_AVL_insert_increasing_order(void **state)
{
    Lor_AVL_bst *tree = Lor_AVL_create();
    assert(tree);
    assert_int_equal(Lor_AVL_init(tree, compare_int, alloc, NULL, free), LOR_SUCCESS);

    int *intptrs[4];
    for (size_t i = 0; i < 4; i++) {
        intptrs[i] = malloc(sizeof *intptrs[i]);
        *intptrs[i] = i+1;
        assert_int_equal(Lor_AVL_insert(tree, intptrs[i], intptrs[i]), LOR_SUCCESS);
    }

    assert_int_equal(*((int *) tree->root->key), 3);
    assert_int_equal(*((int *) tree->root->subtrees[0]->key), 2);
    assert_int_equal(*((int *) tree->root->subtrees[0]->subtrees[0]->key), 1);
    assert_int_equal(*((int *) tree->root->subtrees[0]->subtrees[1]->key), 2);
    assert_int_equal(*((int *) tree->root->subtrees[1]->key), 4);
    assert_int_equal(*((int *) tree->root->subtrees[1]->subtrees[0]->key), 3);
    assert_int_equal(*((int *) tree->root->subtrees[1]->subtrees[1]->key), 4);

    void *tmp;
    tmp = (void *) tree->root->subtrees[0]->subtrees[0]->subtrees[0];
    assert_int_equal(*((int *) tmp), 1);
    tmp = (void *) tree->root->subtrees[0]->subtrees[1]->subtrees[0];
    assert_int_equal(*((int *) tmp), 2);
    tmp = (void *) tree->root->subtrees[1]->subtrees[0]->subtrees[0];
    assert_int_equal(*((int *) tmp), 3);
    tmp = (void *) tree->root->subtrees[1]->subtrees[1]->subtrees[0];
    assert_int_equal(*((int *) tmp), 4);

    assert_int_equal(tree->root->height, 2);

    /* */
    int *ptr = alloc(sizeof *ptr);
    *ptr = 5;
    assert_int_equal(Lor_AVL_insert(tree, ptr, ptr), LOR_SUCCESS);
    assert_int_equal(tree->root->height, 3);
    *ptr = 3;
#ifdef LOR_AVL_ONLY_DISTINCT_KEYS
    assert_int_equal(Lor_AVL_insert(tree, ptr, ptr), LOR_DISTINCT_KEY_ERR);
#else
    int *newdata = alloc(sizeof *newdata);
    *newdata = 87;
    assert_int_equal(Lor_AVL_insert(tree, ptr, newdata), LOR_SUCCESS);
    tmp = (void *) tree->root->subtrees[1]->subtrees[0]->subtrees[0];
    assert_int_equal(*((int *) tmp), 87);
#endif

    assert_int_equal(Lor_AVL_clear(tree), LOR_SUCCESS);
    assert_int_equal(Lor_AVL_destroy(&tree), LOR_SUCCESS);
}

static void TEST_INT_AVL_insert_decreasing_order(void **state)
{
    Lor_AVL_bst *tree = Lor_AVL_create();
    assert(tree);
    assert_int_equal(Lor_AVL_init(tree, compare_int, alloc, NULL, free), LOR_SUCCESS);

    int *intptrs[4];
    for (size_t i = 0; i < 4; i++) {
        intptrs[i] = malloc(sizeof *intptrs[i]);
        *intptrs[i] = 4-i;
        assert_int_equal(Lor_AVL_insert(tree, intptrs[i], intptrs[i]), LOR_SUCCESS);
    }

    assert_int_equal(*((int *) tree->root->key), 3);
    assert_int_equal(*((int *) tree->root->subtrees[0]->key), 2);
    assert_int_equal(*((int *) tree->root->subtrees[0]->subtrees[0]->key), 1);
    assert_int_equal(*((int *) tree->root->subtrees[0]->subtrees[1]->key), 2);
    assert_int_equal(*((int *) tree->root->subtrees[1]->key), 4);
    assert_int_equal(*((int *) tree->root->subtrees[1]->subtrees[0]->key), 3);
    assert_int_equal(*((int *) tree->root->subtrees[1]->subtrees[1]->key), 4);

    void *tmp;
    tmp = (void *) tree->root->subtrees[0]->subtrees[0]->subtrees[0];
    assert_int_equal(*((int *) tmp), 1);
    tmp = (void *) tree->root->subtrees[0]->subtrees[1]->subtrees[0];
    assert_int_equal(*((int *) tmp), 2);
    tmp = (void *) tree->root->subtrees[1]->subtrees[0]->subtrees[0];
    assert_int_equal(*((int *) tmp), 3);
    tmp = (void *) tree->root->subtrees[1]->subtrees[1]->subtrees[0];
    assert_int_equal(*((int *) tmp), 4);

    assert_int_equal(tree->root->height, 2);

    assert_int_equal(Lor_AVL_clear(tree), LOR_SUCCESS);
    assert_int_equal(Lor_AVL_destroy(&tree), LOR_SUCCESS);
}

static void TEST_INT_AVL_insert_unordered0(void **state)
{
    Lor_AVL_bst *tree = Lor_AVL_create();
    assert(tree);
    assert_int_equal(Lor_AVL_init(tree, compare_int, alloc, NULL, free), LOR_SUCCESS);

    const int ints[] = { 12, 99990, -34, 36, 1 };
    int *intptrs[5];
    for (size_t i = 0; i < 5; i++) {
        intptrs[i] = malloc(sizeof *intptrs[i]);
        *intptrs[i] = ints[i];
        assert_int_equal(Lor_AVL_insert(tree, intptrs[i], intptrs[i]), LOR_SUCCESS);
    }

    assert_int_equal(*((int *) tree->root->key), 36);
    assert_int_equal(*((int *) tree->root->subtrees[0]->key), 12);
    assert_int_equal(*((int *) tree->root->subtrees[0]->subtrees[0]->key), 1);
    assert_int_equal(*((int *) tree->root->subtrees[0]->subtrees[0]->subtrees[0]->key), -34);
    assert_int_equal(*((int *) tree->root->subtrees[0]->subtrees[0]->subtrees[1]->key), 1);
    assert_int_equal(*((int *) tree->root->subtrees[0]->subtrees[1]->key), 12);
    assert_int_equal(*((int *) tree->root->subtrees[1]->key), 99990);
    assert_int_equal(*((int *) tree->root->subtrees[1]->subtrees[0]->key), 36);
    assert_int_equal(*((int *) tree->root->subtrees[1]->subtrees[1]->key), 99990);

    assert_int_equal(tree->root->height, 3);

    assert_int_equal(Lor_AVL_clear(tree), LOR_SUCCESS);
    assert_int_equal(Lor_AVL_destroy(&tree), LOR_SUCCESS);
}

static void TEST_INT_AVL_insert_unordered1(void **state)
{
    Lor_AVL_bst *tree = Lor_AVL_create();
    assert(tree);
    assert_int_equal(Lor_AVL_init(tree, compare_int, alloc, NULL, free), LOR_SUCCESS);

    const int ints[] = { 28, 85, 113, 101, -99 };
    int *intptrs[5];
    for (size_t i = 0; i < 5; i++) {
        intptrs[i] = malloc(sizeof *intptrs[i]);
        *intptrs[i] = ints[i];
        assert_int_equal(Lor_AVL_insert(tree, intptrs[i], intptrs[i]), LOR_SUCCESS);
    }

    assert_int_equal(*((int *) tree->root->key), 101);
    assert_int_equal(*((int *) tree->root->subtrees[0]->key), 85);
    assert_int_equal(*((int *) tree->root->subtrees[0]->subtrees[0]->key), 28);
    assert_int_equal(*((int *) tree->root->subtrees[0]->subtrees[0]->subtrees[0]->key), -99);
    assert_int_equal(*((int *) tree->root->subtrees[0]->subtrees[0]->subtrees[1]->key), 28);
    assert_int_equal(*((int *) tree->root->subtrees[0]->subtrees[1]->key), 85);
    assert_int_equal(*((int *) tree->root->subtrees[1]->key), 113);
    assert_int_equal(*((int *) tree->root->subtrees[1]->subtrees[0]->key), 101);
    assert_int_equal(*((int *) tree->root->subtrees[1]->subtrees[1]->key), 113);

    assert_int_equal(tree->root->height, 3);

    assert_int_equal(Lor_AVL_clear(tree), LOR_SUCCESS);
    assert_int_equal(Lor_AVL_destroy(&tree), LOR_SUCCESS);
}

#define NTESTS 20

static void _AVL_insert_increasing_order(Lor_AVL_bst *tree)
{
    int *ptrs[NTESTS];
    for (size_t i = 1; i <= NTESTS; i++) {
        ptrs[i] = malloc(sizeof *ptrs[i]);
        *ptrs[i] = i;
        assert(Lor_AVL_insert(tree, ptrs[i], ptrs[i]) == LOR_SUCCESS);
    }
}

static void _AVL_insert_unordered(Lor_AVL_bst *tree, size_t n, const int ints[n])
{
    int *ptrs[n];
    for (size_t i = 0; i < n; i++) {
        ptrs[i] = malloc(sizeof *ptrs[i]);
        *ptrs[i] = ints[i];
        assert(Lor_AVL_insert(tree, ptrs[i], ptrs[i]) == LOR_SUCCESS);
    }
}

static void TEST_INT_AVL_find(void **state)
{
    Lor_AVL_bst *tree = Lor_AVL_create();
    assert(tree);
    assert_int_equal(Lor_AVL_init(tree, compare_int, alloc, NULL, free), LOR_SUCCESS);

    Lor_AVL_bst_node *f = Lor_AVL_find(tree, &(int){1});
    assert(!f);

    _AVL_insert_increasing_order(tree);
    for (size_t i = 1; i <= NTESTS; i++) {
        int *ptr = malloc(sizeof *ptr);
        *ptr = i;
        Lor_AVL_bst_node *f = Lor_AVL_find(tree, ptr);
        assert_int_equal(*((int *) f->subtrees[0]), i);
        free(ptr);
    }

    f = Lor_AVL_find(tree, &(int){-3});
    assert(!f);

    assert_int_equal(Lor_AVL_clear(tree), LOR_SUCCESS);
    assert_int_equal(Lor_AVL_destroy(&tree), LOR_SUCCESS);
}

static void TEST_INT_AVL_interval_find(void **state)
{
    Lor_AVL_bst *tree = Lor_AVL_create();
    assert(tree);
    assert_int_equal(Lor_AVL_init(tree, compare_int, alloc, NULL, free), LOR_SUCCESS);

    const int ints[NTESTS] = { 12, -32, 1, 990, 5456, 137, 99, 2098, 567, -613,
                               888, 1200, 100123, 43, 756, 24, -45, 1012, 2, 10 };
    _AVL_insert_unordered(tree, NTESTS, ints);

    int *a = malloc(sizeof *a);
    int *b = malloc(sizeof *b);
    *a = -32; *b = 756;

    Lor_AVL_bst_node *lst = Lor_AVL_interval_find(tree, a, b);
    assert(lst);
    free(a);
    free(b);

    const int interval[] = { -32, 1, 2, 10, 12, 24, 43, 99, 137, 567 };
    size_t i = 0;
    for (Lor_AVL_bst_node *p = lst; p; p = p->subtrees[1]) {
        assert_int_equal(*((int *) p->subtrees[0]), interval[i++]);
    }

    Lor_AVL_clear_node_list(tree, lst);
    assert_int_equal(Lor_AVL_clear(tree), LOR_SUCCESS);
    assert_int_equal(Lor_AVL_destroy(&tree), LOR_SUCCESS);
}

static void TEST_INT_AVL_delete(void **state)
{
    Lor_AVL_bst *tree = Lor_AVL_create();
    assert(tree);
    assert_int_equal(Lor_AVL_init(tree, compare_int, alloc, NULL, free), LOR_SUCCESS);

    const int ints[12] = { 12, -32, 1, 990, 5456, 137, 99, 2098, 567, -613, 888, 1200 };
    _AVL_insert_unordered(tree, 12, ints);

    assert_int_equal(tree->root->height, 4);
    assert_int_equal(*((int *) tree->root->key), 137);

    void *deldata[4];

    assert_int_equal(Lor_AVL_delete(tree, &(int){137}, &deldata[0]), LOR_SUCCESS);
    assert_int_equal(*((int *) deldata[0]), 137);
    assert_int_equal(tree->root->subtrees[1]->subtrees[0]->height, 1);

    assert_int_equal(Lor_AVL_delete(tree, &(int){-32}, &deldata[1]), LOR_SUCCESS);
    assert_int_equal(*((int *) deldata[1]), -32);
    assert_int_equal(tree->root->subtrees[0]->height, 2);

    assert_int_equal(Lor_AVL_delete(tree, &(int){5456}, &deldata[2]), LOR_SUCCESS);
    assert_int_equal(*((int *) deldata[2]), 5456);
    assert_int_equal(tree->root->subtrees[1]->subtrees[1]->subtrees[1]->height, 0);

    assert_int_equal(Lor_AVL_delete(tree, &(int){2098}, &deldata[3]), LOR_SUCCESS);
    assert_int_equal(*((int *) deldata[3]), 2098);
    assert_int_equal(tree->root->subtrees[1]->subtrees[1]->height, 1);

    void *nda = NULL;
    assert_int_equal(Lor_AVL_delete(tree, &(int){137}, &nda), LOR_DELETE_NON_EXISTENT_KEY_ERR);
    assert_int_equal(Lor_AVL_delete(tree, &(int){-32}, &nda), LOR_DELETE_NON_EXISTENT_KEY_ERR);
    assert_int_equal(Lor_AVL_delete(tree, &(int){5456}, &nda), LOR_DELETE_NON_EXISTENT_KEY_ERR);
    assert_int_equal(Lor_AVL_delete(tree, &(int){2098}, &nda), LOR_DELETE_NON_EXISTENT_KEY_ERR);

    for (size_t i = 0; i < 4; i ++) {
        tree->freedata(deldata[i]);
    }

    assert_int_equal(Lor_AVL_clear(tree), LOR_SUCCESS);
    assert_int_equal(Lor_AVL_destroy(&tree), LOR_SUCCESS);
}

struct UserTest_ {
    size_t id;
    double salary;
    char name[64];
};

#undef NTESTS
#define NTESTS 6

char *names[] = {
    [0] = "Trigor Tres Quatro",
    [1] = "Araraca Manuira",
    [2] = "Mefisto Menacles",
    [3] = "Mororoca Nhiq",
    [4] = "Zimanu Xiqueiro",
    [5] = "Garita Fuehrirotimanu",
};

const size_t ids[] = { [0] = 10900, [1] = 11900, [2] = 12900, [3] = 13900, [4] = 14900, [5] = 15900};
const double salaries[] = {
    [0] = 1290.133, [1] = 8909.1334, [2] = 789.11111,
    [3] = 5678.99, [4] = 789.67, [5] = 12221.0001,
};

static void TEST_USER_DEF_TYPE_insert(void **state)
{
    Lor_AVL_bst *tree = Lor_AVL_create();
    assert(tree);
    Lor_AVL_init(tree, compare_str, alloc, NULL, free);

    UserTest *arr[NTESTS];
    for (size_t i = 0; i < NTESTS; i++) {
        arr[i] = alloc(sizeof *arr[i]);
    }
    for (size_t i = 0; i < NTESTS; i++) {
        arr[i]->id = ids[i];
        arr[i]->salary = salaries[i];
        strcpy(arr[i]->name, names[i]);
    }

    for (size_t i = 0; i < NTESTS; i++) {
        assert_int_equal(Lor_AVL_insert(tree, &arr[i]->name, arr[i]), LOR_SUCCESS);
    }

    assert_int_equal(tree->root->height, 3);

    assert_string_equal((char *) tree->root->key, "Mororoca Nhiq");
    assert_string_equal((char *) tree->root->subtrees[0]->key, "Mefisto Menacles");
    assert_string_equal((char *) tree->root->subtrees[0]->subtrees[0]->key, "Garita Fuehrirotimanu");
    assert_string_equal((char *) tree->root->subtrees[0]->subtrees[0]->subtrees[0]->key, "Araraca Manuira");
    assert_string_equal((char *) tree->root->subtrees[0]->subtrees[0]->subtrees[1]->key, "Garita Fuehrirotimanu");
    assert_string_equal((char *) tree->root->subtrees[0]->subtrees[1]->key, "Mefisto Menacles");
    assert_string_equal((char *) tree->root->subtrees[1]->key, "Trigor Tres Quatro");
    assert_string_equal((char *) tree->root->subtrees[1]->subtrees[0]->key, "Mororoca Nhiq");
    assert_string_equal((char *) tree->root->subtrees[1]->subtrees[1]->key, "Zimanu Xiqueiro");
    assert_string_equal((char *) tree->root->subtrees[1]->subtrees[1]->subtrees[0]->key, "Trigor Tres Quatro");
    assert_string_equal((char *) tree->root->subtrees[1]->subtrees[1]->subtrees[1]->key, "Zimanu Xiqueiro");

    assert_int_equal(Lor_AVL_clear(tree), LOR_SUCCESS);
    assert_int_equal(Lor_AVL_destroy(&tree), LOR_SUCCESS);
}

#define EPSILON 0.000000001

static void TEST_USER_DEF_TYPE_interval_find(void **state)
{
    Lor_AVL_bst *tree = Lor_AVL_create();
    assert(tree);
    Lor_AVL_init(tree, compare_str, alloc, NULL, free);

    UserTest *arr[NTESTS];
    for (size_t i = 0; i < NTESTS; i++) {
        arr[i] = alloc(sizeof *arr[i]);
    }
    for (size_t i = 0; i < NTESTS; i++) {
        arr[i]->id = ids[i];
        arr[i]->salary = salaries[i];
        strcpy(arr[i]->name, names[i]);
    }

    for (size_t i = 0; i < NTESTS; i++) {
        assert_int_equal(Lor_AVL_insert(tree, &arr[i]->name, arr[i]), LOR_SUCCESS);
    }

    Lor_AVL_bst_node *lst = Lor_AVL_interval_find(tree, names[1], names[5]);
    assert_non_null(lst);
    Lor_AVL_bst_node *p = lst;
    UserTest *tmp = (UserTest *) p->subtrees[0];
    assert_int_equal(tmp->id, ids[1]);
    assert(fabs(tmp->salary - salaries[1]) < EPSILON);
    assert_string_equal(tmp->name, names[1]);
    p = p->subtrees[1];
    assert_null(p);

    Lor_AVL_clear_node_list(tree, lst);

    lst = Lor_AVL_interval_find(tree, names[5], names[1]);
    assert_null(lst);

    assert_int_equal(Lor_AVL_clear(tree), LOR_SUCCESS);
    assert_int_equal(Lor_AVL_destroy(&tree), LOR_SUCCESS);
}

static void TEST_USER_DEF_TYPE_delete(void **state)
{
    Lor_AVL_bst *tree = Lor_AVL_create();
    assert(tree);
    Lor_AVL_init(tree, compare_str, alloc, NULL, free);

    UserTest *arr[NTESTS];
    for (size_t i = 0; i < NTESTS; i++) {
        arr[i] = alloc(sizeof *arr[i]);
    }
    for (size_t i = 0; i < NTESTS; i++) {
        arr[i]->id = ids[i];
        arr[i]->salary = salaries[i];
        strcpy(arr[i]->name, names[i]);
    }

    for (size_t i = 0; i < NTESTS; i++) {
        assert_int_equal(Lor_AVL_insert(tree, &arr[i]->name, arr[i]), LOR_SUCCESS);
    }

    void *ptr[2];
    assert_int_equal(Lor_AVL_delete(tree, names[3], &ptr[0]), LOR_SUCCESS);
    assert_int_equal(((UserTest *) ptr[0])->id, ids[3]);
    assert(fabs(((UserTest *) ptr[0])->salary - salaries[3]) < EPSILON);
    assert_string_equal(((UserTest *) ptr[0])->name, names[3]);
    assert_int_equal(tree->root->subtrees[1]->height, 1);

    ptr[1] = NULL;
    assert_int_equal(Lor_AVL_delete(tree, names[3], &ptr[1]), LOR_DELETE_NON_EXISTENT_KEY_ERR);
    assert_null(ptr[1]);

    assert_int_equal(Lor_AVL_delete(tree, names[4], &ptr[1]), LOR_SUCCESS);
    assert_int_equal(((UserTest *) ptr[1])->id, ids[4]);
    assert(fabs(((UserTest *) ptr[1])->salary - salaries[4]) < EPSILON);
    assert_string_equal(((UserTest *) ptr[1])->name, names[4]);
    assert_int_equal(tree->root->height, 2);
    assert_int_equal(tree->root->subtrees[0]->height, 1);
    assert_int_equal(tree->root->subtrees[1]->height, 1);

    tree->freedata(ptr[0]);
    tree->freedata(ptr[1]);

    assert_int_equal(Lor_AVL_clear(tree), LOR_SUCCESS);
    assert_int_equal(Lor_AVL_destroy(&tree), LOR_SUCCESS);
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
        cmocka_unit_test(TEST_INT_AVL_insert_increasing_order),
        cmocka_unit_test(TEST_INT_AVL_insert_decreasing_order),
        cmocka_unit_test(TEST_INT_AVL_insert_unordered0),
        cmocka_unit_test(TEST_INT_AVL_insert_unordered1),
        cmocka_unit_test(TEST_INT_AVL_find),
        cmocka_unit_test(TEST_INT_AVL_interval_find),
        cmocka_unit_test(TEST_INT_AVL_delete),
        cmocka_unit_test(TEST_USER_DEF_TYPE_insert),
        cmocka_unit_test(TEST_USER_DEF_TYPE_interval_find),
        cmocka_unit_test(TEST_USER_DEF_TYPE_delete),
    };
    return cmocka_run_group_tests(tests, setup, tear_down);
}
