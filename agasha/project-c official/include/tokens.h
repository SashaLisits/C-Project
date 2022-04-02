#pragma once

#include <stdlib.h>

#include "shared.h"

typedef struct tokens {
    char buffer[SIZE_LINE_BUFFER * 2];
    char* array_tokens[SIZE_LINE_BUFFER];
    size_t count_tokens;

    char* iterator_input;
    char* iterator_output;
    size_t token_length;
} tokens_t;

tokens_t* tokens_create();

void tokens_destroy(tokens_t* tokens);

void tokens_find(tokens_t* tokens, const char* line);
