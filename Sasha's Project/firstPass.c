#include "global.h"


int *error;
int first_pass(FILE *fp)
{
    machine_code memory[8092];/* the memory image*/
    machine_code data_image[8092];
    int *error;
    int *IC = IC_INITIAL_VALUE; /* instruction counter*/
    int *DC = 0; /* data counter */
    int lineNumber = 1; /* line counter */
    int in_label_defnition = 0;
    int tempIC;
    char fline[MAX_LINE];
    char *lableName;
    char copyLine[MAX_LINE];
    Line *line;
    Line *tempLine;
    symbol *table = new_symbol(NULL,NULL,0);
    function Ftemp;

    error = 0;



void fix_data_offset_base(symbol *tabel , int *IC)
{
    symbol *temp = tabel;
    int i = 0;

    while(temp && temp->name)
    {
        if(temp->attributes1=data)
        {
            i = temp->value + temp->offset + temp->baseAddress;

            initialize_symbol(data, non, i, temp);
        }

        temp = temp ->next
                i=0;
        //initislize the symbol base on the parameters that have sent to the function
        }
    }
    int initialize_symbol(int type1, int type2, int IC, symbol *table)
    {
        table->value = IC;
        table->attributes1 = type1;
        table->attributes2 = type2;
        initialize_base_offset(IC, table);
        return 1;
    }
}