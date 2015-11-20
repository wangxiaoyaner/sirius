#include"global.h"
static int if_array=0;
static stack<symbItem*> operand_stack; 
static int tmpvarcode=1;//为产生新的中间变量进行编号。//_num
static int parser_ifintestset(string a[],int num,string target)//检查target是否在num个元素的a里
{
	for(int i=0;i<num;i++)
		if(a[i]==target)
			return 1;
	return 0;
}

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

static void parser_create_new_var()
{
	string name="_";
	name+=numtostring(tmpvarcode++);
	if(!symbtable_enter(name,"var","integer",0,0))//assert
		cout << "Error:parser_create_new_var_symbtable_enter"<< endl;
}

int parser_constdefinition()//常量定义
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


int parser_constdeclaration()
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
int parser_vardefinition()
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
int parser_vardeclaration()
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

int parser_functionheader()
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

}

int parser_procedureheader()
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
}

int parser_formalparalist(int &para_size)
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

int parser_formalparasection(int &para_size)
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
		lex_getsym();
	}
	return 1;
}

//表达式，假设没有函数调用语句
//整个表达式
int parser_expression()
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
		global_new_quadRuple(opr,src1,src2,ans);
		cout << opr <<'\t'<<src1->name <<'\t'<<src2->name <<'\t'<<ans->name<<'\t'<<endl;
		operand_stack.push(ans);
	}
	return 1;
}
int parser_term(int &if_low_zero)//NECx8里的取反指令
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
		global_new_quadRuple("assign",src1,NULL,ans);
		global_new_quadRuple("neg",NULL,NULL,ans);
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
		global_new_quadRuple(opr,src1,src2,ans);
		operand_stack.push(ans);
	}
	return 1;
}

int parser_factor()
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
		if(operand->kind=="array")//先放弃
		{
			lex_getsym();
			if(lex_sym!="[")
			{
				global_error(12,"array");
				return 0;
			}
			lex_getsym();
			if(!parser_expression())
				return 0;
//			symbItem
			if(lex_sym!="]")
			{
				global_error(13,"array");
				return 0;
			}
//			lex_getsym();
		}
		else//暂时不考虑function
		{
			operand_stack.push(operand);//获得操作数	
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
//		lex_getsym();
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
	}
	else
	{
		global_error(22,lex_sym);
		return 0;
	}
	lex_getsym();
	return 1;
}
