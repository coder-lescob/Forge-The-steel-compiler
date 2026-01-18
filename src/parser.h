#ifndef HEADER_PARSER
#define HEADER_PARSER

// std libs
#include <stdlib.h>

// project
#include "token.h"

// a node of syntax
typedef struct SyntaxNode {
    // symbole or token
    TokenType  tokentype;
    size_t     symbol;

    // The array of next nodes
    SyntaxNode *nextNodes;
    size_t     numnext;
} SyntaxNode;

// represent a syntax
typedef struct Syntax {
    // a symbole table
    SyntaxNode **symboltable;
    size_t     numsymbols;

    // and nodes
    SyntaxNode *nodes;
    size_t     numnodes;
} Syntax;

// an Abstract Syntax Tree node
typedef struct AST_Node {
    int      symbol;
    Token    *tokens;
    size_t   numtokens;
    AST_Node *nextnodes;
    size_t   numnodes;
} AST_Node;

// The Abstract Syntax Tree representation in code
typedef AST_Node *AST;

/*
* allocates an AST_Node on the heap don't forget to free it.
* @returns an pointer to an AST_Node.
*/
AST_Node *AllocatesAST_Node(int symbol, Token *tokens, size_t numtokens, AST_Node *nextnodes, size_t numnodes);

// the steel syntax
extern Syntax steelsyntax;

/* Initialize the steel syntax */
void InitSteelSyntax(void);

/* Destroys the steel syntax */
void DestroySteelSyntax(void);

/*
* Parses a list of token finishing by the end of file token using the syntax provided.
* @param tokens The list of token ending in a token of type TOKEN_EOF
* @param syntax The syntax to use during parsing.
* @returns an Abstract Syntax Tree (AST).
*/
AST Parse(Token *tokens, Syntax *syntax);

#endif