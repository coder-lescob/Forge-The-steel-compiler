#ifndef HEADER_PREPROC
#define HEADER_PREPROC

// std libs
#include <stdlib.h>

// project
#include "token.h"
#include "stack.h"

typedef enum PPST {
    PREPROC_CONST
} PreProcStatmentType;

typedef struct PPS {
    PreProcStatmentType type;
    Token *tokens;
    size_t numtokens;
} PreProcStatment;

/*
* Pre-process the string of tokens.
* @param tokens The pointer to the array of token.
*   NOTE: THE LAST TOKEN SOULD HAVE TYPE TOKEN_EOF
*/
void PreProccess(Stack *tokens);

#endif