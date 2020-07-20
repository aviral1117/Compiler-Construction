# Compiler-Construction
This is a toy-compiler for a strongly-typed language ERPLAG, the specifications of which are included above (Language-Specifications).

# Includes the following Modules:
- Lexer: Generates appropriate tokens as per the language specifications which are fed to the parser.
- Parser: Parses the input source code to produce the parse tree.
- AST: Generates the abstract syntax tree using the parse tree.
- Scope and Semantics: Ascertains that all the scope and semantic rules are satisfied.
- CodeGen: Produces the final asm file if no errors found in the earlier modules

# System Requirements:
- nasm x64-bit assembler
- GCC 5.4.0 or above

# To generate asm file:
- make
- ./compiler source_code.txt output.asm

# To execute the program:
- nasm -f elf64 output.asm
- gcc output.o
- ./a.out
