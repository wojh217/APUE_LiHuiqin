# include <stdio.h>
# include <string.h>
# include <dirent.h>

int main() {
        char name[100];

        memset(name, 0, sizeof(name));
        getcwd(name, 100);

        printf("cur dir is: %s\n", name);

        chdir("/root");
        memset(name, 0, sizeof(name));
        getcwd(name, 100);
        printf("now cur is: %s\n", name);


        //mkdir("/zhang/my", 0755);
        rmdir("/zhang/my");

        DIR *dir;

        dir = opendir("/zhang/");

        if (dir==0) return -1;

        struct dirent *stdinfo;

        while(1)
        {
                if((stdinfo=readdir(dir)) ==0) break;
                printf("name=%s, type=%d\n", stdinfo->d_name, stdinfo->d_type);
        }
        closedir(dir);
        return 0;

}