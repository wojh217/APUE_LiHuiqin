# include <stdio.h>
# include <string.h>


int main() {

        char *ch = "/root/book.c";

        FILE *fp;

        char name[10];
        //memset(name, 0, sizeof(name));
        //strcpy(name, "zhang");

        if ((fp=fopen(ch, "rb"))==0) {
                printf("open file error\n");
        }
        //fprintf(fp, "aaa");
        //fwrite(name, 1, 5, fp);

        printf("cur location: %d\n", ftell(fp));
        fseek(fp, 1, 0);
        memset(name, 0, sizeof(name));
        fread(name, 1, 5, fp);

        printf("name: %s\n", name);
        fclose(fp);
        return 0;
}