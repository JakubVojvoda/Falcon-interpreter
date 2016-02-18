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

#include "macros.h"
#include "codes.h"

int itoa (int num, char *str, int base)
{
   PTR_CHECK(str);

   int c;
   int len = 0;

   for(int x = num; x != 0; x/= base)
      len++;

   if (num == 0)
      len = 1;

   char *tmp = realloc(str, sizeof(char)*(len+1));
   PTR_CHECK(tmp);
   str = tmp;

   for (int i = len; i > 0; i--) {
      c = '0' + num % base;
      num /= base;
      str[i-1] = c;
   }
   str[len] = '\0';

   return EXIT_SUCCESS;
}
