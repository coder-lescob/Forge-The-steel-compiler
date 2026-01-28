#ifndef HEADER_TOKEN
#define HEADER_TOKEN

// The type of a token
typedef enum TokenType {
    TOKEN_ILLEGAL,
    TOKEN_BLANK,
    TOKEN_ROOT,

    // identifier
    TOKEN_ID,
    
    // symboles
    TOKEN_DOLLAR,
    TOKEN_EQUAL,
    TOKEN_NWLINE,
    TOKEN_DOT,
    TOKEN_OPEN_CURLY_BRACES,
    TOKEN_CLOSE_CURLY_BRACES,

    // math
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,

    // litteral
    TOKEN_NUMBER,

    // preproc
    TOKEN_CONST,

    // comments
    TOKEN_OPEN_COMMENT,
    TOKEN_CLOSE_COMMENT,
    TOKEN_COMMENT,

    TOKEN_EOF
} TokenType;

// The structure of a token
typedef struct Token {
    TokenType type;
    char     *word;
} Token;

/*
* Classify a token, if it doesn't match any category it is marked as ILLEGAL
* @param   str The string of the token
* @returns The type of the token
*/
TokenType ClassifyToken(char *str);

/*
* Compare a and b.
* If they match return 1
* Otherwise return 0
* @param a The first token.
* @param b The second token.
*/
int tokcmp(Token *a, Token *b);

#endif