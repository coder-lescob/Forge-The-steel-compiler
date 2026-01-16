// std libs
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// project
#include "token.h"
#include "lexer.h"

#define STR_LEN 100

int main(void) {
    // get a string from the user
    char str[STR_LEN + 1]; // + the 0 terminator
    fgets(str, STR_LEN, stdin);

    // tokenize the string
    Token *tokens = Tokenize(str);

    // print all of them
    for (Token *token = tokens; token->type != TOKEN_EOF; token++) {
        if (token->word) {
            printf("Token %s of type %d\n", token->word, token->type);

            free(token->word);
        }
    }

    free(tokens);
}