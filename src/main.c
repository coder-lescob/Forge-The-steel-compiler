// std libs
#include <stdio.h>
#include <string.h>

// project
#include "token.h"

#define STR_LEN 100

int main(void) {
    // get a string from the user
    char str[STR_LEN + 1]; // + the 0 terminator
    fgets(str, STR_LEN, stdin);

    // remove the new line character
    if (str[strlen(str) - 1] == '\n') {
        str[strlen(str) - 1] = 0;
    }

    // create a token to hold it
    Token token = {.type = ClassifyToken(str), .word = str};

    // print it
    printf("token %s of type %d\n", token.word, token.type);
}