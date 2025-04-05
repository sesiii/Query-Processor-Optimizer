all:
	flex lexer.l
	bison -d parser.y
	gcc lex.yy.c parser.tab.c main.c stats.c optimizer.c -o query_processor
	./query_processor
clean:
	rm -f lex.yy.c parser.tab.c parser.tab.h query_processor