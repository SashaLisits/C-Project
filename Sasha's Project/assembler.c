#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct {
    unsigned int val: 20;
} word;


int main(int argc, char *argv[])
{
    int i;
    FILE * file;
    char buf[255];

    if(argc<2)
        printf("ERROR : no file name sent");
    
    
    else 
    {
        for(i=1; i<= argc, i++)
        {
            sprintf(buf,"%s.as",argv[i]);
            file=fopen(buf,"r");/* open a new file with end of .am*/
            check(file,"file");
        }
    }
}