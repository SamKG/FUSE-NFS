// This file implements network functions for usage in FUSE 
//

// The job of the networkInfo struct is to store relevant data about the server (ie: address and port)
typedef struct networkInfo {
	int port;
	char* address;
} networkInfo;



// Implement network functions below

// sets up socket connection, returns socket descriptor
int connection_setup(networkInfo* info){
	// define variables
    int sock;                         // socket descriptor for network socket
    int err;                          // error status
    struct sockaddr_in server_addr;   // address of SNFS server
    
    // create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0){
        printf("Error creating client socket, error%d\n", errno);
    }

    // specify server address for socket
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(info->port);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	// connect to server
    err = connect(sock, (struct sockaddr*) &server_addr, sizeof(server_addr));
    if(connect < 0){
        printf("Error connecting to server, error%d\n", errno);
    }

	return sock;
}
