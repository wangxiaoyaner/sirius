#include"global.h"
int symbtable_adr=0;

symbtable symbtable_table[SYMBTABLE_MAX_NUM];

int symbtable_i=0;

int symbtable_find_dup(string name,int level)
{
	for(int i=symbtable_i-1;i>=0;i--)
	{
		if(symbtable_table[i].level!=level)
			return -1;
		if(symbtable_table[i].name==name)
			return i;
	}
	return -1;
}

int symbtable_enter(string name,string kind,string type,int value,int level,int size,int adr,int para_ifvar)
{
	if(symbtable_find_dup(name,level)>-1)
	{
		global_error(5,name);
		return 0;
	}
	symbtable_table[symbtable_i].name=name;
	symbtable_table[symbtable_i].kind=kind;
	symbtable_table[symbtable_i].type=type;
	symbtable_table[symbtable_i].value=value;
	symbtable_table[symbtable_i].level=level;
	symbtable_table[symbtable_i].size=size;
	if(kind=="var")
		symbtable_table[symbtable_i].adr=symbtable_adr++;
	else if(kind=="array")
	{
		symbtable_table[symbtable_i].adr=symbtable_adr;
		symbtable_adr+=size;
	}
	else if(kind=="function"||kind=="procedure")
	{
		symbtable_table[symbtable_i].adr=symbtable_adr++;
		symbtable_adr+=size;
	}
	else if(kind=="parameter")
	{
		symbtable_table[symbtable_i].adr=adr;
		symbtable_table[symbtable_i].para_ifvar=para_ifvar;
	}
	symbtable_i++;
	return 1;
}

void symbtable_delete(int level)
{
	int i;
	for(i=symbtable_i;i>=0;i--)
	{
		if(symbtable_table[i].level!=level||symbtable_table[i].kind=="parameter")
			break;
	}
	symbtable_i=i+1;
	for(;symbtable_table[i].level==level&&i>=0;i--)
	{
		symbtable_table[i].level--;//参数的层次改变
		symbtable_table[i].name="";//名字没有了
	}
}

void symbtable_display()//const,var,array,function,procedure,parameter
{
	int i;
	symbtable *tmp;
	cout<<"level\t"<<"name\t"<<"kind\t"<<"type\t"<<"value\t"<<"adr\t"<<"size\t"<<"para_ifvar\t"<<endl;
	for(i=0;i<symbtable_i;i++)
	{
		tmp=&symbtable_table[i];
		cout<<tmp->level<<'\t'<<tmp->name<<'\t'<<tmp->kind<<'\t';
		if(tmp->kind=="procedure")
			cout<<"";
		else
			cout<<tmp->type;
		cout<<"\t";
		if(tmp->kind=="const")
		{
			if(tmp->type=="char")
				cout << (char)tmp->value;
			else
				cout << tmp->value;
			cout <<'\t'<<"";
		}
		else
			cout << "\t"<<tmp->adr;
		cout <<'\t';
		if(tmp->kind=="array"||tmp->kind=="procedure"||tmp->kind=="function")
			cout << tmp->size;
		cout <<"\t";
		if(tmp->kind=="parameter")
			cout <<tmp->para_ifvar;
		cout << "\t"<<endl;
	}
}
