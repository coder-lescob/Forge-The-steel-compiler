from enum import Enum

class Token_Type(Enum):
    TOKEN_ILLEGAL = 0
    TOKEN_BLANK   = 1
    TOKEN_ROOT    = 2

    # identifier
    TOKEN_ID      = 3
    
    # symboles
    TOKEN_DOLLAR  = 4
    TOKEN_EQUAL   = 5
    TOKEN_NWLINE  = 6
    TOKEN_DOT     = 7
    TOKEN_OPEN_CURLY_BRACES  = 18
    TOKEN_CLOSE_CURLY_BRACES = 19

    # math
    TOKEN_PLUS    = 8
    TOKEN_MINUS   = 9
    TOKEN_STAR    = 10
    TOKEN_SLASH   = 11
    

    # litteral
    TOKEN_NUMBER  = 12

    # preproc
    TOKEN_CONST   = 13

    # comments
    TOKEN_OPEN_COMMENT  = 14
    TOKEN_CLOSE_COMMENT = 15
    TOKEN_COMMENT       = 16

    TOKEN_EOF           = 17

class Syntax_Type(Enum):
    MAIN = 0,
    FRAME = 1
    NONE = ~0 # all 1s

class Syntax_Node:
    def __init__(self, token_type: Token_Type,  next_nodes: list[int], symbol: Syntax_Type, syntax: Syntax_Type = Syntax_Type.NONE):
        self.token      = token_type
        self.next_nodes = next_nodes
        self.syntax     = syntax
        self.symbol     = symbol

    def to_c(self, idx):
        # allocates the node
        print(
            f"steelsyntax.nodes[{idx}] = (SyntaxNode) "
            f"{"{"}.tokentype = {self.token.name}, "
            f".symbol = {self.symbol.value if isinstance(self.symbol.value, int) else self.symbol.value[0]}, "
            f"{f".syntax = {self.syntax.value}" if self.syntax != Syntax_Type.NONE else ""}, "
            f".numnext = {len(self.next_nodes)}, "
            f".nextNodes = {f"calloc({len(self.next_nodes)}, sizeof(SyntaxNode *))" if len(self.next_nodes) > 0 else "NULL"}"
            f"{"}"};"
        )

        # prints all next nodes
        for i, next_idx in enumerate(self.next_nodes):
            print(f"steelsyntax.nodes[{idx}].nextNodes[{i}] = &steelsyntax.nodes[{next_idx}];")

        print()

class Syntax:
    def __init__(self, nodes: list[Syntax_Node], symbol_table: list[int]):
        self.nodes        = nodes
        self.symbol_table = symbol_table

    def to_c(self):
        # prints the allocator
        print(
            "// syntax creation code (generated using gen_syntax.py) \n"
            "// constants\n"
            f"#define NUM_SYMBOLS {len(self.symbol_table)}\n"
            f"#define NUM_NODES   {len(self.nodes)}\n"
            "\n"
            "steelsyntax = (Syntax) {\n"
            "    // allocates space\n"
            "    .symboltable = calloc(NUM_SYMBOLS, sizeof(SyntaxNode *)),\n"
            "    .numsymbols  = NUM_SYMBOLS,\n"
            "\n"
            "    .nodes    = calloc(NUM_NODES, sizeof(SyntaxNode)),\n"
            "    .numnodes = NUM_NODES\n"
            "};\n"
        )

        # prints each node
        for i, node in enumerate(self.nodes):
            node.to_c(i)

        # print symbol table
        for i, sym in enumerate(self.symbol_table):
            print(f"steelsyntax.symboltable[{i}] = &steelsyntax.nodes[{sym}];")

def main():
    # syntax yet
    nodes = [
        Syntax_Node(Token_Type.TOKEN_ILLEGAL, next_nodes=[], symbol=Syntax_Type.MAIN, syntax=Syntax_Type.FRAME),
        Syntax_Node(Token_Type.TOKEN_OPEN_CURLY_BRACES, next_nodes=[2], symbol=Syntax_Type.FRAME),
        Syntax_Node(Token_Type.TOKEN_CLOSE_CURLY_BRACES, next_nodes=[], symbol=Syntax_Type.FRAME)
    ]

    symbol_table = [
        0, 1
    ]

    steel_syntax = Syntax(nodes, symbol_table)

    steel_syntax.to_c()

if __name__ == '__main__':
    main()