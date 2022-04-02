#include <stdlib.h>

#include "../include/helper.h"
#include "../include/label.h"

label_t* label_create(const char* name, int value) {

    label_t* label;

    label = malloc(sizeof(*label));
    label->name = helper_clone_string(name);
    label->value = value;

    return label;

}

void label_destroy(label_t* label) {

    free(label->name);
    free(label);

}