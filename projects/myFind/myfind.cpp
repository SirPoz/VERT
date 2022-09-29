#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string>
#include <vector>
#include <unistd.h>
#include <iostream>

using namespace std;
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
    string searchPath;
    vector<string>filenames;
 

    while ((c = getopt(argc, argv, "Ri")) != EOF)
    {
        printf("Test: %c\n\n", c);
        switch(c)
        {
            case 'R':
                recursiveSearch = true;
                //printf("-Recursive Search is activated\n\n");  
            break;
            case 'i':
                insensitiveSearch = true;
                //printf("-Insensitive Search is activated\n\n");
            break;
        }

    }
   
    if ( argc - optind < 2 ) {
        fprintf(stderr, "Fehler, es fehlen Argumente für diese Funktion\n");
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    } 
    if (optind < argc)
    {
        searchPath = argv[optind++];
        while (optind < argc)
        {  
            
            filenames.push_back(argv[optind++]);
        }
    }

    cout << "Searchpath" << searchPath << endl;
    for(string file : filenames)
    {
        cout << "File to search:" << file << endl;
    }
    
    
    
    return (0);
}
