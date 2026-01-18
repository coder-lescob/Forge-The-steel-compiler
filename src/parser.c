#include "parser.h"

Syntax steelsyntax = {.numnodes = 0, .nodes = NULL, .numsymbols = 0, .symboltable = NULL};

// build steel syntax
void InitSteelSyntax(void) {
    // constants
    #define NUM_SYMBOLS 1
    #define NUM_NODES   3

    steelsyntax = (Syntax) {
        // allocates space
        .symboltable = calloc(NUM_SYMBOLS, sizeof(SyntaxNode *)),
        .numsymbols  = NUM_SYMBOLS,

        .nodes    = calloc(NUM_NODES, sizeof(SyntaxNode)),
        .numnodes = NUM_NODES
    };

    steelsyntax.nodes[0]       = (SyntaxNode) {.tokentype = TOKEN_ID, .numnext = 2, .nextNodes = calloc(2, sizeof(SyntaxNode *))};
    steelsyntax.nodes[0].nextNodes[0] = &steelsyntax.nodes[1];
    steelsyntax.nodes[0].nextNodes[1] = &steelsyntax.nodes[2];
    
    steelsyntax.nodes[1]       = (SyntaxNode) {.tokentype = TOKEN_ID, .numnext = 0, .nextNodes = NULL};
    steelsyntax.nodes[2]       = (SyntaxNode) {.tokentype = TOKEN_NUMBER, .numnext = 0, .nextNodes = NULL};
    steelsyntax.symboltable[0] = &steelsyntax.nodes[0];
}

void DestroySteelSyntax(void) {
    // free resources
    for (size_t i = 0; i < steelsyntax.numnodes; i++) {
        if (steelsyntax.nodes[i].nextNodes)
            free(steelsyntax.nodes[i].nextNodes);
    }

    free(steelsyntax.symboltable);
    free(steelsyntax.nodes);

    // make sizes 0
    steelsyntax.numnodes   = 0;
    steelsyntax.numsymbols = 0;
}

// allocates an AST_Node on the heap
AST_Node *AllocatesAST_Node(AST_Node node) {
    // allocates, don't forget to free it
    AST_Node *_node = malloc(sizeof(AST_Node));
    *_node = node;
    return _node;
}

static AST ParseNode(Token *token, SyntaxNode *syntaxnode) {
    if (token->type == TOKEN_EOF) return NULL;
    if (!syntaxnode) return NULL;
    if (token->type == syntaxnode->tokentype) {
        // allocate a node
        AST_Node *node = AllocatesAST_Node((AST_Node) {.token = token, .symbol = syntaxnode->symbol});

        // allocate place for next nodes
        node->nextnodes = calloc(syntaxnode->numnext, sizeof(AST_Node));
        node->numnodes = 0;

        // loop over each of the next nodes
        for (size_t i = 0; i < syntaxnode->numnext; i++) {
            AST branch = ParseNode(token + 1, syntaxnode->nextNodes[i]);
            if (branch) {
                node->nextnodes[node->numnodes++] = branch;
            }
        }

        if (node->numnodes == 0 && syntaxnode->numnext != 0) {
            // oops syntax error
            free(node);
            return NULL;
        }

        // node successful

        return node;
    }
    return NULL;
}

// parses a list of token using syntax
AST Parse(Token *tokens, Syntax *syntax) {
    // Start seach from token 0 syntax node 0
    AST ast = ParseNode(tokens, syntax->nodes);
    return ast;
}