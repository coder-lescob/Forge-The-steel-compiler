#ifndef DEBUG_H
#define DEBUG_H

#include "error.h"
#include "parser.h"

/**
 * Prints a ParsingResult.
 * @param result the result to print
 */
void Debug_PrintAST(AST_Node *result, size_t tree_level);

/**
 * prints a nodetype.
 * @param type the nodetype
 */
void Debug_PrintNodeType(NodeType type);

/**
 * prints an error.
 * @param error the error
 */
void Debug_PrintError(Error error);

#endif