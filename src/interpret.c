///////////////////////////////////////////////////////////////////////////////
//
//  Falcon Interpreter
//
//  for Formal Languages and Compilers 2012/2013
//  by Varga Tomas, Vojvoda Jakub, Nesvadba Tomas, Spanko Jaroslav, Warzel Adam
//
///////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include <errno.h>
#include <string.h>

#include "interpret.h"
#include "macros.h"
#include "types.h"
#include "codes.h"
#include "builtin.h"
#include "ial.h"
#include "debug.h"
#include "synan.h"
#include "functions.h"


int T_StringFill(T_String *str, T_Var var);
void OperationPrint(int op);
int RelType(T_Var arg1, T_Var arg2);
int Jump(T_List *list, T_LabelList *lbl_list, T_Var *var);

// T_Instr --------------------------------------------------------------------
int T_InstrInit(T_Instr *instr)
{
   DEBUG_T_INSTRINIT;
   PTR_CHECK(instr);

   instr->IID = IID_NOP;
   instr->arg1 = NULL;
   instr->arg2 = NULL;
   instr->arg3 = NULL;

   return EXIT_SUCCESS;
}

void T_InstrDelete(T_Instr *instr)
{
   DEBUG_T_INSTRDELETE;
   if (instr !=NULL) {
      free(instr->arg1);
      free(instr->arg2);
      free(instr->arg3);
   }
}

int T_InstrSet(T_Instr *instr, int IID, T_Var *arg1, T_Var *arg2, T_Var *arg3)
{
   PTR_CHECK(instr);

   instr->IID = IID;
   instr->arg1 = arg1;
   instr->arg2 = arg2;
   instr->arg3 = arg3;

   return EXIT_SUCCESS;
}

int Interpret(T_List *list, T_LabelList *lbl_list)
{
   DEBUG_INTERPRET(list);
   list->actual = list->first;
   while (list->actual != NULL) {
      T_Instr *current = list->actual->data;
      switch (current->IID) {

      // No Operation  -----------------------------------------------------------------------------------------------
      case IID_NOP: break;

      // Priradenie --------------------------------------------------------------------------------------------------
      case IID_COPY:
         DEBUG_INTERPRET_COPY(current);
         if (current->arg2->NID == NID_UNDEF) {
            errno = UNDEFINED_VARIABLE;
            return EXIT_FAILURE;
         }
         T_VarCopy(current->arg1, current->arg2);
      break;

      // Navestia ----------------------------------------------------------------------------------------------------
      case IID_LABEL: break;

      // Skoci na nasledujuce navestie -------------------------------------------------------------------------------
      // IID: IID_JUMP; arg1: NID = NID_STRING, str.data = meno labelu
      case IID_JUMP:
         DEBUG_INTERPRET_JUMP(current);
         CALL_CHECK(Jump(list, lbl_list, current->arg1));
      break;

      // Skoci na navestie ak neplati podmienka -------------------------------------------------------------------------
      case IID_BRAFAL:
         DEBUG_INTERPRET_BRAFAL(current);
         if ((current->arg1->NID == NID_BOOL && current->arg1->vals.b_val == FALSE)
             || (current->arg1->NID == NID_NIL)
             || (current->arg1->NID == NID_NUMBER && current->arg1->vals.d_val == 0.0)
             || (current->arg1->NID == NID_STRING && (strcmp(current->arg1->vals.str.data, "") == 0)))
         {
            Jump(list, lbl_list, current->arg2);
         }
      break;

      //  + (plus) ----------------------------------------------------------------------------------------------------
      case IID_PLUS:
         DEBUG_INTERPRET_PLUS(current);
         if (current->arg3 == NULL) {
           if (current->arg1->NID == NID_UNDEF || current->arg2->NID == NID_UNDEF) {
            errno = UNDEFINED_VARIABLE;
            return EXIT_FAILURE;
           }
            // Soucet cisel vysledok
            if (current->arg1->NID == NID_NUMBER && current->arg2->NID == NID_NUMBER) {
               current->arg1->vals.d_val += current->arg2->vals.d_val;
            }
            // Konkatenacia retazca
            else if (current->arg1->NID == NID_STRING) {
               if (current->arg2->NID == NID_STRING) {
                  CALL_CHECK(T_StringCat(&(current->arg1->vals.str), &(current->arg2->vals.str)));
                  current->arg1->NID = NID_STRING;
               }
               else {
                  T_String hstring;
                  PTR_CHECK( (hstring.data = malloc(sizeof(char)*TSTRING_SIZE)));
                  // pri chybe nastavi errno
                  CALL_CHECK( T_StringFill(&hstring, *(current->arg2)));
                  CALL_CHECK( T_StringCat(&(current->arg1->vals.str), &hstring));
                  current->arg1->NID = NID_STRING;
                  //free(hstring.data);
               }
            }
            // Semanticka chyba
            else {
               errno = TYPE_COMPATIBILITY_ERROR;
               return EXIT_FAILURE;
            }
         }
         else {
            if (current->arg2->NID == NID_UNDEF || current->arg3->NID == NID_UNDEF) {
            errno = UNDEFINED_VARIABLE;
            return EXIT_FAILURE;
           }
            // Zrusenie obsahu premennej kde sa bude zapisovat
            T_VarDelete(current->arg1);
            T_VarInit(current->arg1);

            // Soucet cisel
            if (current->arg2->NID == NID_NUMBER && current->arg3->NID == NID_NUMBER) {
               current->arg1->vals.d_val = current->arg2->vals.d_val + current->arg3->vals.d_val;
               current->arg1->NID = NID_NUMBER;
            }
            // Konkatenacia retazca
            else if (current->arg2->NID == NID_STRING) {
               current->arg1->vals.str.data = malloc(sizeof(char)*TSTRING_SIZE);

               if (current->arg3->NID == NID_STRING) {
                  // Konkatenacia T_String - vysledok do arg2
                  // Prekopirovanie do premennej vysledku
                  T_StringCopy(&(current->arg1->vals.str), &(current->arg2->vals.str));
                  CALL_CHECK(T_StringCat(&(current->arg1->vals.str), &(current->arg3->vals.str)));
                  current->arg1->NID = NID_STRING;
               }
               else {
                  T_String hstring;
                  PTR_CHECK( (hstring.data = malloc(sizeof(char)*TSTRING_SIZE)));
                  // pri chybe nastavi errno vo funkcii
                  CALL_CHECK( T_StringFill(&hstring, *(current->arg3)));
                  CALL_CHECK( T_VarCopy(current->arg1, current->arg2));
                  CALL_CHECK( T_StringCat(&(current->arg1->vals.str), &hstring));
                  current->arg1->NID = NID_STRING;
                  //free(hstring.data);
               }
            }
            // Semanticka chyba
            else {
               errno = TYPE_COMPATIBILITY_ERROR;
               return EXIT_FAILURE;
            }
         }
         break;

      // - (minus) ----------------------------------------------------------------------------------------------------
      case IID_DIFF:
         DEBUG_INTERPRET_DIFF(current);
         if (current->arg3 == NULL) {
            if (current->arg1->NID == NID_UNDEF || current->arg2->NID == NID_UNDEF) {
               errno = UNDEFINED_VARIABLE;
               return EXIT_FAILURE;
            }
            // Rozdiel len cisel
            if (current->arg1->NID == NID_NUMBER && current->arg2->NID == NID_NUMBER) {
               current->arg1->vals.d_val -= current->arg2->vals.d_val;
            }
            // Semanticka chyba
            else {
               errno = TYPE_COMPATIBILITY_ERROR;
               return EXIT_FAILURE;
            }
         }
         else {
            if (current->arg2->NID == NID_UNDEF || current->arg3->NID == NID_UNDEF) {
               errno = UNDEFINED_VARIABLE;
               return EXIT_FAILURE;
            }
            // Rozdiel len cisel
            if (current->arg2->NID == NID_NUMBER && current->arg3->NID == NID_NUMBER) {
               // Zrusenie obsahu premennej kde sa bude zapisovat
               T_VarDelete(current->arg1);
               T_VarInit(current->arg1);
               current->arg1->NID = NID_NUMBER;
               current->arg1->vals.d_val = current->arg2->vals.d_val - current->arg3->vals.d_val;
            }
            // Semanticka chyba
            else {
               errno = TYPE_COMPATIBILITY_ERROR;
               return EXIT_FAILURE;
            }
         }
         break;

      // * (krat) ---------------------------------------------------------------------------------------------------
      case IID_MUL:
         DEBUG_INTERPRET_MUL(current);
         if (current->arg3 == NULL) {
            if (current->arg1->NID == NID_UNDEF || current->arg2->NID == NID_UNDEF) {
               errno = UNDEFINED_VARIABLE;
               return EXIT_FAILURE;
            }
            // Nasobeni cisel
            if (current->arg1->NID == NID_NUMBER && current->arg2->NID == NID_NUMBER) {
               current->arg1->vals.d_val *= current->arg2->vals.d_val;
            }
            // Mocnina retazca
            else if (current->arg1->NID == NID_STRING && current->arg2->NID == NID_NUMBER) {
               // pomocna premenna, kolkokrat sa umocnit
               int mocn = (int)current->arg2->vals.d_val;

               // Vrati prazdny retazec
               if ( mocn == 0 ) {
                  if (current->arg1->vals.str.length > 0) {
                     char *hptr;
                     PTR_CHECK( (hptr = realloc(current->arg1->vals.str.data, sizeof(char)*TSTRING_SIZE)));
                     current->arg1->vals.str.data = hptr;
                  }

                  current->arg1->vals.str.data[0] = '\0';
                  current->arg1->vals.str.length = 0;
               }
               // Umocnovani
               else if ( mocn > 0 ) {
                  T_Var pom;
                  T_VarInit(&pom);
                  T_VarCopy(&pom, current->arg1);

                  // Vykona sa (mocn-1)-nasobna konkatenacia
                  mocn -= 1;

                  for ( ; mocn > 0; mocn--)
                     T_StringCat(&(current->arg1->vals.str), &(pom.vals.str));
                  current->arg1->NID = NID_STRING;
               }
               // Semanticka chyba
               else {
                  errno = TYPE_COMPATIBILITY_ERROR;
                  return EXIT_FAILURE;
               }
            }
            else {
               errno = TYPE_COMPATIBILITY_ERROR;
               return EXIT_FAILURE;
            }
         }
         else {
            if (current->arg2->NID == NID_UNDEF || current->arg3->NID == NID_UNDEF) {
               errno = UNDEFINED_VARIABLE;
               return EXIT_FAILURE;
            }
            // Zrusenie obsahu premennej kde sa bude zapisovat
            T_VarDelete(current->arg1);
            T_VarInit(current->arg1);

            // Nasobenie cisel
            if (current->arg2->NID == NID_NUMBER && current->arg3->NID == NID_NUMBER) {
               current->arg1->NID = NID_NUMBER;
               current->arg1->vals.d_val = current->arg2->vals.d_val * current->arg3->vals.d_val;
            }
            // Mocnina retazca
            else if (current->arg2->NID == NID_STRING && current->arg3->NID == NID_NUMBER) {

               int mocn = (int)current->arg3->vals.d_val;

               // Semanticka chyba
               if ( mocn < 0 ) {
                  errno = RUNTIME_ERROR;
                  return EXIT_FAILURE;
               }

               // Alokacia polozky do ktorej sa bude zapisovat
               PTR_CHECK( (current->arg1->vals.str.data = malloc(sizeof(char)*TSTRING_SIZE)));

               // Vrati prazdny retazec
               if ( mocn == 0 ) {
                  current->arg1->NID = NID_STRING;
                  current->arg1->vals.str.data[0] = '\0';
                  current->arg1->vals.str.length = 0;
               }
               // Vykona mocn-nasobnu konkatenaciu
               else {
                  for ( ; mocn > 0; mocn--)
                     T_StringCat(&(current->arg1->vals.str), &(current->arg2->vals.str));
                  current->arg1->NID = NID_STRING;
               }
            }
            // Semanticka chyba
            else {
               errno = TYPE_COMPATIBILITY_ERROR;
               return EXIT_FAILURE;
            }
         }
         break;

      // / (deleno) ------------------------------------------------------------------------------------
      case IID_DIV:
         DEBUG_INTERPRET_DIV(current);
         // Deleni len cisel
         if (current->arg3 == NULL) {
            if (current->arg1->NID == NID_UNDEF || current->arg2->NID == NID_UNDEF) {
               errno = UNDEFINED_VARIABLE;
               return EXIT_FAILURE;
            }
            if (current->arg1->NID == NID_NUMBER && current->arg2->NID == NID_NUMBER) {
               if (current->arg2->vals.d_val == 0) {
                  errno = DIVIDE_ZERO;
                  return EXIT_FAILURE;
               }
               current->arg1->vals.d_val /= current->arg2->vals.d_val;
            }
            else {
               errno = TYPE_COMPATIBILITY_ERROR;
               return EXIT_FAILURE;
            }
         }
         else {
            if (current->arg2->NID == NID_UNDEF || current->arg3->NID == NID_UNDEF) {
               errno = UNDEFINED_VARIABLE;
               return EXIT_FAILURE;
            }
            if (current->arg2->NID == NID_NUMBER && current->arg3->NID == NID_NUMBER) {
               if (current->arg3->vals.d_val == 0) {
                  errno = DIVIDE_ZERO;
                  return EXIT_FAILURE;
               }
               T_VarDelete(current->arg1);
               T_VarInit(current->arg1);
               current->arg1->NID = NID_NUMBER;
               current->arg1->vals.d_val = current->arg2->vals.d_val / current->arg3->vals.d_val;
            }
            else {
               errno = TYPE_COMPATIBILITY_ERROR;
               return EXIT_FAILURE;
            }
         }
         break;

      // ** (umocnovanie) ----------------------------------------------------------------------------------
      case IID_PWR:
         DEBUG_INTERPRET_PWR(current);
         // Mocnina len cisel
         if (current->arg3 == NULL) {
            if (current->arg1->NID == NID_UNDEF || current->arg2->NID == NID_UNDEF) {
               errno = UNDEFINED_VARIABLE;
               return EXIT_FAILURE;
            }
            if (current->arg1->NID == NID_NUMBER && current->arg2->NID == NID_NUMBER) {
               current->arg1->vals.d_val = pow(current->arg1->vals.d_val, current->arg2->vals.d_val);
            }
            else {
               errno = TYPE_COMPATIBILITY_ERROR;
               return EXIT_FAILURE;
            }
         }
         else {
            if (current->arg2->NID == NID_UNDEF || current->arg3->NID == NID_UNDEF) {
               errno = UNDEFINED_VARIABLE;
               return EXIT_FAILURE;
            }
            if (current->arg2->NID == NID_NUMBER && current->arg3->NID == NID_NUMBER) {
               T_VarDelete(current->arg1);
               T_VarInit(current->arg1);
               current->arg1->NID = NID_NUMBER;
               current->arg1->vals.d_val = pow(current->arg2->vals.d_val, current->arg3->vals.d_val);
            }
            else {
               errno = TYPE_COMPATIBILITY_ERROR;
               return EXIT_FAILURE;
            }
         }
         break;

      // cut string[index:index] --------------------------------------------------------------------------
      case IID_CUT:
         DEBUG_INTERPRET_CUT(current);
         // Vybranie podretazca z retazca
         if (current->arg1->NID == NID_UNDEF || current->arg2->NID == NID_UNDEF || current->arg3->NID == NID_UNDEF) {
            errno = UNDEFINED_VARIABLE;
            return EXIT_FAILURE;
         }

         if (current->arg1->NID == NID_STRING) {
            T_String hstr;
            T_StringInit(&hstr);

            if ((current->arg2 == NULL) &&(current->arg3->NID==NID_NUMBER))
               CutString(&hstr , &(current->arg1->vals.str), 0, current->arg3->vals.d_val);
            else if ((current->arg3 == NULL)&& (current->arg2->NID==NID_NUMBER))
               CutString(&hstr, &(current->arg1->vals.str), current->arg2->vals.d_val, current->arg1->vals.str.length);
            else if ((current->arg3->NID==NID_NUMBER)&& (current->arg2->NID==NID_NUMBER))
               CutString(&hstr, &(current->arg1->vals.str), current->arg2->vals.d_val, current->arg3->vals.d_val);
	    else {
               errno = TYPE_COMPATIBILITY_ERROR;
               return EXIT_FAILURE;
            }

            // Zrusenie obsahu premennej kde sa bude zapisovat
            T_VarDelete(current->arg1);
            T_VarInit(current->arg1);

            // Naplne vyslednej premennej
            current->arg1->NID = NID_STRING;
            current->arg1->vals.str.length = strlen(hstr.data);
            current->arg1->vals.str = hstr;
         }
         // Semanticka chyba
         else {
               errno = TYPE_COMPATIBILITY_ERROR;
               return EXIT_FAILURE;
               }
         break;

      /* False 0
         True 1
      */
      // == relacny operator - porovnanie -----------------------------------------------------------------------------
      case IID_EQUAL:
         if (current->arg1->NID == NID_UNDEF || current->arg2->NID == NID_UNDEF || current->arg3->NID == NID_UNDEF) {
            errno = UNDEFINED_VARIABLE;
            return EXIT_FAILURE;
         }

         // musia sa rovnat typy aj hodnoty
         T_VarDelete(current->arg1);
         T_VarInit(current->arg1);
         current->arg1->NID = NID_BOOL;

         if ((current->arg2->NID != NID_CONST && current->arg2->NID != NID_UNDEF && current->arg2->NID != NID_FUNCTION)
            && (current->arg3->NID != NID_CONST && current->arg3->NID != NID_UNDEF && current->arg3->NID != NID_FUNCTION))
            {
            current->arg1->vals.b_val = RelType((*current->arg2), (*current->arg3));
         }
         // Semanticka chyba
         else {
            errno = TYPE_COMPATIBILITY_ERROR;
            return EXIT_FAILURE;
         }
         break;

      // != relacny operator - nerovnost --------------------------------------------------------------------------------
      case IID_UNEQUAL:
         if (current->arg1->NID == NID_UNDEF || current->arg2->NID == NID_UNDEF || current->arg3->NID == NID_UNDEF) {
            errno = UNDEFINED_VARIABLE;
            return EXIT_FAILURE;
         }

         // nesmu sa rovnat typy alebo hodnoty - negacia rovnosti
         T_VarDelete(current->arg1);
         T_VarInit(current->arg1);
         current->arg1->NID = NID_BOOL;

         if ((current->arg2->NID != NID_CONST && current->arg2->NID != NID_UNDEF && current->arg2->NID != NID_FUNCTION)
            && (current->arg3->NID != NID_CONST && current->arg3->NID != NID_UNDEF && current->arg3->NID != NID_FUNCTION))
            {
            int retval = RelType((*current->arg2), (*current->arg3));
            current->arg1->vals.b_val = ((retval == 0) ? 1 : 0 );
         }
         // Semanticka chyba
         else {
            errno = UNDEFINED_VARIABLE;
            return EXIT_FAILURE;
         }
         break;

      // >= relacny operator --------------------------------------------------------------------------------------------------
      case IID_BEQUAL:
         if (current->arg1->NID == NID_UNDEF || current->arg2->NID == NID_UNDEF || current->arg3->NID == NID_UNDEF) {
            errno = UNDEFINED_VARIABLE;
            return EXIT_FAILURE;
         }
         // len NID_NUMBER alebo NID_STRING
         T_VarDelete(current->arg1);
         T_VarInit(current->arg1);
         current->arg1->NID = NID_BOOL;

         if ((current->arg2->NID == current->arg3->NID) && current->arg2->NID == NID_NUMBER) {
            current->arg1->vals.b_val = ((current->arg2->vals.d_val >= current->arg3->vals.d_val) ? 1 : 0);
         }
         else if ( (current->arg2->NID == current->arg3->NID) && current->arg2->NID == NID_STRING) {
            int retval = strcmp(current->arg2->vals.str.data, current->arg3->vals.str.data);
            current->arg1->vals.b_val = (( retval >= 0 ) ? 1 : 0);
         }
         // Semanticka chyba
         else {
            errno = TYPE_COMPATIBILITY_ERROR;
            return EXIT_FAILURE;
         }
         break;

      // <= relacny operator --------------------------------------------------------------------------------------------------
      case IID_SMEQUAL:
         if (current->arg1->NID == NID_UNDEF || current->arg2->NID == NID_UNDEF || current->arg3->NID == NID_UNDEF) {
            errno = UNDEFINED_VARIABLE;
            return EXIT_FAILURE;
         }
         // len NID_NUMBER alebo NID_STRING
         T_VarDelete(current->arg1);
         T_VarInit(current->arg1);
         current->arg1->NID = NID_BOOL;

         if ((current->arg2->NID == current->arg3->NID) && current->arg2->NID == NID_NUMBER) {
            current->arg1->vals.b_val = ((current->arg2->vals.d_val <= current->arg3->vals.d_val) ? 1 : 0);
         }
         else if ( (current->arg2->NID == current->arg3->NID) && current->arg2->NID == NID_STRING) {
            int retval = strcmp(current->arg2->vals.str.data, current->arg3->vals.str.data);
            current->arg1->vals.b_val = (( retval <= 0 ) ? 1 : 0);
         }
         // Semanticka chyba
         else {
            errno = TYPE_COMPATIBILITY_ERROR;
            return EXIT_FAILURE;
         }
         break;

      // < relacny operator --------------------------------------------------------------------------------------------------
      case IID_SMALLER:
         if (current->arg1->NID == NID_UNDEF || current->arg2->NID == NID_UNDEF || current->arg3->NID == NID_UNDEF) {
            errno = UNDEFINED_VARIABLE;
            return EXIT_FAILURE;
         }
         // len NID_NUMBER alebo NID_STRING
         T_VarDelete(current->arg1);
         T_VarInit(current->arg1);
         current->arg1->NID = NID_BOOL;

         if ((current->arg2->NID == current->arg3->NID) && current->arg2->NID == NID_NUMBER) {
            current->arg1->vals.b_val = ((current->arg2->vals.d_val < current->arg3->vals.d_val) ? 1 : 0);
         }
         else if ( (current->arg2->NID == current->arg3->NID) && current->arg2->NID == NID_STRING) {
            int retval = strcmp(current->arg2->vals.str.data, current->arg3->vals.str.data);
            current->arg1->vals.b_val = (( retval < 0 ) ? 1 : 0);
         }
         // Semanticka chyba
         else {
            errno = TYPE_COMPATIBILITY_ERROR;
            return EXIT_FAILURE;
         }
         break;

      // > relacny operator --------------------------------------------------------------------------------------------------
      case IID_BIGGER:
         if (current->arg1->NID == NID_UNDEF || current->arg2->NID == NID_UNDEF || current->arg3->NID == NID_UNDEF) {
            errno = UNDEFINED_VARIABLE;
            return EXIT_FAILURE;
         }
         // len NID_NUMBER alebo NID_STRING
         T_VarDelete(current->arg1);
         T_VarInit(current->arg1);
         current->arg1->NID = NID_BOOL;

         if ((current->arg2->NID == current->arg3->NID) && current->arg2->NID == NID_NUMBER) {
            current->arg1->vals.b_val = ((current->arg2->vals.d_val > current->arg3->vals.d_val) ? 1 : 0);
         }
         else if ( (current->arg2->NID == current->arg3->NID) && current->arg2->NID == NID_STRING) {
            int retval = strcmp(current->arg2->vals.str.data, current->arg3->vals.str.data);
            current->arg1->vals.b_val = (( retval > 0 ) ? 1 : 0);
         }
         // Semanticka chyba
         else {
            errno = TYPE_COMPATIBILITY_ERROR;
            return EXIT_FAILURE;
         }
         break;

      // Volani vnitrnich funkci z builtinu
      case IID_INPUT:
            CALL_CHECK( Input(current->arg1));
            current->arg1->NID = NID_STRING;
      break;

      case IID_NUMERIC:
         // viz builtin
         CALL_CHECK(Numeric(current->arg1, current->arg2));
      break;

      case IID_PRINT:
         CALL_CHECK(Print(current->arg1, current->arg2));
      break;

      case IID_TYPEOF:
         CALL_CHECK(TypeOf(current->arg1, current->arg2));
      break;

      case IID_LEN:
         CALL_CHECK(Len(current->arg1, current->arg2));
      break;

      case IID_FIND:
         if ((current->arg2->NID == NID_UNDEF) || (current->arg3->NID == NID_UNDEF)) {
            errno = UNDEFINED_VARIABLE;
            return EXIT_FAILURE;
         }

         if ((current->arg2->NID == NID_STRING) && (current->arg3->NID == NID_STRING)) {
            CALL_CHECK(Find(current->arg2, current->arg3, current->arg1)); // v arg2 najdi podretezec arg3, vrat pozici pomoci arg1
         }
         else {
            errno = TYPE_COMPATIBILITY_ERROR;
            return EXIT_FAILURE;
         }
      break;

      case IID_SORT:
         if (current->arg2->NID == NID_UNDEF) {
            errno = UNDEFINED_VARIABLE;
            return EXIT_FAILURE;
         }

         if (current->arg2->NID == NID_STRING) {
            CALL_CHECK( Sort(current->arg2, current->arg1));
         }
         else {
            errno = TYPE_COMPATIBILITY_ERROR;
            return EXIT_FAILURE;
         }
      break;

      }
      T_ListSucc(list);
   }

   DEBUG_INTERPRET_END;
   return EXIT_SUCCESS;
}

/* Naplni T_String podla hodnoty NID T_Var.
   Vyuzite v interprete, pri konkatenacii retazcov.
*/
int T_StringFill(T_String *str, T_Var var)
{
   switch ( (int) var.NID) {
      case (int)NID_BOOL:
         if (var.vals.b_val == 0) {
            strcpy(str->data, "false");
            str->length = strlen("false");
         }
         else {
            strcpy(str->data, "true");
            str->length = strlen("true");
         }
         break;

      case (int)NID_NIL:
         strcpy(str->data, "Nil");
         str->length = strlen("Nil");
         break;

      case (int)NID_NUMBER:
         if (itoa(var.vals.d_val, str->data, DECIMAL_BASE) == EXIT_FAILURE) {
            errno = INTERNAL_ERROR;
            return EXIT_FAILURE;
         }
         str->length = strlen(str->data);
         break;

      default:
         errno = SEMANTIC_ERROR;
         return EXIT_FAILURE;
   }
   return EXIT_SUCCESS;
}

int RelType(T_Var arg1, T_Var arg2)
{
   if ((arg1.NID == arg2.NID) && arg1.NID == NID_STRING && (strcmp(arg1.vals.str.data, arg2.vals.str.data) == 0))
      return 1;  // true
   else if ((arg1.NID == arg2.NID) && arg1.NID == NID_BOOL && (arg1.vals.b_val == arg2.vals.b_val))
      return 1; // true
   else if ((arg1.NID == arg2.NID) && arg1.NID == NID_NUMBER && (arg1.vals.d_val == arg2.vals.d_val))
      return 1;  // true

   return 0; // false
}

void OperationPrint(int op) {
   switch (op) {
      case IID_NOP:
         printf("  Operacia: NOP \n");
         break;
      case IID_COPY:
         printf("  Operacia: COPY \n");
         break;
      case IID_BRAFAL:
         printf("  Operacia: BRAFAL\n");
         break;
      case IID_LABEL:
         printf("  Operacia: LABEL \n");
         break;
      case IID_JUMP:
         printf("  Operacia: JUMP \n");
         break;
      case IID_PLUS:
         printf("  Operacia: PLUS \n");
         break;
      case IID_DIFF:
         printf("  Operacia: DIFF \n");
         break;
      case IID_MUL:
         printf("  Operacia: MUL \n");
         break;
      case IID_DIV:
         printf("  Operacia: DIV \n");
         break;
      case IID_PWR:
         printf("  Operacia: PWR \n");
         break;
      case IID_CUT:
         printf("  Operacia: CUT \n");
         break;

      case IID_EQUAL:
         printf("  Operacia: EQUAL \n");
         break;
      case IID_UNEQUAL:
         printf("  Operacia: UNEQUAL \n");
         break;
      case IID_BEQUAL:
         printf("  Operacia: BEQUAL \n");
         break;
      case IID_SMEQUAL:
         printf("  Operacia: SMEQUAL \n");
         break;
      case IID_SMALLER:
         printf("  Operacia: SMALLER \n");
         break;
      case IID_BIGGER:
         printf("  Operacia: BIGGER \n");
         break;

      case IID_INPUT:
         printf("  Operacia: IID_INPUT \n");
         break;
      case IID_NUMERIC:
         printf("  Operacia: NUMERIC \n");
         break;
      case IID_PRINT:
         printf("  Operacia: PRINT \n");
         break;
      case IID_TYPEOF:
         printf("  Operacia: TYPEOF \n");
         break;
      case IID_LEN:
         printf("  Operacia: LEN \n");
         break;
      case IID_FIND:
         printf("  Operacia: FIND \n");
         break;
      case IID_SORT:
         printf("  Operacia: SORT \n");
         break;
      default:
         printf("  Operacia: !!! uknown !!! \n");
         break;
   }
}

int Jump(T_List *list, T_LabelList *lbl_list, T_Var *var)
{
   if (var->NID == NID_STRING) {
      T_LabelItem *pom = lbl_list->first;

      while (pom != NULL) {
         if (strcmp((*pom->ptr_label)->data->arg1->vals.str.data, var->vals.str.data) == 0) {
            list->actual = (*pom->ptr_label);
            return EXIT_SUCCESS;
         }
         else {
            pom = pom->next;
         }
      }
   }

   errno = INTERNAL_ERROR;
   return EXIT_FAILURE;
}
