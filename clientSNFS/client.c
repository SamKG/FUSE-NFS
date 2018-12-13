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
	return newpath + mount_path_length; 
}

static int client_getattr(const char *path, struct stat *stbuf)
{
	path = edit_path(path);
	rpcRecv received = network_getattr(netinfo,path);
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
	if(received.err != 0)
		return -received.err;
	return 0; 
}

static int client_read(const char *path, char *buf, size_t size, off_t offset,
		struct fuse_file_info *fi)
{
	path = edit_path(path);
	rpcRecv received =  network_read(netinfo,path,buf,size,offset);
	return 0;
}

static struct fuse_operations client_oper = {
	.getattr	= client_getattr,
	.readdir	= client_readdir,
	.open		= client_open,
	.read		= client_read,
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
	// Run fuse_main with the other parameters
	return fuse_main(argcpassed, argvpassed, &client_oper, NULL);
}
