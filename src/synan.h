///////////////////////////////////////////////////////////////////////////////
//
//  Falcon Interpreter
//
//  for Formal Languages and Compilers 2012/2013
//  by Varga Tomas, Vojvoda Jakub, Nesvadba Tomas, Spanko Jaroslav, Warzel Adam
//
///////////////////////////////////////////////////////////////////////////////

#ifndef SYNAN_H
#define SYNAN_H

#include "types.h"

#define TRUE 1
#define FALSE 0

#define OPERACE 10
#define HODNOTA 20

#define MYSHORTCUT do{	\
	pom->zavorka+=pom->next->zavorka;\
	pom->operace=pom->next->operace;\
	del=pom->next;\
	pom->next=del->next;\
	if (del==*last) *last=pom;\
	free(del);\
	} while (0)

#define GENERSHORT do{ \
         pom=malloc(sizeof(struct Exlist));\
	 PTR_CHECK(pom);\
         pom->value=var;\
         pom->operace=TOKEN_EOL;\
         pom->zavorka=par; par=0;\
         pom->next=NULL;\
         if (*L==NULL) {	last=pom;	*L=pom;     }\
         else     {	        last->next=pom;	last=pom;   }	\
	 }while (0)

/*
<synan> 	   -> <stat> <synan>
<synan>		-> EOF
<stat>		-> <keyword>
<stat>		-> id = <expr>
<stat>		-> func_id(<item> <it-list>
<keyword>	-> if <expr> <if>
<keyword>	-> while <expr> <end>
<keyword>	-> function id(id <it-list>
<keyword>   -> function id() EOL
<expr>		-> <item> + id|number|string|func_id(<item> <it-list>
<expr>      -> <item> -|*|/|** id|number|func_id(<item> <it-list>
<expr>		-> <item> EOL
<expr>      -> ( <expr>
<expr>      -> ) + |id|number|string|func_id(<item>|id <it-list>
<expr>      -> ) -|*|/|** id|number|func_id(<item>|id <it-list>
<expr>      -> ) EOL
<item>		-> id
<item>		-> number
<item>		-> bool
<item>	   -> string
<item>		-> nil
<it-list>	-> , <item> <it-list>
<it-list>	-> ) EOL
<if>		-> <stat2> <if>
<if>		-> else <end>
<if>		-> end EOL
<end>		-> <stat2> <end>
<end>		-> end EOL
*/

int SynanInit(T_List *inst_list, T_AllocList *alloc_list, T_LabelList *label_list);
void SynanFinish();

int Synan();

#endif
