#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

int main(void)
{
    FILE *fp;

    if (mkfifo("foo", 0660) == -1)
    {
        fprintf(stderr, "myfifo: Error creating fifo foo\n");
        return EXIT_FAILURE;
    }

    if ((fp = fopen("foo", "w")) != NULL)
    {
        fprintf(fp, "This is my nice message!");
    }

    fclose(fp);

    return EXIT_SUCCESS;
}
