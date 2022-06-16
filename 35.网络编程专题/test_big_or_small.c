#include <stdio.h>

typedef union
{
    unsigned short int value;
    unsigned char byte[2];
}to;

int main(int argc, char *argv)
{
    to typeorder;
    typeorder.value = 0xabcd;

    if (typeorder.byte[0] == 0xcd && typeorder.byte[1] == 0xab)
    {
        printf("Low endian byte order!\n");
    }
    if (typeorder.byte[0] == 0xab && typeorder.byte[1] == 0xcd)
    {
     printf("High endian byte order!\n");
    }
    return 0;

}