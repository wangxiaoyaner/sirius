#include"global.h"
static int if_array=0;
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
		global_error(10,"");//常量定义开头是标识符
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
		global_error(10,"");//变量定义开头是标识符
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
		cout <<"no"<<lex_sym<< endl;
		if(!parser_formalparasection(para_size))
		{		
			parser_test(testset,3);
			if(lex_sym=="")
			{
				global_error(20,"");
				return 0;
			}
		}
		cout << "I am here" << lex_sym << endl;
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
		/*if(lex_sym!=";")
		{
			global_error(11,"348");
			return 0;
		}
		lex_getsym();*/
		cout << "I am her:"<<lex_sym<<endl;
	}
	return 1;
}


