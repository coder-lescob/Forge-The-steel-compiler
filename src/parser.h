#ifndef HEADER_PARSER
#define HEADER_PARSER

// std libs
#include <stdlib.h>

// project
#include "error.h"
#include "stack.h"
#include "token.h"

typedef enum NodeType {
    NODE_NULL,
    NODE_NUMBER,
    NODE_BINARY_OPERATION,
    NODE_MISSING_CLOSE_PARENTHESES,
    NODE_UNKNOWN_SUFFIX_OP,
} NodeType;

/** 
 * an Abstract Syntax Tree node 
 */
typedef struct AST_Node {
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
 * if the error is     ERROR_NULL then there is no error
 * if the error is not ERROR_NULL then there is an error
 */
typedef struct ParsingResult {
    Error error;
    AST ast;
} ParsingResult;

/**
 * a binding power
 */
typedef struct BindingPower {
    float lhs, rhs;
} BindingPower;

/**
 * allocates an AST_Node on the heap don't forget to free it.
 * @returns an pointer to an AST_Node.
 * 
 * @note all pointer fields are initialized to NULL
 */
AST_Node *CreateAST_Node(NodeType type);

/**
 * Frees the ast given.
 * @param ast The ast.
 */
void FreeAST(AST ast);

/**
 * create a parsing result of a given type
 */
ParsingResult CreateParsingResult(NodeType type);

/**
 * Pushes the node at the end of result
 * @param result the destination for the node
 * @param node   the node to push 
 * 
 * @note if result contains an error nothing is done
 * @note node MUST be allocated on the heap to avoid data corruption
 */
void PushNode(ParsingResult *result, ParsingResult *node);

/**
 * Pushes the tokens at the end of the result
 * @param result the destination node for the token
 * @param token  the token to push
 * @param allowed_types a null terminated array of allowed types
 */
void PushToken(ParsingResult *result, Token **tokens, TokenType *allowed_types);

/**
 * Try to parse using F if it is not successful the error is returned
 * but no tokens are consumed.
 * @param F the parsing function
 * @param tokens the pointer to the token stream
 * @return the result of F if F was successful else 
 */
ParsingResult TryParse(ParsingResult (*F)(Token **), Token **tokens);

/**
 * Parses a list of token finishing by the end of file token
 * @param tokens The list of token ending in a token of type TOKEN_EOF
 * @return an Abstract Syntax Tree (AST).
 */
ParsingResult Parse(Token *tokens);

/**
 * parses a number from a list of given tokens
 * @param token a pointer to the next token of the stream
 */
ParsingResult ParseNumber(Token **token);

/**
 * parses an expression from the token stream
 * @param token a pointer to the next token of the stream
 */
ParsingResult ParseExpression(Token **token, float min_binding_power);

/**
 * get the binding power of op
 * @param op the token operator
 * 
 * @note if op isn't an operator (0, 0) is returned
 */
BindingPower GetBindingPower(Token *op);

#endif