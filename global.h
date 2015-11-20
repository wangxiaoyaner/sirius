#include<stdio.h>
#include<ctype.h>
#include<string.h>
#include<string>
#include<iostream>
#include<stdlib.h>
#include<map>
#include<stack>
#include<sstream>
using namespace std;
#define GLOBAL_MAX_ERROR_NUM 100
#define LEX_W_NUM  32
#define LEX_MAX_LINE 4096//每行最多的字符数

typedef struct symbItem{
	string name;
	string kind;//var,const,array,procedure,function?
	string type;//char integer
	int value;//char 类型怎么办，存ASCII？
	int level;
	int adr;
	int size;//func,proc,array
	int para_ifvar;
	struct symbItem *link;
}symbItem;
typedef struct quadRuple{
	string opr;
	symbItem *src1;
	symbItem *src2;
	symbItem *ans;
	struct quadRuple *link;
}quadRuple;
typedef struct symbTable{
	string name;
	int level;
	symbItem *first_item;
	symbItem *last_item;
	struct symbTable *father;
	struct symbTable *firstchild;
	struct symbTable *lastchild;
	struct symbTable *brother;
}symbTable;

extern symbTable *symbtable_table;
extern symbTable *symbtable_now;
symbItem* symbtable_check(string name);
int symbtable_find_dup(string name);
void symbtable_up_level();
void symbtable_new_level(string name);
int symbtable_enter(string name,string kind,string type,int value,int para_ifvar);
void symbtable_display();
extern stack<symbItem*> global_const_pool;

extern string global_err_message[];

extern int global_lex_line_num;
extern int global_error_num;
void global_error(int err_no,string ident);

extern quadRuple *quadruple_first;
extern quadRuple *quadruple_last;

void global_new_quadRuple(string opr,symbItem *src1,symbItem *src2,symbItem *ans);
void global_quadruple_display();
extern const string lex_words[];//直接判断，类型以lex_token命名。

extern FILE *sourcefile,*out;
extern string lex_token,lex_sym;
extern int single_quote_mark;
extern int lex_value,lex_line_no,lex_char_i,lex_mark;
extern char lex_ch;
extern char lex_line[];
void lex_getch();
int lex_getsym();


int parser_functionheader();
int parser_constdefinition();
int parser_constdeclaration();
int parser_vardefinition();
int parser_vardeclaration();
int parser_procedureheader();
int parser_formalparalist(int &para_size);
int parser_formalparasection(int &para_size);
int parser_expression();
int parser_term(int &if_low_zero);
int parser_factor();
