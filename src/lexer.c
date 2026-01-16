// std libs
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// project
#include "token.h"
#include "lexer.h"
#include "stack.h"

// The size of the buffer
#define BUFFER_SIZE 64

#define AddLetter(str, letter) \
    str[strlen(str)] = letter; \
    str[strlen(str)] = 0

static void ClassifyCurrentAndNext(char *buffer, char letter, TokenType *current, TokenType *next) {

    // Get the type of the current token
    *current = ClassifyToken(buffer);
    
    // avoid buffer overflows
    if (strlen(buffer) >= BUFFER_SIZE) {

        // next should be illegal
        *next = TOKEN_ILLEGAL;
        return;
    }

    // add the letter to the end of the buffer
    AddLetter(buffer, letter);

    // classify it
    *next = ClassifyToken(buffer);

    // remove the letter from the buffer
    buffer[strlen(buffer) - 1] = 0;
}

static void AddToken(Stack *tokens, char *word, TokenType type) {

    // check if the type is illegal
    if (type == TOKEN_ILLEGAL) {
        // lexical error
        printf("Lexical error %s\n", word);
    }

    // Blank tokens are ignored
    else if (type != TOKEN_BLANK) {

        // create the token
        Token token = {.type = type};

        if (word) {
            // copy the string in the token
            token.word = calloc(strlen(word) + 1 /* + 0 terminator */, sizeof(char));
            memcpy(token.word, word, strlen(word));
        }

        // push it
        Push((*tokens), token, Token);
    }

    // clear the buffer if there is any
    if (word) {
        memset(word, 0, BUFFER_SIZE);
    }
}

Token *Tokenize(char *str) {

    // creates a stack of 500 tokens
    Stack tokens = CreateStack(500, Token);

    // The buffer to hold the tempory token
    char buffer[BUFFER_SIZE + 1] = ""; // + 0 terminator

    // the type of the current and next tempory token
    TokenType currentType, nextType;

    for (size_t i = 0; i < strlen(str); i++) {
        ClassifyCurrentAndNext(buffer, str[i], &currentType, &nextType);

        // if adding letter makes the token illegal then...
        if (nextType == TOKEN_ILLEGAL) {

            // we got the largest token possible
            AddToken(&tokens, buffer, currentType);
        }

        // prevent buffer overflows
        if (strlen(buffer) >= BUFFER_SIZE) {

            // exit eurly
            printf("Too long token %s at %ld\n", buffer, i);
            exit(-1);
        }

        // add the letter to the end of the buffer
        AddLetter(buffer, str[i]);
    }

    // get the next type
    nextType = ClassifyToken(buffer);

    // in case a token is left on buffer
    if (strlen(buffer) > 0 && nextType != TOKEN_ILLEGAL && nextType != TOKEN_BLANK) {
        AddToken(&tokens, buffer, currentType);
    }

    // add the end of file
    AddToken(&tokens, NULL, TOKEN_EOF);

    // returns all tokens
    return (Token *)tokens.data;
}