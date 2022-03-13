/**
 * At this point it is assumed that there are no nested macro settings.
 * The macro name is correct and so is its definition.
 * After each definition will come the word "endm".
 * Macro setting will always be before the macro call.
 * */
#include "global.h"

/* struct for macros */
typedef struct macro {
    char *name; /* the macro's name */
    char *text; /* the text that the macro contains */
    struct macro *next;
}macro;

/******************************************************************/

#define MACRO "macro"
#define ENDM "endm"
#define COMMENT ";"

/*******************************************************************/

/* creat a new macro and add it to the head of the list , return a pointer to the new macro in the list */
macro  *new_macro(macro *,char *);

/*goes through the list and checks if there is a macro of like the word sent to the function */
macro *macro_name ( char *, macro *);

/* make a new line */
Line *new_line();

/* get a line and insert to LINE the first word and the rest using strtok */
Line *get_word(char *, char *,int);

/* print the content of the macro insted of the macro name*/
FILE *print_content(FILE * , macro *);

/* insert the text in the definition into the macro's text */
void insert_text (macro *,char *);

/*free the list from the mamory */
int free_list(macro *);

/**function gets pointer and type of action done before,
 * accordingly prints the appropriate error in case of need*/
int check(void * , char *);
