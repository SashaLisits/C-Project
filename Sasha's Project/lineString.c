#include <stdio.h>
#include <string.h>

//Function macro using local labels
#define IS_STR_EMPTY(str)




char *getLabel(char *line){
    // TODO
}

char *getAction(char *line){

}




								 \
do {														 \
		__label__ empty, not_empty, exit;					 \
		if (strlen(str))									 \
				goto not_empty;								 \
		else												 \
				goto empty;									 \
																\
		not_empty:											 \
				printf("string = %s\n", str);				 \
				goto exit;									 \
		empty:												 \
				printf("string is empty\n");				 \
		exit: ;												 \
} while(0);													 \

int main()
{
    char string[20] = {'\0'};

    //Pass empty string to Macro function
    IS_STR_EMPTY(string);


    //Pass non-empty string to Macro function
    strcpy(string, "geeksForgeeks");
    IS_STR_EMPTY(string);

    return 0;
}

