// std libs
#include <string.h>

// project
#include "token.h"

static int isid(char *str) {
    // If the string is empty it is not an id
    if (strlen(str) == 0) return 0;

    for (size_t i = 0; i < strlen(str); i++) {
        // belongs to [a; z] U [A; Z] U { _ }
        if ((str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'a' && str[i] <= 'z') || str[i] == '_') continue;

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
        {TOKEN_DOLLAR, "$"},
        {TOKEN_NWLINE, "\n"},
        {TOKEN_CONST, "const"},

        // math symbols
        {TOKEN_EQUAL, "="},
        {TOKEN_PLUS, "+"},
        {TOKEN_MINUS, "-"},
        {TOKEN_STAR, "*"},
        {TOKEN_SLASH, "/"},

        // comments
        {TOKEN_OPEN_COMMENT, "/*"},
        {TOKEN_CLOSE_COMMENT, "*/"},
        {TOKEN_COMMENT, "//"}
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