///////////////////////////////////////////////////////////////////////////////
//
//  Falcon Interpreter
//
//  for Formal Languages and Compilers 2012/2013
//  by Varga Tomas, Vojvoda Jakub, Nesvadba Tomas, Spanko Jaroslav, Warzel Adam
//
///////////////////////////////////////////////////////////////////////////////

#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

#include <stdio.h>

int debug;

void DebugInit();

// T_Var ----------------------------------------------------------------------
#define DEBUG_VAR_INIT(var) \
if (debug) printf("---- VAR INIT START ----\n" \
                  "  Promenna na adrese: %p\n", (void*)var)

#define DEBUG_VAR_INIT_END(var) do { \
   if (debug) { \
      printf("  Vypis promenne po inicializaci:\n"); \
      T_VarPrint(var); \
      printf("---- VAR INIT END ----\n"); \
   } \
} while (0)

#define DEBUG_VAR_DELETE(var) \
if (debug) printf("---- VAR DELETE START ----\n" \
                  "  Promenna na adrese: %p\n", (void *)var)

#define DEBUG_VAR_DELETE_END(var) do { \
   if (debug) { \
      printf("  Vypis promenne pred smazanim:\n"); \
      T_VarPrint(var); \
      printf("  Vypis promenne po smazani:\n"); \
      T_VarPrint(var); \
      printf("---- VAR DELETE END ----\n"); \
   } \
} while (0)

#define DEBUG_VAR_COPY(var1, var2) do { \
   if (debug) { \
      printf("---- VAR COPY START ----\n" \
      "  Cilova promenna na adrese: %p\n" \
      "  Zdrojova promenna na adrese: %p\n", (void *)var1, (void *)var2); \
   } \
} while (0)

#define DEBUG_VAR_COPY_PRECOPY(var1, var2) do { \
   if (debug) { \
      printf("  Obsah promennych pred kopirovanim\n"); \
      T_VarPrint(var1); \
      T_VarPrint(var2); \
   } \
} while (0)

#define DEBUG_VAR_COPY_POSTCOPY(var1, var2) do { \
   if (debug) { \
      printf("  Obsah promennych po kopirovani\n"); \
      T_VarPrint(var1); \
      T_VarPrint(var2); \
      printf("---- VAR COPY END ----\n"); \
   } \
} while (0)

#define DEBUG_DOUBLE2VAR(val, var) do { \
   if (debug) { \
      printf("---- DOUBLE TO VAR ----\n" \
             "  Zmena promenne na adrese: %p\n" \
             "  Obsah pred zmenou\n", (void *)var); \
      T_VarPrint(var); \
   } \
} while (0)

#define DEBUG_DOUBLE2VAR_END(val, var) do { \
   if (debug) { \
      printf("  Obsah po zmene\n"); \
      T_VarPrint(var); \
      printf("---- DOUBLE TO VAR END ----\n"); \
   } \
} while (0)

#define DEBUG_STRING2VAR(str, var) do { \
   if (debug) { \
      printf("---- STRING TO VAR ----\n" \
             "  Zmena promenne na adrese: %p\n" \
             "  Obsah pred zmenou\n", (void *)var); \
      T_VarPrint(var); \
   } \
} while (0)

#define DEBUG_STRING2VAR_END(str, var) do { \
   if (debug) { \
      printf("  Obsah po zmene\n"); \
      T_VarPrint(var); \
      printf("---- STRING TO VAR END ----\n"); \
   } \
} while (0)

// T_String -------------------------------------------------------------------
#define DEBUG_STRINGINIT(str) \
if (debug) printf("---- T_STRING INIT START ----\n" \
                  "  Retezec na adrese: %p\n", (void *)str)

#define DEBUG_STRINGINIT_END(str) do { \
   if (debug) { \
      printf("  Vypis retezce po inicializaci:\n"); \
      printf("  T_String: \"%s\", size=%d\n", str->data, str->length); \
      printf("---- T_STRING INIT END ----\n"); \
   } \
} while (0)

#define DEBUG_STRING_DELETE(str) \
if (debug) printf("---- T_STRING DELETE START ----\n" \
                  "Retezec na adrese: %p\n", (void *)str)

#define DEBUG_STRING_DELETE_END(str) do { \
   if (debug) { \
      printf("  Vypis retezce pred smazanim:\n"); \
      printf("  T_String: \"%s\", size=%d\n", str->data, str->length); \
      printf("  Vypis retezce po smazani:\n"); \
      printf("  T_String: \"%s\", size=%d\n", str->data, str->length); \
      printf("---- T_STRING DELETE END ----\n"); \
   } \
} while (0)

#define DEBUG_STRING_COPY_0(str1, str2) \
if (debug) printf("---- T_STRING COPY START ----\n" \
                  "  Cilovy retezec na adrese: %p\n" \
                  "  Zdrojovy retezec na adrese: %p\n", (void *)str1, (void *)str2)

#define DEBUG_STRING_COPY_1(str1, str2) do { \
   if (debug) { \
      printf("  Obsah promennych pred kopirovanim:\n"); \
      printf("  dest : \"%s\", size=%d\n", str1->data, str1->length); \
      printf("  src  : \"%s\", size=%d\n", str2->data, str2->length); \
   } \
} while (0)

#define DEBUG_STRING_COPY_2(str1, str2) do { \
   if (debug) { \
      printf("  Obsah promennych po kopirovani:\n"); \
      printf("  dest : \"%s\", size=%d\n", str1->data, str1->length); \
      printf("  src  : \"%s\", size=%d\n", str2->data, str2->length); \
      printf("---- T_STRING COPY END ----\n"); \
   } \
} while (0)

#define DEBUG_STRINGREAD_START(str) \
if (debug) printf("---- T_STRING READ START ----\n" \
                  "  Promenna na adrese: %p\n", (void *)str)

#define DEBUG_STRINGREAD_END(str) do { \
   if (debug) { \
      printf("  Obsah prom. po citani zo suboru\n"); \
      printf("  T_String : \"%s\", size=%d\n", str->data, str->length); \
      printf("---- T_STRING READ END ----\n"); \
   } \
} while (0)

#define DEBUG_STRINGAPPEND_START(str) \
if (debug) printf("---- T_STRING APPEND START ----\n" \
                  "  Promenna na adrese: %p\n", (void *)str)

#define DEBUG_STRINGAPPEND(str,c) do { \
   if (debug) { \
      printf("  Obsah prom. pred pridanim\n"); \
      printf("  T_String : \"%s\", size=%d\n", str->data, str->length); \
      printf("  Prida na koniec : %d\n", c); \
   } \
} while (0)

#define DEBUG_STRINGAPPEND_END(str) do { \
   if (debug) { \
      printf("  Obsah prom. po pridani\n"); \
      printf("  T_String : \"%s\", size=%d\n", str->data, str->length); \
      printf("---- T_STRING APPEND END ----\n"); \
   } \
} while (0)

#define DEBUG_STRINGCAT_START(dest, src) \
if (debug) printf("---- T_STRING CAT START ----\n" \
                  "  Cilova promenna na adrese: %p\n" \
                  "  Zdrojova promenna na adrese: %p\n", (void *)dest, (void *)src)

#define DEBUG_STRINGCAT(str) do { \
   if (debug) { \
      printf("  Obsah promennych pred konkatenaciou\n"); \
      printf("  dest : \"%s\", size=%d\n", dest->data, dest->length); \
      printf("  src  : \"%s\", size=%d\n", src->data, src->length); \
   } \
} while (0)

#define DEBUG_STRINGCAT_END(str1,str2) do { \
   if (debug) { \
      printf("  Obsah promennych po konkatenacii\n"); \
      printf("  dest : \"%s\", size=%d\n", str1->data, str1->length); \
      printf("  src  : \"%s\", size=%d\n", str2->data, str2->length); \
      printf("---- T_STRING CAT END ----\n"); \
   } \
} while (0)

#define DEBUG_CSTR2TSTR(cstr, str) do { \
   if (debug) { \
      printf("---- STRING TO T_STRING ----\n" \
             "  Zmena promenne na adrese: %p\n" \
             "  Obsah pred zmenou\n" \
             "  String: \"%s\"\n", (void *)str, cstr); \
   } \
} while (0)

#define DEBUG_CSTR2TSTR_END(cstr, str) do { \
   if (debug) { \
      printf("  Obsah po zmene\n"); \
      printf("  T_String: \"%s\", size=%d\n",str->data, str->length); \
      printf("---- STRING TO T_STRING END ----\n"); \
   } \
} while (0)

// T_List ---------------------------------------------------------------------
#define DEBUG_TLISTINIT(list) \
if (debug) printf("---- T_LIST INIT START ----\n" \
                  "  T_List na adrese: %p\n", (void *)list)

#define DEBUG_TLISTINIT_END(list) do { \
   if (debug) { \
      printf("  Vypis seznamu po inicializaci:\n"); \
      T_ListPrint(list); \
      printf("---- T_LIST INIT END ----\n"); \
   } \
} while (0)

#define DEBUG_TLISTDELETE(list) \
if (debug) printf("---- T_LIST DELETE START ----\n" \
                  "  T_List na adrese: %p\n", (void *)list)

#define DEBUG_TLISTDELETE_END(list) do { \
   if (debug) { \
      printf("  Vypis promenne pred smazanim:\n"); \
      T_ListPrint(list); \
      printf("  Vypis promenne po smazani:\n"); \
      T_ListPrint(list); \
      printf("---- T_LIST DELETE END ----\n"); \
   } \
} while (0)

#define DEBUG_TLISTSUCC(list) \
if (debug) printf("---- T_LIST SUCC START ----\n" \
                  "  Actual na adrese: %p\n", (void *)list->actual)

#define DEBUG_TLISTSUCC_END(list) do { \
   if (debug) { \
      printf("  Actual na adrese: %p po Succ.\n",(void *)list->actual); \
      printf("---- T_LIST SUCC END ----\n"); \
   } \
} while (0)

#define DEBUG_TLISTAPPEND_START(list,instr) \
if (debug) printf("---- T_LIST APPEND START ----\n" \
                  "  T_List na adrese: %p Pridavana instr. na: %p\n", (void *)list, (void *)instr)

#define DEBUG_TLISTAPPEND(list,instr) do { \
   if (debug) { \
      printf("  Obsah zoznamu pred pridanim\n"); \
      T_ListPrint(list); \
      printf("  Prida na koniec instrukciu:\n"); \
      T_InstrPrint(instr); \
   } \
} while (0)

#define DEBUG_TLISTAPPEND_END(list,instr) do { \
   if (debug) { \
      printf("  Obsah zoznamu po pridani\n"); \
      T_ListPrint(list); \
      printf("---- T_LIST APPEND END ----\n"); \
   } \
} while (0)

// T_Arglist-------------------------------------------------------------------
#define DEBUG_TARGLISTINIT(list) \
if (debug) printf("---- T_ARGLIST INIT START ----\n" \
                  "  T_Arglist na adrese: %p\n", (void *)list)

#define DEBUG_TARGLISTINIT_END(list) do { \
   if (debug) { \
      printf("  Vypis promenne po inicializaci:\n"); \
      T_ArgListPrint(list); \
      printf("---- T_ARGLIST INIT END ----\n"); \
   } \
} while (0)

#define DEBUG_TARGLISTDELETE(list) \
if (debug) printf("---- T_ARGLIST DELETE START ----\n" \
                  "  T_Arglist na adrese: %p\n", (void *)list)

#define DEBUG_TARGLISTDELETE_END(list) do { \
   if (debug) { \
      printf("  Vypis promenne pred smazanim:\n"); \
      T_ArgListPrint(list); \
      printf("  Vypis promenne po smazani:\n"); \
      T_ArgListPrint(list); \
      printf("---- T_ARGLIST DELETE END ----\n"); \
   } \
} while (0)

#define DEBUG_TARGLISTSUCC(list) \
if (debug) printf("---- T_ARGLIST SUCC START ----\n" \
                  "  Actual na adrese: %p\n", (void *)list->actual)

#define DEBUG_TARGLISTSUCC_END(list) do { \
   if (debug) { \
      printf("  Actual na adrese: %p po Succ.\n",(void *)list->actual); \
      printf("---- T_ARGLIST INIT END ----\n"); \
   } \
} while (0)

#define DEBUG_TARGLISTAPPEND_START(list,instr) \
if (debug) printf("---- T_ARGLIST APPEND START ----\n" \
                  "  T_ArgList na adrese: %p Pridavana instr. na: %p\n", (void *)list, (void *)instr)

#define DEBUG_TARGLISTAPPEND(list,instr) do { \
   if (debug) { \
      printf("  Obsah arg. zoznamu pred pridanim\n"); \
      T_ArgListPrint(list); \
      printf("  Prida na koniec:\n"); \
      printf("  T_String: \"%s\", length=%d\n",instr->data, instr->length); \
   } \
} while (0)

#define DEBUG_TARGLISTAPPEND_END(list,instr) do { \
   if (debug) { \
      printf("  Obsah arg. zoznamu po pridani\n"); \
      T_ArgListPrint(list); \
      printf("---- T_ARGLIST APPEND END ----\n"); \
   } \
} while (0)


// T_AllocList ----------------------------------------------------------------
#define DEBUG_TALLOCLISTINIT(list) \
if (debug) printf("---- T_ALLOCLIST INIT START ----\n" \
                  "  T_AllocList na adrese: %p\n", (void *)list)

#define DEBUG_TALLOCLISTINIT_END(list) do { \
   if (debug) { \
      printf("  Vypis promenne po inicializaci:\n"); \
      T_AllocListPrint(list); \
      printf("---- T_ALLOCLIST INIT END ----\n"); \
   } \
} while (0)

#define DEBUG_TALLOCLISTDELETE_START(list) \
if (debug) printf("---- T_ALLOCLIST DELETE START ----\n" \
                  "  T_AllocList na adrese: %p\n", (void *)list)

#define DEBUG_TALLOCLISTDELETE_END(list) do { \
   if (debug) { \
      printf("  Vypis promenne pred smazanim:\n"); \
      T_AllocListPrint(list); \
      printf("  Vypis promenne po smazani:\n"); \
      T_AllocListPrint(list); \
      printf("---- T_ALLOCLIST DELETE END ----\n"); \
   } \
} while (0)

#define DEBUG_TALLOCLISTAPPEND_START(list,instr) \
if (debug) printf("---- T_ALLOCLIST APPEND START ----\n" \
                  " T_AllocList na adrese: %p Pridavana uk. na adrese: %p\n", (void *)list, (void *)instr)

#define DEBUG_TALLOCLISTAPPEND(list,instr) do { \
   if (debug) { \
      printf("  Obsah alloc. zoznamu pred pridanim\n"); \
      T_AllocListPrint(list); \
      printf("  Prida na koniec: void*: %p",(void *)instr); \
   } \
} while (0)

#define DEBUG_TALLOCLISTAPPEND_END(list,instr) do { \
   if (debug) { \
      printf("  Obsah alloc. zoznamu po pridani\n"); \
      T_AllocListPrint(list); \
      printf("---- T_ALLOCLIST APPEND END ----\n"); \
   } \
} while (0)

#define DEBUG_TALLOCLISTDELETELAST_START(list) \
if (debug) printf("---- T_ALLOCLIST DELETE_LAST START ----\n" \
                  "  T_AllocList na adrese: %p\n", (void *)list)

#define DEBUG_TALLOCLISTDELETELAST(list) do { \
   if (debug) { \
      printf("  Obsah alloc. zoznamu pred odstranenim posledneho\n"); \
      T_AllocListPrint(list); \
   } \
} while (0)

#define DEBUG_TALLOCLISTDELETELAST_END(list) do { \
   if (debug) { \
      printf("  Obsah alloc. zoznamu po odstraneni posledneho\n"); \
      T_AllocListPrint(list); \
      printf("---- T_ALLOCLIST DELETE_LAST END ----\n"); \
   } \
} while (0)

// T_Instr --------------------------------------------------------------------
#define DEBUG_TINSTRINIT(instr) \
if (debug) printf("---- T_INSTR INIT START ----\n" \
                  "  T_Instr na adrese: %p\n", (void *)instr)

#define DEBUG_TINSTRINIT_END(instr) do { \
   if (debug) { \
      printf("Vypis promenne po inicializaci:\n"); \
      T_InstrPrint(instr); \
      printf("---- T_INSTR INIT END ----\n"); \
   } \
} while (0)

#define DEBUG_TINSTRDELETE(instr) \
if (debug) printf("---- T_INSTR DELETE START ----\n" \
                  "  T_Instr na adrese: %p\n", (void *)instr)

#define DEBUG_TINSTRDELETE_END(instr) do { \
   if (debug) { \
      printf("Vypis promenne pred smazanim:\n"); \
      T_InstrPrint(instr); \
      printf("Vypis promenne po smazani:\n"); \
      T_InstrPrint(instr); \
      printf("---- T_INSTR DELETE END ----\n"); \
   } \
} while (0)

#define DEBUG_TINSTRSET(instr) \
if (debug) printf("---- T_INSTR SET START ----\n" \
                  "  T_Instr na adrese: %p\n", (void *)instr)

#define DEBUG_TINSTRSET_END(instr) do { \
   if (debug) { \
      printf("Vypis promenne po nastaveni:\n"); \
      T_InstrPrint(instr); \
      printf("---- T_INSTR SET END ----\n"); \
   } \
} while (0)

// INPUT ----------------------------------------------------------------------
#define DEBUG_INPUT(var) \
if (debug) printf("---- INPUT START ----\n" \
                  "  Promenna na adrese: %p\n", (void*) var)

#define DEBUG_INPUT_END(var) do { \
   if (debug) { \
      printf("  Vypis promenne po nastaveni:\n"); \
      printf("  adresa: %p\n",(void *)var); \
      T_VarPrint(var); \
      printf("---- INPUT END ----\n"); \
   } \
} while (0)

// PRINT ----------------------------------------------------------------------
#define DEBUG_PRINT(write_var, read_var) \
if (debug) printf("---- PRINT ----\n" \
                  "  Adresy => write_var: %p, read_var: %p\n", (void *)write_var, (void *)read_var)

// NUMERIC --------------------------------------------------------------------
#define DEBUG_NUMERIC(write_var, read_var) do { \
   if (debug) { \
      printf("---- NUMERIC----\n" \
             "  Adresy => write_var: %p, read_var: %p\n", (void *)write_var, (void *)read_var); \
      printf("  WRITE_VAR: \n"); \
      T_VarPrint(write_var); \
      printf("  READ_VAR: \n"); \
      T_VarPrint(read_var); \
   } \
} while (0)

// TYPEOF ---------------------------------------------------------------------
#define DEBUG_TYPEOF(write_var, read_var) \
if (debug) printf("---- TYPEOF START ----\n" \
                  "  Adresy => write_var: %p, read_var: %p\n", (void *)write_var, (void *)read_var)

#define DEBUG_TYPEOF_END(write_var, read_var) do { \
   if (debug) { \
      printf("Vypis promenne po typeof:\n" \
             "  Adresy => write_var: %p, read_var: %p\n" \
             "  WRITE_VAR: \n", (void *)write_var,(void *)read_var); \
      T_VarPrint(write_var); \
      printf("  READ_VAR: \n"); \
      T_VarPrint(read_var); \
      printf("---- TYPEOF END ----\n"); \
   } \
} while (0)

// LEN ------------------------------------------------------------------------
#define DEBUG_LEN(write_var, read_var) \
if (debug) printf("---- LEN START ----\n" \
                  "  Adresy => write_var: %p, read_var: %p\n", (void *)write_var,(void *)read_var)

#define DEBUG_LEN_END(write_var, read_var) do { \
   if (debug) { \
      printf("  Vypis promenne po typeof:\n"); \
      printf("  Adresy => write_var: %p, read_var: %p\n", (void *)write_var,(void *)read_var); \
      printf("  WRITE_VAR: \n"); \
      T_VarPrint(write_var); \
      printf("  READ_VAR: \n"); \
      T_VarPrint(read_var); \
      printf("---- TYPEOF END ----\n"); \
   } \
} while (0)

// HASH TABLE -----------------------------------------------------------------

#define DEBUG_HASHINSERT(table, string, var) \
if (debug) printf("---- HASH INSERT START ----\n" \
                  "  Adresa var: %p, String: \"%s\"\n", (void *)var, string)

#define DEBUG_HASHINSERT_END(table, string, var) do { \
   if (debug) { \
      printf("  Vypis promenne po T_HTableInsert:\n" \
             "  Adresa v tabulce: %p\n" \
             "---------- HASH INSERT END -----------\n", (void *)var); \
   } \
} while (0)

#define DEBUG_HASHEXPORT(table, string, var) do { \
   if (debug) { \
      printf("---- HASH EXPORT START ----\n" \
             "  Adresa var: %p, String: \"%s\"\n" \
             "  Obsah tabulky: ", (void *)var, string); \
      T_HashTablePrint(table); \
   } \
} while (0)

#define DEBUG_HASHEXPORT_END(table, string, var) do { \
   if (debug) { \
      printf("  Vypis promenne po T_HTableExport:\n" \
             "  Adresa var: %p\n" \
             "---- HASH EXPORT END ----\n", (void *)var); \
   } \
} while (0)

#define DEBUG_KEYCREAT(key) \
if (debug) printf("  HashTable Key: %d \n", key)

// Hlavni funkce --------------------------------------------------------------

#define DEBUG_SYNAN_INIT \
if (debug) printf("------------------------- Pocatecni inicializace SYNANU ------------------------\n")

#define DEBUG_SYNAN_INIT_END \
if (debug) printf("-------------------------- SYNAN uspesne inicializovan -------------------------\n")

#define DEBUG_SYNAN_FINISH \
if (debug) printf("---------------------------- Ukonceni funkce SYNANU ----------------------------\n")

#define DEBUG_SYNAN_FINISH_END \
if (debug) printf("----------------------------- SYNAN uspesne ukoncen ----------------------------\n")

#define DEBUG_LEX_INIT \
if (debug) printf("------------------------- Pocatecni inicializace LEXU --------------------------\n")

#define DEBUG_LEX_INIT_END \
if (debug) printf("-------------------------- LEX uspesne inicializovan ---------------------------\n")

#define DEBUG_LEX_FINISH \
if (debug) printf("-------------------------------- Ukonceni funkce LEXU --------------------------\n")

#define DEBUG_LEX_FINISH_END \
if (debug) printf("------------------------------ LEX uspesne ukoncen -----------------------------\n")

#define DEBUG_MAIN_SYNAN \
if (debug) printf("---------------------------------- SYNAN START ---------------------------------\n")

#define DEBUG_MAIN_SYNAN_END \
if (debug) printf("----------------------------------- SYNAN END ----------------------------------\n")

#define DEBUG_GET_TOKEN \
if (debug) printf("----------------------------------- LEX START ----------------------------------\n")

#define DEBUG_GET_TOKEN_END \
if (debug) printf("------------------------------------ LEX END -----------------------------------\n")

#define DEBUG_INTERPRET(list) do { \
   if (debug) { \
      printf("-------------------------------- INTERPRET START ------------------------------\n" \
             "Seznam instrukci na adrese: %p\n" \
             "Vypis zoznamu instrukcii: \n", (void *)list); \
      T_ListPrint(list); \
      printf("--------------------------------------------------------------------------------\n"); \
   } \
} while (0)

#define DEBUG_INTERPRET_END \
if (debug) printf("------------------------------------- INTERPRET END ----------------------------\n")

// SYNAN ----------------------------------------------------------------------

#define DEBUG_VAR_GEN(var) \
if (debug) printf("---- Generate Variable START ----\n" \
                  "Adresa var pred generovanim: %p\n", (void *)var)

#define DEBUG_VAR_GEN_END(var) \
if (debug) printf("Adresa var po generovani: %p\n" \
                  "---- Generate Variable END ----\n", (void *)*var)

#define DEBUG_SYNAN \
if (debug) printf("---- <synan> START ----\n")

#define DEBUG_SYNAN_END \
if (debug) printf("---- <synan> END ----\n")

#define DEBUG_STAT \
if (debug) printf("---- <stat> START ----\n")

#define DEBUG_STAT_KEYWORD \
if (debug) printf("  Nalezeno klicove slovo.\n")

#define DEBUG_STAT_FUNCTION \
if (debug) printf("  Nalezeno volani fce.\n")

#define DEBUG_STAT_VAR \
if (debug) printf("  Nalezena promenna.\n")

#define DEBUG_STAT_END \
if (debug) printf("---- <stat> END ----\n")

#define DEBUG_KW \
if (debug) printf("---- <keyword> START ----\n")

#define DEBUG_KW_IF \
if (debug) printf("  Klicovym slovem je IF.\n")

#define DEBUG_KW_WHILE \
if (debug) printf("  Klicovym slovem je WHILE.\n")

#define DEBUG_KW_FUNCTION \
if (debug) printf("  Klicovym slovem je FUNCTION.\n")

#define DEBUG_KW_END \
if (debug) printf("---- <keyword> END ----\n")

#define DEBUG_EXPR \
if (debug) printf("---- <expr> START ----\n")

#define DEBUG_EXPR_END \
if (debug) printf("---- <expr> END ----\n")

#define DEBUG_ITEM \
if (debug) printf("---- <item> START ----\n")

#define DEBUG_ITEM_STR \
if (debug) printf("  Nalezen retezec.\n")

#define DEBUG_ITEM_NUM \
if (debug) printf("  Nalezeno cislo.\n")

#define DEBUG_ITEM_TRUE \
if (debug) printf("  Nalezen bool - TRUE.\n")

#define DEBUG_ITEM_FALSE \
if (debug) printf("  Nalezen bool - FALSE.\n")

#define DEBUG_ITEM_NIL \
if (debug) printf("  Nalezen NIL.\n")

#define DEBUG_ITEM_ID \
if (debug) printf("  Nalezen identifikator.\n")

#define DEBUG_ITEM_END \
if (debug) printf("---- <item> END ----\n")

#define DEBUG_IT_LIST \
if (debug) printf("---- <it-list> START ----\n")

#define DEBUG_IT_LIST_END \
if (debug) printf("---- <it-list> END ----\n")

#define DEBUG_IF \
if (debug) printf("---- <if> START ----\n")

#define DEBUG_IF_ELSE \
if (debug) printf("  Nalezen ELSE\n")

#define DEBUG_IF_END \
if (debug) printf("---- <if> END ----\n")

#define DEBUG_WHILE \
if (debug) printf("---- <while> START ----\n")

#define DEBUG_WHILE_END \
if (debug) printf("---- <while> END ----\n")




// DEBUG INTERPRET ----------------------------------------------
#define DEBUG_T_INSTRINIT \
if (debug) printf("-- interpret T_INSTR INIT --\n")

#define DEBUG_T_INSTRDELETE \
if (debug) printf("-- interpret T_INSTR DELETE --\n")

#define DEBUG_INTERPRET_JUMP(current) do { \
   if (debug) { \
      printf("  Interpret == JUMP\n"); \
      OperationPrint(current->IID); \
      printf("  arg1: \n"); \
      T_VarPrint(current->arg1); \
      printf("  arg2: \n"); \
      T_VarPrint(current->arg2); \
      printf("  arg3: \n"); \
      T_VarPrint(current->arg3); \
   } \
} while (0)

#define DEBUG_INTERPRET_COPY(current) do { \
   if (debug) { \
      printf("  Interpret == COPY\n"); \
      OperationPrint(current->IID); \
      printf("  arg1: \n"); \
      T_VarPrint(current->arg1); \
      printf("  arg2: \n"); \
      T_VarPrint(current->arg2); \
      printf("  arg3: \n"); \
      T_VarPrint(current->arg3); \
   } \
} while (0)

#define DEBUG_INTERPRET_BRAFAL(current) do { \
   if (debug) { \
      printf("  Interpret == BRAFAL\n"); \
      OperationPrint(current->IID); \
      printf("  arg1: \n"); \
      T_VarPrint(current->arg1); \
      printf("  arg2: \n"); \
      T_VarPrint(current->arg2); \
      printf("  arg3: \n"); \
      T_VarPrint(current->arg3); \
   } \
} while (0)

#define DEBUG_INTERPRET_PLUS(current) do { \
   if (debug) { \
      printf("  Interpret == PLUS\n"); \
      OperationPrint(current->IID); \
      printf("  arg1: \n"); \
      T_VarPrint(current->arg1); \
      printf("  arg2: \n"); \
      T_VarPrint(current->arg2); \
      printf("  arg3: \n"); \
      T_VarPrint(current->arg3); \
   } \
} while (0)

#define DEBUG_INTERPRET_DIFF(current) do { \
   if (debug) { \
      printf("Interpret == MINUS\n"); \
      OperationPrint(current->IID); \
      printf("  arg1: \n"); \
      T_VarPrint(current->arg1); \
      printf("  arg2: \n"); \
      T_VarPrint(current->arg2); \
      printf("  arg3: \n"); \
      T_VarPrint(current->arg3); \
   } \
} while (0)

#define DEBUG_INTERPRET_MUL(current) do { \
   if (debug) { \
      printf("  Interpret == KRAT\n"); \
      OperationPrint(current->IID); \
      printf("  arg1: \n"); \
      T_VarPrint(current->arg1); \
      printf("  arg2: \n"); \
      T_VarPrint(current->arg2); \
      printf("  arg3: \n"); \
      T_VarPrint(current->arg3); \
   } \
} while (0)

#define DEBUG_INTERPRET_DIV(current) do { \
   if (debug) { \
      printf("Interpret == DELENO\n"); \
      OperationPrint(current->IID); \
      printf("  arg1: \n"); \
      T_VarPrint(current->arg1); \
      printf("  arg2: \n"); \
      T_VarPrint(current->arg2); \
      printf("  arg3: \n"); \
      T_VarPrint(current->arg3); \
   } \
} while (0)

#define DEBUG_INTERPRET_PWR(current) do { \
   if (debug) { \
      printf("Interpret == MOCNINA ** \n"); \
      OperationPrint(current->IID); \
      printf("  arg1: \n"); \
      T_VarPrint(current->arg1); \
      printf("  arg2: \n"); \
      T_VarPrint(current->arg2); \
      printf("  arg3: \n"); \
      T_VarPrint(current->arg3); \
   } \
} while (0)

#define DEBUG_INTERPRET_CUT(current) do { \
   if (debug) { \
      printf("Interpret == CUT ** \n"); \
      OperationPrint(current->IID); \
      printf("  arg1: \n"); \
      T_VarPrint(current->arg1); \
      printf("  arg2: \n"); \
      T_VarPrint(current->arg2); \
      printf("  arg3: \n"); \
      T_VarPrint(current->arg3); \
   } \
} while (0)

#endif
