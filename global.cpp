#include"global.h"

stack<symbItem*> global_const_pool;
int global_lex_line_num=0;
int global_lex_lie_num=0;
int global_error_num=1;

void global_error(string right,string wrong)
{	
	cout << "error "<< global_error_num++ <<" : line[" << global_lex_line_num<<"," <<global_lex_lie_num<< "]   \"" << right <<"\" expected, but \""<<wrong<<"\" found\t\n";
}
void global_error(string words)
{
		cout << "error "<< global_error_num++ <<" : line[" << global_lex_line_num <<","<<global_lex_lie_num<< "]   " << words<<"\n";
}
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
			cout<<tmp->ans->name;
		cout << endl;
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
void global_init(){
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
}
