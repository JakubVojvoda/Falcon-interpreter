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
#include <stdio.h>
#include <ctype.h>

#include "types.h"
#include "macros.h"
#include "ial.h"
#include "codes.h"
#include "lex.h"
#include "debug.h"

enum { // Lexical analyzer states
   LEX_START,
   LEX_ALNUM, // Alpha-numeric state
   LEX_NUM, // Numeric state
   LEX_NUM_DEC, // Digits beyond decimal point
   LEX_NUM_EXP, // Exponent
   LEX_ONLYNUM,
   LEX_POST_E,
   LEX_STRING,
   LEX_EXCLAMATION, // Unequal
   LEX_SMEQUAL,
   LEX_BEQUAL,
   LEX_EQUAL,
   LEX_STAR,
   LEX_SLASH,
   LEX_LCOMMENT, // Line Comment
   LEX_COMMENT,
   LEX_COMMENT_STAR,
};

FILE *fd;

int LexInit(char *filename)
{
   DEBUG_LEX_INIT;
   fd = fopen(filename, "r");

   if (fd == NULL) {
      errno = INTERNAL_ERROR;
      return EXIT_FAILURE;
   }

   DEBUG_LEX_INIT_END;
   return EXIT_SUCCESS;
}

void LexFinish()
{
   DEBUG_LEX_FINISH;

   fclose(fd);

   DEBUG_LEX_FINISH_END;
}

int GetToken(T_String *token)
{
   DEBUG_GET_TOKEN;
   if (token == NULL) {
      errno = INTERNAL_ERROR;
      DEBUG_GET_TOKEN_END;
      return TOKEN_PROBLEM;
   }

   int c;
   int state = LEX_START;

   T_StringDelete(token);
   CALL_CHECK(T_StringInit(token));

   while (1) {
      c = fgetc(fd);
      if (ferror(fd)) {
         errno = INTERNAL_ERROR;
         DEBUG_GET_TOKEN_END;
         return TOKEN_PROBLEM;
      }
      switch (state) {
//---------------------------------------------------------------------------------
      case LEX_START:
         if (c == '\n'){
            T_StringAppend(token, c);
            DEBUG_GET_TOKEN_END;
            return TOKEN_EOL;
         }

         else if (isspace(c))			//mezera
            continue;

         else if ((isalpha(c)) || (c == '_')) {	//pismeno , podtrzka
            state = LEX_ALNUM;
            T_StringAppend(token, c);
         }

         else if (isdigit(c)) {			//cifra
            state = LEX_NUM;
            T_StringAppend(token, c);
         }

         else if (c == '"')			//string
            state = LEX_STRING;

         else if (c == '!') {			//unequal
            state = LEX_EXCLAMATION;
            T_StringAppend(token, c);
         }

         else if (c == '*') {			//star
            state = LEX_STAR;
            T_StringAppend(token, c);
         }

         else if (c == '/') {			//slash
            state = LEX_SLASH;
            T_StringAppend(token, c);
         }

         else if (c == '<') {
            state = LEX_SMEQUAL;
            T_StringAppend(token, c);
         }

         else if (c == '>') {
            state = LEX_BEQUAL;
            T_StringAppend(token, c);
         }

         else if (c == '=') {
            state = LEX_EQUAL;
            T_StringAppend(token, c);
         }

         else if (c == ',') {T_StringAppend(token, c); DEBUG_GET_TOKEN_END; return TOKEN_COMMA;}
         else if (c == '+') {T_StringAppend(token, c); DEBUG_GET_TOKEN_END; return TOKEN_PLUS;}
         else if (c == '-') {T_StringAppend(token, c); DEBUG_GET_TOKEN_END; return TOKEN_MINUS;}
         else if (c == ':') {T_StringAppend(token, c); DEBUG_GET_TOKEN_END; return TOKEN_COLUMN;}
         else if (c == '(') {T_StringAppend(token, c); DEBUG_GET_TOKEN_END; return TOKEN_LBRACKET;}
         else if (c == ')') {T_StringAppend(token, c); DEBUG_GET_TOKEN_END; return TOKEN_RBRACKET;}
         else if (c == '[') {T_StringAppend(token, c); DEBUG_GET_TOKEN_END; return TOKEN_LSBRACKET;}
         else if (c == ']') {T_StringAppend(token, c); DEBUG_GET_TOKEN_END; return TOKEN_RSBRACKET;}

         else if (c == EOF) {
            DEBUG_GET_TOKEN_END;
            return TOKEN_EOF;
         }

         else {
            DEBUG_GET_TOKEN_END;
            errno = LEXICAL_ERROR;
            return TOKEN_BROKEN;
         }
      break;
//---------------------------------------------------------------------------------
      case LEX_ALNUM:
         if ((isalpha(c)) || (isdigit(c)) || (c == '_'))
            T_StringAppend(token, c);

         else {
            ungetc(c, fd);
            DEBUG_GET_TOKEN_END;
            return TOKEN_IDENTIFICATOR;
         }
      break;
//---------------------------------------------------------------------------------
      case LEX_NUM:
         if (isdigit(c))
            T_StringAppend(token, c);

         else if (c == '.') {
            T_StringAppend(token, c);
            state = LEX_ONLYNUM;
         }

         else if ((c == 'e')) {
            T_StringAppend(token, c);
            state = LEX_POST_E;
         }

         else {
            errno = LEXICAL_ERROR;
            DEBUG_GET_TOKEN_END;
            return TOKEN_BROKEN;
         }
      break;
//---------------------------------------------------------------------------------
      case LEX_NUM_DEC:
         if ((isspace(c)) || (c == '+') || (c == '-') || (c == '*') || (c == '/') || (c == ':') || (c == ')') || (c == ']') || (c == EOF)) {
            ungetc(c, fd);
            DEBUG_GET_TOKEN_END;
            return TOKEN_NUMBER;
         }

         else if (isdigit(c))
            T_StringAppend(token, c);

         else {
            errno = LEXICAL_ERROR;
            DEBUG_GET_TOKEN_END;
            return TOKEN_BROKEN;
         }
      break;
//---------------------------------------------------------------------------------
      case LEX_ONLYNUM:
         if (isdigit(c)) {
            state = LEX_NUM_DEC;
            T_StringAppend(token, c);
         }

         else {
            errno = LEXICAL_ERROR;
            DEBUG_GET_TOKEN_END;
            return TOKEN_BROKEN;
         }
      break;
//---------------------------------------------------------------------------------
      case LEX_POST_E:
         if (isdigit(c)) {
            state = LEX_NUM_DEC;
            T_StringAppend(token, c);
         }

         else if ((c == '+') || (c == '-')) {
            T_StringAppend(token, c);
            state = LEX_ONLYNUM;
         }

         else {
            errno = LEXICAL_ERROR;
            DEBUG_GET_TOKEN_END;
            return TOKEN_BROKEN;
         }
      break;
//---------------------------------------------------------------------------------
      case LEX_STRING:
         if (c == '"') {
            DEBUG_GET_TOKEN_END;
            return TOKEN_STRING;
         }

         else if ((c == EOF)||(c == '\n')) {
            errno = LEXICAL_ERROR;
            DEBUG_GET_TOKEN_END;
            return TOKEN_BROKEN;
         }

         else
            T_StringAppend(token,c);

      break;
//---------------------------------------------------------------------------------
      case LEX_EXCLAMATION:
         if (c == '=') {
            DEBUG_GET_TOKEN_END;
            return TOKEN_UNEQUAL;
         }

         else {
            ungetc(c,fd);
            errno = LEXICAL_ERROR;
            DEBUG_GET_TOKEN_END;
            return TOKEN_BROKEN;
         }
      break;
//---------------------------------------------------------------------------------
      case LEX_EQUAL:
         if (c == '=') {
            DEBUG_GET_TOKEN_END;
            return TOKEN_EQUAL;
         }

         else {
            ungetc(c,fd);
            DEBUG_GET_TOKEN_END;
            return TOKEN_ASSIGN;
         }
       break;
//---------------------------------------------------------------------------------
     case LEX_SMEQUAL:
        if (c == '=') {
           DEBUG_GET_TOKEN_END;
           return TOKEN_SMEQUAL;
        }

        else {
            ungetc(c,fd);
            DEBUG_GET_TOKEN_END;
            return TOKEN_SMALLER;
        }
      break;
//---------------------------------------------------------------------------------
      case LEX_BEQUAL:
         if (c == '=') {
            DEBUG_GET_TOKEN_END;
            return TOKEN_BEQUAL;
         }

         else {
             ungetc(c,fd);
             DEBUG_GET_TOKEN_END;
             return TOKEN_BIGGER;
         }
      break;
//---------------------------------------------------------------------------------
      case LEX_STAR:
         if (c == '*') {
            DEBUG_GET_TOKEN_END;
            return TOKEN_DSTAR;
         }

         else {
            ungetc(c,fd);
            DEBUG_GET_TOKEN_END;
            return TOKEN_STAR;
         }
      break;
//---------------------------------------------------------------------------------
      case LEX_SLASH:
         if (c == '/') {
            T_StringDelete(token);
            T_StringInit(token);
            state=LEX_LCOMMENT;
         }


         else if (c == '*')
            state=LEX_COMMENT;

         else {
            ungetc(c,fd);
            DEBUG_GET_TOKEN_END;
            return TOKEN_SLASH;
         }
      break;
//---------------------------------------------------------------------------------
      case LEX_LCOMMENT:
         if (c == '\n')
            state = LEX_START;

         else if (c == EOF) {
            DEBUG_GET_TOKEN_END;
            return TOKEN_EOF;
         }
      break;
//---------------------------------------------------------------------------------
      case LEX_COMMENT:
         if (c == '*')
            state = LEX_COMMENT_STAR;

         else if (c == EOF) {
            DEBUG_GET_TOKEN_END;
            return TOKEN_EOF;
         }
      break;
//---------------------------------------------------------------------------------
      case LEX_COMMENT_STAR:
         if (c == '/')
            state = LEX_START;

         else if (c == EOF) {
            DEBUG_GET_TOKEN_END;
            return TOKEN_EOF;
         }

         else
            state = LEX_COMMENT;
      break;
//---------------------------------------------------------------------------------
      default:
         errno = INTERNAL_ERROR;
         DEBUG_GET_TOKEN_END;
         return TOKEN_PROBLEM;
      break;
      }
   }

   // FSM shouldn't get here
   errno = INTERNAL_ERROR;
   DEBUG_GET_TOKEN_END;
   return TOKEN_PROBLEM;
}

int PutBack(int c)
{
   if (ungetc(c, fd) == EOF) {
      errno = INTERNAL_ERROR;
      return EXIT_FAILURE;
   }
   else
      return EXIT_SUCCESS;
}
