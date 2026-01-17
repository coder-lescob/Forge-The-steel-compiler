#include "preproc.h"
#include "stack.h"

#include <string.h>

static PreProcStatmentType GetPreProcType(TokenType type) {
    switch (type)
    {
        case TOKEN_PREPROC_ALIAS: return PREPROC_ALIAS;

        default:
            printf("Illegal preprocessing statment.");
            break;
    }
    exit(-1);
}

void PreProccess(Token *tokens) {
    // Create a stack of preproc statments
    Stack statments = CreateStack(500, PreProcStatment);

    size_t numtokens = 0;
    for (size_t i = 0; tokens[i].type != TOKEN_EOF; i++, numtokens++);

    size_t rank = 0;
    for (size_t i = 0; i < numtokens && tokens[i].type != TOKEN_EOF; i++, rank++) {
        if (rank == 0 && tokens[i].type == TOKEN_DOLLAR) {

            // preprocessing statment found
            // count how many token is it to the next line
            size_t oldIdx = i, size;
            for (
                size = 1, i++ /* skip dollar */; 
                tokens[i].type != TOKEN_NWLINE && tokens[i].type != TOKEN_EOF; 
                i++, size++
            );

            // create the proproc statment
            PreProcStatment statment = {
                .type      = GetPreProcType(tokens[oldIdx + 1 /* skip the preproc token */ ].type), 
                .tokens    = calloc(size, sizeof(Token)),
                .numtokens = size
            };

            // copy tokens
            for (size_t j = 0; j < statment.numtokens; j++) {
                statment.tokens[j] = tokens[oldIdx + j];
            }

            // erase tokens
            for (size_t j = oldIdx, k = i; tokens[j].type != TOKEN_EOF; j++, k++) {
                tokens[j] = tokens[k];
            }

            // push it
            Push(statments, statment, PreProcStatment);
        }

        if (tokens[i].type == TOKEN_NWLINE) rank = 0;
    }

    for (size_t i = 0; i < statments.ptr; i++) {
        PreProcStatment s = ((PreProcStatment *)statments.data)[i];

        // free every ressources
        printf("Statment of type %d with %ld tokens (", s.type, s.numtokens);

        for (size_t j = 0; j < s.numtokens; j++) {
            printf("%s ", s.tokens[j].word);
            free(s.tokens[j].word);
        }
        printf("\b) freed\n");

        free(s.tokens);
    }

    FreeStack(statments);
}