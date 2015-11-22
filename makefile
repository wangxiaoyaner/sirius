compile:main.o lexer.o parser.o symbtable.o global.o
	g++ -o compile main.o lexer.o parser.o symbtable.o global.o
clean:rm compile *.o
