#include "parser.h"
#include <string.h>

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
    if (!_node) return NULL;
    *_node = node;
    return _node;
}

static AST_Node *DescentAST(AST ast, size_t symbol) {
    // loop over each next node of the current node
    if (ast && ast->nextnodes)
    for (size_t i = 0; i < ast->numnodes; i++) {
        // get the node
        AST_Node *node = ast->nextnodes[i];
        if (!node) continue;

        // if end node
        if (node->numnodes == 0) {
            return node;
        }
        else {
            // see further
            DescentAST(node, symbol);
        }
    }
    return ast;
}

static void PushNode(AST ast, SyntaxNode *node, Token *token) {
    // descent the ast
    AST_Node *astnode = DescentAST(ast, node->symbol);

    // create the current node
    AST_Node *current = AllocatesAST_Node(
        (AST_Node){
            .symbol    = node->symbol,
            .token     = token,
            .nextnodes = NULL,
            .numnodes  = 0
        }
    );

    if (astnode->numnodes && astnode->nextnodes) {
        // allocates more place
        AST_Node **oldptr = astnode->nextnodes;
        astnode->nextnodes = calloc(astnode->numnodes + 1, sizeof(AST_Node *));
        memcpy(astnode->nextnodes, oldptr, astnode->numnodes);
        free(oldptr);

        // link the node to the ast
        astnode->nextnodes[astnode->numnodes++] = current;
    }
    else {
        // link node with others
        astnode->nextnodes    = malloc(sizeof(AST_Node *));
        astnode->nextnodes[0] = current;
        astnode->numnodes     = 1;
    }
}

static int nodecmp(SyntaxNode a, SyntaxNode b) {
    return a.nextNodes == b.nextNodes && a.tokentype == b.tokentype && a.symbol == b.symbol && a.syntax == b.syntax && a.numnext == b.numnext;
}

static size_t GetNodeIdx(Syntax *syntax, SyntaxNode node) {
    for (size_t i = 0; i < syntax->numnodes; i++) {
        if (nodecmp(syntax->nodes[i], node)) {
            return i;
        }
    }
    return -1;
}

// parses a list of token using syntax
AST Parse(Token *tokens, Syntax *syntax) {
    // Start seach from token 0 syntax node 0

    typedef struct returninfo {
        size_t node, tokenptr;
    } returninfo;

    // should be enough depth
    Stack returnStack = CreateStack(500, returninfo);
    size_t nodeIdx = 0ul, tokenptr = 0ul;

    // create an ast
    AST ast = AllocatesAST_Node((AST_Node){.symbol = 0});

    while (nodeIdx < syntax->numnodes) {
        // get the node and the token
        SyntaxNode *node = &syntax->nodes[nodeIdx];
        Token *token     = &tokens    [tokenptr++];

        // is it last token ?
        if (token->type == TOKEN_EOF) {
            // no tokens left will syntax still wants tokens, syntax error
            return NULL;
        }

        // If token node
        else if (node->tokentype != TOKEN_ILLEGAL) {
            // If token met
            if (node->tokentype == token->type) {
                // push the node
                PushNode(ast, node, token);
            }
            else {
                // verify before
                goto ret;
            }
        }
        else {
            // syntax node
            returninfo info = (returninfo){.node = nodeIdx, .tokenptr = tokenptr - 1};
            Push(returnStack, info, returninfo);
            nodeIdx = GetNodeIdx(syntax, *syntax->symboltable[node->syntax]);
            continue; // don't inc nodeIdx
        }

        // If last node
        if (node->numnext == 0) {
ret:
            // if no return val possible
            if (returnStack.ptr == 0) {
                // If no token left
                if (tokens[tokenptr].type == TOKEN_EOF) {
                    // end of parsing
                    break;
                }
                
                // tokens left
                return NULL;
            }
            else {
                // get the last return address and put it in the current node
                returninfo info = PopLast(returnStack, returninfo);
                nodeIdx         = info.node;
                tokenptr        = info.tokenptr; // next token
                continue;
            }
        }

        if (node->nextNodes) {
            Token *tok = &tokens[tokenptr];
            size_t i;
            for (i = 0; i < node->numnext; i++) {
                SyntaxNode *n = node->nextNodes[i];
                if (tok->type == n->tokentype) {
                    nodeIdx = GetNodeIdx(syntax, *n);
                    break;
                }
            }
            if (i == node->numnext) {
                return NULL;
            }
        }
    }

    return ast;
}