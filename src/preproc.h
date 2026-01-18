#ifndef HEADER_PREPROC
#define HEADER_PREPROC

// std libs
#include <stdlib.h>

// project
#include "token.h"

typedef enum PPST {
    PREPROC_ALIAS
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
void PreProccess(Token *tokens);

#endif