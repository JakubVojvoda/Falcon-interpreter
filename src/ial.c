///////////////////////////////////////////////////////////////////////////////
//
//  Falcon Interpreter
//
//  for Formal Languages and Compilers 2012/2013
//  by Varga Tomas, Vojvoda Jakub, Nesvadba Tomas, Spanko Jaroslav, Warzel Adam
//
///////////////////////////////////////////////////////////////////////////////

#include <errno.h>
#include <string.h> // strcpy()
#include <stdlib.h> // malloc()

#include "codes.h"
#include "types.h"
#include "ial.h"
#include "macros.h"
#include "debug.h"

void computeJumps (T_String *substring, int *char_jumps)
{
   int i, k;
   int length = substring->length;

   for (i = 0; i <= MAX_CHARS; i++)
      char_jumps[i] = length;
   for (i = 0; i < length - 1; ++i) {
      k = substring->data[i];
      char_jumps[k] = length - i - 1;
   }
}

int Find(T_Var *str, T_Var *sub_str, T_Var *write_var)
{
   // Basic check for NULL pointers
   PTR_CHECK(str);
   PTR_CHECK(sub_str);
   PTR_CHECK(write_var);

   if (str->NID != NID_STRING || sub_str->NID != NID_STRING) {
      errno = TYPE_COMPATIBILITY_ERROR;
      return EXIT_FAILURE;
   }

   int j = sub_str->vals.str.length - 1;
   int k = sub_str->vals.str.length - 1;
   int q = str->vals.str.length - 1;
   int char_jumps[MAX_CHARS];
   int l;

   computeJumps( &(sub_str->vals.str), char_jumps);

   while (j <= q && k >= 0) {
      if (str->vals.str.data[j] == sub_str->vals.str.data[k]) {
         j--;
         k--;
      }
      else {
         l = str->vals.str.data[j];
         j += char_jumps[l] + 1;
         k = sub_str->vals.str.length - 1;
      }
   }
   if (k < 0) {
      write_var->NID = 2.0;
      write_var->vals.d_val = (double)(j + 1);
   }
   else
      return EXIT_FAILURE;

   return EXIT_SUCCESS;
}

int Sort(T_Var *read_str, T_Var *write_str)
{
   // Basic check for NULL pointers
   PTR_CHECK(read_str);
   PTR_CHECK(write_str);

   // Numeric identificator of a type is string
   if (read_str->NID != NID_STRING)
      return TYPE_COMPATIBILITY_ERROR;

   int i, j;
   char pom;
   int step = read_str->vals.str.length / 2 - 1;
   int length = read_str->vals.str.length - 1;

   // write_str remains same as on input
   strcpy(write_str->vals.str.data, read_str->vals.str.data);

   while (step > 0) {
      for (i = step; i <= length; i++) {
         j = i - step;
         while ( j >= 0 && write_str->vals.str.data[j] > write_str->vals.str.data[j+step] ) {
            pom = write_str->vals.str.data[j+step];
            write_str->vals.str.data[j+step] = write_str->vals.str.data[j];
            write_str->vals.str.data[j] = pom;
            j = j - step;
         }
      }
      step /= 2;
   }
   // Fill structure T_Var
   write_str->NID = NID_STRING;
   write_str->vals.str.length = read_str->vals.str.length;

   return 0;
}

/* Inicializacia hash tabulky.
   Pred inicializaciou -- T_HashTable *table = malloc(sizeof(T_HashTable));
*/
int T_HTableInit(T_HashTable *table)
{
   PTR_CHECK(table);

   for(int i = 0; i < HASH_TABLE_SIZE; i++)
      (*table)[i] = NULL;

   return EXIT_SUCCESS;
}

/* Hlada polozku s rovnakym klucom (string) v hashovacej tabulke,
   ak polozku s rovnakym retazcom najde vracia ukazovatel na tuto polozku,
   inac vracia NULL.
*/
T_HashItem *T_HTableSearch(T_HashTable *table, char *string)
{
   int index = T_KeyCreate(string);
   T_HashItem *item = (*table)[index];

   while (item != NULL) {
      if (!strcmp(string, item->string->data))
         return item;
      else
         item = item->next;
   }

   return NULL;
}

/* Funkcia vlozi polozku s klucom string do tabulky, ak v tabulke existuje polozka
   s rovnakym klucom aktualizuje jej obsah, ak neexistuje prida novu polozku na
   zaciatok zoznamu.
*/
int T_HTableInsert(T_HashTable *table, char *string, T_Var *var)
{
   DEBUG_HASHINSERT(table, string, var);
   T_HashItem *new_item = T_HTableSearch(table, string);

   if (new_item != NULL) // Update existing variable
      return T_VarCopy(new_item->var, var);

   new_item = malloc(sizeof(struct T_HashItem));
   PTR_CHECK(new_item);

   T_String *new_string = malloc(sizeof(T_String));
   if (new_string == NULL) {
      free(new_item);
      errno = INTERNAL_ERROR;
      return EXIT_FAILURE;
   }

   CALL_CHECK(T_StringInit(new_string));
   CALL_CHECK(CStringToT_String(string, new_string));

   new_item->string = new_string;

   if (var != NULL) {
      T_Var *new_var = malloc(sizeof(T_Var));
      if (new_var == NULL) {
         free(new_item);
         free(new_string);
         errno = INTERNAL_ERROR;
         return EXIT_FAILURE;
      }

      CALL_CHECK(T_VarInit(new_var));
      CALL_CHECK(T_VarCopy(new_var, var));

      new_item->var = new_var;
   }
   int index = T_KeyCreate(string);
   new_item->next = (*table)[index];
   (*table)[index] = new_item;

   DEBUG_HASHINSERT_END(table, string, new_item->var);
   return EXIT_SUCCESS;
}

/* Vyhlada polozku s klucom string, ak je najdena naplni parameter var,
   hodnotami polozky hashovacej tabulky a vrati HASH_FOUND, inac zostava
   v nedefinovanom stave a funkcia vracia HASH_NOT_FOUND.
*/
int T_HTableExport(T_HashTable *table, char *string, T_Var **var)
{
   DEBUG_HASHEXPORT(table, string, var);
   T_HashItem *item = T_HTableSearch(table, string);

   if (item != NULL) {
      *var = item->var;
      DEBUG_HASHEXPORT_END(table, string, var);
      return HASH_FOUND;
   }
   else {
      DEBUG_HASHEXPORT_END(table, string, var);
      return HASH_NOT_FOUND;
   }
}

// Zrusi vsetky polozky v tabulke.
void T_HTableDelete (T_HashTable *table)
{
   T_HashItem *item;

   for (int i = 0; i <= HASH_TABLE_SIZE-1; i++) {
      item = (*table)[i];
      while ( (*table)[i] != NULL ) {
         (*table)[i] = (*table)[i]->next;
         free(item);
         item = (*table)[i];
      }
   }
}

/* Hashovacia funkcia, ktora vypocita index zo zadaneho kluca string.
   Je tu snaha aby bol kluc unikatny (implementacia = pokus-omyl).
*/
int T_KeyCreate (char *string)
{
   int l = strlen(string);
   int length = strlen(string);
   int key = 0;

   // create a unique key
   while (length) {
      key += (*string>>string[length-1])^(X * string[length/2]);
      length--;
   }

   key += string[l-1];
   key >>= (string[l-1]%2);

   if (key >= 0) {
      DEBUG_KEYCREAT( key % HASH_TABLE_SIZE);
      return (key % HASH_TABLE_SIZE);
   }
   else {
      DEBUG_KEYCREAT( (-1 * key) % HASH_TABLE_SIZE);
      return ((-1 * key) % HASH_TABLE_SIZE);
   }
}

void T_HashTablePrint(T_HashTable *table)
{
   if (table != NULL) {
      printf("T_HashTable = ");

      for (int i = 0; i < HASH_TABLE_SIZE; i++) {
         if ((*table)[i] != NULL)
            printf("%s, ", (*table)[i]->string->data);
         }
      printf("\n");
   }
}
