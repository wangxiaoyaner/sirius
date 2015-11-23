#include"global.h"
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
static void parser_test(string a[],int num)//寻找合法后继或者终止符号集合
{
	while(!parser_ifintestset(a,num,lex_sym))
			lex_getsym();
}

static string numtostring(int num)
{
	stringstream tmp;
	string res;
	tmp << num;
	tmp >> res;
	return res;
}
static symbItem* parser_create_new_lable(string name,int level)
{
	symbItem* ans=new symbItem();
	ans->name=level?name+numtostring(level):"main";
	ans->kind="constpool";
	ans->type="lable";
	ans->link=NULL;
	global_const_pool.push(ans);
	return ans;
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
			global_error(40,"");
		}
	}
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
				global_error(41,"");
				return 0;
			}
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
				global_error(41,"");
				return 0;
			}
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
				global_error(41,"");
				return 0;
			}
		}
	}
	if(lex_sym!="begin")
	{
		global_error(41,"");
		return 0;
	}
	lex_getsym();
	symbItem *func_lab=parser_create_new_lable(symbtable_now->name,symbtable_now->level);
	global_new_quadruple("lab",func_lab,NULL,NULL);
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
			global_error(14,"");
			return 0;
		}
	}
	while(lex_sym==";")
	{
		lex_getsym();
		if(!parser_statement(NULL))
		{
			parser_test(testset,3);
			if(lex_sym=="")
			{
				global_error(14,"");
				return 0;
			}
		}
	}
	if(lex_sym!="end")
	{
		global_error(14,"");
		return 0;
	}
	lex_getsym();

}
static int parser_constdefinition()//常量定义
{
	if(lex_sym=="ident")
	{
		string constname=lex_token;
		lex_getsym();
		if(lex_sym=="="||lex_sym==":=")//简单矫正
		{
			if(lex_sym==":=")
			{
				global_error(7,"");//是=不是：=
			}
			lex_getsym();

			if(lex_sym=="char")//词法分析的时候随便给了一个值
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
					global_error(9,"");//+-后面应该是数字
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
				global_error(8,constname);//常量都没定义明白就不填表了。
				return 0;
			}
		}
		else
		{
			global_error(6,"");//标识符后面是=
			return 0;
		}
	}
	else
	{
		global_error(17,"");//常量定义开头是标识符
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
			global_error(11,"");
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
				global_error(11,"");
				return 0;
			}
		}
	}

	if(lex_sym!=";")
	{
		global_error(11,"");
		return 0;
	}
	lex_getsym();
	return 1;
}

//int symbtable_enter(string name,kind,type,int value,level,size,adr,para_ifvar)
static int parser_vardefinition()
{
	int size=0;
	symbItem *start;
	string consttype;
	if(lex_sym=="ident")
	{
	//constname=lex_token;
		if(!symbtable_enter(lex_token,"var","",0,0))
			return 0;
		start=symbtable_now->last_item;
		lex_getsym();
		while(lex_sym==",")
		{
			lex_getsym();
			if(lex_sym!="ident")
			{
				global_error(17,"");
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
			global_error(18,"");
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
					global_error(12,"");
					return 0;
				}
				else
					lex_getsym();
				if(lex_sym!="digit")
				{
					global_error(14,"");
					return 0;
				}
				size=lex_value;
				lex_getsym();
				if(lex_sym!="]")
				{
					global_error(13,"");
					return 0;
				}
				else
					lex_getsym();
				if(lex_sym!="of")
				{
					global_error(15,"");
					//return 0;//允许变量定义的时候没有of
				}
				else
					lex_getsym();
				if(lex_sym=="integer"||lex_sym=="char")//这里你调试了很久
					consttype=lex_sym;
				else
				{
					global_error(16,"");
					return 0;
				}
			}
			else//!array,!integer,!char
			{
				global_error(16,"");
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
		global_error(17,"");//变量定义开头是标识符
		return 0;
	}
	return 1;
}
static int parser_vardeclaration()
{
	string testset[]={"",";"};
	if(!parser_vardefinition())
	{
		parser_test(testset,2);
		if(lex_sym=="")
		{
			global_error(11,"");
			return 0;
		}
	}

	if(lex_sym!=";")
	{
		global_error(11,"");
	}
	lex_getsym();
	while(lex_sym=="ident")
	{
		if(!parser_vardefinition())
		{
			parser_test(testset,2);
			if(lex_sym=="")
			{
				global_error(11,"");
				return 0;
			}
		}
		if(lex_sym!=";")
			global_error(11,"");
		lex_getsym();
	}
	return 1;
}
//这里需要产生一条LABLE语句，但是我假定只有过程头部,已经验证过procedure且预读
//name,"procedure","",0,level,size,0,
//过程首部

static int parser_functiondeclaration()
{
	int para_size=0;
	symbTable *start=symbtable_now;
	string testset[]={"",":"};
	string functype;
	if(lex_sym!="ident")
	{
		global_error(16,"");
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
					global_error(18,"");
					return 0;
				}
			}
			if(lex_sym!=":")
			{
				global_error(18,"");
				return 0;
			}
			lex_getsym();
		}
		if(lex_sym!="char"&&lex_sym!="integer")
		{
			global_error(16,"");
			return 0;
		}

		functype=lex_sym;
		lex_getsym();
		start->last_item->size=para_size;//那是上一层的需要反填的东西。
		start->last_item->type=functype;
		if(lex_sym!=";")
		{
			global_error(11,"");
			return 0;
		}
		lex_getsym();
	}
	if(!parser_procedure())
	{
		string testset[]={"",";"};
		parser_test(testset,2);
		if(lex_sym!="")
		{
			global_error(11,"");
			return 0;
		}
	}
	if(lex_sym!=";")
	{
		global_error(11,"");
		return 0;
	}
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
		global_error(16,"procedure declaration");
		return 0;
	}
	else
	{
		if(!symbtable_enter(lex_token,"procedure","",0,0))
			return 0;
		symbtable_new_level(lex_token);//进入了新的一层
		lex_getsym();
		if(lex_sym!=";")
		{
			if(!parser_formalparalist(para_size))
			{
				parser_test(testset,2);
				if(lex_sym=="")
				{
					global_error(11,"277");
					return 0;
				}
			}
			if(lex_sym!=";")
			{
				global_error(11,"284");
				return 0;
			}
			lex_getsym();

		}
		else
		{
			lex_getsym();
		}
		start->last_item->size=para_size;
	}
	if(!parser_procedure())
	{
		string testset[]={"",";"};
		parser_test(testset,2);
		if(lex_sym=="")
		{
			global_error(11,"");
			return 0;
		}
	}
	if(lex_sym!=";")
	{
		global_error(11,"");
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
		global_error(19,"");
		return 0;
	}
	else
	{
		lex_getsym();
		if(!parser_formalparasection(para_size))
		{
			parser_test(testset,3);
			if(lex_sym=="")
			{
				global_error(20,"");
				return 0;
			}
		}
		while(lex_sym==";")
		{
			lex_getsym();
			if(!parser_formalparasection(para_size))
			{
				parser_test(testset,3);
				if(lex_sym=="")
				{
					global_error(20,"");
					return 0;
				}
			}
		}
		if(lex_sym!=")")
		{
			global_error(20,"");
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
		global_error(17,"305");
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
				global_error(17,"318");
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
			global_error(18,"330");
			return 0;
		}
		lex_getsym();
		if(lex_sym!="integer"&&lex_sym!="char")
		{
			global_error(16,"336");
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

//表达式，假设没有函数调用语句
//整个表达式
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
	while(lex_sym=="-"||lex_sym=="+")//需要一个新的变量了。
	{
		opr=lex_sym;
		lex_getsym();
		if(!parser_term(if_low_zero))
			return 0;
		src2=operand_stack.top();
		operand_stack.pop();
		src1=operand_stack.top();
		operand_stack.pop();
		parser_create_new_var();//建立一个新表项。
		ans=symbtable_now->last_item;
		global_new_quadruple(opr,src1,src2,ans);
		operand_stack.push(ans);
	}
	return 1;
}
static int parser_term(int &if_low_zero)//NECx8里的取反指令
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
		global_new_quadruple("assign",src1,NULL,ans);
		global_new_quadruple("neg",NULL,NULL,ans);
		operand_stack.push(ans);
		if_low_zero=0;
	}
	while(lex_sym=="*"||lex_sym=="/")
	{
		opr=lex_sym;
		lex_getsym();
		if(!parser_factor())
			return 0;
		parser_create_new_var();
		ans=symbtable_now->last_item;
		src2=operand_stack.top();
		operand_stack.pop();
		src1=operand_stack.top();
		operand_stack.pop();
		global_new_quadruple(opr,src1,src2,ans);
		operand_stack.push(ans);
	}
	return 1;
}

static int parser_factor()//取得因此放到栈上
{
	symbItem *operand;
	if(lex_sym=="ident")
	{
		operand=symbtable_check(lex_token);
		if(operand==NULL)
		{
			global_error(21,lex_token);
			return 0;
		}
		if(operand->kind=="array")
		{
			parser_create_new_var();
			symbItem *ans=symbtable_now->last_item,*src2;
			lex_getsym();
			if(lex_sym!="[")
			{
				global_error(12,"array");
				return 0;
			}
			lex_getsym();
			if(!parser_expression())
				return 0;
			src2=operand_stack.top();
			operand_stack.pop();
			if(lex_sym!="]")
			{
				global_error(13,"array");
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
				global_error(30,operand->name);
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
			return 0;
		if(lex_sym!=")")
		{
			global_error(20,"");
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
		global_error(22,lex_sym);
		return 0;
	}
	return 1;
}
static int if_ralation_opr(string a)
{
	if(a==":=")
	{
		global_error(7,"");
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
	if(lex_sym=="ident")//
	{
		tmp=symbtable_check(lex_token);
		if(tmp==NULL)//他么还没定义就敢用
		{
			global_error(21,lex_token);
			return 0;
		}
		if(tmp==forbid)//该标识符不可以被赋值
		{
			global_error(37,"");
			return 0;	
		}
		if(tmp->kind=="const")
		{
			global_error(25,"");
			return 0;
		}
		else if(tmp->kind=="var"||tmp->kind=="parameter")
		{
			lex_getsym();
			if(lex_sym==":="||lex_sym=="=")
			{
				if(lex_sym=="=")
					global_error(27,"");
			}
			else
			{
				global_error(36,"");
				return 0;
			}
			lex_getsym();
			//static stack<symbItem*> operand_stack;
			if(!parser_expression())
			{
				return 0;//你特么语句久错了还好意思往下作。。。
			}
			symbItem *expre=operand_stack.top();
			operand_stack.pop();
			global_new_quadruple("assign",expre,NULL,tmp);
		}//读字符的工作在分支内完成。
		else if(tmp->kind=="array")
		{//tmp [src1]
			lex_getsym();
			if(lex_sym!="[")
			{
				global_error(12,"");
				return 0;
			}
			lex_getsym();
			if(!parser_expression())//表达式的结果是可能是一个变量，你不可能知道数组越界否。
				return 0;
            symbItem *src2=operand_stack.top();
            operand_stack.pop();
			if(lex_sym!="]")
			{
                global_error(13,"");
                return 0;
			}
			lex_getsym();
			if(lex_sym==":="||lex_sym=="=")
            {
                if(lex_sym=="=")
                    global_error(27,"");
            }
            else
            {
                global_error(36,"");
                return 0;
            }
            lex_getsym();
            if(!parser_expression())
                return 0;
            symbItem *ans=operand_stack.top();
            operand_stack.pop();
            global_new_quadruple("sarray",tmp,src2,ans);//tmp[src2]=ans
		}
		else if(tmp->kind=="function")//函数标示符=表达式
        {//函数标识符只能是本层函数或者他的父层函数
			if(!symbtable_if_can_change_func(tmp->name))
			{
				global_error(38,"");
				return 0;
			}
			lex_getsym();
			if(lex_sym!="="&&lex_sym!=":=")
			{
				global_error(36,"");
				return 0;
			}
			if(lex_sym=="=")
				global_error(7,"");
			lex_getsym();
			if(!parser_expression())
				return 0;
			symbItem *src1=operand_stack.top();
			operand_stack.pop();
			global_new_quadruple("assign",src1,NULL,tmp);		
        }
        else if(tmp->kind=="procedure")//过程调用语句,tmp已经经过差表
        {
			if(!tmp->size)
			{
				lex_getsym();
				if(lex_sym=="(")
				{
					global_error(30,tmp->name);
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
            global_error(0,"我擦，还能是什么东西");
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
            return 0;
		oprname=global_anti_ralation[oprname];
		
		global_new_quadruple(oprname,src1,src2,lable1);

		if(lex_sym!="then")
		{
			global_error(24,"");//容忍缺then的行为
		}
		lex_getsym();
		if(!parser_statement(NULL))
			return 0;
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
			return 0;
		if(lex_sym!="while")
		{
			global_error(29,"");
			return 0;
		}
		lex_getsym();
		if(!parser_condition(&src1,&src2,oprname))
			return 0;
		oprname=global_ralation[oprname];
		global_new_quadruple(oprname,src1,src2,lable1);		
	}
	else if(lex_sym=="write")
	{
		string testset[]={"",")"};
		lex_getsym();
		if(lex_sym!="(")
		{
			global_error(19,"");
			return 0;
		}
		lex_getsym();
		if(lex_sym=="string")
		{
			symbItem *item=new symbItem();
			item->name=lex_token;
			item->value=0;
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
						global_error(20,"");
						return 0;
					}
				}
				else
				{
					global_new_quadruple("writed",operand_stack.top(),NULL,NULL);
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
					global_error(20,"");
					return 0;
				}
			}
			else
			{
				global_new_quadruple("writee",operand_stack.top(),NULL,NULL);
				operand_stack.pop();
			}

		}
		if(lex_sym!=")")
		{
			global_error(20,"");
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
			global_error(19,"");
			return 0;
		}
		do{
			lex_getsym();
			if(lex_sym!="ident")
			{
				global_error(17,"");
				flag=1;
			}
			else//"ident"
			{
				symbItem *ans=symbtable_check(lex_token);
				if(ans==NULL)
				{
					global_error(21,"");
					flag=1;
				}
				else
				{
					if(ans->kind=="const")
					{
						global_error(25,"");
						flag=1;
					}
					else if(ans->kind=="procedure")
					{
						global_error(39,"");
						flag=1;
					}
					else if(ans->kind=="function"&&!symbtable_if_can_change_func(ans->name))
					{
							global_error(38,"");
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
					global_error(20,"");
					return 0;
				}
			}
			else
				lex_getsym();
		}while(lex_sym==",");
		if(lex_sym!=")")
		{
			global_error(20,"");
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
			global_error(17,"in for branch");
			return 0;
		}
		tmp=symbtable_check(lex_token);
		if(tmp->kind=="const")
		{
			global_error(25,"in for branch");
			return 0;
		}
		else if(tmp->kind=="procedure")
		{
			global_error(39,"");
			return 0;
		}
		else
		{
			if(!parser_statement(NULL))
				return 0;//生成首次赋值语句。
		}
		if(lex_sym!="downto"&&lex_sym!="to")
		{
			global_error(33,"");
			return 0;
		}
		else
			downto=lex_sym=="downto";
		lex_getsym();
		if(!parser_expression())
			return 0;
		target=operand_stack.top();
		operand_stack.pop();
		if(lex_sym!="do")
		{
			global_error(34,"");
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
		return 0;
	*src1=operand_stack.top();
	operand_stack.pop();

	oprname=lex_sym;
	if(!if_ralation_opr(oprname))
	{
		global_error(28,"");
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
	queue<symbItem*> para_queue;
	symbItem *src;
	if(lex_sym!="(")
	{
		global_error(19,func_proc->name);
		return 0;
	}
	lex_getsym();
	if(!parser_expression())
	{
		return 0;
	}
	para_queue.push(operand_stack.top());
	operand_stack.pop();
	while(lex_sym==",")
	{
		lex_getsym();
		if(!parser_expression())
		{
			return 0;
		}
		para_queue.push(operand_stack.top());
		operand_stack.pop();
	}
	if(func_proc->size!=para_queue.size())
	{
		global_error(30,func_proc->name);
		return 0;
	}//good,我是指针我怕谁
	
	//require: enough para item in the global_table
	int j=func_proc->size;
	symbItem *k=func_proc->link;
	if(func_proc->name==symbtable_now->name)//调用自身此时符号表没调整。
		k=symbtable_now->first_item;
	for(int i=1;i<=j;i++)
	{
		src=para_queue.front();
		para_queue.pop();
		if(k->para_ifvar)//可能撤表了也可能没撤表
			global_new_quadruple("rpara",src,NULL,NULL);
		else
			global_new_quadruple("fpara",src,NULL,NULL);
		k=k->link;//Require k<>NULL;
	}
	if(lex_sym!=")")
	{
		global_error(20,func_proc->name);
		return 0;
	}
	lex_getsym();
	return 1;
}

