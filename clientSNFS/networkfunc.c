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
	memcpy(rpcinfo.path, path, strlen(path));
	rpcinfo.flags = flags;	
	
	send(sockfd, &rpcinfo,sizeof(rpcCall),0); 
	rpcRecv received;
	recv(sockfd, (void*) (&received), sizeof(rpcRecv), 0);
		
	close(sockfd);
	return received;
}
// TODO: Manually fill in stat structure with received data!
rpcRecv network_getattr(const networkInfo* netinfo, const char* path){
	int sockfd = connection_setup(netinfo);
	rpcCall rpcinfo;
	rpcinfo.procedure = GETATTR;
	memcpy(rpcinfo.path,path,strlen(path));
	
	send(sockfd, &rpcinfo,sizeof(rpcCall),0); 
	rpcRecv received;
	recv(sockfd, (void*) (&received), sizeof(rpcRecv), 0);
		
	close(sockfd);
	return received;
}
rpcRecv network_flush(const networkInfo* netinfo, const char* path){
	int sockfd = connection_setup(netinfo);
	rpcCall rpcinfo;
	rpcinfo.procedure = FLUSH;
	memcpy(rpcinfo.path,path,strlen(path));
	
	send(sockfd, &rpcinfo,sizeof(rpcCall),0); 
	rpcRecv received;
	recv(sockfd, (void*) (&received), sizeof(rpcRecv), 0);
		
	close(sockfd);
	return received;
}
rpcRecv network_release(const networkInfo* netinfo, const char* path){
	int sockfd = connection_setup(netinfo);
	rpcCall rpcinfo;
	rpcinfo.procedure = RELEASE;
	memcpy(rpcinfo.path,path,strlen(path));
	
	send(sockfd, &rpcinfo,sizeof(rpcCall),0); 
	rpcRecv received;
	recv(sockfd, (void*) (&received), sizeof(rpcRecv), 0);
		
	close(sockfd);
	return received;
}
rpcRecv network_truncate(const networkInfo* netinfo, const char* path, const int size){
	int sockfd = connection_setup(netinfo);
	rpcCall rpcinfo;
	rpcinfo.procedure = TRUNCATE;
	rpcinfo.size = size;
	memcpy(rpcinfo.path,path,strlen(path));
	
	send(sockfd, &rpcinfo,sizeof(rpcCall),0); 
	rpcRecv received;
	recv(sockfd, (void*) (&received), sizeof(rpcRecv), 0);
		
	close(sockfd);
	return received;
}
rpcRecv network_read(const networkInfo* netinfo, const char* path, char* buff, size_t size, off_t offset){
	int sockfd = connection_setup(netinfo);
	rpcCall rpcinfo;
	rpcinfo.procedure = READ;
	rpcinfo.size = size;
	memcpy(rpcinfo.path,path,strlen(path));
	
	send(sockfd, &rpcinfo,sizeof(rpcCall),0); 
	rpcRecv received;
	// The server should set received.dataLen to be the number of bytes read from file
	recv(sockfd, (void*) (&received), sizeof(rpcRecv), 0);
	// Now we have to explicitly read data from server into buffer
	recv(sockfd, (void*) buff, received.dataLen,0);	
	close(sockfd);
	return received;
}
rpcRecv network_write(const networkInfo* netinfo, const char* path, char* buff, size_t size, off_t offset){
	int sockfd = connection_setup(netinfo);
	rpcCall rpcinfo;
	rpcinfo.procedure = WRITE;
	rpcinfo.size = size;
	memcpy(rpcinfo.path,path,strlen(path));
	
	send(sockfd, &rpcinfo,sizeof(rpcCall),0); 
	// Now we send string data to server
	send(sockfd, buff, size, 0);
	rpcRecv received;
	recv(sockfd, (void*) (&received), sizeof(rpcRecv), 0);
	close(sockfd);
	return received;
}
rpcRecv network_opendir(const networkInfo* netinfo, const char* path){
	int sockfd = connection_setup(netinfo);
	rpcCall rpcinfo;
	rpcinfo.procedure = OPENDIR;
	memcpy(rpcinfo.path,path,strlen(path));
	send(sockfd, &rpcinfo,sizeof(rpcCall),0); 
	rpcRecv received;
	recv(sockfd, (void*) (&received), sizeof(rpcRecv), 0);
	close(sockfd);
	return received;
}
rpcRecv network_readdir(const networkInfo* netinfo, const char* path, void* buf, off_t offset){
	int sockfd = connection_setup(netinfo);
	rpcCall rpcinfo;
	rpcinfo.procedure = READDIR;
	memcpy(rpcinfo.path,path,strlen(path));
	send(sockfd, &rpcinfo,sizeof(rpcCall),0); 
	rpcRecv received;
	recv(sockfd, (void*) (&received), sizeof(rpcRecv), 0);
	close(sockfd);
	return received;
}
