#include "Pre-assembler.h"
#include "global.h"

/* get a line and insert to LINE the first word and the rest using strtok */
Line *get_word(char line[], char *skip,int lineNumber)
{
    char *pword;
    char *prest;
    Line *newLine;

    /*if the line length is to long and we are not in the pre-assembler */
    if(strlen(line)>MAX_LINE && lineNumber)
        printf("ERRO : in line %d to long line :(\n",lineNumber);

    newLine = new_line();

    if(line){
        pword = strtok(line, skip);/* separate the first word from the rest of the line */
        prest = strtok(NULL,"");/* get the rest*/

        /*if there is only one part in the line*/
        if (prest == NULL && strlen(pword) > 0)
        {
            newLine->endLine = 1;
            pword[strlen(pword) - 1] = '\0';
        }

        newLine->word = pword;
        newLine->rest = prest;
    }
    else {
        newLine->rest = NULL;
        newLine->word = NULL;
    }
    newLine->lineNumber = lineNumber;
    return newLine; /* return the expended line*/
}

/**function gets pointer and type of action done before,
 * accordingly prints the appropriate error in case of need*/
int check(void * toCheck, char *type)
{
    if(!toCheck)/* if the pointer is null for some reason*/
    {
        if(strcmp(type,"file")==0)
        {
            printf("ERROR :  can not open file :(\n");
            return 0;
        }

        else if(strcmp(type,"malloc")==0)
            printf("ERROR : malloc failure :(\n");

        else if(strcmp(type,"calloc")==0)
            printf("ERROR : calloc failure :(\n");

        else if(strcmp(type,"realloc")==0)
            printf("ERROR : realloc failure :(\n");

        exit(1);
    }
    return 1;
}

/**
 *  return
 *  0: symbol not legal
 *  1: symbol defenition
 *  2: not symbol
 */
int is_label_definition(Line *line )
{
    char last;
    int len;
    int i = 0;
    len = strlen(line->word);
    last = *(line->word + len - 1);
    if (len >=MAX_LABEl)
        return 0;

    while (i == (len - 1))
    {
        if (!(isalpha(line->word[i]) != 0 || isdigit(line->word[i]) != 0))
            return 0;

        i++;
    }
    /* if its a symbol defenition*/
    if (last == ':')
    {
        line->word[len] = '\0';
        return 1;
    }
    return 2;
}

/* return the label name wuthout ':'*/
char *get_label_name(char *word)
{
    int length;

    length = strlen(word);
    word[length-1] = '\0';/* delet the last char from the string*/

    return word;
}

/* check if the word is a valid string*/
int is_string(char *word)
{
    int len,i;
    len = strlen(word);

    if(word[0] == '\"' && word[len-1] =='\"')
        return 1;

    else return 0;
}

/*go to the next char that is not a white tab*/
char *move_spaces(char *line)
{
    while(isspace(*line))
        line++;

    return line;
}

/* check if the first word is a valid label name and there is not unnecessary words */
int check_rest(Line *line,char *type)
{
    /* if there is a labels name*/
    if(line->word)
    {
        /* if the label name is valid*/
        if((line->word[0]>='A' && line->word[0]<='Z') || (line->word[0]>='a' && line->word[0]<='z'))
        {
            if(line->rest)
            {
                printf("ERROR :unnecessary words after guidance sentece in line %d :(\n",line->lineNumber);
                return 0;
            }
            else
            {
                printf("WARNING : symbol defnition with %s in line %d :(\n",type,line->lineNumber);
                return 1;
            }
        }
        else
        {
            printf("ERROR : invalid label name in line %d :(",line->lineNumber);
            return 0;
        }
    }
    else {
        printf("ERROR : missing label name after .%s in line %d :(\n",type,line->lineNumber);
        return 0;
    }
}

/* return if the word is a function/*/
function is_function(char *word)
{
    int i;
    for(i=0; i<NUMBER_OF_FUNCTIONS-1;i++ )
        /*if word is a name of a function*/
        if(!strcmp(word,functions[i].name))
            return functions[i];

    return functions[NUMBER_OF_FUNCTIONS-1];/**/
}

/*check if the word is a label from the table*/
symbol * is_label(char * word ,symbol * head)
{
    symbol *p;/* point for the head of the list*/
    p = head;

    while( p && p->name)
    {
        if(strcmp(word,head->name)==0)
            return p;

        p = p->next;
    }
    return NULL;
}

/**
 *  Checks if the beginning of the word is the name of a defined register
 * The function does not check if there are unnecessary letters after the register,
 * the function returns the register number if found, if you do not return ERROR.
 * */
int is_register(char *str)
{
    char *ptr = str;
    int num,j = 1;
    int i = 0;

    /* if it is a start of register name */
    if(str[0]=='r')
    {
        i++;
        /*as long as it's a two-digit number*/
        while(isdigit(ptr[i])!=0 && j>=0)
        {
            num += atoi(ptr[i])*pow(10,j);
            j--;
            i++;
        }
        /* if the register number is bigger then two-digit*/
        if(isdigit(ptr[i])!=0)
            return ERROR;

        else
        {
            switch (num)
            {
                case 0:
                    return r0;
                    break;
                case 1:
                    return r1;
                    break;
                case 2:
                    return r2;
                case 3:
                    return r3;
                case 4:
                    return r4;
                case 5:
                    return r5;
                case 6:
                    return r6;
                case 7:
                    return r7;
                case 8:
                    return r8;
                case 9:
                    return r9;
                case 10:
                    return r10;
                case 11:
                    return r11;
                case 12:
                    return 12;
                case 13:
                    return r13;
                case 14:
                    return r14;
                case 15:
                    return r15;
                case 16:
                    return r16;
                default:
                    return ERROR;
                    break;
            }
        }
    }
    else return ERROR;
}

/* check if the two given arrays are equal the return value will be 1, else 0*/
int is_equal(int array[4],int array2[4])
{
    int i = 0;
    while(i<4)
    {
        if(array[i]!= array2[i])
            return 0;
        i++;
    }
    return 1;
}
