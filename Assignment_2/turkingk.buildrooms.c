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
//END_GLOBALS

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
    char Name[];
};

const char *ROOM_NAMES[TOTAL_NUM_ROOMS] = {
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

//Struct GLOBALS
struct ROOM RoomList[MAX_NUM_ROOMS];

boolean IsAllRoomMinConnectionsValid()
{
    for(int i = 0; i < MAX_NUM_ROOMS;i++){
        if(RoomList[i].TotalConnections < MIN_ROOM_CONNECTIONS
            && RoomList[i].TotalConnections <= MAX_ROOM_CONNECTIONS)
        {
            return FALSE;
        }
    }
    return TRUE;
}

boolean IsNumRoomConnectionsMaxed(int RoomPos)
{
    if(RoomList[RoomPos].Name == NULL){
        return FALSE;
    }
    else if(RoomList[RoomPos].TotalConnections > MAX_ROOM_CONNECTIONS){
        return FALSE;
    }
    else{
        return TRUE;
    }
}

boolean IsAlreadyConnected(int RoomPos1, int RoomPos2)
{
    for(int i = 0; i < MAX_NUM_ROOMS;i++){
        if(RoomList[RoomPos1].Connections[i]->Name == RoomList[RoomPos2].Name)
        {
            return FALSE;
        }
    }
    return TRUE;
}

boolean HasConnection(int RoomPos1, int RoomPos2)
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



// NAME: ConstructRoom
// DESC: like a class constructor this function inializes
// a ROOM struct
struct ROOM ConstructRoom(char Nam[],enum ROOM_TYPES Type)
{
    struct ROOM construct = {
        RType:Type
    };
    strcpy(construct.Name,Nam);
    return construct;
}




int main()
{
    srand(time(NULL));


    return 0;
}