#ifndef HEADER_LEXER
#define HEADER_LEXER

// project
#include "token.h"

/*
* Tokenizte a string into a list of token.
* The last token is an end of file token, 
* marked with the type TOKEN_EOF.
* @param str The string to tokenize
* @returns The pointer to the array of token, 
* which is allocated on the heap, don't forget to free it.
*/
Token *Tokenize(char *str);

#endif