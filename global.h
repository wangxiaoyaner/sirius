#include<stdio.h>
#include<ctype.h>
#include<string.h>
#include<string>
#include<iostream>
#include<map>
using namespace std;
#define GLOBAL_MAX_ERROR_NUM 100
#define LEX_W_NUM  32
#define LEX_MAX_LINE 4096//每行最多的字符数
#define SYMBTABLE_MAX_NUM 10000

typedef struct symbtable{
	string name;
	string kind;//var,const,array,procedure,function?
	string type;//char integer
	int value;//char 类型怎么办，存ASCII？
	int level;
	int adr;
	int size;//func,proc,array
	int para_ifvar;
}symbtable;

extern string global_err_message[];

extern int global_lex_line_num;
extern int global_error_num;
void global_error(int err_no,string ident);

extern const string lex_words[];//直接判断，类型以lex_token命名。

extern FILE *sourcefile,*out;
extern string lex_token,lex_sym;
extern int single_quote_mark;
extern int lex_value,lex_line_no,lex_char_i,lex_mark;
extern char lex_ch;
extern char lex_line[];
void lex_getch();
int lex_getsym();

extern int symbtable_adr;
extern symbtable symbtable_table[];
extern int symbtable_i;
int symbtable_enter(string name,string kind,string type,int value,int level,int size,int adr,int para_ifvar);
void symbtable_display();//const,var,array,function,procedure,parameter
void symbtable_delete(int level);


int parser_constdefinition(int level);
int parser_constdeclaration(int level);
int parser_vardefinition(int level);
int parser_vardeclaration(int level);
