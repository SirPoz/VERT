#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <unistd.h>

/* Verbose Flag wird global gesetzt, damit der komplette Code es sehen kann. */
unsigned short opt_verbose = 0;

/* Hilfsfunktion */
void print_usage(char *programm_name)
{
    printf("Usage: %s [-R] [-i] searchpath filename1 filename2 ... filenameN\n\n", programm_name);
    return;
}

/* Entry Point */
int main(int argc, char *argv[])
{
    /*variables for option handling*/
    int c;
   
    bool recursiveSearch = false;
    bool insensitiveSearch = false;
   
    /*variables for searchpath and filename handling*/
    char * searchPath;
    char * filenames[5];
    int files = 0;

    while ((c = getopt(argc, argv, "Ri")) != EOF)
    {
        printf("Test: %c\n\n", c);
        switch(c)
        {
            case 'R':
                recursiveSearch = true;
                printf("-Recursive Search is activated\n\n");
             
            break;
            case 'i':
                insensitiveSearch = true;
                printf("-Insensitive Search is activated\n\n");
               
            break;
        }

    }

    if ( optind >= argc ) {
        fprintf(stderr, "Fehler: Es wurden Optionen, die Argumente erwarten, ohne Argument angegeben.\n");
        exit(EXIT_FAILURE);
    } 
    if (optind < argc)
    {
        
        while (optind < argc)
        {
            
            printf("%s ", argv[optind++]);
        }
        
    }
    
    
    
    return (0);
}
