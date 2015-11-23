#include"global.h"
symbTable *symbtable_table=NULL;
symbTable *symbtable_now=NULL;
static symbTable *alltable[100][100];
static int alltable_j[100]={0};
int symbtable_level=-1;
symbItem* symbtable_check(string name)
{
	symbTable *tmp=symbtable_now;
	symbItem *i;
	while(tmp!=NULL)
	{
		i=tmp->first_item;
		while(i!=NULL)
		{
			if(i->name==name)
				return i;
			i=i->link;
		}
		tmp=tmp->father;
	}
	return NULL;
}
//require:symbtable_now is not null;

int symbtable_find_dup(string name)
{
	symbItem *tmp=symbtable_now->first_item;
	while(tmp!=NULL)
	{
		if(tmp->name==name)
			return 1;
		tmp=tmp->link;
	}
	return 0;
}
void symbtable_up_level()
{
	//需要作参数交换.
	int i=0;//标明第几个参数
	symbItem *parafirst,*paralast;
//参数交换过程
	symbTable *father=symbtable_now->father;
	parafirst=symbtable_now->first_item;
	if(parafirst!=NULL && parafirst->kind=="parameter")
	{	
		father->last_item->link=parafirst;
		while(father->last_item->link!=NULL && father->last_item->link->kind=="parameter")
		{
			symbtable_now->first_item=symbtable_now->first_item->link;
			father->last_item=father->last_item->link;
			father->last_item->level--;//参数建一层
			father->last_item->adr=++i;
		}
		father->last_item->link=NULL;	
	}

	if(symbtable_now!=NULL)//assert
		symbtable_now=symbtable_now->father;
	symbtable_level--;
}
void symbtable_new_level(string name)
{
	symbTable *childTable;
	childTable=new symbTable();
	childTable->name=name;
	childTable->level=++symbtable_level;
	childTable->first_item=NULL;
	childTable->last_item=NULL;
	childTable->firstchild=NULL;
	childTable->lastchild=NULL;
	childTable->brother=NULL;
	if(symbtable_table==NULL)
	{
		childTable->father=NULL;
		symbtable_table=childTable;	
		symbtable_now=childTable;//初始化要做好
	}
	else
		childTable->father=symbtable_now;
	if(symbtable_now->firstchild==NULL)
	{
		symbtable_now->firstchild=childTable;
		symbtable_now->lastchild=childTable;
	}
	else
	{
		symbtable_now->lastchild->brother=childTable;
		symbtable_now->lastchild=symbtable_now->lastchild->brother;
	}
	symbtable_now=childTable;
	alltable[symbtable_level][alltable_j[symbtable_level]++]=childTable;
}

int symbtable_if_can_change_func(string name)
{
	symbTable *tmp=symbtable_now;
	while(tmp!=NULL)
	{
		if(tmp->name==name)
			return 1;
		tmp=tmp->father;
	}
	return 0;
}

int symbtable_enter(string name,string kind,string type,int value,int para_ifvar)
{
	if(symbtable_find_dup(name))
	{
		global_error("Duplicate identifier: \""+name+"\".");
		return 0;
	}
	symbItem *new_item=new symbItem();
	new_item->name=name;
	new_item->kind=kind;
	new_item->type=type;
	new_item->value=value;
	new_item->level=symbtable_level;//别丢了呀
	new_item->para_ifvar=para_ifvar;
	new_item->link=NULL;
	if(symbtable_now->first_item==NULL)
	{
		symbtable_now->first_item=new_item;
		symbtable_now->last_item=new_item;
	}
	else
	{
		symbtable_now->last_item->link=new_item;
		symbtable_now->last_item=symbtable_now->last_item->link;
	}
	return 1;
}
static void symbtable_print_single(symbTable *itable)
{
	symbItem *tmp=itable->first_item;
	while(tmp!=NULL)
	{
		cout <<  tmp->name << "\t" << tmp->level 
			<< "\t"<< tmp->kind << "\t"<< tmp->type 
			<<"\t"<< tmp->size << "\t"<<tmp->para_ifvar<<"\t"<<tmp->value<<endl;
		
		tmp=tmp->link;
	}

}

void symbtable_display()
{	
	cout << "name\tlevel\tkind\ttype\tsize\tpara_ifvar\tvalue\n";

	for(int i=0;i<90;i++)
	{
		for(int j=0;j<alltable_j[i];j++)
		{
			cout << "NAME:"<<alltable[i][j]->name<<endl;
			symbtable_print_single(alltable[i][j]);
		}
	}
}
