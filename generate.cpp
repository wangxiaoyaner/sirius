#include"global.h"
static map<string,symbItem*> tmpregpool;//eax,ecx,edx;
static map<int,symbItem*> globalregpool;
static FILE *x86codes=fopen("x86codes.asm","w");
static void initpool()
{
	tmpregpool["eax"]=NULL;
	tmpregpool["ecx"]=NULL;
	tmpregpool["edx"]=NULL;
	globalregpool[1]=NULL;
	globalregpool[2]=NULL;
	globalregpool[3]=NULL;
}
static void realloc_globalreg(int num,symbItem* newitem)
{
	if(globalregpool[num]==newitem)
		return;
	globalregpool[num]=newitem;	
}

void generate_main()
{
	fputs("global main\nsection .data\nsection .text\n",x86codes);
}



