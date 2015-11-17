#include"global.h"
static int parser_ifintestset(string a[],int num,string target)
{
	for(int i=0;i<num;i++)
		if(a[i]==target)
			return 1;
	return 0;
}

static void parser_test(string a[],int num)
{
	while(!parser_ifintestset(a,num,lex_sym))
			lex_getsym();
}

int parser_constdefinition(int level)
{
	if(lex_sym=="ident")
	{
		string constname=lex_token;
		lex_getsym();
		if(lex_sym=="="||lex_sym==":=")//简单矫正
		{
			if(lex_sym==":=")
			{
				global_error(7,"");
			}
			lex_getsym();

			if(lex_sym=="char")
			{
				if(!symbtable_enter(constname,"const","char",lex_value,level,0,0,0))
					return 0;
				lex_getsym();
			}
			else if(lex_sym=="+"||lex_sym=="-")
			{
				int mark=lex_sym=="-"?-1:1;
				lex_getsym();
				if(lex_sym=="digit")
				{
					if(!symbtable_enter(constname,"const","integer",lex_value*mark,level,0,0,0))
						return 0;
					lex_getsym();
				}
				else
				{
					global_error(9,"");
					return 0;
				}
			}
			else if(lex_sym=="digit")
			{
				if(!symbtable_enter(constname,"const","integer",lex_value,level,0,0,0))
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
			global_error(6,"");
			return 0;
		}
	}
	else
	{
		global_error(10,"");
		return 0;
	}
	return 1;
}


int parser_constdeclaration(int level)
{
	string testset[]={",",";",""};
	if(!parser_constdefinition(level))//去找他的合法后继们
	{
		parser_test(testset,3);
		if(lex_sym=="")
		{
			global_error(11,"");
			return 0;
		}
	}//读到了合法的后继之后就应该继续往下干。
	while(lex_sym==",")
	{
		lex_getsym();
		if(!parser_constdefinition(level))
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
int parser_vardefinition(int level)
{
	int start,size=0,if_array=0;
	string consttype;
	if(lex_sym=="ident")
	{
		//constname=lex_token;
		start=symbtable_i;
		if(!symbtable_enter(lex_token,"var","",0,level,0,0,0))
			return 0;
		lex_getsym();
		while(lex_sym==",")
		{
			lex_getsym();
			if(!symbtable_enter(lex_token,"var","",0,level,0,0,0))
				return 0;
			lex_getsym();
		}
		if(lex_sym!=":")
			global_error(11,"");
		lex_getsym();
		if(lex_sym=="integer"||"char")
			consttype=lex_sym;
		else if(lex_sym=="array")
		{
			if_array=1;
			lex_getsym();
			if(lex_sym!="[")
			{
				global_error(12,"");
			//	return 0;
			}
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
				//return 0;
			}
			lex_getsym();
			if(lex_sym!="of")
			{
				global_error(15,"");
				//return 0;
			}
			lex_getsym();
			if(lex_sym=="integer"||lex_sym=="char")
				consttype=lex_sym;
			else
			{
				global_error(16,"");
				return 0;
			}
		}
		else
		{
			global_error(16,"");
			return 0;
		}
		for(int i=start;i<symbtable_i;i++)//反填
		{
			symbtable_table[i].type=consttype;
			symbtable_table[i].size=size;
			if(if_array)
				symbtable_table[i].kind="array";
		}
		lex_getsym();
	}
	else
	{
		global_error(10,"");
		return 0;
	}
	return 1;
}
int parser_vardeclaration(int level)
{
	string testset[]={"",";"};
	if(!parser_vardefinition(level))
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
	while(lex_sym=="ident")
	{
		if(!parser_vardefinition(level))
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





