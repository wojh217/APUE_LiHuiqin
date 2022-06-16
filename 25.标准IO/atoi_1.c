#include <stdlib.h>
#include <stdio.h>

int main()
{
    char ch[] = "12345";
    char ch2[] = "456abc123";

    printf("%d\n", atoi(ch));
    printf("%d\n", atoi(ch2));

    exit(0);
}