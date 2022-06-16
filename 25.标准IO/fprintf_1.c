#include <stdlib.h>
#include <stdio.h>

int main()
{
        FILE *fp = NULL;

        fp = fopen("out.tmp", "w");
        if (fp == NULL)
        {
                perror("fopen()");
                exit(1);
        }

        fprintf(fp, "my name is %s\n", "Zhang");

        fprintf(fp, "address is %s\n", "London");

        fclose(fp);

        exit(0);
}
