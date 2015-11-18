#include"global.h"
int main()
{
	sourcefile=fopen("test","r");
	lex_getch();
	lex_getsym();
//初始的时候就进入新的一层了
	symbtable_new_level("main");
	while(1)
	{
		if(lex_sym=="const")
		{
			lex_getsym();
			parser_constdeclaration();
		}
		else if(lex_sym=="var")
		{
			lex_getsym();
			parser_vardeclaration();
		}
		else if(lex_sym=="")
			break;
		else if(lex_sym=="procedure")
		{
			lex_getsym();
			parser_procedureheader();
		}
		else if(lex_sym=="function")
		{
			lex_getsym();
			parser_functionheader();
		}
		else
		{
			cout << "wrong";
			lex_getsym();
		}
			
	}
	symbtable_display();
	return 0;
}
