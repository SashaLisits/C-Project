#pragma once

#include <stdlib.h>

typedef struct instruction {
    const char* name;
    int opcode;
    int funct;
    int type;
} instruction_t;

const instruction_t INSTRUCTION_MOV  = {  "mov",  0,  0, 1 };
const instruction_t INSTRUCTION_CMP  = {  "cmp",  1,  0, 1 };
const instruction_t INSTRUCTION_ADD  = {  "add",  2, 10, 1 };
const instruction_t INSTRUCTION_SUB  = {  "sub",  2, 11, 1 };
const instruction_t INSTRUCTION_LEA  = {  "lea",  4,  0, 1 };
const instruction_t INSTRUCTION_CLR  = {  "clr",  5, 10, 2 };
const instruction_t INSTRUCTION_NOT  = {  "not",  5, 11, 2 };
const instruction_t INSTRUCTION_INC  = {  "inc",  5, 12, 2 };
const instruction_t INSTRUCTION_DEC  = {  "dec",  5, 13, 2 };
const instruction_t INSTRUCTION_JMP  = {  "jmp",  9, 10, 2 };
const instruction_t INSTRUCTION_BNE  = {  "bne",  9, 11, 2 };
const instruction_t INSTRUCTION_JSR  = {  "jsr",  9, 12, 2 };
const instruction_t INSTRUCTION_RED  = {  "red", 12,  0, 2 };
const instruction_t INSTRUCTION_PRN  = {  "prn", 13,  0, 2 };
const instruction_t INSTRUCTION_RTS  = {  "rts", 14,  0, 3 };
const instruction_t INSTRUCTION_STOP = { "stop", 15,  0, 3 };

const instruction_t* INSTRUCTIONS[] = {
    &INSTRUCTION_MOV,
    &INSTRUCTION_CMP,
    &INSTRUCTION_ADD,
    &INSTRUCTION_SUB,
    &INSTRUCTION_LEA,
    &INSTRUCTION_CLR,
    &INSTRUCTION_NOT,
    &INSTRUCTION_INC,
    &INSTRUCTION_DEC,
    &INSTRUCTION_JMP,
    &INSTRUCTION_BNE,
    &INSTRUCTION_JSR,
    &INSTRUCTION_RED,
    &INSTRUCTION_PRN,
    &INSTRUCTION_RTS,
    &INSTRUCTION_STOP,
    NULL
};
