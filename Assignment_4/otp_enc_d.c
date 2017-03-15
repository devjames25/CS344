#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#define STACKMAX 5

//Taken from assignment3
struct PidStackObj
{
    int NumBackPid;
    pid_t BackgroundPids[STACKMAX];
};

//NON MACRO GLOBALS
struct PidStackObj PidStack;
char* PROGNAME = "otp_enc_d";

/// NAME: _InitPidObj
/// DESC: Creates pid stack with -1 in each val.
/// SOURCE: assignment3
void _InitPidObj()
{
    int i;
    PidStack.NumBackPid = -1;

    for(i = 0; i < STACKMAX -1; i++){
        PidStack.BackgroundPids[i] = -1;
    }
}

/// NAME: PushBackPid
/// DESC: adds a pid to the stack
/// SOURCE: assignment3
void PushBackPid(pid_t processId)
{
    PidStack.BackgroundPids[++(PidStack.NumBackPid)] = processId;
}

/// NAME: PopBackPid
/// DESC: removes pid from top
/// SOURCE: assignment3
pid_t PopBackPid()
{
    return PidStack.BackgroundPids[PidStack.NumBackPid--];
}

/// NAME: TopBackPid
/// DESC: reads top of stack
/// SOURCE: assignment3
pid_t TopBackPid()
{
    return PidStack.BackgroundPids[PidStack.NumBackPid];
}

/// NAME: KillBGProcesses
/// DESC: helper function for exting.
/// SOURCE: assignment3
void TerminateServer(int sig)
{
    int i;
    for(i = 0;i < PidStack.NumBackPid + 1;i++){
        kill(PidStack.BackgroundPids[i], SIGINT); // interrupt all bg pids.
    }
}

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

void SpecificError(const char* msg) 
{
	fprintf(stderr,"%s: %s\n",PROGNAME,msg);
	exit(1);
}

int main(int argc, char *argv[])
{
	//initialization.
	_InitPidObj();
	signal(SIGINT,TerminateServer);





	

	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
	socklen_t sizeOfClientInfo;
	char buffer[256];
	struct sockaddr_in serverAddress, clientAddress;

	if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0){ 
		error("ERROR opening socket");
	}

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0){ // Connect socket to port
		error("ERROR on binding");
	}
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

	// Accept a connection, blocking if one is not available until one connects
	sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
	establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
	if (establishedConnectionFD < 0) error("ERROR on accept");

	


	return 0; 
}
