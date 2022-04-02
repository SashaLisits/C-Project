#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "../include/tokens.h"

tokens_t* tokens_create() {

    tokens_t* tokens;

    tokens = malloc(sizeof(*tokens));

    return tokens;

}

void tokens_destroy(tokens_t* tokens) {

    free(tokens);

}

static void tokens_flush(tokens_t* tokens) {

    if (tokens->token_length == 0) {

        return;

    }

    strncpy(
        tokens->iterator_output,
        tokens->iterator_input,
        tokens->token_length
    );

    tokens->iterator_output[tokens->token_length] = '\0';

    tokens->array_tokens[tokens->count_tokens] = tokens->iterator_output;
    tokens->count_tokens++;

    /* moves output pointer to next position in buffer */
    tokens->iterator_output += tokens->token_length + 1;

}

static const char* marks = ",;:.[]#";

void tokens_find(tokens_t* tokens, const char* line) {

    bool mark_found;
    char quote_mark;
    char* line_iterator;
    char* iterator_marks;

    tokens->count_tokens = 0;
    tokens->iterator_input = (char*) line;
    tokens->iterator_output = tokens->buffer;

    quote_mark = '\0';

    line_iterator = (char*) line;
    while (*line_iterator != '\0') {

        /* handles the special case of finding double quotes */
        if (*line_iterator == '"') {

            if (quote_mark == '\0') { /* outside quotes */

                quote_mark = '"';

            } else { /* inside quotes */

                quote_mark = '\0';

            }

            /* flushes the token before the double quotes */
            tokens_flush(tokens);

            /* flushes the double quotes */
            tokens->iterator_input = line_iterator;
            tokens->token_length = 1;
            tokens_flush(tokens);

            line_iterator++;

            /* resets the token input */
            tokens->iterator_input = line_iterator;
            tokens->token_length = 0;

            continue;

        }

        /* checks if we are inside double quotes */
        if (quote_mark != '\0') {

            line_iterator++;
            tokens->token_length++;
            continue;

        }

        /* breaks loop, everything after this is a comment */
        if (*line_iterator == ';') {

            break;

        }

        /* flush token and reset when a space is found */
        if (isspace(*line_iterator)) {

            tokens_flush(tokens);

            line_iterator++;

            /* resets the token input */
            tokens->iterator_input = line_iterator;
            tokens->token_length = 0;

            continue;

        }

        mark_found = false;
        iterator_marks = (char*) marks;
        while (*iterator_marks != '\0') {

            /* a mark has been found, flush token and mark */
            if (*line_iterator == *iterator_marks) {

                mark_found = true;
                break;

            }

            iterator_marks++;

        }

        if (mark_found) {

            /* flushes the token before the mark */
            tokens_flush(tokens);

            /* flushes the mark */
            tokens->iterator_input = line_iterator;
            tokens->token_length = 1;
            tokens_flush(tokens);

            line_iterator++;

            /* resets the token input */
            tokens->iterator_input = line_iterator;
            tokens->token_length = 0;

            continue;

        }

        line_iterator++;
        tokens->token_length++;

    }

    tokens_flush(tokens);

}