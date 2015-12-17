#include"global.h"
int main()
{
	char filename[100];
	printf("Enter filename:\n");
	scanf("%s",filename);
	if((sourcefile=fopen(filename,"r"))==NULL)
	{
			cout << "file open error\n";
	}
	else{
		global_init();
		symbtable_new_level("main");
		cout << "befor parser"<<endl;
		parser_program();
		cout << "end parser" << endl;
		if(!global_error_num)
		{
			optimazation_adr_alloc();
			symbtable_display();
			global_quadruple_display();
			generate_main();
			return 0;
		}
	}
}
