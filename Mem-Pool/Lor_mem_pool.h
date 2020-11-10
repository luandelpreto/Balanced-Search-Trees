/* C Header file:
 *              Lor_mem_pool.h
 *
 * Interface for Lorena library memory pool.  This extension is
 * provided if the user doesn't want to provide its own special
 * memory manager.
 */
#ifndef LOR_MEM_POOL_H
#define LOR_MEM_POOL_H 1

#include <stdbool.h>
#include <stddef.h>

typedef struct _Lor_mem_pool Lor_mem_pool;

/**********************************************************
 * \brief Alloc a Lor_mem_pool from the heap
 *
 * \return a  pointer  to  Lor_mem_pool  on  the  heap,  if
 *         successfull
 * \return NULL    if allocation fails
 **********************************************************/
extern Lor_mem_pool *Lor_mem_pool_create(void);

/**********************************************************
 * \brief Initialize the memory pool with specified size
 *
 * \param pool    the Lor_mem_pool pointer to objecto to be
 *                initialized
 * \param size    the size of the initial block allocated
 *
 * \return LOR_SUCCESS                if  initialization was successfull
 * \return LOR_ZERO_SIZE_ALLOC_ERR    if trying to alloc a zero sized block
 * \return LOR_USIZE_OVERFLOW_ERR     if a size_t overflow happened during allocation
 **********************************************************/
extern int Lor_mem_pool_init(Lor_mem_pool *pool, size_t size);

/**********************************************************
 * \brief Deallocates a Lor_mem_pool of the heap
 *
 * \param pool    a pointer to the pointer that  points  to
 *                the Lor_mem_pool object on the heap
 *
 * \return LOR_SUCCESS                  if operation was successfull
 * \return LOR_POSSIBLE_MEMLEAK_WARN    if the block of memory associated
 *                                         with the pool is non-NULL
 **********************************************************/
extern int Lor_mem_pool_destroy(Lor_mem_pool **pool);

/**********************************************************
 * \brief Discard all the heap memory that the memory  pool
 *        is responsible for.
 *
 * \param pool              the memory pool whose memory blocks will
 *                          be discarded
 * \param invalidate_mem    a boolean value that  indicates  if  the
 *                          memory is to be invalidated
 *
 * \return LOR_SUCCESS             if the operation was successfull
 * \return LOR_FREE_NULLPTR_WARN   if the memory pool block is NULL
 **********************************************************/
extern int Lor_mem_pool_discard(Lor_mem_pool *pool, bool invalidate_mem);

/**********************************************************
 * \brief Allocates a block of memory from the memory pool.
 *
 * \param pool    the memory pool from which allocate the block
 * \param len     the size of the block to be allocated
 *
 * \return the new block allocated from the pool
 * \return NULL if the allocation fails
 **********************************************************/
extern void *Lor_mem_pool_alloc(Lor_mem_pool *pool, size_t len);

/**********************************************************
 * \brief Allocates a block of memory from the memory  pool
 *        and zero the memory.
 *
 * \param pool     the memory pool from which allocate the block
 * \param count    the number of blocks to be allocated
 * \param size     the size of each block
 *
 * \return the new block allocated from the pool
 * \return NULL if the allocation fails
 **********************************************************/
extern void *Lor_mem_pool_calloc(Lor_mem_pool *pool, size_t count, size_t size);

/**********************************************************
 * \brief Allocate memory from the memory pool and  copy  a
 *        string  into  it.  This  handles  NULL-terminated
 *        ascii strings.
 *
 * \param pool    the memory pool from which allocate the block
 * \param str     the string to be copied into the block
 *
 * \return a pointer to the block containing the string
 * \return NULL if the allocation fails
 **********************************************************/
extern char *Lor_mem_pool_strdup(Lor_mem_pool *pool, const char *str);

/**********************************************************
 * \brief Allocate memory from the memory pool and  copy  a
 *        string  into  it.  This  handles  NULL-terminated
 *        ascii strings.
 *
 * \param pool    the memory pool from which allocate the block
 * \param str     the string to be copied into the block
 * \param len     the maximum size to be copied from str
 *
 * \return a pointer to the block containing the string
 * \return NULL if the allocation fails
 **********************************************************/
extern char *Lor_mem_pool_strndup(Lor_mem_pool *pool, const char *str, size_t len);

/**********************************************************
 * \brief Move the memory associated with the 'src' pool to
 *        the 'dst' pool. The 'src' pool will be empty  and
 *        not contain any memory. 'src'  still needs  to be
 *        free'd with a call to Lor_mem_pool_discard.
 *
 * \param dst    the destination pool to which to move 'src'
 * \param src    the source pool to move to 'dst'
 *
 * \return LOR_SUCCESS    if operation was successfull
 * \return
 **********************************************************/
extern int Lor_mem_pool_combine(Lor_mem_pool *dst, Lor_mem_pool *src);

/**********************************************************
 * \brief Check if a memory pointed at by 'mem' is part  of
 *        the range of memory managed by the specified pool.
 *
 * \param pool    the memory pool to be checked against
 * \param mem     the pointer to the memory to be checked
 *
 * \return true    if 'mem' is in the block associated with 'pool'
 * \return false   otherwise
 **********************************************************/
extern bool Lor_mem_pool_contains(Lor_mem_pool *pool, void *mem);

#endif
