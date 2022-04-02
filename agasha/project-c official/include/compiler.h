#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "label.h"
#include "macro.h"
#include "tokens.h"

typedef struct compiler {
    tokens_t* tokens;
    FILE* file_err;
    FILE* file_as;
    FILE* file_am;
    FILE* file_ob;
    FILE* file_ent;
    FILE* file_ext;
    int size_code;
    int size_data;
    int errors;

    label_t** array_labels;
    size_t count_labels;
    size_t length_labels;

    macro_t** array_macros;
    size_t count_macros;
    size_t length_macros;

    macro_t* live_macro;

    int program_counter;
} compiler_t;

compiler_t* compiler_create(
    FILE* file_err,
    FILE* file_as,
    FILE* file_am,
    FILE* file_ob,
    FILE* file_ent,
    FILE* file_ext
);

void compiler_destroy(compiler_t* compiler);

void compiler_pass_one(compiler_t* compiler);

void compiler_pass_two(compiler_t* compiler);

