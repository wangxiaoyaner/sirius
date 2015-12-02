#include"global.h"
queue<string> my_write_string;
int my_writes_num=1;
static int parser_procedure();
static int parser_compoundstatement();
static int parser_constdefinition();
static int parser_constdeclaration();
static int parser_vardefinition();
static int parser_vardeclaration();
static int parser_functiondeclaration();
static int parser_proceduredeclaration();
static int parser_formalparalist(int &para_size);
static int parser_formalparasection(int &para_size);
static int parser_expression();
static int parser_factor();
static int parser_term(int &if_low_zero);
static int parser_statement(symbItem *forbid);
static int parser_condition(symbItem **src1,symbItem **src2,string &oprname);
static int parser_realparameterlist(symbItem *func_proc);
static int if_array=0;
static int tmplablecode=1;
static stack<symbItem*> operand_stack;
static int tmpvarcode=1;//为产生新的中间变量进行编号。//_num
static int parser_ifintestset(string a[],int num,string target)//检查target是否在num个元素的a里
{
	for(int i=0;i<num;i++)
		if(a[i]==target)
			return 1;
	return 0;
}
//停止符号集仅仅管到结尾着一个，先不考虑明显的关键词
static void  parser_test(string a[],int num)//寻找合法后继或者终止符号集合
{
	while(!parser_ifintestset(a,num,lex_sym))
	{
		lex_getsym();
	}
}
static void parser_test(string a[],int num,int &flag)
{
	flag=0;
	while(!parser_ifintestset(a,num,lex_sym))
	{
		lex_getsym();
		if(!flag)
		{
			global_error("Illegal parser in procedure");
			flag=1;
		}
	}
}
string numtostring(int num)
{
	stringstream tmp;
	string res;
	tmp << num;
	tmp >> res;
	return res;
}
static symbItem* parser_create_new_lable()
{
	string name="lab";
	name+=numtostring(tmplablecode++);
	symbItem* ans=new symbItem();
	ans->name=name;
	ans->kind="constpool";
	ans->type="lable";
	ans->link=NULL;
	global_const_pool.push(ans);
	return ans;
}
static symbItem* parser_create_new_const(symbItem *src)
{
	symbItem *ans=new symbItem();
	if(src->type=="char")
	{
		char tmp=src->value;
		ans->name="\'";
		ans->name+=tmp;
		ans->name+="\'";
	}
	else
		ans->name=numtostring(src->value);
	ans->kind="constpool";
	ans->value=src->value;
	ans->type=src->type;
	ans->link=NULL;
	global_const_pool.push(ans);
	return ans;
}
static void parser_create_new_var()
{
	string name="_";
	name+=numtostring(tmpvarcode++);
	if(!symbtable_enter(name,"var","integer",0,0))//assert
		cout << "Error:parser_create_new_var_symbtable_enter"<< endl;
}
void parser_program()
{
	lex_getch();
	lex_getsym();
	string testset[]={".",""};
	if(!parser_procedure())
	{
		parser_test(testset,2);
		if(lex_sym=="")
		{
			global_error(".","nothing");
			return;
		}
	}
	if(lex_sym!=".")
		global_error(".","nothing");
}
static int parser_procedure()
{
	if(lex_sym=="const")
	{
		lex_getsym();
		if(!parser_constdeclaration())
		{
			string testset[]={"","var","procedure","function","begin"};
			parser_test(testset,5);
			if(lex_sym=="")
			{
				global_error("begin","nothing");
				return 0;
			}
		}
	}
	{
		string testset[]={"","var","procedure","function","begin"};
		int flag;
		parser_test(testset,5,flag);
		if(lex_sym=="")
		{
			global_error("begin","nothing");
			return 0;
		}
	}
	if(lex_sym=="var")
	{
		lex_getsym();
		if(!parser_vardeclaration())
		{
			string testset[]={"","procedure","function","begin"};
			parser_test(testset,4);
			if(lex_sym=="")
			{
				global_error("begin","nothing");
				return 0;
			}
		}
	}
	{
		int flag;
		string testset[]={"","procedure","function","begin"};
		parser_test(testset,4,flag);
		if(lex_sym=="")
		{
			global_error("begin","nothing");
			return 0;
		}
	}
	while(lex_sym=="function"||lex_sym=="procedure")
	{
		string testset[]={"","procedure","function","begin"};
		int flag=1;
		if(lex_sym=="function")
		{
			lex_getsym();
			flag=parser_functiondeclaration();
		}
		else
		{
			lex_getsym();
			flag=parser_proceduredeclaration();
		}
		if(!flag)
		{
			parser_test(testset,4);
			if(lex_sym=="")
			{
				global_error("begin","nothing");
				return 0;
			}
		}
	}
	{
		int flag;
		string testset[]={"","begin"};
		parser_test(testset,2,flag);
		if(lex_sym=="")
		{
			global_error("begin","nothing");
			return 0;
		}
	}
	if(lex_sym!="begin")
	{
		global_error("begin","nothing");
		return 0;
	}
	lex_getsym();
	symbItem *func;	
	if(symbtable_now->father)
		func=symbtable_now->father->last_item;
	else
	{
		func=new symbItem();
		func->name="main";
		func->kind="constpool";
		func->type="lable";
		func->link=NULL;
		global_const_pool.push(func);
	}
	global_new_quadruple("func",func,NULL,NULL);
	if(!parser_compoundstatement())
		return 0;
	global_new_quadruple("ret",NULL,NULL,NULL);
	symbtable_up_level();
	return 1;
}
static int parser_compoundstatement()//require 读了begin 后面的一个单词
{
	string testset[]={"",";","end"};
	if(!parser_statement(NULL))
	{
		parser_test(testset,3);
		if(lex_sym=="")
		{
			global_error("end","nothing");
			return 0;
		}
	}
	while(true)
	{
		if(lex_sym==";")
			lex_getsym();
		else
		{
			string testset[]={"ident","end","","if","for","do","begin","write","read"};
			parser_test(testset,9);
			if(lex_sym=="end")
				break;
			else if(lex_sym=="")
			{
				global_error("end","nothing");
				return 0;
			}
		}
		if(!parser_statement(NULL))
		{
			parser_test(testset,3);
			if(lex_sym=="")
			{
				global_error("end","nothing");
				return 0;
			}
		}
	}
	if(lex_sym!="end")
	{
		global_error("end","nothing");
		return 0;
	}
	lex_getsym();
	return 1;
}
static int parser_constdefinition()//常量定义
{
	if(lex_sym=="ident")
	{
		string constname=lex_token;
		lex_getsym();
		if(lex_sym=="="||lex_sym==":=")
		{
			if(lex_sym==":=")
			{
				global_error("=",":=");//是=不是：=
			}
			lex_getsym();
			if(lex_sym=="char")
			{
				if(!symbtable_enter(constname,"const","char",lex_value,0))
					return 0;
				lex_getsym();

			}
			else if(lex_sym=="+"||lex_sym=="-")
			{
				int mark=lex_sym=="-"?-1:1;
				lex_getsym();
				if(lex_sym=="digit")
				{
					if(!symbtable_enter(constname,"const","integer",lex_value*mark,0))
						return 0;
					lex_getsym();
				}
				else
				{
					global_error("digit",lex_sym);//+-后面应该是数字
					return 0;
				}
			}
			else if(lex_sym=="digit")
			{
				if(!symbtable_enter(constname,"const","integer",lex_value,0))
					return 0;
				lex_getsym();
			}
			else
			{
				global_error("const's value",lex_sym);//常量都没定义明白就不填表了。
				return 0;
			}
		}
		else
		{
			global_error("=",lex_sym);//标识符后面是=
			return 0;
		}
	}
	else
	{
		global_error("ident",lex_sym);//常量定义开头是标识符
		return 0;
	}
	return 1;
}


static int parser_constdeclaration()
{
	string testset[]={",",";",""};
	if(!parser_constdefinition())//去找他的合法后继们
	{
		parser_test(testset,3);
		if(lex_sym=="")//找到了终止符号集
		{
			global_error(";","nothing");
			return 0;
		}
	}//读到了合法的后继之后就应该继续往下干。
	while(lex_sym==",")
	{
		lex_getsym();
		if(!parser_constdefinition())
		{
			parser_test(testset,3);
			if(lex_sym=="")
			{
				global_error(";","nothing");
				return 0;
			}
		}
	}

	if(lex_sym!=";")
	{
		global_error(";","nothing");
	//	string testset={"","var","function","procedure"}//找他的合法后继。。
		return 0;
	}
	lex_getsym();
	return 1;
}

static int parser_vardefinition()
{
	int size=0;
	symbItem *start;
	string consttype;
	if(lex_sym=="ident")
	{
		if(!symbtable_enter(lex_token,"var","",0,0))
			return 0;
		start=symbtable_now->last_item;
		lex_getsym();
		while(lex_sym==",")
		{
			lex_getsym();
			if(lex_sym!="ident")
			{
				global_error("ident",lex_sym);
				return 0;
			}
			else
			{
				if(!symbtable_enter(lex_token,"var","",0,0))
					return 0;
				lex_getsym();
			}
		}
		if(lex_sym!=":")
		{
			global_error(":",lex_sym);
			return 0;
		}
		lex_getsym();
		if(lex_sym=="integer"||lex_sym=="char")
			consttype=lex_sym;
		else
		{
			if(lex_sym=="array")
			{
				if_array=1;
				lex_getsym();
				if(lex_sym!="[")
				{
					global_error("[",lex_sym);
					return 0;
				}
				else
					lex_getsym();
				if(lex_sym!="digit")
				{
					global_error("digit",lex_sym);
					return 0;
				}
				size=lex_value;
				lex_getsym();
				if(lex_sym!="]")
				{
					global_error("]",lex_sym);
					return 0;
				}
				else
					lex_getsym();
				if(lex_sym!="of")
				{
					global_error("of",lex_sym);
				}
				else
					lex_getsym();
				if(lex_sym=="integer"||lex_sym=="char")//这里你调试了很久
					consttype=lex_sym;
				else
				{
					global_error("integer or char",lex_sym);
					return 0;
				}
			}
			else//!array,!integer,!char
			{
				global_error("type",lex_sym);
					return 0;
			}
		}
		while(start!=NULL)
		{
			start->type=consttype;
			start->size=size;
			if(if_array)
			{
				start->kind="array";
			}
			start=start->link;
		}
		if_array=0;
		lex_getsym();
	}
	else
	{
		global_error("ident",lex_sym);//变量定义开头是标识符
		return 0;
	}
	return 1;
}
static int parser_vardeclaration()
{
	string testset[]={"",";"};
	do{
		if(!parser_vardefinition())
		{
			parser_test(testset,2);
			if(lex_sym=="")
			{
				global_error(";","nothing");
				return 0;
			}
		}
		if(lex_sym!=";")
		{
			global_error(";","nothing");
		}
		else
			lex_getsym();
	}while(lex_sym=="ident");
	return 1;
}
static int parser_functiondeclaration()
{
	int para_size=0;
	symbTable *start=symbtable_now;
	string testset[]={"",":"};
	string functype;
	if(lex_sym!="ident")
	{
		global_error("ident",lex_sym);
		return 0;
	}
	else
	{
		if(!symbtable_enter(lex_token,"function","",0,0))
			return 0;
		symbtable_new_level(lex_token);
		lex_getsym();
		if(lex_sym!=":")
		{
			if(!parser_formalparalist(para_size))
			{
				parser_test(testset,2);
				if(lex_sym=="")
				{
					global_error(":","nothing");
					return 0;
				}
			}
			if(lex_sym!=":")
			{
				global_error(":","nothing");
				return 0;
			}
		}
		lex_getsym();
		if(lex_sym!="char"&&lex_sym!="integer")
		{
			global_error("char or integer",lex_sym);
			return 0;
		}

		functype=lex_sym;
		lex_getsym();
		start->last_item->size=para_size;//那是上一层的需要反填的东西。
		start->last_item->type=functype;
		if(lex_sym!=";")
		{
			global_error(";",lex_sym);
			string testset[]={"","begin","const","var","begin","function","procedure"};
			parser_test(testset,6);
			if(lex_sym=="")
			{
				global_error("begin","nothing");
				return 0;
			}
		}
		else
			lex_getsym();
	}
	if(!parser_procedure())
	{
		string testset[]={"",";"};
		parser_test(testset,2);
		if(lex_sym!="")
		{
			global_error(";","nothing");
			return 0;
		}
	}
	if(lex_sym!=";")
	{
		global_error(";",lex_sym);
		return 0;
	}
	else
	lex_getsym();
	return 1;
}
static int parser_proceduredeclaration()
{
	int para_size=0;
	symbTable *start=symbtable_now;
	string procename;
	string testset[]={"",";"};
	if(lex_sym!="ident")
	{
		global_error("ident",lex_sym);
		return 0;
	}
	else
	{
		if(!symbtable_enter(lex_token,"procedure","",0,0))
			return 0;
		symbtable_new_level(lex_token);
		lex_getsym();
		if(lex_sym!=";")
		{
			if(!parser_formalparalist(para_size))
			{
				parser_test(testset,2);
				if(lex_sym=="")
				{
					global_error(";","nothing");
					return 0;
				}
			}
			if(lex_sym!=";")
			{
				global_error(";",lex_sym);
				string testset[]={"","const","var","begin","function","procedure"};
				parser_test(testset,6);
				if(lex_sym=="")
				{
					global_error("procedure\""+lex_token+"\" body expected.");
					return 0;
				}
			}
			else
				lex_getsym();
		}
		else lex_getsym();
		start->last_item->size=para_size;
	}
	if(!parser_procedure())
	{
		string testset[]={"",";"};
		parser_test(testset,2);
		if(lex_sym=="")
		{
			global_error(";","nothing");
			return 0;
		}
	}
	if(lex_sym!=";")
	{
		global_error(";",lex_sym);
		return 0;
	}
	lex_getsym();
	return 1;
}

static int parser_formalparalist(int &para_size)
{
	string testset[]={"",";",")"};
	if(lex_sym!="(")
	{
		global_error("(",lex_sym);
		return 0;
	}
	else
	{
		do{
			lex_getsym();
			if(!parser_formalparasection(para_size))
			{
				parser_test(testset,3);
				if(lex_sym=="")
				{
					global_error(")","nothing");
					return 0;
				}
			}
		}while(lex_sym==";");
		if(lex_sym!=")")
		{
			global_error(")",lex_sym);
			return 0;
		}
		lex_getsym();
	}
	return 1;
}

static int parser_formalparasection(int &para_size)
{
	int paraifvar=0;
	symbItem *start;
	string paratype;
	if(lex_sym=="var")
	{
		paraifvar=1;
		lex_getsym();
	}
	if(lex_sym!="ident")
	{
		global_error("ident",lex_sym);
		return 0;
	}
	else
	{
		para_size++;
		if(!symbtable_enter(lex_token,"parameter","",0,paraifvar))
			return 0;
		start=symbtable_now->last_item;
		lex_getsym();
		while(lex_sym==",")
		{
			lex_getsym();
			if(lex_sym!="ident")
			{
				global_error("ident",lex_sym);
				return 0;
			}
			else
			{
				para_size++;
				if(!symbtable_enter(lex_token,"parameter","",0,paraifvar))
					return 0;
				lex_getsym();
			}
		}
		if(lex_sym!=":")
		{
			global_error(":",lex_sym);
			return 0;
		}
		lex_getsym();
		if(lex_sym!="integer"&&lex_sym!="char")
		{
			global_error("char or integer",lex_sym);
			return 0;
		}
		else
		{
			paratype=lex_sym;
			while(start!=NULL)
			{
				start->type=paratype;
				start=start->link;
			}
			lex_getsym();
		}
	}
	return 1;
}

static int parser_expression()
{
	symbItem *src1,*src2,*ans;
	string opr;
	int if_low_zero=0;
	if(lex_sym=="-"||lex_sym=="+")
	{
		if_low_zero=(lex_sym=="-")?1:0;
		lex_getsym();
	}
	if(!parser_term(if_low_zero))
		return 0;
	while(lex_sym=="-"||lex_sym=="+")
	{
		opr=lex_sym=="+"?"add":"sub";
		lex_getsym();
		if(!parser_term(if_low_zero))
			return 0;
		src2=operand_stack.top();
		operand_stack.pop();
		if(src2&&src2->kind=="const")
			src2=parser_create_new_const(src2);
		src1=operand_stack.top();
		operand_stack.pop();
		if(src1&&src1->kind=="const")
			src1=parser_create_new_const(src1);
		parser_create_new_var();
		ans=symbtable_now->last_item;
		global_new_quadruple(opr,src1,src2,ans);
		operand_stack.push(ans);
	}
	return 1;
}
static int parser_term(int &if_low_zero)
{
	symbItem *src1,*src2,*ans;
	string opr;
	if(!parser_factor())
		return 0;
	if(if_low_zero)
	{
		parser_create_new_var();
		ans=symbtable_now->last_item;
		src1=operand_stack.top();
		operand_stack.pop();
		if(src1&&src1->kind=="const")
			src1=parser_create_new_const(src1);
		global_new_quadruple("assign",src1,NULL,ans);
		global_new_quadruple("neg",ans,NULL,NULL);
		operand_stack.push(ans);
		if_low_zero=0;
	}
	while(lex_sym=="*"||lex_sym=="/")
	{
		opr=lex_sym=="*"?"imul":"idiv";
		lex_getsym();
		if(!parser_factor())
			return 0;
		parser_create_new_var();
		ans=symbtable_now->last_item;
		src2=operand_stack.top();
		operand_stack.pop();
		if(src2&&src2->kind=="const")
			src2=parser_create_new_const(src2);
		src1=operand_stack.top();
		operand_stack.pop();
		if(src1&&src1->kind=="const")
			src1=parser_create_new_const(src1);
		global_new_quadruple(opr,src1,src2,ans);
		operand_stack.push(ans);
	}
	return 1;
}

static int parser_factor()
{
	symbItem *operand;
	if(lex_sym=="ident")
	{
		operand=symbtable_check(lex_token);
		if(operand==NULL)
		{
			global_error("legal ident",lex_token);
			return 0;
		}
		if(operand->kind=="array")
		{
			parser_create_new_var();
			symbItem *ans=symbtable_now->last_item,*src2;
			lex_getsym();
			if(lex_sym!="[")
			{
				global_error("[",lex_sym);
				return 0;
			}
			lex_getsym();
			if(!parser_expression())
			{
				string testset[]={"","]"};
				parser_test(testset,2);
				if(lex_sym=="")
				{
					global_error("]","nothing");
					return 0;
				}
			}
			else{
				src2=operand_stack.top();
				operand_stack.pop();
				if(src2->kind=="const")
					src2=parser_create_new_const(src2);
			}
			if(lex_sym!="]")
			{
				global_error("]",lex_sym);
				return 0;
			}
			global_new_quadruple("larray",operand,src2,ans);
			operand_stack.push(ans);
			lex_getsym();
		}
		else if(operand->kind=="function")
		{
			parser_create_new_var();
			symbItem *ans=symbtable_now->last_item;
			lex_getsym();
			if(lex_sym=="("&&!operand->size)
			{
				global_error("no parameter",lex_sym);
				return 0;
			}
			if(!operand->size)
				lex_getsym();
			else
			{
				if(!parser_realparameterlist(operand))
					return 0;
			}
			global_new_quadruple("call",operand,NULL,NULL);
			global_new_quadruple("assign",operand,NULL,ans);
			operand_stack.push(ans);
		}
		else if(operand->kind=="procedure")
		{
			global_error("factor","procedure "+operand->name);
			return 0;
		}
		else
		{
			operand_stack.push(operand);//获得操作数
			lex_getsym();
		}
	}
	else if(lex_sym=="(")
	{
		lex_getsym();
		if(!parser_expression())
		{
			string testset[]={"",")"};
			parser_test(testset,2);
			if(lex_sym=="")
			{
				global_error(")","nothing");
				return 0;
			}
		}
		if(lex_sym!=")")
		{
			global_error(")",lex_sym);
			return 0;
		}
		lex_getsym();
	}
	else if(lex_sym=="digit")
	{
		symbItem *item=new symbItem();
		item->name=numtostring(lex_value);
		item->value=lex_value;
		item->kind="constpool";
		item->type="digit";//digit,char,string,lable
		item->link=NULL;
		operand_stack.push(item);
		global_const_pool.push(item);//放到常量池里面
		lex_getsym();
	}
	else
	{
		global_error("legal factor",lex_sym);
		return 0;
	}
	return 1;
}
static int if_ralation_opr(string a)
{
	if(a==":=")
	{
		global_error("=",a);
		return 1;
	}
	return a=="="||
		a==">"||a==">="||
		a=="<"||a=="<="||
		a=="<>";
}

static int parser_statement(symbItem *forbid)
{
	symbItem *tmp;
	if(lex_sym=="ident")
	{
		tmp=symbtable_check(lex_token);
		if(tmp==NULL)
		{
			global_error("legal ident",lex_token);
			return 0;
		}
		if(tmp==forbid)//该标识符不可以被赋值
		{
			global_error("this ident\""+tmp->name+"\"can not be changed.");
			return 0;
		}
		if(tmp->kind=="const")
		{
			global_error(tmp->name+" can not be changed.");
			return 0;
		}
		else if(tmp->kind=="var"||tmp->kind=="parameter")
		{
			lex_getsym();
			if(lex_sym==":="||lex_sym=="=")
			{
				if(lex_sym=="=")
					global_error(":=","=");
			}
			else
			{
				global_error(":=",lex_sym);
				return 0;
			}
			lex_getsym();
			if(!parser_expression())
			{
				return 0;
			}
			symbItem *expre=operand_stack.top();
			operand_stack.pop();
			if(expre->kind=="const")
				expre=parser_create_new_const(expre);
			global_new_quadruple("assign",expre,NULL,tmp);
		}
		else if(tmp->kind=="array")
		{//tmp [src1]
			symbItem *src2;
			lex_getsym();
			if(lex_sym!="[")
			{
				global_error("[",lex_sym);
				return 0;
			}
			lex_getsym();
			if(!parser_expression())
			{
				string testset[]={"","]",":="};
				parser_test(testset,3);
				if(lex_sym=="")
				{
					global_error("]","nothing");
					return 0;
				}
			}
			else
			{
				src2=operand_stack.top();
				operand_stack.pop();
				if(src2->kind=="const")
					src2=parser_create_new_const(src2);
			}
			if(lex_sym!="]")
			{
                global_error("]",lex_sym);
                return 0;
			}
			lex_getsym();
			if(lex_sym==":="||lex_sym=="=")
            {
                if(lex_sym=="=")
                    global_error(":=",lex_sym);
            }
            else
            {
                global_error(":=",lex_sym);
                return 0;
            }
            lex_getsym();
            if(!parser_expression())
                return 0;
            symbItem *ans=operand_stack.top();
            operand_stack.pop();
			if(ans->kind=="const")
				ans=parser_create_new_const(ans);
            global_new_quadruple("sarray",tmp,src2,ans);//tmp[src2]=ans
		}
		else if(tmp->kind=="function")//函数标示符=表达式
        {//函数标识符只能是本层函数或者他的父层函数
			if(!symbtable_if_can_change_func(tmp->name))
			{
				global_error("function \""+tmp->name+"\" can not be changed in this scope.");
				return 0;
			}
			lex_getsym();
			if(lex_sym!="="&&lex_sym!=":=")
			{
				global_error(":=",lex_sym);
				return 0;
			}
			if(lex_sym=="=")
				global_error(":=",lex_sym);
			lex_getsym();
			if(!parser_expression())
				return 0;
			symbItem *src1=operand_stack.top();
			operand_stack.pop();
			if(src1&&src1->kind=="const")
				src1=parser_create_new_const(src1);
			global_new_quadruple("assign",src1,NULL,tmp);
        }
        else if(tmp->kind=="procedure")//过程调用语句,tmp已经经过差表
        {
			if(!tmp->size)
			{
				lex_getsym();
				if(lex_sym=="(")
				{
					global_error("no parameter",lex_sym);
					return 0;
				}
			}
			else
			{
				lex_getsym();
				if(!parser_realparameterlist(tmp))
					return 0;
			}
			global_new_quadruple("call",tmp,NULL,NULL);
        }
        else
        {
			global_error("no possible.");
            return 0;
        }
	}
	else if(lex_sym=="if")//
    {
		symbItem *lable1,*lable2,*src1,*src2;
		string oprname;
		lable1=parser_create_new_lable();
        lex_getsym();
        if(!parser_condition(&src1,&src2,oprname))
		{
			string testset[]={"","then","else"};
			parser_test(testset,3);
			if(lex_sym=="")
			{
				global_error("then","nothing");
				return 0;
			}
		}
		else
		{
			oprname=global_anti_ralation[oprname];
			if(src1&&src1->kind=="const")
				src1=parser_create_new_const(src1);
			if(src2&&src2->kind=="const")
				src2=parser_create_new_const(src2);
			global_new_quadruple(oprname,src1,src2,lable1);
		}
		if(lex_sym!="then")
		{
			global_error("then",lex_sym);//容忍缺then的行为
		}
		else
			lex_getsym();
		if(!parser_statement(NULL))
		{
			return 0;//????
		}
		if(lex_sym!="else")
		{
			global_new_quadruple("lab",lable1,NULL,NULL);
		}
		else
		{
			lable2=parser_create_new_lable();
			global_new_quadruple("jmp",lable2,NULL,NULL);
			global_new_quadruple("lab",lable1,NULL,NULL);
			lex_getsym();
			if(!parser_statement(NULL))
				return 0;
			global_new_quadruple("lab",lable2,NULL,NULL);
		}
    }
	else if(lex_sym=="begin")
	{
		lex_getsym();
		if(!parser_compoundstatement())
			return 0;
	}
	else if(lex_sym=="do")
	{
		symbItem *lable1=parser_create_new_lable();
		symbItem *src1,*src2;
		string oprname;
		global_new_quadruple("lab",lable1,NULL,NULL);
		lex_getsym();
		if(!parser_statement(NULL))
		{
			string testset[]={"","while"};
			parser_test(testset,2);
			if(lex_sym=="")
			{
				global_error("while","nothing");
				return 0;
			}
		}
		if(lex_sym!="while")
		{
			global_error("while",lex_sym);
			return 0;
		}
		lex_getsym();
		if(!parser_condition(&src1,&src2,oprname))
			return 0;
		oprname=global_ralation[oprname];
		if(src1&&src1->kind=="const")
			src1=parser_create_new_const(src1);	
		if(src2&&src2->kind=="const")
			src2=parser_create_new_const(src2);
		global_new_quadruple(oprname,src1,src2,lable1);
	}
	else if(lex_sym=="write")
	{
		string testset[]={"",")"};
		lex_getsym();
		if(lex_sym!="(")
		{
			global_error("(",lex_sym);
			string testset2[]={"",")"};
			parser_test(testset2,2);
			if(lex_sym=="")
			{
				global_error(")","nothing");
				return 0;
			}
		}
		else
			lex_getsym();
		if(lex_sym=="string")
		{
			symbItem *item=new symbItem();
			item->name=lex_token;	
			my_write_string.push(lex_token);
			item->value=my_writes_num++;
			item->kind="constpool";
			item->type="string";//digit,char,string,lable
			item->link=NULL;
			global_const_pool.push(item);
			global_new_quadruple("writes",item,NULL,NULL);
			lex_getsym();
			if(lex_sym==",")
			{
				lex_getsym();
				if(!parser_expression())
				{
					parser_test(testset,2);
					if(lex_sym=="")
					{
						global_error(")","nothing");
						return 0;
					}
				}
				else
				{
					symbItem *src=operand_stack.top();
					if(src->kind=="const")
						src=parser_create_new_const(src);
					global_new_quadruple("writee",src,NULL,NULL);
					operand_stack.pop();
				}

			}

		}
		else//表达式
		{
			if(!parser_expression())
			{
				parser_test(testset,2);
				if(lex_sym=="")
				{
					global_error(")","nothing");
					return 0;
				}
			}
			else
			{
				symbItem *src=operand_stack.top();
				if(src->kind=="const")
					src=parser_create_new_const(src);
				global_new_quadruple("writee",src,NULL,NULL);
				operand_stack.pop();
			}

		}
		if(lex_sym!=")")
		{
			global_error(")",lex_sym);
			return 0;
		}
		lex_getsym();
	}
	else if(lex_sym=="read")//func,var,para?
	{
		lex_getsym();
		int flag=0;
		if(lex_sym!="(")
		{
			global_error("(",lex_sym);
			return 0;
		}
		do{
			lex_getsym();
			if(lex_sym!="ident")
			{
				global_error("ident",lex_sym);
				flag=1;
			}
			else//"ident"
			{
				symbItem *ans=symbtable_check(lex_token);
				if(ans==NULL)
				{
					global_error(lex_token+" is not declared in this scope.");
					flag=1;
				}
				else
				{
					if(ans->kind=="const")
					{
						global_error("const \""+ans->name+"\" can not be changed");
						flag=1;
					}
					else if(ans->kind=="procedure")
					{
						global_error("procedure \""+ans->name+"\" can not be changed");
						flag=1;
					}
					else if(ans->kind=="function"&&!symbtable_if_can_change_func(ans->name))
					{
							global_error("function \""+ans->name+"\" can not be changed in this scope.");
							flag=1;
					}
					else
					{
						global_new_quadruple("read",ans,NULL,NULL);
					}
				}
			}
			if(flag)
			{
				string testset[]={"",",",")"};
				parser_test(testset,3);
				if(lex_sym=="")
				{
					global_error(")",lex_sym);
					return 0;
				}
			}
			else
				lex_getsym();
		}while(lex_sym==",");
		if(lex_sym!=")")
		{
			global_error(")",lex_sym);
			return 0;
		}
		lex_getsym();
	}
	else if(lex_sym=="for")
	{
		int downto=0;
		symbItem *tmp,*target,*lable1,*lable2;
		lex_getsym();
		if(lex_sym!="ident")
		{
			global_error("ident",lex_sym);
			return 0;
		}
		tmp=symbtable_check(lex_token);
		int flag=0;
		if(tmp->kind=="const")
		{
			global_error("const \""+tmp->name+"\" can not be changed.");
			flag=1;
		}
		else if(tmp->kind=="procedure")
		{
			global_error("procedure \""+tmp->name+"\" can not be changed.");
			flag=1;
		}
		else
		{
			if(!parser_statement(NULL))
				flag=1;//生成首次赋值语句。
		}
		if(flag)
		{
			string testset[]={"","to","downto"};
			parser_test(testset,3);
			if(lex_sym=="")
			{
				global_error("downto or to","nothing");
				return 0;
			}
		}
		if(lex_sym!="downto"&&lex_sym!="to")
		{
			global_error("downto or to",lex_sym);
			return 0;
		}
		else
			downto=lex_sym=="downto";
		lex_getsym();
		if(!parser_expression())
		{
			string testset[]={"","do"};
			parser_test(testset,2);
			if(lex_sym=="")
			{
				global_error("do","nothing");
				return 0;
			}
		}
		else
		{
			target=operand_stack.top();
			operand_stack.pop();
			if(target->kind=="const")
				target=parser_create_new_const(target);
		}
		if(lex_sym!="do")
		{
			global_error("do",lex_sym);
		//	return 0;
		}
		lex_getsym();
		lable1=parser_create_new_lable();
		lable2=parser_create_new_lable();
		global_new_quadruple("lab",lable1,NULL,NULL);
		string opr=downto?"jl":"jg";
		global_new_quadruple(opr,tmp,target,lable2);
		if(!parser_statement(tmp))
			return 0;
		opr=downto?"dec":"inc";
		global_new_quadruple(opr,tmp,NULL,NULL);
		global_new_quadruple("jmp",lable1,NULL,NULL);
		global_new_quadruple("lab",lable2,NULL,NULL);
	}
	else
	{
		//空语句怎么处理，那就不处理好了?
	}
	return 1;
}
static int parser_condition(symbItem **src1,symbItem **src2,string &oprname)//传进lable1
{
	if(!parser_expression())
	{
		string testset[]={"","<","<=",">",">=","=","<>"};
		parser_test(testset,7);
		if(lex_sym=="")
		{
			global_error("relational operators","nothing");
			return 0;
		}
	}
	else
	{
		*src1=operand_stack.top();
		operand_stack.pop();
	}
	oprname=lex_sym;
	if(!if_ralation_opr(oprname))
	{
		global_error("relational operators","nothing");
		return 0;
	}
	lex_getsym();
	if(!parser_expression())
		return 0;
	*src2=operand_stack.top();
	operand_stack.pop();
	return 1;
}
//Require: item is func or proc
//暂时不跳读，因为，一行而已。。本身久错了
static int parser_realparameterlist(symbItem *func_proc)
{//func后面一定要紧跟着参数信息，这是四元式中操作数的一部分，虽然你没用到他的值，但你用到的是他的其他信息;
	//need to find the location of func or proc
	stack<symbItem*> para_stack;
	symbItem *src;
	if(lex_sym!="(")
	{
		global_error("(",lex_sym);
		return 0;
	}
	do{
		lex_getsym();
		if(!parser_expression())
		{
			string testset[]={"",",",")"};
			parser_test(testset,3);
			if(lex_sym=="")
			{
				global_error(")","nothing");
				return 0;
			}
		}
		else
		{
			para_stack.push(operand_stack.top());
			operand_stack.pop();
		}
	}while(lex_sym==",");
	if((unsigned)func_proc->size!=para_stack.size())
	{
		global_error("wrong num of parametes in \""+func_proc->name+"\"call.");
		return 0;
	}//good,我是指针我怕谁

	//require: enough para item in the global_table
	int j=func_proc->size;
	symbItem *k=func_proc->link;
	if(func_proc->name==symbtable_now->name)//调用自身此时符号表没调整。
		k=symbtable_now->first_item;
	for(int i=1;i<=j;i++)
	{
		src=para_stack.top();
		para_stack.pop();
		if(k->para_ifvar&&src->kind!="const")//可能撤表了也可能没撤表
			global_new_quadruple("rpara",src,func_proc,NULL);
		else
		{
			if(src->kind=="const")
				src=parser_create_new_const(src);
			global_new_quadruple("fpara",src,func_proc,NULL);
		}
		k=k->link;//Require k<>NULL;
	}
	if(lex_sym!=")")
	{
		global_error(")",lex_sym);
		return 0;
	}
	lex_getsym();
	return 1;
}

