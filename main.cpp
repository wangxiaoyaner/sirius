#include"global.h"
int main()
{
	freopen("ans","w",stdout);
	sourcefile=fopen("test","r");
	global_init();
	symbtable_new_level("main");
	parser_program();
	symbtable_display();
	global_quadruple_display();
	return 0;
}
