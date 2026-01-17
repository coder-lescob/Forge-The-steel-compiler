// std libs
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// project
#include "token.h"
#include "lexer.h"
#include "preproc.h"

static size_t flen(FILE *fptr) {
    size_t size = 0;
    for (char c = 0; (c = fgetc(fptr)) != EOF; size++);
    fseek(fptr, 0, SEEK_SET);

    return size + 1;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("usage: %s <filename>\n", argv[0]);
        exit(-1);
    }

    // open the file
    FILE *fptr = fopen(argv[1], "r");

    if (!fptr) {
        printf("unable to open file %s\n", argv[1]);
        exit(-1);
    }

    // get the length of the file
    size_t size = flen(fptr);

    // get a string from the user
    char *str = calloc(size + 1, sizeof(char)); // + the 0 terminator
    fread(str, sizeof(char), size, fptr);

    // tokenize the string
    Token *tokens = Tokenize(str);

    // call the pre-processor
    PreProccess(tokens);

    // print all of them
    for (Token *token = tokens; token->type != TOKEN_EOF; token++) {
        if (token->word) {
            printf("Token %s of type %d\n", token->word, token->type);

            free(token->word);
        }
    }

    free(tokens);
    fclose(fptr);
}