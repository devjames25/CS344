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

/// NAME: boolean
/// DESC: This helps me not mix up my true and false while programming.
///       And since we are using an earlier version of C.
/// SOURCE: http://stackoverflow.com/questions/1909825/error-with-the-declaration-of-enum
typedef enum {FALSE = 0, TRUE = 1} boolean;

/// NAME: ROOM_NAMES
/// DESC: Global of possible room names.
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

/// NAME: IsNumRoomConnectionsNotMaxed
/// DESC: this is a helper function that checks if a room has too many connections
boolean IsNumRoomConnectionsNotMaxed(int RoomPos)
{
    if(RoomList[RoomPos].TotalConnections == MAX_ROOM_CONNECTIONS){
        return FALSE; // doesnt have too many connections
    }
    else{
        return TRUE; // maxed out connections 
    }
}


/// NAME: IsAlreadyConnected
/// DESC: helper function Checks the roomlist global if two rooms are connected or not.
boolean IsAlreadyConnected(int RoomPos1, int RoomPos2)
{
    int i;

    //this tells the connect room function not to connect the rooms 
    if(RoomList[RoomPos1].TotalConnections == MAX_ROOM_CONNECTIONS){
        return TRUE;
    }

    //iterates through the list of connections and checks for a conneciton.
    for(i = 0; i < RoomList[RoomPos1].TotalConnections;i++){
        if(RoomList[RoomPos1].Connections[i] == NULL ){//short circuit if list isnt completely full
            return FALSE;
        }
        else if(strcmp(RoomList[RoomPos1].Connections[i]->Name,RoomList[RoomPos2].Name) == 0){
            return TRUE; //checks if rooms are connected.
        }
    }
    return FALSE;
}

/// NAME: IsConnected
/// DESC: Another helper function, for checking if the rooms are the same or connected.
boolean IsConnected(int RoomPos1, int RoomPos2)
{
    if(IsAlreadyConnected(RoomPos1,RoomPos2) == TRUE){//use helper function to check connection
        return TRUE;
    }
    else if(strcmp(RoomList[RoomPos1].Name, RoomList[RoomPos2].Name)== 0){//checks if referencing the same room.
        return TRUE;
    }
    else{
        return FALSE;
    }
}

/// NAME: RandomPickRoom
/// DESC: returns a random int which is a room position.
int RandomPickRoom()
{
    int RoomPos;

    do
    {
        RoomPos = rand() % TOTAL_NUM_ROOMS;// gen random room;
    }
    while(IsNumRoomConnectionsNotMaxed(RoomPos) == FALSE);// ONLY return non maxed out connected rooms.

    return RoomPos;
}

/// NAME: RandomRoomWithinRange
/// DESC: gens a random number with in Ranger
///       This was made because I was having problems with random numbers out of range.
int RandomRoomWithinRange(int min,int max)
{
    int RoomPos;

    do
    {
        RoomPos = rand() % max + min; // gen random num.
    }
    while(IsNumRoomConnectionsNotMaxed(RoomPos) == FALSE
            && RoomPos <= max
            && RoomPos >= min);// check if numbers are out of range.
    return RoomPos;
}

// NAME: RandomConnectARoom
// DESC: connects a random room to a specified room.
void RandomConnectARoom(int RoomPos)
{
    boolean connected = FALSE;
    int Room1,Room2;

    // checks is specified room connections are maxxed.
    if(RoomList[RoomPos].TotalConnections == MAX_ROOM_CONNECTIONS){
        return;
    }

    do
    {
        Room1 = RoomPos;
        Room2 = RandomRoomWithinRange(0,MAX_NUM_ROOMS);// gen a room number

        if(IsConnected(Room1,Room2) == FALSE){ // begin connecting the rooms.
            int conc1 = RoomList[Room1].TotalConnections;
            int conc2 = RoomList[Room2].TotalConnections;

            RoomList[Room1].Connections[conc1] = &RoomList[Room2];
            RoomList[Room2].Connections[conc2] = &RoomList[Room1];
            RoomList[Room1].TotalConnections++;
            RoomList[Room2].TotalConnections++;
            connected = TRUE;// stop the loop.
        }
    }
    while(connected == FALSE); // once connected stop.
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
/// DESC: gens a the roomlist global with default values
void InitRoomList()
{
    int i;   
    // tells me which 7 rooms I picked out of 10
    for(i = 0; i < MAX_NUM_ROOMS;i++){
        InitializedRooms[i] = FALSE;
    }

    // begin init
    for(i = 0; i < MAX_NUM_ROOMS;i++){
        RoomList[i].TotalConnections = 0;
        initRoomConnections(i);
    
        while(TRUE){
            int RandomRoom = RandomPickRoom(); // sets a room name to be used.
            if(InitializedRooms[RandomRoom] == FALSE){ // checks if the room was already picked
                InitializedRooms[RandomRoom] = TRUE;
                memset(RoomList[i].Name,'\0',sizeof(RoomList[i].Name)); // I made a habit of placing these everywhere.
                strcpy(RoomList[i].Name,ROOM_NAMES[RandomRoom]);
                RoomList[i].RType = MID_ROOM;
                break;
            }
        }
    }
    //defines a start and winning room.
    RoomList[0].RType = START_ROOM;
    RoomList[MAX_NUM_ROOMS - 1].RType = END_ROOM;
}

/// NAME: RandomNumInRange
/// DESC: duplicate function will be cleaned if I have time later.
int RandomNumInRange(int min,int max){
    int Rando;
    do{
        Rando = rand() % max + min;
    }
    while(Rando > max || Rando < min);

    return Rando;
}

/// NAME: FillRoomList
/// DESC: Begins generating the graph.
void FillRoomList()
{
    InitRoomList();
    int MaxConnectionsForRoom = MIN_ROOM_CONNECTIONS;
    int i,j;
    for(i = 0;i < MAX_NUM_ROOMS;i++ ){
        //ConnectionsForRoom = RandomNumInRange(2,MIN_ROOM_CONNECTIONS);
        for(j = 0;j < MaxConnectionsForRoom;j++){
            RandomConnectARoom(i); // this is run 3 times per room to garuntee the room has 3 connections.
        }
    }
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
                printf("\n\tC%d connectTotal: %d",j,RoomList[i].Connections[j]->TotalConnections);
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

/// NAME: GenRoomDir
/// DESC: creates a new rooms directory with moderately secure permissions.
void GenRoomDir()
{
    char* staticDirName = "turkingk.rooms.";
    int pid = getpid();
    int ModeratlyecPermissionSetting = 0770;// rwxrwx---
    int unsecPermissionSetting = 0777; // rwxrwxrwx EXTREMELY UNSAFE

    memset(folderName,'\0',sizeof(folderName));
    sprintf(folderName,"%s%d",staticDirName,pid); // sets the new folder name.

    //printf("%s",folderName);
    mkdir(folderName,ModeratlyecPermissionSetting); // creates folder.
}


/// NAME: GenRoomFiles
/// DESC: copys roomlist struct and prints to a file.
void GenRoomFiles()
{
    FILE *roomFile;
    int i,j;
    char folderDIR[256];

    sprintf(folderDIR,"./turkingk.rooms.%d",getpid()); // gets the folders future directory.

    GenRoomDir(); // sets a folder.
    
    // prevents errors, checks if the folder exists or directory was changed.
    if(chdir(folderDIR) != 0){
        printf("DIR NOT CHANGED TO: %s\n",folderDIR);
        return;
    }

    //begins genning files
    for(i = 0; i < MAX_NUM_ROOMS; i++){
        //no conflicts should happen since the directory was 
        //dynamically generated.
        roomFile = fopen(RoomList[i].Name,"w");
        
        //prints room name then connections
        fprintf(roomFile,"ROOM NAME: %s\n",RoomList[i].Name);
        for(j = 0;j < RoomList[i].TotalConnections;j++){
            fprintf(roomFile,"CONNECTION %d: %s\n",j+1,RoomList[i].Connections[j]->Name);
        }
        
        //checks the room type and prints to file.
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


int main(void)
{
    srand(time(NULL));
    FillRoomList();
    //PrintRooms_DEBUG();
    GenRoomFiles();
    return 0;
}