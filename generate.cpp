#include"global.h"
static map<string,symbItem*> tmpregpool;//eax,ecx,edx;
static map<int,symbItem*> globalregpool;
static FILE *x86codes=fopen("x86codes.asm","w");
static void clear_tmpregpool();
static void init_pool()
{
	clear_tmpregpool();
	globalregpool[1]=NULL;
	globalregpool[2]=NULL;
	globalregpool[3]=NULL;
}
static void clear_tmpregpool()
{
	tmpregpool["eax"]=NULL;
	tmpregpool["ecx"]=NULL;
	tmpregpool["edx"]=NULL;
}
static void realloc_globalreg(int num,symbItem* newitem)
{
	if(globalregpool[num]==newitem)
		return;
	globalregpool[num]=newitem;	
}
/*
 +,src1,src2,ans
 -,src1,src2,ans
 *,src1,src2,ans
 /,src1,src2,ans 
 Assign src1,null,ans
Neg,src1,null,null
larray src1,src2,ans
Sarray src1,src2,ans
Inc src1,null,null
Dec src1,null,null
Jl src1,src2,ans
Jle src1,src2,ans
Jg src1,src2,ans
Jge src1,src2,ans
Jne src1,src2,ans
Je src1,src2,ans
Fpara src1,null,null
Rpara src1,nul,null
 */
//每次处理一个函数,之后改成每次处理一个基本块，这样在常量池上的处理久一样了。
static void generate_basic(quadfunc *nowfunc)//
{
	init_pool();
	quadruple *nowquad=nowfunc->firstcode;
	while(nowquad!=NULL)
	{
		if(nowquad->opr=="lab")
		{
			fprintf(x86codes,"%s:\n",nowquad->src1->name.data());
		}
		else if(nowquad->opr=="call")
		{
			int callerlev=nowfunc->table->level,calleelev=nowquad->src1->level+1;//函数代码所在层次=定义层次+1
			if(callerlev<=calleelev)
			{
				for(int i=(calleelev-1)*4;i>=0;)
				{
					fprintf(x86codes,"push dword [ebp+%d]\n",i+20);
					i-=4;
				}
			}
			else
			{
				cout << "IMPOSSIBLE" << endl;
			}
			fprintf(x86codes,"push esi\npush edi\npush ebx\ncall _%s%d\npop ebx\npop edi\npop esi\nadd esp,%d\n",
					nowquad->src1->name.data(),nowquad->src1->level+1,calleelev*4);

		}
		else if(nowquad->opr=="ret")
		{
			if(nowfunc->table->level)
			{
				symbItem *tmp=tmpregpool["eax"];//在eax里还留着的只会是本层的局部变量。
				if(tmp!=NULL)
					fprintf(x86codes,"mov [ebp-%d],eax\n",tmp->adr);
				fprintf(x86codes,"mov eax,[ebp-4]\nmov esp,ebp\npop ebp\nret\n");
			}
			else
				fprintf(x86codes,"mov esp,ebp\nret\n");
		}
		else if(nowquad->opr=="func")
		{
			if(nowquad->src1->name=="main")
			{
				fprintf(x86codes,"_main0:\nmov esp,ebp\nsub esp,%d\n",nowfunc->table->localsnum*4+4);
			}
			else
			{
				fprintf(x86codes,"_%s%d:\npush ebp\nmov ebp,esp\nsub esp,%d\n"
						,nowquad->src1->name.data(),nowquad->src1->level,nowfunc->table->localsnum*4+4);
			}
		}
		else if(nowquad->opr=="jmp")
		{
			fprintf(x86codes,"jmp %s\n",nowquad->src1->name.data());
		}
		nowquad=nowquad->link;
	}
}
void generate_main()
{
	fputs("global main\nsection .data\nsection .text\n",x86codes);
	quadfunc *nowfunc=quadruple_codes;
	while(nowfunc)
	{
		generate_basic(nowfunc);
		nowfunc=nowfunc->link;
	}
}



