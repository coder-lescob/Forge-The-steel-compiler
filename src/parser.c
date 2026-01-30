#include "parser.h"
#include <string.h>

Syntax steelsyntax = {.numnodes = 0, .nodes = NULL, .numsymbols = 0, .symboltable = NULL};

// build steel syntax
void InitSteelSyntax(void) {
    // syntax creation code (generated using gen_syntax.py) 
    // constants
    #define NUM_SYMBOLS 3
    #define NUM_NODES   5

    steelsyntax = (Syntax) {
        // allocates space
        .symboltable = calloc(NUM_SYMBOLS, sizeof(SyntaxNode *)),
        .numsymbols  = NUM_SYMBOLS,

        .nodes    = calloc(NUM_NODES, sizeof(SyntaxNode)),
        .numnodes = NUM_NODES
    };

    steelsyntax.nodes[0] = (SyntaxNode) {.tokentype = TOKEN_ILLEGAL, .symbol = 0, .syntax = 1, .numnext = 0, .nextNodes = NULL};

    steelsyntax.nodes[1] = (SyntaxNode) {.tokentype = TOKEN_OPEN_CURLY_BRACES, .symbol = 1, .numnext = 1, .nextNodes = calloc(1, sizeof(SyntaxNode *))};
    steelsyntax.nodes[1].nextNodes[0] = &steelsyntax.nodes[2];

    steelsyntax.nodes[2] = (SyntaxNode) {.tokentype = TOKEN_ILLEGAL, .symbol = 1, .syntax = 2, .numnext = 2, .nextNodes = calloc(2, sizeof(SyntaxNode *))};
    steelsyntax.nodes[2].nextNodes[0] = &steelsyntax.nodes[2];
    steelsyntax.nodes[2].nextNodes[1] = &steelsyntax.nodes[3];

    steelsyntax.nodes[3] = (SyntaxNode) {.tokentype = TOKEN_CLOSE_CURLY_BRACES, .symbol = 1, .numnext = 0, .nextNodes = NULL};

    steelsyntax.nodes[4] = (SyntaxNode) {.tokentype = TOKEN_NUMBER, .symbol = 2, .numnext = 0, .nextNodes = NULL};

    steelsyntax.symboltable[0] = &steelsyntax.nodes[0];
    steelsyntax.symboltable[1] = &steelsyntax.nodes[1];
    steelsyntax.symboltable[2] = &steelsyntax.nodes[4];
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
            return DescentAST(node, symbol);
        }
    }
    return ast;
}

void FreeAST(AST ast) {
    if (!ast) return /* was last node */ ;

    // free all ast ressources
    for (size_t i = 0; i < ast->numnodes; i++) {
        FreeAST(ast->nextnodes[i]);
    }
    free(ast->nextnodes);

    // if there is still a word free it
    if (ast->token) {
        if (ast->token->word) {
            free(ast->token->word);
        }

        if ((ast->flag & AST_FLAG_FREE) > 0) {
            free(ast->token);
        }
    }

    // finally free the ast itself
    free(ast);
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
        astnode->nextnodes = calloc(astnode->numnodes + 1 /* not perfect but might do the trick */ , sizeof(AST_Node *));
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

static SyntaxNode *GetNextNode(SyntaxNode *node, Token *token) {
    // no tokens left
    if (token->type == TOKEN_EOF) return NULL;

    // use default node for specialized feature of the parser
    SyntaxNode *defaultNode = NULL;
    for (size_t i = 0; i < node->numnext; i++) {
        SyntaxNode *n = node->nextNodes[i];
        if (token->type == n->tokentype) {
            return n;
        }
        else if (n->tokentype == TOKEN_ILLEGAL || n->tokentype == TOKEN_BLANK) {
            defaultNode = n;
        }
    }
    return defaultNode;
}

static Token *GetNextToken(Token *tokens, size_t *tokenptr) {
    Token *tok = &tokens[*tokenptr];
    if (tok->type == TOKEN_NWLINE) {
        // skip all new lines
        for (tok = &tokens[++(*tokenptr)]; tok->type == TOKEN_NWLINE && tok->type != TOKEN_EOF; tok++, (*tokenptr)++);

        if (tok->type == TOKEN_EOF || tok->type == TOKEN_NWLINE) {
            return NULL;
        }
    }
    return tok;
}

// parses a list of token using syntax
AST Parse(Token *tokens, Syntax *syntax) {

    // a structure to hold return info when returning to previous node
    typedef struct returninfo {
        SyntaxNode *node;
        size_t tokenptr;
    } returninfo;

    // should be enough depth
    Stack returnStack = CreateStack(500, returninfo);

    // Start parsing from token 0 syntax node 0 of symbol 0.
    size_t tokenptr = 0ul;
    SyntaxNode *node = syntax->symboltable[0];

    // create an ast
    Token *root = calloc(1, sizeof(Token));
    *root = (Token) {.type = TOKEN_ROOT, .word = calloc(100, sizeof(char))};
    memcpy(root->word, "root", sizeof("root"));
    AST ast = AllocatesAST_Node((AST_Node){.symbol = ~0 /* all ones */, .token = root, .flag = AST_FLAG_FREE});

    while (node /* while node valid */) {
        // get the token
        Token *token = &tokens[tokenptr++];

        // if the current token is a new line continue
        if (token->type == TOKEN_NWLINE) continue /* skip token */ ;

        // debug print
        // printf("Token %s %d    node %d\n", token->word, token->type, node->tokentype);

        // is it last token ?
        if (token->type == TOKEN_EOF) {
            // no tokens left, syntax still wants tokens, syntax error
            goto syntaxerror;
        }

        // If token node
        else if (node->tokentype != TOKEN_ILLEGAL) {
            // a blank node means, I needed another node but not another token
            if (node->tokentype == TOKEN_BLANK) {
                tokenptr--;
                goto next;
            }

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

            // get the next valid token
            Token *tok;
            size_t tokidx = tokenptr;
            if (!(tok = GetNextToken(tokens, &tokidx))) {
                goto syntaxerror;
            }

            // create and push return infos
            returninfo info = (returninfo){.node = GetNextNode(node, tok), .tokenptr = tokenptr--};
            Push(returnStack, info, returninfo);
            
            // start over from the node in syntax
            node = syntax->symboltable[node->syntax];
            continue; // don't go to next node
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
                goto syntaxerror;
            }
            else {
                // get the last return address and put it in the current node
                returninfo info = PopLast(returnStack, returninfo);
                node            = info.node;
                tokenptr        = info.tokenptr;
                continue;
            }
        }

next:
        if (node->nextNodes) {
            // Get the next token
            Token *tok;
            if (!(tok = GetNextToken(tokens, &tokenptr))) {
                goto syntaxerror;
            }

            // set the next node
            node = GetNextNode(node, tok);
        }

        // If next node not valid
        if (!node) {
            // dead end
            // so which decison was bad ?
            goto syntaxerror;
        }
    }

    return ast;

syntaxerror:
    FreeAST(ast);
    return NULL;
}