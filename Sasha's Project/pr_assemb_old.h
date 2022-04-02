/**
 * At this point it is assumed that there are no nested macro settings.
 * The macro name is correct and so is its definition.
 * After each definition will come the word "endm".
 * Macro setting will always be before the macro call.
 * */
#include "global.h"

/* struct for macros */

typedef struct {
    char *name;
    int comand;
    char **ptr;
    int put;

}Table;
/******************************************************************/

#define MACRO "macro"
#define ENDM "endm"
#define COMMENT ";"

/*******************************************************************/
typedef int booleann;


Table *adding_into_table(Table *table, char *name, char **ptr, int numOfptr);
booleann Writing_into_table(char *line, Table *table, FILE *outP);
void preAssembler(char *line, int *error, FILE *input, FILE *outP, Table *table);
