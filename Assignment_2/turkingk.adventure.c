#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#define MIN_ROOM_CONNECTIONS 3
#define MAX_ROOM_CONNECTIONS 6
#define MAX_NUM_ROOMS 7
#define TOTAL_NUM_ROOMS 10

//Type definitions
typedef enum {FALSE = 0, TRUE = 1} boolean;

enum ROOM_TYPES{
    START_ROOM,
    MID_ROOM,
    END_ROOM
};

struct ROOM 
{
    int TotalConnections;
    struct ROOM* Connections[MAX_ROOM_CONNECTIONS];
    enum ROOM_TYPES RType;
    char Name[100];
};
//End Type definitions

//Globals
char FolderName[256];
struct ROOM RoomList[MAX_NUM_ROOMS];
//End Globals

void ClearFolderNameGlobal()
{
    memset(FolderName,'\0',sizeof(FolderName));
}

void SelectFolder()
{
    char *fd = "turkingk.rooms.";
    char currentDir[100];
    DIR *d; //Current Directory
    struct dirent *dp;
    struct stat *buffer;
    time_t lastModified;  // This is equivalent to a long int.
    time_t Newest = 0;

    buffer = malloc(sizeof(struct stat));
    dp = malloc(sizeof(struct dirent));

    ClearFolderNameGlobal();
    memset(currentDir, '\0', sizeof(currentDir));
    getcwd(currentDir, sizeof(currentDir));
    d = opendir(currentDir);

    if (d != NULL) {
        while (dp= readdir(d)) {	
            if (strstr(dp->d_name,fd) != NULL){
                stat(dp->d_name, buffer);
                lastModified = buffer->st_mtime;

                if(lastModified > Newest){
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


void initRoomConnections(int RoomNum){
    int i;
    for(i = 0; i < MAX_ROOM_CONNECTIONS; i++){
            RoomList[RoomNum].Connections[i] = NULL;
    }
}

void initRoomList(){
    int i;
    for(i = 0;i < MAX_NUM_ROOMS;i++){
        memset(RoomList[i].Name,'\0',sizeof(RoomList[i].Name));
        RoomList[i].TotalConnections = 0;
        initRoomConnections(i);
    }
}

void FillRoomListNames()
{
    DIR *dir;
    struct dirent *ent;
    int FileCount = 0;

    initRoomList();

    //http://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
    /*Filling struct Names*/
    if ((dir = opendir (FolderName)) != NULL) {
    /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL) {
            //printf ("%s\n", ent->d_name);
            if(strlen(ent->d_name) > 2){
                strcpy(RoomList[FileCount].Name,ent->d_name);
                FileCount++;
            }
        }
    }
}

int FindRoomPosFromName(char *RoomName)
{
    int RoomPos = -1;
    int i;

    for(i = 0;i < MAX_NUM_ROOMS; i++ ){
        if( strcmp(RoomList[i].Name,RoomName) == 0 ){
            return i;
        }
    }
    return RoomPos;
}

void CleanLabelFromStr(char *LabelStr, char *ValueStr)
{
    int EOLpos = 0;
    int i;

    strtok(LabelStr,":");
    strcpy(ValueStr,strtok(NULL,""));
    ValueStr[strlen(ValueStr) - 1] = '\0';
    LabelStr[strlen(LabelStr) - 1] = '\0';

    for(i = 0;i < strlen(ValueStr);i++){
        ValueStr[i] = ValueStr[i+1];
    }

    // printf("label: %s\n",LabelStr);
    // printf("value: %s\n",ValueStr);
}

void ReCreateConnection(int roomPos1,int roomPos2)
{
    int totCon1 = RoomList[roomPos1].TotalConnections;
    int totCon2 = RoomList[roomPos2].TotalConnections;

    RoomList[roomPos1].Connections[totCon1] = &RoomList[roomPos2];
    RoomList[roomPos1].TotalConnections++;
}



void ReCreateStructRooms()
{

    char FileLineBuffer[256];
    char FileValueBuffer[256];

    FILE *RoomFile;
    int i;

    FillRoomListNames();
    chdir(FolderName);

    //dont need to check if file exists since we grabed it eariler
    for(i = 0;i < MAX_NUM_ROOMS;i++){
        RoomFile = fopen(RoomList[i].Name,"r");//OPEN FILE

        if(RoomFile == NULL){
            printf("%s file was not accessed\n",RoomList[i].Name);
            return;
        }

        memset(FileLineBuffer,'\0',sizeof(FileLineBuffer));
        memset(FileValueBuffer,'\0',sizeof(FileValueBuffer));

        while(fgets(FileLineBuffer,sizeof(FileLineBuffer),RoomFile) != NULL){

            CleanLabelFromStr(FileLineBuffer,FileValueBuffer);
            if(strcmp(FileLineBuffer,"ROOM TYP") == 0){

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
            else if(strcmp(FileLineBuffer,"CONNECTION ") == 0){
                int conncRoomPos = FindRoomPosFromName(FileValueBuffer);
                ReCreateConnection(i,conncRoomPos);
                //printf("Room CONNECTION:%s,%d\n",FileValueBuffer,conncRoomPos);
            }
        }

        fclose(RoomFile);//END FILE

    }
    chdir("..");
}

int FindStartRoomPos(){
    int i;
    for(i =0;i< MAX_NUM_ROOMS; i++){
        if(RoomList[i].RType == START_ROOM){
            return i;
        }
    }
    return -1;
}

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

int main()
{
    SelectFolder();
    ReCreateStructRooms();
    PrintRooms_DEBUG();
}