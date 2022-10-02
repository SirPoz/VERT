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

#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <limits.h>
#include <string.h>
#include <algorithm>



using namespace std;


#define KEY 521125900 /* unique key e.g., matriculation number */
#define PERM 0660 /* starts with 0 to declare it as octal */

#define MAX_DATA 255

typedef struct
{
	long mType;
    long mPID;
	char mFilename[MAX_DATA];
    char mPath[MAX_DATA];
} message_t;


/* Verbose Flag wird global gesetzt, damit der komplette Code es sehen kann. */
unsigned short opt_verbose = 0;

/* Hilfsfunktion */
void print_usage(char *programm_name)
{
    printf("Usage: %s [-R] [-i] searchpath filename1 filename2 ... filenameN\n\n", programm_name);
    return;
}

string tolowers(string tolower)
{

   for_each(tolower.begin(), tolower.end(), [](char &c)
                 { c = ::tolower(c); });
   return tolower;
}

void searchfile(string searchpath, string filename, bool recursive, bool casesensitive)
{

   struct dirent *direntp;
   DIR *dirp;
   string newsearchpath;
   string filepath;
   string currentfile;

   if ((dirp = opendir(searchpath.c_str())) == NULL)
   {
      //perror("Failed to open directory");
      return;
   }

   while ((direntp = readdir(dirp)) != NULL)
   {
      if (direntp->d_type == 8)
      {
         

         currentfile = (casesensitive) ? direntp->d_name : tolowers(direntp->d_name);
         filename = (casesensitive) ? filename : tolowers(filename);
        
         if (currentfile.find(filename) != std::string::npos)
         {
            filepath = searchpath + "/" + direntp->d_name;
            

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
            strcpy(msg.mFilename, filename.c_str());
            strcpy(msg.mPath,filepath.c_str());
            msg.mPID = (long)getpid();

    
    
            /* Send message */
            if (msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0) == -1)
            {
            /* error handling */
            fprintf(stderr, "%d: Can't send message\n", (int)getpid());
            
            }    
         }
        
      }
      if (direntp->d_type == 4)
      {

         if (recursive)
         {
            // strcmp returns false when both strings are equal
            if (strcmp(direntp->d_name, "..") && strcmp(direntp->d_name, "."))
            {
               newsearchpath = searchpath + "/" + direntp->d_name;
               
               searchfile(newsearchpath, filename, recursive, casesensitive);
            }
         }
      }
   }
}


void searchFile(string file, string path)
{
    
    
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
   
   /*recursive and case insensitive Search are deactivated by default*/
    bool recursiveSearch = false;
    bool insensitiveSearch = false;
   
    /*variables for searchpath and filename handling*/
    string searchPath;
    vector<string>filenames;


 
    /* reading the options */
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
   /*checks if enough arguments have been passed*/
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
            searchfile(searchPath, file, recursiveSearch, !insensitiveSearch);
            return 0;
        }
    }



    /* initialise variables to check the amount of messages in the message queue*/
    struct msqid_ds buf;
    uint msgcount;

    


    /* Parent reads the queue after it has created all necessary forks*/
    while(1)
    {
        /*get the number of messages in queue*/
        msgctl(msgid, IPC_STAT, &buf);
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


