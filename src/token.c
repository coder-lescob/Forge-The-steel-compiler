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
        {TOKEN_DOLLAR, "$"},
        {TOKEN_NWLINE, "\n"},
        {TOKEN_PREPROC_ALIAS, "alias"}
    };

    for (size_t i = 0; i < (sizeof(staticTokens) / sizeof(Token)); i++) {
        if (strcmp(staticTokens[i].word, str) == 0) {
            return staticTokens[i].type;
        }
    }

    if (isid(str)) return TOKEN_ID;

    // per default it's illegal token
    return TOKEN_ILLEGAL;
}