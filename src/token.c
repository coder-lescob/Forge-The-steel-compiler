// std libs
#include <stdlib.h>
#include <string.h>

// project
#include "token.h"

static int isid(char *str) {
    // If the string is empty it is not an id
    if (strlen(str) == 0) return 0;

    for (size_t i = 0; i < strlen(str); i++) {
        // belongs to [a; z] U [A; Z] U { _ }
        if ((str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A' && str[i] <= 'Z') || str[i] == '_') continue;

        // belongs to [0; 9]
        if (i > 0 && str[i] >= '0' && str[i] <= '9') continue;

        // not id
        return 0;
    }

    // id
    return 1;
}

static int isnum(char *str) {
    // If the string is empty it is not a number
    if (strlen(str) == 0) return 0;

    for (size_t i = 0; i < strlen(str); i++) {
        if (str[i] >= '0' && str[i] <= '9') continue;
        return 0;
    }
    return 1;
}

// implements Classify token
TokenType ClassifyToken(char *str) {
    if (!str) return TOKEN_ILLEGAL;

    if (strlen(str) == 1 && (str[0] == ' ' || str[0] == '\t'))
        // blank tokens
        return TOKEN_BLANK;

    /*
    * Here add all tokens for the language
    */

    static Token staticTokens[] = {
        // preproc
        {TOKEN_DOLLAR, "$", 1 /* string on stack no need to free */},
        {TOKEN_NWLINE, "\n", 1 /* string on stack no need to free */},
        {TOKEN_CONST, "const", 1 /* string on stack no need to free */},
        {TOKEN_DOT, ".", 1 /* string on stack no need to free */},
        {TOKEN_OPEN_PARENTHESES, "(", 1 /* string on stack no need to free */ },
        {TOKEN_CLOSE_PARENTHESES, ")", 1 /* string on stack no need to free */},
        {TOKEN_OPEN_CURLY_BRACES, "{", 1 /* string on stack no need to free */},
        {TOKEN_CLOSE_CURLY_BRACES, "}", 1 /* string on stack no need to free */},

        // math symbols
        {TOKEN_EQUAL, "=", 1 /* string on stack no need to free */},
        {TOKEN_PLUS, "+", 1 /* string on stack no need to free */},
        {TOKEN_MINUS, "-", 1 /* string on stack no need to free */},
        {TOKEN_STAR, "*", 1 /* string on stack no need to free */},
        {TOKEN_SLASH, "/", 1 /* string on stack no need to free */},

        // comments
        {TOKEN_OPEN_COMMENT, "/*", 1 /* string on stack no need to free */},
        {TOKEN_CLOSE_COMMENT, "*/", 1 /* string on stack no need to free */},
        {TOKEN_COMMENT, "//", 1 /* string on stack no need to free */},
    };

    for (size_t i = 0; i < (sizeof(staticTokens) / sizeof(Token)); i++) {
        if (strcmp(staticTokens[i].word, str) == 0) {
            return staticTokens[i].type;
        }
    }

    if (isnum(str)) return TOKEN_NUMBER;
    if (isid(str))  return TOKEN_ID;

    // per default it's illegal token
    return TOKEN_ILLEGAL;
}

int tokcmp(Token *a, Token *b) {
    // If their type doesn't match, they don't
    if (a->type != b->type) return 0;
    return strcmp(a->word, b->word) == 0;
}

void freetoken(Token *token) {
    // if needed free its string
    if ((!token->freed) && token->word) {
        // free it
        free(token->word);

        // ensure no double free
        token->word  = NULL;
        token->freed = 1;
    }
}