/* C Header file:
 *              Lor_error_log.h
 *
 * This probably will be extended
 */
#ifndef LOR_ERROR_LOG
#define LOR_ERROR_LOG 1

#include <stdio.h>
#include <errno.h>

#define LOR_PERROR(msg)                                                         \
    fprintf(stderr, "[%s:%s:%lu]: ", "libLorenaBSTs", __FILE__, __LINE__+0UL);  \
    perror(msg);

#endif
