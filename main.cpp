#include"global.h"
int main()
{
	sourcefile=fopen("test","r");
	freopen("1","w",stdout);
	lex_getch();
	while(lex_getsym())
	{

		if(lex_sym=="ident")
			cout << global_lex_line_num <<": " << lex_token <<'\t'<<"ident\n";
		else if(lex_sym=="digit")
			cout << global_lex_line_num << ": " << lex_value <<'\t'<<"digit\n";
		else if(lex_sym=="char")
			cout << global_lex_line_num << ": " << lex_token <<"\tchar\n";
		else if(lex_sym=="string")
			cout << global_lex_line_num << ": " << lex_token << "\tstring\n";
		else
			cout << global_lex_line_num << ": " << lex_sym << "\n";
	}
	return 0;
}
