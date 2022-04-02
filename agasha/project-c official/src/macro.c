#include <stdlib.h>

#include "../include/helper.h"
#include "../include/macro.h"

#define BLOCK_SIZE (8)

macro_t* macro_create(const char* name) {

    macro_t* macro = malloc(sizeof(*macro));
    macro->name = helper_clone_string(name);

    macro->count_lines = 0;
    macro->length_lines = BLOCK_SIZE;
    macro->array_lines = calloc(
        macro->length_lines,
        sizeof(*macro->array_lines)
    );

    return macro;

}

void macro_destroy(macro_t* macro) {

    size_t index;

    for (index = 0; index < macro->count_lines; index++) {

        free(macro->array_lines[index]);

    }

    free(macro->array_lines);
    free(macro->name);
    free(macro);

}

void macro_add_line(macro_t* macro, const char* line) {

    char* macro_line;

    /* resize full array */
    if (macro->count_lines == macro->length_lines) {

        macro->length_lines = macro->length_lines + BLOCK_SIZE;
        macro->array_lines = realloc(
            macro->array_lines,
            macro->length_lines * sizeof(*macro->array_lines)
        );

    }

    macro_line = helper_clone_string(line);

    /* saves new line in array */
    macro->array_lines[macro->count_lines] = macro_line;
    macro->count_lines++;

}