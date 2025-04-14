all:
	flex lexer.l
	bison -d parser.y
	g++ -Wno-write-strings lex.yy.c parser.tab.c main.cpp stats.cpp optimizer.cpp -o query_processor -lm	
	./query_processor
	rm -f lex.yy.c parser.tab.c parser.tab.h query_processor
clean:
	rm -f lex.yy.c parser.tab.c parser.tab.h query_processor