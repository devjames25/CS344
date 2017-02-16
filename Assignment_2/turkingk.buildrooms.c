#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

//GLOBALS
#define MIN_ROOM_CONNECTIONS 3
#define MAX_ROOM_CONNECTIONS 6
#define MAX_NUM_ROOMS 7
#define TOTAL_NUM_ROOMS 10

typedef enum {FALSE = 0, TRUE = 1} boolean;


char *ROOM_NAMES[TOTAL_NUM_ROOMS] = {
        "Dixon_Rec_Center",
        "Memorial_Union",
        "Valley_Library",
        "Reser_Stadium",
        "DearBorn_Hall",
        "Rogers_Hall",
        "Kearney",
        "Kelly_Engineering",
        "Buxton_Dorm",
        "West_Dining"
    };
//END_GLOBALS


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

//ROOM GLOBAL
struct ROOM RoomList[MAX_NUM_ROOMS];
boolean InitializedRooms[TOTAL_NUM_ROOMS];
char folderName[256];

///NAME: IsAllRoomMinConnectionsValid
///DESC: checks if all connections in RoomList
///      have less than MIN_ROOM_CONNECTIONS and
///      MAX_ROOM_CONNECTIONS
boolean IsAllRoomMinConnectionsValid()
{
    int i;
    for(i = 0; i < MAX_NUM_ROOMS;i++){
        if(RoomList[i].TotalConnections < MIN_ROOM_CONNECTIONS)
        {
            return FALSE;
        }
    }
    return TRUE;
}

boolean IsNumRoomConnectionsNotMaxed(int RoomPos)
{
    if(RoomList[RoomPos].TotalConnections == MAX_ROOM_CONNECTIONS){
        return FALSE;
    }
    else{
        return TRUE;
    }
}

boolean IsAlreadyConnected(int RoomPos1, int RoomPos2)
{
    int i;

    if(RoomList[RoomPos1].TotalConnections == MAX_ROOM_CONNECTIONS){
        return TRUE;
    }
    for(i = 0; i < RoomList[RoomPos1].TotalConnections;i++){
        if(RoomList[RoomPos1].Connections[i] == NULL ){
            return FALSE;
        }
        else if(strcmp(RoomList[RoomPos1].Connections[i]->Name,RoomList[RoomPos2].Name) == 0){
            return TRUE;
        }
    }
    return FALSE;
}

boolean IsConnected(int RoomPos1, int RoomPos2)
{
    if(IsAlreadyConnected(RoomPos1,RoomPos2) == TRUE){
        return TRUE;
    }
    else if(strcmp(RoomList[RoomPos1].Name, RoomList[RoomPos2].Name)== 0){
        return TRUE;
    }
    else{
        return FALSE;
    }
}

int RandomPickRoom()
{
    int RoomPos;

    do
    {
        RoomPos = rand() % TOTAL_NUM_ROOMS;
    }
    while(IsNumRoomConnectionsNotMaxed(RoomPos) == FALSE);

    return RoomPos;
}

int RandomRoomWithinRange(int min,int max)
{
    int RoomPos;

    do
    {
        RoomPos = rand() % max + min;
    }
    while(IsNumRoomConnectionsNotMaxed(RoomPos) == FALSE
            && RoomPos <= max
            && RoomPos >= min);
    return RoomPos;
}


int FindNullConnectionPos(int Room1){
    int i;
    for(i = 0; i < MAX_ROOM_CONNECTIONS -1;i++){
        if(RoomList[Room1].Connections[i] == NULL
            ||RoomList[Room1].Connections[i]->Name == NULL ){
            return i;
        }
    }
    return -1;
}

boolean IsRoomInitialized(int RoomPos)
{
    return InitializedRooms[RoomPos];
}

void RandomConnectARoom(int RoomPos)
{
    boolean connected = FALSE;
    int Room1,Room2;

    if(RoomList[RoomPos].TotalConnections == MAX_ROOM_CONNECTIONS){
        return;
    }

    do
    {
        Room1 = RoomPos;
        Room2 = RandomRoomWithinRange(0,MAX_NUM_ROOMS);

        if(IsConnected(Room1,Room2) == FALSE){
            int conc1 = RoomList[Room1].TotalConnections;
            int conc2 = RoomList[Room2].TotalConnections;

            RoomList[Room1].Connections[conc1] = &RoomList[Room2];
            RoomList[Room2].Connections[conc2] = &RoomList[Room1];
            RoomList[Room1].TotalConnections++;
            RoomList[Room2].TotalConnections++;
            connected = TRUE;
        }
    }
    while(connected == FALSE);
}

void initRoomConnections(int RoomNum){
    int i;
    for(i = 0; i < MAX_ROOM_CONNECTIONS; i++){
            RoomList[RoomNum].Connections[i] = NULL;
    }
}

void InitRoomList()
{
    int i;   
    for(i = 0; i < MAX_NUM_ROOMS;i++){
        InitializedRooms[i] = FALSE;
    }

    for(i = 0; i < MAX_NUM_ROOMS;i++){
        RoomList[i].TotalConnections = 0;
        initRoomConnections(i);
    
        while(TRUE){
            int RandomRoom = RandomPickRoom();
            if(InitializedRooms[RandomRoom] == FALSE){
                InitializedRooms[RandomRoom] = TRUE;
                memset(RoomList[i].Name,'\0',sizeof(RoomList[i].Name));
                strcpy(RoomList[i].Name,ROOM_NAMES[RandomRoom]);
                RoomList[i].RType = MID_ROOM;
                break;
            }
        }
    }
    RoomList[0].RType = START_ROOM;
    RoomList[MAX_NUM_ROOMS - 1].RType = END_ROOM;
}

int RandomNumInRange(int min,int max){
    int Rando;
    do{
        Rando = rand() % max + min;
    }
    while(Rando > max || Rando < min);

    return Rando;
}

void FillRoomList()
{
    InitRoomList();
    int MaxConnectionsForRoom = MIN_ROOM_CONNECTIONS;
    int i,j;
    for(i = 0;i < MAX_NUM_ROOMS;i++ ){
        //ConnectionsForRoom = RandomNumInRange(2,MIN_ROOM_CONNECTIONS);
        for(j = 0;j < MaxConnectionsForRoom;j++){
            RandomConnectARoom(i);
        }
    }
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
                printf("\n\tC%d connectTotal: %d",j,RoomList[i].Connections[j]->TotalConnections);
            }
        }
        printf("\n");
    }
    printf("\n");
}

void GenRoomDir()
{
    char* staticDirName = "turkingk.rooms.";
    int pid = getpid();
    int ModeratlyecPermissionSetting = 0770;// rwxrwx---
    int unsecPermissionSetting = 0777; // rwxrwxrwx EXTREMELY UNSAFE

    memset(folderName,'\0',sizeof(folderName));
    sprintf(folderName,"%s%d",staticDirName,pid);

    //printf("%s",folderName);
    mkdir(folderName,ModeratlyecPermissionSetting);
}

void GenRoomFiles()
{
    FILE *roomFile;
    int i,j;
    char folderDIR[256];

    sprintf(folderDIR,"./turkingk.rooms.%d",getpid());

    GenRoomDir();
    
    if(chdir(folderDIR) != 0){
        printf("DIR NOT CHANGED TO: %s\n",folderDIR);
        return;
    }

    for(i = 0; i < MAX_NUM_ROOMS; i++){
        //no conflicts should happen since the directory was 
        //dynamically generated.
        roomFile = fopen(RoomList[i].Name,"w");
        
        fprintf(roomFile,"ROOM NAME: %s\n",RoomList[i].Name);
        for(j = 0;j < RoomList[i].TotalConnections;j++){
            fprintf(roomFile,"CONNECTION %d: %s\n",j+1,RoomList[i].Connections[j]->Name);
        }
        
        if(RoomList[i].RType == START_ROOM){
            fprintf(roomFile,"ROOM TYPE: %s\n","START_ROOM");
        }
        else if(RoomList[i].RType == MID_ROOM){
            fprintf(roomFile,"ROOM TYPE: %s\n","MID_ROOM");
        }
        else if(RoomList[i].RType == END_ROOM){
            fprintf(roomFile,"ROOM TYPE: %s\n","END_ROOM");
        }
        else{
            fprintf(roomFile,"ROOM TYPE: %s\n","NULL");
        }
        fclose(roomFile);
    }
}


int main()
{
    srand(time(NULL));
    FillRoomList();
    //PrintRooms_DEBUG();
    GenRoomFiles();
    return 0;
}