/* C Header file:
 *              Lor_error_log.h
 *
 * This probably will be extended
 */
#ifndef LOR_ERROR_LOG
#define LOR_ERROR_LOG 1

#include <stdio.h>
#include <errno.h>

#define LOR_PERROR(msg, func)                                                            \
    fprintf(stderr, "[%s:%s:%s:%lu]: ", "libLorenaBSTs", __FILE__, func, __LINE__+0UL);  \
    perror(msg);

#endif
