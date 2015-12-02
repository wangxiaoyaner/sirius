#include"global.h"
int main()
{
	//freopen("ans","w",stdout);
	if((sourcefile=fopen("test","r"))==NULL)
	{
			cout << "file open error\n";
	}
	global_init();
	symbtable_new_level("main");
	parser_program();
	if(!global_error_num)
	{
		optimazation_adr_alloc();
		symbtable_display();
		global_quadruple_display();
		generate_main();
		return 0;
	}
}
