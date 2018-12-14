/*
FUSE: Filesystem in Userspace
Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>

This program can be distributed under the terms of the GNU GPL.
See the file COPYING.

gcc -Wall client.c `pkg-config fuse --cflags --libs` -o client
*/

//We need to write these header functions (that implement network calls to server)
#include "networkfunc.h"

static const char *client_str = "Hello World!\n";
static const char *client_path = "/client";
static networkInfo* netinfo = NULL;
static int mount_path_length;

/*shortens the path if it includes the mount address, adds a beginning / if it does not
Example, if the mount path is /tmp/fuse:
	/tmp/fuse/file1.txt will return /file1.txt
	file2.txt will return /file2.txt	*/
static char* edit_path(const char *path){
	char* newpath = (char*) ((void*)path);
	return newpath + mount_path_length + 1; 
}

static int client_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
	path = edit_path(path);
	rpcRecv received = network_create(netinfo, path, mode);
	if(received.retval < 0)
		return -received.err;
	return 0; 
}

static int client_getattr(const char *path, struct stat *stbuf)
{
	path = edit_path(path);
	rpcRecv received = network_getattr(netinfo, path, stbuf);
	if(received.retval < 0){
		return -received.err;
	}
	return 0;
}

static int client_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		off_t offset, struct fuse_file_info *fi)
{
	path = edit_path(path);
	rpcRecv received = network_readdir(netinfo, path, buf, offset);
	for (int i = 0 ; i < received.dataLen ; i++){
		if (filler(buf,((struct dirent*) received.dataArray)[i].d_name,NULL,0) != 0){
			return -ENOMEM;
		}
	}
	return 0;
}

static int client_open(const char *path, struct fuse_file_info *fi)
{
	path = edit_path(path);
	rpcRecv received = network_open(netinfo,path, O_RDWR);
	if(received.retval < 0)
		return -received.err;
	return 0; 
}

static int client_read(const char *path, char *buf, size_t size, off_t offset,
		struct fuse_file_info *fi)
{
	path = edit_path(path);
	rpcRecv received = network_read(netinfo,path,buf,size,offset);
	if(received.retval <= 0)
		return -received.err;
	// retval will be the return of pread on the server side, aka number of bytes read
	return received.retval;
}

static int client_write(const char *path, const char *buf, size_t size, off_t offset,
		struct fuse_file_info *fi)
{
	path = edit_path(path);
	rpcRecv received = network_write(netinfo, path, buf, size, offset);
	if(received.retval < 0)
		return -received.err;
	return received.retval;
}

static int client_flush(const char *path, struct fuse_file_info *fi)
{
	path = edit_path(path);
	rpcRecv received = network_flush(netinfo, path);
	if(received.retval < 0)
		return -received.err;
	return 0;
}

static int client_release(const char *path, struct fuse_file_info *fi)
{
	return 0;
}

static int client_mkdir(const char *path, mode_t mode)
{
	path = edit_path(path);
	printf("Making directory at %s\n",path);
	rpcRecv received = network_mkdir(netinfo, path,mode);
	if(received.retval < 0)
		return -received.err;
	
	return 0;
}
static struct fuse_operations client_oper = {
	.create		= client_create,
	.getattr	= client_getattr,
	.readdir	= client_readdir,
	.open		= client_open,
	.read		= client_read,
	.write		= client_write,
	.flush 		= client_flush,
	.release	= client_release,
	.mkdir		= client_mkdir,
};

int main(int argc, char *argv[])
{
	int argcpassed = 0;	
	char** argvpassed = (char**) malloc(sizeof(char*)*256);	
	
	// Parse out port and address to use	
	char* portString;
	char* addressString;
	for (int i = 0 ; i < argc ; i++){
		char* curr = argv[i];
		if (strcmp(curr,"-port") == 0){
			portString = argv[++i];
		}
		else if(strcmp(curr,"-address") == 0){
			addressString = argv[++i];	
		}
		else{
			if(strcmp(curr, "-mount") == 0){
				mount_path_length = strlen(argv[i + 1]);
			}
			argvpassed[argcpassed++] = curr;
		}
	}
	printf("Starting fuse with %s:%s\n",addressString,portString);	
	netinfo = (networkInfo*) malloc(sizeof(networkInfo));	
	netinfo->port = atoi(portString);
	netinfo->address = (char*) malloc(sizeof(char)*strlen(addressString)+1);
	memcpy(netinfo->address,addressString,strlen(addressString)+1);
	printf("Connecting to server...");
	fflush(stdout);
	rpcRecv retval = network_ping(netinfo);	
	while(retval.retval != 1){
		printf("retrying...\n");
		retval = network_ping(netinfo);	
		sleep(3);
	};
	if (retval.retval == 1){
		printf("Success!\n");
	}
	else{
		printf("Failed to connect with server!!\n");
		return 0;
	}
	// Run fuse_main with the other parameters
	printf("Setting up fuse!\n");
	return fuse_main(argcpassed, argvpassed, &client_oper, NULL);
}
