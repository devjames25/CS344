#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>

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
    int NumBackPid;
    pid_t BackgroundPids[STACKMAX];
};

//Globals
struct PidStackObj PidStack;
pid_t LastForeGroundStatus;
bool ForegroundOnly = false;
//End Globals

void _InitPidObj()
{
    int i;
    PidStack.NumBackPid = -1;

    for(i = 0; i < STACKMAX; i++){
        PidStack.BackgroundPids[i] = -1;
    }
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

    if(str == NULL){
        return true;
    }

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
    //int i;

    Obj->NumArgs = 0;
    InputBuffer[strlen(InputBuffer) -1] = '\0';

    if(InputBuffer[strlen(InputBuffer) -1] == '&'){
        Obj->Background = true;
        InputBuffer[strlen(InputBuffer) -1] = '\0';
        //printf("Background: enabled\n");
    }
    else{
        Obj->Background = false;
        //printf("Background: disabled\n");
    }

    //command
    memset(Buffer,'\0',sizeof(Buffer)); // clear Buffer
    strcpy(Buffer,InputBuffer); //copy buffer
    strtok(Buffer," "); // grab only command part of input;
    strcpy(Obj->Command,Buffer); // take command place in new obj.
    //printf("Command: %s\n",Obj->Command);

    //InputFile Name
    memset(Buffer,'\0',sizeof(Buffer));
    strcpy(Buffer,InputBuffer);
    InputFileName = strstr(Buffer,"<");
    if(InputFileName != NULL){
        memmove(InputFileName, InputFileName+2, strlen(InputFileName));
        strtok(InputFileName," ");
        InputFileName[strlen(InputFileName)] = '\0';
        strcpy(Obj->InputFile,InputFileName);
        //printf("Inputfile is:%s\n",Obj->InputFile);
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
        //printf("Outputfile is:%s\n",Obj->OutputFile);
    }
     
     //TODO FIX
    //arguments
    memset(Buffer,'\0',sizeof(Buffer));
    strcpy(Buffer,InputBuffer);
    strtok(Buffer," ");

    Temp = strtok(NULL,"");

    //printf("Temp Line:%s\n",Temp);
    if(_IsLeadCharSpecial(Temp) == false && Temp != NULL ){ // && _IsLeadCharSpecial(Buffer) == false
        strcpy(Buffer,Temp);
        strtok(Buffer,"<>&#");

        
        
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
    
    // int i;
    // for(i = 0;i < Obj->NumArgs;i++){
    //     printf("arg%d:%s\n",i,Obj->Arguments[i]);
    // }
}

void _InitArgList(struct InputObj* Obj,char** Args)
{
    int i;

    Args[0] = Obj->Command;
    for(i = 0;i < Obj->NumArgs ;i++){
        Args[i+1] = Obj->Arguments[i];
    }

    Args[i+1] = NULL;
}

void SetupRedirects(struct InputObj* Obj)
{
    int InputFileDescriptor = STDIN_FILENO;
    int OutputFileDescriptor = STDOUT_FILENO;

    if(Obj->InputFile[0] != '\0'){
        //printf("INPUT: %s\n",Obj->InputFile);
        InputFileDescriptor = open(Obj->InputFile,O_RDONLY);

        if(InputFileDescriptor < 0){
            //perror("Cannot open input file.\n");
            exit(1);
        }

        dup2(InputFileDescriptor,0);
        close(InputFileDescriptor);
    }
    if(Obj->OutputFile[0] != '\0'){
        //printf("OUTPUT: %s\n",Obj->OutputFile);
        
        // Can also use creat() func
        OutputFileDescriptor = open(Obj->OutputFile,O_WRONLY | O_CREAT | O_TRUNC,0644);

        if(OutputFileDescriptor < 0){
            //perror("Cannot create or open ouput file.\n");
            exit(1);
        }

        dup2(OutputFileDescriptor,1);
        close(OutputFileDescriptor);
    }
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
            SetupRedirects(Obj);

            _InitArgList(Obj,ArgList);
            execvp(Obj->Command, ArgList);
            
            fflush(stderr);
            //perror("Could not find command\n");
            

            exit(1);
            break;

        default: // Parent
            if(Obj->Background == true && ForegroundOnly == false){
                PushBackPid(pid);
                printf("Background Pid is %d\n",TopBackPid());
            }
            else{
                
                waitpid(pid,&ProcessStatus,0);
                LastForeGroundStatus = ProcessStatus;
                //printf("parent(%d) waited for child process(%d)\n",getpid() ,pid);
            }
            break;
    }
}

// SOURCE: http://stackoverflow.com/questions/2377811/tracking-the-death-of-a-child-process
bool _BackGroundPidCompleted()
{
    int ChildStatus;
    int i;

    bool BGProcessReturned = false;

    for(i = 0; i < PidStack.NumBackPid + 1; i++){
        waitpid(PidStack.BackgroundPids[i], &ChildStatus, WNOHANG);

        if(WEXITSTATUS(ChildStatus)){
            printf("Background pid %d is done: exit value %d\n",PopBackPid(),WEXITSTATUS(ChildStatus));
            fflush(stdout);
            fflush(stdin);

            BGProcessReturned = true;
        }
        else{
            printf("Background pid %d is done: terminated by signal %d\n", PopBackPid(), WTERMSIG(ChildStatus));
            fflush(stdout);
            fflush(stdin);

            BGProcessReturned = true;
        }
    }
    return BGProcessReturned;
}

//SOURCE: https://piazza.com/class/ixhzh3rn2la6vk?cid=365
void TrapStopSignal(int sig)
{
    if(ForegroundOnly == false){
        char* message = ("\nEntering foreground-only mode (& is now ignored)\n");
        write(STDOUT_FILENO, message, 50);
        ForegroundOnly = true;
    }
    else{
        char* message = "\nExiting foreground-only mode\n";
        write(STDOUT_FILENO, message, 31);
        ForegroundOnly = false;
    }
}

void TrapTermSignal(int sig)
{
    printf("\nterminated by signal %d\n",sig); 
    fflush(stdout);
    fflush(stdin);  
}

void FreeAndClearInputObj(struct InputObj* Obj)
{
    memset(Obj->InputFile,'\0',sizeof(Obj->InputFile));
    memset(Obj->OutputFile,'\0',sizeof(Obj->OutputFile));
    memset(Obj->Command,'\0',sizeof(Obj->Command));
    free(Obj);//IMPORTANT WITHOUT THIS SIGABRT SIGNAL WILL BE TRIGGERED <<<<<<<<
}

void RunShell()
{
    char InputBuffer[1028];
    struct InputObj *Obj;
    int ForegroundStatus;

    //init signals
    struct sigaction StopSignal;
    StopSignal.sa_handler = TrapStopSignal;
    StopSignal.sa_flags = 0;

    struct sigaction TermSignal;
    TermSignal.sa_handler = TrapTermSignal;
    StopSignal.sa_flags = 0;
    //end init signals

    do
    {
        sigaction(SIGTSTP,&StopSignal,NULL);
        sigaction(SIGINT,&TermSignal,NULL);
        

        if(_BackGroundPidCompleted() == true){
            continue;
        }

        fflush(stdout);
        fflush(stdin);

        printf(": ");
        fgets(InputBuffer,1028,stdin);
        
        fflush(stdout);
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
            if(WEXITSTATUS(LastForeGroundStatus)){
                ForegroundStatus = WEXITSTATUS(LastForeGroundStatus);
            }
            else{
                ForegroundStatus = WTERMSIG(LastForeGroundStatus);
            }

            printf("exit value %d\n",ForegroundStatus);
        }
        else{
            //read in a command.
            Obj = malloc(1 * sizeof(struct InputObj)); 
            ParseInput(InputBuffer,Obj);
            RunCommand(Obj);

            FreeAndClearInputObj(Obj);
        }
    }
    while(true);
}

int main(void)
{
    _InitPidObj();
    RunShell();
    return 0;
}