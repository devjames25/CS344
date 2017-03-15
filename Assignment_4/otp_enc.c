#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <fcntl.h>
#include <sys/stat.h>

//NON MACRO GLOBALS
char* PROGNAME = "otp_enc";

struct FileInfoObject{
	int KeyDescriptor;
	int TextDescriptor;

	int FileLength;// Key and Text must be the same length

	char* KeyFileName;
	char* TextFileName;
};
typedef struct FileInfoObject FileInfoObject;

void error(const char *msg) 
{ 
	perror(msg); exit(0); // Error function used for reporting issues
} 

void SpecificError(const char* msg) 
{
	fprintf(stderr,"%s: %s\n",PROGNAME,msg);
	exit(1);
}

void validArgc(int argc)
{
    //expected ./Keygen ###
    if(argc == 3){
        SpecificError("Invalid number of arguments.");
        exit(1);
    }
}

FileInfoObject* InitEncryptionObject(char** argv)
{
	FileInfoObject* file_ob = malloc(1 * sizeof(FileInfoObject));

	file_ob->TextFileName = argv[1];
	file_ob->KeyFileName = argv[2];

	file_ob->TextDescriptor = open(argv[1],O_RDONLY);
	if(file_ob->TextDescriptor < 0){
		SpecificError("Couldnt open plaintext file");
	}

	file_ob->KeyDescriptor = open(argv[2],O_RDONLY);
	if(file_ob->KeyDescriptor < 0){
		SpecificError("Couldnt open keytext file");
	}

	return file_ob;
}

char* ValidateTextfileContent(FileInfoObject* Obj)
{
	int i;
	char* TextfileContent = malloc( Obj->FileLength * sizeof(char*));
	int Buffer;

	if(read(Obj->TextDescriptor,TextfileContent,Obj->FileLength) < 0){ // redundant.
		SpecificError("Couldnt open plaintext file");
	}

	for(i = 0; i < Obj->FileLength; i++){
		Buffer = (TextfileContent[i]);
		if( !(Buffer == ' ' || (Buffer >= 'A' && Buffer <= 'Z')) ){
			SpecificError("Invalid character in text file.");
		}

	}
	return TextfileContent;

}

void CloseEncrytionObjFD(FileInfoObject* Obj)
{
	close(Obj->TextDescriptor);
	close(Obj->KeyDescriptor);

	Obj->TextDescriptor = -1;
	Obj->KeyDescriptor = -1;
}

int GetEncryptionObjFileLength(FileInfoObject* Obj)
{
	struct stat Key,Text;

	if(stat(Obj->KeyFileName,&Key) < 0){
		SpecificError("Error getting stats of Keyfile.");
	}
	if(stat(Obj->TextFileName,&Text) < 0){
		SpecificError("Error getting stats of Textfile.");
	}

	if(Key.st_size -1 < Text.st_size -1){
		SpecificError("Error Textfile and Keyfile are not the same lengths");
	}
	else{
		Obj->FileLength = Text.st_size -1;
	}

	return Obj->FileLength;
}

int main(int argc, char* argv[])
{
	int socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char buffer[256];
    
	validArgc(argc);
	FileInfoObject* FileInfo = InitEncryptionObject(argv);
	GetEncryptionObjFileLength(FileInfo);

	//Read Key and TextFile.
	char* KeyText = malloc( FileInfo->FileLength * sizeof(char*));
	char* RecievedText = malloc( FileInfo->FileLength * sizeof(char*));
	char* PlainText= ValidateTextfileContent(FileInfo);
	read(FileInfo->KeyDescriptor,KeyText,FileInfo->FileLength);

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number

	// only connections on your current machine.
	serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { 
		fprintf(stderr, "CLIENT: ERROR, no such host\n");
		exit(1);
	}
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0){ 
		error("CLIENT: ERROR opening socket");
	}
	
	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){ // Connect socket to address
	 	error("CLIENT: ERROR connecting");
	}

	//sending data to server.
	send(socketFD,KeyText,FileInfo->FileLength,0);
	send(socketFD,PlainText,FileInfo->FileLength,0);

	//recieving data from server.
	recv(socketFD,RecievedText,FileInfo->FileLength,0);
	printf("%s/n",RecievedText);

	//freeing data.
	CloseEncrytionObjFD(FileInfo);
	free(KeyText);
	free(PlainText);
	free(RecievedText);
	free(FileInfo);

	return 0;
}
