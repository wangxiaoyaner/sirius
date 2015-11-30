#include"global.h"
static int level;
static map<string,symbItem*> tmpregpool;//eax,ecx,edx;
static map<int,symbItem*> globalregpool;
static map<int ,string> adr_reg;
static FILE *x86codes=fopen("x86codes.asm","w");
static void clear_tmpregpool();
static void init_pool()
{
	adr_reg[1]="ebx";
	adr_reg[2]="edi";
	adr_reg[3]="esi";
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
static void handle_src1(symbItem *src1,string &num1)//将操作数的值所在的寄存器放到num1里面。
{
	num1="eax";
	if(src1->kind=="constpool")
	{
		fprintf(x86codes,"mov eax,%s\n",src1->name.data());
	}
	else if(src1->level==level)
	{
		if(src1->kind=="parameter")
		{
			if(src1->type=="integer")
			{
				if(src1->para_ifvar)
					fprintf(x86codes,"mov eax,[ebp+%d]\nmov eax,[eax]\n",16+4*(level+src1->adr));
				else
					fprintf(x86codes,"mov eax,[ebp+%d]\n",16+4*(level+src1->adr));
			}
			else
			{
				if(src1->para_ifvar)
					fprintf(x86codes,"mov eax,[ebp+%d]\nmovzx eax,byte [eax]\n",16+4*(level+src1->adr));
				else
					fprintf(x86codes,"movzx eax,byte [ebp+%d]\n",16+4*(level+src1->adr));
			}
		}
		else if(src1->adr%4)
		{
			num1=adr_reg[src1->adr];
		}
		else
		{
			if(src1->type=="integer")
				fprintf(x86codes,"mov eax,[ebp-%d]\n",-src1->adr);
			else 
				fprintf(x86codes,"movzx eax,byte [ebp-%d]\n",-src1->adr);
		}
	}
	else
	{
		if(src1->kind=="parameter")//不是这一层的参数
		{
			if(src1->type=="integer"){
				if(src1->para_ifvar)
				{
					fprintf(x86codes,"mov eax,[ebp+%d]\nadd eax,%d\nmov eax,[eax]\nmov eax,[eax]\n",20+src1->level*4,16+(src1->level+src1->adr)*4);
				}
				else
				{
					fprintf(x86codes,"mov eax,[ebp+%d]\nadd eax,%d\nmov eax,[eax]\n",20+src1->level*4,16+(src1->level+src1->adr)*4);
				}
			}
			else
			{
				if(src1->para_ifvar)
				{
					fprintf(x86codes,"mov eax,[ebp+%d]\nadd eax,%d\nmov eax,[eax]\nmovzx eax,byte [eax]\n",20+src1->level*4,16+(src1->level+src1->adr)*4);
				}
				else
				{
					fprintf(x86codes,"mov eax,[ebp+%d]\nadd eax,%d\nmovzx eax,byte [eax]\n",20+src1->level*4,16+(src1->level+src1->adr)*4);
				}

			}
		}
		else if(src1->adr%4)
		{//找到所在层的调用层的ebp，
			if(src1->type=="integer")
			{
				if(src1->level+1==level)//仅差一层
				{
					fprintf(x86codes,"mov eax,[ebp+%d]\n",4+4*src1->adr);
				}
				else
				{
					fprintf(x86codes,"mov eax,[ebp+%d]\nadd eax,%d\nmov eax,[eax]\n",20+(src1->level+1)*4,4+src1->adr*4);
				}
			}
			else 
			{
				if(src1->level+1==level)//仅差一层
				{
					fprintf(x86codes,"movzx eax,byte [ebp+%d]\n",4+4*src1->adr);
				}
				else
				{
					fprintf(x86codes,"mov eax,[ebp+%d]\nadd eax,%d\nmovzx eax,byte [eax]\n",20+(src1->level+1)*4,4+src1->adr*4);
				}
			}
		}
		else
		{
			if(src1->type=="integer")
				fprintf(x86codes,"mov eax,[ebp+%d]\nsub eax,%d\nmov eax,[eax]\n",20+src1->level*4,-src1->adr);
			else
				fprintf(x86codes,"mov eax,[ebp+%d]\nsub eax,%d\nmovzx eax,byte[eax]\n",20+src1->level*4,-src1->adr);
		}
	}	
}
static void handle_src2(symbItem *src2,string &num2)
{
	if(src2->kind=="constpool")
	{
		num2=src2->name;
	}
	else if(src2->level==level)
	{
		if(src2->kind=="parameter")
		{
			if(src2->para_ifvar)
			{
				fprintf(x86codes,"mov ecx,[ebp+%d]\n",16+4*(level+src2->adr));
				if(src2->type=="integer")
					num2="[ecx]";
				else
				{
					fprintf(x86codes,"movzx ecx byte[ecx]\n");
					num2="ecx";
				}
			}
			else
			{
				if(src2->type=="integer")
					num2="[ebp+"+numtostring(16+4*(level+src2->adr))+"]";
				else
				{
					fprintf(x86codes,"movzx ecx,byte [ebp+%d]\n",(16+4*(level+src2->adr)));
					num2="ecx";
				}
			}
		}
		else if(src2->adr%4)
		{
			num2=adr_reg[src2->adr];
		}
		else
		{
			if(src2->type=="integer")
			{
				string tmp=src2->adr>0?"+":"";
				tmp+=numtostring(src2->adr);
				num2="[ebp"+tmp+"]";
			}
			else
			{
				fprintf(x86codes,"movzx ecx,byte [ebp-%d]\n",-src2->adr);
				num2="ecx";
			}
		}
	}
	else
	{
		if(src2->kind=="parameter")//不是这一层的参数
		{
			num2="[ecx]";
			if(src2->para_ifvar)
			{
				if(src2->type=="integer")
					fprintf(x86codes,"mov ecx,[ebp+%d]\nadd ecx,%d\nmov ecx,[ecx]\n",20+src2->level*4,16+(src2->level+src2->adr)*4);
				else
				{
					fprintf(x86codes,"mov ecx,[ebp+%d]\nadd ecx,%d\nmov ecx,[ecx]\nmovzx ecx,byte [ecx]\n",20+src2->level*4,16+(src2->level+src2->adr)*4);
					num2="ecx";
				}
			}
			else
			{
				if(src2->type=="integer")
					fprintf(x86codes,"mov ecx,[ebp+%d]\nadd ecx,%d\n",20+src2->level*4,16+(src2->level+src2->adr)*4);
				else
				{
					num2="ecx";
					fprintf(x86codes,"mov ecx,[ebp+%d]\nadd ecx,%d\nmovzx ecx,byte [ecx]\n",20+src2->level*4,16+(src2->level+src2->adr)*4);

				}
			}
		}
		else if(src2->adr%4)
		{//找到所在层的调用层的ebp，
			if(src2->level+1==level)//仅差一层
			{
				num2="[ebp+"+numtostring(4+4*src2->adr)+"]";
				if(src2->type=="char")
				{
					fprintf(x86codes,"movzx ecx,byte%s\n",num2.data());
					num2="ecx";
				}
			}
			else
			{
				fprintf(x86codes,"mov ecx,[ebp+%d]\nadd ecx,%d\n",20+(src2->level+1)*4,4+src2->adr*4);
				num2="[ecx]";
				if(src2->type=="char")
				{
					num2="ecx";
					fprintf(x86codes,"movzx ecx,byte [ecx]\n");
				}
			}
		}
		else
		{
			num2="[ecx]";
			string tmp=src2->adr>0?"add":"sub";
			int tmpn=src2->adr>0?src2->adr:-src2->adr;
			fprintf(x86codes,"mov ecx,[ebp+%d]\n%s ecx,%d\n",20+src2->level*4,tmp.data(),tmpn);
			if(src2->type=="char")
			{
				fprintf(x86codes,"movzx ecx,byte [ecx]\n");
				num2="ecx";
			}
		}
	}
}
static  void handle_ans(symbItem *ans,string num1)
{
	if(ans->level==level)
	{
		if(ans->kind=="parameter")
		{
			if(ans->para_ifvar)
				fprintf(x86codes,"mov edx,[ebp+%d]\nmov [eax],%s\n",16+4*(level+ans->adr),num1.data());
			else
				fprintf(x86codes,"mov [ebp+%d],%s\n",16+4*(level+ans->adr),num1.data());
		}
		else if(ans->adr%4)
		{
			if(adr_reg[ans->adr]!=num1)
				fprintf(x86codes,"mov %s,%s\n",adr_reg[ans->adr].data(),num1.data());
		}
		else
		{
			string tmp=ans->adr>0?"+":"";
			tmp+=numtostring(ans->adr);
			fprintf(x86codes,"mov [ebp%s],%s\n",tmp.data(),num1.data());
		}
	}
	else
	{
		if(ans->kind=="parameter")//不是这一层的参数
		{
			if(ans->para_ifvar)
			{
				fprintf(x86codes,"mov edx,[ebp+%d]\nadd edx,%d\nmov edx,[edx]\nmov [edx],%s\n",20+ans->level*4,16+(ans->level+ans->adr)*4,num1.data());
			}
			else
			{
				fprintf(x86codes,"mov edx,[ebp+%d]\nadd edx,%d\nmov [edx],%s\n",20+ans->level*4,16+(ans->level+ans->adr)*4,num1.data());
			}
		}
		else if(ans->adr%4)
		{//找到所在层的调用层的ebp，
			if(ans->level+1==level)//仅差一层
			{
				fprintf(x86codes,"mov [ebp+%d],%s\n",4+4*ans->adr,num1.data());
			}
			else
			{
				fprintf(x86codes,"mov edx,[ebp+%d]\nadd edx,%d\nmov [edx],%s\n",20+(ans->level+1)*4,4+ans->adr*4,num1.data());
			}
		}
		else
		{	
			string tmp=ans->adr>0?"add":"sub";
			int tmpn=ans->adr>0?ans->adr:-ans->adr;
			fprintf(x86codes,"mov edx,[ebp+%d]\n%s edx,%d\nmov [edx],%s\n",20+ans->level*4,tmp.data(),tmpn,num1.data());
		}
	}	
}
//每次处理一个函数,之后改成每次处理一个基本块，这样在常量池上的处理久一样了。
static void generate_basic(quadfunc *nowfunc)//
{
	init_pool();
	quadruple *nowquad=nowfunc->firstcode;
	level=nowfunc->table->level;
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
			/*	symbItem *tmp=tmpregpool["eax"];//在eax里还留着的只会是本层的局部变量。
				if(tmp!=NULL)
					fprintf(x86codes,"mov [ebp-%d],eax\n",tmp->adr);*/
				fprintf(x86codes,"mov eax,[ebp-4]\nmov esp,ebp\npop ebp\nret\n");
			}
			else
				fprintf(x86codes,"mov esp,ebp\nret\n");
		}
		else if(nowquad->opr=="func")
		{
			if(nowquad->src1->name=="main")
			{
				fprintf(x86codes,"_main0:\nmov ebp,esp\nsub esp,%d\n",nowfunc->table->localsnum*4+4);
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
		else if(nowquad->opr=="add"||nowquad->opr=="sub"||nowquad->opr=="imul")
		{//只需要一个寄存器就好。
			string num1,num2;//结果将放到寄存器num1里面。
			handle_src1(nowquad->src1,num1);
			handle_src2(nowquad->src2,num2);
			fprintf(x86codes,"%s %s,%s\n",nowquad->opr.data(),num1.data(),num2.data());
			handle_ans(nowquad->ans,num1);
		}else if(nowquad->opr=="idiv")
		{
			string num1,num2="ecx";
			handle_src1(nowquad->src1,num1);
			if(num1!="eax")
			{
				fprintf(x86codes,"mov eax,%s\n",num1.data());
			}
			fprintf(x86codes,"cdq\n");
			if(nowquad->src2->kind!="constpool")
				handle_src2(nowquad->src2,num2);
			else
				fprintf(x86codes,"mov ecx,%d\n",nowquad->src2->value);
			fprintf(x86codes,"%s %s\n",nowquad->opr.data(),num2.data());
			handle_ans(nowquad->ans,num1);
		}
		else if(nowquad->opr=="je"||nowquad->opr=="jl"||nowquad->opr=="jle"||
				nowquad->opr=="jg"||nowquad->opr=="jne"||nowquad->opr=="jge")
		{
			string num1,num2;
			handle_src1(nowquad->src1,num1);
			handle_src2(nowquad->src2,num2);
			fprintf(x86codes,"cmp %s,%s\n%s %s\n",num1.data(),num2.data(),nowquad->opr.data(),nowquad->ans->name.data());
		}
		else if(nowquad->opr=="assign")
		{
			string num1;
			if(nowquad->src1->kind=="function")
			{
				handle_ans(nowquad->ans,"eax");
			}
			else if(nowquad->ans->kind=="function")
			{
				handle_src1(nowquad->src1,num1);
				if(nowquad->ans->level==level-1)
				{
					fprintf(x86codes,"mov [ebp-4],%s\n",num1.data());
				}
				else
				{
					fprintf(x86codes,"mov ecx,[ebp+%d]\nsub ecx,4\nmov [ecx],%s\n",20+4*(nowquad->ans->level+1),num1.data());
				}
			}
			else{
				handle_src1(nowquad->src1,num1);
				handle_ans(nowquad->ans,num1);
			}
		}
		else if(nowquad->opr=="neg"||nowquad->opr=="inc"||nowquad->opr=="dec")
		{
			string num2;
			handle_src2(nowquad->src1,num2);//取地址或者全局寄存器
			fprintf(x86codes,"%s %s\n",nowquad->opr.data(),num2.data());
		}
		else if(nowquad->opr=="rpara")//rpara增加一个参数(笑脸)
		{
			string num2;
			if(nowquad->src1->adr%4&&nowquad->src1->level==level)
			{	
				int addd=4*(4-nowquad->src1->adr+nowquad->src2->size+nowquad->src2->level+1);//怎么把全局寄存器的地址传过去。。还是当前层的
				fprintf(x86codes,"mov  ecx,esp\nsub ecx,%d\npush ecx\n",addd);
			}else{
				handle_src2(nowquad->src1,num2);
				fprintf(x86codes,"push %s\n",num2.data());
			}
		}
		else if(nowquad->opr=="fpara")
		{
			string num1;
			handle_src1(nowquad->src1,num1);
			fprintf(x86codes,"push %s\n",num1.data());
		}
		else if(nowquad->opr=="larray")//larray src1 src2 ans : ans=src1[src2]
		{//地址肯定小于0
			string num1,ans="edx";
			handle_src1(nowquad->src2,num1);
			if(nowquad->src1->level==level)
			{
				fprintf(x86codes,"mov edx,ebp\nsub edx,%d\nimul %s,4\nsub edx,%s\n"
						,-nowquad->src1->adr,num1.data(),num1.data(),ans.data());
			}
			else
			{
				fprintf(x86codes,"mov edx,[ebp+%d]\n,sub edx,%d\nimul %s,4\nsub edx,%s\n",
					20+nowquad->src1->level*4,-nowquad->src1->adr,num1.data(),num1.data(),ans.data());	
			}
			if(nowquad->src1->type=="integer")
				fprintf(x86codes,"mov edx,[edx]\n");
			else
				fprintf(x86codes,"movzx edx,byte [edx]\n");

			handle_ans(nowquad->ans,ans);
		}
		else if(nowquad->opr=="sarray")
		{
			string num1;
			handle_src1(nowquad->src2,num1);
			if(nowquad->src1->level==level)
			{
				fprintf(x86codes,"mov edx,ebp\nsub edx,%d\nimul %s,4\nsub edx,%s\n"
						,-nowquad->src1->adr,num1.data(),num1.data());
			}
			else
			{
				fprintf(x86codes,"mov edx,[ebp+%d]\n,sub edx,%d\nimul %s,4\nsub edx,%s\n",
					20+nowquad->src1->level*4,-nowquad->src1->adr,num1.data(),num1.data());	
			}
			handle_src1(nowquad->ans,num1);
			fprintf(x86codes,"mov [edx],%s\n",num1.data());
		}
		else if(nowquad->opr=="writes")
		{
			fprintf(x86codes,"push str%d\ncall printf\nadd esp,4\n",nowquad->src1->value);
		}
		else if(nowquad->opr=="writee")//tmp var的类型们不一定是int
		{
			string num1;
			handle_src1(nowquad->src1,num1);		
			if(nowquad->src1->type=="integer")
			{
				fprintf(x86codes,"push %s\npush strint\ncall printf\nadd esp,8\n",num1.data());
			}
			else
			{
				fprintf(x86codes,"push %s\npush strchar\ncall printf\nadd esp,8\n",num1.data());
			}
		}
		else if(nowquad->opr=="read")
		{
			if(nowquad->src1->type=="integer")
				fprintf(x86codes,"sub esp,4\npush esp\n push strint\ncall scanf\nadd esp,8\nmov eax,[esp]\n");
			else
				fprintf(x86codes,"sub esp,4\npush esp\n push strchar\ncall scanf\nadd esp,8\nmovzx eax,byte[esp]\n");
				handle_ans(nowquad->src1,"eax");
				fprintf(x86codes,"add esp,4\n");
		}
		else
		{
			cout << "IMPOSSIBLE!"<<endl;
		}
		nowquad=nowquad->link;
	}
}
void generate_main()
{
	fputs("global main\nextern printf\nextern scanf\nsection .data\nstrint:db'\%d',0\nstrchar: db'%c',0\n",x86codes);
	for(int i=1;i<my_writes_num;i++)
	{
		fprintf(x86codes,"str%d: db \"%s\",10,0\n",i,my_write_string.front().data());
		my_write_string.pop();
	}
	fprintf(x86codes,"section .text\n");
	quadfunc *nowfunc=quadruple_codes;
	while(nowfunc)
	{
		generate_basic(nowfunc);
		nowfunc=nowfunc->link;
	}
}



