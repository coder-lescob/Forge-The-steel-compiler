#ifndef HEADER_ERROR
#define HEADER_ERROR

/**
 * this defines all possible errors
 */
typedef enum {
    ERROR_NULL,
    ERROR_LEXIC,
    ERROR_SYNTAX,
    ERROR_SYNTAX_PROPAGATING,
    ERROR_PREPROCESSING,
    ERROR_ALLOCATION_FAILED,
} Error;

#endif