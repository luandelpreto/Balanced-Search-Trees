/* C Header file:
 *              Lor_BSTs.h
 *
 * This is the public interface through which the functionalities of Lorena
 * library should be exposed.
 */
#ifndef LOR_BSTS_H
#define LOR_BSTS_H 1

#include <Lor_mem_pool.h>
#include <Lor_AVLbst.h>

enum {
    LOR_SUCCESS=0,
    LOR_ZERO_SIZE_ALLOC_ERR,
    LOR_USIZE_OVERFLOW_ERR,
    LOR_ALLOC_FAIL_ERR,
    LOR_COMPARE_FN_NOT_PROVIDED_ERR,
    LOR_ALLOC_FN_NOT_PROVIDED_ERR,
    LOR_DESTROY_ROOT_NON_NULL,
    LOR_DISTINCT_KEY_ERR,
    LOR_MAX_HEIGHT_ERR,
    LOR_EMPTY_TREE_ERR,
    LOR_DELETE_NON_EXISTENT_KEY_ERR,
    LOR_FREE_NULLPTR_WARN,
    LOR_POSSIBLE_MEMLEAK_WARN,
    LOR_SRC_EMPTY_WARN,
};

#endif
