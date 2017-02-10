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
typedef enum {FALSE = 0, TRUE} boolean;
//END_GLOBALS

const char *ROOM_NAMES[10] = {
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

enum ROOM_TYPES{
    START_ROOM,
    MID_ROOM,
    END_ROOM
};

struct ROOM 
{
    struct ROOM* Connections[MAX_ROOM_CONNECTIONS];
    enum ROOM_TYPES RType;
    char Name[];
};

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

struct ROOM InitilizeRooms(){
    struct ROOM start = {};
    struct ROOM end = {};

    return start;
}


int main()
{
    srand(time(NULL));


    return 0;
}