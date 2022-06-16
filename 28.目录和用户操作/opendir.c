#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>

#define PAT "/svr/audio"

int main()
{

	DIR *dirp;
	struct dirent *readres;

	dirp = opendir(PAT);
	if(dirp == NULL)
	{
		perror("opendir()");
		exit(1);
	}

	// readres = readdir(dirp);
	// while(readres != NULL)
	// {
		
	// 	printf("%s\n", readres->d_name);
	// 	readres = readdir(dirp);
		
	// }

	while(  (readres = readdir(dirp)) != NULL )
	{
		printf("%s\n", readres->d_name);
	}
	

	closedir(dirp);

	exit(0);

}