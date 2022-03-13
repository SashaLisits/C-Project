#include "stdlib"
#include "stdio"
#include "string.h"

#define MAX_LIMIT 20

int main(){
    char str[MAX_LIMIT];
    fgets(str, MAX_LIMIT, stdin);
    printf("%s", str);

    return 0;
}

// A C/C++ program for splitting a string
// using strtok()



int split()
{
    char str[] = "";

    // Returns first token
    char *token = strtok(str, "-");

    // Keep printing tokens while one of the
    // delimiters present in str[].
    while (token != NULL)
    {
        printf("%s\n", token);
        token = strtok(NULL, "-");
    }

    return 0;
}


// C program to illustrate
// strcpy() function ic C/C++


int copy ()
{
    char str1[]="";
    char str2[] = "GeeksforGeeks";
    char str3[40];
    char str4[40];
    char str5[] = "GfG";

    strcpy(str2, str1);
    strcpy(str3, "Copy successful");
    strcpy(str4, str5);
    printf ("str1: %s\nstr2: %s\nstr3: %s\nstr4:%s\n", str1, str2, str3, str4);
    return 0;
}

