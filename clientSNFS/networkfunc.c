#include "networkfunc.h"

int connection_setup(networkInfo* info){
	printf("Opening socket!\n");
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
		memcpy(&server_addr.sin_addr, hostp->h_addr, sizeof(server_addr.sin_addr));
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

int connection_close(int sockfd){
	printf("Closing socket!\n");
	close(sockfd);	
}
rpcRecv network_open(const networkInfo* netinfo,const char* path, const int flags){
	int sockfd = connection_setup(netinfo);
	rpcCall rpcinfo;
	rpcinfo.procedure = OPEN;
	rpcinfo.path = path;
	rpcinfo.flags = flags;	
	
	send(sockfd, &rpcinfo,sizeof(rpcCall),0); 
	rpcRecv received;
	recv(sockfd, (void*) (&received), sizeof(rpcRecv), 0);
		
	close(sockfd);
	return received;
}
rpcRecv network_getattr(networkInfo* netinfo, const char* path){
	int sockfd = connection_setup(netinfo);
	rpcCall rpcinfo;
	rpcinfo.procedure = GETATTR;
	rpcinfo.path = path;
	rpcinfo.flags = flags;	
	
	send(sockfd, &rpcinfo,sizeof(rpcCall),0); 
	rpcRecv received;
	recv(sockfd, (void*) (&received), sizeof(rpcRecv), 0);
		
	close(sockfd);
	return received;
}
