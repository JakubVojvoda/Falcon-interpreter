///////////////////////////////////////////////////////////////////////////////
//
//  Falcon Interpreter
//
//  for Formal Languages and Compilers 2012/2013
//  by Varga Tomas, Vojvoda Jakub, Nesvadba Tomas, Spanko Jaroslav, Warzel Adam
//
///////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>

#include "synan.h"
#include "lex.h"
#include "ial.h"
#include "codes.h"
#include "types.h"
#include "interpret.h"
#include "macros.h"
#include "functions.h"
#include "debug.h"

// Modes indicating whether currently processed line is in/out-side a code block
#define MODE_OUTSIDE 1
#define MODE_INSIDE 2

#define GenerateInstruction(type, arg1, arg2, arg3) \
   T_InstrSet(&instr, type, arg1, arg2, arg3); \
   T_ListAppend(inst_list, &instr)

#define BRACKET_CHECK do {\
   if (lex_rc != TOKEN_RBRACKET) { \
      errno = SYNTACTIC_ERROR; \
      return EXIT_FAILURE; \
   } \
} while (0)

#define TOKEN_CHECK \
   if (lex_rc == TOKEN_BROKEN || lex_rc == TOKEN_PROBLEM) return EXIT_FAILURE;

T_HashTable *kw_table;
T_HashTable *builtin_table;
T_HashTable *id_table;

T_List *inst_list;
T_AllocList *alloc_list;
T_IdList *table_list;
T_LabelList *lbl_list;
T_LabelList *cal_list;

T_Var *read_var;
T_Var *write_var;

T_Instr instr;
T_String token;

int lex_rc;
int parentheses;
int params_count;
int tmp_count;

int stat(int mode);
int keyword(int mode);
int expr();
int item();
int it_list(T_ArgItem *item_arg);
int print_list();
int if_state(int mode, T_Var *jmp);
int while_state(T_Var *start_lbl, T_Var *end_lbl);
int op();

int SynanInit(T_List *instr_list, T_AllocList *al_list, T_LabelList *label_list)
{
   DEBUG_SYNAN_INIT;

   write_var = NULL;
   read_var = NULL;

   kw_table = malloc(sizeof(T_HashTable));
   builtin_table = malloc(sizeof(T_HashTable));
   id_table = malloc(sizeof(T_HashTable));
   table_list = malloc(sizeof(T_IdList));

   T_Var *var = malloc(sizeof(T_Var));

   if (kw_table == NULL || builtin_table == NULL || id_table == NULL || table_list == NULL || var == NULL || cal_list) {
      free(kw_table);
      free(builtin_table);
      free(id_table);
      free(table_list);
      free(var);
      free(cal_list);
   }

   T_LabelListInit(cal_list);

   // Saves reserved keywords into an identificator table as constants.
   T_VarInit(var);
   var->NID = NID_CONST;

   char *reserved[] = {"as", "def", "directive", "export", "from", "import", "launch", "load", "macro", "input", "print", "numeric", "typeOf", "len", ""};
   T_HTableInit(id_table);
   for (int i = 0; strcmp(reserved[i], "") != 0; i++)
      T_HTableInsert(id_table, reserved[i], var);

   var->NID = NID_FUNCTION;
   var->vals.args.size = 0;

   char *builtin[] = {"input", "print", "numeric", "typeOf", "len", ""};
   T_HTableInit(builtin_table);
   for (int i = 0; strcmp(builtin[i], "") != 0; i++)
      T_HTableInsert(builtin_table, builtin[i], var);

   var->NID = NID_UNDEF;
   T_HTableInsert(id_table, "0ret", var);

   T_VarDelete(var);
   free(var);

   // Saves usable keywords
   char *keywords[] = {"else", "end", "false", "function", "if", "nil", "return", "true", "while", ""};
   T_HTableInit(kw_table);
   for (int i = 0; strcmp(keywords[i], "") != 0; i++)
      T_HTableInsert(kw_table, keywords[i], NULL);

   // Initializing common variables
   inst_list = instr_list;
   alloc_list = al_list;
   lbl_list = label_list;

   T_InstrInit(&instr);
   T_StringInit(&token);

   DEBUG_SYNAN_INIT_END;
   return EXIT_SUCCESS;
}

void SynanFinish()
{
   DEBUG_SYNAN_FINISH;

   T_HTableDelete(kw_table);
   T_HTableDelete(id_table);
   T_LabelListDelete(cal_list);
   free(kw_table);
   free(id_table);

   DEBUG_SYNAN_FINISH_END;
}

int GenerateVariable(int label, T_Var **var)
{
   DEBUG_VAR_GEN(var);
   char *var_name = malloc(sizeof(char));
   T_Var *tmp_var = malloc(sizeof(T_Var));

   if (var_name == NULL || tmp_var == NULL) {
      free(var_name);
      free(tmp_var);
      errno = INTERNAL_ERROR;
      return EXIT_FAILURE;
   }

   T_VarInit(tmp_var);

   if (itoa(tmp_count++, var_name, DECIMAL_BASE) != EXIT_FAILURE) {
      if (label) {
         T_StringInit(&(tmp_var->vals.str));
         CStringToT_String(var_name, &(tmp_var->vals.str));
         tmp_var->NID = NID_STRING;
      }
   }
   if ((T_HTableInsert(id_table, var_name, tmp_var) == EXIT_FAILURE)
      || (T_HTableExport(id_table, var_name, var) == HASH_NOT_FOUND))
   {
      free(var_name);
      free(tmp_var);
      return EXIT_FAILURE;
   }

   free(var_name);
   free(tmp_var);

   DEBUG_VAR_GEN_END(var);
   return EXIT_SUCCESS;
}

int CallBuiltin()
{
   if (strcmp(token.data, "input") == 0) {
      lex_rc = GetToken(&token);
      TOKEN_CHECK;
      if (lex_rc != TOKEN_LBRACKET) {
         errno = SYNTACTIC_ERROR;
         return EXIT_FAILURE;
      }

      lex_rc = GetToken(&token);
      TOKEN_CHECK;
      if (lex_rc != TOKEN_RBRACKET) {
         errno = SYNTACTIC_ERROR;
         return EXIT_FAILURE;
      }

      if (T_HTableExport(id_table, "0ret", &write_var) == HASH_NOT_FOUND) {
         errno = INTERNAL_ERROR;
         return EXIT_FAILURE;
      }
      GenerateInstruction(IID_INPUT, write_var, NULL, NULL);
      return EXIT_SUCCESS;
   }

   else if (strcmp(token.data, "print") == 0) {
      lex_rc = GetToken(&token);
      TOKEN_CHECK;
      if (lex_rc != TOKEN_LBRACKET) {
         errno = SYNTACTIC_ERROR;
         return EXIT_FAILURE;
      }
      if (T_HTableExport(id_table, "0ret", &write_var) == HASH_NOT_FOUND) {
         errno = INTERNAL_ERROR;
         return EXIT_FAILURE;
      }
      lex_rc = GetToken(&token);
      TOKEN_CHECK;
      CALL_CHECK(item());

      GenerateInstruction(IID_PRINT, read_var, write_var, NULL);
      CALL_CHECK(print_list());
      return EXIT_SUCCESS;
   }

   else if (strcmp(token.data, "numeric") == 0) {
      lex_rc = GetToken(&token);
      TOKEN_CHECK;
      if (lex_rc != TOKEN_LBRACKET) {
         errno = SYNTACTIC_ERROR;
         return EXIT_FAILURE;
      }
      if (T_HTableExport(id_table, "0ret", &write_var) == HASH_NOT_FOUND) {
         errno = INTERNAL_ERROR;
         return EXIT_FAILURE;
      }

      lex_rc = GetToken(&token);
      TOKEN_CHECK;
      CALL_CHECK(item());

      GenerateInstruction(IID_NUMERIC, read_var, write_var, NULL);

      lex_rc = GetToken(&token);
      TOKEN_CHECK;
      if (lex_rc != TOKEN_RBRACKET) {
         errno = SYNTACTIC_ERROR;
         return EXIT_FAILURE;
      }
      return EXIT_SUCCESS;
   }
   else if (strcmp(token.data, "typeOf") == 0) {
      lex_rc = GetToken(&token);
      TOKEN_CHECK;
      if (lex_rc != TOKEN_LBRACKET) {
         errno = SYNTACTIC_ERROR;
         return EXIT_FAILURE;
      }
      if (T_HTableExport(id_table, "0ret", &write_var) == HASH_NOT_FOUND) {
         errno = INTERNAL_ERROR;
         return EXIT_FAILURE;
      }

      lex_rc = GetToken(&token);
      TOKEN_CHECK;
      CALL_CHECK(item());

      GenerateInstruction(IID_TYPEOF, read_var, write_var, NULL);

      lex_rc = GetToken(&token);
      TOKEN_CHECK;
      if (lex_rc != TOKEN_RBRACKET) {
         errno = SYNTACTIC_ERROR;
         return EXIT_FAILURE;
      }

      return EXIT_SUCCESS;
   }
   else if (strcmp(token.data, "len") == 0) {
      lex_rc = GetToken(&token);
      TOKEN_CHECK;
      if (lex_rc != TOKEN_LBRACKET) {
         errno = SYNTACTIC_ERROR;
         return EXIT_FAILURE;
      }
      if (T_HTableExport(id_table, "0ret", &write_var) == HASH_NOT_FOUND) {
         errno = INTERNAL_ERROR;
         return EXIT_FAILURE;
      }

      lex_rc = GetToken(&token);
      TOKEN_CHECK;
      CALL_CHECK(item());

      GenerateInstruction(IID_LEN, read_var, write_var, NULL);

      lex_rc = GetToken(&token);
      TOKEN_CHECK;
      if (lex_rc != TOKEN_RBRACKET) {
         errno = SYNTACTIC_ERROR;
         return EXIT_FAILURE;
      }

      return EXIT_SUCCESS;
   }
   else if (strcmp(token.data, "find") == 0) {
      // TODO
   }
   else if (strcmp(token.data, "sort") == 0) {
      lex_rc = GetToken(&token);
      TOKEN_CHECK;
      if (lex_rc != TOKEN_LBRACKET) {
         errno = SYNTACTIC_ERROR;
         return EXIT_FAILURE;
      }
      if (T_HTableExport(id_table, "0ret", &write_var) == HASH_NOT_FOUND) {
         errno = INTERNAL_ERROR;
         return EXIT_FAILURE;
      }

      lex_rc = GetToken(&token);
      TOKEN_CHECK;
      CALL_CHECK(item());

      GenerateInstruction(IID_SORT, read_var, write_var, NULL);

      lex_rc = GetToken(&token);
      TOKEN_CHECK;
      if (lex_rc != TOKEN_RBRACKET) {
         errno = SYNTACTIC_ERROR;
         return EXIT_FAILURE;
      }

      return EXIT_SUCCESS;
   }

   return EXIT_SUCCESS;
}

int CallFunction()
{
   // Saves a name of called function, currently located in token
   T_String f_name;
   T_StringInit(&f_name);
   T_StringCopy(&f_name, &token);

   // Opening bracket must follow
   lex_rc = GetToken(&token);
   TOKEN_CHECK;
   if (lex_rc == TOKEN_LBRACKET) {
      T_HashTable *tmp_table = malloc(sizeof(T_HashTable));
      T_Var *f_var = malloc(sizeof(T_Var));

      if (tmp_table == NULL || f_var == NULL) {
         free(tmp_table);
         free(f_var);
         errno = INTERNAL_ERROR;
         return EXIT_FAILURE;
      }

      T_HTableInit(tmp_table);
      T_VarInit(f_var);

      // Adds current id_table to table list and change its pointer
      T_IdListAppend(table_list, id_table);
      id_table = tmp_table;

      T_Var *var = malloc(sizeof(T_Var));
      PTR_CHECK(var);

      T_VarInit(var);
      var->NID = NID_CONST;

      char *reserved[] = {"as", "def", "directive", "export", "from", "import", "launch", "load", "macro", "input", "print", "numeric", "typeOf", "len", ""};

      T_HTableInit(id_table);

      for (int i = 0; strcmp(reserved[i], "") != 0; i++)
         T_HTableInsert(id_table, reserved[i], var);

      free(var);

      // Gets all function parameters into hash table
      if (write_var->vals.args.size > 0) {
         lex_rc = GetToken(&token);
         TOKEN_CHECK;
         CALL_CHECK(item());
         CALL_CHECK(T_HTableInsert(id_table, write_var->vals.args.first->data.data, read_var));
         CALL_CHECK(it_list(write_var->vals.args.first->next));
      }
      else {
         lex_rc = GetToken(&token);
         TOKEN_CHECK;
         if (lex_rc != TOKEN_RBRACKET) {
            errno = SYNTACTIC_ERROR;
            return EXIT_FAILURE;
         }
      }

      // Calls the function with the saved name
      T_StringToT_Var(&f_name, f_var);
      GenerateInstruction(IID_JUMP, f_var, NULL, NULL);

      // Generates a lable as a return point
      GenerateVariable(TRUE, &write_var);
      GenerateInstruction(IID_LABEL, write_var, NULL, NULL);
      T_LabelListAppend(cal_list, inst_list->last);

      free(f_var);

      return EXIT_SUCCESS;
   }

   errno = SYNTACTIC_ERROR;
   return EXIT_FAILURE;
}

int Synan()
{
   DEBUG_SYNAN;

   lex_rc = GetToken(&token);
   TOKEN_CHECK;
   if (lex_rc == TOKEN_EOF) {
      DEBUG_SYNAN_END;
      return EXIT_SUCCESS;
   }
   else
      CALL_CHECK(stat(MODE_OUTSIDE));

   DEBUG_SYNAN_END;
   return Synan();

   errno = SYNTACTIC_ERROR;
   return EXIT_FAILURE;
}

int stat(int mode)
{
   DEBUG_STAT;
   if (lex_rc == TOKEN_EOL)
      return EXIT_SUCCESS;

   if (lex_rc == TOKEN_IDENTIFICATOR) {
      if (T_HTableSearch(kw_table, token.data) != NULL) { // Keyword
         DEBUG_STAT_KEYWORD;
         return keyword(mode);
      }

      if (T_HTableSearch(builtin_table, token.data) != NULL)
         return CallBuiltin();

      int htable_rc = T_HTableExport(id_table, token.data, &write_var);
      if (htable_rc == HASH_FOUND && write_var->NID == NID_FUNCTION) { // Known function
         DEBUG_STAT_FUNCTION;
         CALL_CHECK(CallFunction());
         DEBUG_STAT_END;
         return EXIT_SUCCESS;
      }
      else { // Variable
         DEBUG_STAT_VAR;
         if (htable_rc == HASH_NOT_FOUND) {
            // Variable doesn't exist, create it as undefined and save into table
            T_Var *tmp_var = malloc(sizeof(T_Var));
            PTR_CHECK(tmp_var);

            T_VarInit(tmp_var);

            if (T_HTableInsert(id_table, token.data, tmp_var) == EXIT_FAILURE) {
               free(tmp_var);
               return EXIT_FAILURE;
            }
            free(tmp_var);
         }

         // Assign write_var pointer to newly created variable
         if (T_HTableExport(id_table, token.data, &write_var) == HASH_NOT_FOUND) {
            errno = INTERNAL_ERROR;
            return EXIT_FAILURE;
         }

         // Next expected token is assignment character
         lex_rc = GetToken(&token);
         TOKEN_CHECK;
         if (lex_rc == TOKEN_ASSIGN) {
            CALL_CHECK(expr());

            // write_var points to variable from the table, read_var points to the expression result
            GenerateInstruction(IID_COPY, write_var, read_var, NULL);

            DEBUG_STAT_END;
            return EXIT_SUCCESS;
         }
      }
   }

   errno = SYNTACTIC_ERROR;
   return EXIT_FAILURE;
}

int keyword(int mode)
{
   DEBUG_KW;

   if (lex_rc == TOKEN_IDENTIFICATOR) {
      if (strcmp(token.data,"if") == 0 ) {
         DEBUG_KW_IF;

         CALL_CHECK(expr());

         GenerateVariable(TRUE, &write_var);
         GenerateInstruction(IID_BRAFAL, read_var, write_var, NULL);

         CALL_CHECK(if_state(MODE_OUTSIDE, write_var));

         DEBUG_KW_END;
         return EXIT_SUCCESS;
      }

      else if (strcmp(token.data,"while") == 0 ) {
         DEBUG_KW_WHILE;

         T_Var *start_lbl;
         GenerateVariable(TRUE, &start_lbl);
         GenerateInstruction(IID_LABEL, start_lbl, NULL, NULL);
         T_LabelListAppend(lbl_list, inst_list->last);

         CALL_CHECK(expr());

         GenerateVariable(TRUE, &write_var);
         GenerateInstruction(IID_BRAFAL, read_var, write_var, NULL);

         CALL_CHECK(while_state(start_lbl, write_var));

         DEBUG_KW_END;
         return EXIT_SUCCESS;
      }

      else if (strcmp(token.data,"function") == 0 && (mode == MODE_OUTSIDE)) {
         DEBUG_KW_FUNCTION;

         lex_rc = GetToken(&token);
//         TOKEN_CHECK;
//         if (lex_rc == TOKEN_IDENTIFICATOR) {
//            lex_rc = GetToken(&token);
//            TOKEN_CHECK;
//            if (lex_rc == TOKEN_LBRACKET) {
//
//               f_item();
//               CALL_CHECK(f_());
//
//               lex_rc = GetToken(&token);
//               TOKEN_CHECK;
//
//               DEBUG_KW_END;
//               return EXIT_SUCCESS;
//            }
//         }
      }
   }

   errno = SYNTACTIC_ERROR;
   return EXIT_FAILURE;
}

int while_state(T_Var *start_lbl, T_Var *end_lbl)
{
   DEBUG_WHILE;

   lex_rc = GetToken(&token);
   TOKEN_CHECK;
   if (lex_rc == TOKEN_IDENTIFICATOR) {
      if (strcmp(token.data, "end") == 0) {
         GenerateInstruction(IID_JUMP, start_lbl, NULL, NULL);
         GenerateInstruction(IID_LABEL, end_lbl, NULL, NULL);
         T_LabelListAppend(lbl_list, inst_list->last);

         return EXIT_SUCCESS;
      }
   }

   CALL_CHECK(stat(MODE_INSIDE));
   CALL_CHECK(while_state(start_lbl, end_lbl));

   DEBUG_WHILE_END;

   return EXIT_SUCCESS;
}

int f_item()
{
   if (TOKEN_IDENTIFICATOR)
      read_var->vals.args.first->data = token;
   
   return EXIT_SUCCESS;
}

int item()
{
   DEBUG_ITEM;

   double tmp;
   switch (lex_rc) {
   case TOKEN_STRING:
      DEBUG_ITEM_STR;

      CALL_CHECK(GenerateVariable(FALSE, &read_var));
      T_StringToT_Var(&token, read_var);

      DEBUG_ITEM_END;
      return EXIT_SUCCESS;
   break;
   case TOKEN_NUMBER:
      DEBUG_ITEM_NUM;

      CALL_CHECK(GenerateVariable(FALSE, &read_var));
      tmp = strtod(token.data, NULL);
      CDoubleToT_Var(tmp, read_var);

      DEBUG_ITEM_END;
      return EXIT_SUCCESS;
   break;

   case TOKEN_IDENTIFICATOR:
      if (strcmp(token.data, "true") == 0) {
         DEBUG_ITEM_TRUE;

         CALL_CHECK(GenerateVariable(FALSE, &read_var));
         read_var->NID = NID_BOOL;
         read_var->vals.b_val = 1;

         DEBUG_ITEM_END;
         return EXIT_SUCCESS;
      }
      else if (strcmp(token.data, "false") == 0) {
         DEBUG_ITEM_FALSE;

         CALL_CHECK(GenerateVariable(FALSE, &read_var));
         read_var->NID = NID_BOOL;
         read_var->vals.b_val = 0;

         DEBUG_ITEM_END;
         return EXIT_SUCCESS;
      }
      else if (strcmp(token.data, "nil") == 0) {
         DEBUG_ITEM_NIL;

         CALL_CHECK(GenerateVariable(FALSE, &read_var));
         read_var->NID = NID_NIL;

         DEBUG_ITEM_END;
         return EXIT_SUCCESS;
      }
      else if (T_HTableExport(id_table, token.data, &read_var) == HASH_FOUND) {
         DEBUG_ITEM_ID;
         return EXIT_SUCCESS;
      }
   break;
   }

   errno = SYNTACTIC_ERROR;
   return EXIT_FAILURE;
}

int it_list(T_ArgItem *item_arg)
{
   DEBUG_IT_LIST;

   lex_rc = GetToken(&token);
   TOKEN_CHECK;

   if (lex_rc == TOKEN_RBRACKET) {
      DEBUG_IT_LIST_END;
      return EXIT_SUCCESS;
   }

   else if (lex_rc == TOKEN_COMMA) {
      lex_rc = GetToken(&token);
      TOKEN_CHECK;
      CALL_CHECK(item());

      if (item_arg == NULL) {
         errno = SYNTACTIC_ERROR;
         return EXIT_FAILURE;
      }
      CALL_CHECK(T_HTableInsert(id_table, item_arg->data.data, read_var));
      CALL_CHECK(it_list(item_arg->next));
   }

   else if (lex_rc == TOKEN_BROKEN || lex_rc == TOKEN_PROBLEM)
      return EXIT_FAILURE;

   else {
      errno = SYNTACTIC_ERROR;
      return EXIT_FAILURE;
   }
   
   return EXIT_SUCCESS;
}

int print_list()
{
   lex_rc = GetToken(&token);
   TOKEN_CHECK;

   if (lex_rc == TOKEN_RBRACKET)
      return EXIT_SUCCESS;

   else if (lex_rc == TOKEN_COMMA) {
      lex_rc = GetToken(&token);
      TOKEN_CHECK;
      CALL_CHECK(item());

      if (T_HTableExport(id_table, "0ret", &write_var) == HASH_NOT_FOUND) {
         errno = INTERNAL_ERROR;
         return EXIT_FAILURE;
      }
      GenerateInstruction(IID_PRINT, read_var, write_var, NULL);
      CALL_CHECK(print_list());
   }

   else if (lex_rc == TOKEN_BROKEN || lex_rc == TOKEN_PROBLEM)
      return EXIT_FAILURE;

   else {
      errno = SYNTACTIC_ERROR;
      return EXIT_FAILURE;
   }
   
   return EXIT_SUCCESS;
}

int if_state(int mode, T_Var *jmp)
{
   DEBUG_IF;

   lex_rc = GetToken(&token);
   TOKEN_CHECK;
   if (lex_rc == TOKEN_IDENTIFICATOR) {
      if (mode == MODE_OUTSIDE && strcmp(token.data, "else") == 0) {
         DEBUG_IF_ELSE;
         // Expression is true -- jump behind else
         GenerateVariable(TRUE, &write_var);
         GenerateInstruction(IID_JUMP, write_var, NULL, NULL);

         // Expression is false -- else follows
         GenerateInstruction(IID_LABEL, jmp, NULL, NULL);
         T_LabelListAppend(lbl_list, inst_list->last);

         return if_state(MODE_INSIDE, write_var);
      }

      else if (strcmp(token.data, "end") == 0) {
         DEBUG_IF_END;
         // Once end is found, places its label
         GenerateInstruction(IID_LABEL, jmp, NULL, NULL);
         T_LabelListAppend(lbl_list, inst_list->last);

         return EXIT_SUCCESS;
      }
   }

   CALL_CHECK(stat(MODE_INSIDE));

   CALL_CHECK(if_state(mode, jmp));

   DEBUG_IF_END;
   return EXIT_SUCCESS;
}

//------------------------------------------------------------------------
//-------RESENI VYRAZU POMOCI VYGENEROVANEHO LISTU -----------------------
//------------------------------------------------------------------------

// struktura drzici hodnoty a ruzne specifika - zaporky a prava operaca
typedef struct Exlist{
   T_Var *value;
   int operace;
   int zavorka;
   struct Exlist *next;
} Expr_list;

// Vyber retezce : zpracovani  [ num/id/() :  num/id/() ] EOL
// String / id je jiz nacteny
int expr_string(T_Var **num1 , T_Var **num2)
{
// [ -------cekam cislo nebo :
   lex_rc = GetToken(&token);
   TOKEN_CHECK;

   if ((lex_rc == NID_NUMBER) || (lex_rc == TOKEN_IDENTIFICATOR)) {
      //1. NUMBER
      if (lex_rc == NID_NUMBER) {
         *num1 = malloc(sizeof(T_Var));
         PTR_CHECK(*num1);
         CDoubleToT_Var(atof(token.data), *num1);
         if (debug)printf("%f" , (*num1)->vals.d_val);
      }

      //2. IDENTIFIKATOR
      if (lex_rc == TOKEN_IDENTIFICATOR) {
         if (T_HTableExport(id_table, token.data, num1) == HASH_NOT_FOUND) {
            if (errno == 0) errno = SYNTACTIC_ERROR;
            if (debug)printf("non exist id");
            return EXIT_FAILURE;
         }
      }

      //------------------------------------------------------------------
      //COMA PO CISLE NEBO ID
      lex_rc = GetToken(&token);
      TOKEN_CHECK;
      if (debug) printf("-----%s----", token.data);
      if (lex_rc != TOKEN_COLUMN) {
         if (errno == 0) errno = SYNTACTIC_ERROR;
         if (debug)printf("cekam colum1");
         return EXIT_FAILURE;
      }
   }

   //colum PO HRANATE ZAVORCE
   else if (lex_rc == TOKEN_COLUMN) { *num1 = NULL; if (debug) printf("prazdno"); }
   else {
      if (errno == 0) errno = SYNTACTIC_ERROR;
      if (debug) printf("cekam colum2");
      return EXIT_FAILURE;
   }
//------------------------------------------------------------------
   lex_rc = GetToken(&token);
   TOKEN_CHECK;
   if ((lex_rc == NID_NUMBER) || (lex_rc == TOKEN_IDENTIFICATOR)) {
      //1. NUMBER 2
      if (lex_rc == NID_NUMBER) {
         *num2 = malloc(sizeof(T_Var));
         PTR_CHECK(*num2);
         CDoubleToT_Var(atof(token.data), *num2);
         if (debug) printf("%f" , (*num2)->vals.d_val);
      }
     //2. IDENTIFIKATOR 2
      if (lex_rc == TOKEN_IDENTIFICATOR) {
         if (T_HTableExport(id_table, token.data, &(*num2)) == HASH_NOT_FOUND) {
            if (errno == 0) errno = SYNTACTIC_ERROR;
            if (debug) printf("non exist id");
            return EXIT_FAILURE;}
     }
     // hranata po cisle nebo id
      lex_rc = GetToken(&token);
      TOKEN_CHECK;
      if (lex_rc != TOKEN_RSBRACKET) {
         if (errno == 0) errno = SYNTACTIC_ERROR;
         if (debug) printf("cekam hranata");
         return EXIT_FAILURE;
      }
   }
   else if (lex_rc == TOKEN_RSBRACKET) {*num2 = NULL; if (debug) printf("prazdno2");}
      else {
         if (errno == 0) errno = SYNTACTIC_ERROR;
         if (debug) printf("cekam hranata");
         return EXIT_FAILURE;
      }

   lex_rc=GetToken(&token);

   if ((lex_rc == TOKEN_EOL) || (lex_rc == TOKEN_EOF)) { 
       if (debug) printf(" %d ", lex_rc);
   }
   else {
     if (errno == 0) errno = SYNTACTIC_ERROR;
     if (debug) printf("neni EOL");
     return EXIT_FAILURE;
   }
   return EXIT_SUCCESS;
}

// generator listu z tokenu - vlastnost operace,zavorek,hodnoty/NID
int gener_list(Expr_list **L)
{
   T_Var *var;

   int waitfor=HODNOTA;
   int err;
   parentheses = 0;
   int par = 0;

   Expr_list *pom = NULL;
   Expr_list *last = NULL;
   *L = NULL; //inic listu

   lex_rc = GetToken(&token);
   TOKEN_CHECK;
   while ((lex_rc != TOKEN_EOF) && (lex_rc != TOKEN_EOL)) {
      switch (lex_rc) {
      //-------------------op == HODNOTA---------------
      //--------------------CISLO---------------------
      case TOKEN_NUMBER:
         if (waitfor == HODNOTA) {
            if (debug) printf("%d cislo\n",lex_rc);

            CALL_CHECK(GenerateVariable(FALSE, &var));
            CDoubleToT_Var(atof(token.data),var);

            GENERSHORT; //viz synan.h

            waitfor=OPERACE;
         }
         else { errno=SYNTACTIC_ERROR; if (debug) printf("cekam operaci"); return EXIT_FAILURE; }
      break;
       //--------------------STRING---------------------
      case TOKEN_STRING:
         if (waitfor == HODNOTA) {
            if (debug) printf("%d string\n",lex_rc);

            CALL_CHECK(GenerateVariable(FALSE, &var));
            T_StringToT_Var(&token,var);

            GENERSHORT;

            waitfor=OPERACE;
         }
      else { errno=SYNTACTIC_ERROR; if (debug) printf("cekam operaci"); return EXIT_FAILURE; }
      break;
       //--------------------ID---------------------
      case TOKEN_IDENTIFICATOR:
         // 1 . nil , false , true
         if (waitfor == HODNOTA) {
            waitfor = OPERACE;
            if (strcmp(token.data, "nil") == 0) {
               if (debug) printf("%d nil\n", lex_rc);

               CALL_CHECK(GenerateVariable(FALSE, &var));
               var->NID = NID_NIL;
               GENERSHORT;
            }

            else if (strcmp(token.data, "false") == 0) {
               if (debug) printf("%d false\n", lex_rc);

               CALL_CHECK(GenerateVariable(FALSE, &var));

               var->NID = NID_BOOL;
               var->vals.b_val = FALSE;

               GENERSHORT;
            }

            else if (strcmp(token.data, "true") == 0) {
               if (debug) printf("%d true\n", lex_rc);

               CALL_CHECK(GenerateVariable(FALSE, &var));

               var->NID = NID_BOOL;
               var->vals.b_val = TRUE;

               GENERSHORT;
            }

            // 2. identifikatory - pouze ukladame ukaz na tabulku
            else if (T_HTableExport(id_table, token.data, &var) == HASH_FOUND) {
               if (debug) printf ("%s identifikator\n", token.data);

               if (var->NID == NID_FUNCTION) { if (debug) printf("funkceeeeeeeeeeeeee");}
               //CALL_CHECK(CallFunction());
               //lex_rc=GetToken(&token);
               //if ((lex_rc==EOL) || (lex_rc==EOF))
               //(*L)->value = nularet;

               GENERSHORT;
            }
         else { errno = UNDEFINED_VARIABLE; if (debug) printf("id neexistuje"); return EXIT_FAILURE; }
         }
         else { errno = SYNTACTIC_ERROR; if (debug) printf("cekam operaci"); return EXIT_FAILURE; }
      break;

      //--------------------zavorka---------------------
      //vaze se k hodnote v pravo - ceka se stale na hodnotu
      case TOKEN_LBRACKET :
         if (waitfor == HODNOTA) {
            par--;
            parentheses--;
            if (debug) printf("%d levaz\n", lex_rc);
         }
         else { errno = SYNTACTIC_ERROR; if (debug) printf("cekam operaci"); return EXIT_FAILURE; }
      break;
      //--------------------op == OPERACE ---------------------
      //--------------------zavorka---------------------
      case TOKEN_RBRACKET :
         if (waitfor == OPERACE) {
            last->zavorka++;
            parentheses++;
            if (debug) printf("%d pravaz\n", lex_rc);
         }
         else { errno = SYNTACTIC_ERROR; if (debug) printf("cekam hodnotu"); return EXIT_FAILURE; }
         if (parentheses > 0) { errno=SYNTACTIC_ERROR; if (debug) printf("cekam hodnotu"); return EXIT_FAILURE; }
      break;
      //--------------------operace ---------------------
      case TOKEN_DSTAR:
      case TOKEN_STAR:   case TOKEN_SLASH:
      case TOKEN_PLUS:   case TOKEN_MINUS:
      case TOKEN_EQUAL:  case TOKEN_UNEQUAL:   case TOKEN_BEQUAL:
      case TOKEN_SMEQUAL:case TOKEN_SMALLER:   case TOKEN_BIGGER:
         if (waitfor==OPERACE) {
            if (debug) printf("%d operace\n", lex_rc);
            last->operace = lex_rc;
            waitfor = HODNOTA;
         }
         else { errno=SYNTACTIC_ERROR; if (debug) printf("cekam hodnotu"); return EXIT_FAILURE; }
      break;
      //------------------------PRAVAZAVORKA----------------------------
      case TOKEN_LSBRACKET:
         if (waitfor==OPERACE) {
            if (debug) printf("%d leva hranata zavorka\n",lex_rc);
            if ((*L!=NULL)&&((*L)->next==NULL)) {
               T_Var *svar;
               T_Var *svar2;

               if ((err = expr_string(&svar ,&svar2)) != EXIT_SUCCESS) return EXIT_FAILURE;
               else {
                  if ((svar!=NULL)||(svar2!=NULL)) {

                     CALL_CHECK(GenerateVariable(FALSE, &read_var));

                     read_var->NID=NID_STRING;
                     GenerateInstruction(IID_COPY, read_var, (*L)->value, NULL);

                     GenerateInstruction(IID_CUT, read_var, svar, svar2);

                     (*L)->value=read_var;
                  }
                  if ((lex_rc == TOKEN_EOL) || (lex_rc == TOKEN_EOF)) return EXIT_SUCCESS;
               }
            }
         }
      else { errno=SYNTACTIC_ERROR; if (debug) printf("cekam hodnotu"); return EXIT_FAILURE;}
      break;

      case TOKEN_BROKEN : if (debug) printf("%d lex chyba kdyz token brken chyba\n",lex_rc); return EXIT_FAILURE;
      default : { errno = SYNTACTIC_ERROR; if (debug) printf("%d syn chyba kdyz token brken chyba\n", lex_rc); return EXIT_FAILURE;} break;
      }

      lex_rc = GetToken(&token);
      TOKEN_CHECK;
   }
   if (*L == NULL) { errno = SYNTACTIC_ERROR; if (debug) printf("prazdno"); return EXIT_FAILURE;}
   if (parentheses != 0) { errno = SYNTACTIC_ERROR; if (debug) printf("spatne zavorky"); return EXIT_FAILURE;}
   return EXIT_SUCCESS;
}

/*
 * Princip vyhodnoceni pro jednotlive operace
 * pokud jde o hledanou operaci :
 * - porovnej typy, vyhodnot/ vytvor instrukci
 * - secti zavorky, prirad dalsi operaci
 * - urci typ vystupu pokud to jde (var -> NID)
 * jinak se posun
 *
*/

//-------------------------------------------------------------------
// VYHODNOCENI LISTU PRO **
//-------------------------------------------------------------------
// pouze NUM-NUM

int FIRST_eval(Expr_list**first, Expr_list** last)
{
   Expr_list *pom;
   Expr_list *del;
   T_Var *var;
   //1. **
   if (debug) printf("\n-----* /----\n");
   pom = *first;
   while ((pom != NULL) && (pom != *last)) {
      if (pom->operace==TOKEN_DSTAR)
         if (((pom->value->NID == NID_NUMBER) || (pom->value->NID == NID_UNDEF))
            &&((pom->next->value->NID == NID_NUMBER) || (pom->next->value->NID == NID_UNDEF)))
         {
            if (debug) printf("%f - %f  ** \n",pom->value->vals.d_val ,pom->next->value->vals.d_val );

            CALL_CHECK(GenerateVariable(FALSE, &var));
            var->NID=NID_NUMBER;

            GenerateInstruction(IID_PWR, var, pom->value, pom->next->value);
            pom->value=var;

            MYSHORTCUT;
         }
         else { errno = SEMANTIC_ERROR; if (debug) printf("semantika ** \n"); return EXIT_FAILURE; }
      else pom = pom->next;
   }
   return EXIT_SUCCESS;
}

//-------------------------------------------------------------------
// VYHODNOCENI LISTU PRO  *    /
//-------------------------------------------------------------------
int SECOND_eval(Expr_list** first, Expr_list** last)
{
   Expr_list *pom;
   Expr_list *del;
   T_Var *var;

   pom = *first;
   if (debug) printf("\n-----* /----\n");
   while ((pom != NULL) && (pom != *last)) {
      if (pom->operace == TOKEN_SLASH) {
         if (((pom->value->NID == NID_NUMBER) || (pom->value->NID == NID_UNDEF))
             &&((pom->next->value->NID == NID_NUMBER) || (pom->next->value->NID == NID_UNDEF)))
         {
            if (debug) printf("%f - %f  / \n", pom->value->vals.d_val, pom->next->value->vals.d_val);

            CALL_CHECK(GenerateVariable(FALSE, &var));
            var->NID=NID_NUMBER;

            GenerateInstruction(IID_DIV, var, pom->value, pom->next->value);

            pom->value=var;
            MYSHORTCUT;
         }
         else { errno = SEMANTIC_ERROR; if (debug) printf("semantika / \n"); return EXIT_FAILURE; }
      }
      else if (pom->operace == TOKEN_STAR) {
         if (((pom->value->NID == NID_NUMBER) || (pom->value->NID==NID_UNDEF) || (pom->value->NID==NID_STRING))
             &&((pom->next->value->NID == NID_NUMBER) || (pom->next->value->NID == NID_UNDEF)))
         {
            if (debug) printf("%f - %f  * \n", pom->value->vals.d_val, pom->next->value->vals.d_val);

            CALL_CHECK(GenerateVariable(FALSE, &var));

            if (pom->value->NID == NID_NUMBER) var->NID = NID_NUMBER;
            if (pom->value->NID == NID_STRING) var->NID = NID_STRING;

            GenerateInstruction(IID_MUL, var, pom->value, pom->next->value);

            pom->value=var;
            MYSHORTCUT;
         }
         else { errno = SEMANTIC_ERROR; if (debug) printf("semantika * \n"); return EXIT_FAILURE; }
      }
      else pom = pom->next;
   }
   return EXIT_SUCCESS;
}

//-------------------------------------------------------------------
// VYHODNOCENI LISTU PRO  +    -
//-------------------------------------------------------------------
int THIRT_eval(Expr_list** first, Expr_list** last)
{
   Expr_list *pom;
   Expr_list *del;

   T_Var *var;
   pom=*first;
   if (debug) printf("\n-----+ - ----");
   while ((pom != NULL) && (pom != *last)) {
      if (pom->operace == TOKEN_MINUS)
      //------------------------NUM - NUM --------------------------
      if (((pom->value->NID == NID_NUMBER) || (pom->value->NID == NID_UNDEF))
          &&((pom->next->value->NID == NID_NUMBER) || (pom->next->value->NID == NID_UNDEF)))
      {
         if (debug) printf("%f - %f  - \n", pom->value->vals.d_val, pom->next->value->vals.d_val);
         CALL_CHECK(GenerateVariable(FALSE, &var));
         var->NID = NID_NUMBER;

         GenerateInstruction(IID_DIFF, var, pom->value, pom->next->value);

         pom->value=var;

         MYSHORTCUT;
      }
      else { errno = SEMANTIC_ERROR; if (debug) printf("semantika -\n"); return EXIT_FAILURE; }

      else if (pom->operace == TOKEN_PLUS)
      //------------------------NUM - NUM --------------------------
      if ((pom->value->NID == NID_NUMBER) || (pom->value->NID == NID_UNDEF) || (pom->value->NID == NID_STRING)) {
         if (debug) printf("%f - %f  + \n", pom->value->vals.d_val, pom->next->value->vals.d_val);
         CALL_CHECK(GenerateVariable(FALSE, &var));
         var->NID = pom->value->NID;

         GenerateInstruction(IID_PLUS, var, pom->value, pom->next->value);
         pom->value = var;

         MYSHORTCUT;
      }
      else { errno = SEMANTIC_ERROR; if (debug) printf("semantika + \n"); return EXIT_FAILURE; }
      else pom = pom->next;
   }
   return EXIT_SUCCESS;
}

//-------------------------------------------------------------------
// VYHODNOCENI LISTU PRO  > , >= ,<= < , != ,==
//-------------------------------------------------------------------
int FOURTH_eval(Expr_list**first, Expr_list** last)
{
   Expr_list *pom;
   Expr_list *del;
   T_Var *var;

   pom = *first;
   if (debug) printf("\n   > , >= , <= < , != , ==   ");
   while ((pom != NULL) && (pom != *last)) {
      // mozne plynule vyhodnocovat prioritne na nejnizi urovni
      // v dane useku zbyly pouze operace relace
      CALL_CHECK(GenerateVariable(FALSE, &var));
      var->NID = NID_BOOL;

      switch (pom->operace) {
      case TOKEN_BIGGER  : T_InstrSet(&instr, IID_BIGGER, var, pom->value, pom->next->value); break;
      case TOKEN_SMALLER : T_InstrSet(&instr, IID_SMALLER, var, pom->value, pom->next->value); break;
      case TOKEN_BEQUAL  : T_InstrSet(&instr, IID_BEQUAL, var, pom->value, pom->next->value); break;
      case TOKEN_SMEQUAL : T_InstrSet(&instr, IID_SMEQUAL, var, pom->value, pom->next->value); break;
      case TOKEN_EQUAL   : T_InstrSet(&instr, IID_EQUAL, var, pom->value, pom->next->value); break;
      case TOKEN_UNEQUAL : T_InstrSet(&instr, IID_UNEQUAL, var, pom->value, pom->next->value); break;
      default : free(var); var = NULL;
      }
      if (var!=NULL) {
         T_ListAppend(inst_list, &instr);
         pom->value=var;
         MYSHORTCUT;
      }
   }
   return EXIT_SUCCESS;
}

//-------------------------------------------------------------------
// VYHODNOCENI LISTU PRO  pro usek First - Last
//-------------------------------------------------------------------
int eval(Expr_list* first, Expr_list* last)
{
  int err = 0;
  while ((first != last) && (err == 0)) {
    // postupne volani vyhodnoceni pro dany usek , konec v pripade chyby
    if ((first != last) && (err == 0)) err = FIRST_eval(&first, &last); // **
    if ((first != last) && (err == 0)) err = SECOND_eval(&first, &last); // * /
    if ((first != last) && (err == 0)) err = THIRT_eval(&first, &last); // + -
    if ((first != last) && (err == 0)) err = FOURTH_eval(&first, &last); // > , >= ,<= < , != ,==
  }
  return err;
}

//-------------------------------------------------------------------
// NALEZENI USEKU UZAVOREK A VOLANI JEJICH VYHODNOCENI

//-------------------------------------------------------------------
int evaluate(Expr_list **L)
{
   Expr_list *first = NULL;
   Expr_list *last = NULL;
   int err = 0;

   while (((*L)->next != NULL) && (err == 0)) {
      //-----------------------hledani zavorky-----
      // zavorka leva ma zapornou hodnotu
      first = *L;
      while ((first->zavorka >= 0) && (first->next != NULL)) first = first->next; {
         if (first->zavorka < 0) {
            last = first;
            //zavorka prava ma kladnou hodnotu
            while ((last->zavorka <= 0) && (last->next != NULL)) {
               last=last->next;
               if (last->zavorka < 0) first = last;
            }
         }
         else {
            last = first;
            first = *L;
         }
         err = eval(first, last);

         if (err != 0) return EXIT_FAILURE;
      }
   }

   return err;
}

//-------------------------------------------------------------------
// UVOLNENI LISTU PRI CHYBE
//-------------------------------------------------------------------
void expr_free(Expr_list **L)
{
   Expr_list *pom;
   while ((*L) != NULL) {
      pom = (*L);
      (*L) = pom->next;
      free(pom);
   }
   return;
}

//-------------------------------------------------------------------
// EXPR - funce resi nacitani tokenu do listu ktery potom vyhodnocuje
// 1. Tvorba listu gener_list(&List)
// 2. Shodnoceni zda je nutne vyhodnotit
// 3. Predani read_var adresy kde se bude po vzkonani instrukce vyhodnoceni
// nachazet hodnota
// 4. Uvolneni listu a EXIT_FAILURE v pripade chyby
//-------------------------------------------------------------------
int expr()
{
   int err = 0;
   Expr_list *List;

   err = gener_list(&List);

   if (err == 0)
     if (List->next != NULL)
       err=evaluate(&List);

   if (err == 0) {
      read_var=List->value;
      free(List);
   }
   else expr_free(&List);

   return err;
}
