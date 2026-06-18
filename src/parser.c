#include "parser.h"

#include <string.h>
#include <stdbool.h>

// allocates an AST_Node on the heap
AST_Node *CreateAST_Node(NodeType type) {
    // allocates, don't forget to free it
    AST_Node *node = malloc(sizeof(AST_Node));
    if (node == NULL) return NULL;
    
    node->error      = ERROR_NULL;
    node->type       = type;
    node->num_tokens = 0;
    node->tokens     = NULL;
    node->numnodes   = 0;
    node->nextnodes  = NULL;

    return node;
}

AST_Node *CreateErrorAST_Node(Error error, NodeType type) {
    AST_Node *node = CreateAST_Node(type);
    if (node == NULL) return NULL;

    // set erropr state
    node->error = error;

    return node;
}

void FreeAST(AST ast) {
    if (!ast) return /* was a leaf node */ ;

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

static Token *next_token(Token **previous_token) {
    Token *next_token = *previous_token;

    if (next_token->type == TOKEN_EOF) {
        // ouf parsing done
        return NULL;
    }
    
    (*previous_token)++;

    return next_token;
}

void PushNode(AST_Node *result, AST_Node *node) {
    if (result->error != ERROR_NULL || result == NULL || node == NULL) {
        return;
    }

    // reallocate the node
    AST_Node **new_nodes = realloc(result->nextnodes, sizeof(AST_Node *) * (result->numnodes + 1));
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
    result->nextnodes = new_nodes;
    result->numnodes++;

    // put the next node at the end
    /**
     * @note the node MUST be stored on the heap to avoid data corruption
     */
    result->nextnodes[result->numnodes - 1] = node;
}

void PushToken(AST_Node *result, Token **tokens, TokenType *allowed_types) {
    if (result == NULL || result->error != ERROR_NULL) {
        return;
    }

    Token *token = next_token(tokens);

    if (token == NULL) {
        result->error = ERROR_UNEXPECTED_EOF;
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

    Token *new_tokens = realloc(result->tokens, sizeof(Token) * (result->num_tokens + 1));
    if (new_tokens == NULL) {
        // allocation failed
        result->error = ERROR_ALLOCATION_FAILED;
        return;
    }

    result->tokens = new_tokens;
    result->num_tokens++;
    memcpy(&result->tokens[result->num_tokens - 1], token, sizeof(Token));
}

#define CHECK_NODE_ERROR(NODE) \
    if ((NODE)->error != ERROR_NULL) return (NODE);

AST_Node *TryParse(AST_Node *(*F)(Token **), Token **tokens) {
    // try to parse with F
    Token *error_tokens = *tokens;
    AST_Node *result = F(&error_tokens);

    // If there is no error then update the token pointer
    if (result->error == ERROR_NULL) {
        *tokens = error_tokens;
    }

    return result;
}

// parses a list of token
AST_Node *Parse(Token *tokens) {
    return ParseExpression(&tokens, 0.0f);
}

static TokenType binary_operators[] = {
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    0,
};

AST_Node *ParseNumber(Token **token) {
    // for now let just say we want a number token
    AST_Node *result = CreateAST_Node(NODE_NUMBER);

    TokenType allowed_types[] = {TOKEN_NUMBER, 0,};
    PushToken(result, token, allowed_types);

    return result;
}

// implemented as derivation of pratt parsing
AST_Node *ParseExpression(Token **token, float min_binding_power) {

    // parse the first number of the expression
    AST_Node *lhs = TryParse(ParseNumber, token);

    if (lhs->error != ERROR_NULL) {
        // then lhs is not a number
        // so we don't need it
        FreeAST(lhs);
        lhs = NULL;

        Token *tok = next_token(token);
        if (tok == NULL) {
            return CreateErrorAST_Node(ERROR_UNEXPECTED_EOF, NODE_ERROR);
        }

        if (tok->type == TOKEN_OPEN_PARENTHESES) {
            lhs = ParseExpression(token, 0.0);

            if (lhs->error != ERROR_NULL) {
                return lhs;
            }

            Token *close_parentheses = next_token(token);
            if (close_parentheses == NULL || close_parentheses->type != TOKEN_CLOSE_PARENTHESES) {
                // Oh, oh missing close parentheses
                // free the old result
                FreeAST(lhs);
                lhs = NULL;

                lhs = CreateErrorAST_Node(ERROR_MISSING_CLOSE_PARENTHESES, NODE_ERROR);
                
                TokenType all[] = {0};
                PushToken(lhs, token, all);

                return lhs;
            }
        }
        else {
            // Oh, oh unknown suffix operator
            // free the old result
            FreeAST(lhs);
            lhs = NULL;

            lhs = CreateErrorAST_Node(ERROR_EXPECTED_NUMBER_ID, NODE_ERROR);
            
            TokenType all[] = {0};
            PushToken(lhs, token, all);

            return lhs;
        }
    }

    while (*token != NULL && (*token)->type != TOKEN_EOF) {
        // peek the operator token
        Token *op = *token;

        if (op == NULL || op->type == TOKEN_EOF || op->type == TOKEN_CLOSE_PARENTHESES) {
            break;
        }

        // get the binding power of the operator
        BindingPower binding_power = GetBindingPower(op);

        if (binding_power.lhs < min_binding_power) {
            break;
        }
        
        // push the operator in an operaton
        AST_Node *operation = CreateAST_Node(NODE_BINARY_OPERATION);
        PushToken(operation, token, binary_operators);
        
        AST_Node *rhs = ParseExpression(token, binding_power.rhs);
        PushNode(operation, lhs);
        PushNode(operation, rhs);

        lhs = operation;
        CHECK_NODE_ERROR(lhs);
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
            return (BindingPower) { 99.0f, 99.0f };
    }
}