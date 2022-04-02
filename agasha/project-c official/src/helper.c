#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/helper.h"

char* helper_clone_string(const char* string) {

    char* blob_clone;
    size_t size_clone;

    size_clone = strlen(string) + 1;
    blob_clone = malloc(size_clone);
    strcpy(blob_clone, string);

    return blob_clone;

}

char* helper_find_blob(
    const char* blob_haystack,
    size_t size_haystack,
    const char* blob_needle,    
    size_t size_needle
) {

    bool found_needle;
    char data_haystack, data_needle;
    size_t index_haystack, index_needle;
    size_t limit;

    /* cannot find the needle if it's bigger than the haystack */
    if (size_needle > size_haystack) {

        return NULL;

    }

    limit = size_haystack - size_needle;

    for (index_haystack = 0; index_haystack <= limit; index_haystack++) {

        /* starts with the assumption that our search will be successful */
        found_needle = true;

        /* checks if the needle is in the current index_haystack */
        for (index_needle = 0; index_needle < size_needle; index_needle++) {

            data_haystack = blob_haystack[index_haystack + index_needle];
            data_needle = blob_needle[index_needle];

            /* the needle is not at this position, search failed */
            if (data_haystack != data_needle) {

                found_needle = false;
                break;

            }

        }

        /* returns the point to the position if the needle was found */
        if (found_needle) {

            return (char*) &blob_haystack[index_haystack];

        }

    }

    /* did not found the needle after all the search */
    return NULL;

}

char* helper_find_suffix(const char* string, const char delimiter) {

    int index_iterator;
    int index_suffix;

    index_suffix = 0;
    index_iterator = 0;

    /* loop while the null terminating character is not found. */
    while (string[index_iterator] != '\0') {

        /* updates the index for the suffix if the delimiter is found */
        if (string[index_iterator] == delimiter) {

            index_suffix = index_iterator + 1;

        }

        index_iterator++;

    }

    return (char*) &string[index_suffix];

}