#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#define MIN_ROOM_CONNECTIONS 3
#define MAX_ROOM_CONNECTIONS 6
#define MAX_NUM_ROOMS 7
#define TOTAL_NUM_ROOMS 10

/// NAME: boolean
/// DESC: This helps me not mix up my true and false while programming.
///       And since we are using an earlier version of C.
/// SOURCE: http://stackoverflow.com/questions/1909825/error-with-the-declaration-of-enum
typedef enum {FALSE = 0, TRUE = 1} boolean;

///NAME: ROOM_TYPES
///DESC: Determines if start,mid, or end room.
enum ROOM_TYPES{
    START_ROOM,
    MID_ROOM,
    END_ROOM
};

///NAME: ROOM
///DESC: Defines Room specs.
struct ROOM 
{
    int TotalConnections;
    struct ROOM* Connections[MAX_ROOM_CONNECTIONS];
    enum ROOM_TYPES RType;
    char Name[100];
};
//End Type definitions

//Globals
char *TimeFileName = "currentTime.txt";
char FolderName[256];
struct ROOM RoomList[MAX_NUM_ROOMS];
pthread_mutex_t TimeFile_Mutex;
//End Globals


/// NAME: ClearFolderNameGlobal
/// DESC: helper function to clear the folder global name.
void ClearFolderNameGlobal()
{
    memset(FolderName,'\0',sizeof(FolderName));
}

/// NAME: SelectFolder
/// DESC: this is similar to post 234 on piazza.
void SelectFolder()
{
    char *fd = "turkingk.rooms.";
    char currentDir[100];
    DIR *d; //Current Directory
    struct dirent *dp;
    struct stat *buffer;
    time_t lastModified;  // This is equivalent to a long int.
    time_t Newest = 0;

    //creates buffers to read in folder stats.
    buffer = malloc(sizeof(struct stat));
    dp = malloc(sizeof(struct dirent));

    //clears folder name.
    ClearFolderNameGlobal();
    memset(currentDir, '\0', sizeof(currentDir));
    getcwd(currentDir, sizeof(currentDir));
    d = opendir(currentDir);// opens current directory.

    //makes sure the current directory was opened to not throw erros.
    if (d != NULL) {
        while (dp= readdir(d)) {// read all files.	
            if (strstr(dp->d_name,fd) != NULL){
                stat(dp->d_name, buffer);// read in stats of a folder
                lastModified = buffer->st_mtime; // give me the last lastModified date of a file (long int)

                if(lastModified > Newest){ // if this folder is the newest set as the newest.
                    Newest = lastModified;
                    strcpy(FolderName,dp->d_name);
                }
                //printf("%s: %s\n", dp->d_name, ctime(&lastModified));
                //printf("%s: %ld\n", dp->d_name, lastModified);
            }
        }
    }
    //printf("This is the newest: %s\n",FolderName);
}

/// NAME: initRoomConnections
/// DESC: makes sure no garbage is in the room connections
void initRoomConnections(int RoomNum){
    int i;
    for(i = 0; i < MAX_ROOM_CONNECTIONS; i++){
            RoomList[RoomNum].Connections[i] = NULL;
    }
}



/// NAME: InitRoomList
/// DESC: gens a the roomlist global with default values.
void initRoomList(){
    int i;
    for(i = 0;i < MAX_NUM_ROOMS;i++){
        memset(RoomList[i].Name,'\0',sizeof(RoomList[i].Name)); // sets name to null
        RoomList[i].TotalConnections = 0; // sets connection to 0
        initRoomConnections(i); // sets connecitons to null.
    }
}

/// NAME: FillRoomListNames
/// DESC: from each file fill the names for each room.
/// SOURCE: http://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
void FillRoomListNames()
{
    DIR *dir;
    struct dirent *ent;
    int FileCount = 0;//iterates through the room structs

    initRoomList();//initalize the roomlist.

    /*Filling struct Names*/
    if ((dir = opendir (FolderName)) != NULL) {
    /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL) {
            //printf ("%s\n", ent->d_name);
            if(strlen(ent->d_name) > 2){ // copy file name to struct.
                strcpy(RoomList[FileCount].Name,ent->d_name);
                FileCount++;
            }
        }
    }
}


/// NAME: FindRoomPosFromName
/// DESC: Helper function to find the integer position of a room.
int FindRoomPosFromName(char *RoomName)
{
    int RoomPos = -1;
    int i;

    for(i = 0;i < MAX_NUM_ROOMS; i++ ){
        if( strcmp(RoomList[i].Name,RoomName) == 0 ){ // if the roomname matches return its position.
            return i;
        }
    }
    return RoomPos; // return -1 if nothing was found.
}


/// NAME: CleanLabelFromStr
/// DESC:  helper function, this returns a label of a file buffer, and value of the other half of it.
void CleanLabelFromStr(char *LabelStr, char *ValueStr)
{
    int EOLpos = 0;
    int i;

    strtok(LabelStr,":");// cut label from value at :
    strcpy(ValueStr,strtok(NULL,""));// set value from strtok.
    ValueStr[strlen(ValueStr) - 1] = '\0';//append end of line
    LabelStr[strlen(LabelStr) - 1] = '\0';

    for(i = 0;i < strlen(ValueStr);i++){ // remove space form value string.
        ValueStr[i] = ValueStr[i+1];
    }

    // printf("label: %s\n",LabelStr);
    // printf("value: %s\n",ValueStr);
}


/// NAME: ReCreateConnection
/// DESC: creates a connection FOR ONLY 1 of the structs.
void ReCreateConnection(int roomPos1,int roomPos2)
{
    int totCon1 = RoomList[roomPos1].TotalConnections;

    RoomList[roomPos1].Connections[totCon1] = &RoomList[roomPos2]; // add address
    RoomList[roomPos1].TotalConnections++; // inc connection.
}


/// NAME: ReCreateStructRooms
/// DESC: re create all structs from the files in the most recent directory.
void ReCreateStructRooms()
{
    char FileLineBuffer[256];
    char FileValueBuffer[256];

    FILE *RoomFile;// file pointer
    int i;

    FillRoomListNames(); // fill struct with file names
    chdir(FolderName); // change to the directory containing all the files.

    //dont need to check if file exists since we grabed it eariler
    for(i = 0;i < MAX_NUM_ROOMS;i++){
        RoomFile = fopen(RoomList[i].Name,"r");//OPEN FILE

        if(RoomFile == NULL){ // check if file was opened
            printf("%s file was not accessed\n",RoomList[i].Name);
            return;
        }

        memset(FileLineBuffer,'\0',sizeof(FileLineBuffer));
        memset(FileValueBuffer,'\0',sizeof(FileValueBuffer));

        // get each line from the file.
        while(fgets(FileLineBuffer,sizeof(FileLineBuffer),RoomFile) != NULL){

            //get the label and value from the line.
            CleanLabelFromStr(FileLineBuffer,FileValueBuffer);
            if(strcmp(FileLineBuffer,"ROOM TYP") == 0){ // fill in room type.

                if(strcmp(FileValueBuffer,"START_ROOM") == 0){
                    RoomList[i].RType = START_ROOM;
                }
                else if(strcmp(FileValueBuffer,"END_ROOM") == 0){
                    RoomList[i].RType = END_ROOM;
                }
                else{
                    RoomList[i].RType = MID_ROOM;
                }
                //printf("Room typ:%s\n",FileValueBuffer);
            }
            else if(strcmp(FileLineBuffer,"CONNECTION ") == 0){ // fill in connections.
                int conncRoomPos = FindRoomPosFromName(FileValueBuffer);
                ReCreateConnection(i,conncRoomPos);
                //printf("Room CONNECTION:%s,%d\n",FileValueBuffer,conncRoomPos);
            }
        }
        fclose(RoomFile);//END FILE
    }
    chdir(".."); // go back to main directory.
}

/// NAME: FindStartRoomPos
/// DESC: return the position within the structs where the start room is.
int FindStartRoomPos(){
    int i;
    for(i =0;i< MAX_NUM_ROOMS; i++){ // for each room
        if(RoomList[i].RType == START_ROOM){ 
            return i; // return room position.
        }
    }
    return -1; // start room not found.
}

/// NAME: printStepPath
/// DESC: from a array of ints get each name and print their names.
void printStepPath(int *Path,int steps)
{
    int i;
    for(i = 0;i < steps + 1;i++){ // for amount of steps print a name.
        printf("%s\n",RoomList[Path[i]].Name);
    }
}

/// NAME: CreateCurrentTimeFile
/// DESC: Creates a time file in current diretory.
/// SOURCE: http://stackoverflow.com/questions/5141960/get-the-current-time-in-c
void* CreateCurrentTimeFile()
{
    char TimeStr[256];
    time_t CurrTime;
    struct tm * TimeInfo;
    FILE *TimeFile;

    memset(TimeStr,'\0',sizeof(TimeStr)); // clear time string of garbage.

    time(&CurrTime); // get current time.
    TimeInfo = localtime(&CurrTime); // put time into an easily accessable struct.
    strftime(TimeStr,256, "%I:%M%P %A, %B %d, %Y", TimeInfo); // format string.
    //printf("\n%s\n\n",TimeStr);

    TimeFile = fopen(TimeFileName,"w");//Will create or overwrite a file
    fprintf(TimeFile,"%s\n",TimeStr); // print time to file.
    fclose(TimeFile);

    return NULL;
}

/// NAME: ReadCurrentTimeFile
/// DESC: reads in a file and display the current time.
void ReadCurrentTimeFile()
{
    char Buffer[256];
    FILE *TimeFile;

    memset(Buffer,'\0',sizeof(Buffer)); // clear buffer of garbage.

    TimeFile = fopen(TimeFileName,"r"); // readin a file.
    if(TimeFile == NULL){// check if the file exists.
        printf("%s was not accessed.\n",TimeFileName);
        return;
    }

    //read in each line in the file (there should only be one.)
    while(fgets(Buffer,256,TimeFile) != NULL){
        printf("%s\n",Buffer); // print the line.
    }
    fclose(TimeFile);
}


/// NAME: TimeThread
/// DESC: creates a seperate thread to write a file containing local time.
boolean TimeThread()
{
    pthread_t WriteTimeFile_Thread; // holder for the thread that will contain the function.
    pthread_mutex_lock(&TimeFile_Mutex); // this thread cannot be used untill its done running.

    // if something went wrong dont continue.
    if(pthread_create(&WriteTimeFile_Thread,NULL,CreateCurrentTimeFile,NULL) != 0){ // begin running write file function.
        printf("Error from thread!");
        return FALSE;
    }

    //once done unlock the mutex.
    pthread_mutex_unlock(&TimeFile_Mutex);
    // prevent runnaway processes.
    pthread_join(WriteTimeFile_Thread,NULL);
    return TRUE;
}

/// NAME: RunGame
/// DESC: singleton to run the game.
void RunGame()
{
    int currStep = 0;
    int stepRecord[1028];
    int currRoomPos,i;
    boolean MatchedToRoom = FALSE;
    struct ROOM currRoom;
    char InputBuffer[256];

    stepRecord[currStep] = FindStartRoomPos();// find the start room position.

    do{
        //record step to room.
        currRoomPos = stepRecord[currStep];
        currRoom = RoomList[currRoomPos];

        printf("CURRENT LOCATION: %s\n",currRoom.Name);

        printf("POSSIBLE CONNECTIONS:");
        //print out connections by name.
        for(i = 0; i < currRoom.TotalConnections - 1; i++){
            printf(" %s,",currRoom.Connections[i]->Name);
        }
        // print last connection.
        printf(" %s.\n",currRoom.Connections[i]->Name);//NOTE: i is post dec

        // ask for user input.
        memset(InputBuffer,'\0',sizeof(InputBuffer));
        printf("WHERE TO? >");
        scanf("%255s",InputBuffer);
        printf("\n");

        //conditional for what is printed after user input.
        MatchedToRoom = FALSE;

        //check if input matches the name of a room.
        for(i = 0; i < currRoom.TotalConnections; i++){
            if(strcmp(InputBuffer,currRoom.Connections[i]->Name) == 0){ // if match
                ++currStep; // inc step count.
                stepRecord[currStep] = FindRoomPosFromName(InputBuffer); // record position of the room in roomlist.
                currRoomPos = stepRecord[currStep]; // iterate to next room.
                currRoom = RoomList[currRoomPos];
                MatchedToRoom = TRUE; // tell later conditions that room match happened.
                if(currRoom.RType == END_ROOM){ // check if room is end room.
                    printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
                    printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n",currStep + 1);
                    printStepPath(stepRecord,currStep);
                    return;
                }
            }
        }

        // if user input is equal to time, print it out.
        if(strcmp(InputBuffer,"time") == 0 && MatchedToRoom == FALSE){
            //printf("TIME FUNCTION NOT DONE YET.\n\n");
            if( TimeThread() == TRUE){
                ReadCurrentTimeFile(); // if file was written read it.
            }
            
        }
        // error message to user.
        else if(MatchedToRoom == FALSE){
            printf("HUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
        }
    }
    while(TRUE);
}


/// NAME: PrintRooms_DEBUG
/// DESC: DEBUG FUNCTION lets me check if a roomlist is correct.
void PrintRooms_DEBUG()
{
    int i,j;
    for(i = 0; i < MAX_NUM_ROOMS;i++){
        printf("\n%d: ",i);
        printf("Name: %s",RoomList[i].Name);
        printf("\nTotalConnections: %d",RoomList[i].TotalConnections);
        if(RoomList[i].TotalConnections > 0){
            printf("\n\tConnections:");
            for(j = 0;j < RoomList[i].TotalConnections;j++){
                printf("\n\tC%d:%s",j,RoomList[i].Connections[j]->Name);
                //printf("\n\tC%d connectTotal: %d",j,RoomList[i].Connections[j]->TotalConnections);
            }
        }

        if(RoomList[i].RType == START_ROOM){
            printf("\nRoom Type: START_ROOM");
        }
        else if(RoomList[i].RType == END_ROOM){
            printf("\nRoom Type: END_ROOM");
        }
        else{
            printf("\nRoom Type: MID_ROOM");
        }
        printf("\n");
    }
    printf("\n");
}

int main(void)
{
    SelectFolder();
    ReCreateStructRooms();    

    RunGame();
    //PrintRooms_DEBUG();
}