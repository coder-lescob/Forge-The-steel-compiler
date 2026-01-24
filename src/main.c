// std libs
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// project
#include "stack.h"
#include "token.h"
#include "lexer.h"
#include "preproc.h"
#include "parser.h"

static size_t flen(FILE *fptr) {
    size_t size = 0;
    for (char c = 0; (c = fgetc(fptr)) != EOF; size++);
    fseek(fptr, 0, SEEK_SET);

    return size + 1;
}

static void Indent(size_t indent, int last) {
    // renders indent - 1 bars to show previous branches going down
    for (size_t i = 1; i < indent; i++) printf("│ ");

    // if there were any node before indent > 0
    if (indent) {
        // last one should be visally different
        if (last) printf("└─");
        
        // Than a random node not at the end
        else      printf("├─"); 
    }
}

static void PrintNode(AST_Node *node, size_t indent, int last) {
    Indent(indent, last);
    if (node && node->token && node->token->word)
        printf("node %s of type 0x%lx\n", node->token->word, node->symbol);
    else 
        printf("invalid node\n");

    if (node && node->nextnodes) {
        for (size_t i = 0; i < node->numnodes; i++) {
            PrintNode(node->nextnodes[i], indent + 1, i == node->numnodes - 1);
        }
    }
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
    Stack tokens = Tokenize(str);

    // call the pre-processor
    PreProccess(&tokens);

    // print all of them
    for (Token *token = tokens.data; token->type != TOKEN_EOF; token++) {
        if (token->word && token->type != TOKEN_NWLINE) {
            printf("Token %s of type %d\n", token->word, token->type);
        }
    }

    InitSteelSyntax();

    AST ast = Parse(tokens.data, &steelsyntax);

    DestroySteelSyntax();

    if (ast)
        PrintNode(ast, 0, 1);
    else
        printf("Empty ast\n");

    for (Token *token = tokens.data; token->type != TOKEN_EOF; token++) {
        if (token->word != NULL) {
            free(token->word);
            token->word = NULL;
        }
    }

    // free the ast
    FreeAST(ast);

    FreeStack(tokens);
    fclose(fptr);
}