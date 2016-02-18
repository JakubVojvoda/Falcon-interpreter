///////////////////////////////////////////////////////////////////////////////
//
//  Falcon Interpreter
//
//  for Formal Languages and Compilers 2012/2013
//  by Varga Tomas, Vojvoda Jakub, Nesvadba Tomas, Spanko Jaroslav, Warzel Adam
//
///////////////////////////////////////////////////////////////////////////////

#ifndef BUILTIN_H
#define BUILTIN_H

#include "types.h"

int Input(T_Var *var);
int Print(T_Var *read_var, T_Var *write_var);
int Numeric(T_Var *read_var, T_Var *write_var);
int TypeOf(T_Var *read_var, T_Var *write_var);
int Len(T_Var *read_var, T_Var *write_var);

#endif
