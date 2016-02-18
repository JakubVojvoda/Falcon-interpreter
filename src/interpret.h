///////////////////////////////////////////////////////////////////////////////
//
//  Falcon Interpreter
//
//  for Formal Languages and Compilers 2012/2013
//  by Varga Tomas, Vojvoda Jakub, Nesvadba Tomas, Spanko Jaroslav, Warzel Adam
//
///////////////////////////////////////////////////////////////////////////////

#ifndef INTERPRET_H
#define INTERPRET_H

#include "types.h"

enum { // Numeric instruction identification
   IID_NOP, // No operation                                       0
   IID_COPY, // Copy from r-value to l-value                      1

   IID_LABEL,//                                                   2
   IID_JUMP, // Jump to designated label                          3
   IID_BRATRUE, // Branch if true                                 4
   IID_BRAFAL, // Branch if false                                 5
   IID_BRANTRUE, // Branch if not true -- equal to BRAFAL         6
   IID_BRANFAL, // Branch if not false -- equal to BRATRUE        7

   IID_PLUS, // Addition                                          8
   IID_DIFF, // Difference                                        9
   IID_MUL, // Multiplication                                     10
   IID_DIV, // Divion                                             11
   IID_PWR, // Power                                              12
   IID_CUT, // Cut string                                         13

   IID_EQUAL, // ==                                               14
   IID_UNEQUAL, // !=                                             15
   IID_BEQUAL,  // >=                                             16
   IID_SMEQUAL, // <=                                             17
   IID_SMALLER, // <                                              18
   IID_BIGGER,  // >                                              19

   // Built-in functions
   IID_INPUT, //                                                  20
   IID_NUMERIC, //                                                21
   IID_PRINT, //                                                  22
   IID_TYPEOF, //                                                 23
   IID_LEN, //                                                    24
   IID_FIND, //                                                   25
   IID_SORT //                                                    26
};

int T_InstrInit(T_Instr *instr);
void T_InstrDelete(T_Instr *instr);
int T_InstrSet(T_Instr *instr, int IID, T_Var *arg1, T_Var *arg2, T_Var *arg3);

int Interpret(T_List *list, T_LabelList *lbl_list);

//int T_StringFill(T_String *str, T_Var var);
//void OperationPrint(int op);
//int RelType(T_Var arg1, T_Var arg2);

#endif
