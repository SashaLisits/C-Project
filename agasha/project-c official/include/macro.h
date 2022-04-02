#pragma once

#include <stdlib.h>

typedef struct macro {
    char* name;
    char** array_lines;
    size_t count_lines;
    size_t length_lines;
} macro_t;

macro_t* macro_create(const char* name);

void macro_destroy(macro_t* macro);

void macro_add_line(macro_t* macro, const char* line);
