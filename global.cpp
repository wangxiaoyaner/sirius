#include"global.h"
string global_err_message[]={"",//0
	"Illegal char declaration",//1
	"Missing single quotes",//2
	"Illegal string declaration",//3
	"Illegal character",//4
	"Duplicate identifier",//5
	"\'=\'should be in the back of identifier",//6
	"\"=\"expected but \":=\" found",//7
	"const should be integer or char",//8
	"digit should be in the back of \"+\"or\"-\"",//9
	"constdeclaration should be started with identifier",//10
	"\";\" expected",//11
	"\"[\" expected",//12
	"\"]\" expected",//13
	"small int expected",//14
	"\"of\" expected",//15
	"type expected",//16
	"identifier expected",//17
	"\":\" expected",//18
	"\"(\" expected",//19
	"\")\" expected"//20

};

int global_lex_line_num=0;


int global_error_num=1;

int lasterrorline=-1;
void global_error(int err_no,string ident)
{	
	if(lasterrorline!=global_lex_line_num)
		cout << "error "<< global_error_num++ <<" : line[" << global_lex_line_num << "]   " << global_err_message[err_no] <<" "<<ident<<"\n";
	lasterrorline=global_lex_line_num;
}
	
