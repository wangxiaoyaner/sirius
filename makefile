compile:main.o lexer.o parser.o symbtable.o global.o optimization.o generate.o
	g++ -o compile main.o lexer.o parser.o symbtable.o global.o optimization.o generate.o
clean:rm compile *.o
