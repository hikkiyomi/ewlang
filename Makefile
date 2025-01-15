LEXER=lexer
PARSER=parser
BINARY=ewlang


run-lexer:
	flex $(LEXER).l

run-parser:
	bison -dy $(PARSER).y

build: run-parser run-lexer
	g++ -O3 --std=c++20 -Wall -Wextra -pedantic \
		y.tab.c \
		lex.yy.c \
		definitions.h \
		vm_definitions.h \
		interpreter.cpp \
		vm.cpp \
		nodes.h \
		nodes.cpp \
		bigint.h \
		bigint.cpp \
		-o $(BINARY)

run:
	./$(BINARY) input output

build-and-run: build run
