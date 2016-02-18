///////////////////////////////////////////////////////////////////////////////
//
//  Falcon Interpreter
//
//  for Formal Languages and Compilers 2012/2013
//  by Varga Tomas, Vojvoda Jakub, Nesvadba Tomas, Spanko Jaroslav, Warzel Adam
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MACROS_H_INCLUDED
#define MACROS_H_INCLUDED

#include <stdlib.h>

#define PTR_CHECK(ptr) do { \
   if (ptr == NULL) { \
      errno = INTERNAL_ERROR; \
      return EXIT_FAILURE; \
   } \
} while (0);

#define CALL_CHECK(p) \
if (p == EXIT_FAILURE) \
  return EXIT_FAILURE

#endif
