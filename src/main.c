// std libs
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// project
#include "stack.h"
#include "token.h"
#include "lexer.h"
#include "preproc.h"
#include "parser.h"
#include "debug.h"

static size_t flen(FILE *fptr) {
    size_t size = 0;
    for (int c = 0; (c = fgetc(fptr)) != EOF; size++);
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

    // close the file
    fclose(fptr);

    // tokenize the string
    Stack tokens = Tokenize(str);

    // call the pre-processor
    PreProcess(&tokens);

    // print all of them
    for (Token *token = tokens.data; token->type != TOKEN_EOF; token++) {
        if (token->word && token->type != TOKEN_NWLINE) {
            printf("Token %s of type %d\n", token->word, token->type);
        }
    }

    // new line
    printf("\n");

    // parse the tokens using the steel syntax
    AST result = Parse(tokens.data);

    if (result->error != ERROR_NULL) {
        printf("error: ");
        Debug_PrintError(result->error);
        printf("\n");
    }

    // print the ast
    if (result) {
        Debug_PrintAST(result, 0);
    }
    else {
        printf("Empty ast\n");
    }

    /**
     * @note as soon as the tokens are freed the ast becomes invalid
     */

    // free all tokens TODO: create a function to do so.
    for (Token *token = tokens.data; token->type != TOKEN_EOF; token++) {
        freetoken(token);
    }

    // free the ast
    FreeAST(result);

    // free the tokens stack
    FreeStack(tokens);
}