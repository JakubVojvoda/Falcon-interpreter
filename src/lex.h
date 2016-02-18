///////////////////////////////////////////////////////////////////////////////
//
//  Falcon Interpreter
//
//  for Formal Languages and Compilers 2012/2013
//  by Varga Tomas, Vojvoda Jakub, Nesvadba Tomas, Spanko Jaroslav, Warzel Adam
//
///////////////////////////////////////////////////////////////////////////////

/*
START
c( )     -> START
c(a)     -> ALNUM
c(_)     -> ALNUM
c(#) 	   -> NUM
c(")     -> STRING
c(!)     -> EXCLAMATION
c(*)     -> STAR
c(/)     -> SLASH
c(,)     -> FINISH, bez ungetc
c(=, +, -, :, (, ), <, >, [, ])
         -> FINISH, bez ungetc
c(EOF)   -> FINISH  -- plati u vsech
else     -> ERROR			DONE

ALNUM
c(a)     -> ALNUM
c(#)     -> ALNUM
c(_)     -> ALNUM
else     -> FINISH			DONE

NUM
c( ,+,-,*,/,:)     -> FINISH
c(#)     -> NUM
c(.)     -> NUM_DEC
c(e)	   -> NUM_EXP
else	   -> ERROR			DONE

NUM_DEC
c( ,+,-,*,/,:)     -> FINISH
c(#)     -> NUM_DEC
else     -> ERROR			DONE

NUM_EXP

c(#)     -> NUM_DEC
c(+)     -> NUM_ONLY
c(-)     -> NUM_ONLY
else     -> ERROR			DONE

NUM_ONLY
c(#)     ->NUM_DEC
else      -> ERROR			DONE
STRING
c(")     -> FINISH
else     -> QUOTE			DONE

EXCLAMATION
c(=)     -> FINISH
else     -> ERROR			DONE

STAR
c(*)     -> FINISH, bez ungetc
else     -> FINISH			???

SLASH
c( )     -> FINISH
c(/)     -> LCOMMENT
c(*)     -> COMMENT
else     -> FINISH			DONE

LCOMMENT
c(EOL)   -> START
else     -> LCOMMENT			DONE

COMMENT
c(*)     -> COMMENT_STAR
else     -> COMMENT			DONE

COMMENT_STAR
c(/)     -> START
else     -> COMMENT			DONE
*/

#ifndef LEX_H_INCLUDED
#define LEX_H_INCLUDED

#include "types.h"
#include "debug.h"

enum { // Token types -- return values
   TOKEN_PROBLEM, // Error during reading
   TOKEN_BROKEN,
   TOKEN_IDENTIFICATOR,
   TOKEN_NUMBER,
   TOKEN_STRING,
   TOKEN_ASSIGN,
   TOKEN_EQUAL,
   TOKEN_UNEQUAL,
   TOKEN_BEQUAL, // Bigger or equal
   TOKEN_SMEQUAL, // Smaller or equal
   TOKEN_SMALLER,
   TOKEN_BIGGER,
   TOKEN_PLUS,
   TOKEN_MINUS,
   TOKEN_STAR,
   TOKEN_DSTAR, // Double star
   TOKEN_SLASH,
   TOKEN_COMMA,
   TOKEN_COLUMN,
   TOKEN_LBRACKET,
   TOKEN_RBRACKET,
   TOKEN_LSBRACKET, // Left square bracket
   TOKEN_RSBRACKET, // Right square bracket
   TOKEN_EOL,
   TOKEN_EOF,
};

int LexInit(char *filename);
void LexFinish();
int PutBack(int c);
int GetToken(T_String *token);

#endif
