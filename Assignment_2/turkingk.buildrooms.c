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
    char *Name;
};

//ROOM GLOBAL
struct ROOM RoomList[MAX_NUM_ROOMS];

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
    // if(RoomList[RoomPos].Name == NULL){
    //     return FALSE;
    // }
    if(RoomList[RoomPos].TotalConnections > MAX_ROOM_CONNECTIONS){
        return FALSE;
    }
    else{
        return TRUE;
    }
}

boolean IsAlreadyConnected(int RoomPos1, int RoomPos2)
{
    for(int i = 0; i < MAX_NUM_ROOMS;i++){
        printf("IsAlreadyConnected: %d\n",i);
        if(RoomList[RoomPos1].Connections[i] == NULL){
            return FALSE;
        }
        else if(strcmp(RoomList[RoomPos1].Connections[i]->Name,RoomList[RoomPos2].Name) == 0)
        {
            return FALSE;
        }
    }
    return TRUE;
}

boolean IsNotConnected(int RoomPos1, int RoomPos2)
{
    if(IsAlreadyConnected(RoomPos1,RoomPos2)){
        return FALSE;
    }
    else if(RoomList[RoomPos1].Name == NULL || RoomList[RoomPos2].Name == NULL){
        return FALSE;
    }
    else if(RoomList[RoomPos1].Name == RoomList[RoomPos2].Name){
        return FALSE;
    }
    else{
        return TRUE;
    }
}

int RandomPickRoom()
{
    int RoomPos;

    do
    {
        RoomPos = rand() % 10;
    }
    while(IsNumRoomConnectionsNotMaxed(RoomPos) == FALSE);

    return RoomPos;
}

void RandomConnectTwoRooms()
{
    boolean connected = FALSE;
    int Room1,Room2;

    do
    {
        Room1 = RandomPickRoom();
        Room2 = RandomPickRoom();

        if(IsNotConnected(Room1,Room2)){
            RoomList[Room1].Connections[RoomList[Room1].TotalConnections] = &RoomList[Room2];
            RoomList[Room2].Connections[RoomList[Room2].TotalConnections] = &RoomList[Room1];
            RoomList[Room1].TotalConnections++;
            RoomList[Room2].TotalConnections++;
            connected = TRUE;
        }
    }
    while(connected == FALSE);
}

void RandomConnectARoom(int RoomPos)
{
    boolean connected = FALSE;
    int Room1,Room2;

    do
    {
        Room1 = RoomPos;
        //printf("rando");

        Room2 = RandomPickRoom();

        //printf("rando connectrom");

        if(IsNotConnected(Room1,Room2)){
            RoomList[Room1].Connections[RoomList[Room1].TotalConnections] = &RoomList[Room2];
            RoomList[Room2].Connections[RoomList[Room2].TotalConnections] = &RoomList[Room1];
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
    boolean InitializedRooms[TOTAL_NUM_ROOMS];
    for(int i = 0; i < MAX_NUM_ROOMS;i++){
        InitializedRooms[i] = FALSE;
    }

    for(int i = 0; i < MAX_NUM_ROOMS;i++){
        RoomList[i].TotalConnections = 0;
        RoomList[i].Name = NULL;
        initRoomConnections(i);
    
        while(TRUE){
            int RandomRoom = RandomPickRoom();
            //printf("\n%d\n",RandomRoom);
            if(InitializedRooms[RandomRoom] == FALSE){
                InitializedRooms[RandomRoom] = TRUE;
                //strcpy(RoomList[i].Name,ROOM_NAMES[RandomRoom]);
                RoomList[i].Name = ROOM_NAMES[RandomRoom];
                RoomList[i].RType = MID_ROOM;
                break;
            }
        }
    }
    RoomList[0].RType = START_ROOM;
    RoomList[MAX_NUM_ROOMS - 1].RType = END_ROOM;
}

void FillRoomList()
{
    InitRoomList();
    int MaxConnectionsForRoom;
    
    do{
        
        MaxConnectionsForRoom = rand() % (MAX_ROOM_CONNECTIONS + 1 - MIN_ROOM_CONNECTIONS) + MIN_ROOM_CONNECTIONS;
        for(int i = 0;i < MAX_NUM_ROOMS;i++ ){
            
            for(int j = 0;j < MaxConnectionsForRoom;j++){
                printf("\ndsadasda\n");
                RandomConnectARoom(i);
                printf("\nasdasdasd\n");
            }
        }
    }
    while(IsAllRoomMinConnectionsValid == FALSE);
}

void PrintRooms_DEBUG()
{
    for(int i = 0; i < MAX_NUM_ROOMS;i++){
        printf("\n%d: ",i);
        printf("\n\tName: %s",RoomList[i].Name);
        printf("\n\tTotalConnections: %d",RoomList[i].TotalConnections);
        // if(RoomList[i].TotalConnections > 0){
        //     printf("\n\t\tConnections:");
        //     for(int j = 0;j < RoomList[i].TotalConnections;j++){
        //         printf("\n\t\tC%d:%s",j,RoomList[i].Connections[j]->Name);
        //     }
        // }
        
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