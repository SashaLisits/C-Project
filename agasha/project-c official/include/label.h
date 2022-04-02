#pragma once

#include <stdint.h>

typedef struct label {
    char* name;
    uint32_t value;
} label_t;

label_t* label_create(const char* name, int value);

void label_destroy(label_t* label);