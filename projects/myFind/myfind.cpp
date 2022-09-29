#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <vector>
#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstring>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string>

#define KEY 521125900 /* unique key e.g., matriculation number */
#define PERM 0660 /* starts with 0 to declare it as octal */

#define MAX_DATA 255

typedef struct
{
	long mType;
    long mPID;
	string mFilename;
    string mPath;
} message_t;

using namespace std;
/* Verbose Flag wird global gesetzt, damit der komplette Code es sehen kann. */
unsigned short opt_verbose = 0;

/* Hilfsfunktion */
void print_usage(char *programm_name)
{
    printf("Usage: %s [-R] [-i] searchpath filename1 filename2 ... filenameN\n\n", programm_name);
    return;
}

void searchFile(string file)
{
    message_t msg;
    int msgid = -1;
    if ((msgid = msgget(KEY, PERM)) == -1)
    {
      /* error handling */
      fprintf(stderr, "%d: Can't access message queue\n", (int)getpid());
      
    }

   /* Fill message */
    msg.mType = 1;
    msg.mFilename = file;
    msg.mPath = "./";
    msg.mPID = (long)getpid();

    cout << msg.mType << ":" << msg.mFilename << ":" << msg.mPath << ":" << msg.mPID << endl;
    
    /* Send message */
    if (msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0) == -1)
    {
      /* error handling */
      fprintf(stderr, "%d: Can't send message\n", (int)getpid());
      
    }
    
}

/* Entry Point */
int main(int argc, char *argv[])
{
    /*variables for option handling*/
    int c;
   
   /*recursive and case insensitive Search are deactivated by standard*/
    bool recursiveSearch = false;
    bool insensitiveSearch = false;
   
    /*variables for searchpath and filename handling*/
    string searchPath;
    vector<string>filenames;
 

    while ((c = getopt(argc, argv, "Ri")) != EOF)
    {
       /*check for options set and change the depending variables if necessary*/
        switch(c)
        {
            case 'R':
                recursiveSearch = true;
            break;
            case 'i':
                insensitiveSearch = true;
            break;
        }

    }
   /*checks if enough arguments have been submitted*/
    if ( argc - optind < 2 ) {
        fprintf(stderr, "Fehler, es fehlen Argumente für diese Funktion\n");
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    } 

    /*first non option argv must be the searchpath, everything after that are filenames*/
    if (optind < argc)
    {
        searchPath = argv[optind++];
        while (optind < argc)
        {  
            
            filenames.push_back(argv[optind++]);
        }
    }










    /*create message queue for later communication*/
    message_t msg;  /* message buffer */
    int msgid = -1; /* message queue ID */
    if ((msgid = msgget(KEY, PERM | IPC_CREAT | IPC_EXCL)) == -1)
    {
      /* error handling */
      fprintf(stderr, "%s: Error creating message queue\n", argv[0]);
      return EXIT_FAILURE;
    }








    /*FORKING of childprocesses*/
    pid_t childpid;
    for(string file : filenames)
    {
        childpid = fork();
        if (childpid == -1)
        { 
            perror("Failed to fork");
            return 0;
        }
        if(childpid == 0)
        {
            searchFile(file);
            return 0;
        }
        else
        {
            cout << "Procees " << (long)getpid() << " is creating more forks"<< endl;
        }
    }








    /* Parent reads the queue after it has created all necessary forks*/
    while ((childpid = waitpid(-1, NULL, WNOHANG)))
    {
      if (msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), 0, 0) == -1)
      {
         /* error handling */
         fprintf(stderr, "%s: Can't receive from message queue\n", argv[0]);
         return EXIT_FAILURE;
      }
      cout << msg.mPID <<": "<< msg.mFilename <<": "<< msg.mPath << endl;
      if((childpid == -1) && (errno != EINTR)){
        break;
      }
    }
    
    return (0);
}


