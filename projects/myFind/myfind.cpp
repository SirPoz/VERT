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

using namespace std;


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


/* Verbose Flag wird global gesetzt, damit der komplette Code es sehen kann. */
unsigned short opt_verbose = 0;

/* Hilfsfunktion */
void print_usage(char *programm_name)
{
    printf("Usage: %s [-R] [-i] searchpath filename1 filename2 ... filenameN\n\n", programm_name);
    return;
}

void searchFile(string file, string path)
{
    message_t msg;
    int msgid = -1;
    /*tries accessing the message queue*/
    if ((msgid = msgget(KEY, PERM)) == -1)
    {
      /* error handling */
      fprintf(stderr, "%d: Can't access message queue\n", (int)getpid());
    }

   /* Fill message */
    msg.mType = 1;
    msg.mFilename = file;
    msg.mPath = path;
    msg.mPID = (long)getpid();

    
    
    /* Send message */
    if (msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0) == -1)
    {
      /* error handling */
      fprintf(stderr, "%d: Can't send message\n", (int)getpid());
      
    }
    
    
}


//
//
//
//
//  START OF MAIN FUNCTION
//
//
//
//



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
            default:
                printf("-%c is not a supported option\n",c);
        }

    }
   /*checks if enough arguments have been submitted*/
    if ( argc - optind < 2 ) {
        fprintf(stderr, "There are not enough arguments to start the search\n");
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
    /* iterates through the filenames vector and forkes for every filename*/
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
            /*the childprocess starts searching for the file in the searchpath then it will close itself*/
            searchFile(file, searchPath);
            return 0;
        }
    }



    /* initialise variables to check the amount of messages in the message queue*/
    struct msqid_ds buf;
    int rc;
    uint msgcount;




    /* Parent reads the queue after it has created all necessary forks*/
    while(1)
    {
        /*get the number of messages in queue*/
        rc = msgctl(msgid, IPC_STAT, &buf);
        msgcount = (uint)(buf.msg_qnum);
        
        /*if messages in queue try to read them and print them*/
        if(msgcount > 0)
        {
            if (msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), 0, 0) == -1)
            {
                fprintf(stderr, "%s: Can't receive from message queue\n", argv[0]);
                return EXIT_FAILURE;
            }
            cout << msg.mPID <<": "<< msg.mFilename <<": "<< msg.mPath << endl;
        }
        else
        {
            /*if no messages in queue, check if childprocesses are still running*/
            if((childpid = waitpid(-1, NULL, WNOHANG)))
            {
                if((childpid == -1) && (errno != EINTR))
                {
                    /* no child process are left alive and no messages are in the queue so while(1) is broken*/
                    break;
                }
            }
        }     
    }

    /* deletes the created message queue, only for development purposes*/
    if(msgctl (msgid, IPC_RMID, NULL) == -1)
    {
        fprintf(stderr, "%s: Can't remove message queue\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    return (0);
}


