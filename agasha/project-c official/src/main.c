#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/compiler.h"
#include "../include/helper.h"
#include "../include/shared.h"

#define BLOB_FILE_EXTENSION             ".as\0"
#define SIZE_FILE_EXTENSION             (4)

#define BLOB_FORWARD_SLASH_END          "/\0"
#define SIZE_FORWARD_SLASH_END          (2)

int main(int argc, char* argv[]) {

    char buffer[SIZE_LINE_BUFFER];
    char* input_file;
    char* output_folder;
    char* pointer_extension;
    char* pointer_file_name;
    compiler_t* compiler;
    FILE* file_as;
    FILE* file_am;
    FILE* file_ent;
    FILE* file_ext;
    FILE* file_ob;

    /* error, the path for the input file has not been provided. */
    if (argc != 3) {

        fprintf(
            stderr,
            "Usage: %s <assembler-file-path> <output-folder-path>\n",
            argv[0]
        );

        return (-1);

    }

    input_file = argv[1];
    output_folder = argv[2];

    pointer_extension = helper_find_blob(
        output_folder,
        strlen(output_folder) + 1,
        BLOB_FORWARD_SLASH_END,
        SIZE_FORWARD_SLASH_END
    );

    /* error, the output folder does not have the "/" forward slash */
    if (pointer_extension == NULL) {

        fprintf(
            stderr,
            "The output folder must have the \"/\" forward slash!\n"
        );

        return (-2);

    }

    pointer_extension = helper_find_blob(
        input_file,
        strlen(input_file) + 1,
        BLOB_FILE_EXTENSION,
        SIZE_FILE_EXTENSION
    );

    /* error, the input file does not have the ".as" extension */
    if (pointer_extension == NULL) {

        fprintf(
            stderr,
            "The input file path must have the \".as\" extension!\n"
        );

        return (-3);

    }

    /* retrieves the pointer to the file name, without parent folders */
    pointer_file_name = helper_find_suffix(input_file, '/');

    /* opens the input file for reading */
    file_as = fopen(input_file, "r");

    if  (file_as == NULL) {

        fprintf(
            stderr,
            "Failed to open the input file!\n"
        );

        return (-4);

    }

    /* removes the ".as" extension from the input file */
    *pointer_extension = '\0';

    /* creates the file path for the output ".am" file */
    strcpy(buffer, output_folder);
    strcat(buffer, pointer_file_name);
    strcat(buffer, ".am");

    /* opens the input file for writing */
    file_am = fopen(buffer, "w");

    if  (file_am == NULL) {

        fprintf(
            stderr,
            "Failed to open the output file '%s'!\n",
            buffer
        );

        return (-5);

    }

    /* creates the file path for the output ".ent" file */
    strcpy(buffer, output_folder);
    strcat(buffer, pointer_file_name);
    strcat(buffer, ".ent");

    /* opens the input file for writing */
    file_ent = fopen(buffer, "w");

    if  (file_ent == NULL) {

        fprintf(
            stderr,
            "Failed to open the output file '%s'!\n",
            buffer
        );

        return (-6);

    }

    /* creates the file path for the output ".ext" file */
    strcpy(buffer, output_folder);
    strcat(buffer, pointer_file_name);
    strcat(buffer, ".ext");

    /* opens the input file for writing */
    file_ext = fopen(buffer, "w");

    if  (file_ext == NULL) {

        fprintf(
            stderr,
            "Failed to open the output file '%s'!\n",
            buffer
        );

        return (-7);

    }

    /* creates the file path for the output ".ob" file */
    strcpy(buffer, output_folder);
    strcat(buffer, pointer_file_name);
    strcat(buffer, ".ob");

    /* opens the input file for writing */
    file_ob = fopen(buffer, "w");

    if  (file_ob == NULL) {

        fprintf(
            stderr,
            "Failed to open the output file '%s'!\n",
            buffer
        );

        return (-8);

    }

    compiler = compiler_create(
        stderr,
        file_as,
        file_am,
        file_ob,
        file_ent,
        file_ext
    );

    compiler_pass_one(compiler);

    if (compiler->errors != 0) {

        fprintf(
            stderr,
            "%d errors were found, stopping compilation process.\n",
            compiler->errors
        );

        return (-3);

    }

    compiler_pass_two(compiler);

    compiler_destroy(compiler);

    fclose(file_ob);
    fclose(file_ext);
    fclose(file_ent);
    fclose(file_am);
    fclose(file_as);

    return 0;

}