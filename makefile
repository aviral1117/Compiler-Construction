 # Group 1
#    Aviral Sethi : 2016B3A70532P
#    SANJEET MALHOTRA : 2016B4A70601P
#    PRIYANKA VERMA : 2016B3A70492P
#    ANIRUDH VIJAY : 2016B3A70525P

main: driver.o lexer.o parser.o ast.o scope.o semantics.o codeGen.o
	@gcc -o compiler lexer.o parser.o ast.o scope.o semantics.o codeGen.o driver.o
driver.o: driver.c codeGen.h
	@gcc -c driver.c
parser.o: parser.c parser.h parserDef.h
	@gcc -c parser.c
lexer.o: lexer.c lexer.h lexerDef.h
	@gcc -c lexer.c
ast.o: ast.c ast.h
	@gcc -c ast.c
scope.o: scope.c scope.h
	@gcc -c scope.c
semantics.o: semantics.c semantics.h semanticsDef.h
	@gcc -c semantics.c
codeGen.o: codeGen.c codeGen.h
	@gcc -c codeGen.c
clean:
	rm parser.o lexer.o driver.o ast.o scope.o semantics.o codeGen.o