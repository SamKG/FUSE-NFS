// This file implements network functions for usage in FUSE 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

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
	struct hostent *hostp;			  // used if host is passed by name
    
    // create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0){
        printf("Error creating client socket, error%d\n", errno);
    }

    // specify server address for socket
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(info->port);
	server_addr.sin_addr.s_addr = inet_addr(info->address);

	// if the address is passed by name, not by ip
	if(server_addr.sin_addr.s_addr == (unsigned long) INADDR_NONE){
		// find host by name
		hostp = gethostbyname(info->address);
		if(hostp == (struct hostent*) NULL){
			printf("Host not found: %d\n", h_errno);
			close(sock);
			return -1;
		}
		memcpy(&server_addr.sin_addr, hostp-h_addr, sizeof(server_addr.sin_addr));
	}

	// connect to server
    err = connect(sock, (struct sockaddr*) &server_addr, sizeof(server_addr));
    if(err < 0){
        printf("Error connecting to server, error %d\n", errno);
		close(sock);
		return -1;
    }

	return sock;
}
