#include"global.h"
int main()
{
	sourcefile=fopen("test","r");
	freopen("1","w",stdout);
	lex_getch();
	lex_getsym();
	int level=0;
	while(1)
	{
		if(lex_sym=="const")
		{
			lex_getsym();
			parser_constdeclaration(level);		
		}
		else if(lex_sym=="var")
		{
			lex_getsym();
			parser_vardeclaration(level);
		}
		else if(lex_sym=="")
			break;
		else if(lex_sym=="procedure")
		{
			lex_getsym();
			parser_procedureheader(level);
		}
		else if(lex_sym=="function")
		{
			lex_getsym();
			parser_functionheader(level);
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
