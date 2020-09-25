/* C file:
 *         AVLbst.c
 * Implementation for AVL binary search tree
 */
#include "AVLbstdef.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>

static void AVL_traverser_init(AVL_traverser *trav, AVL_bst *restrict);
static void tree_left_rotate(AVL_bst_node *node);
static void tree_right_rotate(AVL_bst_node *node);

AVL_bst *AVL_create(void)
{
    AVL_bst *tree = malloc(sizeof *tree);
    if (!tree) {
        perror("malloc failed");
        errno = 0;
        return NULL;
    }
    return tree;
}

int AVL_init(AVL_bst *restrict tree, AVL_compare compare, AVL_alloc alloc,
              AVL_free_node freenode, AVL_free_data freedata)
{
    assert(tree);

    if (!compare) {
        return AVL_COMPARE_FN_NOT_PROVIDED_ERR;
    }
    if (!alloc) {
        return AVL_ALLOC_FN_NOT_PROVIDED_ERR;
    }

    tree->alloc = alloc;
    tree->root = tree->alloc(sizeof *tree->root);
    tree->root->subtrees[0] = NULL;  /* empty tree */

    tree->compare = compare;
    tree->nitems = 0;
    tree->freenode = (freenode) ? freenode : free;
    tree->freedata = (freedata) ? freedata : free;

    return AVL_SUCCESS;
}

int AVL_clear(AVL_bst *restrict tree)
{
    assert(tree);

    if (!tree->root) {
        return AVL_FREE_NULLPTR_ERR;
    }
    if (!tree->root->subtrees[0]) { /* empty tree */
        return AVL_EMPTY_TREE_ERR;
    }

    AVL_bst_node *p = tree->root;
    for (AVL_bst_node *q = NULL; p->subtrees[1]; p = q) {
        if (!p->subtrees[0]->subtrees[1]) { /* left of p is a leaf */
            q = p->subtrees[1];
            tree->freedata(p->subtrees[0]->subtrees[0]);
            tree->freenode(p->subtrees[0]);
            tree->freenode(p);
        }
        else {
            q = p->subtrees[0];
            p->subtrees[0] = q->subtrees[1];
            q->subtrees[1] = p;
        }
    }
    /* free last leaf node */
    tree->freedata(p->subtrees[0]);
    tree->freenode(p);

    *tree = (AVL_bst){ .root = NULL, .nitems = 0 };
    return AVL_SUCCESS;
}

int AVL_destroy(AVL_bst **restrict tree)
{
    if (!(*tree)) {
        return AVL_FREE_NULLPTR_ERR;
    }
    else {
        if (!(*tree)->root) {
            free(*tree);
        }
        else {
            return AVL_DESTROY_ROOT_NON_NULL;
        }
    }
    return AVL_SUCCESS;
}

AVL_bst_node *AVL_find(AVL_bst *restrict tree, const void *key)
{
    assert(tree && key);

    if (!tree->root->subtrees[0]) {
        return NULL;
    }

    AVL_bst_node *tmpnode = tree->root;
    while (tmpnode->subtrees[1]) {
        if (tree->compare(tmpnode->key, key) > 0) {
            tmpnode = tmpnode->subtrees[0];
        }
        else {
            tmpnode = tmpnode->subtrees[1];
        }
    }
    return (!tree->compare(tmpnode->key, key)) ? tmpnode : NULL;
}

AVL_bst_node *AVL_interval_find(AVL_bst *restrict tree, const void *a, const void *b)
{
    assert(tree && a && b);

    AVL_bst_node *list = NULL;
    AVL_bst_node *stack[2 * tree->nitems];
    size_t top = 0;

    stack[top++] = tree->root;
    while (top) {
        AVL_bst_node *tmpnode = stack[--top];
        if (!tmpnode->subtrees[1]) { /* if leaf, test for interval */
            if (tree->compare(tmpnode->key, a) >= 0 && tree->compare(tmpnode->key, b) <= 0) {
                AVL_bst_node *newnode = tree->alloc(sizeof *newnode);
                newnode->key = tmpnode->key;
                newnode->subtrees[0] = tmpnode->subtrees[0];
                newnode->subtrees[1] = list;
                list = newnode;
            }
        }
        /* Else, search for interval */
        else if (tree->compare(tmpnode->key, a) <= 0) {
            stack[top++] = tmpnode->subtrees[1];
        }
        else if (tree->compare(tmpnode->key, b) >= 0) {
            stack[top++] = tmpnode->subtrees[0];
        }
        else {
            stack[top++] = tmpnode->subtrees[0];
            stack[top++] = tmpnode->subtrees[1];
        }
    }
    return list;
}

inline void *AVL_get_data_from_node(AVL_bst_node *node)
{
    assert(node);

    if (node->subtrees[1]) { /* not a leaf node */
        return NULL;
    }
    return (void *) node->subtrees[0];
}

void AVL_process_node_list(AVL_bst_node *nodelst, AVL_map mapfn)
{
    for (AVL_bst_node *p = nodelst; p; p = p->subtrees[1]) {
        mapfn(p->subtrees[0]);
    }
}

void AVL_clear_node_list(AVL_bst *restrict tree, AVL_bst_node *nodelst)
{
    assert(tree && nodelst);

    AVL_bst_node *q = NULL;
    for (AVL_bst_node *p = nodelst; p; p = q) {
        q = p->subtrees[1];
        tree->freedata(p->subtrees[0]);
        tree->freenode(p);
    }
}

int AVL_insert(AVL_bst *restrict tree, void *key, void *data)
{
    assert(tree && tree->root && key && data);

    if (!tree->root->subtrees[0]) {  /* empty tree */
        tree->root->subtrees[0] = (AVL_bst_node *) data;
        tree->root->subtrees[1] = NULL;
        tree->root->key = key;
        tree->root->height = 0;
        tree->nitems++;
    }
    else {
        AVL_traverser trav;
        AVL_traverser_init(&trav, tree);

        while (trav.current->subtrees[1] && trav.height <= AVL_BST_MAX_HEIGHT) {
            trav.stack[trav.height++] = trav.current;  /* for rebalancing */
            if (tree->compare(key, trav.current->key) < 0) {
                trav.current = trav.current->subtrees[0];
            }
            else {
                trav.current = trav.current->subtrees[1];
            }
        }
        if (trav.height > AVL_BST_MAX_HEIGHT){
            return AVL_MAX_HEIGHT_ERR;
        }
        /* Found a candidate leaf */
        if (!tree->compare(trav.current->key, key)) { /* permit only distinct keys */
#ifdef AVL_ONLY_DISTINCT_KEYS
            return AVL_DISTINCT_KEY_ERR;
#else  /* Updates the data if try same key insertion */
            void *tmpdata = (void *) trav.current->subtrees[0];
            trav.current->subtrees[0] = (AVL_bst_node *) data;
            tree->freedata(tmpdata);
            goto UPDATE_ONLY;
#endif
        }

        AVL_bst_node *oldleaf = tree->alloc(sizeof *oldleaf);
        oldleaf->key = trav.current->key;
        oldleaf->subtrees[0] = trav.current->subtrees[0];
        oldleaf->subtrees[1] = NULL;
        oldleaf->height = 0;

        AVL_bst_node *newleaf = tree->alloc(sizeof *newleaf);
        newleaf->key = key;
        newleaf->subtrees[0] = (AVL_bst_node *) data;
        newleaf->subtrees[1] = NULL;
        newleaf->height = 0;

        if (tree->compare(trav.current->key, key) < 0) {
            trav.current->subtrees[0] = oldleaf;
            trav.current->subtrees[1] = newleaf;
            trav.current->key = key;
        }
        else {
            trav.current->subtrees[0] = newleaf;
            trav.current->subtrees[1] = oldleaf;
        }

        trav.current->height = 1;
        ++tree->nitems;
        /* Rebalance */
        while (trav.height) {
            trav.current = trav.stack[--trav.height];
            int32_t oldheight = trav.current->height;

            if (trav.current->subtrees[0]->height - trav.current->subtrees[1]->height == 2) {
                /* Left-left unbalanced */
                if (trav.current->subtrees[0]->subtrees[0]->height - trav.current->subtrees[1]->height == 1) {
                    tree_right_rotate(trav.current);
                    trav.current->subtrees[1]->height = trav.current->subtrees[1]->subtrees[0]->height + 1;
                    trav.current->height = trav.current->subtrees[1]->height + 1;
                }
                /* Left-right unbalanced */
                else {
                    tree_left_rotate(trav.current->subtrees[0]);
                    tree_right_rotate(trav.current);
                    int32_t tmpheight = trav.current->subtrees[0]->subtrees[0]->height;
                    trav.current->subtrees[0]->height = tmpheight + 1;
                    trav.current->subtrees[1]->height = tmpheight + 1;
                    trav.current->height = tmpheight + 2;
                }
            }
            else if (trav.current->subtrees[0]->height - trav.current->subtrees[1]->height == -2) {
                /* Right-right unbalanced */
                if (trav.current->subtrees[1]->subtrees[1]->height - trav.current->subtrees[0]->height == 1) {
                    tree_left_rotate(trav.current);
                    trav.current->subtrees[0]->height = trav.current->subtrees[0]->subtrees[1]->height + 1;
                    trav.current->height = trav.current->subtrees[0]->height + 1;
                }
                /* Right-left unbalanced */
                else {
                    tree_right_rotate(trav.current->subtrees[1]);
                    tree_left_rotate(trav.current);
                    int32_t tmpheight = trav.current->subtrees[1]->subtrees[1]->height;
                    trav.current->subtrees[0]->height = tmpheight + 1;
                    trav.current->subtrees[1]->height = tmpheight + 1;
                    trav.current->height = tmpheight + 2;
                }
            }
            else { /* update height even if there was no rotation */
                if (trav.current->subtrees[0]->height > trav.current->subtrees[1]->height) {
                    trav.current->height = 1 + trav.current->subtrees[0]->height;
                }
                else {
                    trav.current->height = 1 + trav.current->subtrees[1]->height;
                }
            }
            if (trav.current->height == oldheight)
                break;
        }
    }

#ifndef AVL_ONLY_DISTINCT_KEYS
   UPDATE_ONLY:
#endif

    return AVL_SUCCESS;
}

int AVL_delete(AVL_bst *restrict tree, void *key, void **data)
{
    assert(tree && tree->root && key);

    if (!tree->root->subtrees[0]) {  // empty tree
        *data = NULL;
        return AVL_EMPTY_TREE_ERR;
    }
    else if (!tree->root->subtrees[1]) { // only one element in tree
        if (!tree->compare(tree->root->key, key)) {
            *data = (void *) tree->root->subtrees[0];
            tree->root->subtrees[0] = NULL;
            --tree->nitems;
        }
        else {
            *data = NULL;
            return AVL_DELETE_NON_EXISTENT_KEY_ERR;
        }
    }
    else {
        AVL_traverser trav;
        AVL_traverser_init(&trav, tree);

        /* otherchild keeps track of the other child of the parent node */
        AVL_bst_node *parentnode = NULL;
        AVL_bst_node *otherchild = NULL;

        while (trav.current->subtrees[1]) { // while current is not a leaf
            trav.stack[trav.height++] = trav.current;
            parentnode = trav.current;

            if (tree->compare(trav.current->key, key) > 0) {
                trav.current = trav.current->subtrees[0];
                otherchild = parentnode->subtrees[1];
            }
            else {
                trav.current = trav.current->subtrees[1];
                otherchild = parentnode->subtrees[0];
            }
        }
        if (tree->compare(trav.current->key, key)) {
            *data = NULL;
            return AVL_DELETE_NON_EXISTENT_KEY_ERR;
        }
        trav.height--; /* ? remove parentnode from stack */
        parentnode->key = otherchild->key;
        parentnode->height = otherchild->height;
        parentnode->subtrees[0] = otherchild->subtrees[0];
        parentnode->subtrees[1] = otherchild->subtrees[1];
        *data = (void *) trav.current->subtrees[0];
        tree->freenode(otherchild);
        tree->freenode(trav.current);
        --tree->nitems;
        /* Rebalance */
        while (trav.height) {
            trav.current = trav.stack[--trav.height];
            int32_t oldheight = trav.current->height;

            if (trav.current->subtrees[0]->height - trav.current->subtrees[1]->height == 2) {
                /* Left-left unbalanced */
                if (trav.current->subtrees[0]->subtrees[0]->height - trav.current->subtrees[1]->height == 1) {
                    tree_right_rotate(trav.current);
                    trav.current->subtrees[1]->height = trav.current->subtrees[1]->subtrees[0]->height + 1;
                    trav.current->height = trav.current->subtrees[1]->height + 1;
                }
                /* Left-right unbalanced */
                else {
                    tree_left_rotate(trav.current->subtrees[0]);
                    tree_right_rotate(trav.current);
                    int32_t tmpheight = trav.current->subtrees[0]->subtrees[0]->height;
                    trav.current->subtrees[0]->height = tmpheight + 1;
                    trav.current->subtrees[1]->height = tmpheight + 1;
                    trav.current->height = tmpheight + 2;
                }
            }
            else if (trav.current->subtrees[0]->height - trav.current->subtrees[1]->height == -2) {
                /* Right-right unbalanced */
                if (trav.current->subtrees[1]->subtrees[1]->height - trav.current->subtrees[0]->height == 1) {
                    tree_left_rotate(trav.current);
                    trav.current->subtrees[0]->height = trav.current->subtrees[0]->subtrees[1]->height + 1;
                    trav.current->height = trav.current->subtrees[0]->height + 1;
                }
                /* Right-left unbalanced */
                else {
                    tree_right_rotate(trav.current->subtrees[1]);
                    tree_left_rotate(trav.current);
                    int32_t tmpheight = trav.current->subtrees[1]->subtrees[1]->height;
                    trav.current->subtrees[0]->height = tmpheight + 1;
                    trav.current->subtrees[1]->height = tmpheight + 1;
                    trav.current->height = tmpheight + 2;
                }
            }
            else { /* update height even if there was no rotation */
                if (trav.current->subtrees[0]->height > trav.current->subtrees[1]->height) {
                    trav.current->height = 1 + trav.current->subtrees[0]->height;
                }
                else {
                    trav.current->height = 1 + trav.current->subtrees[1]->height;
                }

            }
            if (trav.current->height == oldheight)
                break;
        }
    }
    return AVL_SUCCESS;
}

int AVL_traverse_preorder(AVL_bst *restrict tree, AVL_map mapfn)
{
    if (!tree->root->subtrees[0]) { /* empty tree */
        return AVL_EMPTY_TREE_ERR;
    }

    AVL_traverser trav;
    AVL_traverser_init(&trav, tree);

    size_t itemsmapped = tree->nitems;
    while (itemsmapped && trav.height <= AVL_BST_MAX_HEIGHT) {
        if (!trav.current->subtrees[1]) { /* if it's a leaf */
            mapfn(trav.current->subtrees[0]); /* map over data */
            itemsmapped--;
            trav.current = trav.stack[--trav.height]->subtrees[1];
        }
        else {
            trav.stack[trav.height++] = trav.current;
            trav.current = trav.current->subtrees[0];
        }
    }

    if (trav.height > AVL_BST_MAX_HEIGHT) {
        return AVL_MAX_HEIGHT_ERR;
    }
    else {
        return AVL_SUCCESS;
    }
}

/* End Of File */
