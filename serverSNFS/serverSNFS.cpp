#include <iostream>
#include <vector>
#include <thread>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include "SNFS.h"

using namespace std;

char mount_path[1024];

char* edit_path(const char* path){
	char* new_path = (char*) malloc(sizeof(char) * (strlen(path) + strlen(mount_path) + 1));
	new_path[0] = '\0';
	strcat(new_path, mount_path);
	strcat(new_path, path);

	return new_path;
}

void server_ping(int sock){
	rpcRecv ret;
	ret.retval = 1;
	send(sock, &ret, sizeof(ret), 0);
}

void server_create(int sock, const char* path, mode_t mode){
	// edit path
	path = edit_path(path);

	// create file
	rpcRecv ret;
	ret.retval = creat(path, mode);
	if(ret.retval < 0)
		ret.err = errno;
	else{
		ret.err = 0;
	}
	// close(ret.retval);
	// send return struct to client
	send(sock, &ret, sizeof(ret), 0);
}

void server_open(int sock, const char* path, const int flags){
	// first edit path to indicate server side mount point
	path = edit_path(path);

	// execute operation and put relevant results into return struct
	rpcRecv ret;
	ret.retval = open(path, flags);
	if(ret.retval < 0)
		ret.err = errno;
	else{
		//close file
		ret.err = 0;
		close(ret.retval);
	}

	// send return struct to client
	send(sock, &ret, sizeof(ret), 0);
}

void server_read(int sock, const char* path, size_t size, off_t offset, int rpc_fd){
	// first edit path to indicate server side mount point
	path = edit_path(path);

	// execute operation and put relevant results into return struct
	rpcRecv ret;
	int res;
	char* buf = (char*)malloc(size);
	int fd;
	
	if(rpc_fd != -1)
		fd = rpc_fd;
	else fd = open(path, O_RDONLY);

	if(fd < 0){
		// set error values, send empty data
		ret.retval = fd;
		ret.err = errno;
		goto ERROR;
	}

	// file successfully opened
	res = pread(fd, buf, size, offset);
	printf("READ DATA %s (size %d) FROM %s\n",buf,res,path);
	if(res <= 0){
		ret.retval = res;
		ret.err = errno;
		
		if(fd != rpc_fd)
			close(fd);
		goto ERROR;
	}

	// file successfully read
	ret.retval = res;
	ret.err = 0;
	
	// close file if just opened
	if(fd != rpc_fd)
		close(fd);

	// send return struct to client
	send(sock, &ret, sizeof(ret), 0);
	send(sock, buf, res, 0);
	free(buf);
	return;

ERROR:
	free(buf);
	send(sock, &ret, sizeof(ret), 0);
}

void server_write(int sock, const char* path, size_t size, off_t offset, int rpc_fd){
	// first edit path to indicate server side mount point
	path = edit_path(path);

	// execute operation and put relevant results into return struct
	rpcRecv ret;
	int res;
	char* buf = (char*)malloc(size);
	int fd;

	if(rpc_fd != -1)
		fd = rpc_fd;
	else fd = open(path, O_WRONLY);

	if(fd < 0){
		// set error values, send empty data
		ret.retval = fd;
		ret.err = errno;
		goto ERROR;
	}

	// file opened, receive data to be written
	recv(sock, buf, size, 0);
	printf("Writing data %s (size %d)\n",buf,(int)size);
	res = pwrite(fd, buf, size, offset);
	if(res < 0){
		ret.retval = res;
		ret.err = errno;
		if(fd != rpc_fd)
			close(fd);
		goto ERROR;
	}

	// file successfully read
	ret.retval = res;
	ret.err = 0;

	// close file if just opened
	if(fd != rpc_fd)
		close(fd);

	// send return struct to client
	send(sock, &ret, sizeof(ret), 0);
	free(buf);
	return;

ERROR:
	free(buf);
	send(sock, &ret, sizeof(ret), 0);
}

void server_getattr(int sock, const char *path){
	// first edit path for mount address
	path = edit_path(path);

	// define variables
	struct stat st;
	rpcRecv ret;
	int res = lstat(path, &st);

	if(res < 0){
		ret.retval = res;
		ret.err = errno;
		send(sock, &ret, sizeof(ret), 0);
		return;
	}

	// lstat successful
	ret.retval = res;
	ret.err = 0;

	// send return value
	send(sock, &ret, sizeof(ret), 0);

	// send stat struct
	send(sock, &st, sizeof(st), 0);
}

void server_readdir(int sock, const char *path){
	// first edit path for mount address
	path = edit_path(path);

	// define variables
	struct stat st;
	rpcRecv ret;
	
	DIR* dir = opendir(path);
	if (dir == NULL){
		ret.retval = -1;
		ret.err = -1;
		send(sock, &ret, sizeof(ret), 0);
		return;
	}
	ret.retval = 1;
	ret.err = 0;
	
	// send return value
	struct dirent* dirp = readdir(dir);
	struct dirent* directories = (struct dirent*) malloc(sizeof(struct dirent)*1024);
	int count = 0;
	while(dirp != NULL){
		memcpy((void*)&(directories[count++]),dirp,sizeof(struct dirent));
		dirp = readdir(dir);
	}
	ret.dataLen = count;
	send(sock, &ret, sizeof(ret), 0);
	for (int i = 0 ; i < count ; i++){
		send(sock, &(directories[i]),sizeof(struct dirent),0);
	}
	
	return;	

}

void server_mkdir(int sock, const char *path,mode_t mode){
	// first edit path for mount address
	path = edit_path(path);
	printf("Making directory at %s\n",path);
	struct stat st;
	rpcRecv ret;
	if (mkdir(path,mode) == -1){
		ret.retval = -1;
		ret.err = errno;
		send(sock, &ret, sizeof(ret), 0);
		return;
	}	
	ret.retval = 0;
	ret.err = 0;
	
	send(sock, &ret, sizeof(ret), 0);
	return;	

}
void server_flush(int sock, const char *path, int rpc_fd){
	// edit path
	path = edit_path(path);

	// open file first
	rpcRecv ret;
	int fd;
	
	if(rpc_fd != -1)
		fd = rpc_fd;
	else fd = open(path, O_RDWR);

	// error check
	if(fd < 0){
ERROR:
		ret.retval = fd;
		ret.err = errno;
		send(sock, &ret, sizeof(ret), 0);
		return;
	}

	// open successful
	ret.retval = fsync(fd);
	if(ret.retval < 0){
		goto ERROR;
	}
	ret.err = 0;

	// close file if just opened
	if(fd != rpc_fd)
		close(fd);

	// send return struct
	send(sock, &ret, sizeof(ret), 0);
}
void server_truncate(int sock, const char *path, off_t size){
	path = edit_path(path);
	rpcRecv ret;
	
	ret.retval = truncate(path, size);
	
	if(ret.retval == -1){
		ret.err = errno;
	}
	else{
		ret.err = 0;
	}
	send(sock, &ret, sizeof(ret), 0);
}

void server_release(int sock, const char *path, int rpc_fd){
	rpcRecv ret;
	ret.retval = close(rpc_fd);

	if(ret.retval == -1){
		ret.err = errno;
	}
	else{
		ret.err = 0;
	}
	send(sock, &ret, sizeof(ret), 0);
}

void connection_handler(int sock){
	rpcCall rpcinfo;
	recv(sock, &rpcinfo, sizeof(rpcinfo), 0);
	printf("Received Procedure call request! (Procedure %d)\n",rpcinfo.procedure);
	switch(rpcinfo.procedure){
		case CREATE:
			server_create(sock, rpcinfo.path, rpcinfo.mode);
			break;
		case OPEN:
			server_open(sock, rpcinfo.path, rpcinfo.flags);
			break;
		case READ:
			server_read(sock, rpcinfo.path, rpcinfo.size, rpcinfo.offset, rpcinfo.fd);
			break;
		case FLUSH:
			server_flush(sock, rpcinfo.path, rpcinfo.fd);
			break;
		case RELEASE:
			server_release(sock, rpcinfo.path, rpcinfo.fd);
			break;
		case WRITE:
			server_write(sock, rpcinfo.path, rpcinfo.size, rpcinfo.offset, rpcinfo.fd);
			break;
		case GETATTR:
			server_getattr(sock, rpcinfo.path);
			break;
		case MKDIR:
			server_mkdir(sock,rpcinfo.path,rpcinfo.mode);
			break;
		case READDIR:
			server_readdir(sock,rpcinfo.path);
			break;
		case PING:
			server_ping(sock);
			break;
		case TRUNCATE:
			server_truncate(sock, rpcinfo.path, rpcinfo.offset);
			break;
	}

	close(sock);
}

int main(int argc, char** argv){
	// define variables
	int s_sock;                             // socket descriptor for server socket
	int c_sock = 0;                         // descriptor for client sockets
	int err;                                // error status
	struct sockaddr_in server_addr;         // address of server
	struct sockaddr_in client_addr;         // address of client
	vector<thread> threads;                 // stores created threads
	int sin_size = sizeof(sockaddr_in);     // size of socket address, used in accept()
	fd_set accept_fd;                       // for use in select
	struct timeval timeout;                 // allows setting a timeout period

	// parse port and mount directory
	char* portString;
	char* mountString;
	for (int i = 0 ; i < argc ; i++){
		char* curr = argv[i];
		if (strcmp(curr,"-port") == 0){
			portString = argv[++i];
		}
		else if(strcmp(curr,"-mount") == 0){
			mountString = argv[++i];	
		}
	}
	int port = atoi(portString);
	printf("Starting server with port %d at mount path %s\n",port,mountString);
	memcpy(mount_path, mountString, strlen(mountString) + 1);

	// create server socket
	s_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(s_sock < 0){
		printf("Error creating server socket, error%d\n", errno);
	}

	// set server address
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	// bind server socket to address and port
	err = bind(s_sock, (struct sockaddr*) &server_addr, sizeof(server_addr));
	if (err < 0){
		printf("Error binding server, error%d\n", errno);
	}

	// listen for connections
	err = listen(s_sock, 10);
	if(err < 0){
		printf("Error listening, error%d\n", errno);
	}

	// accept connections
	while(1){
		// set a 10 second timeout
		timeout.tv_sec = 10;
		timeout.tv_usec = 0;

		FD_ZERO(&accept_fd);
		FD_SET(s_sock, &accept_fd);
		err = select(s_sock + 1, &accept_fd, NULL, NULL, &timeout);

		if((err == 1) && (FD_ISSET(s_sock, &accept_fd))){
			c_sock = accept(s_sock, (struct sockaddr*) &client_addr, (socklen_t*) &sin_size);
			threads.push_back(thread(&connection_handler, c_sock));
		}
		else{
			cout << "Accept timeout\n";
			sleep(1);
			//break;
		}
	}
	if(c_sock < 0){
		printf("Error accepting client, error%d\n", errno);
	}
	for(auto &t: threads){
		t.join();
	}

	// close server socket
	close(s_sock);

	return 0;
}
