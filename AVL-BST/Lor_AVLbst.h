/* C Header file:
 *               Lor_AVLbst.h
 *
 * Interface for AVL binary search tree.
 *
 * This implements a 'leaf tree', where the data are stored on the
 * leafs of the BST.
 *
 * This implementation has two 'modes':
 * 1) Permits only same distinct keys in the tree. To use this,  define
 *    in the implementation scope:
 *#define LOR_AVL_ONLY_DISTINCT_KEYS
 * 2) Updates the data if you try same key insertion (The previous data
 *    will be lost. This is the standard mode.
 *
 * IMPORTANT: It is responsability of the user to allocate the data; It
 * is also responsability of the user to deallocate the data  except in
 * AVL_clear and for updates in AVL_insert. It is responsability of the
 * user to allocate and deallocate the keys.
 *
 * Public functions:
 *
 * AVL_bst *Lor_AVL_create(void);
 *     This functions returns a new Lor_AVL_bst on the heap.
 *
 * int Lor_AVL_init(Lor_AVL_bst *restrict tree, Lor_AVL_compare compare, Lor_AVL_alloc alloc,
 *              Lor_AVL_free_node freenode, Lor_AVL_free_data freedata);
 *     This function initializes the AVL_bst attributes. Pass NULL to
 *     freedata if the allocations are done in  stack, otherwise pass
 *     a free-like function.
 *     Parameters:
 *         - tree     -> an AVL tree created by AVL_create
 *         - compare  -> a comparison function for keys
 *         - alloc    -> an alloc function for tree nodes
 *         - freenode -> a free function for deallocation of nodes
 *         - freedata -> a free function for deallocation of data
 *     Returns:
 *         - LOR_AVL_SUCCESS if successfull
 *         - LOR_AVL_COMPARE_FN_NOT_PROVIDED_ERR if compare function has not been
 *           provided
 *         - LOR_AVL_ALLOC_FN_NOT_PROVIDED_ERR if  allocation  function  has  not
 *           been provided
 *
 * int Lor_AVL_destroy(Lor_AVL_bst **restrict tree);
 *     This function destroys a Lor_AVL_bst allocated by AVL_create.
 *     Parameters:
 *         - tree -> the tree to be destroyed (allocated by AVL_create)
 *     Returns:
 *         - LOR_AVL_SUCCESS if successfull
 *         - LOR_AVL_FREE_NULLPTR_ERR if *tree is a NULL pointer
 *         - LOR_AVL_DESTROY_ROOT_NON_NULL if the root of the tree is non-NULL
 *
 * int Lor_AVL_clear(Lor_AVL_bst *restrict tree);
 *     This function empties the parameter tree without freeing tree.
 *     Parameters:
 *         - tree -> the tree to be emptied
 *     Returns:
 *         - LOR_AVL_SUCCESS if successfull
 *         - LOR_AVL_FREE_NULLPTR_ERR if the root of the tree is NULL
 *         - LOR_AVL_EMPTY_TREE_ERR if the tree is already empty
 *
 * Lor_AVL_bst_node *Lor_AVL_find(Lor_AVL_bst *restrict tree, const void *key);
 *     This function searches for key in tree.
 *     Parameters:
 *         - tree -> the AVL tree to be searched
 *         - key  -> the key to be searched
 *     Returns:
 *         - NULL if key is not on tree
 *         - Lor_AVL_bst_node *node, the leaf node with that key
 *
 * Lor_AVL_bst_node *Lor_AVL_interval_find(Lor_AVL_bst *restrict tree, const void *a, const void *b);
 *     This function searches for a key interval [a, b[
 *     Parameters:
 *         - tree -> the AVL tree to be searched
 *         - a    -> lower limit of interval
 *         - b    -> upper limit of interval
 *     Returns:
 *         - NULL if the interval does not exist in the tree's keys
 *         - Lor_AVL_bst_node *list, a list of AVL tree nodes with  the  interval
 *           [a, b[. If one of the limits is out of range in the tree's  keys,
 *           it returns [a, <max key on tree>] or [<min key on tree>, b[.
 *
 * void *Lor_AVL_get_data_from_node(Lor_AVL_bst_node *node);
 *     This function gets the data from a leaf node.
 *     Parameters:
 *         - node -> the leaf node from which the data is obtained
 *     Returns:
 *         - NULL, if node is not a leaf
 *         - void *data, a void pointer to the data in *node
 *
 * void Lor_AVL_process_node_list(Lor_AVL_bst_node *nodelst, Lor_AVL_map mapfn);
 *     Function that processes the node list created by AVL_interval_find.
 *     Parameters:
 *         - nodelst ->  a  linked   list  from  Lor_AVL_bst_node's  created  by
 *                       AVL_interval_find
 *         - mapfn   -> a mapping function to map the nodes of the list
 *
 * void Lor_AVL_clear_node_list(Lor_AVL_bst *restrict tree, Lor_AVL_bst_node *nodelst);
 *     Function that empties the nodes list created by AVL_interval_find.
 *     Note that this doesn't frees only the list of leaf nodes returned
 *     by Lor_AVL_interval_find, but not the data itself.
 *     Parameters:
 *         - tree    -> the AVL tree from which the list were obtained
 *         - nodelst -> the node's list created by Lor_AVL_interval_find
 *
 * int Lor_AVL_insert(Lor_AVL_bst *restrict tree, void *key, void *data);
 *     Function that inserts a new data with given key in the tree.
 *     IMPORTANT: It's advised that key and data must  be pont  to
 *                different locations in memory.
 *     Parameters:
 *         - tree -> the AVL tree on which to insert the data
 *         - key  -> the key of the data
 *         - data -> the data itself as a void pointer
 *     Returns:
 *         - LOR_AVL_SUCCESS, if successfull
 *         - LOR_AVL_MAX_HEIGHT_ERR, if the tree reaches the  maximum  permitted
 *           height
 *         - LOR_AVL_DISTINCT_KEY_ERR (if AVL_ONLY_DISTINCT_KEYS is defined)
 *
 * int Lor_AVL_delete(Lor_AVL_bst *restrict tree, void *key, void **data);
 *     Function that deletes the data with the given key in the tree.
 *     Parameters:
 *         - tree -> the AVL tree from which the data will be removed
 *         - key  -> the key of the data to be removed
 *         - data -> a void ** to return the data as an output parameter
 *     Returns:
 *         - LOR_AVL_SUCCESS, if successfull
 *         - LOR_AVL_EMPTY_TREE_ERR, if the tree is empty
 *         - LOR_AVL_DELETE_NON_EXISTENT_KEY_ERR, if the given key does not exist
 *           in the tree
 *
 * int Lor_AVL_traverse_lr(Lor_AVL_bst *restrict tree, Lor_AVL_map mapfn);
 *     Function the traverses the tree applying mapfn function over data.
 *     Parameters:
 *         - tree  -> the Lor_AVL_bst tree to be traversed
 *         - mapfn -> the mapping function to be applied over each node  of
 *           tree
 *     Returns:
 *         - LOR_AVL_SUCCESS, if successfull
 *         - LOR_AVL_EMPTY_TREE_ERR, if the tree is empty
 *         - LOR_AVL_MAX_HEIGHT_ERR, if the height of the tree is greater  than
 *           the permitted
 **************************************************************************/
#ifndef LOR_AVL_BST_H
#define LOR_AVL_BST_H 1

/*#define LOR_AVL_ONLY_DISTINCT_KEYS */

#include <inttypes.h>
#include <stdlib.h>

enum {
    LOR_AVL_SUCCESS=0,
    LOR_AVL_COMPARE_FN_NOT_PROVIDED_ERR,
    LOR_AVL_ALLOC_FN_NOT_PROVIDED_ERR,
    LOR_AVL_FREE_NULLPTR_ERR,
    LOR_AVL_DESTROY_ROOT_NON_NULL,
    LOR_AVL_DISTINCT_KEY_ERR,
    LOR_AVL_MAX_HEIGHT_ERR,
    LOR_AVL_EMPTY_TREE_ERR,
    LOR_AVL_DELETE_NON_EXISTENT_KEY_ERR,
};

typedef struct _Lor_AVL_bst_node Lor_AVL_bst_node;
typedef struct _Lor_AVL_bst Lor_AVL_bst;

typedef int32_t (*Lor_AVL_compare)(const void *key1, const void *key2);
typedef void *(*Lor_AVL_alloc)(size_t nbytes);
typedef void (*Lor_AVL_free_node)(void *ptr);
typedef void (*Lor_AVL_free_data)(void *ptr);
typedef void (*Lor_AVL_map)(void *ptr);

extern Lor_AVL_bst *Lor_AVL_create(void);
extern int Lor_AVL_init(Lor_AVL_bst *restrict tree, Lor_AVL_compare compare, Lor_AVL_alloc alloc,
                     Lor_AVL_free_node freenode, Lor_AVL_free_data freedata);
extern int Lor_AVL_destroy(Lor_AVL_bst **restrict tree);
extern int Lor_AVL_clear(Lor_AVL_bst *restrict tree);
extern Lor_AVL_bst_node *Lor_AVL_find(Lor_AVL_bst *restrict tree, const void *key);
extern Lor_AVL_bst_node *Lor_AVL_interval_find(Lor_AVL_bst *restrict tree, const void *a, const void *b);
extern void *Lor_AVL_get_data_from_node(Lor_AVL_bst_node *node);
extern void Lor_AVL_process_node_list(Lor_AVL_bst_node *nodelst, Lor_AVL_map mapfn);
extern void Lor_AVL_clear_node_list(Lor_AVL_bst *restrict tree, Lor_AVL_bst_node *nodelst);
extern int Lor_AVL_insert(Lor_AVL_bst *restrict tree, void *key, void *data);
extern int Lor_AVL_delete(Lor_AVL_bst *restrict tree, void *key, void **data);
extern int Lor_AVL_traverse_lr(Lor_AVL_bst *restrict tree, Lor_AVL_map mapfn);

#endif
