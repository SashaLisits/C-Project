

#include "assembler.h"
#include "tables.h"

struct action {
    char *name;
    int funct;
    int opcode;
};

struct action actions = {{"mov", 0, 0},
                         {"cmp", 0, 1},
                         {"add", 10, 2},
                         {"sub", 11, 2},
                         {"lea", 0, 4},
                         {"clr", 10, 5},
                         {"not", 11, 5},
                         {"inc", 12, 5},
                         {"dec", 13, 5},
                         {"jmp", 10, 9},
                         {"bne", 11, 9},
                         {"jsr", 12, 9},
                         {"red", 0, 12},
                         {"prn", 0, 13},
                         {"rts", 0, 14},
                         {"stop", 0, 15}};
enum {
    mov = 0, cmp, add, sub, lea, clr, not, inc, dec, jmp, bne, jsr, red, prn, rts, stop
};

char *otherNames[] = {"macro", ".data", ".string", ".entry", ".extern"};

enum {
    MACRO = 0, DATA, STRING, ENRTY, EXTERN
};


void addToTable(struct Node **head, char *symbol, struct data dataToAdd) {
    /* 1. allocate node */
    struct Node *new_node = (struct Node *) malloc(sizeof(struct Node));
    /* 2. put in the data */
    new_node->data = dataToAdd;
    
    /* 3. Make next of new node as head */
    new_node->next = (*head);
    
    /* 4. move the head to point to the new node */
    (*head) = new_node;
}

//void printList(struct Node *n) {
//    while (n != NULL) {
//        printf(" %d ", n->data);
//        n = n->next;
//    }
//}