///////////////////////////////////////////////////////////////////////////////
//
//  Falcon Interpreter
//
//  for Formal Languages and Compilers 2012/2013
//  by Varga Tomas, Vojvoda Jakub, Nesvadba Tomas, Spanko Jaroslav, Warzel Adam
//
///////////////////////////////////////////////////////////////////////////////

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "builtin.h"
#include "codes.h"
#include "types.h"
#include "macros.h"
#include "debug.h"

int Input(T_Var *var)
{
   DEBUG_INPUT(var);
   if (var == NULL) {
      T_StringRead(stdin, NULL, '\n');
      return EXIT_SUCCESS;
   }

  T_StringRead(stdin, &(var->vals.str), '\n');
   if (errno) {
      DEBUG_INPUT_END(var);
      return EXIT_FAILURE;
   }

   DEBUG_INPUT_END(var);
   return EXIT_SUCCESS;
}

int Print(T_Var *read_var, T_Var *write_var)
{
   DEBUG_PRINT(write_var, read_var);
   // Basic check for NULL pointers
   PTR_CHECK(read_var);

   // Printing variable content according to its ID
   if (read_var->NID == NID_NIL)
      printf("Nil");

   if (read_var->NID == NID_BOOL) {
      if (read_var->vals.b_val==0)
         printf("false");
      else
         printf("true");
   }

   if (read_var->NID == NID_NUMBER)
      printf("%g",read_var->vals.d_val);

   if (read_var->NID == NID_STRING)
      printf("%s",read_var->vals.str.data);

   // Function always returns nil
   if (write_var != NULL)
      write_var->NID = NID_NIL;

   return EXIT_SUCCESS;
}

int Numeric(T_Var *write_var, T_Var *read_var)
{
   DEBUG_NUMERIC(write_var,read_var);
   // Basic check for NULL pointer
   PTR_CHECK(read_var);
   PTR_CHECK(write_var);

   if (read_var->NID == NID_NUMBER)
      memcpy(write_var, read_var, sizeof(T_Var));

   char *ukaz;
   double pom = strtod(read_var->vals.str.data, &ukaz);

   if ((int)pom == 0 && ukaz == read_var->vals.str.data) {
      errno = TYPE_CHANGE_ERROR; //chyba 12, zadani
      return EXIT_FAILURE;
   }
   else {
      write_var->vals.d_val = pom; //vrat
      write_var->NID = NID_NUMBER;
   }

   return EXIT_SUCCESS;
}

int TypeOf(T_Var *write_var, T_Var *read_var)
{
   DEBUG_TYPEOF(write_var, read_var);
   /*Basic check for NULL pointers*/
   PTR_CHECK(read_var);
   PTR_CHECK(write_var);

   write_var->NID = NID_NUMBER;
   write_var->vals.d_val = read_var->NID;

   DEBUG_TYPEOF_END(write_var, read_var);
   return EXIT_SUCCESS;
}

int Len(T_Var *write_var, T_Var *read_var)
{
   DEBUG_LEN(write_var, read_var);
   PTR_CHECK(read_var);
   PTR_CHECK(write_var);

   write_var->NID = NID_NUMBER;

   if (read_var->NID == NID_STRING)
      write_var->vals.d_val = read_var->vals.str.length;
   else
      write_var->vals.d_val = 0.0;

   DEBUG_LEN_END(write_var, read_var);
   return EXIT_SUCCESS;
}
