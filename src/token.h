#ifndef HEADER_TOKEN
#define HEADER_TOKEN

// The type of a token
typedef enum {
    TOKEN_ILLEGAL,
    TOKEN_BLANK,

    // identifier
    TOKEN_ID,
    
    // symboles
    TOKEN_DOLLAR,
    TOKEN_NWLINE,

    // preproc
    TOKEN_PREPROC_ALIAS,

    TOKEN_EOF
} TokenType;

// The structure of a token
typedef struct {
    TokenType type;
    char     *word;
} Token;

/*
* Classify a token, if it doesn't match any category it is marked as ILLEGAL
* @param   str The string of the token
* @returns The type of the token
*/
TokenType ClassifyToken(char *str);

#endif