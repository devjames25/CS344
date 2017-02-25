#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#define STACKMAX 64

struct InputObj
{
    bool Background;
    char InputFile[128];
    char OutputFile[128];
    char Command[1028];
    int NumArgs;
    char *Arguments[128];
};

struct PidStackObj
{
    int NumForePid;
    int NumBackPid;
    pid_t ForegroundPids[STACKMAX];
    pid_t BackgroundPids[STACKMAX];
};

//Globals
struct PidStackObj PidStack;
//End Globals

void _InitPidObj()
{
    int i;

    PidStack.NumForePid = -1;
    PidStack.NumBackPid = -1;

    for(i = 0; i < STACKMAX; i++){
        PidStack.ForegroundPids[i] = -1;
        PidStack.BackgroundPids[i] = -1;
    }
}

void PushForePid(pid_t processId)
{
    PidStack.ForegroundPids[++(PidStack.NumForePid)] = processId;
}

pid_t PopForePid()
{
    return PidStack.ForegroundPids[PidStack.NumForePid--];
}

pid_t TopForePid()
{
    return PidStack.ForegroundPids[PidStack.NumForePid]; 
}

void PushBackPid(pid_t processId)
{
    PidStack.BackgroundPids[++(PidStack.NumBackPid)] = processId;
}

pid_t PopBackPid()
{
    return PidStack.BackgroundPids[PidStack.NumBackPid--];
}

pid_t TopBackPid()
{
    return PidStack.BackgroundPids[PidStack.NumBackPid];
}

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

bool _IsLeadCharSpecial(char *str)
{
    bool IsSpecial = false;

    if(str[0] == '&'){
        IsSpecial = true;
    }
    else if(str[0] == '<'){
        IsSpecial = true;
    }
    else if(str[0] == '>'){
        IsSpecial = true;
    }

    return IsSpecial;
}

void ParseInput(char* InputBuffer,struct InputObj* Obj)
{
    //struct InputObj Obj = malloc(1 * sizeof(struct InputObj));
    char Buffer[1028];
    char *InputFileName;
    char *OutputFileName;
    char *Temp;
    int i;

    Obj->NumArgs = 0;
    InputBuffer[strlen(InputBuffer) -1] = '\0';

    if(InputBuffer[strlen(InputBuffer) -1] == '&'){
        Obj->Background = true;
        InputBuffer[strlen(InputBuffer) -1] = '\0';
        printf("Background: enabled\n");
    }
    else{
        Obj->Background = false;
        printf("Background: disabled\n");
    }

    //command
    memset(Buffer,'\0',sizeof(Buffer)); // clear Buffer
    strcpy(Buffer,InputBuffer); //copy buffer
    strtok(Buffer," "); // grab only command part of input;
    strcpy(Obj->Command,Buffer); // take command place in new obj.
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
    strtok(Buffer," ");
    
    // TODO
    Temp = strtok(NULL,"");
    if(_IsLeadCharSpecial(Buffer) == false && Temp != NULL ){
        strcpy(Buffer,Temp);
        strtok(Buffer,"<>&#");
        //printf("Argument Line:%s\n",Buffer);
        
        strtok(Buffer," ");
        Obj->Arguments[0] = Buffer;
        Obj->NumArgs = 1;
        Temp = strtok(NULL," ");
        while(Temp != NULL){
            
            Obj->Arguments[Obj->NumArgs] = Temp;
            Obj->NumArgs++;
            Temp = strtok(NULL," ");
        }
        Obj->Arguments[Obj->NumArgs] = strtok(NULL, "");
           
    }
    
    for(i = 0;i < Obj->NumArgs;i++){
        printf("arg%d:%s\n",i,Obj->Arguments[i]);
    }

    //return Obj;
} 

void _InitArgList(struct InputObj* Obj,char** Args)
{
    int i;

    Args[0] = Obj->Command;
    for(i = 0;i< Obj->NumArgs ;i++){
        Args[i+1] = Obj->Arguments[i];
    }
    Args[i+1] = NULL;
}

void RunCommand(struct InputObj* Obj)
{
    pid_t pid = fork();
    char *ArgList[128];
    int ProcessStatus;

    switch(pid)
    {
        case -1: //Error
            //perror("Something went wrong with your fork\n");
            exit(1);
            break;

        case 0: //Child
            _InitArgList(Obj,ArgList);
            execvp(Obj->Command, ArgList);
            
            //perror("Could not find command\n");
            exit(1);
            break;

        default: // Parent
            if(Obj->Background == true){
                PushBackPid(pid);
                printf("Background Pid is %d\n",TopBackPid());
            }
            else{
                PushForePid(pid);
                waitpid(pid,&ProcessStatus,0);

                printf("parent(%d) waiting for child process(%d)\n",getpid() ,TopForePid());
                PopForePid();
            }
            break;
    }
}

// SOURCE: https://gist.github.com/leehambley/5589953
void _ChildExitedHandler(int sig)
{
    int ChildStatus;

    pid_t ChildPid = waitpid(0,&ChildStatus,0);
    printf("background pid %d is done: exit value %d\n",ChildPid,ChildStatus);


}

void RunShell()
{
    char InputBuffer[1028];
    struct InputObj *Obj;

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
            Obj = malloc(1 * sizeof(struct InputObj)); 
            ParseInput(InputBuffer,Obj);
            RunCommand(Obj);

            free(Obj);//IMPORTANT WITHOUT THIS SIGABRT SIGNAL WILL BE TRIGGERED <<<<<<<<
        }
    }
    while(true);
}


int main(void)
{
    signal(SIGCHLD,_ChildExitedHandler);


    _InitPidObj();
    RunShell();
    return 0;
}