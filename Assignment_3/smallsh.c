#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

struct InputObj
{
    bool Background;
    char InputFile[256];
    char OutputFile[256];
    char Command[1028];
    char *Arguments[128];
};

int ChangeDirectorysFromHome(char* InputBuffer)
{
    char* HomeDirectoryPath = getenv("HOME");
    char NewPath[1028];

    memset(NewPath,'\0',sizeof(NewPath));

    strtok(InputBuffer," ");
    strcpy(InputBuffer,strtok(NULL,""));
    InputBuffer[strlen(InputBuffer) -1] = '\0';

    //printf("    %s\n",InputBuffer);
    if(InputBuffer[0] == '/'){
        sprintf(NewPath,"%s%s",HomeDirectoryPath,InputBuffer);
        //printf("Path: %s\n",NewPath);
    }
    else if(strcmp(InputBuffer,"..") == 0){
        strcpy(NewPath,InputBuffer);
        //printf("Path: %s\n",NewPath);
    }
    else if(InputBuffer[0] == '.' && InputBuffer[1] == '/'){
        sprintf(NewPath,"%s",InputBuffer);
        //printf("Path: %s\n",NewPath);
    }
    else{
        sprintf(NewPath,"%s/%s",HomeDirectoryPath,InputBuffer);
        //printf("Path: %s\n",NewPath);
    }

    if(chdir(NewPath) != 0){
        printf("Directory:%s not found.\n",NewPath);
        return 1;
    }
    return 0;
}

struct InputObj ParseInput(char* InputBuffer)
{
    struct InputObj *Obj = malloc(1 * sizeof(struct InputObj));
    char Buffer[1028];
    char *InputFileName;
    char *OutputFileName;

    InputBuffer[strlen(InputBuffer) -1] = '\0';

    if(InputBuffer[strlen(InputBuffer) -1] == '&'){
        Obj->Background = true;
        printf("Background: enabled\n");
    }
    else{
        Obj->Background = false;
        printf("Background: disabled\n");
    }

    //command
    memset(Buffer,'\0',sizeof(Buffer));
    strcpy(Buffer,InputBuffer);
    strtok(Buffer," ");
    strcpy(Obj->Command,Buffer);
    printf("Command: %s\n",Obj->Command);

    //InputFile Name
    memset(Buffer,'\0',sizeof(Buffer));
    strcpy(Buffer,InputBuffer);
    InputFileName = strstr(Buffer,"<");
    if(InputFileName != NULL){
        memmove(InputFileName, InputFileName+2, strlen(InputFileName));
        strtok(InputFileName," ");
        InputFileName[strlen(InputFileName)] = '\0';
        strcpy(Obj->InputFile,InputFileName);
        printf("Inputfile is:%s\n",Obj->InputFile);
    }

    //OutputFile Name;
    memset(Buffer,'\0',sizeof(Buffer));
    strcpy(Buffer,InputBuffer);
    OutputFileName = strstr(Buffer,">");
    if(OutputFileName != NULL){
        memmove(OutputFileName, OutputFileName+2, strlen(OutputFileName));
        strtok(OutputFileName," ");
        OutputFileName[strlen(OutputFileName)] = '\0';
        strcpy(Obj->OutputFile,OutputFileName);
        printf("Outputfile is:%s\n",Obj->OutputFile);
    }
     
    //arguments
    memset(Buffer,'\0',sizeof(Buffer));
    strcpy(Buffer,InputBuffer);

    return *Obj;

} 

int RunCommand(char* InputBuffer)
{
    return 0;
}

void RunShell()
{
    char InputBuffer[1028];

    do
    {
        fflush(stdout);
        fflush(stdin);

        printf(": ");
        fgets(InputBuffer,1028,stdin);
        fflush(stdin);

        if(strncmp(InputBuffer,"exit",4) == 0){
            printf("Exiting....\n");
            return;
        }
        else if(strncmp(InputBuffer, "#",1) == 0){
            //printf("Comment Comment Comment \n");
        }
        else if(strncmp(InputBuffer,"cd ", 3) == 0){
            //printf("Changing directory...\n");
            ChangeDirectorysFromHome(InputBuffer);
        }
        else if(strncmp(InputBuffer,"status",6) == 0){
            printf("exit value %d\n",9000);
        }
        else{
            //read in a command.
            ParseInput(InputBuffer);
        }
    }
    while(true);
}


int main(void)
{
    RunShell();
    return 0;
}