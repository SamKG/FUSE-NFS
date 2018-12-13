#include "networkfunc.h"

int connection_setup(const networkInfo* info){
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
	return 0;
}
rpcRecv network_open(const networkInfo* netinfo,const char* path, const int flags){
	int sockfd = connection_setup(netinfo);
	rpcCall rpcinfo;
	rpcinfo.procedure = OPEN;
	strcpy(rpcinfo.path, path);
	rpcinfo.flags = flags;	
	
	send(sockfd, &rpcinfo, sizeof(rpcCall),0); 
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
	strcpy(rpcinfo.path,path);
	
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
	strcpy(rpcinfo.path,path);
	
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
	strcpy(rpcinfo.path,path);
	
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
	strcpy(rpcinfo.path,path);
	
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
	rpcinfo.offset = offset;
	strcpy(rpcinfo.path, path);
	
	send(sockfd, &rpcinfo,sizeof(rpcCall),0); 
	rpcRecv received;
	// The server should set received.retval to be the number of bytes read from file
	recv(sockfd, (void*) (&received), sizeof(rpcRecv), 0);

	// do not receive more data on a failed read or open
	if(received.retval < 0)
		return received;

	// Now we have to explicitly read data from server into buffer
	recv(sockfd, (void*) buff, received.retval,0);	
	close(sockfd);
	return received;
}
rpcRecv network_write(const networkInfo* netinfo, const char* path, char* buff, size_t size, off_t offset){
	int sockfd = connection_setup(netinfo);
	rpcCall rpcinfo;
	rpcinfo.procedure = WRITE;
	rpcinfo.size = size;
	strcpy(rpcinfo.path,path);
	
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
	strcpy(rpcinfo.path,path);
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
	strcpy(rpcinfo.path,path);
	send(sockfd, &rpcinfo,sizeof(rpcCall),0); 
	rpcRecv received;
	recv(sockfd, (void*) (&received), sizeof(rpcRecv), 0);
	struct dirent* dataArray = (struct dirent*) malloc(sizeof(struct dirent)*received.dataLen);	
	for (int i = 0 ; i < received.dataLen ; i++){
		recv(sockfd, ((void*) dataArray) + sizeof(struct dirent)*i, sizeof(struct dirent),0);
	}
	received.dataArray = (void*) dataArray;
	close(sockfd);
	return received;
}
rpcRecv network_releasedir(const networkInfo* netinfo, const char* path){
	int sockfd = connection_setup(netinfo);
	rpcCall rpcinfo;
	rpcinfo.procedure = RELEASEDIR;
	strcpy(rpcinfo.path,path);
	send(sockfd, &rpcinfo,sizeof(rpcCall),0); 
	rpcRecv received;
	recv(sockfd, (void*) (&received), sizeof(rpcRecv), 0);
	close(sockfd);
	return received;
}
rpcRecv network_mkdir(const networkInfo* netinfo, const char* path, mode_t mode){
	int sockfd = connection_setup(netinfo);
	rpcCall rpcinfo;
	rpcinfo.procedure = MKDIR;
	rpcinfo.mode = mode;
	strcpy(rpcinfo.path,path);
	send(sockfd, &rpcinfo,sizeof(rpcCall),0); 
	rpcRecv received;
	recv(sockfd, (void*) (&received), sizeof(rpcRecv), 0);
	close(sockfd);
	return received;
}
rpcRecv network_ping(const networkInfo* netinfo){
	int sockfd = connection_setup(netinfo);
	rpcCall rpcinfo;
	rpcinfo.procedure = PING;
	send(sockfd, &rpcinfo,sizeof(rpcCall),0); 
	rpcRecv received;
	recv(sockfd, (void*) (&received), sizeof(rpcRecv), 0);
	close(sockfd);
	return received;
}
