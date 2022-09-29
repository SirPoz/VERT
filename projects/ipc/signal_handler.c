/* Test program for signal() */

#include <stdio.h>
#include <signal.h>

void myhandler(int sig)
{
   printf("CTRL+C (Sig Nr:%d) ignored...\n", sig);
   fflush(stdout);
}

int main(int argc, char *argv[])
{
   (void)signal(SIGINT, myhandler);

   while (1)
      ;
   return 0;
}
