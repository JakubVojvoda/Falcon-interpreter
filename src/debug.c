///////////////////////////////////////////////////////////////////////////////
//
//  Falcon Interpreter
//
//  for Formal Languages and Compilers 2012/2013
//  by Varga Tomas, Vojvoda Jakub, Nesvadba Tomas, Spanko Jaroslav, Warzel Adam
//
///////////////////////////////////////////////////////////////////////////////

#include "debug.h"

void DebugInit()
{
   #ifndef OUR_DEBUG
      debug = 0;
   #else
      debug = 1;
      printf("---------------------------- DEBUGGER Inicializovan ----------------------------\n");
   #endif
}
