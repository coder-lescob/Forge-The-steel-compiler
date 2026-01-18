#include "parser.h"

Syntax steelsyntax = {};

// build steel syntax
void InitSteelSyntax(void) {
    // constants
    #define NUM_SYMBOLS 1
    #define NUM_NODES   1

    steelsyntax = (Syntax) {
        // allocates space
        .symboltable = calloc(NUM_SYMBOLS, sizeof(SyntaxNode *)),
        .numsymbols  = NUM_SYMBOLS,

        .nodes    = calloc(NUM_NODES, sizeof(SyntaxNode)),
        .numnodes = NUM_NODES
    };

    steelsyntax.nodes[0]       = (SyntaxNode) {.tokentype = TOKEN_ID, .numnext = 0, .nextNodes = NULL};
    steelsyntax.symboltable[0] = &steelsyntax.nodes[0];
}

void DestroySteelSyntax(void) {
    // free resources
    free(steelsyntax.symboltable);
    free(steelsyntax.nodes);

    // make sizes 0
    steelsyntax.numnodes   = 0;
    steelsyntax.numsymbols = 0;
}

// allocates an AST_Node on the heap
AST_Node *AllocatesAST_Node(int symbol, Token *tokens, size_t numtokens, AST_Node *nextnodes, size_t numnodes) {
    // allocates, don't forget to free it
    AST_Node *node = malloc(sizeof(AST_Node));
    node->symbol    = symbol;
    node->tokens    = tokens;
    node->numtokens = numtokens;
    node->nextnodes = nextnodes;
    node->numnodes  = numnodes;
    return node;
}

// parses a list of token using syntax
AST Parse(Token *tokens, Syntax *syntax) {

}