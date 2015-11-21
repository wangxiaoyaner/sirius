#include"global.h"

stack<symbItem*> global_const_pool;
string global_err_message[]={"",//0
	"Illegal char declaration",//1
	"Missing single quotes",//2
	"Illegal string declaration",//3
	"Illegal character",//4
	"Duplicate identifier",//5
	"\"=\"expected",//6
	"\"=\"expected but \":=\" found",//7
	"const should be integer or char",//8
	"digit should be in the back of \"+\"or\"-\"",//9
	"\"begin\" expected",//10
	"\";\" expected",//11
	"\"[\" expected",//12
	"\"]\" expected",//13
	"\"end\" expected",//14
	"\"of\" expected",//15
	"type expected",//16
	"identifier expected",//17
	"\":\" expected",//18
	"\"(\" expected",//19
	"\")\" expected",//20
	"Identifier not declared in this scope",//21
	"Illegal factor",//22
    "Illegal statement",//23
    "\"then\" expected",//24
    "change const.",//25
    "Array access violation",//26
    "\":=\"expected but \"=\" found",//27
    "relational operators expected",//28
    "\"while\" expected",//29
    "Parameter transfer error",//30
    "\"\"\"expected",//31
    "Read object error",//32
    "\"downto\" or \"to\" expected",//33
    "\"do\" expected",//34
    "\",\" expected",//35
	"\":=\" expected"//36
};



int global_lex_line_num=0;

int global_error_num=1;

int lasterrorline=-1;
void global_error(int err_no,string ident)
{	
	if(lasterrorline!=global_lex_line_num)
		cout << "error "<< global_error_num++ <<" : line[" << global_lex_line_num << "]   " << global_err_message[err_no] <<" "<<ident<<"\n";
	lasterrorline=global_lex_line_num;
}
//链表形式的四元式，先做一下实验
quadruple *quadruple_first=NULL,*quadruple_last=NULL;

void global_new_quadruple(string opr,symbItem *src1,symbItem *src2,symbItem *ans)
{	
	quadruple *newitem;
	newitem=new quadruple();
	newitem->opr=opr;
	newitem->src1=src1;
	newitem->src2=src2;
	newitem->ans=ans;
	newitem->link=NULL;
	if(quadruple_first==NULL)
	{
		quadruple_first=newitem;
		quadruple_last=newitem;
	}
	else
	{
		quadruple_last->link=newitem;
		quadruple_last=quadruple_last->link;
	}
}
void global_quadruple_display()
{
	quadruple *tmp=quadruple_first;
	while(tmp!=NULL)
	{
		cout << tmp->opr <<'\t';
		if(tmp->src1)
			cout<<tmp->src1->name<<'\t';
		if(tmp->src2)
			cout<<tmp->src2->name<<'\t';
		if(tmp->ans)
			cout<<tmp->ans->name<<endl;
		tmp=tmp->link;
	}
}
void global_const_pool_del()
{
	symbItem *tmp;
	while(!global_const_pool.empty())
	{
		tmp=global_const_pool.top();
		global_const_pool.pop();
		delete tmp;
	}
}
map<string,string> global_anti_ralation;
map<string,string> global_ralation;
global_anti_ralation[">"]="jle";
global_anti_ralation[">="]="jl";
global_anti_ralation["<="]="jg";
global_anti_ralation["<"]="jge";
global_anti_ralation["="]="jne";
global_anti_ralation["<>"]="je";

global_ralation[">"]="jg";
global_ralation[">="]="jge";
global_ralation["<="]="jle";
global_ralation["<"]="jl";
global_ralation["="]="je";
global_ralation["<>"]="jne";
