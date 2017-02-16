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

void ReCreateRooms()
{
    
}

int main()
{
    SelectFolder();


}