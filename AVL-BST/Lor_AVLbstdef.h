/* C Header file:
 *               Lor_AVLbstdef.h
 * Type definitions for AVL binary search tree
 * NOTE: This header file is for exclusive use of the implementation
 * and should not be exposed.
 */
#ifndef LOR_AVL_BST_DEF_H
#define LOR_AVL_BST_DEF_H 1

#include "Lor_AVLbst.h"
#include <assert.h>

#ifndef LOR_AVL_BST_MAX_HEIGHT
#define LOR_AVL_BST_MAX_HEIGHT 32
#endif

struct _Lor_AVL_bst_node {
    int32_t height;
    void *key;
    struct _Lor_AVL_bst_node *subtrees[2];  /* [0] for left, [1] for right subtree */
};                                          /* In this tree model, the data is stored */
                                            /* on a leaf's left node.                 */

struct _Lor_AVL_bst {
    size_t nitems;          /* number of items */
    Lor_AVL_bst_node *root;
    Lor_AVL_compare compare;
    Lor_AVL_alloc alloc;
    Lor_AVL_free_node freenode;
    Lor_AVL_free_data freedata;
};

typedef struct {
    size_t height;                                   /* number of nodes in avl_stack */
    const Lor_AVL_bst *tree;                         /* the tree being traversed */
    Lor_AVL_bst_node *current;                       /* current node in the tree */
    Lor_AVL_bst_node *stack[LOR_AVL_BST_MAX_HEIGHT]; /* nodes above current node */
} Lor_AVL_traverser;

/*========== Inline functions ===========*/

static inline void Lor_AVL_traverser_init(Lor_AVL_traverser *trav, Lor_AVL_bst *restrict tree)
{
    *trav = (Lor_AVL_traverser){ .tree = tree,
                                 .current = tree->root,
                                 .height = 0,
                          };
}

static inline void tree_left_rotate(Lor_AVL_bst_node *node)
{
    void *tmpkey = node->key;
    Lor_AVL_bst_node *tmpnode = node->subtrees[0];

    node->key = node->subtrees[1]->key;
    node->subtrees[0] = node->subtrees[1];
    node->subtrees[1] = node->subtrees[1]->subtrees[1];
    node->subtrees[0]->subtrees[1] = node->subtrees[0]->subtrees[0];
    node->subtrees[0]->subtrees[0] = tmpnode;
    node->subtrees[0]->key = tmpkey;
}

static inline void tree_right_rotate(Lor_AVL_bst_node *node)
{
    void *tmpkey = node->key;
    Lor_AVL_bst_node *tmpnode = node->subtrees[1];

    node->key = node->subtrees[0]->key;
    node->subtrees[1] = node->subtrees[0];
    node->subtrees[0] = node->subtrees[0]->subtrees[0];
    node->subtrees[1]->subtrees[0] = node->subtrees[1]->subtrees[1];
    node->subtrees[1]->subtrees[1] = tmpnode;
    node->subtrees[1]->key = tmpkey;
}

#endif
