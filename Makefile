LEXER=lexer
PARSER=parser
BINARY=ewlang


run-lexer:
	flex $(LEXER).l

run-parser:
	bison -dy $(PARSER).y

build: run-parser run-lexer
	g++ -O3 --std=c++20 \
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
		importer.cpp \
		-o $(BINARY)

run:
	./$(BINARY) input.ew output

build-and-run: build run

disassemble:
	g++ -S -o $(TARGET).s $(TARGET).cpp
	as -o $(TARGET).o $(TARGET).s
	objdump -d $(TARGET).o > $(TARGET).dump
	nm $(TARGET).o > $(TARGET).mangling
	rm $(TARGET).s $(TARGET).o

decode:
	c++filt $(TARGET)
