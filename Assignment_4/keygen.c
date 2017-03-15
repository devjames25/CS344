#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define KEYGENMIN 48
#define KEYGENMAX 126

char* GenKey(int length)
{
    int i;
    char* Key = malloc(sizeof(char) * length);

    //stop before null char
    for(i = 0;i < length;i++){
        Key[i] = (rand() % (KEYGENMAX + 1 - KEYGENMIN) + KEYGENMIN);
    }
    Key[i] = '\0';

    return Key;
}

void validArgc(int argc)
{
    //expected ./Keygen ###
    if(argc == 1){
        fprintf(stderr,"KeyGen: Enter a key length.\n");
        exit(1);
    }
}

int main(int argc, char* argv[])
{
    int KeyLength;
    char* EncryptionKey;
    validArgc(argc);

    //Begin Prog.
    srand(time(NULL));
    KeyLength = atoi(argv[1]);

    EncryptionKey = GenKey(KeyLength);
    printf("%s",EncryptionKey);

    free(EncryptionKey);
    return 0;
}