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
#include "hashtable.h"

using namespace std;

char mount_path[1024];
hashtable fd_ht;

int open_fd(const char* path, const int flags){
	int fd = ht_lookup(&fd_ht,path);
	if (fd == -1){
		fd = open(path,flags);
		if (fd <= -1){
			printf("FAILED TO OPEN FILE~ %s\n",strerror(errno));
			return fd;
		}
		ht_ins(&fd_ht,path,fd);
	}
	return fd;
}
int open_fd_mode(const char* path, const int flags,mode_t mode){
	int fd = ht_lookup(&fd_ht,path);
	if (fd == -1){
		fd = open(path,flags,mode);
		if (fd <= -1){
			printf("FAILED TO OPEN FILE~ %s\n",strerror(errno));
			return fd;
		}
		ht_ins(&fd_ht,path,fd);
	}
	return fd;
}
int close_fd(const char* path){
	int fd = ht_lookup(&fd_ht,path);
	if (fd == -1){
		printf("\tNOT ABLE TO CLOSE! (already closed)!\n");
		return -1;
	}
	close(fd);
	ht_delete(&fd_ht,path);
	printf("\tCLOSED FILE\n");
	return 1;
}
char* edit_path(const char* path){
	char* new_path = (char*) malloc(sizeof(char) * (strlen(path) + strlen(mount_path) + 1));
	new_path[0] = '\0';
	strcat(new_path, mount_path);
	strcat(new_path, path);
	printf("\tEdited %s to %s\n",path,new_path);
	return new_path;
}

void server_ping(int sock){
	printf("Received ping from client!\n");
	rpcRecv ret;
	ret.retval = 1;
	ret.err = 0;
	send(sock, &ret, sizeof(ret), 0);
}

void server_create(int sock, const char* path, mode_t mode){
	// edit path
	path = edit_path(path);
	printf("Creating file %s\n",path);
	// create file
	rpcRecv ret;
	ret.retval = open_fd_mode(path, O_CREAT|O_WRONLY|O_TRUNC,mode);
	if(ret.retval < 0)
		ret.err = errno;
	else{
		ht_ins(&fd_ht,path,ret.retval);
		printf("INSERTING INTO HT\n");
		ret.err = 0;
	}
	// send return struct to client
	send(sock, &ret, sizeof(ret), 0);
}

void server_open(int sock, const char* path, const int flags){
	// first edit path to indicate server side mount point
	path = edit_path(path);
	printf("Opening file %s\n",path);
	// execute operation and put relevant results into return struct
	rpcRecv ret;
	ret.retval = open_fd(path, flags);
	if(ret.retval < 0)
		ret.err = EACCES;
	else{
		ht_ins(&fd_ht,path,ret.retval);
		ret.err = 0;
	}

	// send return struct to client
	send(sock, &ret, sizeof(ret), 0);
}

void server_opendir(int sock, const char* path){
	// first edit path to indicate server side mount point
	path = edit_path(path);
	printf("Opening dir %s\n",path);
	// execute operation and put relevant results into return struct
	rpcRecv ret;
	ret.retval = 1;
	if(ret.retval < 0)
		ret.err = EACCES;
	else{
		ht_ins(&fd_ht,path,ret.retval);
		ret.err = 0;
	}

	// send return struct to client
	send(sock, &ret, sizeof(ret), 0);
}
void server_read(int sock, const char* path, size_t size, off_t offset){
	// first edit path to indicate server side mount point
	path = edit_path(path);
	printf("Reading file %s\n",path);
	// execute operation and put relevant results into return struct
	rpcRecv ret;
	int res;
	char* buf = (char*)malloc(size);
	int fd;
	
	fd = ht_lookup(&fd_ht,path);

	if(fd < 0){
		// set error values, send empty data
		ret.retval = fd;
		ret.err = EBADF;
		goto ERROR;
	}

	// file successfully opened
	res = pread(fd, buf, size,offset);
	printf("READ DATA %s (size %d,offset %d) FROM %s\n",buf,res,offset,path);
	if(res <= 0){
		ret.retval = res;
		ret.err = errno;
		goto ERROR;
	}

	// file successfully read
	ret.retval = res;
	ret.err = 0;
	
	// send return struct to client
	send(sock, &ret, sizeof(ret), 0);
	send(sock, buf, res, 0);
	free(buf);
	return;

ERROR:
	free(buf);
	send(sock, &ret, sizeof(ret), 0);
}

void server_write(int sock, const char* path, size_t size, off_t offset){
	// first edit path to indicate server side mount point
	path = edit_path(path);

	printf("Writing file %s\n",path);
	// execute operation and put relevant results into return struct
	rpcRecv ret;
	int res;
	char* buf = (char*)malloc(size);
	int fd;

	fd = ht_lookup(&fd_ht,path);
	
	recv(sock, buf, size, 0);
	printf("Writing data %s (size %d)\n",buf,(int)size);
	res = pwrite(fd, buf, size,offset);
	if(res < 0){
		ret.retval = res;
		ret.err = errno;
		goto ERROR;
	}

	// file successfully read
	ret.retval = res;
	ret.err = 0;

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

	printf("Getattr %s (perms %d)\n",path,st.st_mode);
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
	printf("Readdir %s\n",path);
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
	free(directories);	
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
void server_fsync(int sock, const char *path,int mode){
	// first edit path for mount address
	path = edit_path(path);
	printf("fsync at %s\n",path);
	struct stat st;
	rpcRecv ret;
	int fd = ht_lookup(&fd_ht,path);

	if(fd < 0){
		// set error values, send empty data
		ret.retval = fd;
		ret.err = EBADF;
		send(sock, &ret, sizeof(ret), 0);
		return;
	}
	if (mode){
		ret.retval = fdatasync(fd);
	}	
	else {
		ret.retval = fsync(fd);
	}
	ret.err = 0;
	send(sock, &ret, sizeof(ret), 0);
	return;	
}
void server_unlink(int sock, const char *path){
	// first edit path for mount address
	path = edit_path(path);
	printf("unlinking %s\n",path);
	struct stat st;
	rpcRecv ret;

	ret.retval = unlink(path);
	ret.err = 0;
	if (ret.retval < 0){
		ret.err = errno;
	}
	send(sock, &ret, sizeof(ret), 0);
	return;	

}
void server_access(int sock, const char *path,int mode){
	// first edit path for mount address
	path = edit_path(path);
	printf("access check at %s\n",path);
	struct stat st;
	rpcRecv ret;

	ret.retval = access(path,mode);
	ret.err = 0;
	if (ret.retval < 0){
		ret.err = errno;
	}
	send(sock, &ret, sizeof(ret), 0);
	return;	

}
void server_flush(int sock, const char *path){
	// edit path
	path = edit_path(path);
	printf("Flushing %s\n",path);
	// open file first
	rpcRecv ret;
	ret.retval = 1;
	send(sock, &ret, sizeof(ret), 0);
}
void server_truncate(int sock, const char *path, off_t size){
	path = edit_path(path);
	printf("Truncating %s\n",path);
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

void server_release(int sock, const char *path){
	path = edit_path(path);
	rpcRecv ret;
	printf("Releasing %s\n",path);
	ret.retval = close_fd(path);	
	if(ret.retval == -1){
		ret.err = -1;
	}
	else{
		ret.err = 0;
	}
	send(sock, &ret, sizeof(ret), 0);
}

void server_releasedir(int sock, const char *path){
	path = edit_path(path);
	rpcRecv ret;
	ret.retval = 1;
	printf("Releasing directory %s\n",path);
	send(sock, &ret, sizeof(ret), 0);
}
void connection_handler(int sock){
	rpcCall rpcinfo;
	recv(sock, &rpcinfo, sizeof(rpcinfo), 0);
	switch(rpcinfo.procedure){
		case CREATE:
			server_create(sock, rpcinfo.path, rpcinfo.mode);
			break;
		case OPEN:
			server_open(sock, rpcinfo.path, rpcinfo.flags);
			break;
		case FLUSH:
			server_flush(sock, rpcinfo.path);
			break;
		case RELEASE:
			server_release(sock, rpcinfo.path);
			break;
		case TRUNCATE:
			server_truncate(sock, rpcinfo.path, rpcinfo.offset);
			break;
		case GETATTR:
			server_getattr(sock, rpcinfo.path);
			break;
		case READ:
			server_read(sock, rpcinfo.path, rpcinfo.size, rpcinfo.offset);
			break;
		case WRITE:
			server_write(sock, rpcinfo.path, rpcinfo.size, rpcinfo.offset);
			break;
		case OPENDIR:
			server_opendir(sock, rpcinfo.path);
			break;
		case READDIR:
			server_readdir(sock,rpcinfo.path);
			break;
		case RELEASEDIR:
			server_releasedir(sock, rpcinfo.path);
			break;
		case MKDIR:
			server_mkdir(sock,rpcinfo.path,rpcinfo.mode);
			break;
		case FSYNC:
			server_fsync(sock,rpcinfo.path,rpcinfo.flags);
			break;
		case UNLINK:
			server_unlink(sock,rpcinfo.path);
			break;
		case ACCESS:
			server_access(sock,rpcinfo.path,rpcinfo.flags);
			break;
		case PING:
			server_ping(sock);
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
	fd_set accept_fd;
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
	strcpy(mount_path, mountString);
	
	//initialize fd lookup
	hashtable_init(&fd_ht);
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
			sleep(.1);
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
