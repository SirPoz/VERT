/* Test program for named pipes (FIFO) */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
   FILE *fp;

   if (mkfifo("foo", 0660) == -1)
   {
      fprintf(stderr, "myfifo: Error creating fifo foo\n");
      return EXIT_FAILURE;
   }

   if ((fp = fopen("foo", "w")) != NULL)
   {
      fprintf(fp, "This is a FIFO test message\n");
   }
   fclose(fp);

   return EXIT_SUCCESS;
}
