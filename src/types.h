///////////////////////////////////////////////////////////////////////////////
//
//  Falcon Interpreter
//
//  for Formal Languages and Compilers 2012/2013
//  by Varga Tomas, Vojvoda Jakub, Nesvadba Tomas, Spanko Jaroslav, Warzel Adam
//
///////////////////////////////////////////////////////////////////////////////

#ifndef TYPES_H
#define TYPES_H

#include <stdint.h> // uint8_t, uint32_t,  ...
#include <stdio.h> // FILE

// T_String size exponent
#define TSTRING_SIZE 8
// T_Stack item space exponent
#define TSTACK_SIZE 100

// Numeric type identification
#define NID_CONST -2.0
#define NID_UNDEF -1.0
#define NID_NIL 0.0
#define NID_BOOL 1.0
#define NID_NUMBER 3.0
#define NID_FUNCTION 6.0
#define NID_STRING 8.0

#define OP_EQUAL 100.0
#define OP_UNEQUAL 101.0
#define OP_SMEQUAL 102.0
#define OP_BEQUAL 103.0
#define OP_BIGGER 104.0
#define OP_SMALLER 105.0
#define OP_PLUS 106.0
#define OP_MINUS 107.0
#define OP_STAR 108.0
#define OP_DSTAR 109.0
#define OP_SLASH 110.0
#define OP_FUNC 111.0
#define OP_ASSIGN 112.0

#define HASH_TABLE_SIZE 101  // size of Hash Table

typedef struct {
   uint32_t length;
   char *data;
} T_String;

typedef struct T_ArgumentItem {
   struct T_ArgumentItem *next;
   T_String data;
} T_ArgItem;

typedef struct {
   T_ArgItem *first;
   T_ArgItem *last;
   T_ArgItem *actual;
   int size;
} T_ArgList;

typedef struct {
   union {
      uint8_t b_val; // Bool value
      double d_val; // Double value
      T_String str;
      T_ArgList args;
   } vals;
   double NID; // Numeric ID of a type/operator
} T_Var;

typedef struct {
   uint32_t size; // Number of items on the stack
   uint32_t index; // Index behind stack pointer
   T_Var *pointer; // Pointer to current stack slice
   T_Var **item; // Pointer to the array of items
} T_Stack;

typedef struct {
   int IID; // Numeric ID of an instruction
   T_Var *arg1; // Arguments
   T_Var *arg2;
   T_Var *arg3;
} T_Instr;

typedef struct T_Item {
  struct T_Item *next;
  T_Instr *data;
} T_ListItem;

typedef struct {
   T_ListItem *first;
   T_ListItem *last;
   T_ListItem *actual;
} T_List;

typedef struct T_SemanTree {
	int operation;
	struct T_SemanTree *left;
	struct T_SemanTree *right;
} T_Tree;

typedef struct T_HashItem {
  T_String *string;
  T_Var *var;
  struct T_HashItem *next;
} T_HashItem;

typedef struct T_LabelItem {
  struct T_LabelItem *next;
  T_ListItem **ptr_label;
} T_LabelItem;

typedef struct {
   T_LabelItem *first;
   T_LabelItem *last;
} T_LabelList;

typedef T_HashItem *T_HashTable[HASH_TABLE_SIZE];

typedef struct T_IdentificatorItem {
   struct T_IdentificatorItem *previous;
   struct T_IdentificatorItem *next;
   T_HashTable *data;
} T_IdItem;

typedef struct {
   T_IdItem *first;
   T_IdItem *last;
   T_IdItem *actual;
} T_IdList;

typedef struct T_AlocItem {
  void *ptr;
  struct T_AlocItem *next;
} T_AllocItem;

typedef struct {
  T_AllocItem *first;
  T_AllocItem *last;
} T_AllocList;

int T_LabelListInit(T_LabelList *list);
void T_LabelListDelete(T_LabelList *list);
int T_LabelListAppend(T_LabelList *list, T_ListItem *label);

void T_InstrPrint(T_Instr *instr);
void T_VarPrint(T_Var *var);
void T_ArgListPrint(T_ArgList *list);
void T_AllocListPrint(T_AllocList *list);

int T_VarInit(T_Var *var);
void T_VarDelete(T_Var *var);
int T_VarCopy(T_Var *write_var, T_Var *read_var);
int CDoubleToT_Var(double val, T_Var *var);
int T_StringToT_Var(T_String *str, T_Var *var);
int IsFunction(T_Var *var);
void T_VarPrint(T_Var *var);

int T_StringInit(T_String *str);
void T_StringDelete(T_String *str);
uint32_t T_StringRead(FILE *fd, T_String *str, char end);
int T_StringAppend(T_String *str, int c);
int T_StringCat(T_String *dest, T_String *src);
int T_StringCopy(T_String *dest, T_String *src);
int CStringToT_String(char *c_str, T_String *str);
int CutString(T_String *substring, T_String *string, double from, double to);


int T_StackInit(T_Stack *stack);
int T_StackDelete(T_Stack *stack);
int T_StackPush(T_Stack *stack, T_Var *item);
int T_StackPop(T_Stack *stack, T_Var *item);
int T_StackExtend(T_Stack *stack);

int T_ListInit(T_List *list);
void T_ListDelete(T_List *list);
int T_ListSucc(T_List *list);
int T_ListAppend(T_List *list, T_Instr *instr);
void T_ListPrint(T_List *list);

int T_ArgListInit(T_ArgList *list);
void T_ArgListDelete(T_ArgList *list);
int T_ArgListSucc(T_ArgList *list);
int T_ArgListAppend(T_ArgList *list, T_String *instr);
void T_ArgListPrint(T_ArgList *list);

int T_TreeInit(T_Tree *tree);
void T_TreeDelete(T_Tree *tree);
int T_TreeSetOperation(T_Tree *tree, int new_op);
int T_TreeSetLeft(T_Tree *tree, T_Tree *item);
int T_TreeSetRight(T_Tree *tree, T_Tree *item);

int T_IdListInit(T_IdList *list);
void T_IdListDelete(T_IdList *list);
int T_IdListAppend(T_IdList *list, T_HashTable *str);
void T_IdListDeleteLast(T_IdList *list);

int T_AllocListInit(T_AllocList *list);
int T_AllocListAppend(T_AllocList *list, void *item);
void T_AllocListDeleteLast(T_AllocList *list);
void T_AllocListDelete(T_AllocList *list);

#endif
