/* C Header file:
 *               AVLbst.h
 *
 * Interface for AVL binary search tree.
 *
 * This implements a 'leaf tree', where the data are stored on the
 * leafs of the BST.
 *
 * Public functions:
 *
 * AVL_bst *AVL_create(void);
 *     This functions returns a new AVL_bst on the heap.
 *
 * int AVL_init(AVL_bst *restrict tree, AVL_compare compare, AVL_alloc alloc,
                      AVL_free_node freenode, AVL_free_data freedata);
 *     This function initializes the AVL_bst attributes.
 *     Parameters:
 *         - tree     -> an AVL tree created by AVL_create
 *         - compare  -> a comparison function for keys
 *         - alloc    -> an alloc function for tree nodes
 *         - freenode -> a free function for deallocation of nodes
 *         - freedata -> a free function for deallocation of data
 *     Returns:
 *         - AVL_SUCCESS if successfull
 *         - AVL_COMPARE_FN_NOT_PROVIDED_ERR if compare function has not been
 *           provided
 *         - AVL_ALLOC_FN_NOT_PROVIDED_ERR if  allocation  function  has  not
 *           been provided
 *
 * int AVL_destroy(AVL_bst **restrict tree);
 *     This function destroys a AVL_bst allocated by AVL_create.
 *     Parameters:
 *         - tree -> the tree to be destroyed (allocated by AVL_create)
 *     Returns:
 *         - AVL_SUCCESS if successfull
 *         - AVL_FREE_NULLPTR_ERR if *tree is a NULL pointer
 *         - AVL_DESTROY_ROOT_NON_NULL if the root of the tree is non-NULL
 *
 * int AVL_clear(AVL_bst *restrict tree);
 *     This function empties the parameter tree without freeing tree.
 *     Parameters:
 *         - tree -> the tree to be emptied
 *     Returns:
 *         - AVL_SUCCESS if successfull
 *         - AVL_FREE_NULLPTR_ERR if the root of the tree is NULL
 *         - AVL_EMPTY_TREE_ERR if the tree is already empty
 *
 * AVL_bst_node *AVL_find(AVL_bst *restrict tree, const void *key);
 *     This function searches for key in tree.
 *     Parameters:
 *         - tree -> the AVL tree to be searched
 *         - key  -> the key to be searched
 *     Returns:
 *         - NULL if key is not on tree
 *         - AVL_bst_node *node, the leaf node with that key
 *
 * AVL_bst_node *AVL_interval_find(AVL_bst *restrict tree, const void *a, const void *b);
 *     This function searches for a key interval [a, b)
 *     Parameters:
 *         - tree -> the AVL tree to be searched
 *         - a    -> lower limit of interval
 *         - b    -> upper limit of interval
 *     Returns:
 *         - NULL if the interval does not exist in the tree's keys
 *         - AVL_bst_node *list, a list of AVL tree nodes with  the  interval
 *           [a, b). If one of the limits is out of range in the tree's  keys,
 *           it returns [a, <max key on tree>] or [<min key on tree>, b).
 *
 * void *AVL_get_data_from_node(AVL_bst_node *node);
 *     This function gets the data from a leaf node.
 *     Parameters:
 *         - node -> the leaf node from which the data is obtained
 *     Returns:
 *         - NULL, if node is not a leaf
 *         - void *data, a void pointer to the data in *node
 *
 * void AVL_process_node_list(AVL_bst_node *nodelst, AVL_map mapfn);
 *     Function that processes the node list created by AVL_interval_find.
 *     Parameters:
 *         - nodelst ->  a  linked   list  from  AVL_bst_node's  created  by
 *                       AVL_interval_find
 *         - mapfn   -> a mapping function to map the nodes of the list
 *
 * void AVL_clear_node_list(AVL_bst *restrict tree, AVL_bst_node *nodelst);
 *     Function that empties the nodes list created by AVL_interval_find
 *     Parameters:
 *         - tree    -> the AVL tree from which the list were obtained
 *         - nodelst -> the node's list created by AVL_interval_find
 *
 * int AVL_insert(AVL_bst *restrict tree, void *key, void *data);
 *     Function that inserts a new data with given key in the tree.
 *     Parameters:
 *         - tree -> the AVL tree on which to insert the data
 *         - key  -> the key of the data
 *         - data -> the data itself as a void pointer
 *     Returns:
 *         - AVL_SUCCESS, if successfull
 *         - AVL_MAX_HEIGHT_ERR, if the tree reaches the  maximum  permitted
 *           height
 *         - AVL_DISTINCT_KEY_ERR (TO CHANGE)
 *
 * int AVL_delete(AVL_bst *restrict tree, void *key, void **data);
 *     Function that deletes the data with the given key in the tree.
 *     Parameters:
 *         - tree -> the AVL tree from which the data will be removed
 *         - key  -> the key of the data to be removed
 *         - data -> a void ** to return the data as an output parameter
 *     Returns:
 *         - AVL_SUCCESS, if successfull
 *         - AVL_EMPTY_TREE_ERR, if the tree is empty
 *         - AVL_DELETE_NON_EXISTENT_KEY_ERR, if the given key does not exist
 *           in the tree
 *
 * int AVL_traverse_preorder(AVL_bst *restrict tree, AVL_map mapfn);
 *     Function the traverses the tree applying mapfn function.
 *     Parameters:
 *         - tree  -> the AVL_bst tree to be traversed
 *         - mapfn -> the mapping function to be applied over each node  of
 *           tree
 *     Returns:
 *         - AVL_SUCCESS, if successfull
 *         - AVL_EMPTY_TREE_ERR, if the tree is empty
 *         - AVL_MAX_HEIGHT_ERR, if the height of the tree is greater  than
 *           the permitted
 *
 * TODO:
 * - Change to permit same key insertions
 * - Add compare_data and change compare to compare_key
 * - Rigorous testing
 * - ...
 */
#ifndef AVL_BST_H
#define AVL_BST_H 1

#include <inttypes.h>
#include <stdlib.h>

enum {
    AVL_SUCCESS=0,
    AVL_COMPARE_FN_NOT_PROVIDED_ERR,
    AVL_ALLOC_FN_NOT_PROVIDED_ERR,
    AVL_FREE_NULLPTR_ERR,
    AVL_DESTROY_ROOT_NON_NULL,
    AVL_DISTINCT_KEY_ERR,
    AVL_MAX_HEIGHT_ERR,
    AVL_EMPTY_TREE_ERR,
    AVL_DELETE_NON_EXISTENT_KEY_ERR,
};

typedef struct _AVL_bst_node AVL_bst_node;
typedef struct _AVL_bst AVL_bst;

typedef int32_t (*AVL_compare)(const void *key1, const void *key2);
typedef void *(*AVL_alloc)(size_t nbytes);
typedef void (*AVL_free_node)(void *ptr);
typedef void (*AVL_free_data)(void *ptr);
typedef void (*AVL_map)(void *ptr);

extern AVL_bst *AVL_create(void);
extern int AVL_init(AVL_bst *restrict tree, AVL_compare compare, AVL_alloc alloc,
                     AVL_free_node freenode, AVL_free_data freedata);
extern int AVL_destroy(AVL_bst **restrict tree);
extern int AVL_clear(AVL_bst *restrict tree);
extern AVL_bst_node *AVL_find(AVL_bst *restrict tree, const void *key);
extern AVL_bst_node *AVL_interval_find(AVL_bst *restrict tree, const void *a, const void *b);
extern void *AVL_get_data_from_node(AVL_bst_node *node);
extern void AVL_process_node_list(AVL_bst_node *nodelst, AVL_map mapfn);
extern void AVL_clear_node_list(AVL_bst *restrict tree, AVL_bst_node *nodelst);
extern int AVL_insert(AVL_bst *restrict tree, void *key, void *data);
extern int AVL_delete(AVL_bst *restrict tree, void *key, void **data);
extern int AVL_traverse_preorder(AVL_bst *restrict tree, AVL_map mapfn);

#endif
