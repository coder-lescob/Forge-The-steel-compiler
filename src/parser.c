#include "parser.h"

#include <string.h>
#include <stdbool.h>

// allocates an AST_Node on the heap
AST_Node *CreateAST_Node(NodeType type) {
    // allocates, don't forget to free it
    AST_Node *node = malloc(sizeof(AST_Node));
    if (!node) return NULL;
    
    node->type       = type;
    node->num_tokens = 0;
    node->tokens     = NULL;
    node->numnodes   = 0;
    node->nextnodes  = NULL;

    return node;
}

void FreeAST(AST ast) {
    if (!ast) return /* was last node */ ;

    // free all ast ressources
    for (size_t i = 0; i < ast->numnodes; i++) {
        FreeAST(ast->nextnodes[i]);
    }
    free(ast->nextnodes);

    // free all the tokens
    if (ast->tokens != NULL) {
        free(ast->tokens);
    }

    // finally free the ast itself
    free(ast);
}

ParsingResult CreateParsingResult(NodeType type) {
    return (ParsingResult) {
        .error = ERROR_NULL,
        .ast   = CreateAST_Node(type),
    };
}

static Token *next_token(Token **previous_token) {
    Token *next_token = (*previous_token)++;

    if (next_token->type == TOKEN_EOF) {
        // ouf parsing done
        return NULL;
    }

    return next_token;
}

void PushNode(ParsingResult *result, ParsingResult *node) {
    if (result->error != ERROR_NULL || result == NULL || node == NULL) {
        return;
    }

    // reallocate the node
    AST_Node **new_nodes = realloc(result->ast->nextnodes, sizeof(AST_Node *) * (result->ast->numnodes + 1));
    if (new_nodes == NULL) {
        // allocation failed
        // set the error state to ALLOCATION_FAILED
        result->error = ERROR_ALLOCATION_FAILED;
        // we don't care about what was stored in the ast, 
        // it might be useful later for debugging
        return;
    }

    // propagate any error
    // so that it is O(1) to check for having an error
    if (node->error != ERROR_NULL) {
        result->error = ERROR_SYNTAX_PROPAGATING;
    }

    // allocation sucessful
    // replace the old nodes with the new ones
    result->ast->nextnodes = new_nodes;
    result->ast->numnodes++;

    // put the next node at the end
    /**
     * @note the node MUST be stored on the heap to avoid data corruption
     */
    result->ast->nextnodes[result->ast->numnodes - 1] = node->ast;
}

void PushToken(ParsingResult *result, Token **tokens, TokenType *allowed_types) {
    if (result == NULL || result->error != ERROR_NULL) {
        return;
    }

    Token *token = next_token(tokens);

    if (token == NULL) {
        return;
    }

    if (allowed_types != NULL) {
        // verify that the token is allowed
        bool valid = false;
        for (TokenType *type = allowed_types; (*type) != 0; type++) {
            if (token->type == *type) {
                valid = true;
                break;
            }
        }

        if (!valid) {
            // error
            result->error = ERROR_SYNTAX;
        }
    }

    Token *new_tokens = realloc(result->ast->tokens, sizeof(Token) * (result->ast->num_tokens + 1));
    if (new_tokens == NULL) {
        // allocation failed
        result->error = ERROR_ALLOCATION_FAILED;
        return;
    }

    result->ast->tokens = new_tokens;
    result->ast->num_tokens++;
    memcpy(&result->ast->tokens[result->ast->num_tokens - 1], token, sizeof(Token));
}

// parses a list of token
ParsingResult Parse(Token *tokens) {
    return ParseExpression(&tokens, 0.0f);
}

static TokenType binary_operators[] = {
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    0,
};

ParsingResult ParseNumber(Token **token) {
    // for now let just say we want a number token
    ParsingResult result = CreateParsingResult(NODE_NUMBER);
    TokenType allowed_types[] = {
        TOKEN_NUMBER,
        0,
    };
    PushToken(&result, token, allowed_types);

    return result;
}

// implemented as derivation of pratt parsing
ParsingResult ParseExpression(Token **token, float min_binding_power) {

    // parse the first number of the expression
    ParsingResult lhs = ParseNumber(token);

    while ((**token).type != TOKEN_EOF) {
        // peek the operator token
        Token *op = *token;

        // get the binding power of the operator
        BindingPower binding_power = GetBindingPower(op);

        if (binding_power.lhs < min_binding_power) {
            break;
        }
        
        // push the operator in an operaton
        ParsingResult operation = CreateParsingResult(NODE_BINARY_OPERATION);
        PushToken(&operation, token, binary_operators);
        
        ParsingResult rhs = ParseExpression(token, binding_power.rhs);
        PushNode(&operation, &lhs);
        PushNode(&operation, &rhs);

        lhs = operation;
    }

    return lhs;
}

BindingPower GetBindingPower(Token *op) {
    switch (op->type) {
        case TOKEN_PLUS: 
        case TOKEN_MINUS: 
            return (BindingPower) { 1.0f, 1.1f };

        case TOKEN_STAR:
        case TOKEN_SLASH:
            return (BindingPower) { 2.0f, 2.1f };

        default: 
            return (BindingPower) { 0.0f, 0.0f };
    }
}