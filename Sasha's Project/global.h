#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdalign.h>

/**********************************************************************************/
#define MAX_LINE 81
#define MAX_LABEl 31
#define IC_INITIAL_VALUE 100
#define ERROR -1
/*addressing options of the functions for the operands*/
#define ADDRESS_TYPE4 {1, 1, 1, 1}
#define ADDRESS_TYPE3 {0, 1, 1, 1}
#define ADDRESS_TYPE2 {0, 1, 1, 0}
#define ADDRESS_TYPE0 {0, 0, 0, 0}

#define NUMBER_OF_FUNCTIONS 17

/**********************************************************************************?
/* represents the first word from the line and the rest of it */
typedef struct Line{
    char *word; /* the first word in the line*/
    char *rest; /* the rest of the line*/
    char endLine;
    int lineNumber; /* the line number*/
}Line;

typedef enum symbol_type1{
    non,code,data,ex,en
}symbol_type1;

/* represent the symbols table */
typedef struct symbol{
    /*symbol_type type;*/
    char *name;
    int value;/* the line of the definition*/
    unsigned int baseAddress;
    unsigned int offset;
    int attributes1;
    int attributest2;
    struct symbol * next;
}symbol ;

/* represent a memory word (code)*/
typedef struct memory_word1{
    unsigned int destination_address :2;
    unsigned int destination_register :4;
    unsigned int source_address :2;
    unsigned int source_register :4;
    unsigned int funct :4;
    unsigned int A :1 ;
    unsigned int R :1 ;
    unsigned int E :1 ;
}memory_word1;

/* represent a memory word(opcode)*/
typedef struct memory_word2{
    unsigned int opcode : 16;
    unsigned int A :1 ;
    unsigned int R :1 ;
    unsigned int E :1 ;
}memory_word2;

/* represent a machine code */
typedef struct machine_code{
    union memory_word
    {
        memory_word1 *code;
        memory_word2 *data;
    }memory_word;

    int missing_label;/* if the label is unknown in the first pass, 1 if missing label if its two unknown lables in one line it will be 2*/
    char *missing_symbol_name1;
    char *missing_symbol_name2;
    int lineSymbol;
}machine_code;

/*represent a function */
typedef struct {
    char *name;
    int func;
    int opcode;
    int source_address[4];
    int destination_address[4];
    int operands;
}function;

/*********************************************************************************/
enum regN{
    r0,r1,r2,r3,r4,r5,r6,r7
    ,r8,r9,r10,r11,r12,r13,r14,r15,r16 }register_number;

char ***reg = {"r0","r1","r2","r3","r4","r5","r6","r7","r8","r9",
                "r10","r11","r12","r13","r14","r15","r16"}; /* register names*/


/* an array of all the functions , the last index represent an options of not a function*/
static function functions[17] = {{"mov", 0, 0, ADDRESS_TYPE4, ADDRESS_TYPE3,2},{"cmp", 0, 1, ADDRESS_TYPE4, ADDRESS_TYPE4,2},
                        {"add", 10, 2, ADDRESS_TYPE4, ADDRESS_TYPE3,2},{"sub", 11, 2, ADDRESS_TYPE4, ADDRESS_TYPE3,2},
                        {"lea", 0, 4, ADDRESS_TYPE2, ADDRESS_TYPE3,2},{"clr", 10, 5, ADDRESS_TYPE0, ADDRESS_TYPE3,1},
                        {"not", 11, 5, ADDRESS_TYPE0, ADDRESS_TYPE3,1},{"inc", 12, 5, ADDRESS_TYPE0, ADDRESS_TYPE3,1},
                        {"dec", 13, 5, ADDRESS_TYPE0, ADDRESS_TYPE3,1},{"jmp", 10, 9, ADDRESS_TYPE0, ADDRESS_TYPE2,1},
                        {"bne", 11, 9, ADDRESS_TYPE0, ADDRESS_TYPE2,1},{"jsr", 12, 9, ADDRESS_TYPE0, ADDRESS_TYPE2,1},
                        {"red", 0, 12, ADDRESS_TYPE0, ADDRESS_TYPE3,1},{"prn", 0, 13, ADDRESS_TYPE0, ADDRESS_TYPE4,1},
                        {"rts", 0, 14, ADDRESS_TYPE0, ADDRESS_TYPE0,0},{"stop", 0, 15, ADDRESS_TYPE0, ADDRESS_TYPE0,0},
                        {"not a function",0,0,ADDRESS_TYPE0,ADDRESS_TYPE0,0}};

/*****************************************************************************************/

/* get a line and insert to LINE the first word and the rest using strtok */
Line *get_word(char [], char *,int );  

/*check if the word is a label from the table*/
symbol * is_label(char *,symbol *);

function is_function(char *);

/*go to the next char that is not a white tab*/
char *move_spaces(char *);

/* check if the first word is a valid label name and there is not unnecessary words */
int check_rest(Line *,char *);

/**
 * calculates the address closest to the address of the operand,
 *  which is smaller than it, and divides by 16
 * */
int Base(int);

/* calculates the distance from the base address to the operand address */
int Offset(int ,int);