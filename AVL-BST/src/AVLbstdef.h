/* C Header file:
 *               AVLbstdef.h
 * Type definitions for AVL binary search tree
 */
#ifndef AVL_BST_DEF_H
#define AVL_BST_DEF_H 1

#include "AVLbst.h"
#include "assert.h"
#include "except.h"

#ifndef AVL_BST_MAX_HEIGHT
#define AVL_BST_MAX_HEIGHT 32
#endif

struct _AVL_bst_node {
    int32_t height;
    void *key;
    struct _AVL_bst_node *subtrees[2];  /* [0] for left, [1] for right subtree */
    //void *data;                        /* In this tree model, the data is stored */
};                                      /* on a leaf's left node.                 */

struct _AVL_bst {
    AVL_bst_node *root;
    AVL_compare compare;
    size_t nitems;          /* number of items */
    //uint32_t generation;    /* generation number */
    AVL_alloc alloc;
    AVL_free_node freenode;
    AVL_free_data freedata;
};

typedef struct {
    const AVL_bst *tree;                     /* the tree being traversed */
    AVL_bst_node *current;                   /* current node in the tree */
    AVL_bst_node *stack[AVL_BST_MAX_HEIGHT]; /* nodes above current node */
    size_t height;                           /* number of nodes in avl_stack */
    //uint32_t generation;
} AVL_traverser;

/*========== Inline functions ===========*/

static inline void AVL_traverser_init(AVL_traverser *trav, AVL_bst *restrict tree)
{
    *trav = (AVL_traverser){ .tree = tree,
                              .current = tree->root,
                              .height = 0,
                          };
}

static inline void tree_left_rotate(AVL_bst_node *node)
{
    assert(node && node->subtrees[0] && node->subtrees[1]); // REMOVE this assert

    void *tmpkey = node->key;
    AVL_bst_node *tmpnode = node->subtrees[0];

    node->key = node->subtrees[1]->key;
    node->subtrees[0] = node->subtrees[1];
    node->subtrees[1] = node->subtrees[1]->subtrees[1];
    node->subtrees[0]->subtrees[1] = node->subtrees[0]->subtrees[0];
    node->subtrees[0]->subtrees[0] = tmpnode;
    node->subtrees[0]->key = tmpkey;
}

static inline void tree_right_rotate(AVL_bst_node *node)
{
    assert(node && node->subtrees[0] && node->subtrees[1]); // REMOVE this assert

    void *tmpkey = node->key;
    AVL_bst_node *tmpnode = node->subtrees[1];

    node->key = node->subtrees[0]->key;
    node->subtrees[1] = node->subtrees[0];
    node->subtrees[0] = node->subtrees[0]->subtrees[0];
    node->subtrees[1]->subtrees[0] = node->subtrees[1]->subtrees[1];
    node->subtrees[1]->subtrees[1] = tmpnode;
    node->subtrees[1]->key = tmpkey;
}

#endif
