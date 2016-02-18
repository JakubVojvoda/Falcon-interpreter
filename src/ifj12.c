///////////////////////////////////////////////////////////////////////////////
//
//  Falcon Interpreter
//
//  for Formal Languages and Compilers 2012/2013
//  by Varga Tomas, Vojvoda Jakub, Nesvadba Tomas, Spanko Jaroslav, Warzel Adam
//
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <errno.h>

#include "codes.h"
#include "debug.h"
#include "interpret.h"
#include "lex.h"
#include "macros.h"
#include "synan.h"

int main(int argc, char *argv[])
{
   DebugInit();

   if (argc != 2)
      return RUNTIME_ERROR;

   CALL_CHECK(LexInit(argv[1]));

   T_List inst_list;
   CALL_CHECK(T_ListInit(&inst_list));

   T_AllocList alloc_list;
   CALL_CHECK(T_AllocListInit(&alloc_list));

   T_LabelList lbl_list;
   CALL_CHECK(T_LabelListInit(&lbl_list));

   if (SynanInit(&inst_list, &alloc_list, &lbl_list) == EXIT_FAILURE) {
      LexFinish();
      return errno;
   }

   DEBUG_MAIN_SYNAN;
   if (Synan() == EXIT_FAILURE) {
      LexFinish();
      SynanFinish(&inst_list);
      return errno;
   }
   DEBUG_MAIN_SYNAN_END;

   if (Interpret(&inst_list, &lbl_list) == EXIT_FAILURE) {
     LexFinish();
     SynanFinish(&inst_list);
     return errno;
   }

   LexFinish();
   SynanFinish(&inst_list);
   return EXIT_SUCCESS;
}
