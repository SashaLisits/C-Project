#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/compiler.h"
#include "../include/helper.h"
#include "../include/instruction.h"
#include "../include/shared.h"

#define BLOCK_SIZE                      (16)

#define LINE_TYPE_INVALID               (-1)
#define LINE_TYPE_NONE                  ( 0)
#define LINE_TYPE_DIRECTIVE             ( 1)
#define LINE_TYPE_INSTRUCTION           ( 2)
#define LINE_TYPE_MACRO_BEGIN           ( 3)
#define LINE_TYPE_MACRO_END             ( 4)
#define LINE_TYPE_MACRO_INVOCATION      ( 5)

#define PARAMETER_IMMEDIATE             ( 1)
#define PARAMETER_REGISTER              ( 2)
#define PARAMETER_INDEXED               ( 3)
#define PARAMETER_MEMORY                ( 4)

#define DIRECTIVE_NONE                  ( 0)
#define DIRECTIVE_DATA                  ( 1)
#define DIRECTIVE_STRING                ( 2)
#define DIRECTIVE_ENTRY                 ( 3)
#define DIRECTIVE_EXTERN                ( 4)

#define ATTRIBUTE_ABSOLUTE              ( 4)
#define ATTRIBUTE_RELOCATABLE           ( 2)
#define ATTRIBUTE_EXTERNAL              ( 1)

#define HEADER_OB "%d %d\n"
#define LINE_OB "%04d A%x-B%x-C%x-D%x-E%x\n"

#define LINE_ENT "%s,%d,%d\n"

#define LINE1_EXT "%s BASE %d\n"
#define LINE2_EXT "%s OFFSET %d\n"

typedef struct parameter { /* instruction parameter */
    bool valid;
    int type;
    int reg;
    char* label;
    int value;
} parameter_t;

typedef struct command { /* instruction with parameters */
    bool valid;
    instruction_t* instruction;
    parameter_t parameter_a;
    parameter_t parameter_b;
} command_t;

static const char* instruction_words[] = {
      "mov",      "cmp",     "add",      "sub",
      "lea",      "clr",     "not",      "inc",
      "dec",      "jmp",     "bne",      "jsr",
      "red",      "prn",     "rts",     "stop",
       NULL
};

static bool compiler_instruction_word(const char* word) {

    char** iterator;

    iterator = (char**) instruction_words;

    /* loop for checking if the label is a reserved word */
    while (*iterator != NULL) {

        /* cannot save a label that is a reserved word */
        if (strcmp(*iterator, word) == 0) {

            return true;

        }

        iterator++;

    }

    return false;


}

static const char* reserved_words[] = {    
      "mov",      "cmp",     "add",      "sub",
      "lea",      "clr",     "not",      "inc",
      "dec",      "jmp",     "bne",      "jsr",
      "red",      "prn",     "rts",     "stop",
    "macro",     "endm",    "data",   "string",
    "entry",   "extern",      NULL
};

static bool compiler_reserved_word(const char* word) {

    char** iterator;

    iterator = (char**) reserved_words;

    /* loop for checking if the label is a reserved word */
    while (*iterator != NULL) {

        /* cannot save a label that is a reserved word */
        if (strcmp(*iterator, word) == 0) {

            return true;

        }

        iterator++;

    }

    return false;

}

static void compiler_save_macro(compiler_t* compiler, macro_t* macro) {

    /* resize full array */
    if (compiler->count_macros == compiler->length_macros) {

        compiler->length_macros = compiler->length_macros + BLOCK_SIZE;
        compiler->array_macros = realloc(
            compiler->array_macros,
            compiler->length_macros * sizeof(*compiler->array_macros)
        );

    }

    /* saves new macro in array */
    compiler->array_macros[compiler->count_macros] = macro;
    compiler->count_macros++;

}

static macro_t* compiler_find_macro(compiler_t* compiler, const char* name) {

    macro_t* macro;
    size_t index;

    for (index = 0; index < compiler->count_macros; index++) {

        macro = compiler->array_macros[index];

        if (strcmp(macro->name, name) == 0) {

            return macro;

        }

    }

    return NULL;

}

static bool compiler_save_label(compiler_t* compiler, const char* name) {

    size_t index;
    label_t* label;

    /* cannot save a label that is a reserved word */
    if (compiler_reserved_word(name)) {

        return false;

    }

    index = 0;

    /* loop for checking if there's already a label with the name */
    while (index < compiler->count_labels) {

        label = compiler->array_labels[index];

        /* cannot save a label that has already been saved */
        if (strcmp(label->name, name) == 0) {

            return false;

        }

        index++;

    }

    /* resize full array */
    if (compiler->count_labels == compiler->length_labels) {

        compiler->length_labels = compiler->length_labels + BLOCK_SIZE;
        compiler->array_labels = realloc(
            compiler->array_labels,
            compiler->length_labels * sizeof(*compiler->array_labels)
        );

    }

    label = label_create(name, compiler->program_counter);

    /* saves new label in array */
    compiler->array_labels[compiler->count_labels] = label;
    compiler->count_labels++;

    return true;

}

static label_t* compiler_find_label(compiler_t* compiler, const char* name) {

    label_t* label;
    size_t index;

    for (index = 0; index < compiler->count_labels; index++) {

        label = compiler->array_labels[index];

        if (strcmp(label->name, name) == 0) {

            return label;

        }

    }

    return NULL;    

}

static bool compiler_valid_directive(
    char** array_tokens,
    size_t count_tokens,
    size_t* length_data
) {

    bool condition_a, condition_b;
    int type;
    int index;
    int result;
    int value;

    *length_data = 0;

    /* directives have the minimum of: "." <directive> */
    if (count_tokens < 2) {

        return false;

    }

    /* all directives must start with a dot "." */
    if (strcmp(array_tokens[0], ".") != 0) {

        return 0;

    }

    type = DIRECTIVE_NONE;

    if (strcmp(array_tokens[1], "data") == 0) {

        type = DIRECTIVE_DATA;

    }

    if (strcmp(array_tokens[1], "string") == 0) {

        type = DIRECTIVE_STRING;

    }

    if (strcmp(array_tokens[1], "entry") == 0) {

        type = DIRECTIVE_ENTRY;

    }

    if (strcmp(array_tokens[1], "extern") == 0) {

        type = DIRECTIVE_EXTERN;

    }

    switch (type) {

        case DIRECTIVE_DATA: {

            /* "data" directive must have an odd number of tokens */
            if (count_tokens % 2 == 0) {

                return false;

            }

            for (index = 2; index < count_tokens; index++) {

                if (index % 2 == 0) { /* even indexes must have values */

                    result = sscanf(array_tokens[index], "%d", &value);

                    if (result != 1) { /* failed to read integer */

                        return false;

                    }

                    *length_data += 1;

                } else { /* odd indexes must have commas */

                    if (strcmp(array_tokens[index], ",") != 0) { /* not a comma */

                        return false;

                    }

                }

            }

            return true;

        }

        case DIRECTIVE_STRING: {

            /* "." "string" "\"" <string> "\"" */
            if (count_tokens != 5) {

                return false;

            }

            condition_a = strcmp(array_tokens[2], "\"") != 0;
            condition_b = strcmp(array_tokens[4], "\"") != 0;

            if (condition_a || condition_b) {

                return false;

            }

            *length_data = strlen(array_tokens[3]) + 1;

            return true;

        }

        case DIRECTIVE_ENTRY:
        case DIRECTIVE_EXTERN: {

            /* "." ("entry" | "extern") <label> */
            if (count_tokens != 3) {

                return false;

            }

            if (compiler_reserved_word(array_tokens[2])) {

                return false;

            }

            return true;

        }

    }

    return false;

}

static bool compiler_parse_register(char* name_register, int* number_register) {

    int result;
    int value;

    /* register names must start with a lowercase 'r' */
    if (*name_register != 'r') {

        return false;

    }

    result = sscanf(&name_register[1], "%d", &value);

    /* failed to parse the number of the register */
    if (result != 1) {

        return false;

    }

    /* the number of the register is out of range */
    if (value < 0 || 15 < value) {

        return false;

    }

    *number_register = value;
    return true;

}

static void compiler_process_parameter(
    char** array_tokens,
    size_t count_tokens,
    parameter_t* parameter,
    size_t* length_code
) {

    bool condition_a, condition_b;
    int result;
    int value;

    parameter->valid = false;

    switch (count_tokens) {

        /* <label>    */
        /* <register> */
        case 1: {

            /* reserved words cannot be labels nor registers */
            if (compiler_reserved_word(array_tokens[0])) {

                return;

            }

            condition_a = compiler_parse_register(array_tokens[0], &value);

            /* the value is a single register */
            if (condition_a) {

                parameter->type = PARAMETER_REGISTER;
                parameter->reg = value;
                parameter->valid = true;
                return;

            }

            *length_code += 2;

            parameter->type = PARAMETER_MEMORY;
            parameter->label = array_tokens[0];
            parameter->valid = true;
            return;

        }

        /* "#" <value> */
        case 2: {

            /* the prefix "#" indicating immediate value not found */
            if (strcmp(array_tokens[0], "#") != 0) {

                return;

            }

            result = sscanf(array_tokens[1], "%d", &value);

            /* failed to read number from string */
            if (result != 1) {

                return;

            }

            *length_code += 1;

            parameter->type = PARAMETER_IMMEDIATE;
            parameter->value = value;
            parameter->valid = true;
            return;

        }

        /* <label> "[" <register> "]" */
        case 4: {

            /* reserved words cannot be labels */
            if (compiler_reserved_word(array_tokens[0])) {

                return;

            }

            condition_a = strcmp(array_tokens[1], "[") != 0;
            condition_b = strcmp(array_tokens[3], "]") != 0;

            /* square brackets not found */    
            if (condition_a || condition_b) {

                return;

            }

            condition_a = compiler_parse_register(array_tokens[2], &value);

            /* failed to parse register */
            if (!condition_a) {

                return;

            }

            *length_code += 2;

            parameter->type = PARAMETER_INDEXED;
            parameter->reg = value;
            parameter->label = array_tokens[0];
            parameter->valid = true;
            return;

        }

    }

}

static void compiler_process_command(
    char** array_tokens,
    size_t count_tokens,
    command_t* command,
    size_t* length_code
) {

    instruction_t* instruction;
    instruction_t** iterator;
    size_t index;

    command->parameter_a.reg = 0;
    command->parameter_b.reg = 0;
    command->parameter_a.valid = false;
    command->parameter_b.valid = false;
    command->valid = false;
    *length_code = 0;

    /* no tokens to process, nothing to do */
    if (count_tokens == 0) {

        return;

    }

    instruction = NULL;
    iterator = (instruction_t**) INSTRUCTIONS;
    while (*iterator != NULL) {

        instruction = *iterator;

        /* instruction for this command found, break loop */
        if (strcmp(array_tokens[0], instruction->name) == 0) {

            break;

        }

        instruction = NULL;
        iterator++;

    }

    if (instruction == NULL) {

        return;

    }

    command->instruction = instruction;

    switch (instruction->type) {

        case 1: { /* parameter, parameter */

            for (index = 1; index < count_tokens; index++) {

                if (strcmp(array_tokens[index], ",") == 0) {

                    break;

                }


            }

            /* failed to find comma */
            if (index == count_tokens) {

                return;

            }

            compiler_process_parameter(
                &array_tokens[1],
                index - 1,
                &command->parameter_a,
                length_code
            );

            compiler_process_parameter(
                &array_tokens[index + 1],
                count_tokens - (index + 1),
                &command->parameter_b,
                length_code
            );

            if (command->parameter_a.valid && command->parameter_b.valid) {

                *length_code += 2;
                command->valid = true;

            }

            return;

        }

        case 2: { /* parameter */

            compiler_process_parameter(
                array_tokens + 1,
                count_tokens - 1,
                &command->parameter_a,
                length_code
            );

            if (!command->parameter_a.valid) {

                return;

            }

            *length_code += 2;

            command->parameter_b.valid = false;
            command->valid = true;

            return;

        }

        case 3: { /* no parameters */

            *length_code = 1;

            command->parameter_a.valid = false;
            command->parameter_b.valid = false;
            command->valid = true;

            return;

        }

    }


}

static int compiler_process_tokens(
    compiler_t* compiler,
    char** array_tokens,
    size_t count_tokens,
    bool processing_macro
) {

    command_t command;
    macro_t* macro;
    size_t length_code;
    size_t length_data;

    while (count_tokens >= 2) {

        /* these tokens have a label before them */
        if (strcmp(array_tokens[1], ":") == 0) {

            compiler_save_label(compiler, array_tokens[0]);

            /* remove label from the array of tokens */
            array_tokens += 2;
            count_tokens -= 2;
            continue;

        }

        /* no label before sentence, break */
        break;

    }

    /* empty line */
    if (count_tokens == 0) {

        return LINE_TYPE_NONE;

    }

    /* tests if this is an instruction */
    if (compiler_instruction_word(array_tokens[0])) {

        compiler_process_command(
            array_tokens,
            count_tokens,
            &command,
            &length_code
        );

        if (!command.valid) {

            return LINE_TYPE_INVALID;

        }

        if (!processing_macro) {

            compiler->size_code += length_code;
            compiler->program_counter += length_code;

        }

        return LINE_TYPE_INSTRUCTION;

    }

    /* tests if this line is a valid directive */
    if (compiler_valid_directive(array_tokens, count_tokens, &length_data)) {

        if (!processing_macro) {

            compiler->size_data += length_data;
            compiler->program_counter += length_data;

        }

        return LINE_TYPE_DIRECTIVE;

    }

    switch (count_tokens) {

        case 1: {

            if (strcmp(array_tokens[0], "endm") == 0) {

                return LINE_TYPE_MACRO_END;

            }

            /* tests for macro invocation */
            if (!compiler_reserved_word(array_tokens[0])) {

                macro = compiler_find_macro(compiler, array_tokens[0]);

                /* macro exists, it can be invoked */
                if (macro != NULL) {

                    return LINE_TYPE_MACRO_INVOCATION;

                }

            }

            break;

        }

        case 2: {

            if (strcmp(array_tokens[0], "macro") == 0) {

                return LINE_TYPE_MACRO_BEGIN;

            }

            break;

        }

    }

    return LINE_TYPE_INVALID;

}

static int compiler_line_type(
    compiler_t* compiler,
    const char* line,
    bool processing_macro
) {

    tokens_t* tokens;

    tokens = compiler->tokens;

    tokens_find(tokens, line);

    return compiler_process_tokens(
        compiler,
        tokens->array_tokens,
        tokens->count_tokens,
        processing_macro
    );

}

compiler_t* compiler_create(
    FILE* file_err,
    FILE* file_as,
    FILE* file_am,
    FILE* file_ob,
    FILE* file_ent,
    FILE* file_ext
) {

    compiler_t* compiler;

    compiler = malloc(sizeof(*compiler));

    compiler->tokens = tokens_create();

    compiler->count_labels = 0;
    compiler->length_labels = BLOCK_SIZE;
    compiler->array_labels = calloc(
        compiler->length_labels,
        sizeof(*compiler->array_labels)
    );

    compiler->file_err = file_err;
    compiler->file_as = file_as;
    compiler->file_am = file_am;
    compiler->file_ob = file_ob;
    compiler->file_ent = file_ent;
    compiler->file_ext = file_ext;

    compiler->errors = 0;
    compiler->program_counter = 100;
    compiler->live_macro = NULL;

    return compiler;

}

void compiler_destroy(compiler_t* compiler) {

    size_t index;

    tokens_destroy(compiler->tokens);

    for (index = 0; index < compiler->count_labels; index++) {

        label_destroy(compiler->array_labels[index]);

    }

    free(compiler->array_labels);
    free(compiler);

}

void compiler_pass_one(compiler_t* compiler) {

    bool condition_a, condition_b, condition_c;
    bool processing_macro;
    char line_buffer[LINE_LIMIT];
    char macro_name[LINE_LIMIT];
    int line_number;
    int line_type;
    macro_t* macro;
    size_t index;

    line_number = 0;

    while (!feof(compiler->file_as)) {

        line_number++;

        processing_macro = compiler->live_macro != NULL;

        /* retrieves one line from the ".as" file */
        fgets(line_buffer, LINE_LIMIT, compiler->file_as);

        line_type = compiler_line_type(compiler, line_buffer, processing_macro);

        condition_a = line_type != LINE_TYPE_MACRO_BEGIN;
        condition_b = line_type != LINE_TYPE_MACRO_END;
        condition_c = compiler->live_macro == NULL;

        /* places the line in the ".am" file if it's outside a macro */
        if (condition_a && condition_b && condition_c) {

            fputs(line_buffer, compiler->file_am);

        }

        /* places the line inside the macro if we are processing one now */
        if (condition_a && condition_b && !condition_c) {

            macro_add_line(compiler->live_macro, line_buffer);
            continue;

        }

        switch (line_type) {

            case LINE_TYPE_NONE: {

                /* empty line, nothing to do */
                break;

            }

            case LINE_TYPE_DIRECTIVE: {

                /* nothing to do */
                break;

            }

            case LINE_TYPE_INSTRUCTION: {

                /* nothing to do */
                break;

            }

            case LINE_TYPE_MACRO_BEGIN: {

                compiler->live_macro = macro_create(compiler->tokens->array_tokens[1]);
                compiler_save_macro(compiler, compiler->live_macro);

                break;

            }

            case LINE_TYPE_MACRO_END: {

                compiler->live_macro = NULL;
                break;

            }

            case LINE_TYPE_MACRO_INVOCATION: {

                sscanf(line_buffer, "%s", macro_name);

                macro = compiler_find_macro(compiler, macro_name);

                /* process the lines that belong to the macro */
                for (index = 0; index < macro->count_lines; index++) {

                    compiler_line_type(
                        compiler,
                        macro->array_lines[index],
                        false
                    );

                }

                break;

            }

            default: { /* line could not be processed, error */

                fprintf(
                    compiler->file_err,
                    "error at line %d\n",
                    line_number
                );

                compiler->errors++;

                break;

            }

        }

    }

    rewind(compiler->file_as);

}

static bool compiler_valid_directive2(
    compiler_t* compiler,
    char** array_tokens,
    size_t count_tokens
) {

    bool condition_a, condition_b;
    char* iterator;
    int type;
    int index;
    int result;
    int value;
    int base, offset;
    label_t* label;

    /* directives have the minimum of: "." <directive> */
    if (count_tokens < 2) {

        return false;

    }

    /* all directives must start with a dot "." */
    if (strcmp(array_tokens[0], ".") != 0) {

        return 0;

    }

    type = DIRECTIVE_NONE;

    if (strcmp(array_tokens[1], "data") == 0) {

        type = DIRECTIVE_DATA;

    }

    if (strcmp(array_tokens[1], "string") == 0) {

        type = DIRECTIVE_STRING;

    }

    if (strcmp(array_tokens[1], "entry") == 0) {

        type = DIRECTIVE_ENTRY;

    }

    if (strcmp(array_tokens[1], "extern") == 0) {

        type = DIRECTIVE_EXTERN;

    }

    switch (type) {

        case DIRECTIVE_DATA: {

            /* "data" directive must have an odd number of tokens */
            if (count_tokens % 2 == 0) {

                return false;

            }

            for (index = 2; index < count_tokens; index++) {

                if (index % 2 == 0) { /* even indexes must have values */

                    result = sscanf(array_tokens[index], "%d", &value);

                    fprintf(
                        compiler->file_ob,
                        LINE_OB,
                        compiler->program_counter,
                        ATTRIBUTE_ABSOLUTE,
                        (value << 12) & 0xF,
                        (value <<  8) & 0xF,
                        (value <<  4) & 0xF,
                        (value <<  0) & 0xF
                    );

                    compiler->program_counter++;

                    if (result != 1) { /* failed to read integer */

                        return false;

                    }

                } else { /* odd indexes must have commas */

                    if (strcmp(array_tokens[index], ",") != 0) { /* not a comma */

                        return false;

                    }

                }

            }

            return true;

        }

        case DIRECTIVE_STRING: {

            /* "." "string" "\"" <string> "\"" */
            if (count_tokens != 5) {

                return false;

            }

            condition_a = strcmp(array_tokens[2], "\"") != 0;
            condition_b = strcmp(array_tokens[4], "\"") != 0;

            if (condition_a || condition_b) {

                return false;

            }

            iterator = array_tokens[3];

            while (true) {

                value = (int) *iterator;

                fprintf(
                    compiler->file_ob,
                    LINE_OB,
                    compiler->program_counter,
                    ATTRIBUTE_ABSOLUTE,
                    (value << 12) & 0xF,
                    (value <<  8) & 0xF,
                    (value <<  4) & 0xF,
                    (value <<  0) & 0xF
                );

                compiler->program_counter++;

                if (*iterator == '\0') {

                    break;

                }

                iterator++;

            }

            return true;

        }

        case DIRECTIVE_ENTRY: {

            /* "." ("entry" | "extern") <label> */
            if (count_tokens != 3) {

                return false;

            }

            if (compiler_reserved_word(array_tokens[2])) {

                return false;

            }

            label = compiler_find_label(compiler, array_tokens[2]);

            if (label != NULL) {

                offset = label->value % 16;
                base = label->value - offset;

                fprintf(
                    compiler->file_ent,
                    LINE_ENT,
                    label->name,
                    base,
                    offset
                );

            }

            return true;

        }

        case DIRECTIVE_EXTERN: {

            /* nothing to do */
            break;

        }

    }

    return false;

}

static int compiler_code_parameter(parameter_t* parameter) {

    switch (parameter->type) {

        case PARAMETER_IMMEDIATE:
            return 0;

        case PARAMETER_MEMORY:
            return 1;

        case PARAMETER_INDEXED:
            return 2;

        case PARAMETER_REGISTER:
            return 3;

    }

    return (-1);

}

static int compiler_calculate_subcode(command_t* command) {

    int subcode;
    int param_code;

    subcode = 0;

    subcode |= command->instruction->funct << 12;

    switch (command->instruction->type) {

        case 1: {

            param_code = compiler_code_parameter(&command->parameter_a);
            subcode |= param_code << 6;
            subcode |= command->parameter_a.reg << 8;

            param_code = compiler_code_parameter(&command->parameter_b);
            subcode |= param_code << 0;
            subcode |= command->parameter_b.reg << 2;

            break;

        }

        case 2: {

            param_code = compiler_code_parameter(&command->parameter_a);
            subcode |= param_code << 6;
            subcode |= command->parameter_a.reg << 8;

            break;

        }

    }

    return subcode;

}

static void compiler_output_parameter(
    compiler_t* compiler,
    parameter_t* parameter
) {

    label_t* label;
    int base, offset;

    switch (parameter->type) {

        case PARAMETER_IMMEDIATE: {

            fprintf(
                compiler->file_ob,
                LINE_OB,
                compiler->program_counter,
                ATTRIBUTE_ABSOLUTE,
                (parameter->value >> 12) & 0xF,
                (parameter->value >>  8) & 0xF,
                (parameter->value >>  4) & 0xF,
                (parameter->value >>  0) & 0xF
            );

            compiler->program_counter++;

            break;

        }

        case PARAMETER_INDEXED:
        case PARAMETER_MEMORY: {

            label = compiler_find_label(compiler, parameter->label);

            if (label != NULL) { /* label found, it's internal */

                offset = label->value % 16;
                base = label->value - offset;

                fprintf(
                    compiler->file_ob,
                    LINE_OB,
                    compiler->program_counter,
                    ATTRIBUTE_RELOCATABLE,
                    (base >> 12) & 0xF,
                    (base >>  8) & 0xF,
                    (base >>  4) & 0xF,
                    (base >>  0) & 0xF
                );

                compiler->program_counter++;

                fprintf(
                    compiler->file_ob,
                    LINE_OB,
                    compiler->program_counter,
                    ATTRIBUTE_RELOCATABLE,
                    (offset >> 12) & 0xF,
                    (offset >>  8) & 0xF,
                    (offset >>  4) & 0xF,
                    (offset >>  0) & 0xF
                );

                compiler->program_counter++;

            } else { /* label not found, it's external */

                fprintf(
                    compiler->file_ob,
                    LINE_OB,
                    compiler->program_counter,
                    ATTRIBUTE_EXTERNAL,
                    0,
                    0,
                    0,
                    0
                );

                fprintf(
                    compiler->file_ext,
                    LINE1_EXT,
                    parameter->label,
                    compiler->program_counter
                );

                compiler->program_counter++;

                fprintf(
                    compiler->file_ob,
                    LINE_OB,
                    compiler->program_counter,
                    ATTRIBUTE_EXTERNAL,
                    0,
                    0,
                    0,
                    0
                );

                fprintf(
                    compiler->file_ext,
                    LINE2_EXT,
                    parameter->label,
                    compiler->program_counter
                );

                compiler->program_counter++;

                fputs("\n", compiler->file_ext);

            }

            break;

        }

    }

}

static void compiler_process_tokens2(
    compiler_t* compiler,
    char** array_tokens,
    size_t count_tokens
) {

    command_t command;
    macro_t* macro;
    int opcode, subcode;
    size_t index;

    while (count_tokens >= 2) {

        /* these tokens have a label before them */
        if (strcmp(array_tokens[1], ":") == 0) {

            /* remove label from the array of tokens */
            array_tokens += 2;
            count_tokens -= 2;
            continue;

        }

        /* no label before sentence, break */
        break;

    }

    /* empty line */
    if (count_tokens == 0) {

        return;

    }

    /* tests if this is an instruction */
    if (compiler_instruction_word(array_tokens[0])) {

        compiler_process_command(
            array_tokens,
            count_tokens,
            &command,
            &index
        );

        /* failed to parse command */
        if (!command.valid) {

            return;

        }

        opcode = command.instruction->opcode;

        fprintf(
            compiler->file_ob,
            LINE_OB,
            compiler->program_counter,
            ATTRIBUTE_ABSOLUTE,
            (opcode >> 12) & 0xF,
            (opcode >>  8) & 0xF,
            (opcode >>  4) & 0xF,
            (opcode >>  0) & 0xF
        );

        compiler->program_counter++;

        switch (command.instruction->type) {

            case 1: { /* instruction parameter_a, parameter_b */

                subcode = compiler_calculate_subcode(&command);

                fprintf(
                    compiler->file_ob,
                    LINE_OB,
                    compiler->program_counter,
                    ATTRIBUTE_ABSOLUTE,
                    (subcode >> 12) & 0xF,
                    (subcode >>  8) & 0xF,
                    (subcode >>  4) & 0xF,
                    (subcode >>  0) & 0xF
                );

                compiler->program_counter++;

                compiler_output_parameter(
                    compiler,
                    &command.parameter_a
                );

                compiler_output_parameter(
                    compiler,
                    &command.parameter_b
                );

                break;

            }

            case 2: { /* instruction parameter */

                subcode = compiler_calculate_subcode(&command);

                fprintf(
                    compiler->file_ob,
                    LINE_OB,
                    compiler->program_counter,
                    ATTRIBUTE_ABSOLUTE,
                    (subcode >> 12) & 0xF,
                    (subcode >>  8) & 0xF,
                    (subcode >>  4) & 0xF,
                    (subcode >>  0) & 0xF
                );

                compiler->program_counter++;

                compiler_output_parameter(
                    compiler,
                    &command.parameter_a
                );

                break;

            }

            case 3: { /* instruction */

                /* nothing to do */
                break;

            }

        }

        return;

    }

    /* tests if this line is a valid directive */
    if (compiler_valid_directive2(compiler, array_tokens, count_tokens)) {

        return;

    }

    switch (count_tokens) {

        case 1: {

            /* ignore end of macro */
            if (strcmp(array_tokens[0], "endm") == 0) {

                return;

            }

            /* tests for macro invocation */
            if (!compiler_reserved_word(array_tokens[0])) {

                macro = compiler_find_macro(compiler, array_tokens[0]);

                /* macro exists, it can be invoked */
                if (macro != NULL) {

                    /* TODO: macro invocation */
                    return;

                }

            }

            break;

        }

        case 2: {

            /* ignore begin of macro */
            if (strcmp(array_tokens[0], "macro") == 0) {

                return;

            }

            break;

        }

    }

}

static void compiler_line_type2(
    compiler_t* compiler,
    const char* line
) {

    tokens_t* tokens;

    tokens = compiler->tokens;

    tokens_find(tokens, line);

    compiler_process_tokens2(
        compiler,
        tokens->array_tokens,
        tokens->count_tokens
    );

}

void compiler_pass_two(compiler_t* compiler) {

    bool condition_a, condition_b;
    bool processing_macro;
    char line_buffer[LINE_LIMIT];
    char macro_name[LINE_LIMIT];
    int line_type;
    macro_t* macro;
    size_t index;

    /* outputs the header of the ".ob" file */
    fprintf(
        compiler->file_ob,
        HEADER_OB,
        (int) compiler->size_code,
        (int) compiler->size_data
    );

    processing_macro = false;
    compiler->program_counter = 100;

    while (!feof(compiler->file_as)) {

        /* retrieves one line from the ".as" file */
        fgets(line_buffer, LINE_LIMIT, compiler->file_as);

        line_type = compiler_line_type(compiler, line_buffer, true);

        condition_a = line_type != LINE_TYPE_MACRO_BEGIN;
        condition_b = line_type != LINE_TYPE_MACRO_END;

        /* ignore lines inside macros */
        if (processing_macro && condition_a && condition_b) {

            continue;

        }

        switch (line_type) {

            case LINE_TYPE_NONE: {

                /* empty line, nothing to do */
                break;

            }

            case LINE_TYPE_DIRECTIVE: {

                compiler_line_type2(compiler, line_buffer);

                break;

            }

            case LINE_TYPE_INSTRUCTION: {

                compiler_line_type2(compiler, line_buffer);

                break;

            }

            case LINE_TYPE_MACRO_BEGIN: {

                processing_macro = true;

                break;

            }

            case LINE_TYPE_MACRO_END: {

                processing_macro = false;

                break;

            }

            case LINE_TYPE_MACRO_INVOCATION: {

                sscanf(line_buffer, "%s", macro_name);

                macro = compiler_find_macro(compiler, macro_name);

                /* process the lines that belong to the macro */
                for (index = 0; index < macro->count_lines; index++) {

                    compiler_line_type2(
                        compiler,
                        macro->array_lines[index]
                    );

                }

                break;

            }

        }

    }

}
