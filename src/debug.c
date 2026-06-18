#include "debug.h"

static void TreeIndent(size_t indent) {
    // renders indent - 1 bars to show previous branches going down
    for (size_t i = 1; i < indent; i++) {
        printf("│ ");
    }

    // if there were any node before indent > 0
    if (indent > 0) {
        printf("├─"); 
    }
}

void Debug_PrintAST(AST_Node *node, size_t tree_level) {
    if (node == NULL) return;

    printf("node ");
    Debug_PrintNodeType(node->type);
    if (node && node->tokens) {
        printf(" { ");
        for (size_t i = 0; i < node->num_tokens; i++) {
            printf("%s,", node->tokens[i].word);
        }
        printf(" }");
    }

    if (node->error != ERROR_NULL) {
        printf(" error: ");
        Debug_PrintError(node->error);
    }

    printf("\n");

    if (node && node->nextnodes) {
        for (size_t i = 0; i < node->numnodes; i++) {
            TreeIndent(tree_level + 1);
            Debug_PrintAST(node->nextnodes[i], tree_level + 1);
        }
    }
}

void Debug_PrintNodeType(NodeType type) {
    switch (type)
    {
        case NODE_NULL:
            printf("NULL (shound not happen)");
            break;

        case NODE_ERROR:
            printf("ERROR");
            break;

        case NODE_NUMBER:
            printf("NUMBER");
            break;

        case NODE_BINARY_OPERATION:
            printf("BINARY OPERATION");
            break;

        default:
            printf("UNKNOWN NODE (probably corrupted)");
            break;
    }
}

void Debug_PrintError(Error error) {
    switch (error)
    {
        case ERROR_NULL:
            printf("NULL ERROR (no problem)");
            break;

        case ERROR_LEXIC:
            printf("LEXICAL ERROR");
            break;

        case ERROR_SYNTAX:
            printf("SYNTAX ERROR");
            break;

        case ERROR_SYNTAX_PROPAGATING:
            printf("PROPAGATING SYNTAX ERROR (see down the line)");
            break;

        case ERROR_PREPROCESSING:
            printf("PREPROCESSING ERROR");
            break;

        case ERROR_ALLOCATION_FAILED:
            printf("ALLOCATION FAILED");
            break;

        case ERROR_UNEXPECTED_EOF:
            printf("UNEXPECTED EOF");
            break;

        case ERROR_MISSING_CLOSE_PARENTHESES:
            printf("MISSING CLOSE PARENTHESES");
            break;

        case ERROR_UNKNOWN_SUFFIX_OP:
            printf("UNKNOWN SUFFIX OPERATOR");
            break;
    
        default:
            printf("UNKNOWN ERROR (probably corrupted)");
            break;
    }
}