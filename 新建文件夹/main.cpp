#include"global.h"
int main()
{
	freopen("ans","w",stdout);
	if((sourcefile=fopen("test","r"))==NULL)
	{
			cout << "file open error\n";
	}	
	global_init();
	symbtable_new_level("main");
	parser_program();
	cout << "hello,world"<<endl;
	symbtable_display();

	optimazation_adr_alloc();
	generate_main();
	
	symbtable_display();
	global_quadruple_display();
	return 0;
}
