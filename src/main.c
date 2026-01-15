#include <stdio.h>
#include "token.h"

#define STR_LEN 100

int main(void) {
    // get a string from the user
    char str[STR_LEN + 1]; // + the 0 terminator
    fgets(str, STR_LEN, stdin);

    // create a token to hold it
    Token token = {.type = TOKEN_ILLEGAL, .word = str};

    // print it
    printf("illegal token %s\n", str);
}