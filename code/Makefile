all:
	flex lexer.l
	bison -d parser.y
	gcc lex.yy.c parser.tab.c main.c -o query_processor
	./query_processor
	rm -f lex.yy.c parser.tab.c parser.tab.h query_processor
clean:
	rm -f lex.yy.c parser.tab.c parser.tab.h query_processor