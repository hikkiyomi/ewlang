LEXER=lexer
PARSER=parser


run-lexer:
	flex $(LEXER).l

run-parser:
	bison -dy $(PARSER).y

build: run-parser run-lexer
	g++ --std=c++20 y.tab.c lex.yy.c definitions.h interpreter.cpp vm.cpp -o parser

run:
	./$(PARSER) input output

build-and-run: build run
