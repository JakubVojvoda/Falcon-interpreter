///////////////////////////////////////////////////////////////////////////////
//
//  Falcon Interpreter
//
//  for Formal Languages and Compilers 2012/2013
//  by Varga Tomas, Vojvoda Jakub, Nesvadba Tomas, Spanko Jaroslav, Warzel Adam
//
///////////////////////////////////////////////////////////////////////////////

#ifndef IAL_H
#define IAL_H

#define MAX_CHARS 256
#define X 89478485  // const. - keyCreate
#define HASH_NOT_FOUND -1
#define HASH_FOUND 0

#include "types.h"

int Find(T_Var *str, T_Var *sub_str, T_Var *write_var);

int Sort(T_Var *read_str, T_Var *write_str);

// Hash Table
int T_KeyCreate (char *string);
int T_HTableInit(T_HashTable *table);
void T_HTableDelete (T_HashTable *table);

T_HashItem *T_HTableSearch(T_HashTable *table, char *string);
int T_HTableInsert(T_HashTable *table, char *string, T_Var *var);
int T_HTableExport(T_HashTable *table, char *string ,T_Var **var);
void T_HashTablePrint(T_HashTable *table);

#endif
