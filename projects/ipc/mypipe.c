/* Test program for unnamed pipes */

#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <limits.h>

int main(int argc, char **argv)
{
   int fd[2];
   pid_t pid;
   char puffer[PIPE_BUF];
   FILE *reading, *writing, *newfile;

   if (argc != 2)
   {
      fprintf(stderr, "Usage: %s Datei\n", argv[0]);
      exit(EXIT_FAILURE);
   }
   if ((newfile = fopen(argv[1], "a+")) < 0)
   {
      perror("fopen");
      exit(EXIT_FAILURE);
   }
   /* create a pipe */
   if (pipe(fd) < 0)
   {
      perror("pipe");
      exit(EXIT_FAILURE);
   }
   /* fork a new process */
   if ((pid = fork()) < 0)
   {
      perror("fork");
      exit(EXIT_FAILURE);
   }
   else if (pid > 0)
   {                /* parent process */
      close(fd[0]); /* close reading end */
      if ((writing = fdopen(fd[1], "w")) == NULL)
      {
         perror("fdopen");
         exit(EXIT_FAILURE);
      }
      /* read PIPE_BUF bytes from standard input */
      while (fgets(puffer, PIPE_BUF, stdin) != NULL)
      {
         /* write into pipe */
         fputs(puffer, writing);
         fflush(writing);
      }
      /* close pipe */
      fclose(writing);

      /* wait for child process to finish */
      if ((waitpid(pid, NULL, 0)) < 0)
      {
         perror("waitpid");
         exit(EXIT_FAILURE);
      }
   }
   else
   {                /* child process */
      close(fd[1]); /* close writing end */
      if ((reading = fdopen(fd[0], "r")) == NULL)
      {
         perror("fdopen");
         exit(EXIT_FAILURE);
      }
      /* read PIPE_BUF bytes from pipe */
      while (fgets(puffer, PIPE_BUF, reading) != NULL)
      {
         fputs(puffer, newfile);
      }
      fclose(newfile);
      fclose(reading);
   }
   exit(EXIT_SUCCESS);
}
