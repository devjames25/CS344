#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>

//GLOBALS
#define MIN_ROOM_CONNECTIONS 3
#define MAX_ROOM_CONNECTIONS 6
#define MAX_NUM_ROOMS 7
#define TOTAL_NUM_ROOMS 10

typedef enum {FALSE = 0, TRUE = 1} boolean;


char *ROOM_NAMES[TOTAL_NUM_ROOMS] = {
        "Dixon Rec Center",
        "Memorial Union",
        "Valley Library",
        "Reser Stadium",
        "DearBorn Hall",
        "Rogers Hall",
        "Kearney",
        "Kelly Engineering",
        "Buxton Dorm",
        "West Dining"
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

///NAME: IsAllRoomMinConnectionsValid
///DESC: checks if all connections in RoomList
///      have less than MIN_ROOM_CONNECTIONS and
///      MAX_ROOM_CONNECTIONS
boolean IsAllRoomMinConnectionsValid()
{
    for(int i = 0; i < MAX_NUM_ROOMS;i++){
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
    if(RoomList[RoomPos1].TotalConnections == MAX_ROOM_CONNECTIONS){
        return TRUE;
    }
    for(int i = 0; i < RoomList[RoomPos1].TotalConnections;i++){
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


int FindNullConnectionPos(int Room1){
    for(int i =0; i < MAX_ROOM_CONNECTIONS -1;i++){
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
        Room2 = RandomPickRoom();


        if(IsConnected(Room1,Room2) == FALSE){
            int conc1 = RoomList[Room1].TotalConnections;
            int conc2 = RoomList[Room2].TotalConnections;
            //int conc1 = FindNullConnectionPos(Room1);
            //int conc2 = FindNullConnectionPos(Room2);

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
    for(int i = 0; i < MAX_ROOM_CONNECTIONS; i++){
            RoomList[RoomNum].Connections[i] = NULL;
    }
}

void InitRoomList()
{
    
    for(int i = 0; i < MAX_NUM_ROOMS;i++){
        InitializedRooms[i] = FALSE;
    }

    for(int i = 0; i < MAX_NUM_ROOMS;i++){
        RoomList[i].TotalConnections = 0;
        initRoomConnections(i);
    
        while(TRUE){
            int RandomRoom = RandomPickRoom();
            if(InitializedRooms[RandomRoom] == FALSE){
                InitializedRooms[RandomRoom] = TRUE;

                strcpy(RoomList[i].Name,ROOM_NAMES[RandomRoom]);
                RoomList[i].RType = MID_ROOM;
                break;
            }
        }
    }
    RoomList[0].RType = START_ROOM;
    RoomList[MAX_NUM_ROOMS - 1].RType = END_ROOM;
}

int ValidRandomConnectionNum(){
    int Rando = MIN_ROOM_CONNECTIONS;
    while(TRUE){
        Rando = rand() % MAX_ROOM_CONNECTIONS + MIN_ROOM_CONNECTIONS;
        if(Rando <= MAX_ROOM_CONNECTIONS
            && Rando >= MIN_ROOM_CONNECTIONS ){
                break;
            }
    }
    return Rando;
}

void FillRoomList()
{
    InitRoomList();
    int MaxConnectionsForRoom = MIN_ROOM_CONNECTIONS;
    for(int i = 0;i < MAX_NUM_ROOMS;i++ ){
        //MaxConnectionsForRoom = ValidRandomConnectionNum() ;
        //printf("%d: Connections: %d\n",i,MaxConnectionsForRoom);
        for(int j = 0;j < MaxConnectionsForRoom;j++){
            RandomConnectARoom(i);
        }
    }
}

void PrintRooms_DEBUG()
{
    for(int i = 0; i < MAX_NUM_ROOMS;i++){
        printf("\n%d: ",i);
        printf("Name: %s",RoomList[i].Name);
        printf("\nTotalConnections: %d",RoomList[i].TotalConnections);
        if(RoomList[i].TotalConnections > 0){
            printf("\n\tConnections:");
            for(int j = 0;j < RoomList[i].TotalConnections;j++){
                printf("\n\tC%d:%s",j,RoomList[i].Connections[j]->Name);
                printf("\n\tC%d connectTotal: %d",j,RoomList[i].Connections[j]->TotalConnections);
            }
        }
        printf("\n");
    }
    printf("\n");
}


int main()
{
    srand(time(NULL));
    //InitRoomList();
    FillRoomList();
    PrintRooms_DEBUG();
    return 0;
}