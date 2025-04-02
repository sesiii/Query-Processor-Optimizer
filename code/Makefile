CC = gcc
FLEX = flex
BISON = bison
CFLAGS = -Wall -g

all: sql_parser

sql_parser: lexer.o parser.tab.o ast.o ra_tree.o main.o
	$(CC) $(CFLAGS) -o $@ $^

lexer.o: lexer.c
	$(CC) $(CFLAGS) -c $

lexer.: lexer.l parser.tab.h
	$(FLEX) -o $@ $

parser.tab.o: parser.tab.c
	$(CC) $(CFLAGS) -c $

parser.tab.c parser.tab.h: parser.y
	$(BISON) -d $

ast.o: ast.c ast.h
	$(CC) $(CFLAGS) -c $

ra_tree.o: ra_tree.c ra_tree.h
	$(CC) $(CFLAGS) -c $

main.o: main.c
	$(CC) $(CFLAGS) -c $

clean:
	rm -f sql_parser *.o lexer.c parser.tab.c parser.tab.h

.PHONY: all clean