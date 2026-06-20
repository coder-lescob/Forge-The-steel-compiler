#ifndef HEADER_PARSER
#define HEADER_PARSER

// std libs
#include <stdlib.h>
#include <stdbool.h>

// project
#include "error.h"
#include "stack.h"
#include "token.h"

typedef enum {
    NODE_NULL,
    NODE_ERROR,
    NODE_NUMBER,
    NODE_BINARY_OPERATION,
} NodeType;

/** 
 * an Abstract Syntax Tree node
 * if the error is     ERROR_NULL then there is no error
 * if the error is not ERROR_NULL then there is an error
 */
typedef struct AST_Node {
    // the error that may be in this node
    Error error;

    // the type of the current node
    NodeType type;

    // The tokens of the current node
    size_t num_tokens;
    Token  *tokens;

    // The next nodes of this node
    struct AST_Node **nextnodes;
    size_t numnodes;
} AST_Node;

// The Abstract Syntax Tree representation in code
typedef AST_Node *AST;

/**
 * a binding power
 */
typedef struct {
    float lhs, rhs;
} BinaryBindingPower;

/**
 * allocates an AST_Node on the heap don't forget to free it.
 * @param type the node type
 * @return a pointer to an AST_Node.
 * 
 * @note all pointer fields are initialized to NULL
 */
AST_Node *CreateAST_Node(NodeType type);

/**
 * allocates an AST_Node on the heap with error `error`
 * @param type the node type
 * @param error the error
 * @return a pointer to an AST_Node
 * 
 * @note must be freed
 * @note all pointers fields are initialized to NULL
 */
AST_Node *CreateErrorAST_Node(Error error, NodeType type);

/**
 * frees the node with FreeAST and then create a new node
 * with the new error and new_type
 * @param node the node
 * @param new_error the new error
 * @param new_type the new node type
 */
void RecycleAST_Node(AST_Node **node, Error new_error, NodeType new_type);

/**
 * Frees the ast given.
 * @param ast The ast.
 */
void FreeAST(AST ast);

/**
 * Pushes the node at the end of result
 * @param result the destination for the node
 * @param node   the node to push 
 * 
 * @note if result contains an error nothing is done
 * @note node MUST be allocated on the heap to avoid data corruption
 */
void PushNode(AST_Node *result, AST_Node *node);

/**
 * Pushes the tokens at the end of the result
 * @param result the destination node for the token
 * @param token  the token to push
 * @param allowed_types a null terminated array of allowed types
 */
void PushToken(AST_Node *result, Token **tokens, TokenType *allowed_types);

/**
 * Try to parse using F if it is not successful the error is returned
 * but no tokens are consumed.
 * @param F the parsing function
 * @param tokens the pointer to the token stream
 * @return the result of F if F was successful else 
 */
AST_Node *TryParse(AST_Node *(*F)(Token **), Token **tokens);

/**
 * Parses a list of token finishing by the end of file token
 * @param tokens The list of token ending in a token of type TOKEN_EOF
 * @return an Abstract Syntax Tree (AST).
 */
AST_Node *Parse(Token *tokens);

/**
 * parses a number from a list of given tokens
 * @param token a pointer to the next token of the stream
 */
AST_Node *ParseNumber(Token **token);

/**
 * parses an expression from the token stream
 * @param token a pointer to the next token of the stream
 */
AST_Node *ParseExpression(Token **token, float min_binding_power);

/**
 * returns true if and only if op is an operator. e.g. "+", "-"...
 */
bool IsOperator(Token *op);

/**
 * get the binary binding power (act on two operands) of op
 * @param op the token operator
 * 
 * @note if op isn't an operator (0, 0) is returned
 */
BinaryBindingPower GetBinaryBindingPower(Token *op);

#endif