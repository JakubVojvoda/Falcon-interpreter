///////////////////////////////////////////////////////////////////////////////
//
//  Falcon Interpreter
//
//  for Formal Languages and Compilers 2012/2013
//  by Varga Tomas, Vojvoda Jakub, Nesvadba Tomas, Spanko Jaroslav, Warzel Adam
//
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h> // realloc()
#include <string.h> // strcpy()
#include <stdio.h> // FILE
#include <errno.h>
#include <stdint.h>

#include "types.h"
#include "codes.h"
#include "macros.h" // PTR_CHECK()
#include "debug.h"
#include "functions.h"
#include "synan.h"

// T_Var ----------------------------------------------------------------------
int T_VarInit(T_Var *var)
{
   DEBUG_VAR_INIT(var);
   PTR_CHECK(var);

	var->NID = NID_UNDEF;

   DEBUG_VAR_INIT_END(var);
   return EXIT_SUCCESS;
}

void T_VarDelete(T_Var *var)
{
   DEBUG_VAR_DELETE(var);

   if (var != NULL) {
      if (var->NID == NID_STRING)
         T_StringDelete(&var->vals.str);

      var->NID = NID_UNDEF;
   }

   DEBUG_VAR_DELETE_END(var);
}

int T_VarCopy(T_Var *write_var, T_Var *read_var)
{
   DEBUG_VAR_COPY(write_var, read_var);
   PTR_CHECK(read_var);
   PTR_CHECK(write_var);

   T_VarDelete(write_var);
   T_VarInit(write_var);

   DEBUG_VAR_COPY_PRECOPY(write_var, read_var);
   if (read_var->NID == NID_STRING)
      T_StringToT_Var(&(read_var->vals.str), write_var);

   else if (write_var->NID == NID_CONST) {
      errno = SEMANTIC_ERROR;
      return EXIT_FAILURE;
   }

   else
      memcpy(write_var, read_var, sizeof(T_Var));

   DEBUG_VAR_COPY_POSTCOPY(write_var, read_var);
   return EXIT_SUCCESS;
}

int CDoubleToT_Var(double val, T_Var *var)
{
   DEBUG_DOUBLE2VAR(val, var);
   PTR_CHECK(var);

	var->NID = NID_NUMBER;
	var->vals.d_val = val;

   DEBUG_DOUBLE2VAR_END(val, var);
   return EXIT_SUCCESS;
}

int T_StringToT_Var(T_String *str, T_Var *var)
{
   DEBUG_STRING2VAR(str, var);
   PTR_CHECK(str);
   PTR_CHECK(var);

	var->NID = NID_STRING;
	var->vals.str.length = str->length;

   size_t req_size = str->length + TSTRING_SIZE - (str->length % TSTRING_SIZE);
	char *tmp = malloc(req_size);
	PTR_CHECK(tmp);

	var->vals.str.data = tmp;
	memcpy(var->vals.str.data, str->data, req_size);

   DEBUG_STRING2VAR_END(str, var);
   return EXIT_SUCCESS;
}

int IsFunction(T_Var *var)
{
   return (var->NID == NID_FUNCTION);
}

void T_VarPrint(T_Var *var)
{
   if (var == NULL) {
      printf("  NULL\n");
      return;
   }

  printf("  T_Var:  ");

  switch ((int)(var->NID)) {
    case -2:
      printf("  Konstanta(%p)\n", (void *)var);
      break;
    case -1:
      printf("  Nedefinovana(%p)\n", (void *)var);
      break;
    case 0:
      printf("  NIL(%p)\n", (void *)var);
      break;
    case 1:
      printf("  Boolean(%p) = %s\n", (void *)var, (var->vals.b_val != 0 ? "TRUE" : "FALSE") );
      break;
    case 3:
      printf("  Number(%p) = %f\n", (void *)var, var->vals.d_val);
      break;
    case 6:
      printf("  Function(%p): size=%d, ", (void *)var, var->vals.args.size);
      T_ArgListPrint(&(var->vals.args));
      break;
    case 8:
      printf("  String(%p) = \"%s\"\n", (void *)var, var->vals.str.data);
      break;
    default:
      printf("  Unknown NID(%p)\n", (void *)var);
      break;
  }
}

// T_String -------------------------------------------------------------------
int T_StringInit(T_String *str)
{
   DEBUG_STRINGINIT(str);
   PTR_CHECK(str);

   str->length = 0;
   if ((str->data = malloc(sizeof(char)*TSTRING_SIZE)) == NULL) {
      errno = INTERNAL_ERROR;
      return EXIT_FAILURE;
   }
   str->data[0] = '\0';

   DEBUG_STRINGINIT_END(str);
   return EXIT_SUCCESS;
}

void T_StringDelete(T_String *str)
{
   DEBUG_STRING_DELETE(str);
   if (str != NULL) {
      if (str->data != NULL) {
         free(str->data);
         str->data = NULL;
      }
      str->length = 0;
   }

   DEBUG_STRING_DELETE_END(str);
}

int T_StringCopy(T_String *dest, T_String *src)
{
   DEBUG_STRING_COPY_0(dest, src);
   PTR_CHECK(dest);
   PTR_CHECK(src);
   DEBUG_STRING_COPY_1(dest,src);

   T_StringDelete(dest);
   size_t  req_size = src->length / TSTRING_SIZE * TSTRING_SIZE + TSTRING_SIZE;
   dest->data = malloc(req_size);
   memcpy(dest->data, src->data, req_size);

   dest->length = src->length;
   DEBUG_STRING_COPY_2(dest,src);
   return EXIT_SUCCESS;
}

uint32_t T_StringRead(FILE *fd, T_String *str, char end)
{
   DEBUG_STRINGREAD_START(str);
   PTR_CHECK(fd);

   if (str == NULL)
      return fscanf(fd, "%*s");

   // Checks whether string isn't already full, flushes it in that case
   if (str->length > 0) {
      T_StringDelete(str);
      T_StringInit(str);
   }

   // Reads characters from a file and saves it dynamically inside T_String
   for (int c = fgetc(fd); !errno && c != EOF && c != end; c = fgetc(fd)) {
      str->data[str->length] = c;
      str->length++;

      // Extends allocated size when needed
      if ((str->length > 0) && !(str->length % TSTRING_SIZE)) {
         if (realloc(str->data, (sizeof(char)*str->length)+(sizeof(char)*TSTRING_SIZE)) == NULL) {
            errno = INTERNAL_ERROR;
            return EXIT_SUCCESS;
         }
      }
   }

   // Adds string termination symbol
   str->data[str->length] = '\0';

   // An error has occured during reading an opened file
   if (ferror(fd))
      errno=INTERNAL_ERROR;

   DEBUG_STRINGREAD_END(str);
   return str->length;
}

// Prida znak na konec retezce
int T_StringAppend(T_String *str, int c)
{
   DEBUG_STRINGAPPEND_START(str);
   PTR_CHECK(str);

   DEBUG_STRINGAPPEND(str,c);
   // Prida znak
   str->data[str->length] = c;
   str->length++;

   if ((str->length > 0) && !(str->length % TSTRING_SIZE)) {
      char *nove;
      if ((nove = realloc(str->data, (sizeof(char)*str->length)+(sizeof(char)*TSTRING_SIZE))) == NULL) {
         errno = INTERNAL_ERROR;
         return EXIT_FAILURE;
      }
      str->data = nove;
   }

   str->data[str->length] = '\0';
   DEBUG_STRINGAPPEND_END(str);
   return EXIT_SUCCESS;
}

// Konkatenace nasich stringu, Adam Warzel
int T_StringCat(T_String *dest, T_String *src)
{
   DEBUG_STRINGCAT_START(dest,src);
   PTR_CHECK(dest);
   PTR_CHECK(src);

   DEBUG_STRINGCAT(src);
   // Zvetsi alokovany prostor v destinaci, aby se tam veslo i src, opet lehka vykradacka Vudce
   if ((src->length > 0)) {
    char *nove;
    int len = dest->length + src->length; //delka spojenych stringu
    // realokace destinace na spravnou delku odpovidajici nasobku osmi
         if ((nove = realloc(dest->data, (sizeof(char) * (len + TSTRING_SIZE - (len % TSTRING_SIZE))))) == NULL) {
            errno = INTERNAL_ERROR;
            return EXIT_FAILURE;
         }
         dest->data = nove;
   }

   //samotna konkatenace a zmena informace o velikosti
   dest->length += src->length;
   strcat(dest->data, src->data);

   DEBUG_STRINGCAT_END(dest,src);
   return EXIT_SUCCESS;
}

int CStringToT_String(char *c_str, T_String *str)
{
   DEBUG_CSTR2TSTR(c_str,str);
   PTR_CHECK(c_str);
   PTR_CHECK(str);

   T_StringDelete(str);

   size_t req_size = strlen(c_str) / TSTRING_SIZE * TSTRING_SIZE + TSTRING_SIZE;

   str->data = malloc(req_size);
   PTR_CHECK(str->data);

   strcpy(str->data, c_str);

   str->length = strlen(c_str);

   DEBUG_CSTR2TSTR_END(c_str,str);
   return EXIT_SUCCESS;
}

/*
// T_Stack --------------------------------------------------------------------
int T_StackInit(T_Stack *stack)
{
   PTR_CHECK(stack);

   // Allocate space for pointer array
   if ((stack->item = malloc(TSTACK_SIZE * sizeof(T_Var *))) == NULL) {
      errno = INTERNAL_ERROR;
      return EXIT_FAILURE;
   }

   // Allocate space for array items
   for (int i = 0; i < TSTACK_SIZE; i++) {
      if ((stack->item[i] = malloc(sizeof(T_Var))) == NULL) {
         errno = INTERNAL_ERROR;
         return EXIT_FAILURE;
      }
   }

   stack->size = 0;
   stack->index = 0;
   stack->pointer = *(stack->item);

   return EXIT_SUCCESS;
}

int TStack_Delete(T_Stack *stack)
{
   PTR_CHECK(stack);

   for (uint32_t i = stack->size; i > 0; i--) {
      T_VarDelete(stack->item[i]);
   }
   T_VarDelete(stack->item[0]);
   free(stack->item);

   stack->pointer = NULL;

   free(stack);

   return EXIT_SUCCESS;
}

int T_StackPush(T_Stack *stack, T_Var *item)
{
   PTR_CHECK(stack);
   PTR_CHECK(item);

   uint32_t size = stack->size;

   if ((size > 0) && ((size + 1) % TSTACK_SIZE)) {
      T_StackExtend(stack);
      if (errno)
         return EXIT_FAILURE;
   }

   T_Var *dest = stack->item[size];
   memcpy(dest, item, sizeof(T_Var));

   stack->size++;
   stack->index++;

   return EXIT_SUCCESS;
}

int T_StackPop(T_Stack *stack, T_Var *item)
{
   PTR_CHECK(stack);
   PTR_CHECK(item);

   T_Var *top = stack->pointer + stack->index;

   item = top;

   top->NID = NID_NIL;
   stack->size--;
   stack->index--;

   return EXIT_SUCCESS;
}

int T_StackExtend(T_Stack *stack)
{
   PTR_CHECK(stack);

   uint32_t space = stack->size / TSTACK_SIZE + 1;

   // Allocate space for pointer array
   if ((realloc(stack->item, space * sizeof(T_Var *))) == NULL) {
      errno = INTERNAL_ERROR;
      return EXIT_FAILURE;
   }

   // Allocate space for array items
   for (int i = 0; i < TSTACK_SIZE; i++) {
      if ((realloc(stack->item[i], sizeof(T_Var))) == NULL) {
         errno = INTERNAL_ERROR;
         return EXIT_FAILURE;
      }
   }

   // Everything went well
   return EXIT_SUCCESS;
}
*/

// T_List ---------------------------------------------------------------------

void T_InstrPrint(T_Instr *instr)
{
   printf("  Operacia = %d(%p)\n",instr->IID, (void *)instr);
   printf("  Arg1 = ");
   T_VarPrint(instr->arg1);
   printf("  Arg2 = ");
   T_VarPrint(instr->arg2);
   printf("  Arg3 = ");
   T_VarPrint(instr->arg3);
}

void T_ListPrint(T_List *list)
{
  if (list == NULL)
    return;

  T_ListItem *hitem = list->first;

  while (hitem != NULL) {
    T_InstrPrint(hitem->data);
    hitem = hitem->next;
  }
  printf("\n");
}

int T_ListInit(T_List *list)
{
   DEBUG_TLISTINIT(list);
   list->first = NULL;
   list->actual = NULL;
   list->last = NULL;

   DEBUG_TLISTINIT_END(list);
   return EXIT_SUCCESS;
}

void T_ListDelete(T_List *list)
{
   DEBUG_TLISTDELETE(list);
   if (list != NULL) {
      T_ListItem *pom;
      while (list->first != NULL) {
         pom = list->first;
         list->first = pom->next;
         free(pom);
      }

      list->actual = NULL;
      list->last = NULL;
   }
   DEBUG_TLISTDELETE_END(list);
}

int T_ListSucc(T_List *list)
{
   DEBUG_TLISTSUCC(list);
   list->actual = list->actual->next;
   DEBUG_TLISTSUCC_END(list);
   return EXIT_SUCCESS;
}

int T_ListAppend(T_List *list, T_Instr *instr)
{
   DEBUG_TLISTAPPEND_START(list, instr);
   PTR_CHECK(list);
   PTR_CHECK(instr);

   DEBUG_TLISTAPPEND(list, instr);
   T_ListItem *pom;
   pom = malloc(sizeof(T_ListItem));

   PTR_CHECK(pom);

   pom->data = malloc(sizeof(T_Instr));
   memcpy(pom->data, instr, sizeof(T_Instr));

   pom->next = NULL;

   if (list->first == NULL) {
     list->first = pom;
     list->last = pom;
   }
   else {
     list->last->next = pom;
     list->last = pom;
   }
   DEBUG_TLISTAPPEND_END(list, instr);

   return EXIT_SUCCESS;
}


/* Inicializuje zoznam tabuliek identifikatorov.
*/
int T_IdListInit(T_IdList *list)
{
   PTR_CHECK(list);

   list->first = NULL;
   list->last = NULL;
   list->actual = NULL;

   return EXIT_SUCCESS;
}

/* Zrusi cely zoznam tabuliek identifikatorov.
*/
void T_IdListDelete(T_IdList *list)
{
   if (list != NULL) {
      for (T_IdItem *tmp = list->first; tmp != NULL; tmp = tmp->next)
         free(tmp);
      list->actual = NULL;
      list->first = NULL;
      list->last = NULL;
   }
}

/* Prida novu polozku na koniec zoznamu a ukazovatel data
   ukazuje na hashovaciu tabulku str.
*/
int T_IdListAppend(T_IdList *list, T_HashTable *str)
{
   PTR_CHECK(list);
   PTR_CHECK(str);

   T_IdItem *new_item;

   new_item = malloc(sizeof(struct T_IdentificatorItem));
   PTR_CHECK(new_item);

   new_item->data = str;
   new_item->previous = list->last;
   new_item->next = NULL;

   if (list->last == NULL) {
      list->first = new_item;
      list->last = new_item;
   }
   else {
      list->last->next = new_item;
      list->last = new_item;
   }

   return EXIT_SUCCESS;
}

/* Zrusi poslednu polozku zoznamu.
*/
void T_IdListDeleteLast(T_IdList *list)
{
   if (list->last == NULL)
      return;

   struct T_IdentificatorItem *hitem;
   hitem = list->last;

   if (list->actual == list->last)
      list->actual = NULL;

   if (list->first == list->last) {
      list->first = NULL;
      list->last = NULL;
   }
   else {
      list->last = list->last->previous;
      list->last->next = NULL;
   }

   free(hitem);
}

// T_Arglist-------------------------------------------------------------------

void T_ArgListPrint(T_ArgList *list)
{
   if (list == NULL)
      return;

   T_ArgItem *hitem = list->first;
   for (int i = 0; i < list->size; i++) {
      if (hitem != NULL) {
         printf("  %d: %s, \n", i++, hitem->data.data);
         hitem = hitem->next;
      }
      else
         break;
   }
}

int T_ArgListInit(T_ArgList *list)
{
   DEBUG_TARGLISTINIT(list);
   PTR_CHECK(list);

   list->actual = NULL;
   list->first = NULL;
   list->last = NULL;

   list->size = 0;

   DEBUG_TARGLISTINIT_END(list);
   return EXIT_SUCCESS;
}

void T_ArgListDelete(T_ArgList *list)
{
   DEBUG_TARGLISTDELETE(list);

   if (list != NULL) {
      for (T_ArgItem *tmp = list->first; tmp != NULL; tmp = tmp->next)
         free(tmp);
      list->actual = NULL;
      list->first = NULL;
      list->last = NULL;
      list->size = 0;
   }

   DEBUG_TARGLISTDELETE_END(list);
}

int T_ArgListSucc(T_ArgList *list)
{
   DEBUG_TARGLISTSUCC(list);
   list->actual = list->actual->next;

   DEBUG_TARGLISTSUCC_END(list);
   return EXIT_SUCCESS;
 }

int T_ArgListAppend(T_ArgList *list, T_String *instr)
{
   DEBUG_TARGLISTAPPEND_START(list, instr);
   PTR_CHECK(list);
   PTR_CHECK(instr);
   DEBUG_TARGLISTAPPEND(list, instr);

   T_ArgItem *pomocna = malloc(sizeof(T_ArgItem));

   PTR_CHECK(pomocna);

   pomocna->data = *instr;
   pomocna->next = NULL;

   if (list->first != NULL) {
     list->last->next = pomocna;
     list->last = pomocna;
   }

   else {
     list->first = pomocna;
     list->last = pomocna;
   }

   DEBUG_TARGLISTAPPEND_END(list, instr);
   return EXIT_SUCCESS;
}

// T_AllocList ----------------------------------------------------------------

void T_AllocListPrint(T_AllocList *list)
{
   if (list == NULL)
      return;

   int i = 0;
   T_AllocItem *hitem = list->first;

   while (hitem != NULL) {
      printf("  %d: addr=%p, ",i++,hitem->ptr);
      hitem = hitem->next;
   }
   printf("\n");
}

int T_AllocListInit(T_AllocList *list)
{
  DEBUG_TALLOCLISTINIT(list);
  PTR_CHECK(list);

  list->first = NULL;
  list->last = NULL;

  DEBUG_TALLOCLISTINIT_END(list);
  return EXIT_SUCCESS;
}

int T_AllocListAppend(T_AllocList *list, void *item)
{
   DEBUG_TALLOCLISTAPPEND_START(list,item);
   PTR_CHECK(list);
   PTR_CHECK(item);
   DEBUG_TALLOCLISTAPPEND(list,item);

   T_AllocItem *new_item = malloc(sizeof(T_AllocItem));
   PTR_CHECK(new_item);

   new_item->ptr = item;
   new_item->next = NULL;

   if (list->last == NULL) {
      list->first = new_item;
      list->last = new_item;
   }
   else {
      list->last->next = new_item;
      list->last = new_item;
   }

   DEBUG_TALLOCLISTAPPEND_END(list,item);
   return EXIT_SUCCESS;
}

void T_AllocListDeleteLast(T_AllocList *list)
{
   DEBUG_TALLOCLISTDELETELAST_START(list);
   if (list != NULL && list->last != NULL) {
      DEBUG_TALLOCLISTDELETELAST(list);
      T_AllocItem *hitem = list->last;

   if (list->first == list->last) {
      list->first = NULL;
      list->last = NULL;
   }
   else {
      list->last = list->first;

      while (list->last->next != hitem)
         list->last = list->last->next;

      free(hitem);
    }
  }
  DEBUG_TALLOCLISTDELETELAST_END(list);
}

void T_AllocListDelete(T_AllocList *list)
{
   DEBUG_TALLOCLISTDELETE_START(list);
   if (list != NULL) {
      while (list->first != list->last)
         T_AllocListDeleteLast(list);

      free(list->first);
      list->first = NULL;
      list->last = NULL;
   }
   DEBUG_TALLOCLISTDELETE_END(list);
}

/* Funkcia vybere podretazec z retazca od indexu from do indexu to.
   Vrati ukazovatel na dany podretazec T_String alebo NULL pri chybe.
*/
int CutString(T_String *substring, T_String *string, double from, double to)
{
   from = (int)from;
   to = (int)to;

   PTR_CHECK(substring);

   substring->data = malloc(sizeof(char)*TSTRING_SIZE);
   PTR_CHECK(substring->data);

   if (from >= to || string->length < to) {
      substring->data = "\0";
      substring->length = 0;
   }
   else {
      for (int j = 0, i = from; i < to; i++, j++)
         T_StringAppend(substring, string->data[i]);

      T_StringAppend(substring, '\0');
      substring->length = strlen(substring->data);
   }
   return EXIT_SUCCESS;
}

// Label list --------------------------------------------------------
int T_LabelListInit(T_LabelList *list)
{
   PTR_CHECK(list);

   list->first = NULL;
   list->last = NULL;

   return EXIT_SUCCESS;
}

void T_LabelListDelete(T_LabelList *list)
{
   if (list != NULL) {
      T_LabelItem *pom;
      while (list->first != NULL) {
         pom = list->first;
         list->first = pom->next;
         //free(&(pom->ptr_label));
         free(pom);
      }
      list->last = NULL;
   }
}

int T_LabelListAppend(T_LabelList *list, T_ListItem *label)
{
   PTR_CHECK(list);
   PTR_CHECK(label);

   T_LabelItem *new_item;
   new_item = malloc(sizeof(struct T_LabelItem));
   PTR_CHECK(new_item);

   new_item->ptr_label = malloc(sizeof(T_Instr*));
   (*new_item->ptr_label) = label;
   new_item->next = NULL;

   if (list->first == NULL) {
     list->first = new_item;
     list->last = new_item;
   }
   else {
     list->last->next = new_item;
     list->last = new_item;
   }

   return EXIT_SUCCESS;
}
