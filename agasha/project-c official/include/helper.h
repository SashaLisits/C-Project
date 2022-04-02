#pragma once

#include <stdlib.h>

/* like "strdup", available in later C standards */
char* helper_clone_string(const char* string);

/* like "strstr", but with support for null characters */
char* helper_find_blob(
    const char* blob_haystack,
    size_t size_haystack,
    const char* blob_needle,
    size_t size_needle
);

/* finds the string suffix after the last occurence of the delimiter */
char* helper_find_suffix(const char* string, const char delimiter);
